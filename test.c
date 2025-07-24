#include <ecs.h>

int main() {
	t_ecs ecs = ECS_DEFAULT;
	ECS_INIT(ecs, u32, u64, float);

	__ECS_LOG(ecs);

	ECS_DESTROY(ecs);
	return 0;
}
