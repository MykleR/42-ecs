/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list_alter.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 17:06:20 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 21:24:04 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"

static void	list_fuse_toleft(t_ecs_ulist *lst)
{
	t_ecs_ulist	*tmp;

	lst->end = lst->next->end;
	tmp = NULL;
	if (lst->next)
		tmp = lst->next->next;
	list_delone(&(lst->next));
	lst->next = tmp;
}

static void	list_pophead(t_ecs_ulist **lst)
{
	t_ecs_ulist	*tmp;

	tmp = (*lst)->next;
	free(*lst);
	*lst = tmp;
}

static void	list_split_toright(t_ecs_ulist *lst, uint32_t val)
{
	t_ecs_ulist	*next;

	next = list_create(val + 1, lst->end, lst->next);
	lst->next = next;
	lst->end = val - 1;
}

void	list_insert(t_ecs_ulist **lst, uint32_t val)
{
	t_ecs_ulist	*node;

	if (!lst || *lst == NULL || ((*lst)->start && val < (*lst)->start - 1))
	{
		if (__builtin_expect(lst != NULL, 1))
			*lst = list_create(val, val, *lst);
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
		if (!node->next || (val < node->next->start - 1))
			node->next = list_create(val, val, node->next);
		node = node->next;
	}
}

void	list_remove(t_ecs_ulist **lst, uint32_t val)
{
	t_ecs_ulist	*tmp;
	t_ecs_ulist	*node;

	if (__builtin_expect(lst == NULL, 0))
		return ;
	if (val == (*lst)->start && val == (*lst)->end)
		return (list_pophead(lst));
	node = *lst;
	while (node)
	{
		if (node->next && val == node->next->start && val == node->next->end)
		{
			tmp = node->next->next;
			list_delone(&(node->next));
			node->next = tmp;
			return ;
		}
		if (val > node->start && val < node->end)
			return (list_split_toright(node, val));
		node->start += (val == node->start);
		node->end -= (val == node->end);
		if (node->start == val + 1 || node->end == val - 1)
			return ;
		node = node->next;
	}
}
