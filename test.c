#include "ecs_log.h"
#include <ecs.h>
#include <stdio.h>
#include <string.h>

typedef struct s_vec2 {
	float x;
	float y;
} t_vec2;

int main()
{
	t_ecs_vec vec;
	ECS_VEC_INIT(vec, char *, 10);

	for (int i = 0; i < 10; i++) {
		ECS_VEC_PUSH(vec, &(char*){strdup((char[2]){i + '0', '\0'})});
	}
	if (ECS_VEC_EMPTY(vec)) LOG_ERR("Vector is empty after init.");
	if (ECS_VEC_NOT_EMPTY(vec)) LOG_OK("Vector is not empty after init.");

	ECS_VEC_REMOVE(vec, 0, { free(*(char **)_item); });
	ECS_VEC_REMOVE(vec, 1, { free(*(char **)_item); });
	ECS_VEC_ITER(vec, LOG_INFO("%s",*(char **)_item); );

	ECS_VEC_CLEAR_ALL(vec, { free(*(char **)_item); });
	if (ECS_VEC_EMPTY(vec)) LOG_OK("Vector is empty after clear.");
	if (ECS_VEC_NOT_EMPTY(vec)) LOG_ERR("Vector is not empty after clear.");

	ECS_VEC_DESTROY(vec);

	t_ecs_archetype arch = ECS_ARCH_DEFAULT;
	ECS_ARCH_INIT(arch, 0b1, t_vec2);
	ECS_ARCH_SHOW(arch);
}
