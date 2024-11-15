/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/15 17:18:09 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(size_t comps[64], uint8_t nb, t_universe *ecs)
{
	size_t	i;

	ecs->mem_tsize = 0;
	ecs->nb_comps = nb;
	ecs->data = NULL;
	ft_memset(ecs->mem_offsets, 0, sizeof(size_t) * 64);
	ft_memset(ecs->mem_sizes, 0, sizeof(size_t) * 64);
	i = -1;
	while (++i < nb)
	{
		if (!comps[i])
			return (false);
		ecs->mem_offsets[i] = ecs->mem_tsize;
		ecs->mem_sizes[i] = comps[i];
		ecs->mem_tsize += comps[i];
	}
	ecs->data = ft_calloc(ecs->entity_cap, ecs->mem_tsize);
	return (ecs->data != NULL);
}

static void ecs_init_pool(t_universe *ecs)
{
	uint32_t	id;

	id = ecs->entity_cap;
	ecs->entity_pool = NULL;
	while(id--)
		list_insert(&ecs->entity_pool, id);
}

t_universe	*ecs_create(size_t comps[64], size_t nb)
{
	t_universe	*ecs;

	ecs = malloc(sizeof(t_universe));
	if (!ecs)
		return (NULL);
	ecs->entity_len = 0;
	ecs->entity_cap = ECS_ENTITY_CAP;
	ecs->queries = qm_create();
	ecs->masks = ft_calloc(ecs->entity_cap, sizeof(uint64_t));
	ecs_init_pool(ecs);
	if (ecs_init_comps(comps, nb, ecs) && ecs->queries && ecs->masks)
		return (ecs);
	ecs_destroy(ecs);
	return (NULL);
}

void	ecs_destroy(t_universe *ecs)
{
	if (!ecs)
		return ;
	qm_destroy(ecs->queries);
	list_clear(&ecs->entity_pool);
	free(ecs->masks);
	free(ecs->data);
	free(ecs);
}
