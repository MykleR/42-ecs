/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:52:19 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/02 20:00:05 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_H
# define ECS_H

# include "uint_list.h"
# include "utils.h"

# include <assert.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdarg.h>

# define ECS_ENTITY_CAP 1024
# define ECS_MAP_INIT_SIZE 8

# define ECS_USED_MASK 0x8000000000000000

typedef struct s_ecs_qentry
{
	t_ecs_ulist		query;
	uint64_t		key;
}					t_ecs_qentry;

typedef struct s_ecs_qmap
{
	t_ecs_qentry	entries[ECS_MAP_INIT_SIZE];
	uint16_t		capacity;
	uint16_t		length;
}					t_ecs_qmap;

typedef struct s_ecs_flist
{
	struct s_ecs_flist	*next;
}	t_ecs_flist;

// Struct to represent the ecs universe
typedef struct s_ecs
{
	uint64_t		masks[ECS_ENTITY_CAP];
	void			*data;
	t_ecs_qmap		*queries;
	t_ecs_flist		*free_list;
	uint16_t		mem_offsets[63];
	uint16_t		mem_sizes[63];
	uint32_t		mem_tsize;
	uint32_t		entity_len;
	uint8_t			nb_comps;
}					t_ecs;

t_ecs			*ecs_create(uint32_t nb_comps, ...);
void			ecs_destroy(t_ecs *ecs);
t_ecs_ulist		*ecs_query(t_ecs *ecs, uint64_t signature);

uint32_t		ecs_entity_create(t_ecs *ecs);
uint32_t		ecs_entity_clone(t_ecs *ecs, uint32_t id);
void			ecs_entity_kill(t_ecs *ecs, uint32_t id);
void			*ecs_entity_get(t_ecs *ecs, uint32_t id, uint8_t comp);
void			ecs_entity_remove(t_ecs *ecs, uint32_t id, uint8_t comp);
void			ecs_entity_add(t_ecs *ecs, uint32_t id,
					uint8_t comp, void *data);
bool			ecs_entity_alive(t_ecs *ecs, uint32_t id);
bool			ecs_entity_has(t_ecs *ecs, uint32_t id, uint8_t comp);

t_ecs_qmap		*qm_create(void);
void			qm_destroy(t_ecs_qmap *map);
void			qm_remove(t_ecs_qmap *map, uint32_t val, uint64_t mask);
void			qm_insert(t_ecs_qmap *map, uint32_t val, uint64_t mask,
					uint64_t prev_mask);
t_ecs_ulist		*qm_get(t_ecs_qmap *map, uint64_t key);

#endif
