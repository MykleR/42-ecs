/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_map.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 18:22:15 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 00:12:06 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "hash_map.h"

//https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
static uint64_t	hash_key(uint64_t key)
{
	uint64_t	hash;

	hash = FNV_OFFSET;
	hash ^= (uint64_t)(unsigned char)(key);
	hash *= FNV_PRIME;
	return (hash);
}

t_hash_map	*hm_create(size_t capacity)
{
	t_hash_map	*hm;

	hm = malloc(sizeof(t_hash_map));
	if (hm == NULL)
		return (NULL);
	hm->length = 0;
	hm->capacity = capacity;
	hm->entries = ft_calloc(sizeof(t_hash_entry), capacity);
	if (hm->entries)
		return (hm);
	free(hm);
	return (NULL);
}

void	hm_destroy_all(t_hash_map *map, void (*free_item)(void*))
{
	size_t	i;

	if (!map)
		return ;
	i = -1;
	while (++i < map->capacity)
		if (map->entries[i].value && free_item)
			free_item(map->entries[i].value);
	free(map->entries);
	free(map);
}

void	*hm_get(t_hash_map *map, uint64_t key)
{
	uint64_t	hash;
	size_t		index;
	size_t		start;

	if (!map || !key)
		return (NULL);
	hash = hash_key(key);
	index = (size_t)(hash % map->capacity);
	start = index;
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return (map->entries[index].value);
		index = (index + 1) % map->capacity;
		if (index == start)
			return (NULL);
	}
	return (NULL);
}

bool	hm_set(t_hash_map *map, uint64_t key, void *val)
{
	uint64_t	hash;
	size_t		index;
	size_t		start;

	if (!map || !val || !key)
		return (false);
	hash = hash_key(key);
	index = (size_t)(hash % map->capacity);
	start = index;
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
		{
			map->entries[index].value = val;
			return (true);
		}
		index = (index + 1) % map->capacity;
		if (index == start)
			return (false);
	}
	map->entries[index].key = key;
	map->entries[index].value = val;
	map->length++;
	return (true);
}
