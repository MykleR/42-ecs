#include <ecs.h>

int main(void)
{
	t_ecs ecs;
	ECS_INIT(ecs, int, float, char);

	// Example usage of ECS macros
	__ECS_LOG(ecs);

	// Clean up
	ECS_DESTROY(ecs);

	return 0;
}
