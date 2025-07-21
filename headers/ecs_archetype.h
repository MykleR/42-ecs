#pragma once

# include <ecs_vec.h>
# include <pp_iter.h>

# ifndef ECS_ARCH_INITSIZE
#  define ECS_ARCH_INITSIZE 0x100
# endif

# define ECS_ARCH_MAXCOMP 63

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_archetype
{
	u64			signature;
	u32			count;
	u16			comp_count;
	u16			comp_sizes[ECS_ARCH_MAXCOMP];
	t_ecs_vec	comp_datas[ECS_ARCH_MAXCOMP];
	t_ecs_vec	ids;
}	t_ecs_archetype;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_ARCH_DEFAULT (t_ecs_archetype) { \
	.comp_sizes = {0}, \
	.comp_datas = {0}, \
	.comp_count = 0, \
	.signature = 0, \
	.count = 0, \
	.ids = {0}}

# define ECS_ARCH_INIT(arch, sig, ...) do { \
	(arch) = ECS_ARCH_DEFAULT; \
	(arch).signature = sig; \
	PP_EACH_IDX(__ECS_ARCH_SETUP_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((arch).ids, u32, ECS_ARCH_INITSIZE); \
} while (0)

# define ECS_ARCH_DESTROY(arch) do { \
	for (u16 i = 0; i < (arch).comp_count; i++) \
		ECS_VEC_DESTROY((arch).comp_datas[i]); \
	ECS_VEC_DESTROY((arch).ids); \
	(arch) = ECS_ARCH_DEFAULT; \
} while (0)

// TODO : FETCH with correct index and type in data
# define ECS_ARCH_GET(arch, comp_id, entity_id) ( \
	assert(__ECS_VEC_EXPECT_IN((arch).ids, entity_id)), \
	ECS_VEC_GET((arch).comp_datas[comp_id], 0, void *)) \
)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_ARCH_SETUP_COMP(ARG, IDX) \
	(arch).comp_count += (arch).comp_count < ECS_ARCH_MAXCOMP; \
	(arch).comp_sizes[IDX] = sizeof(ARG); \
	ECS_VEC_INIT((arch).comp_datas[IDX], ARG, ECS_ARCH_INITSIZE);

#define __ECS_ARCH_LOG_COMP(arch, idx) \
	LOG_DEBUG("Comp[%lu]: size_t(%lu), data(%lu)", \
		idx, (arch).comp_sizes[idx], (arch).comp_datas[idx].len)

# define __ECS_ARCH_LOG(arch) do { \
	LOG_DEBUG("Archetype: %lu",		(arch).signature); \
	LOG_DEBUG("	Entities: %lu",		(arch).count); \
	LOG_DEBUG("	Components: %lu",	(arch).comp_count); \
	for (u16 i = 0; i < (arch).comp_count; i++) \
		__ECS_ARCH_LOG_COMP(arch, i); \
} while (0)
