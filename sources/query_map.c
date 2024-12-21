/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:23:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/21 13:10:25 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "query_map.h"
#include "utils.h"

bool	qm_create(t_ecs_qmap *map)
{
	uint16_t	capacity;

	capacity = ECS_QUERY_CAP - 1;
	capacity |= capacity >> 1;
	capacity |= capacity >> 2;
	capacity |= capacity >> 4;
	capacity |= capacity >> 8;
	capacity++;
	if (__builtin_expect(capacity == 0, 0))
		return (false);
	map->length = 0;
	map->capacity = capacity;
	ft_memset(map->entries, 0, sizeof(t_ecs_qentry) * capacity);
	return (true);
}

void	qm_destroy(t_ecs_qmap *map)
{
	size_t	i;

	if (__builtin_expect(!map, 0))
		return ;
	i = -1;
	while (++i < map->capacity)
		list_destroy(&map->entries[i].query);
	ft_memset(map, 0, sizeof(t_ecs_qmap));
}

t_ecs_ulist	*qm_get(t_ecs_qmap *map, uint64_t key, bool *new)
{
	size_t	index;

	if (__builtin_expect(!map || !key || map->length >= map->capacity, 0))
		return (NULL);
	if (__builtin_expect(new != NULL, 1))
		*new = false;
	index = key & (map->capacity - 1);
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return (&(map->entries + index)->query);
		index = (index + 1) & (map->capacity - 1);
	}
	if (__builtin_expect(!list_create(&(map->entries + index)->query), 0))
		return (NULL);
	if (__builtin_expect(new != NULL, 1))
		*new = true;
	map->length++;
	(map->entries + index)->key = key;
	return (&(map->entries + index)->query);
}

void	qm_remove(t_ecs_qmap *map, uint32_t val, uint64_t mask)
{
	uint32_t	i;
	uint64_t	key;

	if (__builtin_expect(!map, 0))
		return ;
	i = 0;
	while (i < map->capacity)
	{
		key = (map->entries + i)->key;
		if ((key & mask) == key)
			list_remove(&(map->entries + i)->query, val);
		i++;
	}
}

void	qm_insert(t_ecs_qmap *map, uint32_t val,
			uint64_t mask, uint64_t prev_mask)
{
	uint32_t	i;
	uint64_t	key;

	if (__builtin_expect(!map, 0))
		return ;
	i = 0;
	while (i < map->capacity)
	{
		key = (map->entries + i)->key;
		if ((key & mask) == key && !((key & prev_mask) == key))
			list_add(&(map->entries + i)->query, val);
		i++;
	}
}
