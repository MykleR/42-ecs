/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 12:15:51 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(size_t comps[64], uint8_t nb_comps, t_comps *holder)
{
	uint8_t	i;

	i = 0;
	holder->total_size = 0;
	holder->nb_comps = nb_comps;
	ft_memset(holder->offsets, 0, sizeof(size_t) * 64);
	ft_memset(holder->sizes, 0, sizeof(size_t) * 64);
	while (i < nb_comps)
	{
		if (!comps[i])
			return (false);
		holder->offsets[i] = holder->total_size;
		holder->sizes[i] = comps[i];
		holder->total_size += comps[i];
		i++;
	}
	holder->data = ft_calloc(ECS_ENTITY_CAP, holder->total_size);
	return (holder->data != NULL);
}

t_universe	*ecs_create(size_t comps[64], size_t nb_comps)
{
	t_universe	*ecs;

	ecs = malloc(sizeof(t_universe));
	if (!ecs)
		return (NULL);
	ecs->len_entities = 0;
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
