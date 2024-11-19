/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:23:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 22:25:57 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs.h"

static size_t	next_pow2(size_t n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return (++n);
}

bool	qm_is_inquery(uint64_t key, uint64_t mask)
{
	return (key && (key & mask) == key);
}

t_ecs_qmap	*qm_create(void)
{
	t_ecs_qmap	*qm;
	size_t		capacity;

	capacity = next_pow2(QM_INIT_SIZE);
	if (__builtin_expect(capacity == 0, 0))
		return (NULL);
	qm = malloc(sizeof(t_ecs_qmap));
	if (__builtin_expect(qm == NULL, 0))
		return (NULL);
	ft_memset(qm->entries, 0, sizeof(t_ecs_qentry) * capacity);
	qm->capacity = capacity;
	return (qm);
}

void	qm_destroy(t_ecs_qmap *map)
{
	size_t	i;

	if (__builtin_expect(map == NULL, 0))
		return ;
	i = -1;
	while (++i < map->capacity)
		list_clear(&(map->entries[i].query));
	free(map);
}

t_ecs_qentry	*qm_get(t_ecs_qmap *map, uint64_t key, bool *res)
{
	size_t	index;
	size_t	start;

	if (__builtin_expect(res != NULL, 1))
		*res = false;
	if (__builtin_expect(!map || !key, 0))
		return (NULL);
	index = key & (map->capacity - 1);
	start = index;
	while (map->entries[index].key != 0)
	{
		if (key == map->entries[index].key)
			return (map->entries + index);
		index = (index + 1) & (map->capacity - 1);
		if (index == start)
			return (NULL);
	}
	if (__builtin_expect(res != NULL, 1))
		*res = true;
	map->entries[index].key = key;
	return (map->entries + index);
}
