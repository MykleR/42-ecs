/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   query_map.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mykle <mykle@42angouleme.fr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 20:15:32 by mykle             #+#    #+#             */
/*   Updated: 2024/12/21 13:10:06 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUERY_MAP_H
# define QUERY_MAP_H

# include <stdint.h>
# include <stdbool.h>
# include "uint_list.h"

# ifndef ECS_QUERY_CAP
#  define ECS_QUERY_CAP 8
# endif

typedef struct s_ecs_qentry
{
	t_ecs_ulist		query;
	uint64_t		key;
}					t_ecs_qentry;

typedef struct s_ecs_qmap
{
	t_ecs_qentry	entries[ECS_QUERY_CAP];
	uint16_t		capacity;
	uint16_t		length;
}					t_ecs_qmap;

bool			qm_create(t_ecs_qmap *map);
void			qm_destroy(t_ecs_qmap *map);
void			qm_remove(t_ecs_qmap *map, uint32_t val, uint64_t mask);
void			qm_insert(t_ecs_qmap *map, uint32_t val, uint64_t mask,
					uint64_t prev_mask);
t_ecs_ulist		*qm_get(t_ecs_qmap *map, uint64_t key, bool *new);

#endif
