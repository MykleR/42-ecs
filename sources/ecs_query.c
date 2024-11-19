/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 23:55:46 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

t_ecs_ulist	*ecs_query(t_ecs *ecs, uint64_t signature)
{
	t_ecs_qentry	*entry;
	bool			is_new_query;
	uint32_t		i;

	assert(ecs && signature);
	entry = qm_get(ecs->queries, signature, &is_new_query);
	if (!is_new_query)
		return (entry->query);
	i = -1;
	while (++i < ECS_ENTITY_CAP)
		if (qm_is_inquery(signature, *(ecs->masks + i)))
			list_insert(&(entry->query), i);
	return (entry->query);
}

uint32_t	ecs_entity_clone(t_ecs *ecs, uint32_t id)
{
	t_ecs_qentry	*entry;
	uint32_t		new_id;
	uint16_t		i;

	assert(ecs && id < ECS_ENTITY_CAP);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return (0);
	i = -1;
	new_id = ecs_entity_create(ecs);
	*(ecs->masks + new_id) = *(ecs->masks + id);
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

void	ecs_entity_add(t_ecs *ecs, uint32_t id, uint8_t comp, void *data)
{
	t_ecs_qentry	*entry;
	uint64_t		new_mask;
	uint16_t		i;

	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	i = -1;
	new_mask = *(ecs->masks + id) | (1ULL << comp);
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if (qm_is_inquery(entry->key, new_mask)
			&& !qm_is_inquery(entry->key, ecs->masks[id]))
			list_insert(&(entry->query), id);
	}
	if (data)
		ft_memcpy(ecs_entity_get(ecs, id, comp), data, ecs->mem_sizes[comp]);
	else
		ft_memset(ecs_entity_get(ecs, id, comp), 0, ecs->mem_sizes[comp]);
	*(ecs->masks + id) = new_mask;
}

void	ecs_entity_remove(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	t_ecs_qentry	*entry;
	uint16_t		i;

	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	i = -1;
	*(ecs->masks + id) &= ~(1ULL << comp);
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if ((entry->key & (1ULL << comp)) != 0)
			list_remove(&(entry->query), id);
	}
}

void	ecs_entity_kill(t_ecs *ecs, uint32_t id)
{
	t_ecs_qentry	*entry;
	uint16_t		i;

	assert(ecs && id < ECS_ENTITY_CAP);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	i = -1;
	while (++i < ecs->queries->capacity)
	{
		entry = ecs->queries->entries + i;
		if (qm_is_inquery(entry->key, ecs->masks[id]))
			list_remove(&(entry->query), id);
	}
	ecs->entity_len--;
	((t_ecs_flist *)(ecs->masks + id))->next = ecs->free_list;
	ecs->free_list = (t_ecs_flist *)(ecs->masks + id);
	ft_memset(ecs_entity_get(ecs, id, 0), 0, ecs->mem_tsize);
}
