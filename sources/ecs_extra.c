/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_extra.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:08:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/26 15:36:27 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs_extra.h"

void	ecs_queue_create(t_ecs_queue *queue, uint32_t cap)
{
	if (!queue || !cap)
		return ;
	queue->cap = cap;
	queue->len = 0;
	queue->pending = malloc(sizeof(t_ecs_queue_entry) * cap);
}

void	ecs_queue_destroy(t_ecs_queue *queue)
{
	if (!queue)
		return ;
	free(queue->pending);
	queue->pending = NULL;
}

void	ecs_queue_add(t_ecs_queue *queue, t_ecs_queue_entry info)
{
	uint32_t	new_cap;

	new_cap = queue->cap << 1;
	if (__builtin_expect(!queue || new_cap >> 1 != queue->cap, 0))
		return ;
	if (__builtin_expect(queue->len >= queue->cap, 0))
	{
		queue->pending = ft_realloc(queue->pending, queue->cap, new_cap);
		queue->cap = new_cap;
	}
	*(queue->pending + queue->len) = info;
	queue->len++;
}

void	ecs_qeue_process(t_ecs *ecs, t_ecs_queue *queue)
{
	t_ecs_queue_entry	entry;

	if (__builtin_expect(!ecs || !queue || !queue->len, 0))
		return ;
	while (queue->len--)
	{
		entry = *(queue->pending + queue->len);
		if (entry.type == ADD)
			ecs_entity_add(ecs, entry.id, entry.comp, entry.data);
		else if (entry.type == CLONE)
			ecs_entity_clone(ecs, entry.id);
		else if (entry.type == KILL)
			ecs_entity_kill(ecs, entry.id);
		else if (entry.type == REMOVE)
			ecs_entity_remove(ecs, entry.id, entry.comp);
	}
}
