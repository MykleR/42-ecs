/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/22 11:16:46 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

uint32_t	ecs_entity_create(t_ecs *ecs)
{
	uint64_t	*ptr;
	uint64_t	id;

	assert(ecs);
	id = ecs->entity_len;
	if (ecs->free_list)
	{
		ptr = (uint64_t *)ecs->free_list;
		id = ptr - ecs->masks;
		ecs->free_list = ecs->free_list->next;
	}
	*(ecs->masks + id) = ECS_USED_MASK;
	ecs->entity_len += (ecs->entity_len < ECS_ENTITY_CAP);
	return (id);
}

bool	ecs_entity_alive(t_ecs *ecs, uint32_t id)
{
	assert(ecs && id < ECS_ENTITY_CAP);
	return (*(ecs->masks + id) & ECS_USED_MASK);
}

bool	ecs_entity_has(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	return (ecs_entity_alive(ecs, id)
		&& ((*(ecs->masks + id) & (1ULL << comp))));
}

void	*ecs_entity_get(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	size_t	offset;
	size_t	size;

	assert(ecs && id < ECS_ENTITY_CAP && comp < ecs->nb_comps);
	offset = *(ecs->mem_offsets + comp);
	size = ecs->mem_tsize;
	return ((uint8_t *)ecs->data + (id * size + offset));
}
