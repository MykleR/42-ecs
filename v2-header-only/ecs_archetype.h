#pragma once

# include "ecs_vec.h"
# include "pp_iter.h"

# ifndef ECS_ARCH_INITSIZE
#  define ECS_ARCH_INITSIZE 0x100
# endif

# define ECS_ARCH_MAXCOMP 63
# define ECS_ARCH_INVALID_IDX 0xFF

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_archetype
{
	u64			signature;
	u32			count;
	u16			comp_count;
	u8			comp_map[ECS_ARCH_MAXCOMP];
	u16			comp_sizes[ECS_ARCH_MAXCOMP];
	t_ecs_vec	comp_datas[ECS_ARCH_MAXCOMP];
	t_ecs_vec	ids;
}	t_ecs_archetype;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_ARCH_DEFAULT (t_ecs_archetype) { \
	.comp_sizes = {0}, \
	.comp_datas = {0}, \
	. comp_map = {[0 ... ECS_ARCH_MAXCOMP - 1] = ECS_ARCH_INVALID_IDX}, \
	.comp_count = 0, \
	.signature = 0, \
	.count = 0, \
	. ids = {0}}

# define ECS_ARCH_HAS(arch, comp_bit) \
	(((arch).signature & (1ULL << (comp_bit))) != 0)

# define ECS_ARCH_MATCH(arch, sig) \
	(((arch).signature & (sig)) == (sig))

// Initialize archetype with signature and component types
// Usage: ECS_ARCH_INIT(arch, sig, Position, Velocity, ...)
# define ECS_ARCH_INIT(arch, sig, ...) do { \
	(arch) = ECS_ARCH_DEFAULT; \
	(arch).signature = (sig); \
	__ECS_ARCH_BUILD_MAP(arch, sig); \
	PP_EACH_IDX(__ECS_ARCH_SETUP_COMP, __VA_ARGS__); \
	ECS_VEC_INIT((arch).ids, u32, ECS_ARCH_INITSIZE); \
} while (0)

# define ECS_ARCH_DESTROY(arch) do { \
	for (u16 _i = 0; _i < (arch).comp_count; _i++) \
		ECS_VEC_DESTROY((arch).comp_datas[_i]); \
	ECS_VEC_DESTROY((arch).ids); \
	(arch) = ECS_ARCH_DEFAULT; \
} while (0)

// Get component pointer for entity at given row
// Returns:  pointer to component data (cast to appropriate type)
# define ECS_ARCH_GET(arch, comp_bit, row) ({ \
	void *_ptr = NULL; \
	u8 _idx = (arch).comp_map[comp_bit]; \
	if (_idx != ECS_ARCH_INVALID_IDX && (row) < (arch).count) \
		_ptr = __ECS_VEC_PTR((arch).comp_datas[_idx], row); \
	_ptr; \
})

// Get typed component at row (no safety check version for iteration)
# define ECS_ARCH_GET_FAST(arch, comp_bit, row, type) \
	(*(type *)__ECS_VEC_PTR((arch).comp_datas[(arch).comp_map[comp_bit]], row))

// Add entity to archetype, returns row index
# define ECS_ARCH_ADD(arch, entity_id) ({ \
	u32 _row = (arch).count++; \
	ECS_VEC_PUSH((arch).ids, entity_id); \
	for (u16 _i = 0; _i < (arch).comp_count; _i++) { \
		(arch).comp_datas[_i].len++; \
		__ECS_VEC_REALLOC((arch).comp_datas[_i]); \
	} \
	_row; \
})

// Remove entity at row (swap-remove for O(1))
// Returns: entity_id that was moved to fill the gap (or 0 if last)
# define ECS_ARCH_REMOVE(arch, row) ({ \
	u32 _moved_id = 0; \
	u32 _last = --(arch).count; \
	if ((row) != _last) { \
		_moved_id = ECS_VEC_GET((arch).ids, _last, u32); \
		ECS_VEC_SET((arch).ids, row, &_moved_id); \
		for (u16 _i = 0; _i < (arch).comp_count; _i++) \
			memcpy(__ECS_VEC_PTR((arch).comp_datas[_i], row), \
				   __ECS_VEC_PTR((arch).comp_datas[_i], _last), \
				   (arch).comp_datas[_i].mem_size); \
	} \
	(arch).ids.len--; \
	for (u16 _i = 0; _i < (arch).comp_count; _i++) \
		(arch).comp_datas[_i].len--; \
	_moved_id; \
})

// Set component data at row
# define ECS_ARCH_SET(arch, comp_bit, row, data_ptr) do { \
	u8 _idx = (arch).comp_map[comp_bit]; \
	if (_idx != ECS_ARCH_INVALID_IDX) \
		memcpy(__ECS_VEC_PTR((arch).comp_datas[_idx], row), \
			   data_ptr, (arch).comp_datas[_idx].mem_size); \
} while (0)

// Copy all components from src_row in src_arch to dst_row in dst_arch
// Only copies components that exist in both archetypes
# define ECS_ARCH_COPY(dst_arch, dst_row, src_arch, src_row) do { \
	u64 _common = (dst_arch).signature & (src_arch).signature; \
	for (u8 _bit = 0; _bit < ECS_ARCH_MAXCOMP && _common; _bit++) { \
		if (_common & (1ULL << _bit)) { \
			u8 _dst_idx = (dst_arch).comp_map[_bit]; \
			u8 _src_idx = (src_arch).comp_map[_bit]; \
			memcpy(__ECS_VEC_PTR((dst_arch).comp_datas[_dst_idx], dst_row), \
				   __ECS_VEC_PTR((src_arch).comp_datas[_src_idx], src_row), \
				   (dst_arch).comp_datas[_dst_idx].mem_size); \
			_common &= ~(1ULL << _bit); \
		} \
	} \
} while (0)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_ARCH_SETUP_COMP(ARG, IDX) \
	(arch).comp_sizes[IDX] = sizeof(ARG); \
	ECS_VEC_INIT((arch).comp_datas[IDX], ARG, ECS_ARCH_INITSIZE); \
	(arch).comp_count++;

// Build component map from signature (bit position → dense index)
# define __ECS_ARCH_BUILD_MAP(arch, sig) do { \
	u64 _sig = (sig); \
	u8 _dense = 0; \
	for (u8 _bit = 0; _bit < ECS_ARCH_MAXCOMP && _sig; _bit++) { \
		if (_sig & (1ULL << _bit)) { \
			(arch).comp_map[_bit] = _dense++; \
			_sig &= ~(1ULL << _bit); \
		} \
	} \
} while (0)

# define __ECS_ARCH_LOG(arch) do { \
	LOG_DEBUG("Archetype: sig=0x%llx, count=%u, comps=%u", \
		(unsigned long long)(arch).signature, (arch).count, (arch).comp_count); \
	for (u16 _i = 0; _i < (arch).comp_count; _i++) \
		LOG_DEBUG("  Comp[%u]: size=%u, len=%lu", \
			_i, (arch).comp_sizes[_i], (arch).comp_datas[_i].len); \
} while (0)
