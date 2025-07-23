
#pragma once

# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <assert.h>

# ifndef ECS_VEC_MAXCAP
#  define ECS_VEC_MAXCAP 0x2000000000
# endif


// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef uint64_t		u64;
typedef uint_fast32_t	u32;
typedef uint_fast16_t	u16;
typedef uint_fast8_t	u8;
typedef int64_t			i64;
typedef int_fast32_t	i32;
typedef int_fast16_t	i16;
typedef int_fast8_t		i8;

typedef struct s_ecs_vec {
	void *data;
	u64 cap;
	u64 len;
	u64 mem_size;
} t_ecs_vec;


// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

// ----- UTILS -----

#define ECS_VEC_DEFAULT (t_ecs_vec) \
	{.data = NULL, .cap = 0, .len = 0, .mem_size = 0}

#define ECS_VEC_CLEAR(vec) ((vec).len = 0)

#define ECS_VEC_EMPTY(vec) \
	__builtin_expect(ecs_vec_out(&(vec), 0), 0)

#define ECS_VEC_NOT_EMPTY(vec) \
	__builtin_expect(ecs_vec_in(&(vec), 0), 1)

#define ECS_VEC_INIT(vec, type, capacity) \
	ecs_vec_init(&(vec), (capacity), sizeof(type))

#define ECS_VEC_DESTROY(vec) \
	ecs_vec_destroy(&(vec))

// ----- ACCESSORS -----

#define ECS_VEC_GET(vec, index, cast) \
	(*(cast *)ecs_vec_get(&(vec), (index)))

#define ECS_VEC_TOP(vec, cast) \
	ECS_VEC_GET((vec), (vec).len - 1, cast)

// ----- MODIFIERS -----

#define ECS_VEC_SET(vec, index, value) \
	ecs_vec_set(&(vec), (index), &(value))

#define ECS_VEC_PUSH(vec, value) \
	ecs_vec_push(&(vec), &(value))

#define ECS_VEC_POP(vec, ...) do { \
	__ECS_VEC_DELONE((vec), (vec).len - 1, __VA_ARGS__) \
} while (0)

#define ECS_VEC_REMOVE(vec, index, ...) do { \
	__ECS_VEC_DELONE((vec), (index), __VA_ARGS__) \
} while (0)

// ----- ITERATORS -----

#define ECS_VEC_FOREACH(vec, type, ...) do { \
	for (u64 it = 0; it < (vec).len; ++it) { \
		type *const _item = (type *)ecs_vec_ptr(&(vec), it); \
		{__VA_ARGS__} \
	} \
} while (0)

#define ECS_VEC_ITER(vec, type, ...) do { \
	for (u64 it = 0; it < (vec).len; ++it) { \
		type _item = *(type *)ecs_vec_ptr(&(vec), it); \
		{__VA_ARGS__} \
	} \
} while (0)

#define ECS_VEC_CLEAR_ALL(vec, type, ...) do { \
	ECS_VEC_FOREACH((vec), type, __VA_ARGS__); \
	ECS_VEC_CLEAR((vec)); \
} while (0)

// ----- INTERNALS -----

# define __ECS_VEC_DELONE(vec, index, ...) \
	if (__builtin_expect(ecs_vec_out(&(vec), index), 0)) break; \
	void *const _item = ecs_vec_ptr(&(vec), index); \
	--(vec).len; \
	{__VA_ARGS__} \
	if (__builtin_expect(ecs_vec_in(&(vec), index), 1)) \
		memcpy(_item, ecs_vec_ptr(&(vec), (vec).len), (vec).mem_size); \

# define UNUSED_V(x) (void)(x)
# define UNUSED_A __attribute__((unused))
# define __ASSERT_MSG(cond, msg) assert(cond && msg);

// ╔══════════════════════════[ INTERNALS ]══════════════════════════╗

static inline void *ecs_vec_ptr(const t_ecs_vec *vec, u64 index) {
	return vec->data + index * vec->mem_size;
}
static inline bool ecs_vec_isinit(const t_ecs_vec *vec) {
	return vec->data != NULL;
}
static inline bool ecs_vec_out(const t_ecs_vec *vec, u64 index) {
	return index >= vec->len;
}
static inline bool ecs_vec_in(const t_ecs_vec *vec, u64 index) {
	return index < vec->len;
}

static inline bool ecs_vec_init(t_ecs_vec *vec, u64 cap, u64 mem) {
	*vec = (t_ecs_vec){.data=0, .len=0, .cap=cap, .mem_size=mem};
	vec->data = calloc(cap, mem);
	return vec->data != NULL;
}

static inline void ecs_vec_destroy(t_ecs_vec *vec) {
	free(vec->data);
	*vec = ECS_VEC_DEFAULT;
}

static inline void *ecs_vec_get(const t_ecs_vec *vec, u64 index) {
	__ASSERT_MSG(ecs_vec_isinit(vec), 
			"ecs_vec_get: Vector not initialized");
	__ASSERT_MSG(ecs_vec_in(vec, index),
			"ecs_vec_get: Index out of bounds");
	return ecs_vec_ptr(vec, index);
}

static inline void ecs_vec_set(t_ecs_vec *vec, u64 index, const void *value) {
	__ASSERT_MSG(ecs_vec_isinit(vec),
			"ecs_vec_set: Vector not initialized");
	__ASSERT_MSG(ecs_vec_in(vec, index),
			"ecs_vec_set: Index out of bounds");
	memcpy(ecs_vec_ptr(vec, index), value, vec->mem_size);
}

static inline void ecs_vec_resize(t_ecs_vec *vec) {
	__ASSERT_MSG(vec->cap < ECS_VEC_MAXCAP,
			"ecs_vec_resize: Vector capacity exceeded");
	if (__builtin_expect(vec->len >= vec->cap, 0)) {
		u64 new_cap = vec->cap << 1;
		vec->cap = (new_cap >> 1) != vec->cap ? UINT64_MAX : new_cap;
		vec->data = realloc(vec->data, vec->cap * vec->mem_size);
	}
	__ASSERT_MSG(ecs_vec_isinit(vec),
			"ecs_vec_resize: Vector resize failed");
}

static inline void ecs_vec_push(t_ecs_vec *vec, const void *value) {	
	__ASSERT_MSG(ecs_vec_isinit(vec),
			"ecs_vec_push: Vector not initialized");
	ecs_vec_resize(vec);
	memcpy(ecs_vec_ptr(vec, vec->len++), value, vec->mem_size);
}
