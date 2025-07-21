#include "ecs_event.h"
#include <ecs.h>

typedef struct s_vec2 {float x; float y; } t_vec2;
typedef struct s_vec3 {float x; float y; float z; } t_vec3;
ECS_EVENT(vec2, t_vec2)
ECS_EVENT(vec3, t_vec3)

void on_vec2_event(t_vec2* event_data, UNUSED_A void* user_data) {
	LOG_DEBUG(BOLD("Vec2")COLOR(BLUE," Event 1")" - x: %f, y: %f", event_data->x, event_data->y);
}

void on_vec2_event2(t_vec2* event_data, UNUSED_A void* user_data) {
	LOG_DEBUG(BOLD("Vec2")COLOR(GREEN," Event 2")" - x: %f, y: %f", event_data->x, event_data->y);
}

void on_vec3_event(t_vec3* event_data, UNUSED_A void* user_data) {
	LOG_DEBUG(BOLD("Vec3")COLOR(BLUE," Event 1")" - x: %f, y: %f, z: %f", event_data->x, event_data->y, event_data->z);
}

int main()
{	LOG_INFO("ECS Event System Test");
	if (event_sub_vec2(on_vec2_event, NULL) &&
		event_sub_vec2(on_vec2_event2, NULL))
		LOG_OK("Subscribed to Vec2 events");

	if (event_sub_vec3(on_vec3_event, NULL))
		LOG_OK("Subscribed to Vec3 events");

	LOG_INFO("Raising Vec2 and Vec3 events...");
	event_raise_vec2(&(t_vec2){1.0f, 2.0f});
	event_raise_vec3(&(t_vec3){3.0f, 4.0f, 5.0f});

	LOG_INFO("Unsubscribing Vec2 Event 2 and raising again...");
	event_unsub_vec2(on_vec2_event2);
	event_raise_vec2(&(t_vec2){6.0f, 7.0f});
}
