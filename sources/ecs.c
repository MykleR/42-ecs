/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/22 14:18:18 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static bool	ecs_init_comps(t_ecs *ecs, uint8_t nb, va_list args)
{
	uint8_t		i;
	uint32_t	mem_size;

	ecs->mem_tsize = 0;
	ecs->nb_comps = nb;
	ecs->data = NULL;
	ft_memset(ecs->mem_offsets, 0, sizeof(uint16_t) * 63);
	ft_memset(ecs->mem_sizes, 0, sizeof(uint16_t) * 63);
	i = -1;
	while (++i < nb)
	{
		mem_size = va_arg(args, uint32_t);
		if (!mem_size || mem_size >= UINT16_MAX)
			return (false);
		ecs->mem_offsets[i] = ecs->mem_tsize;
		ecs->mem_sizes[i] = mem_size;
		ecs->mem_tsize += mem_size;
	}
	ecs->data = ft_calloc(ECS_ENTITY_CAP, ecs->mem_tsize);
	return (ecs->data != NULL);
}

t_ecs	*ecs_create(uint32_t nb, ...)
{
	t_ecs	*ecs;
	va_list	args;

	ecs = malloc(sizeof(t_ecs));
	if (__builtin_expect(!ecs || nb > 63, 0))
		return (NULL);
	ft_memset(ecs->masks, 0, ECS_ENTITY_CAP * sizeof(uint64_t));
	ecs->free_list = NULL;
	ecs->entity_len = 0;
	ecs->queries = qm_create();
	va_start(args, nb);
	if (!ecs->queries || !ecs_init_comps(ecs, nb, args))
	{
		ecs_destroy(ecs);
		ecs = NULL;
	}
	va_end(args);
	return (ecs);
}

void	ecs_destroy(t_ecs *ecs)
{
	if (!ecs)
		return ;
	qm_destroy(ecs->queries);
	free(ecs->data);
	free(ecs);
}
