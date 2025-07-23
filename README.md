<p align="center"><img src="https://github.com/MykleR/42-ecs/blob/main/42-ecs-logo.png" alt="logo" width="200" align="center"/></p>

<h1 id="logo" align="center"> 42-ECS </h1>

<div align="center">

[![Readme Card](https://github-readme-stats.vercel.app/api/pin/?username=MykleR&repo=so_long&theme=dark&bg_color=30,090a0c,494d59\&title_color=fff\&text_color=fff)](https://github.com/MykleR/so_long)

##

</div>

Easy and fast ECS library designed for 42 School C common core projects.
Example use in [so_long](https://github.com/MykleR/so_long) (more to come).

> [!NOTE]\
> ECS, or Entity-Component-System, is a software architectural pattern commonly used in game development that organizes game objects into entities, components, and systems. promoting code reusability and performance.
> - Entities are unique identifiers,
> - components are data without behavior,
> - and systems are processes that operate on entities with specific components

## ✨ Features

> - **⚡ Optimized Performance**: O(1) query access with pre-filled results
> - **📏 42 School Compliant**: Follows 42 School norm and coding standards
> - **🧩 Component System**: Up to 63 possible components per entity
> - **🔍 Multi-Component Queries**: Query entities by multiple component combinations
> - **🔄 Batch Operations**: Queue system for deferred entity operations

## 🚀 Basic Usage

#### Exaple Setup
```c
#include <ecs.h>

// Define your components
typedef struct s_position { float x, y; } t_position;
typedef struct s_velocity { float dx, dy; } t_velocity;

// Component IDs
#define POSITION 0
#define VELOCITY 1
```
#### Creating Components
```c
// Create ECS with 2 component types
t_ecs *ecs = ecs_create(2, sizeof(t_position), sizeof(t_velocity));

// Create an entity
uint32_t entity = ecs_entity_create(ecs);
// Add components
ecs_entity_add(ecs, entity, POSITION, &(t_position){10.0f, 20.0f});
ecs_entity_add(ecs, entity, VELOCITY, &(t_velocity){1.0f, 12.3f});

// Cleanup (end of program)
ecs_destroy(ecs);
```
#### Querying Entities
```c
// Query entities with both position and velocity
uint64_t signature = (1ULL << POSITION) | (1ULL << VELOCITY);
t_ecs_ulist *entities = ecs_query(ecs, signature);

// Iterate through results
for (uint32_t i = 0; i < entities->len; i++)
{
    uint32_t id = entities->data[i];
    t_position *pos = ecs_entity_get(ecs, id, POSITION);
    t_velocity *vel = ecs_entity_get(ecs, id, VELOCITY);
    // Update position
    pos->x += vel->dx;
    pos->y += vel->dy;
}
```
## ⚙️ Building

```bash
git clone https://github.com/MykleR/42-ecs.git
cd 42-ecs; make
```
> This creates `42-ecs.a` that you can link to your projects.
```bash
gcc -I 42-ecs/headers your_project.c 42-ecs/42-ecs.a -o your_program
```
> 1. Include the headers in your project
> 2. Link against `42-ecs.a`

## 📚 API Reference

### ECS Management
```c
t_ecs *ecs_create(uint32_t nb_comps, ...);  // Create ECS with component sizes
void ecs_destroy(t_ecs *ecs);               // Destroy ECS and free memory
```

### Entity Operations
```c
uint32_t ecs_entity_create(t_ecs *ecs);                    // Create new entity
uint32_t ecs_entity_clone(t_ecs *ecs, uint32_t id);        // Clone existing entity
void ecs_entity_kill(t_ecs *ecs, uint32_t id);             // Destroy entity
bool ecs_entity_alive(t_ecs *ecs, uint32_t id);            // Check if entity exists
```

### Component Operations
```c
void ecs_entity_add(t_ecs *ecs, uint32_t id, uint8_t comp, void *data);     // Add component
void ecs_entity_remove(t_ecs *ecs, uint32_t id, uint8_t comp);              // Remove component
void *ecs_entity_get(t_ecs *ecs, uint32_t id, uint8_t comp);                // Get component data
bool ecs_entity_has(t_ecs *ecs, uint32_t id, uint8_t comp);                 // Check component presence
```

### Query System
```c
t_ecs_ulist *ecs_query(t_ecs *ecs, uint64_t signature);  // Query entities by component mask
```

### Batching Operations (ecs_extra.h)
```c
#include "ecs_extra.h"

t_ecs_queue queue;
ecs_queue_create(&queue);

// Queue operations instead of executing immediately
t_ecs_queue_entry entry = {
    .type = ADD,
    .id = entity_id,
    .comp = POSITION,
    .data = &position_data
};
ecs_queue_add(&queue, entry);

// Process all queued operations at once
ecs_queue_process(ecs, &queue);
ecs_queue_destroy(&queue);
```

## 🧩 Architecture

### Component Signatures
> Components are identified using bitmasks where each bit represents a component type:
> - Component 0: `0x0000000000000001`
> - Component 1: `0x0000000000000002`
> - Component 2: `0x0000000000000004`
> - Multiple components: `(1ULL << COMP_A) | (1ULL << COMP_B)`

### Memory Layout
> - **Entities**: Up to 42,000 entities (configurable via `ECS_ENTITY_CAP`)
> - **Components**: Up to 63 component types per entity
> - **Storage**: Structure of Arrays (SoA) for cache efficiency
> - **Queries**: Pre-computed and cached for O(1) access

### Example Systems
> - **Movement System**: Query entities with Position + Velocity components
> - **Rendering System**: Query entities with Position + Sprite components
> - **Collision System**: Query entities with Position + Collider components

### Performance Notes

> - **Query Caching**: Results are cached and only updated when entities change
> - **Memory Locality**: Components are stored in contiguous arrays
> - **Minimal Allocations**: Uses free-list for entity recycling
> - **Batch Processing**: Queue system minimizes query invalidations

> [!WARNING]\
> Very minimalist ECS with **NO ARCHETYPES** involved, so memory complexity is O(N*C), with N number of entities and C memory size of all components

## License

GNU General Public License v3.0 - see [LICENSE](LICENSE) file for details.
