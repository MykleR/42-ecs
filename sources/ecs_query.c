/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/18 15:33:57 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef NDEBUG
# undef ecs_query
# undef ecs_entity_clone
# undef ecs_entity_add
# undef ecs_entity_remove
# undef ecs_entity_kill
# undef ecs_entity_has
# undef ecs_entity_create
# undef ecs_entity_get

t_list	*__assert_query(t_universe *ecs, uint64_t signature)
{
	assert(ecs);
	return (ecs_query(ecs, signature));
}

uint32_t	__assert_entity_clone(t_universe *ecs, uint32_t id)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	return (ecs_entity_clone(ecs, id));
}

void	__assert_entity_remove(t_universe *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->nb_comps);
	ecs_entity_remove(ecs, id, comp);
}

void	__assert_entity_add(t_universe *ecs, uint32_t id, uint8_t comp,
		void *data)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->nb_comps);
	ecs_entity_add(ecs, id, comp, data);
}

void	__assert_entity_kill(t_universe *ecs, uint32_t id)
{
	assert(id < ECS_ENTITY_CAP);
	ecs_entity_kill(ecs, id);
}

#endif

t_list	*ecs_query(t_universe *ecs, uint64_t signature)
{
	t_map_entry	*entry;
	bool		is_new_query;
	uint32_t	i;

	entry = qm_get(ecs->queries, signature, &is_new_query);
	if (!is_new_query)
		return (entry->query);
	i = -1;
	while (++i < ECS_ENTITY_CAP)
		if (qm_is_inquery(signature, *(ecs->masks + i)))
			list_insert(&(entry->query), i);
	return (entry->query);
}

uint32_t	ecs_entity_clone(t_universe *ecs, uint32_t id)
{
	t_map_entry	*entry;
	uint32_t	new_id;
	size_t		i;

	new_id = ecs_entity_create(ecs);
	ecs->masks[new_id] = ecs->masks[id]; 
	i = -1;
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if (qm_is_inquery(entry->key, ecs->masks[new_id]))
			list_insert(&(entry->query), new_id);
	}
	ft_memcpy(ecs_entity_get(ecs, new_id, 0),
		   ecs_entity_get(ecs, id, 0), ecs->mem_tsize);
	return (new_id);
}

void	ecs_entity_add(t_universe *ecs, uint32_t id, uint8_t comp, void *data)
{
	t_map_entry	*entry;
	uint64_t	new_mask;
	size_t		i;

	i = -1;
	new_mask = *(ecs->masks + id) | (1 << comp);
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if (qm_is_inquery(entry->key, new_mask) &&
			!qm_is_inquery(entry->key, ecs->masks[id]))
			list_insert(&(entry->query), id);
	}
	if (data)
		ft_memcpy(ecs_entity_get(ecs, id, comp), data, ecs->mem_sizes[comp]);
	else
		ft_memset(ecs_entity_get(ecs, id, comp), 0, ecs->mem_sizes[comp]);
	*(ecs->masks + id) = new_mask;
}

void	ecs_entity_remove(t_universe *ecs, uint32_t id, uint8_t comp)
{
	t_map_entry	*entry;
	size_t		i;

	i = -1;
	ecs->masks[id] &= ~(1 << comp);
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if ((entry->key & (1 << comp)) != 0)
			list_remove(&(entry->query), id);
	}
}

void	ecs_entity_kill(t_universe *ecs, uint32_t id)
{
	t_map_entry	*entry;
	size_t		i;

	i = -1;
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if (qm_is_inquery(entry->key, ecs->masks[id]))
			list_remove(&(entry->query), id);
	}
	ecs->masks[id] = 0;
	ecs->entity_len--;
	((t_free_list *)ecs->masks + id)->next = ecs->free_list;
	ecs->free_list = (t_free_list *)(ecs->masks + id);
}

