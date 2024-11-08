/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:23:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 16:20:35 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "query_map.h"
#include "utils.h"

static size_t	next_pow2(size_t n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return (++n);
}

t_query_map	*qm_create(void)
{
	t_query_map	*qm;
	size_t		capacity;

	capacity = next_pow2(QM_INIT_SIZE);
	if (__builtin_expect(capacity == 0, 0))
		return (NULL);
	qm = malloc(sizeof(t_query_map));
	if (__builtin_expect(qm == NULL , 0))
		return (NULL);
	ft_memset(qm->entries, 0, sizeof(t_map_entry) * capacity);
	qm->length = 0;
	qm->capacity = capacity;
	return (qm);
}

void	qm_destroy(t_query_map *map)
{
	size_t	i;

	if (__builtin_expect(map == NULL , 0))
		return ;
	i = -1;
	while (++i < map->capacity)
		list_clear(&(map->entries[i].query), NULL);
	free(map);
}

t_list	*qm_get(t_query_map *map, uint64_t key)
{
	uint64_t	index;
	uint64_t	start;

	if (__builtin_expect(map == NULL , 0))
		return (NULL);
	index = key & (map->capacity - 1);
	start = index;
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return (map->entries[index].query);
		index = (index + 1) & (map->capacity - 1);
		if (index == start)
			return (NULL);
	}
	return (NULL);
}

t_list	*qm_add(t_query_map *map, uint64_t key)
{
	uint64_t	index;
	uint64_t	start;

	if (__builtin_expect(map == NULL , 0))
		return (NULL);
	index = key & (map->capacity - 1);
	start = index;
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return (NULL);
		index = (index + 1) & (map->capacity - 1);
		if (index == start)
			return (NULL);
	}
	map->entries[index].query = list_create(NULL);
	if (__builtin_expect(map->entries[index].query == NULL, 0))
		return (NULL);
	map->entries[index].key = key;
	map->length++;
	return (map->entries[index].query);
}
