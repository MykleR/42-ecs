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

// ----- ACCESSORS -----

#define ECS_VEC_DEFAULT (t_ecs_vec)                                            \
	{.data = NULL, .cap = 0, .len = 0, .mem_size = 0}

#define ECS_VEC_CAST(vec, cast) ((cast *)((vec).data))

#define ECS_VEC_EMPTY(vec)                                                     \
	__ECS_VEC_NEXPECT_OUT(vec, 0)

#define ECS_VEC_NOT_EMPTY(vec)                                                 \
	__ECS_VEC_EXPECT_IN(vec, 0)

#define ECS_VEC_TOP(vec, cast) ({ \
	assert(ECS_VEC_NOT_EMPTY(vec));                                            \
	cast _val = *(cast *)(__ECS_VEC_PTR(vec, (vec).len - 1));                  \
	_val;                                                                      \
})

#define ECS_VEC_GET(vec, index, cast) ({                                       \
	assert(__ECS_VEC_EXPECT_IN(vec, index));                                   \
	cast _val = *(cast *)(__ECS_VEC_PTR(vec, index));                          \
	_val;                                                                      \
})

#define ECS_VEC_SET(vec, index, value) do {                                    \
	if (__ECS_VEC_EXPECT_IN(vec, index))                                       \
		memcpy(__ECS_VEC_PTR(vec, index), (value), (vec).mem_size);            \
} while (0)

// ----- MODIFIERS -----

#define ECS_VEC_INIT(vec, type, capacity) do {                                 \
	(vec) = ECS_VEC_DEFAULT;												   \
	(vec).data = calloc(sizeof(type), (capacity));                             \
	__ECS_VEC_CHECKINIT(vec);                                                  \
	(vec).cap = (capacity);                                                    \
	(vec).mem_size = sizeof(type);                                             \
} while (0)

#define ECS_VEC_DESTROY(vec) do {                                              \
	free((vec).data);                                                          \
	(vec).data = NULL;                                                         \
	(vec) = ECS_VEC_DEFAULT;                                                   \
} while (0)

#define ECS_VEC_PUSH(vec, value) do {                                          \
	__ECS_VEC_REALLOC(vec);                                                    \
	__ECS_VEC_CHECKINIT(vec);                                                  \
	memcpy(__ECS_VEC_PTR(vec, (vec).len++), (value), (vec).mem_size);          \
} while (0)

#define ECS_VEC_POP(vec) do {                                                  \
	if (ECS_VEC_NOT_EMPTY(vec))                                                \
		(vec).len--;                                                           \
} while (0)

#define ECS_VEC_REMOVE(vec, index, ...) do {                                   \
	if (__ECS_VEC_NEXPECT_OUT((vec), (index))) break;                          \
	void *const _item = __ECS_VEC_PTR(vec, index);                             \
    {__VA_ARGS__}                                                              \
    --(vec).len;                                                               \
	if (__ECS_VEC_EXPECT_IN(vec, index))                                       \
		memcpy(_item, __ECS_VEC_PTR(vec, (vec).len), (vec).mem_size);          \
} while (0)

#define ECS_VEC_CLEAR(vec) do {                                                \
	(vec).len = 0;                                                             \
} while (0)

#define ECS_VEC_ITER(vec, ...) do {                                            \
	for (u64 it = 0; it < (vec).len; ++it) {                                   \
		 void *const _item = __ECS_VEC_PTR((vec), it);                         \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

# define ECS_VEC_FOREACH(vec, type, ...) do {                                  \
	for (u64 it = 0; it < (vec).len; ++it) {                                   \
		type *const _item = (type *)__ECS_VEC_PTR((vec), it);                  \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

#define ECS_VEC_CLEAR_ALL(vec, ...) do {                                       \
	ECS_VEC_ITER(vec, __VA_ARGS__);                                            \
	ECS_VEC_CLEAR(vec);                                                        \
} while (0)


// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define UNUSED_V(x) (void)(x)
# define UNUSED_A __attribute__((unused))

# define __ECS_VEC_NEXPECT_OUT(vec, index)                                    \
	(__builtin_expect((index) >= (vec).len, 0))

# define __ECS_VEC_EXPECT_IN(vec, index)                                      \
	(__builtin_expect((index) < (vec).len, 1))

# define __ECS_VEC_NINIT(vec)                                                 \
	(__builtin_expect((vec).data == NULL, 0))

# define __ECS_VEC_PTR(vec, index)                                            \
	((void *const)((vec).data + ((index) * (vec).mem_size)))

# define __ECS_VEC_CHECKINIT(vec)                                             \
	if (__ECS_VEC_NINIT(vec)) break;

# define __ECS_VEC_MULT(x)                                                    \
	((u64)((x) << 1) >> 1 != (x)) ? UINT64_MAX : (x) << 1

# define __ECS_VEC_REALLOC(vec)                                               \
	if (__builtin_expect((vec).cap >= ECS_VEC_MAXCAP, 0))                     \
		break;                                                                \
	if (__builtin_expect((vec).len >= (vec).cap, 0)) {                        \
		(vec).cap = __ECS_VEC_MULT((vec).cap);                                \
		void *new_data = realloc((vec).data, (vec).cap * (vec).mem_size);     \
		(vec).data = new_data;                                                \
	}                                                                         \
