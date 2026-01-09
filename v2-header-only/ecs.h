#pragma once

#include "ecs_vec.h"
#include "ecs_archetype.h"
#include "ecs_utils.h"
#include "ecs_query.h"

# ifndef ECS_ENTITY_MAX
#  define ECS_ENTITY_MAX 0x100000
# endif

# define ECS_MASK_ALIVE 0x8000000000000000

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_free_list {struct s_free_list *next;}	t_free_list;

typedef struct s_ecs_record
{
	u32		arch_idx;
	u32		row;
}	t_ecs_record;

# define ECS_RECORD_NONE (t_ecs_record){.arch_idx = UINT32_MAX, .row = 0}
# define ECS_RECORD_VALID(rec) ((rec).arch_idx != UINT32_MAX)

typedef struct s_ecs
{
	t_free_list		*next_id;
	u64				*masks;
	t_ecs_record	*records;
	u16				comp_sizes[ECS_ARCH_MAXCOMP];
	u16				comp_count;
	u32				entity_count;
	u32				entity_cap;
	t_ecs_vec		archetypes;
	t_ecs_vec		arch_sigs;
}	t_ecs;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_IS_INIT(ecs)		__builtin_expect((ecs).masks != NULL, 1)
# define ECS_NOT_INIT(ecs)		__builtin_expect((ecs).masks == NULL, 0)
# define ECS_NOT_EMPTY(ecs)		__builtin_expect((ecs).entity_count > 0, 1)
# define ECS_EMPTY(ecs)			__builtin_expect((ecs).entity_count == 0, 0)
# define ECS_HAS_RECYCLE(ecs)	__builtin_expect((ecs).next_id != NULL, 1)

# define ECS_DEFAULT (t_ecs){ \
	.next_id = NULL, \
	.masks = NULL, \
	.records = NULL, \
	.comp_sizes = {0}, \
	.comp_count = 0, \
	.entity_count = 0, \
	. entity_cap = 0, \
	.archetypes = {0}, \
	.arch_sigs = {0}}

# define ECS_INIT(ecs, ...) do { \
	__ECS_ASSERT_NOT_INIT(ecs); \
	(ecs) = ECS_DEFAULT; \
	(ecs).entity_cap = ECS_ENTITY_MAX; \
	(ecs).masks = (u64 *)calloc(ECS_ENTITY_MAX, sizeof(u64)); \
	(ecs).records = (t_ecs_record *)malloc(ECS_ENTITY_MAX * sizeof(t_ecs_record)); \
	ASSERT_MSG((ecs).masks && (ecs).records, "%s", "ECS allocation failed"); \
	for (u32 _i = 0; _i < ECS_ENTITY_MAX; _i++) \
		(ecs).records[_i] = ECS_RECORD_NONE; \
	PP_EACH(__ECS_INIT_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((ecs).archetypes, t_ecs_archetype, 32); \
	ECS_VEC_INIT((ecs).arch_sigs, u64, 32); \
} while (0)

# define ECS_DESTROY(ecs) do { \
	__ECS_ASSERT_INIT(ecs); \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		ECS_ARCH_DESTROY(*_item); \
	}); \
	ECS_VEC_DESTROY((ecs).archetypes); \
	ECS_VEC_DESTROY((ecs).arch_sigs); \
	free((ecs).masks); \
	free((ecs).records); \
	(ecs) = ECS_DEFAULT; \
} while (0)

// ╔════════════════════════[ ENTITY MACROS ]═══════════════════════╗

# define ECS_ENTITY_ALIVE(ecs, id) ( \
	(id) < (ecs).entity_cap && \
	((ecs).masks[id] & ECS_MASK_ALIVE) != 0)

# define ECS_ENTITY_CREATE(ecs) ({ \
	__ECS_ASSERT_INIT(ecs); \
	u32 _id; \
	if (ECS_HAS_RECYCLE(ecs)) { \
		_id = (u32)((u64 *)(ecs).next_id - (ecs).masks); \
		(ecs).next_id = (ecs).next_id->next; \
	} else { \
		_id = (ecs).entity_count; \
	} \
	(ecs).masks[_id] = ECS_MASK_ALIVE; \
	(ecs).records[_id] = ECS_RECORD_NONE; \
	(ecs).entity_count++; \
	_id; \
})

# define ECS_ENTITY_KILL(ecs, id) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_ENTITY_ALIVE(ecs, id)) break; \
	if (ECS_RECORD_VALID((ecs).records[id])) { \
		t_ecs_archetype *_arch = __ECS_ARCH_PTR(ecs, (ecs).records[id].arch_idx); \
		u32 _moved = ECS_ARCH_REMOVE(*_arch, (ecs).records[id].row); \
		if (_moved != 0) \
			(ecs).records[_moved].row = (ecs).records[id].row; \
	} \
	(ecs).masks[id] = 0; \
	(ecs).records[id] = ECS_RECORD_NONE; \
	t_free_list *_node = (t_free_list *)&(ecs).masks[id]; \
	_node->next = (ecs).next_id; \
	(ecs).next_id = _node; \
	(ecs).entity_count--; \
} while (0)

# define ECS_ENTITY_HAS(ecs, id, comp_bit) \
	(((ecs).masks[id] & (1ULL << (comp_bit))) != 0)

// ╔═════════════════════[ COMPONENT MACROS ]═══════════════════════╗

# define ECS_GET(ecs, id, comp_bit, type) ({ \
	type *_ptr = NULL; \
	if (ECS_ENTITY_HAS(ecs, id, comp_bit) && \
		ECS_RECORD_VALID((ecs).records[id])) { \
		t_ecs_record _rec = (ecs).records[id]; \
		t_ecs_archetype *_arch = __ECS_ARCH_PTR(ecs, _rec.arch_idx); \
		_ptr = (type *)ECS_ARCH_GET(*_arch, comp_bit, _rec.row); \
	} \
	_ptr; \
})

# define ECS_ADD(ecs, id, comp_bit, data_ptr) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_ENTITY_ALIVE(ecs, id)) break; \
	u64 _old_sig = (ecs).masks[id] & ~ECS_MASK_ALIVE; \
	u64 _new_sig = _old_sig | (1ULL << (comp_bit)); \
	if (_old_sig != _new_sig) \
		__ECS_MIGRATE(ecs, id, _old_sig, _new_sig); \
	(ecs).masks[id] = _new_sig | ECS_MASK_ALIVE; \
	t_ecs_record _rec = (ecs).records[id]; \
	t_ecs_archetype *_arch = __ECS_ARCH_PTR(ecs, _rec.arch_idx); \
	ECS_ARCH_SET(*_arch, comp_bit, _rec.row, data_ptr); \
} while (0)

# define ECS_REMOVE(ecs, id, comp_bit) do { \
	__ECS_ASSERT_INIT(ecs); \
	if (!ECS_ENTITY_ALIVE(ecs, id)) break; \
	if (! ECS_ENTITY_HAS(ecs, id, comp_bit)) break; \
	u64 _old_sig = (ecs).masks[id] & ~ECS_MASK_ALIVE; \
	u64 _new_sig = _old_sig & ~(1ULL << (comp_bit)); \
	__ECS_MIGRATE(ecs, id, _old_sig, _new_sig); \
	(ecs).masks[id] = _new_sig | ECS_MASK_ALIVE; \
} while (0)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_INIT_COMP(ARG) \
	(ecs).comp_sizes[(ecs).comp_count++] = sizeof(ARG);

# define __ECS_ARCH_PTR(ecs, idx) \
	((t_ecs_archetype *)(ecs).archetypes.data + (idx))

# define __ECS_FIND_ARCHETYPE(ecs, sig) ({ \
	i32 _found = -1; \
	for (u32 _i = 0; _i < (ecs).arch_sigs.len; _i++) { \
		if (ECS_VEC_GET((ecs).arch_sigs, _i, u64) == (sig)) { \
			_found = (i32)_i; \
			break; \
		} \
	} \
	_found; \
})

# define __ECS_CREATE_ARCHETYPE(ecs, sig) ({ \
	t_ecs_archetype _new_arch = {0}; \
	_new_arch.signature = (sig); \
	u64 _sig_tmp = (sig); \
	for (u8 _bit = 0; _bit < ECS_ARCH_MAXCOMP && _sig_tmp; _bit++) { \
		if (_sig_tmp & (1ULL << _bit)) { \
			_new_arch. comp_map[_bit] = _new_arch.comp_count; \
			_new_arch.comp_sizes[_new_arch.comp_count] = (ecs).comp_sizes[_bit]; \
			_new_arch.comp_datas[_new_arch.comp_count] = ECS_VEC_DEFAULT; \
			_new_arch.comp_datas[_new_arch.comp_count].mem_size = (ecs).comp_sizes[_bit]; \
			_new_arch.comp_datas[_new_arch.comp_count]. data = \
				calloc(ECS_ARCH_INITSIZE, (ecs).comp_sizes[_bit]); \
			_new_arch.comp_datas[_new_arch.comp_count].cap = ECS_ARCH_INITSIZE; \
			_new_arch.comp_count++; \
			_sig_tmp &= ~(1ULL << _bit); \
		} \
	} \
	ECS_VEC_INIT(_new_arch.ids, u32, ECS_ARCH_INITSIZE); \
	ECS_VEC_PUSH((ecs).archetypes, _new_arch); \
	ECS_VEC_PUSH((ecs).arch_sigs, (sig)); \
	(i32)((ecs).archetypes.len - 1); \
})

# define __ECS_MIGRATE(ecs, id, old_sig, new_sig) do { \
	t_ecs_record _old_rec = (ecs).records[id]; \
	if ((new_sig) != 0) { \
		i32 _new_idx = __ECS_FIND_ARCHETYPE(ecs, new_sig); \
		if (_new_idx < 0) \
			_new_idx = __ECS_CREATE_ARCHETYPE(ecs, new_sig); \
		t_ecs_archetype *_new_arch = __ECS_ARCH_PTR(ecs, _new_idx); \
		u32 _new_row = ECS_ARCH_ADD(*_new_arch, id); \
		if (ECS_RECORD_VALID(_old_rec)) { \
			t_ecs_archetype *_old_arch = __ECS_ARCH_PTR(ecs, _old_rec. arch_idx); \
			ECS_ARCH_COPY(*_new_arch, _new_row, *_old_arch, _old_rec.row); \
		} \
		(ecs).records[id] = (t_ecs_record){.arch_idx = (u32)_new_idx, .row = _new_row}; \
	} \
	if (ECS_RECORD_VALID(_old_rec)) { \
		t_ecs_archetype *_old_arch = __ECS_ARCH_PTR(ecs, _old_rec.arch_idx); \
		u32 _moved = ECS_ARCH_REMOVE(*_old_arch, _old_rec.row); \
		if (_moved != 0) \
			(ecs).records[_moved].row = _old_rec.row; \
	} \
	if ((new_sig) == 0) \
		(ecs).records[id] = ECS_RECORD_NONE; \
} while (0)

// ╔════════════════════════[ ASSERTIONS ]══════════════════════════╗

# define __ECS_ASSERT_INIT(ecs) \
	ASSERT_MSG(ECS_IS_INIT(ecs), "%s", "ECS is not initialized")
# define __ECS_ASSERT_NOT_INIT(ecs) \
	ASSERT_MSG(ECS_NOT_INIT(ecs), "%s", "ECS is already initialized")

// ╔══════════════════════════[ DEBUG ]═════════════════════════════╗

# define __ECS_LOG(ecs) do { \
	LOG_DEBUG("ECS: %u entities, %u components", \
		(ecs).entity_count, (ecs).comp_count); \
	LOG_DEBUG("Archetypes: %lu", (ecs).archetypes.len); \
	for (u16 _i = 0; _i < (ecs).comp_count; _i++) \
		LOG_DEBUG("  Comp[%u]: size=%u", _i, (ecs).comp_sizes[_i]); \
} while (0)
