/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/21 13:11:52 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

uint32_t	ecs_entity_create(t_ecs *ecs)
{
	uint64_t	*ptr;
	uint64_t	id;

	if (__builtin_expect(!ecs, 0))
		return (ECS_ENTITY_CAP - 1);
	id = ecs->entity_len;
	if (ecs->free_list)
	{
		ptr = (uint64_t *)ecs->free_list;
		id = ptr - ecs->masks;
		ecs->free_list = ecs->free_list->next;
	}
	*(ecs->masks + id) = ECS_MASK_ALIVE;
	ecs->entity_len += (ecs->entity_len < ECS_ENTITY_CAP - 1);
	return (id);
}

inline bool	ecs_entity_alive(t_ecs *ecs, uint32_t id)
{
	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP, 0))
		return (false);
	return (*(ecs->masks + id) & ECS_MASK_ALIVE);
}

inline bool	ecs_entity_has(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| comp >= ecs->nb_comps, 0))
		return (false);
	return (ecs_entity_alive(ecs, id)
		&& ((*(ecs->masks + id) & (1ULL << comp))));
}

inline void	*ecs_entity_get(t_ecs *ecs, uint32_t id, uint8_t comp)
{
	size_t	offset;
	size_t	size;

	if (__builtin_expect(!ecs || id >= ECS_ENTITY_CAP
			|| comp >= ecs->nb_comps, 0))
		return (NULL);
	offset = *(ecs->mem_offsets + comp);
	size = ecs->mem_tsize;
	return ((uint8_t *)ecs->data + (id * size + offset));
}
