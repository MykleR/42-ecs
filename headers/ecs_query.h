#pragma once

#include <ecs_vec.h>
#include <pp_iter.h>

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_query
{
	u64			count;
	u64			signature;
	t_ecs_vec	entities;
}	t_ecs_query;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_QUERY_DEFAULT (t_ecs_query) { \
	.count = 0, \
	.signature = 0, \
	.entities = {0} \
}

# define ECS_QUERY_DESTROY(query) do { \
	ECS_VEC_DESTROY((query).entities); \
	(query) = ECS_QUERY_DEFAULT; \
} while (0)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_BUILD_SIGNATURE_IMPL_1(a) (1ULL << (a))
# define __ECS_BUILD_SIGNATURE_IMPL_2(a,b) ((1ULL << (a)) | (1ULL << (b)))  
# define __ECS_BUILD_SIGNATURE_IMPL_3(a,b,c) ((1ULL << (a)) | (1ULL << (b)) | (1ULL << (c)))

# define __ECS_BUILD_SIGNATURE_EXPAND(...) EXPAND(__ECS_BUILD_SIGNATURE_SELECT(__VA_ARGS__, \
	__ECS_BUILD_SIGNATURE_IMPL_3, \
	__ECS_BUILD_SIGNATURE_IMPL_2, \
	__ECS_BUILD_SIGNATURE_IMPL_1, \
	)(__VA_ARGS__))

# define __ECS_BUILD_SIGNATURE_SELECT(arg1, arg2, arg3, arg4, ...) arg4

# define __ECS_BUILD_SIGNATURE(...) __ECS_BUILD_SIGNATURE_EXPAND(__VA_ARGS__)
