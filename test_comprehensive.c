#include <ecs.h>

typedef struct {
	float x, y;
} Position;

typedef struct {
	float dx, dy;
} Velocity;

typedef struct {
	u32 health;
} Health;

int main() {
	t_ecs ecs = ECS_DEFAULT;
	ECS_INIT(ecs, Position, Velocity, Health);

	LOG_INFO("ECS initialized with Position(0), Velocity(1), Health(2)");
	__ECS_LOG(ecs);

	// Create entities
	u64 entity1 = ECS_ENTITY_CREATE(ecs);
	u64 entity2 = ECS_ENTITY_CREATE(ecs);
	u64 entity3 = ECS_ENTITY_CREATE(ecs);

	LOG_INFO("Created 3 entities: %lu, %lu, %lu", entity1, entity2, entity3);

	// Add components to entities
	Position pos1 = {10.0f, 20.0f};
	Velocity vel1 = {1.0f, 2.0f};
	Health health1 = {100};

	LOG_INFO("Adding Position component to entity %lu", entity1);
	ECS_ENTITY_ADD(ecs, entity1, 0, &pos1);
	
	LOG_INFO("Adding Velocity component to entity %lu", entity1);
	ECS_ENTITY_ADD(ecs, entity1, 1, &vel1);
	
	LOG_INFO("Adding Health component to entity %lu", entity1);
	ECS_ENTITY_ADD(ecs, entity1, 2, &health1);

	Position pos2 = {30.0f, 40.0f};
	LOG_INFO("Adding Position component to entity %lu", entity2);
	ECS_ENTITY_ADD(ecs, entity2, 0, &pos2);

	__ECS_LOG(ecs);

	// Test querying
	LOG_INFO("Querying entities with Position component (bit 0)");
	t_ecs_query query_pos = ECS_QUERY(ecs, 0);
	LOG_INFO("Found %lu entities with Position", query_pos.count);
	for (u64 i = 0; i < query_pos.count; i++) {
		u32 entity_id = ECS_VEC_GET(query_pos.entities, i, u32);
		LOG_INFO("  Entity %lu has Position", (u64)entity_id);
	}

	LOG_INFO("Querying entities with Position AND Velocity (bits 0,1)");
	t_ecs_query query_pos_vel = ECS_QUERY(ecs, 0, 1);
	LOG_INFO("Found %lu entities with Position+Velocity", query_pos_vel.count);
	for (u64 i = 0; i < query_pos_vel.count; i++) {
		u32 entity_id = ECS_VEC_GET(query_pos_vel.entities, i, u32);
		LOG_INFO("  Entity %lu has Position+Velocity", (u64)entity_id);
	}

	// Test getting component data
	LOG_INFO("Getting Position data from entity %lu", entity1);
	Position *retrieved_pos = (Position*)ECS_ENTITY_GET(ecs, entity1, 0);
	if (retrieved_pos) {
		LOG_INFO("  Position: (%.1f, %.1f)", retrieved_pos->x, retrieved_pos->y);
	} else {
		LOG_WARN("  Could not retrieve Position for entity %lu", entity1);
	}

	// Test removing component
	LOG_INFO("Removing Velocity from entity %lu", entity1);
	ECS_ENTITY_REMOVE(ecs, entity1, 1);

	LOG_INFO("Querying entities with Position AND Velocity after removal");
	ECS_QUERY_DESTROY(query_pos_vel);
	query_pos_vel = ECS_QUERY(ecs, 0, 1);
	LOG_INFO("Found %lu entities with Position+Velocity", query_pos_vel.count);

	// Cleanup
	ECS_QUERY_DESTROY(query_pos);
	ECS_QUERY_DESTROY(query_pos_vel);
	ECS_DESTROY(ecs);
	LOG_OK("ECS destroyed successfully");
	return 0;
}