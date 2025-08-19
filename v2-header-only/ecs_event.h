#pragma once

# include "ecs_vec.h"

// ╔═══════════════════════════[ MACROS ]═══════════════════════════╗

# define ECS_EVENT_DEFINE(name) \
	__ECS_EVENT_DEFINE_GET(name) \
	__ECS_EVENT_DEFINE_STOP(name) \
	__ECS_EVENT_DEFINE_SUB(name) \
	__ECS_EVENT_DEFINE_UNSUB(name) \
	__ECS_EVENT_DEFINE_RAISE(name)

# define ECS_EVENT_DECLARE(name, type) \
	__ECS_EVENT_DECLARE_TYPE(name, type) \
	__ECS_EVENT_DECLARE_FUNCTIONS(name)

# define ECS_EVENT(name, type) \
	ECS_EVENT_DECLARE(name, type) \
	ECS_EVENT_DEFINE(name)

// ╔═══════════════════════════[ UTILS ]════════════════════════════╗

# define __ECS_EVENT_DECLARE_TYPE(name, type) \
    enum { event_##name = __COUNTER__ }; \
    typedef type t_ecs_event_data_##name; \
    typedef void (*t_ecs_event_callback_##name)( \
		t_ecs_event_data_##name* event_data, void* user_data); \
	typedef struct { \
		t_ecs_event_callback_##name	callback; \
		void*						data; \
	} t_ecs_event_listener_##name; \
	typedef struct { \
		t_ecs_vec	listeners; \
		int			instancied; \
	} t_ecs_event_##name;

# define __ECS_EVENT_DECLARE_FUNCTIONS(name) \
	t_ecs_event_##name* event_get_##name(void); \
	void __event_stop_##name(void); \
	int event_sub_##name(t_ecs_event_callback_##name callback, void* user_data); \
	int event_unsub_##name(t_ecs_event_callback_##name callback); \
	void event_raise_##name(t_ecs_event_data_##name* data);

# define __ECS_EVENT_INIT(event, name) do { \
	ECS_VEC_INIT((event).listeners, t_ecs_event_listener_##name, 0x100); \
	(event).instancied = 1; \
} while (0)

# define __ECS_EVENT_DESTROY(event, name) do { \
	ECS_VEC_DESTROY((event).listeners); \
	(event) = (t_ecs_event_##name){0}; \
} while (0)

# define __ECS_EVENT_DEFINE_GET(name) \
	t_ecs_event_##name* event_get_##name(void) { \
		static t_ecs_event_##name event = {0}; \
		if (!event.instancied) __ECS_EVENT_INIT(event, name); \
		return &event; \
	}

# define __ECS_EVENT_DEFINE_STOP(name) \
	__attribute__((destructor)) \
	void __event_stop_##name(void) { \
		t_ecs_event_##name* event = event_get_##name(); \
		__ECS_EVENT_DESTROY(*event, name); \
	}

# define __ECS_EVENT_DEFINE_SUB(name) \
	int event_sub_##name(t_ecs_event_callback_##name callback, void* user_data) { \
		t_ecs_event_##name* event = event_get_##name(); \
		t_ecs_event_listener_##name listener = { \
			.callback = callback, \
			.data = user_data \
		}; \
		ECS_VEC_PUSH(event->listeners, listener); \
		return (1); \
	}

# define __ECS_EVENT_DEFINE_UNSUB(name) \
	int event_unsub_##name(t_ecs_event_callback_##name callback) { \
		t_ecs_event_##name* event = event_get_##name(); \
		for (u64 i = 0; i < event->listeners.len; ++i) { \
			t_ecs_event_listener_##name listener = ECS_VEC_GET(event->listeners, i, t_ecs_event_listener_##name); \
			if (listener.callback == callback) { \
				ECS_VEC_REMOVE(event->listeners, i); \
				return 1; \
			} \
		} \
		return 0; \
	}

# define __ECS_EVENT_DEFINE_RAISE(name) \
	void event_raise_##name(t_ecs_event_data_##name* data) { \
		t_ecs_event_##name* event = event_get_##name(); \
		ECS_VEC_ITER(event->listeners, t_ecs_event_listener_##name, { \
			_item.callback(data, _item.data); \
		}); \
	}
