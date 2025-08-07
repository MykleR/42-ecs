#include <ecs.h>

typedef struct { float x, y; } Position;
typedef struct { float dx, dy; } Velocity;  
typedef struct { u32 health; } Health;

int main() {
	t_ecs ecs = ECS_DEFAULT;
	ECS_INIT(ecs, Position, Velocity, Health);

	LOG_OK("=== ECS Header-Only Functions Demo ===");
	
	// Create entities
	u64 player = ECS_ENTITY_CREATE(ecs);
	u64 enemy1 = ECS_ENTITY_CREATE(ecs);
	u64 enemy2 = ECS_ENTITY_CREATE(ecs);
	u64 bullet = ECS_ENTITY_CREATE(ecs);
	
	LOG_INFO("Created entities: player=%lu, enemy1=%lu, enemy2=%lu, bullet=%lu", 
		player, enemy1, enemy2, bullet);

	// Add components using ECS_ENTITY_ADD
	Position player_pos = {100.0f, 100.0f};
	Velocity player_vel = {0.0f, 0.0f};
	Health player_health = {100};
	
	ECS_ENTITY_ADD(ecs, player, 0, &player_pos);  // Position
	ECS_ENTITY_ADD(ecs, player, 1, &player_vel);  // Velocity
	ECS_ENTITY_ADD(ecs, player, 2, &player_health); // Health
	LOG_INFO("Player has Position, Velocity, and Health");

	Position enemy1_pos = {200.0f, 50.0f};
	Health enemy1_health = {50};
	ECS_ENTITY_ADD(ecs, enemy1, 0, &enemy1_pos);  // Position
	ECS_ENTITY_ADD(ecs, enemy1, 2, &enemy1_health); // Health
	LOG_INFO("Enemy1 has Position and Health");

	Position enemy2_pos = {150.0f, 200.0f};
	Velocity enemy2_vel = {-10.0f, 5.0f};
	ECS_ENTITY_ADD(ecs, enemy2, 0, &enemy2_pos);  // Position  
	ECS_ENTITY_ADD(ecs, enemy2, 1, &enemy2_vel);  // Velocity
	LOG_INFO("Enemy2 has Position and Velocity");

	Position bullet_pos = {110.0f, 90.0f};
	Velocity bullet_vel = {0.0f, -50.0f};
	ECS_ENTITY_ADD(ecs, bullet, 0, &bullet_pos);  // Position
	ECS_ENTITY_ADD(ecs, bullet, 1, &bullet_vel);  // Velocity
	LOG_INFO("Bullet has Position and Velocity");

	// Query entities with ECS_QUERY
	LOG_INFO("\n--- Query Results ---");
	
	t_ecs_query_result entities_with_pos = ECS_QUERY(ecs, 0);  // Position
	LOG_INFO("Entities with Position (%lu): ", entities_with_pos.count);
	for (u64 i = 0; i < entities_with_pos.count; i++) {
		u32 eid = ECS_VEC_GET(entities_with_pos.entities, i, u32);
		LOG_INFO("  Entity %u", eid);
	}

	t_ecs_query_result moving_entities = ECS_QUERY(ecs, 0, 1);  // Position + Velocity
	LOG_INFO("Moving entities (Position + Velocity) (%lu): ", moving_entities.count);
	for (u64 i = 0; i < moving_entities.count; i++) {
		u32 eid = ECS_VEC_GET(moving_entities.entities, i, u32);
		LOG_INFO("  Entity %u", eid);
	}

	t_ecs_query_result living_entities = ECS_QUERY(ecs, 2);  // Health
	LOG_INFO("Living entities (Health) (%lu): ", living_entities.count);
	for (u64 i = 0; i < living_entities.count; i++) {
		u32 eid = ECS_VEC_GET(living_entities.entities, i, u32);
		LOG_INFO("  Entity %u", eid);
	}

	// Test component checking with ECS_ENTITY_HAS
	LOG_INFO("\n--- Component Checks ---");
	LOG_INFO("Player has Position: %s", ECS_ENTITY_HAS(ecs, player, 0) ? "yes" : "no");
	LOG_INFO("Player has Velocity: %s", ECS_ENTITY_HAS(ecs, player, 1) ? "yes" : "no");
	LOG_INFO("Player has Health: %s", ECS_ENTITY_HAS(ecs, player, 2) ? "yes" : "no");
	LOG_INFO("Enemy1 has Velocity: %s", ECS_ENTITY_HAS(ecs, enemy1, 1) ? "yes" : "no");
	LOG_INFO("Bullet has Health: %s", ECS_ENTITY_HAS(ecs, bullet, 2) ? "yes" : "no");

	// Remove components with ECS_ENTITY_REMOVE
	LOG_INFO("\n--- Component Removal ---");
	LOG_INFO("Removing Health from player (player dies)");
	ECS_ENTITY_REMOVE(ecs, player, 2);
	LOG_INFO("Player has Health after removal: %s", ECS_ENTITY_HAS(ecs, player, 2) ? "yes" : "no");

	ECS_QUERY_DESTROY(living_entities);
	living_entities = ECS_QUERY(ecs, 2);  // Health
	LOG_INFO("Living entities after player death (%lu): ", living_entities.count);
	for (u64 i = 0; i < living_entities.count; i++) {
		u32 eid = ECS_VEC_GET(living_entities.entities, i, u32);
		LOG_INFO("  Entity %u", eid);
	}

	// Cleanup
	ECS_QUERY_DESTROY(entities_with_pos);
	ECS_QUERY_DESTROY(moving_entities);
	ECS_QUERY_DESTROY(living_entities);
	ECS_DESTROY(ecs);
	
	LOG_OK("=== All ECS header-only functions working perfectly! ===");
	return 0;
}