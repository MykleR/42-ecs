/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hash_map.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/27 18:02:40 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 00:10:51 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HASH_MAP_H
# define HASH_MAP_H

# define HM_INIT_CAP 32
# define FNV_OFFSET 14695981039346656037UL
# define FNV_PRIME 1099511628211UL

# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include "utils.h"

typedef struct s_hash_entry
{
	uint64_t	key;
	void		*value;
}	t_hash_entry;

typedef struct s_hash_map
{
	t_hash_entry	*entries;
	size_t			length;
	size_t			capacity;
}	t_hash_map;

t_hash_map	*hm_create(size_t capacity);
void		hm_destroy_all(t_hash_map *map, void (*free_item)(void*));
void		*hm_get(t_hash_map *map, uint64_t key);
bool		hm_set(t_hash_map *map, uint64_t key, void *val);

#endif
