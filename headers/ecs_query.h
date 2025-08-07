#pragma once

#include <ecs_vec.h>

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_query_result
{
	u64			count;
	u64			signature;
	t_ecs_vec	entities;
}	t_ecs_query_result;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_QUERY_DEFAULT (t_ecs_query_result) { \
	.count = 0, \
	.signature = 0, \
	.entities = {0} \
}

# define ECS_QUERY_DESTROY(query) do { \
	ECS_VEC_DESTROY((query).entities); \
	(query) = ECS_QUERY_DEFAULT; \
} while (0)
