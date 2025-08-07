#pragma once

# include <ecs_vec.h>
# include <pp_iter.h>
# include <string.h>

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
# define ECS_ARCH_GET(arch, comp_id, entity_id) ({ \
	void *_data = NULL; \
	/* Find entity index in archetype */ \
	for (u64 _entity_idx = 0; _entity_idx < (arch).ids.len; _entity_idx++) { \
		if (ECS_VEC_GET((arch).ids, _entity_idx, u32) == (u32)(entity_id)) { \
			/* Map global component ID to archetype component index */ \
			u16 _archetype_comp_idx = 0; \
			for (u16 _global_comp_id = 0; _global_comp_id < 64; _global_comp_id++) { \
				if ((arch).signature & (1ULL << _global_comp_id)) { \
					if (_global_comp_id == comp_id) { \
						_data = __ECS_VEC_PTR((arch).comp_datas[_archetype_comp_idx], _entity_idx); \
						break; \
					} \
					_archetype_comp_idx++; \
				} \
			} \
			break; \
		} \
	} \
	_data; \
})

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

// Archetype management macros
# define __ECS_ENTITY_MOVE_TO_ARCHETYPE(ecs, entity_id, new_sig, comp_id, data, is_add) do { \
	/* Remove from current archetype */ \
	u64 _old_sig = ECS_VEC_GET((ecs).entities, entity_id, u64) & ~ECS_MASK_ALIVE; \
	__ECS_REMOVE_FROM_ARCHETYPE(ecs, entity_id, _old_sig); \
	/* Update entity signature */ \
	u64 _new_sig_temp = new_sig; \
	ECS_VEC_SET((ecs).entities, entity_id, &_new_sig_temp); \
	/* Add to new archetype */ \
	__ECS_ADD_TO_ARCHETYPE(ecs, entity_id, _new_sig_temp & ~ECS_MASK_ALIVE, comp_id, data, is_add); \
} while (0)

# define __ECS_REMOVE_FROM_ARCHETYPE(ecs, entity_id, sig) do { \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		if (_item->signature == sig) { \
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

# define __ECS_ADD_TO_ARCHETYPE(ecs, entity_id, sig, comp_id, data, is_add) do { \
	t_ecs_archetype *_target_arch = NULL; \
	/* Find existing archetype */ \
	ECS_VEC_FOREACH((ecs).archetypes, t_ecs_archetype, { \
		if (_item->signature == sig) { \
			_target_arch = _item; break; \
		} \
	}); \
	/* Create new archetype if not found */ \
	if (!_target_arch) { \
		__ECS_CREATE_ARCHETYPE(ecs, sig); \
		_target_arch = (t_ecs_archetype*)__ECS_VEC_PTR((ecs).archetypes, (ecs).archetypes.len - 1); \
	} \
	/* Add entity to archetype */ \
	u32 _ent_id = (u32)(entity_id); \
	/* Check if entity is already in this archetype */ \
	u8 _already_exists = 0; \
	for (u64 _check_i = 0; _check_i < _target_arch->ids.len; _check_i++) { \
		if (ECS_VEC_GET(_target_arch->ids, _check_i, u32) == _ent_id) { \
			_already_exists = 1; break; \
		} \
	} \
	if (!_already_exists) { \
		ECS_VEC_PUSH(_target_arch->ids, _ent_id); \
		_target_arch->count++; \
		/* Add component data */ \
		u16 _archetype_comp_idx = 0; \
		for (u16 _global_comp_id = 0; _global_comp_id < (ecs).comp_count; _global_comp_id++) { \
			if (sig & (1ULL << _global_comp_id)) { \
				__ECS_VEC_REALLOC(_target_arch->comp_datas[_archetype_comp_idx]); \
				void *_comp_data_ptr = __ECS_VEC_PTR(_target_arch->comp_datas[_archetype_comp_idx], _target_arch->comp_datas[_archetype_comp_idx].len); \
				if (is_add && _global_comp_id == comp_id && data != NULL) { \
					memcpy(_comp_data_ptr, data, _target_arch->comp_sizes[_archetype_comp_idx]); \
				} else { \
					/* Push zero data for other components */ \
					memset(_comp_data_ptr, 0, _target_arch->comp_sizes[_archetype_comp_idx]); \
				} \
				_target_arch->comp_datas[_archetype_comp_idx].len++; \
				_archetype_comp_idx++; \
			} \
		} \
	} \
} while (0)

# define __ECS_CREATE_ARCHETYPE(ecs, sig) do { \
	t_ecs_archetype _new_arch = ECS_ARCH_DEFAULT; \
	_new_arch.signature = sig; \
	/* Count components in signature */ \
	for (u16 _i = 0; _i < (ecs).comp_count; _i++) { \
		if (sig & (1ULL << _i)) { \
			_new_arch.comp_sizes[_new_arch.comp_count] = (ecs).comp_sizes[_i]; \
			ECS_VEC_INIT(_new_arch.comp_datas[_new_arch.comp_count], u8, ECS_ARCH_INITSIZE); \
			_new_arch.comp_datas[_new_arch.comp_count].mem_size = (ecs).comp_sizes[_i]; \
			_new_arch.comp_count++; \
		} \
	} \
	ECS_VEC_INIT(_new_arch.ids, u32, ECS_ARCH_INITSIZE); \
	ECS_VEC_PUSH((ecs).archetypes, _new_arch); \
} while (0)

// ECS_MASK_ALIVE is defined in ecs.h but needed here
#ifndef ECS_MASK_ALIVE
# define ECS_MASK_ALIVE 0x8000000000000000
#endif
