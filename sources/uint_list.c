/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/12 21:57:57 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"

static t_list	*list_create(uint32_t val, t_list *prev, t_list *next)
{
	t_list	*result;

	result = malloc(sizeof(t_list));
	if (__builtin_expect(result == NULL , 0))
		return (NULL);
	result->val = val;
	result->next = next;
	result->prev = prev;
	return (result);
}

void	list_addfront(t_list **lst, uint32_t val)
{
	t_list	*node;

	if (__builtin_expect(lst == NULL, 0))
		return ;
	if (*lst == NULL)
	{
		*lst = list_create(val, NULL, NULL);
		return ;
	}
	node = list_create(val, NULL, *lst);
	(*lst)->prev = node;
	*lst = node;
}

void	list_clear(t_list **lst)
{
	t_list	*next;

	if (__builtin_expect(lst == NULL, 0))
		return ;
	while (*lst)
	{
		next = (*lst)->next;
		free(*lst);
		*lst = next;
	}
}

void	list_remove(t_list **lst, uint32_t val)
{
	t_list	*node;

	if (__builtin_expect(lst == NULL || *lst == NULL, 0))
		return ;
	node = *lst;
	while (node->val != val)
		node = node->next;
	if (!node)
		return ;
	if (node == *lst)
		*lst = node->next;
	if (node->next)
		node->next->prev = node->prev;
	if (node->prev)
		node->prev->next = node->next;
	free(node);
}

uint32_t	list_popfront(t_list **lst)
{
	uint32_t	val;

	if (__builtin_expect(lst == NULL, 0))
		return (-1);
	val = (*lst)->val;
	list_remove(lst, val);
	return (val);
}
