#pragma once

#include <ecs_vec.h>

typedef struct s_ecs_archetype
{
	t_ecs_vec		vec_comps;	// Vector containing the data for each entity in this archetype
	t_ecs_vec		vec_ids;	// Vector containing the IDs of entities in this archetype
	uint64_t		signature;	// Unique signature representing the components in this archetype
	uint32_t		count;		// Number of entities in this archetype
	uint64_t		mem_size;	// Size of each entity's data in bytes
}	t_ecs_archetype;
