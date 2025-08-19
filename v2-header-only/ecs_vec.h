#pragma once

# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>
# include <string.h>
# include <assert.h>

# include "ecs_utils.h"

# ifndef ECS_VEC_MAXCAP
#  define ECS_VEC_MAXCAP 0x2000000000
# endif

// ╔═════════════════════════[ DEFINITION ]═════════════════════════╗

typedef struct s_ecs_vec {
	void *data;
	u64 cap;
	u64 len;
	u64 mem_size;
} t_ecs_vec;


// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

// ----- UTILS -----

#define ECS_VEC_DEFAULT (t_ecs_vec){.data=NULL,.cap=0,.len=0,.mem_size=0}
#define ECS_VEC_IS_INIT(vec)	__builtin_expect((vec).data != NULL, 1)
#define ECS_VEC_NOT_INIT(vec)	__builtin_expect((vec).data == NULL, 0)
#define ECS_VEC_EMPTY(vec)		__builtin_expect((vec).len == 0, 0)
#define ECS_VEC_FULL(vec)		__builtin_expect((vec).len >= (vec).cap, 0)
#define ECS_VEC_NOT_EMPTY(vec)	__builtin_expect((vec).len > 0, 1)
#define ECS_VEC_IN(vec, index)	__builtin_expect((u64)(index) < (vec).len, 1)

// ----- INITIALIZATION -----

#define ECS_VEC_INIT(vec, type, capacity) ({                                   \
	__VEC_ASSERT_NOT_INIT(vec);                                                \
	(vec) = (t_ecs_vec){.cap = capacity,                                       \
		.mem_size = sizeof(type), .data = 0, .len = 0};                        \
	(vec).data = calloc(sizeof(type), (capacity));                             \
	(vec).data != NULL;                                                        \
})

#define ECS_VEC_DESTROY(vec) do {                                              \
	free((vec).data);                                                          \
	(vec) = ECS_VEC_DEFAULT;                                                   \
} while (0)

// ----- ACCESSORS -----

#define ECS_VEC_TOP(vec, cast) ({                                              \
	__VEC_ASSERT_INIT(vec);                                                    \
	__VEC_ASSERT_NOT_EMPTY(vec);                                               \
	*(cast *)(__ECS_VEC_PTR(vec, (vec).len - 1));                              \
})

#define ECS_VEC_GET(vec, index, cast) ({                                       \
	__VEC_ASSERT_INIT(vec);                                                    \
	__VEC_ASSERT_IN(vec, index);                                               \
	*(cast *)(__ECS_VEC_PTR(vec, index));                                      \
})

// ----- MODIFIERS -----

#define ECS_VEC_CLEAR(vec) (vec).len = 0

#define ECS_VEC_SET(vec, index, value) do {                                    \
	__VEC_ASSERT_INIT(vec);                                                    \
	__VEC_ASSERT_IN(vec, index);                                               \
	memcpy(__ECS_VEC_PTR(vec, index), (value), (vec).mem_size);                \
} while (0)

#define ECS_VEC_PUSH(vec, value) do {                                          \
	__VEC_ASSERT_INIT(vec);                                                    \
	__ECS_VEC_REALLOC(vec);                                                    \
	memcpy(__ECS_VEC_PTR(vec, (vec).len++), &(value), (vec).mem_size);         \
} while (0)

#define ECS_VEC_POP(vec, ...) do {                                             \
	__ECS_VEC_DELONE(vec, (vec).len-1, __VA_ARGS__)                            \
} while (0)

#define ECS_VEC_REMOVE(vec, index, ...) do {                                   \
	__ECS_VEC_DELONE(vec, index, __VA_ARGS__)                                  \
} while (0)

// ----- ITERATORS -----

#define ECS_VEC_FOREACH(vec, type, ...) do {                                   \
    type *__ptr = (type *)(vec).data;                                          \
    type *const __end = __ptr + (vec).len;                                     \
    for (; __ptr < __end; ++__ptr) {                                           \
        type *const _item = __ptr;                                             \
        {__VA_ARGS__}                                                          \
    }                                                                          \
} while (0)

#define ECS_VEC_ITER(vec, type, ...) do {                                      \
	type *const __data = (type *)(vec).data;                                   \
	for (u64 it = 0; it < (vec).len; ++it) {                                   \
		type _item = *(__data + it);                                           \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

#define ECS_VEC_ITER_ALL(vec, type, ...) do {                                  \
	type *const __data = (type *)(vec).data;                                   \
	for (u64 it = 0; it < (vec).cap; ++it) {                                   \
		type _item = *(__data + it);                                           \
		{__VA_ARGS__}                                                          \
	}                                                                          \
} while (0)

#define ECS_VEC_CLEAR_ALL(vec, type, ...) do {                                 \
	ECS_VEC_FOREACH(vec, type, __VA_ARGS__);                                   \
	ECS_VEC_CLEAR(vec);                                                        \
} while (0)


// ╔═══════════════════════════[ INTERNALS ]═══════════════════════════╗

# define __ECS_VEC_PTR(vec, i) ((vec).data + (i) * (vec).mem_size)

# define __ECS_VEC_REALLOC(vec)                                                \
	__VEC_ASSERT_NO_OVERFLOW(vec);                                             \
	if (ECS_VEC_FULL(vec)) {                                                   \
		u64 new_cap = (vec).cap << 1;                                          \
		(vec).cap = (new_cap >> 1) != (vec).cap ? UINT64_MAX : new_cap;        \
		(vec).data = realloc((vec).data, (vec).cap * (vec).mem_size);          \
	} __VEC_ASSERT_REALLOC(vec);

# define __ECS_VEC_DELONE(vec, index, ...)                                     \
	u64 _index = (u64)(index);                                                 \
	__VEC_ASSERT_INIT(vec);                                                    \
	__VEC_ASSERT_IN(vec, _index);                                              \
	void *const _item = __ECS_VEC_PTR(vec, _index);                            \
	--(vec).len;                                                               \
	{__VA_ARGS__}                                                              \
	if (ECS_VEC_IN(vec, _index))                                               \
		memcpy(_item, __ECS_VEC_PTR(vec, (vec).len), (vec).mem_size);


# define __VEC_ASSERT_INIT(vec)	                                               \
	ASSERT_MSG(ECS_VEC_IS_INIT(vec), "%s", "Vector not initialized")
# define __VEC_ASSERT_NOT_INIT(vec)	                                           \
	ASSERT_MSG(ECS_VEC_NOT_INIT(vec), "%s", "Vector already initialized")
# define __VEC_ASSERT_NOT_EMPTY(vec)                                           \
	ASSERT_MSG(ECS_VEC_NOT_EMPTY(vec), "%s", "Vector is empty")
# define __VEC_ASSERT_IN(vec, index)                                           \
	ASSERT_MSG(ECS_VEC_IN(vec, index), "%s", "Vector Index out of bounds")
# define __VEC_ASSERT_NO_OVERFLOW(vec)                                         \
	ASSERT_MSG((vec).cap < ECS_VEC_MAXCAP, "%s", "Vector capacity overflow")
# define __VEC_ASSERT_REALLOC(vec)                                             \
	ASSERT_MSG(ECS_VEC_IS_INIT(vec), "%s", "Vector reallocation failed")
