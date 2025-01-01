#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <complex.h>
#include <string.h>

#include "raylib.h"
#include "kiss_fft.h"

#include "state.h"
#include "components.h"

// Some constants
#define FFT_SIZE (1 << 8)

// Helper macros to keep the same style as before
#define Float_Complex float complex
#define cfromreal(re) (re)
#define cfromimag(im) ((im) * I)
#define mulcc(a, b) ((a) * (b))
#define addcc(a, b) ((a) + (b))
#define subcc(a, b) ((a) - (b))

// Buffers for our spectrogram logic
static float in_raw[FFT_SIZE] = {0};          // “rolling” input samples
static float in_win[FFT_SIZE] = {0};          // windowed buffer
static Float_Complex out_raw[FFT_SIZE] = {0}; // complex FFT bins
static float out_log[FFT_SIZE] = {0};         // log amplitude array
static float out_smooth[FFT_SIZE] = {0};      // smoothed amplitude
static float out_smear[FFT_SIZE] = {0};       // smeared amplitude for “tails”

// We’ll keep a KissFFT plan around to reuse
static kiss_fft_cfg g_kiss_cfg = NULL;

static int music_task_attached = -1;
static AudioStream *audio_stream_attached = NULL;

/**
 * \brief Initialize the KissFFT plan (call once).
 */
static void initKissFFT(void)
{
    // 0 means forward FFT
    g_kiss_cfg = kiss_fft_alloc(FFT_SIZE, 0, NULL, NULL);
    if (!g_kiss_cfg)
    {
        fprintf(stderr, "Error: kiss_fft_alloc() failed\n");
        exit(1);
    }
}

/**
 * \brief Cleanup the KissFFT plan (call at exit).
 */
static void freeKissFFT(void)
{
    free(g_kiss_cfg);
    g_kiss_cfg = NULL;
}

/**
 * \brief Push one audio frame (sample) into the rolling buffer.
 */
static void fft_push(float frame)
{
    memmove(in_raw, in_raw + 1, (FFT_SIZE - 1) * sizeof(in_raw[0]));
    in_raw[FFT_SIZE - 1] = frame;
}

/**
 * \brief Compute the forward FFT using KissFFT (replaces custom recursion).
 *
 * \param in      real input array (of length n)
 * \param out     complex output array (of length n)
 * \param n       number of points (FFT_SIZE)
 */
static void do_fft(const float *in, Float_Complex *out, size_t n)
{
    // We'll convert “in” to kiss_fft_cpx, do the transform, then put it into `out`.
    // We treat input as purely real, so imaginary part = 0.
    static kiss_fft_cpx fin[FFT_SIZE];
    static kiss_fft_cpx fout[FFT_SIZE];

    // Populate fin
    for (size_t i = 0; i < n; i++)
    {
        fin[i].r = in[i];
        fin[i].i = 0.0f;
    }

    // Execute KissFFT
    kiss_fft(g_kiss_cfg, fin, fout);

    // Copy the result to out_raw[] in float complex form
    //   out[k] = fout[k].r + I * fout[k].i
    for (size_t i = 0; i < n; i++)
    {
        out[i] = fout[i].r + (fout[i].i * I);
    }
}

/**
 * \brief Return the log amplitude for a complex sample z = x + i*y
 */
static inline float amp(Float_Complex z)
{
    float a = crealf(z);
    float b = cimagf(z);
    // log of magnitude-squared
    return logf(a * a + b * b + 1e-12f); // add a tiny offset to avoid log(0)
}

/**
 * \brief Perform the Hann window, do the KissFFT, build log-scale bins, etc.
 *
 * \param dt  time delta (frame time), used in smoothing calculations
 * \return    number of log bins used
 */
static size_t fft_analyze(float dt)
{
    // 1) Hann Window
    for (size_t i = 0; i < FFT_SIZE; ++i)
    {
        float t = (float)i / (float)(FFT_SIZE - 1);
        float hann = 0.5f - 0.5f * cosf(2.0f * PI * t);
        in_win[i] = in_raw[i] * hann;
    }

    // 2) Do FFT via KissFFT
    do_fft(in_win, out_raw, FFT_SIZE);

    // 3) Convert to log-scale “bins”
    float step = 1.06f;
    float lowf = 1.0f;
    size_t m = 0;
    float max_amp = 1.0f;

    // We'll only examine the first half (Nyquist) = [0..FFT_SIZE/2]
    // “f” is an index in [1..FFT_SIZE/2], stepping by ~1.06 scale factor
    for (float f = lowf; (size_t)f < FFT_SIZE / 2; f = ceilf(f * step))
    {
        float f1 = ceilf(f * step);
        float a = 0.0f;
        for (size_t q = (size_t)f; q < FFT_SIZE / 2 && q < (size_t)f1; ++q)
        {
            float b = amp(out_raw[q]);
            if (b > a)
                a = b;
        }
        if (a > max_amp)
            max_amp = a;
        out_log[m++] = a;
    }

    // 4) Normalize 0..1
    for (size_t i = 0; i < m; ++i)
    {
        out_log[i] /= max_amp;
    }

    // 5) Smooth + smear
    //    Smoothness and Smearness define how quickly these envelopes follow or decay.
    for (size_t i = 0; i < m; ++i)
    {
        float smoothness = 8.0f; // how fast it follows upward changes
        out_smooth[i] += (out_log[i] - out_smooth[i]) * smoothness * dt;

        float smearness = 3.0f; // how slow the tail decays
        out_smear[i] += (out_smooth[i] - out_smear[i]) * smearness * dt;
    }

    return m;
}

/**
 * \brief Raylib's Audio Stream callback to feed samples into fft_push().
 */
static void AudioCallbackFFT(void *bufferData, unsigned int frames)
{
    float(*floatBuffer)[2] = (float(*)[2])bufferData;
    for (unsigned int i = 0; i < frames; i++)
    {
        float left_sample = floatBuffer[i][0]; // pick left channel
        fft_push(left_sample);
    }
}

/**
 * \brief Render the FFT bars on screen.
 *
 * \param boundary  A rectangle (entire screen in this case)
 * \param m         number of bins to draw
 */
static void render(Rectangle boundary)
{
    if (SYSTEM_STATE.music_state == MUSIC_NULL)
        return;

    if (music_task_attached != SYSTEM_STATE.music_task_id)
    {
        if (music_task_attached != -1)
        {
            // audio stream changed, detach previous processor
            DetachAudioStreamProcessor(*audio_stream_attached, AudioCallbackFFT);
        }
        else
        {
            // new incoming stream
            printf("attach stream processor %d\n", SYSTEM_STATE.music_stream.channels);
            AttachAudioStreamProcessor(SYSTEM_STATE.music_stream, AudioCallbackFFT);
            music_task_attached = SYSTEM_STATE.music_task_id;
            audio_stream_attached = &SYSTEM_STATE.music_stream;
        }
    }

    float dt = GetFrameTime();         // time between frames
    size_t fft_bins = fft_analyze(dt); // do Hann + KissFFT + log scale + smoothing

    float cell_width = boundary.width / (float)fft_bins;

    // out_smooth[i] is the “smoothed amplitude” in [0..1]
    for (size_t i = 0; i < fft_bins; ++i)
    {
        float x = boundary.x + i * cell_width + (cell_width * 0.5f);
        float y1 = boundary.y + boundary.height - boundary.height * 0.75f * out_smooth[i];
        float y2 = boundary.y + boundary.height;

        DrawLineEx((Vector2){x, y1},
                   (Vector2){x, y2},
                   cell_width, SYSTEM_STATE.ui_dynamicColor);
    }
}

static bool attach_callback()
{
    initKissFFT();
    return true;
}

static bool detach_callback()
{
    if (music_task_attached != -1)
        DetachAudioStreamProcessor(*audio_stream_attached, AudioCallbackFFT);

    return true;
}

// this must happen after main been initialized
__attribute__((constructor(COMPONENT_REGISTER_ORDER))) static void init()
{
    register_component("spectrogram", render, attach_callback, detach_callback);
}