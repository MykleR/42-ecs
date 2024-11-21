/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:23:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/22 00:23:47 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

t_ecs_qmap	*qm_create(void)
{
	t_ecs_qmap	*qm;
	uint16_t	capacity;

	capacity = ECS_MAP_INIT_SIZE - 1;
	capacity |= capacity >> 1;
	capacity |= capacity >> 2;
	capacity |= capacity >> 4;
	capacity |= capacity >> 8;
	capacity++;
	if (__builtin_expect(capacity == 0, 0))
		return (NULL);
	qm = malloc(sizeof(t_ecs_qmap));
	if (__builtin_expect(qm == NULL, 0))
		return (NULL);
	ft_memset(qm->entries, 0, sizeof(t_ecs_qentry) * capacity);
	qm->capacity = capacity;
	qm->length = 0;
	return (qm);
}

void	qm_destroy(t_ecs_qmap *map)
{
	size_t	i;

	if (__builtin_expect(map == NULL, 0))
		return ;
	i = -1;
	while (++i < map->capacity)
		list_destroy(map->entries[i].query);
	free(map);
}

t_ecs_ulist	*qm_get(t_ecs_qmap *map, uint64_t key)
{
	size_t	index;

	if (__builtin_expect(!map || !key || map->length >= map->capacity, 0))
		return (NULL);
	index = key & (map->capacity - 1);
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return ((map->entries + index)->query);
		index = (index + 1) & (map->capacity - 1);
	}
	map->length++;
	(map->entries + index)->key = key;
	(map->entries + index)->query = NULL;
	return (NULL);
}

void	qm_remove(t_ecs_qmap *map, uint32_t val, uint64_t mask)
{
	uint32_t	i;
	uint64_t	key;

	i = 0;
	while (i < map->capacity)
	{
		key = (map->entries + i)->key;
		if ((key & mask) == key)
			list_remove((map->entries + i)->query, val);
		i++;
	}
}

void	qm_insert(t_ecs_qmap *map, uint32_t val, uint64_t mask, uint64_t prev_mask)
{
	uint32_t	i;
	uint64_t	key;

	i = 0;
	while (i < map->capacity)
	{
		key = (map->entries + i)->key;
		if ((key & mask) == key && !((key & prev_mask) == key))
			list_add((map->entries + i)->query, val);
		i++;
	}
}
