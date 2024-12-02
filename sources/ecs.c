/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:12:49 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/02 20:24:01 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"
#include "utils.h"

static bool	ecs_init_comps(t_ecs *ecs, uint8_t nb, va_list args)
{
	uint8_t		i;
	uint32_t	mem_size;

	ecs->nb_comps = nb;
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

	ecs = ft_calloc(sizeof(t_ecs), 1);
	if (__builtin_expect(!ecs || nb > 63, 0))
		return (NULL);
	va_start(args, nb);
	if (!qm_create(&ecs->queries) || !ecs_init_comps(ecs, nb, args))
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
	qm_destroy(&ecs->queries);
	free(ecs->data);
	ft_memset(ecs, 0, sizeof(t_ecs));
	free(ecs);
}
