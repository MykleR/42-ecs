/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/07 19:12:05 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(size_t comps[64], size_t nb_comps, t_comps *holder)
{
	size_t	i;

	i = -1;
	holder->total_size = 0;
	ft_memset(holder->offsets, 0, sizeof(size_t) * 64);
	ft_memset(holder->sizes, 0, sizeof(size_t) * 64);
	while (++i < nb_comps)
	{
		holder->offsets[i] = holder->total_size;
		holder->sizes[i] = comps[i];
		holder->total_size += comps[i];
	}
	holder->nb_comps = nb_comps;
	holder->data = ft_calloc(ECS_ENTITY_CAP, holder->total_size);
	return (holder->data != NULL);
}

t_universe	*ecs_create(size_t comps[64], size_t nb_comps)
{
	t_universe	*ecs;

	ecs = malloc(sizeof(t_universe));
	if (!ecs)
		return (NULL);
	ecs->queries = qm_create();
	ecs->entities = ft_calloc(ECS_ENTITY_CAP, sizeof(t_entity));
	if (ecs_init_comps(comps, nb_comps, &(ecs->components)) 
			&& ecs->queries && ecs->entities)
		return (ecs);
	qm_destroy(ecs->queries);
	free(ecs->entities);
	free(ecs->components.data);
	free(ecs);
	return (NULL);
}

void	ecs_destroy(t_universe *ecs)
{
	if (!ecs)
		return ;
	qm_destroy(ecs->queries);
	free(ecs->entities);
	free(ecs->components.data);
	free(ecs);
}
