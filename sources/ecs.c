/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 21:23:14 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(uint16_t comps[63], uint8_t nb, t_ecs *ecs)
{
	uint8_t	i;

	ecs->mem_tsize = 0;
	ecs->nb_comps = nb;
	ecs->data = NULL;
	ft_memset(ecs->mem_offsets, 0, sizeof(uint16_t) * 63);
	ft_memset(ecs->mem_sizes, 0, sizeof(uint16_t) * 63);
	i = -1;
	while (++i < nb)
	{
		if (!comps[i])
			return (false);
		ecs->mem_offsets[i] = ecs->mem_tsize;
		ecs->mem_sizes[i] = comps[i];
		ecs->mem_tsize += comps[i];
	}
	ecs->data = ft_calloc(ECS_ENTITY_CAP, ecs->mem_tsize);
	return (ecs->data != NULL);
}

t_ecs	*ecs_create(uint16_t comps[63], uint8_t nb)
{
	t_ecs	*ecs;

	ecs = malloc(sizeof(t_ecs));
	if (__builtin_expect(!ecs, 0))
		return (NULL);
	ft_memset(ecs->masks, 0, ECS_ENTITY_CAP * sizeof(uint64_t));
	ecs->free_list = NULL;
	ecs->entity_len = 0;
	ecs->queries = qm_create();
	if (ecs_init_comps(comps, nb, ecs) && ecs->queries)
		return (ecs);
	ecs_destroy(ecs);
	return (NULL);
}

void	ecs_destroy(t_ecs *ecs)
{
	if (!ecs)
		return ;
	qm_destroy(ecs->queries);
	free(ecs->data);
	free(ecs);
}
