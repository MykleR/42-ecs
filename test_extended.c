#include <ecs.h>

int main() {
	t_ecs ecs = ECS_DEFAULT;
	ECS_INIT(ecs, u32, u64, float);

	LOG_INFO("ECS initialized");
	__ECS_LOG(ecs);

	// Create some entities
	u64 entity1 = ECS_ENTITY_CREATE(ecs);
	u64 entity2 = ECS_ENTITY_CREATE(ecs);
	u64 entity3 = ECS_ENTITY_CREATE(ecs);

	LOG_INFO("Created 3 entities: %lu, %lu, %lu", entity1, entity2, entity3);
	__ECS_LOG(ecs);

	// Check if entities are alive
	LOG_INFO("Entity 1 alive: %s", ECS_ENTITY_ALIVE(entity1) ? "yes" : "no");
	LOG_INFO("Entity 2 alive: %s", ECS_ENTITY_ALIVE(entity2) ? "yes" : "no");
	LOG_INFO("Entity 3 alive: %s", ECS_ENTITY_ALIVE(entity3) ? "yes" : "no");

	// Kill an entity
	ECS_ENTITY_KILL(ecs, entity2);
	LOG_INFO("Killed entity 2");
	__ECS_LOG(ecs);

	// Create another entity (should reuse the killed one's ID)
	u64 entity4 = ECS_ENTITY_CREATE(ecs);
	LOG_INFO("Created entity 4: %lu", entity4);
	__ECS_LOG(ecs);

	ECS_DESTROY(ecs);
	LOG_OK("ECS destroyed");
	return 0;
}