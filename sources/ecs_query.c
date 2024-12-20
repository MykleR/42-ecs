/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/20 18:23:39 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"
#include "utils.h"

t_ecs_ulist	*ecs_query(t_ecs *ecs, uint64_t signature)
{
	t_ecs_ulist	*query;
	bool		is_new;
	uint32_t	i;

	if (__builtin_expect(!ecs || !signature, 0))
		return (NULL);
	query = qm_get(&ecs->queries, signature, &is_new);
	if (!query || !is_new)
		return (query);
	i = -1;
	while (++i < ECS_ENTITY_CAP)
		if ((signature & *(ecs->masks + i)) == signature)
			list_add(query, i);
	return (query);
}

uint32_t	ecs_entity_clone(t_ecs *ecs, uint32_t id)
{
	uint32_t		clone;

	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| !ecs_entity_alive(ecs, id), 0))
		return (0);
	clone = ecs_entity_create(ecs);
	*(ecs->masks + clone) = *(ecs->masks + id);
	qm_insert(&ecs->queries, clone, *(ecs->masks + id), 0);
	ft_memcpy(ecs_entity_get(ecs, clone, 0),
		ecs_entity_get(ecs, id, 0), ecs->mem_tsize);
	return (clone);
}

void	ecs_entity_add(t_ecs *ecs, uint32_t id, uint8_t comp, void *data)
{
	uint64_t		new_mask;

	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| comp >= ecs->nb_comps || !ecs_entity_alive(ecs, id), 0))
		return ;
	new_mask = *(ecs->masks + id) | (1ULL << comp);
	qm_insert(&ecs->queries, id, new_mask, *(ecs->masks + id));
	*(ecs->masks + id) = new_mask;
	if (data)
		ft_memcpy(ecs_entity_get(ecs, id, comp), data, ecs->mem_sizes[comp]);
	else
		ft_memset(ecs_entity_get(ecs, id, comp), 0, ecs->mem_sizes[comp]);
}

void	ecs_entity_remove(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| comp >= ecs->nb_comps || !ecs_entity_alive(ecs, id), 0))
		return ;
	qm_remove(&ecs->queries, id, (1ULL < comp));
	*(ecs->masks + id) &= ~(1ULL << comp);
}

void	ecs_entity_kill(t_ecs *ecs, uint32_t id)
{
	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| !ecs_entity_alive(ecs, id), 0))
		return ;
	qm_remove(&ecs->queries, id, *(ecs->masks + id));
	*(ecs->masks + id) = 0;
	ecs->entity_len--;
	((t_ecs_flist *)(ecs->masks + id))->next = ecs->free_list;
	ecs->free_list = (t_ecs_flist *)(ecs->masks + id);
}
