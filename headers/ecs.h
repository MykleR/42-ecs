#pragma once

#include <ecs_vec.h>
#include <ecs_archetype.h>
#include <ecs_log.h>
#include <ecs_event.h>
#include <ecs_query.h>

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
	u16			entity_count;
	t_ecs_vec	entities;
	t_ecs_vec	archetypes;
	t_ecs_vec	queries;
}	t_ecs;


// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_IS_INIT(ecs)	__builtin_expect((ecs).comp_count > 0, 1)
# define ECS_NOT_INIT(ecs)	__builtin_expect((ecs).comp_count == 0, 0)
# define ECS_NOT_EMPTY(ecs)	__builtin_expect((ecs).entity_count > 0, 1)
# define ECS_EMPTY(ecs)		__builtin_expect((ecs).entity_count == 0, 0)
# define ECS_HAS_RECYCLE(ecs)	__builtin_expect((ecs).next_id != NULL, 1)

# define ECS_DEFAULT (t_ecs) { \
	.comp_sizes = {0}, \
	.comp_count = 0, \
	.next_id = NULL, \
	.entities = {0}, \
	.archetypes = {0}, \
	.queries = {0}, \
}

# define ECS_INIT(ecs, ...) do { \
	__ECS_ASSERT_NOT_INIT(ecs); \
	(ecs) = ECS_DEFAULT; \
	PP_EACH(__ECS_INIT_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((ecs).archetypes, t_ecs_archetype, ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).queries, void*, ECS_INITSIZE); \
	ECS_VEC_INIT((ecs).entities, u64, ECS_ENTITY_INITSIZE); \
} while (0)

# define ECS_DESTROY(ecs) do { \
	__ECS_ASSERT_INIT(ecs); \
	ECS_VEC_CLEAR_ALL((ecs).archetypes, t_ecs_archetype, ECS_ARCH_DESTROY(*_item);); \
	ECS_VEC_CLEAR_ALL((ecs).queries, void, free(_item);); \
	ECS_VEC_DESTROY((ecs).archetypes); \
	ECS_VEC_DESTROY((ecs).queries); \
	ECS_VEC_DESTROY((ecs).entities); \
	(ecs) = ECS_DEFAULT; \
} while (0)

# define ECS_ENTITY_ALIVE(id) ( \
	__ECS_ASSERT_INIT(ecs), \
	(ECS_VEC_GET((ecs).entities, id, u64) & ECS_MASK_ALIVE) != 0 \
)

# define ECS_ENTITY_CREATE(ecs) ({ \
	__ECS_ASSERT_INIT(ecs); \
	u64 _id = (ecs).entities.len; \
	if (ECS_HAS_RECYCLE(ecs)) __ECS_DEQUEUE(ecs) \
	else ECS_VEC_PUSH((ecs).entities, (u64){ECS_MASK_ALIVE}); \
	(ecs).entity_count++; \
	_id; \
})

// TODO - Remove from archetypes
# define ECS_ENTITY_KILL(ecs, id) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_ENTITY_ALIVE(id)) break; \
	__ECS_QUEUE(ecs, id); \
	(ecs).entity_count--; \
} while (0)

# define ECS_ENTITY_GET(ecs, id, comp) ({ \
	void *_data = NULL; \
	if (ECS_VEC_IN((ecs).entities, id)) { \
		u64 _entity = ECS_VEC_GET((ecs).entities, id, u64); \
		ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
			if (_item->signature == _entity) { \
				_data = ECS_ARCH_GET(*_item, comp, id); break; \
		}}); \
	} \
	_data; \
})

# define ECS_QUERY(ecs, ...) ({ \
	u64 _signature = __ECS_BUILD_SIGNATURE(__VA_ARGS__); \
	t_ecs_query_result _result = ECS_QUERY_DEFAULT; \
	_result.signature = _signature; \
	ECS_VEC_INIT(_result.entities, u32, ECS_INITSIZE); \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		if ((_item->signature & _signature) == _signature) { \
			ECS_VEC_FOREACH(_item->ids, u32, { \
				u32 _entity_id = *_item; \
				if (ECS_VEC_IN((ecs).entities, _entity_id) && \
					(ECS_VEC_GET((ecs).entities, _entity_id, u64) & ECS_MASK_ALIVE)) { \
					ECS_VEC_PUSH(_result.entities, _entity_id); \
					_result.count++; \
				} \
			}); \
		} \
	}); \
	_result; \
})

# define ECS_ENTITY_ADD(ecs, entity_id, comp_id, data) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_VEC_IN((ecs).entities, entity_id)) break; \
	u64 _entity_sig = ECS_VEC_GET((ecs).entities, entity_id, u64); \
	if (!(_entity_sig & ECS_MASK_ALIVE)) break; \
	u64 _new_sig = (_entity_sig & ~ECS_MASK_ALIVE) | (1ULL << (comp_id)) | ECS_MASK_ALIVE; \
	__ECS_ENTITY_MOVE_TO_ARCHETYPE(ecs, entity_id, _new_sig, comp_id, data, 1); \
} while (0)

# define ECS_ENTITY_REMOVE(ecs, entity_id, comp_id) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_VEC_IN((ecs).entities, entity_id)) break; \
	u64 _entity_sig = ECS_VEC_GET((ecs).entities, entity_id, u64); \
	if (!(_entity_sig & ECS_MASK_ALIVE)) break; \
	u64 _new_sig = (_entity_sig & ~ECS_MASK_ALIVE & ~(1ULL << (comp_id))) | ECS_MASK_ALIVE; \
	__ECS_ENTITY_MOVE_TO_ARCHETYPE(ecs, entity_id, _new_sig, comp_id, NULL, 0); \
} while (0)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗


# define __ECS_INIT_COMP(ARG) \
	(ecs).comp_sizes[(ecs).comp_count++] = sizeof(ARG); \

# define __ECS_BUILD_SIGNATURE(...) \
	(__ECS_SIGNATURE_OR(0, ##__VA_ARGS__))

# define __ECS_SIGNATURE_OR(acc, comp, ...) \
	IF(NOT_EMPTY(__VA_ARGS__))( \
		__ECS_SIGNATURE_OR((acc) | (1ULL << (comp)), ##__VA_ARGS__), \
		(acc) | (1ULL << (comp)) \
	)

# define __ECS_DEQUEUE(ecs) { \
	_id = ((u64*)(ecs).next_id - (u64*)(ecs).entities.data); \
	(ecs).next_id = (ecs).next_id->next; \
	u64 _alive = ECS_MASK_ALIVE; \
	ECS_VEC_SET((ecs).entities, _id, &_alive); \
}

# define __ECS_QUEUE(ecs, id) do { \
	void *_entity = __ECS_VEC_PTR((ecs).entities, id); \
	((t_free_list*)_entity)->next = (ecs).next_id; \
	(ecs).next_id = (t_free_list*)_entity; \
} while (0)

# define __ECS_ENTITY_MOVE_TO_ARCHETYPE(ecs, entity_id, new_sig, comp_id, data, is_add) do { \
	/* Remove from current archetype */ \
	u64 _old_sig = ECS_VEC_GET((ecs).entities, entity_id, u64) & ~ECS_MASK_ALIVE; \
	__ECS_REMOVE_FROM_ARCHETYPE(ecs, entity_id, _old_sig); \
	/* Update entity signature */ \
	ECS_VEC_SET((ecs).entities, entity_id, &new_sig); \
	/* Add to new archetype */ \
	__ECS_ADD_TO_ARCHETYPE(ecs, entity_id, new_sig, comp_id, data, is_add); \
} while (0)

# define __ECS_REMOVE_FROM_ARCHETYPE(ecs, entity_id, signature) do { \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		if (_item->signature == signature) { \
			/* Find and remove entity from this archetype */ \
			for (u64 _arch_i = 0; _arch_i < _item->ids.len; _arch_i++) { \
				if (ECS_VEC_GET(_item->ids, _arch_i, u32) == (u32)(entity_id)) { \
					ECS_VEC_REMOVE(_item->ids, _arch_i); \
					_item->count--; \
					/* Remove component data */ \
					for (u16 _comp_i = 0; _comp_i < _item->comp_count; _comp_i++) { \
						ECS_VEC_REMOVE(_item->comp_datas[_comp_i], _arch_i); \
					} \
					break; \
				} \
			} \
			break; \
		} \
	}); \
} while (0)

# define __ECS_ADD_TO_ARCHETYPE(ecs, entity_id, signature, comp_id, data, is_add) do { \
	t_ecs_archetype *_target_arch = NULL; \
	/* Find existing archetype */ \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		if (_item->signature == signature) { \
			_target_arch = _item; break; \
		} \
	}); \
	/* Create new archetype if not found */ \
	if (!_target_arch) { \
		__ECS_CREATE_ARCHETYPE(ecs, signature); \
		_target_arch = (t_ecs_archetype*)__ECS_VEC_PTR((ecs).archetypes, (ecs).archetypes.len - 1); \
	} \
	/* Add entity to archetype */ \
	u32 _ent_id = (u32)(entity_id); \
	ECS_VEC_PUSH(_target_arch->ids, _ent_id); \
	_target_arch->count++; \
	/* Add component data - simplified for now */ \
	for (u16 _comp_idx = 0; _comp_idx < _target_arch->comp_count; _comp_idx++) { \
		__ECS_VEC_REALLOC(_target_arch->comp_datas[_comp_idx]); \
		if (is_add && _comp_idx == comp_id && data) { \
			memcpy(__ECS_VEC_PTR(_target_arch->comp_datas[_comp_idx], _target_arch->comp_datas[_comp_idx].len), \
				   data, _target_arch->comp_sizes[_comp_idx]); \
		} else { \
			/* Push zero data for other components */ \
			memset(__ECS_VEC_PTR(_target_arch->comp_datas[_comp_idx], _target_arch->comp_datas[_comp_idx].len), \
				   0, _target_arch->comp_sizes[_comp_idx]); \
		} \
		_target_arch->comp_datas[_comp_idx].len++; \
	} \
} while (0)

# define __ECS_CREATE_ARCHETYPE(ecs, signature) do { \
	t_ecs_archetype _new_arch = ECS_ARCH_DEFAULT; \
	_new_arch.signature = signature; \
	/* Count components in signature */ \
	for (u16 _i = 0; _i < (ecs).comp_count; _i++) { \
		if (signature & (1ULL << _i)) { \
			_new_arch.comp_sizes[_new_arch.comp_count] = (ecs).comp_sizes[_i]; \
			ECS_VEC_INIT(_new_arch.comp_datas[_new_arch.comp_count], u8, ECS_ARCH_INITSIZE); \
			_new_arch.comp_datas[_new_arch.comp_count].mem_size = (ecs).comp_sizes[_i]; \
			_new_arch.comp_count++; \
		} \
	} \
	ECS_VEC_INIT(_new_arch.ids, u32, ECS_ARCH_INITSIZE); \
	ECS_VEC_PUSH((ecs).archetypes, _new_arch); \
} while (0)

# define __ECS_LOG(ecs) do { \
	LOG_DEBUG("ECS: %lu entities, %lu components", (ecs).entities.len, (ecs).comp_count); \
	LOG_DEBUG("Archetypes: %lu", (ecs).archetypes.len); \
	LOG_DEBUG("Queries: %lu", (ecs).queries.len); \
	for (u16 i = 0; i < (ecs).comp_count; i++) \
		LOG_DEBUG("Comp[%lu]: size_t(%lu)", i, (ecs).comp_sizes[i]); \
	for (u32 i = 0; i < (ecs).entities.len; i++) \
		LOG_DEBUG("Entity[%lu]: %lu", i, ECS_VEC_GET((ecs).entities, i, u64)); \
} while (0)


# ifndef NDEBUG
#  define __ECS_ASSERT_MSG(cond, msg) assert(cond && msg)
# else
#  define __ECS_ASSERT_MSG(cond, msg) ((void)0)
# endif
# define __ECS_ASSERT_INIT(ecs)			__ECS_ASSERT_MSG(ECS_IS_INIT(ecs), "ECS is not initialized")
# define __ECS_ASSERT_NOT_INIT(ecs)		__ECS_ASSERT_MSG(ECS_NOT_INIT(ecs), "ECS is already initialized")
# define __ECS_ASSERT_NOT_EMPTY(ecs)	__ECS_ASSERT_MSG(ECS_NOT_EMPTY(ecs), "ECS is empty")

// const t_query	*ecs_query(t_ecs *ecs, uint64_t signature);

// u32		ecs_entity_clone	(t_ecs *ecs, u32 id);
// void		ecs_entity_kill		(t_ecs *ecs, u32 id);
// void		ecs_entity_remove	(t_ecs *ecs, u32 id, u8 comp);
// void		ecs_entity_add		(t_ecs *ecs, u32 id, u8 comp, void *data);
// int		ecs_entity_has		(const t_ecs *ecs, u32 id, u8 comp);

