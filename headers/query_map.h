/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/07 13:15:26 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 11:48:23 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUERY_MAP_H
# define QUERY_MAP_H

# include "list.h"
# include <stdbool.h>
# include <stdint.h>
# include <stdlib.h>

# define QM_INIT_SIZE 32

typedef struct s_map_entry
{
	uint64_t	key;
	t_list		*query;
}				t_map_entry;

typedef struct s_query_map
{
	size_t		length;
	size_t		capacity;
	t_map_entry	entries[QM_INIT_SIZE];
}				t_query_map;

t_query_map		*qm_create(void);
void			qm_destroy(t_query_map *map);
t_list			*qm_get(t_query_map *map, uint64_t key);
t_list			*qm_add(t_query_map *map, uint64_t key);

#endif
