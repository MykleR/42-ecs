/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 13:40:58 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(size_t comps[64], size_t nb_comps, t_comps *holder)
{
	size_t	i;

	i = -1;
	holder->total_size = 0;
	while (++i < nb_comps)
	{
		holder->offsets[i] = holder->total_size;
		holder->sizes[i] = comps[i];
		holder->total_size += comps[i];
	}
	holder->nb_comps = nb_comps;
	holder->data = ft_calloc(ECS_INIT_CAP, holder->total_size);
	return (holder->data != false);
}

t_universe	*ecs_create(size_t comps[64], size_t nb_comps)
{
	t_universe	*ecs;

	ecs = malloc(sizeof(t_universe));
	if (!ecs)
		return (NULL);
	ecs->queries = NULL;
	ecs->components.data = NULL;
	ecs->entities = ft_calloc(ECS_INIT_CAP, sizeof(t_entity));
	if (ecs->entities && ecs_init_comps(comps, nb_comps, &(ecs->components)))
		return (ecs);
	free(ecs->entities);
	free(ecs->components.data);
	free(ecs);
	return (NULL);
}

void	ecs_destroy(t_universe *ecs)
{
	if (!ecs)
		return ;
	free(ecs->entities);
	free(ecs->components.data);
	list_clear(&(ecs->queries), NULL);
	free(ecs);
}
