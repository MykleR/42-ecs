#include "ecs.h"

// Define component bits
#define COMP_POSITION	0
#define COMP_VELOCITY	1
#define COMP_HEALTH		2
#define COMP_DISABLED	3

typedef struct { float x, y; } Position;
typedef struct { float dx, dy; } Velocity;
typedef struct { int current, max; } Health;

void f(t_ecs *ecs, u32 entity, Position pos, Velocity vel)
{
	Health health = {100, 100};
	ECS_ADD(*ecs, entity, COMP_POSITION, &pos);
	ECS_ADD(*ecs, entity, COMP_VELOCITY, &vel);
}

int main(void)
{
	t_ecs ecs = {0};
	t_ecs_query move_query = {0};
	
	// Initialize ECS with component types
	ECS_INIT(ecs, Position, Velocity, Health, char);
	
	// Create query for entities with Position AND Velocity, but NOT Disabled
	ECS_QUERY_INIT(move_query, 
		(1ULL << COMP_POSITION) | (1ULL << COMP_VELOCITY),  // include
		(1ULL << COMP_DISABLED));                           // exclude
	
	// Create entities
	u32 player = ECS_ENTITY_CREATE(ecs);
	u32 npc = ECS_ENTITY_CREATE(ecs);
	Health npc_health = {50, 50};
	ECS_ADD(ecs, player, COMP_HEALTH, &npc_health);
	f(&ecs, player, (Position){5.0f, 0.0f}, (Velocity){1.0f, 1.0f});
	f(&ecs, npc, (Position){9.0f, 10.0f}, (Velocity){-1.0f, 0.5f});
	// Mark query as dirty after adding archetypes

	
	int x = 10;
	while (x--) {
		// Iterate and update (SIMD-friendly due to SoA layout)
		printf("---- Frame %d ----\n", 10 - x);
		ECS_QUERY_EACH(ecs, move_query, {
			Position *p = QGET_PTR(COMP_POSITION, Position);
			Velocity v = QGET(COMP_VELOCITY, Velocity);
			printf("Entity before move: Position(%.2f, %.2f)\n", p->x, p->y);
			p->x += v.dx;
			p->y += v.dy;
			if (p->x < 5.0f)
				ECS_ENTITY_KILL(ecs, entity); // Just to test dynamic removal
			printf("Entity after move: Position(%.2f, %.2f)\n", p->x, p->y);
		});
	}
	
	// Cleanup
	ECS_QUERY_DESTROY(move_query);
	ECS_DESTROY(ecs);
	return 0;
}
