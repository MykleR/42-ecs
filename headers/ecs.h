/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/05 13:52:19 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 13:40:35 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_H
# define ECS_H

# include "hash_map.h"
# include "binary_tree.h"
# include "utils.h"
# include "list.h"

# define ECS_INIT_CAP 1024

// Struct to represent an entity
// components	: byte mask for checking components
// alive		: flag to indicate if entity is used
// id			: entity id to retreive components data
typedef struct s_entity
{
	uint64_t	components;
	uint32_t	alive;
	uint32_t	id;
}__attribute__((packed))	t_entity;

// Struct to represent compoments holder for universe
// total_size	: the total size of data memory access
// nb_comps		: how many components
// sizes		: memory size foreach components
// offsets		: how much you need to offset for mem acces foreach components
// data			: data array holding data foreach entities foreach components
typedef struct s_comps
{
	size_t		total_size;
	uint8_t		nb_comps;
	size_t		sizes[64];
	size_t		offsets[64];
	void		*data;
}__attribute__((packed))	t_comps;

// Strut to represent the ecs universe
typedef struct s_universe
{
	t_list		*queries;
	t_entity	*entities;
	t_comps		components;
}	t_universe;

// Struct to represent a query
// signature	: the byte mask for verifying entities components
// entities		: all entities that satisfy the query signature
typedef struct s_query
{
	uint64_t	signature;
	t_list		*entities;
}__attribute__((packed))	t_query;

typedef void	(*t_system_call)(t_universe *ecs, t_query query);
// Struct to represent a system
// query	: the pointer to the needed query
// caller	: the function pointer to call the system
typedef struct s_system
{
	t_query			*query;
	t_universe		*ecs;
	t_system_call	caller;
}__attribute__((packed))	t_system;

t_universe			*ecs_create(size_t comps[64], size_t nb_comps);
void				ecs_destroy(t_universe *ecs);
t_query				*ecs_query(t_universe *ecs, uint64_t signature);

void				entity_has(t_universe *ecs, t_entity ent,
						uint8_t comp);
void				entity_remove(t_universe *ecs, t_entity ent,
						uint8_t comp);
void				entity_add(t_universe *ecs, t_entity ent,
						uint8_t comp, void *data);
void				*entity_get(t_universe *ecs, t_entity ent,
						uint8_t comp);
void				entity_kill(t_universe *ecs, t_entity ent);
bool				entity_create(t_universe *ecs, t_entity *ent);

#endif
