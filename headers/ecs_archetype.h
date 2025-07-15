#pragma once

# include <ecs_vec.h>
# include <pp_iter.h>

# define ECS_ARCH_INITSIZE 0x100

typedef struct s_ecs_archetype
{
	u64			signature;
	u32			count;
	u16			comp_count;
	u16			comp_size[63];
	t_ecs_vec	comp_data[63];
	t_ecs_vec	ids;
}	t_ecs_archetype;

#define ECS_ARCH_DEFAULT (t_ecs_archetype) \
	{0, 0, 0, {0}, {0}, {0}}

#define __ECS_ARCH_SETUP_COMP(ARG, IDX) \
	(arch).comp_count++; \
	(arch).comp_size[IDX] = sizeof(ARG); \
	ECS_VEC_INIT((arch).comp_data[IDX], ARG, ECS_ARCH_INITSIZE);

#define ECS_ARCH_INIT(arch, sig, ...) do { \
	(arch) = ECS_ARCH_DEFAULT; \
	(arch).signature = sig; \
	PP_EACH_IDX(__ECS_ARCH_SETUP_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((arch).ids, u32, ECS_ARCH_INITSIZE); \
} while (0)

#define ECS_ARCH_SHOW(arch) do { \
	LOG_INFO("Archetype: %p", &(arch)); \
	LOG_INFO("  Signature: %llu", (unsigned long long)(arch).signature); \
	LOG_INFO("  Count: %lu", (arch).count); \
	LOG_INFO("  Component Count: %lu", (arch).comp_count); \
	for (u16 i = 0; i < (arch).comp_count; i++) { \
		LOG_INFO("    Component %lu: Size = %lu", i, (arch).comp_size[i]); \
	} \
} while (0)
