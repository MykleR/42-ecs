/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 16:10:25 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef DNDEBUG
# undef ecs_entity_create
# undef ecs_entity_get
# undef ecs_entity_has

uint64_t	__assert_entity_create(t_universe *ecs)
{
	assert(ecs);
	return (ecs_entity_create(ecs));
}

void	*__assert_entity_get(t_universe *ecs, uint64_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->components.nb_comps);
	return (ecs_entity_get(ecs, id, comp));
}

bool	__assert_entity_has(t_universe *ecs, uint64_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->components.nb_comps);
	return (ecs_entity_has(ecs, id, comp));
}
#endif

uint64_t	ecs_entity_create(t_universe *ecs)
{
	t_entity	*result;

	result = ecs->entities + ecs->len_entities;
	result->alive = 1;
	result->mask = 0;
	return (ecs->len_entities++);
}

bool	ecs_entity_has(t_universe *ecs, uint64_t id, uint8_t comp)
{
	t_entity	ent;

	ent = *(ecs->entities + id);
	return (ent.alive && (ent.mask & (1 << comp)) != 0);
}

void	*ecs_entity_get(t_universe *ecs, uint64_t id, uint8_t comp)
{
	size_t	offset;
	size_t	size;

	offset = ecs->components.offsets[comp];
	size = ecs->components.total_size;
	return ((uint8_t *)ecs->components.data + (id * size + offset));
}
