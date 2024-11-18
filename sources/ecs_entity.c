/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/18 15:34:42 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef NDEBUG
# undef ecs_query
# undef ecs_entity_clone
# undef ecs_entity_add
# undef ecs_entity_remove
# undef ecs_entity_kill
# undef ecs_entity_has
# undef ecs_entity_create
# undef ecs_entity_get

uint32_t	__assert_entity_create(t_universe *ecs)
{
	assert(ecs);
	return (ecs_entity_create(ecs));
}

void	*__assert_entity_get(t_universe *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->nb_comps);
	return (ecs_entity_get(ecs, id, comp));
}

bool	__assert_entity_has(t_universe *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ECS_ENTITY_CAP);
	assert(comp < ecs->nb_comps);
	return (ecs_entity_has(ecs, id, comp));
}
#endif

uint32_t	ecs_entity_create(t_universe *ecs)
{
	uint64_t	*ptr;

	if (ecs->entity_len >= ECS_ENTITY_CAP)
		return (ECS_ENTITY_CAP - 1);
	if (!ecs->free_list)
		return (ecs->entity_len++);
	ptr = (uint64_t *)ecs->free_list;
	ecs->free_list = ecs->free_list->next;
	ecs->entity_len++;
	ft_memset(ptr, 0, ecs->mem_tsize);
	return (ptr - ecs->masks);
}

bool	ecs_entity_has(t_universe *ecs, uint32_t id, uint8_t comp)
{

	return ((*(ecs->masks + id) & (1 << comp)));
}

void	*ecs_entity_get(t_universe *ecs, uint32_t id, uint8_t comp)
{
	size_t	offset;
	size_t	size;

	offset = *(ecs->mem_offsets + comp);
	size = ecs->mem_tsize;
	return ((uint8_t *)ecs->data + (id * size + offset));
}
