/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_components.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/08 15:24:28 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 16:38:45 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef DNDEBUG
# undef ecs_entity_add
# undef ecs_entity_remove

void	__assert_entity_remove(t_universe *ecs, uint64_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->components.nb_comps);
	ecs_entity_remove(ecs, id, comp);
}

void	__assert_entity_add(t_universe *ecs, uint64_t id, uint8_t comp,
		void *data)
{
	assert(ecs);
	assert(data);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->components.nb_comps);
	ecs_entity_add(ecs, id, comp, data);
}

#endif

static t_list	*new_int_node(uint64_t id)
{
	return (list_create((void *)(uintptr_t)(id)));
}

static bool	is_inquery(uint64_t query_mask, uint64_t entity_mask)
{
	return ((query_mask & entity_mask) == query_mask);
}

void	ecs_entity_add(t_universe *ecs, uint64_t id, uint8_t comp, void *data)
{
	t_entity	*entity;
	t_map_entry	entry;
	uint64_t	new_mask;
	size_t		i;

	if (ecs_entity_has(ecs, id, comp))
		return ;
	entity = ecs->entities + id;
	new_mask = entity->mask | (1 << comp);
	i = 0;
	while (i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries[i];
		if (is_inquery(entry.key, new_mask) && !is_inquery(entry.key, entity->mask))
			list_addback(entry.query, new_int_node(id));
		i++;
	}
	ft_memcpy(ecs_entity_get(ecs, id, comp), data, ecs->components.sizes[comp]);
	entity->mask |= new_mask;
}

void	ecs_entity_remove(t_universe *ecs, uint64_t id, uint8_t comp)
{
	t_entity	*entity;
	t_map_entry	entry;
	size_t		i;

	if (!ecs_entity_has(ecs, id, comp))
		return ;
	i = 0;
	entity = ecs->entities + id;
	while (i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries[i];
		if (is_inquery(entry.key, entity->mask))
			list_delone(list_find(entry.query, (void *)(uintptr_t)(id)), NULL);
		i++;
	}
	entity->mask &= ~(1 << comp);
}
