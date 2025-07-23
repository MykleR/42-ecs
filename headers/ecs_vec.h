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

# define UNUSED __attribute__((unused))

#define ECS_VEC_DEFAULT (t_ecs_vec) {.data=NULL,.cap=0,.len=0,.mem_size=0}
#define ECS_VEC_IS_INIT(vec)	__builtin_expect((vec).data != NULL, 1)
#define ECS_VEC_NOT_INIT(vec)	__builtin_expect((vec).data == NULL, 0)
#define ECS_VEC_EMPTY(vec)		__builtin_expect((vec).len == 0, 0)
#define ECS_VEC_FULL(vec)		__builtin_expect((vec).len >= (vec).cap, 0)
#define ECS_VEC_NOT_EMPTY(vec)	__builtin_expect((vec).len > 0, 1)
#define ECS_VEC_IN(vec, index)	__builtin_expect((u64)(index) < (vec).len, 1)
#define ECS_VEC_OUT(vec, index)	__builtin_expect((u64)(index) >= (vec).len, 0)

// ----- INITIALIZATION -----

#define ECS_VEC_INIT(vec, type, capacity) (                                    \
	__ASSERT_MSG(ECS_VEC_NOT_INIT(vec), "Vector already initialized"),         \
	(vec) = (t_ecs_vec){.cap=capacity,.mem_size=sizeof(type),.data=0,.len=0},  \
	(vec).data = calloc(sizeof(type), (capacity)),                             \
	(vec).data != NULL                                                         \
)

#define ECS_VEC_DESTROY(vec) do {                                              \
	free((vec).data);                                                          \
	(vec) = ECS_VEC_DEFAULT;                                                   \
} while (0)

// ----- ACCESSORS -----

#define ECS_VEC_TOP(vec, cast) (                                               \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector not initialized"),              \
	__ASSERT_MSG(ECS_VEC_NOT_EMPTY(vec), "Vector is empty"),                   \
	*(cast *)(__ECS_VEC_PTR(vec, (vec).len - 1))                               \
)

#define ECS_VEC_GET(vec, index, cast) (                                        \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector not initialized"),              \
	__ASSERT_MSG(ECS_VEC_IN(vec, index), "Vector Index out of bounds"),        \
	*(cast *)(__ECS_VEC_PTR(vec, index))                                       \
)

// ----- MODIFIERS -----

#define ECS_VEC_CLEAR(vec) ((vec).len = 0;)

#define ECS_VEC_SET(vec, index, value) do {                                    \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector not initialized");              \
	__ASSERT_MSG(ECS_VEC_IN(vec, index), "Vector Index out of bounds");        \
	memcpy(__ECS_VEC_PTR(vec, index), (value), (vec).mem_size);                \
} while (0)

#define ECS_VEC_PUSH(vec, value) do {                                          \
	__ECS_VEC_REALLOC(vec);                                                    \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector realloc failed");               \
	memcpy(__ECS_VEC_PTR(vec, (vec).len++), &(value), (vec).mem_size);         \
} while (0)

#define ECS_VEC_POP(vec, ...) do {                                             \
	__ECS_VEC_DELONE((vec), (vec).len, __VA_ARGS__)                            \
} while (0)

#define ECS_VEC_REMOVE(vec, index, ...) do {                                   \
	__ECS_VEC_DELONE((vec), (index), __VA_ARGS__)                              \
} while (0)

// ----- ITERATORS -----

#define ECS_VEC_FOREACH(vec, type, ...) do {                                   \
	for (u64 it = 0; it < (vec).len; ++it) {                                   \
		type *const _item = __ECS_VEC_PTR(vec, it);                            \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

#define ECS_VEC_ITER(vec, type, ...) do {                                      \
	for (u64 it = 0; it < (vec).len; ++it) {                                   \
		type _item = ECS_VEC_GET(vec, it, type);                               \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

#define ECS_VEC_CLEAR_ALL(vec, ...) do {                                       \
	ECS_VEC_ITER(vec, __VA_ARGS__);                                            \
	ECS_VEC_CLEAR(vec);                                                        \
} while (0)


// ╔═══════════════════════════[ INTERNALS ]═══════════════════════════╗

# define __ASSERT_MSG(cond, msg) assert(cond && msg)

# define __ECS_VEC_PTR(vec, index)                                             \
	((vec).data + (index) * (vec).mem_size)

# define __ECS_VEC_REALLOC(vec)                                                \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector not initialized");              \
	__ASSERT_MSG((vec).cap < ECS_VEC_MAXCAP, "Vector capacity overflow");	   \
	if (ECS_VEC_FULL(vec)) {                                                   \
		u64 new_cap = (vec).cap << 1;                                          \
		(vec).cap = (new_cap >> 1) != (vec).cap ? UINT64_MAX : new_cap;        \
		(vec).data = realloc((vec).data, (vec).cap * (vec).mem_size);          \
	}

# define __ECS_VEC_DELONE(vec, index, ...) \
	__ASSERT_MSG(ECS_VEC_IS_INIT(vec), "Vector not initialized");              \
	__ASSERT_MSG(ECS_VEC_NOT_EMPTY(vec), "Vector is empty");                   \
	void *const _item = __ECS_VEC_PTR(vec, index);                             \
	--(vec).len;                                                               \
	{__VA_ARGS__}                                                              \
	if (ECS_VEC_IN(vec, index))                                                \
		memcpy(_item, __ECS_VEC_PTR(vec, (vec).len), (vec).mem_size); 
