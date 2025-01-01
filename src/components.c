#include "components.h"
#include <string.h>
#include <stdlib.h>

#include "utils.h"

#define MAX_COMPONENTS 100

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

static ComponentEntry *component_map = NULL;
static LoadedComponentEntry *loaded_components_map = NULL;

// Initialize component system
void init_component_system()
{
    sh_new_arena(component_map);         // Use string arena for registered components
    sh_new_arena(loaded_components_map); // Use string arena for loaded components
}

// Free component system
void cleanup_component_system()
{
    shfree(component_map);
    shfree(loaded_components_map);
}

// Register a component
bool register_component(const char *name,
                        void (*render)(Rectangle),
                        bool (*attach_callback)(),
                        bool (*detach_callback)())
{
    if (shgeti(component_map, name) != -1)
    {
        oct_eprintf("Component already registered: %s", name);
        return false;
    }

    Component comp = {
        .name = name,
        .render = render,
        .attach_callback = attach_callback,
        .detach_callback = detach_callback,
    };

    shput(component_map, name, comp);
    oct_printf("Registered component: %s", name);
    return true;
}

bool load_component(const char *name)
{
    if (shgeti(loaded_components_map, name) != -1)
    {
        return true; // Already loaded
    }
    int index = shgeti(component_map, name);
    if (index == -1)
    {
        oct_eprintf("Component not found: %s", name);
        return false;
    }

    Component *comp = &component_map[index].value;
    if (comp->attach_callback())
    {
        shput(loaded_components_map, name, true); // Mark as loaded
        oct_printf("Loaded component: %s", name);
        return true;
    }
    oct_eprintf("Failed to attach component: %s", name);
    return false;
}

// Unload a component
bool unload_component(const char *name)
{
    int index = shgeti(component_map, name);
    if (index == -1)
    {
        oct_eprintf("Component not found: %s", name);
        return false;
    }

    Component *comp = &component_map[index].value;
    if (comp->detach_callback())
    {
        shdel(loaded_components_map, name);
        oct_printf("Unloaded component: %s", name);
        return true;
    }

    oct_eprintf("Failed to detach component: %s", name);
    return false;
}

// Render a component
void render_component(const char *name, Rectangle boundary)
{
    int index = shgeti(loaded_components_map, name);
    if (index == -1)
    {
        oct_eprintf("Component not loaded: %s", name);
        return;
    }

    Component *comp = &component_map[index].value;
    comp->render(boundary);
}
