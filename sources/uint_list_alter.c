/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list_alter.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 17:06:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 17:00:53 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"

static void	list_delone(t_ecs_ulist **lst)
{
	t_ecs_ulist	*node;

	if (__builtin_expect(!lst || !(*lst), 0))
		return ;
	node = *lst;
	if (node->next)
		node->next->prev = node->prev;
	if (node->prev)
		node->prev->next = node->next;
	*lst = NULL;
	free(node);
}

static void	list_fuse_toleft(t_ecs_ulist *lst)
{
	t_ecs_ulist	*tmp;

	if (__builtin_expect(!lst->next, 0))
		return ;
	lst->end = lst->next->end;
	tmp = lst->next;
	list_delone(&tmp);
}

static void	list_split_toright(t_ecs_ulist *lst, uint32_t val)
{
	t_ecs_ulist	*next;

	next = list_create(val + 1, lst, lst->next);
	next->end = lst->end;
	lst->end = val - 1;
}

void	list_insert(t_ecs_ulist **lst, uint32_t val)
{
	t_ecs_ulist	*node;

	if (__builtin_expect(lst == NULL, 0))
		return ;
	if (*lst == NULL || ((*lst)->start && val < (*lst)->start - 1))
	{
		*lst = list_create(val, NULL, *lst);
		return ;
	}
	node = *lst;
	while (node && (val < node->start || val > node->end))
	{
		node->start -= (val == node->start - 1);
		node->end += (val == node->end + 1);
		if (val >= node->start && val <= node->end)
		{
			if (node->next && node->next->start == node->end + 1)
				list_fuse_toleft(node);
			return ;
		}
		if (!node->next)
			node->next = list_create(val, node, NULL);
		node = node->next;
	}
}

void	list_remove(t_ecs_ulist **lst, uint32_t val)
{
	t_ecs_ulist	*node;

	if (__builtin_expect(lst == NULL, 0))
		return ;
	node = *lst;
	while (node)
	{
		if (val == node->start && val == node->end)
		{
			if (node == *lst)
				*lst = node->next;
			list_delone(&node);
			return ;
		}
		if (val > node->start && val < node->end)
		{
			list_split_toright(node, val);
			return ;
		}
		node->start += (val == node->start);
		node->end -= (val == node->end);
		if (node->start == val + 1 || node->end == val - 1)
			return ;
		node = node->next;
	}
}
