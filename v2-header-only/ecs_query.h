#pragma once

# include "ecs_vec.h"
# include "ecs_archetype.h"

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_query
{
	u64			include;		// Components that MUST be present
	u64			exclude;		// Components that MUST NOT be present
	t_ecs_vec	arch_indices;	// Cached archetype indices matching query
	u8			dirty;			// Set when archetypes change
}	t_ecs_query;

typedef struct s_ecs_query_iter
{
	t_ecs_query		*query;
	void			*ecs;			// Pointer to t_ecs (forward decl)
	u32				arch_cursor;	// Current archetype index in query
	u32				row_cursor;		// Current row in archetype
	u32				entity_id;		// Current entity being iterated
	t_ecs_archetype	*current_arch;	// Cached current archetype ptr
}	t_ecs_query_iter;

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_QUERY_DEFAULT (t_ecs_query){ \
	.include = 0, \
	.exclude = 0, \
	.arch_indices = {0}, \
	.dirty = 1}

// Create query with required components
// Usage: ECS_QUERY(query, COMP_POS | COMP_VEL, COMP_DISABLED)
# define ECS_QUERY_INIT(query, inc, exc) do { \
	(query) = ECS_QUERY_DEFAULT; \
	(query).include = (inc); \
	(query).exclude = (exc); \
	ECS_VEC_INIT((query).arch_indices, u32, 16); \
} while (0)

# define ECS_QUERY_DESTROY(query) do { \
	ECS_VEC_DESTROY((query).arch_indices); \
	(query) = ECS_QUERY_DEFAULT; \
} while (0)

// Check if archetype matches query
# define ECS_QUERY_MATCH(query, arch) ( \
	(((arch).signature & (query).include) == (query).include) && \
	(((arch).signature & (query).exclude) == 0) \
)

// Refresh query cache (call when archetypes are added)
# define ECS_QUERY_REFRESH(ecs, query) do { \
	if (!(query).dirty) break; \
	ECS_VEC_CLEAR((query).arch_indices); \
	for (u32 _i = 0; _i < (ecs).archetypes.len; _i++) { \
		t_ecs_archetype *_arch = __ECS_ARCH_PTR(ecs, _i);\
		if (ECS_QUERY_MATCH(query, *_arch)) \
			ECS_VEC_PUSH((query).arch_indices, _i); \
	} \
	(query).dirty = 0; \
} while (0)

// ╔═════════════════════════[ ITERATION ]══════════════════════════╗

// Begin iteration over query results
# define ECS_QUERY_BEGIN(ecs, query) ({ \
	ECS_QUERY_REFRESH(ecs, query); \
	t_ecs_query_iter _iter = { \
		.query = &(query), \
		.ecs = &(ecs), \
		.arch_cursor = 0, \
		.row_cursor = 0, \
		.entity_id = 0, \
		.current_arch = NULL \
	}; \
	__ECS_QUERY_ADVANCE_ARCH(&_iter, &(ecs)); \
	_iter; \
})

// Check if iteration is complete
# define ECS_QUERY_DONE(iter) \
	((iter).arch_cursor >= (iter).query->arch_indices.len)

// Advance to next entity
# define ECS_QUERY_NEXT(iter, ecs) do { \
	(iter).row_cursor++; \
	if ((iter).row_cursor >= (iter).current_arch->count) { \
		(iter).arch_cursor++; \
		(iter).row_cursor = 0; \
		__ECS_QUERY_ADVANCE_ARCH(&(iter), &(ecs)); \
	} \
	if (! ECS_QUERY_DONE(iter)) \
		(iter).entity_id = ECS_VEC_GET((iter).current_arch->ids, (iter).row_cursor, u32); \
} while (0)

// Get component from current iteration position
# define ECS_QUERY_GET(iter, comp_bit, type) \
	ECS_ARCH_GET_FAST(*(iter).current_arch, comp_bit, (iter).row_cursor, type)

// Get component pointer from current iteration position
# define ECS_QUERY_GET_PTR(iter, comp_bit) \
	ECS_ARCH_GET(*(iter).current_arch, comp_bit, (iter).row_cursor)

// ╔══════════════════════[ CONVENIENCE MACROS ]════════════════════╗

// Iterate over all entities matching query
// Usage: ECS_QUERY_EACH(ecs, query, { pos->x += vel->x; })
# define ECS_QUERY_EACH(ecs, query, ...) do { \
	ECS_QUERY_REFRESH(ecs, query); \
	for (u32 _qi = 0; _qi < (query).arch_indices.len; _qi++) { \
		u32 _arch_idx = ECS_VEC_GET((query).arch_indices, _qi, u32); \
		t_ecs_archetype *_arch = __ECS_ARCH_PTR(ecs, _arch_idx);\
		for (u32 _row = 0; _row < _arch->count; _row++) { \
			UNUSED u32 entity = ECS_VEC_GET(_arch->ids, _row, u32); \
			{__VA_ARGS__} \
		} \
	} \
} while (0)

// Get typed component in QUERY_EACH loop
# define QGET(comp_bit, type) \
	ECS_ARCH_GET_FAST(*_arch, comp_bit, _row, type)

// Get typed component pointer in QUERY_EACH loop
# define QGET_PTR(comp_bit, type) \
	((type *)ECS_ARCH_GET(*_arch, comp_bit, _row))

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_QUERY_ADVANCE_ARCH(iter_ptr, ecs_ptr) do { \
	while ((iter_ptr)->arch_cursor < (iter_ptr)->query->arch_indices.len) { \
		u32 _idx = ECS_VEC_GET((iter_ptr)->query->arch_indices, (iter_ptr)->arch_cursor, u32); \
		(iter_ptr)->current_arch = &ECS_VEC_GET((ecs_ptr)->archetypes, _idx, t_ecs_archetype); \
		if ((iter_ptr)->current_arch->count > 0) { \
			(iter_ptr)->entity_id = ECS_VEC_GET((iter_ptr)->current_arch->ids, 0, u32); \
			break; \
		} \
		(iter_ptr)->arch_cursor++; \
	} \
} while (0)

# define __ECS_QUERY_LOG(query) do { \
	LOG_DEBUG("Query: inc=0x%llx, exc=0x%llx, archetypes=%lu", \
		(unsigned long long)(query).include, \
		(unsigned long long)(query).exclude, \
		(query).arch_indices.len); \
} while (0)
