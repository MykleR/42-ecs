/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_extra.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:52 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/20 15:59:29 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_EXTRA_H
# define ECS_EXTRA_H

# include "ecs.h"

# define ECS_QUEUE_INIT_CAP 64

typedef enum e_ecs_alteration
{
	ADD,
	CLONE,
	KILL,
	REMOVE,
}	t_ecs_alteration;

typedef struct s_ecs_queue_entry
{
	void				*data;
	uint32_t			id;
	uint8_t				comp;
	t_ecs_alteration	type;
}				t_ecs_queue_entry;

typedef struct s_ecs_queue
{
	uint32_t			len;
	uint32_t			cap;
	t_ecs_queue_entry	*pending;
}	t_ecs_queue;

bool	ecs_queue_create(t_ecs_queue *queue);
void	ecs_queue_destroy(t_ecs_queue *queue);
void	ecs_queue_process(t_ecs *ecs, t_ecs_queue *queue);
void	ecs_queue_add(t_ecs_queue *queue, t_ecs_queue_entry info);
#endif
