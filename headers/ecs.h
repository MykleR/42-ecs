/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:52:19 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/13 13:48:39 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_H
# define ECS_H

# include "assert.h"
# include "uint_list.h"
# include "utils.h"

# include <stdbool.h>
# include <stdint.h>

# define ECS_ENTITY_CAP 1024
# define QM_INIT_SIZE 16

typedef struct s_map_entry
{
	t_list		*query;
	uint64_t	key;
}				t_map_entry;

typedef struct s_query_map
{
	size_t		capacity;
	t_map_entry	entries[QM_INIT_SIZE];
}				t_query_map;

// Struct to represent the ecs universe
typedef struct s_universe
{
	t_list		*entity_pool;
	t_query_map	*queries;
	uint64_t	*masks;
	void		*data;
	size_t		mem_offsets[64];
	size_t		mem_sizes[64];
	size_t		mem_tsize;
	uint32_t	entity_len;
	uint32_t	entity_cap;
	uint8_t		nb_comps;
}				t_universe;

t_universe		*ecs_create(size_t comps[64], size_t nb_comps);
void			ecs_destroy(t_universe *ecs);
t_list			*ecs_query(t_universe *ecs, uint64_t signature);

uint32_t		ecs_entity_create(t_universe *ecs);
uint32_t		ecs_entity_clone(t_universe *ecs, uint32_t id);
void			ecs_entity_kill(t_universe *ecs, uint32_t id);
void			*ecs_entity_get(t_universe *ecs, uint32_t id, uint8_t comp);
void			ecs_entity_remove(t_universe *ecs, uint32_t id, uint8_t comp);
void			ecs_entity_add(t_universe *ecs, uint32_t id, uint8_t comp,
					void *data);
bool			ecs_entity_has(t_universe *ecs, uint32_t id, uint8_t comp);

t_query_map		*qm_create(void);
void			qm_destroy(t_query_map *map);
t_map_entry		*qm_get(t_query_map *map, uint64_t key, bool *res);
bool			qm_is_inquery(uint64_t key, uint64_t mask);

# ifndef DNDEBUG
uint32_t	__assert_entity_create(t_universe *ecs);
uint32_t	__assert_entity_clone(t_universe *ecs, uint32_t id);
void		__assert_entity_kill(t_universe *ecs, uint32_t id);
void		*__assert_entity_get(t_universe *ecs, uint32_t id, uint8_t comp);
void		__assert_entity_remove(t_universe *ecs, uint32_t id, uint8_t comp);
void		__assert_entity_add(t_universe *ecs, uint32_t id, uint8_t comp, void *data);
bool		__assert_entity_has(t_universe *ecs, uint32_t id, uint8_t comp);
t_list		*__assert_query(t_universe *ecs, uint64_t signature);

#  define ecs_query(ecs, sig) __assert_query(ecs, sig)
#  define ecs_entity_create(ecs) __assert_entity_create(ecs)
#  define ecs_entity_kill(ecs, id) __assert_entity_kill(ecs, id)
#  define ecs_entity_clone(ecs, id) __assert_entity_clone(ecs, id)
#  define ecs_entity_get(ecs, id, comp) __assert_entity_get(ecs, id, comp)
#  define ecs_entity_remove(ecs, id, comp) __assert_entity_remove(ecs, id, comp)
#  define ecs_entity_add(ecs, id, comp, data) __assert_entity_add(ecs, id, comp,data)
#  define ecs_entity_has(ecs, id, comp) __assert_entity_has(ecs, id, comp)
# endif

#endif
