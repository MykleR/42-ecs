/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecs_extra.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:08:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/20 16:00:17 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ecs_extra.h"
#include "utils.h"

bool	ecs_queue_create(t_ecs_queue *queue)
{
	if (__builtin_expect(!queue, 0))
		return (false);
	queue->cap = ECS_QUEUE_INIT_CAP;
	queue->len = 0;
	queue->pending = ft_calloc(sizeof(t_ecs_queue_entry), queue->cap);
	return (queue->pending != NULL);
}

void	ecs_queue_destroy(t_ecs_queue *queue)
{
	if (!queue)
		return ;
	free(queue->pending);
	queue->pending = NULL;
	queue->len = 0;
	queue->cap = 0;
}

void	ecs_queue_add(t_ecs_queue *queue, t_ecs_queue_entry info)
{
	if (__builtin_expect(!queue || !queue->pending
			|| queue->len == UINT32_MAX, 0))
		return ;
	if (__builtin_expect(queue->len >= queue->cap, 0))
	{
		if ((queue->cap << 1) >> 1 != queue->cap)
			return ;
		queue->pending = ft_realloc(queue->pending,
				queue->cap * sizeof(t_ecs_queue_entry),
				(queue->cap << 1) * sizeof(t_ecs_queue_entry));
		if (!queue->pending)
			return ;
		queue->cap <<= 1;
	}
	*(queue->pending + queue->len) = info;
	queue->len++;
}

void	ecs_queue_process(t_ecs *ecs, t_ecs_queue *queue)
{
	t_ecs_queue_entry	entry;

	if (__builtin_expect(!ecs || !queue, 0))
		return ;
	if (!queue->len)
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
	queue->len = 0;
}
