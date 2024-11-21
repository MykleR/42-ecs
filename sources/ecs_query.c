/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/22 00:34:42 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

t_ecs_ulist	*ecs_query(t_ecs *ecs, uint64_t signature)
{
	t_ecs_ulist	*query;
	uint32_t		i;

	assert(ecs && signature);
	query = qm_get(ecs->queries, signature);
	if (query)
		return (query);
	if (ecs->queries->length >= ecs->queries->capacity)
		return (NULL);
	query = list_create(ECS_QUERY_INIT_SIZE);
	i = -1;
	while (++i < ECS_ENTITY_CAP)
		if ((signature & *(ecs->masks + i)) == signature)
			list_add(query, i);
	return (query);
}

uint32_t	ecs_entity_clone(t_ecs *ecs, uint32_t id)
{
	uint32_t		clone;

	assert(ecs && id < ECS_ENTITY_CAP);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return (0);
	clone = ecs_entity_create(ecs);
	*(ecs->masks + clone) = *(ecs->masks + id);
	qm_insert(ecs->queries, id, *(ecs->masks + id), 0);
	ft_memcpy(ecs_entity_get(ecs, clone, 0),
		ecs_entity_get(ecs, id, 0), ecs->mem_tsize);
	return (clone);
}

void	ecs_entity_add(t_ecs *ecs, uint32_t id, uint8_t comp, void *data)
{
	uint64_t		new_mask;

	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	new_mask = *(ecs->masks + id) | (1ULL << comp);
	qm_insert(ecs->queries, id, new_mask, *(ecs->masks + id));
	*(ecs->masks + id) = new_mask;
	if (data)
		ft_memcpy(ecs_entity_get(ecs, id, comp), data, ecs->mem_sizes[comp]);
	else
		ft_memset(ecs_entity_get(ecs, id, comp), 0, ecs->mem_sizes[comp]);
}

void	ecs_entity_remove(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	qm_remove(ecs->queries, id, (1ULL < comp));
	*(ecs->masks + id) &= ~(1ULL << comp);
}

void	ecs_entity_kill(t_ecs *ecs, uint32_t id)
{
	assert(ecs && id < ECS_ENTITY_CAP);
	if (__builtin_expect(!ecs_entity_alive(ecs, id), 0))
		return ;
	qm_remove(ecs->queries, id, *(ecs->masks + id));
	*(ecs->masks + id) = 0;
	ecs->entity_len--;
	((t_ecs_flist *)(ecs->masks + id))->next = ecs->free_list;
	ecs->free_list = (t_ecs_flist *)(ecs->masks + id);
	ft_memset(ecs_entity_get(ecs, id, 0), 0, ecs->mem_tsize);
}
