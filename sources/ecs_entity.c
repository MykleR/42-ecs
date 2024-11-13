/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 22:32:05 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/13 13:46:45 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

#ifndef DNDEBUG
# undef ecs_entity_create
# undef ecs_entity_get
# undef ecs_entity_has

uint32_t	__assert_entity_create(t_universe *ecs)
{
	assert(ecs);
	return (ecs_entity_create(ecs));
}

void	*__assert_entity_get(t_universe *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ecs->entity_cap);
	assert(comp < ecs->nb_comps);
	return (ecs_entity_get(ecs, id, comp));
}

bool	__assert_entity_has(t_universe *ecs, uint32_t id, uint8_t comp)
{
	assert(ecs);
	assert(id < ecs->entity_cap);
	assert(comp < ecs->nb_comps);
	return (ecs_entity_has(ecs, id, comp));
}
#endif

uint32_t	ecs_entity_create(t_universe *ecs)
{
	if (ecs->entity_len < ecs->entity_cap)
	{
		ecs->entity_len++;
		return (list_popfront(&(ecs->entity_pool)));
	}
	return (-1);
}

bool	ecs_entity_has(t_universe *ecs, uint32_t id, uint8_t comp)
{

	return ((*(ecs->masks + id) & (1 << comp)) != 0);
}

void	*ecs_entity_get(t_universe *ecs, uint32_t id, uint8_t comp)
{
	size_t	offset;
	size_t	size;

	offset = *(ecs->mem_offsets + comp);
	size = ecs->mem_tsize;
	return ((uint8_t *)ecs->data + (id * size + offset));
}
