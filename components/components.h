#pragma once

#include "raylib.h"
#include <stdbool.h>

#include "utils.h"

typedef struct Component
{
    const char *name;                   // Component name
    void (*render)(Rectangle boundary); // Render function
    bool (*attach_callback)();          // Attach callback
    bool (*detach_callback)();          // Detach callback
} Component;

// Hashmap for registered components
typedef struct
{
    const char *key;
    Component value;
} ComponentEntry;

// Hashmap for loaded components
typedef struct
{
    const char *key;
    bool value;
} LoadedComponentEntry;

bool register_component(const char *name,
                        void (*render)(Rectangle),
                        bool (*attach_callback)(),
                        bool (*detach_callback)());

bool load_component(const char *name);
bool unload_component(const char *name);
void render_component(const char *name, Rectangle boundary);

void init_component_system();
void cleanup_component_system();