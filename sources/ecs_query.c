/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_query.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:33 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 15:21:48 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef DNDEBUG
# undef ecs_query

t_list	*__assert_query(t_universe *ecs, uint64_t signature)
{
	assert(ecs);
	return (ecs_query(ecs, signature));
}

#endif

static bool	push_entity(t_list *query, uint64_t id)
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

	i = 0;
	while (i < ECS_ENTITY_CAP)
	{
		entity = *(entities + i);
		if (entity.alive && (entity.mask & signature) == signature
			&& !push_entity(query, i))
			return (false);
		i++;
	}
	return (true);
}

t_list	*ecs_query(t_universe *ecs, uint64_t signature)
{
	t_list	*query;
	bool	creates;

	query = qm_get(ecs->queries, signature);
	creates = !query;
	if (creates)
		query = qm_add(ecs->queries, signature);
	if (!query)
		return (NULL);
	if (creates && !fill_query(ecs->entities, query, signature))
		return (NULL);
	return (query);
}
