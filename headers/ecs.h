#pragma once

#include <ecs_vec.h>
#include <ecs_archetype.h>
#include <ecs_log.h>

typedef struct s_ecs_free_list
{
	struct s_ecs_free_list	*next; // Pointer to the next free entity in the list
}	t_ecs_free_list;

typedef struct s_ecs
{
	t_ecs_vec			vec_entities;	// Vector containing the IDs of all entities
	t_ecs_vec			vec_free_list;	// Vector for managing free entities
	t_ecs_vec			vec_archetypes; // Vector containing all archetypes
	t_ecs_vec			vec_comps;      // Vector containing all components
	uint64_t			next_entity_id; // ID to be assigned to the next entity created
}	t_ecs;
