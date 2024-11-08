/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:52:19 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 12:03:00 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_H
# define ECS_H

# include "assert.h"
# include "list.h"
# include "query_map.h"
# include "utils.h"

# define ECS_ENTITY_CAP 1024

// Struct to represent an entity
// components	: byte mask for checking components
// alive		: flag to indicate if entity is used
// id			: entity id to retreive components data
typedef struct s_entity
{
	uint64_t	mask;
	uint32_t	alive;
}				t_entity;

// Struct to represent compoments holder for universe
// total_size	: the total size of data memory access
// nb_comps		: how many components
// sizes		: memory size for each components
// offsets		: how much you need to offset for mem access for each components
// data			: data array holding data foreach entities for each components
typedef struct s_comps
{
	void		*data;
	size_t		total_size;
	uint8_t		nb_comps;
	size_t		sizes[64];
	size_t		offsets[64];
}				t_comps;

// Struct to represent the ecs universe
typedef struct s_universe
{
	uint64_t	len_entities;
	t_query_map	*queries;
	t_entity	*entities;
	t_comps		components;
}				t_universe;

typedef void	(*t_system_call)(t_universe *ecs);

t_universe		*ecs_create(size_t comps[64], size_t nb_comps);
void			ecs_destroy(t_universe *ecs);
t_list			*ecs_query(t_universe *ecs, uint64_t signature);

uint64_t		ecs_entity_create(t_universe *ecs);
void			ecs_entity_kill(t_universe *ecs, uint64_t id);
void			*ecs_entity_get(t_universe *ecs, uint64_t id, uint8_t comp);
void			ecs_entity_remove(t_universe *ecs, uint64_t id, uint8_t comp);
void			ecs_entity_add(t_universe *ecs, uint64_t id, uint8_t comp,
					void *data);
bool			ecs_entity_has(t_universe *ecs, uint64_t id, uint8_t comp);

# ifndef DNDEBUG
t_list			*__assert_query(t_universe *ecs, uint64_t signature);
uint64_t		__assert_entity_create(t_universe *ecs);
void			*__assert_entity_get(t_universe *ecs, uint64_t id,
					uint8_t comp);
void			__assert_entity_remove(t_universe *ecs, uint64_t id,
					uint8_t comp);
void			__assert_entity_add(t_universe *ecs, uint64_t id, uint8_t comp,
					void *data);
bool			__assert_entity_has(t_universe *ecs, uint64_t id, uint8_t comp);
#  define ecs_query(ecs, sig) __assert_query(ecs, sig)
#  define ecs_entity_create(ecs) __assert_entity_create(ecs)
#  define ecs_entity_get(ecs, id, comp) __assert_entity_get(ecs, id, comp)
#  define ecs_entity_remove(ecs, id, comp) __assert_entity_remove(ecs, id, comp)
#  define ecs_entity_add(ecs, id, comp, data) __assert_entity_add(ecs, id, comp,data)
#  define ecs_entity_has(ecs, id, comp) __assert_entity_has(ecs, id, comp)
# endif

#endif
