#pragma once

# include <stdio.h>
# include <stdint.h>

typedef uint64_t		u64;
typedef uint_fast32_t	u32;
typedef uint_fast16_t	u16;
typedef uint_fast8_t	u8;
typedef int64_t			i64;
typedef int_fast32_t	i32;
typedef int_fast16_t	i16;
typedef int_fast8_t		i8;

# define UNUSED __attribute__((unused))

# define COMMA ,

# define	BOLD(X)				"\033[1m" X "\033[22m"
# define 	ITALIC(X)			"\033[3m" X "\033[23m"
# define	RESET				"\033[0m"
# define	RED					"\033[31m"
# define	GREEN				"\033[32m"
# define	YELLOW				"\033[33m"
# define	BLUE				"\033[34m"
# define	MAGENTA				"\033[35m"
# define	CYAN				"\033[36m"
# define	GRAY				"\033[90m"
# define 	COLOR(C, X)			C X RESET

# ifndef NVERBOSE
#  define __LOG(c, t, msg, ...)	\
	fprintf(stderr, BOLD(COLOR(c,"%8s")) " > " msg "\n", t, ##__VA_ARGS__)
# else
#  define __LOG(c, t, msg, ...)
# endif

# define LOG_DEBUG(msg, ...)		__LOG(BLUE,		"debug: ", msg, ##__VA_ARGS__)
# define LOG_INFO(msg, ...)			__LOG(CYAN,       "info: ",msg, ##__VA_ARGS__)
# define LOG_WARN(msg, ...)			__LOG(YELLOW, "warning: ", msg, ##__VA_ARGS__)
# define LOG_OK(msg, ...)			__LOG(GREEN,	 "done: ", msg, ##__VA_ARGS__)
# define LOG_ERR(msg, ...)			__LOG(RED,		"error: ", msg, ##__VA_ARGS__)
# define LOG_FATAL(ret, msg, ...) ({__LOG(RED,		"fatal: ", msg, ##__VA_ARGS__);\
		return ret;})


# ifndef NDEBUG
#  define ASSERT_MSG(cond, msg, ...) for (; !(cond); assert(cond)) { \
	LOG_ERR("Assertion failed: " msg, ##__VA_ARGS__);}
# else
#  define ASSERT_MSG(cond, msg, ...) ((void)0)
# endif
