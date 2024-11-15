/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/15 17:56:50 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"

t_list	*list_create(uint32_t val, t_list *prev, t_list *next)
{
	t_list	*result;

	result = malloc(sizeof(t_list));
	if (__builtin_expect(result == NULL , 0))
		return (NULL);
	result->start = val;
	result->end = val;
	if (next)
		next->prev = result;
	result->next = next;
	if (prev)
		prev->next = result;
	result->prev = prev;
	return (result);
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

uint32_t	list_iter(t_list *lst)
{
	static t_list	*node = NULL;
	static uint32_t index = 0;

	if (!lst)
		return (UINT32_MAX);
	if (!node)
	{
		node = lst;
		index = node->start;
	}
	if (index > node->end)
	{
		node = node->next;
		if (!node)
			return (UINT32_MAX);
		index = node->start;
	}
	return (index++);
}

uint32_t	list_popfront(t_list **lst)
{
	uint32_t	val;

	if (__builtin_expect(!lst || !(*lst), 0))
		return (-1);
	val = (*lst)->start;
	list_remove(lst, val);
	return (val);
}
