#include "utils.h"
#include <sys/stat.h>

bool ensure_directory_exists(const char *dir)
{
    struct stat st = {0};
    if (stat(dir, &st) == -1)
    {
        if (mkdir(dir, 0700) != 0)
        {
            oct_eprintf("Error: Failed to create directory %s\n", dir);
            return false;
        }
    }
    return true;
}
