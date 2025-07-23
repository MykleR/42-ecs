#include <ecs_vec.h>
#include <ecs_log.h>

#define N 9900000

int main(void)
{
	t_ecs_vec vec = ECS_VEC_DEFAULT;
	if (!ECS_VEC_INIT(vec, int, 4))
		LOG_FATAL(1, "Failed to initialize vector");
	else LOG_OK("Vector initialized successfully");
	
	for (int i = 0; i < N; ++i)
		ECS_VEC_PUSH(vec, (int){i * 2});
	LOG_INFO("Vector contents:");
	ECS_VEC_ITER(vec, int, LOG_DEBUG("Item %lu: %d", it, _item););

	LOG_INFO("Removing item at index 2...");
	ECS_VEC_REMOVE(vec, 2);
	ECS_VEC_ITER(vec, int, LOG_DEBUG("Item %lu: %d", it, _item););

	if (ECS_VEC_NOT_EMPTY(vec))
		LOG_OK("Vector is not empty");
	else LOG_ERR("Vector should not be empty");

	LOG_INFO("Popping last item...");
	ECS_VEC_POP(vec);

	for (int i = 0; i < N - 2; ++i)
		ECS_VEC_REMOVE(vec, 0);

	if (ECS_VEC_EMPTY(vec))
		LOG_OK("Vector is empty");
	else LOG_ERR("Vector should be empty");

	ECS_VEC_DESTROY(vec);
	return 0;
}
