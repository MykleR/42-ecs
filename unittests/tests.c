/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tests.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mykle <mykle@42angouleme.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 23:16:35 by mykle             #+#    #+#             */
/*   Updated: 2025/01/08 00:02:23 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"
#include "utest.h"

typedef struct s_vector
{
	float	x;
	float	y;
}	t_vector;

#define NB_COMPS 2
#define POS 0
#define VEL 1

#define X 10000
#define Y 10000

static t_ecs	*ecs;

static uint32_t	id;

UTEST(ecs, create) {
	ecs = ecs_create(NB_COMPS, sizeof(t_vector), sizeof(t_vector));
	ASSERT_NE(ecs, NULL);
	EXPECT_TRUE(ecs->masks != NULL);
	EXPECT_EQ(ecs->nb_comps, NB_COMPS);
	EXPECT_EQ(ecs->entity_len, 0L);
	EXPECT_EQ(ecs->free_list, NULL);
	EXPECT_EQ(ecs->mem_tsize, 2 * sizeof(t_vector));
}

UTEST(ecs, query_empty)
{
	t_ecs_ulist	*query = ecs_query(ecs, (1ULL << POS));
	ASSERT_NE(query, NULL);
	EXPECT_EQ(query->len, 0L);
	EXPECT_EQ(query->cap, (uint32_t)ECS_ULIST_INIT_CAP);
}

UTEST(ecs, entity_create)
{
	id = ecs_entity_create(ecs);
	EXPECT_EQ(id, 0L);
	EXPECT_EQ(ecs->masks[id], ECS_MASK_ALIVE);
}

UTEST(ecs, entity_add)
{
	ecs_entity_add(ecs, id, POS, &(t_vector){X, Y});
	ASSERT_EQ(ecs->masks[id], (ECS_MASK_ALIVE | (1UL << POS)));
}

UTEST(ecs, entity_get)
{
	t_vector *pos = ecs_entity_get(ecs, id, POS);
	ASSERT_NE(pos, NULL);
	EXPECT_EQ(pos->x, X);
	EXPECT_EQ(pos->y, Y);
}

UTEST(ecs, query_pos)
{
	t_ecs_ulist	*query = ecs_query(ecs, (1ULL << POS));
	ASSERT_NE(query, NULL);
	ASSERT_NE(query->values, NULL);
	EXPECT_EQ(query->len, 1UL);
	EXPECT_EQ(query->values[0], 0UL);
}

UTEST(ecs, destroy)
{
	ecs_destroy(ecs);
	ASSERT_TRUE(1);
}

UTEST_MAIN();

