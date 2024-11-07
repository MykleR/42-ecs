/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/07 19:16:13 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"


static bool push_entity(t_list *query, uint64_t id)
{
	t_list	*node;

	node = list_create((void *)(uintptr_t)(id));
	if (!node)
		return (false);
	list_addback(query, node);
	return (true);
}

static bool	should_add(uint64_t entity, uint64_t query, uint8_t comp)
{
	uint64_t	mask;

	mask = (1 << comp);
	return ((entity & query & mask) == mask);
}

static t_entity	*safe_entity_getter(t_universe *ecs, uint64_t id)
{
	assert(id < ECS_ENTITY_CAP);
	return (ecs->entities + id);
}

uint64_t	ecs_entity_create(t_universe *ecs)
{
	t_entity	*result;

	result = ecs->entities;
	result->alive = 1;
	result->mask = 0;
	return (0);
}

bool	ecs_entity_has(t_universe *ecs, uint64_t id, uint8_t comp)
{
	t_entity	ent;

	ent = *safe_entity_getter(ecs, id);
	return (ent.alive && (ent.mask & (1 << comp)) != 0);
}

void	*ecs_entity_get(t_universe *ecs, uint64_t id, uint8_t comp)
{
	if (!ecs_entity_has(ecs, id, comp))
		return (NULL);
	return (uint8_t*)ecs->components.data + (id * ecs->components.total_size + ecs->components.offsets[comp]);
}

void	ecs_entity_add(t_universe *ecs, uint64_t id, uint8_t comp, void *data)
{
	size_t		index;
	t_entity	*entity;
	t_query_map	*map;

	if (ecs_entity_has(ecs, id, comp))
		return ;
	entity = safe_entity_getter(ecs, id);
	entity->mask |= (1 << comp);
	ft_memcpy(ecs_entity_get(ecs, id, comp),
		   data, ecs->components.sizes[comp]);	
	map = ecs->queries;
	index = -1;
	while (++index < map->capacity)
		if (should_add(entity->mask, map->entries[index].key, comp))
			push_entity(map->entries[index].query, id);
}

void	ecs_entity_remove(t_universe *ecs, uint64_t id, uint8_t comp)
{
	//t_query_map	*map;
	//size_t		index;

	if (!ecs_entity_has(ecs, id, comp))
		return ;
	ecs->entities[id].mask &= ~(1 << comp);
	//map = ecs->queries;
	//index = -1;
	//while (++index < map->capacity)
	//	if (should_add(ent.components, map->entries[index].key, comp))
	//			remove_entity(map->entries[index].query, ent.id);
}
