#pragma once

#include <ecs_vec.h>
#include <ecs_archetype.h>
#include <ecs_log.h>
#include <ecs_event.h>

# ifndef ECS_ENTITY_MAX
#  define ECS_ENTITY_MAX 0x100000
# endif

# ifndef ECS_ENTITY_INITSIZE
#  define ECS_ENTITY_INITSIZE 0x2000
# endif

# define ECS_INITSIZE 0x20
# define ECS_MASK_ALIVE 0x8000000000000000

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_free_list {struct s_free_list *next;} t_free_list;

typedef struct s_ecs
{
	t_free_list	*next_id;
	u16			comp_sizes[ECS_ARCH_MAXCOMP];
	u16			comp_count;
	t_ecs_vec	entities;
	t_ecs_vec	archetypes;
	t_ecs_vec	queries;
}	t_ecs;


// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_DEFAULT (t_ecs) { \
	.comp_sizes = {0}, \
	.comp_count = 0, \
	.next_id = NULL, \
	.entities = {0}, \
	.archetypes = {0}, \
	.queries = {0}, \
}

# define ECS_INIT(ecs, ...) do { \
	(ecs) = ECS_DEFAULT; \
	PP_EACH(__ECS_INIT_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((ecs).archetypes, t_ecs_archetype, ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).queries, void*, ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).entities, u32, ECS_ENTITY_INITSIZE); \
} while (0)

# define ECS_DESTROY(ecs) do { \
	ECS_VEC_CLEAR_ALL((ecs).archetypes, t_ecs_archetype, ECS_ARCH_DESTROY(*_item);); \
	ECS_VEC_CLEAR_ALL((ecs).queries, void, free(_item);); \
	ECS_VEC_DESTROY((ecs).archetypes); \
	ECS_VEC_DESTROY((ecs).queries); \
	ECS_VEC_DESTROY((ecs).entities); \
	(ecs) = ECS_DEFAULT; \
} while (0)

# define ECS_ENTITY_ALIVE(id) \
	__builtin_expect((ECS_VEC_GET((ecs).entities, id, u64) & ECS_MASK_ALIVE) != 0, 1)

# define ECS_ENTITY_CREATE(ecs) ({ \
	u64 _id = (ecs).entities.len; \
	ECS_VEC_PUSH((ecs).entities, &(u64){ECS_MASK_ALIVE}); \
	_id; \
})

// TODO - Remove from archetypes
# define ECS_ENTITY_KILL(ecs, id) do { \
	if (!ECS_ENTITY_ALIVE(id)) break; \
	ECS_VEC_REMOVE((ecs).entities, id) \
} while (0)

# define ECS_ENTITY_GET(ecs, id, comp) ({ \
	void *_data = NULL; \
	if (__ECS_VEC_EXPECT_IN((ecs).entities, id)) { \
		u64 _entity = ECS_VEC_GET((ecs).entities, id, u64); \
		ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
			if (_item->signature == _entity) { \
				_data = ECS_ARCH_GET(*_item, comp, id); break; \
		}}); \
	} \
	_data; \
})

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_INIT_COMP(ARG) \
	(ecs).comp_sizes[(ecs).comp_count++] = sizeof(ARG); \

# define __ECS_LOG(ecs) do { \
	LOG_DEBUG("ECS: %lu entities, %lu components", (ecs).entities.len, (ecs).comp_count); \
	LOG_DEBUG("Archetypes: %lu", (ecs).archetypes.len); \
	LOG_DEBUG("Queries: %lu", (ecs).queries.len); \
	for (u16 i = 0; i < (ecs).comp_count; i++) \
		LOG_DEBUG("Comp[%lu]: size_t(%lu)", i, (ecs).comp_sizes[i]); \
	for (u32 i = 0; i < (ecs).entities.len; i++) \
		LOG_DEBUG("Entity[%lu]: %lu", i, ECS_VEC_GET((ecs).entities, i, u64)); \
} while (0)

// const t_query	*ecs_query(t_ecs *ecs, uint64_t signature);

// u32		ecs_entity_create	(t_ecs *ecs);
// u32		ecs_entity_clone	(t_ecs *ecs, u32 id);
// void		ecs_entity_kill		(t_ecs *ecs, u32 id);
// void		*ecs_entity_get		(t_ecs *ecs, u32 id, u8 comp);
// void		ecs_entity_remove	(t_ecs *ecs, u32 id, u8 comp);
// void		ecs_entity_add		(t_ecs *ecs, u32 id, u8 comp, void *data);
// int		ecs_entity_has		(const t_ecs *ecs, u32 id, u8 comp);
// int		ecs_entity_alive	(const t_ecs *ecs, u32 id);

