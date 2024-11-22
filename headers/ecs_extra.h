/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_extra.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:00:52 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/22 11:24:27 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ECS_EXTRA_H
# define ECS_EXTRA_H

# include "ecs.h"

typedef enum e_ecs_alteration
{
	ADD,
	CLONE,
	KILL,
	REMOVE,
}	t_ecs_alteration;

typedef struct s_ecs_queue_payload
{
	void		*data;
	uint32_t	id;
	uint8_t		comp;		
}				t_ecs_queue_payload;

typedef struct s_ecs_queue
{
	struct s_ecs_queue	*next;
	t_ecs_queue_payload	payload;
	t_ecs_alteration	type;
}	t_ecs_queue;

void	ecs_queue_process(t_ecs *ecs, t_ecs_queue **queue);
void	ecs_queue_add(t_ecs_queue **queue, t_ecs_alteration type,
			t_ecs_queue_payload p);
void	ecs_queue_destroy(t_ecs_queue **queue);

#endif
