/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/07 19:15:24 by mrouves          ###   ########.fr       */
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

static bool	fill_query(t_entity *entities, t_list *query, uint64_t signature)
{
	size_t		i;
	t_entity	entity;

	i = -1;
	while (++i < ECS_ENTITY_CAP)
	{
		entity = *(entities + i);
		if (entity.alive && (entity.mask & signature) == signature)
			if (!push_entity(query, i))
				return (false);
	}
	return (true);
}

t_list	*ecs_query(t_universe *ecs, uint64_t signature)
{
	t_list	*query;
	bool	creats;

	query = qm_get(ecs->queries, signature);
	creats = !query;
	if (creats)
		query = qm_add(ecs->queries, signature);
	if (!query)
		return (NULL);
	if (creats && !fill_query(ecs->entities, query, signature))
	{
		// TODO REMOVE QUERY with qm_delete()
		return (NULL);
	}
	return (query);
}
