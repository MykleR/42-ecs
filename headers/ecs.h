#pragma once

#include <ecs_vec.h>
#include <ecs_archetype.h>
#include <ecs_log.h>

# ifndef ECS_ENTITY_MAX
#  define ECS_ENTITY_MAX 0x100000
# endif

# ifndef ECS_ENTITY_INITSIZE
#  define ECS_ENTITY_INITSIZE 0x2000
# endif

# define ECS_INITSIZE 0x20

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_free_list {struct s_free_list *next;} t_free_list;

typedef struct s_ecs
{
	u16				comp_sizes[ECS_ARCH_MAXCOMP];
	u16				comp_count;
	u32				entity_count;
	t_ecs_vec		entities;
	t_ecs_vec		archetypes;
	t_ecs_vec		queries;
	t_free_list		*next_id;
}	t_ecs;


// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_DEFAULT (t_ecs) { \
	.comp_sizes = {0}, \
	.comp_count = 0, \
	.entity_count = 0, \
	.entities = {0}, \
	.archetypes = {0}, \
	.queries = {0}, \
	.next_id = NULL \
}

# define __ECS_INIT_COMP(ARG, IDX) \
	(ecs).comp_count += (ecs).comp_count < ECS_ARCH_MAXCOMP; \
	(ecs).comp_sizes[IDX] = sizeof(ARG); \

# define ECS_INIT(ecs, ...) do { \
	(ecs) = ECS_DEFAULT; \
	PP_EACH_IDX(__ECS_INIT_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((ecs).archetypes, t_ecs_archetype, ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).queries, sizeof(void*), ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).entities, u32, ECS_ENTITY_INITSIZE); \
} while (0)

# define ECS_DESTROY(ecs) do { \
	ECS_VEC_CLEAR_ALL((ecs).archetypes, ECS_ARCH_DESTROY(*(t_ecs_archetype *)_item);); \
	ECS_VEC_CLEAR_ALL((ecs).queries, free(*(void **)_item);); \
	ECS_VEC_DESTROY((ecs).archetypes); \
	ECS_VEC_DESTROY((ecs).queries); \
	ECS_VEC_DESTROY((ecs).entities); \
	(ecs) = ECS_DEFAULT; \
} while (0)

// int				ecs_init(t_ecs *ecs, u16 comp_count, ...);
// void				ecs_destroy(t_ecs *ecs);
// const t_query	*ecs_query(t_ecs *ecs, uint64_t signature);

// u32		ecs_entity_create	(t_ecs *ecs);
// u32		ecs_entity_clone	(t_ecs *ecs, u32 id);
// void		ecs_entity_kill		(t_ecs *ecs, u32 id);
// void		*ecs_entity_get		(t_ecs *ecs, u32 id, u8 comp);
// void		ecs_entity_remove	(t_ecs *ecs, u32 id, u8 comp);
// void		ecs_entity_add		(t_ecs *ecs, u32 id, u8 comp, void *data);
// int		ecs_entity_has		(const t_ecs *ecs, u32 id, u8 comp);
// int		ecs_entity_alive	(const t_ecs *ecs, u32 id);

