/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/18 15:57:19 by mrouves          ###   ########.fr       */
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

t_list	*list_iter(t_list *lst, uint32_t *prev)
{
	static uint32_t	index = UINT32_MAX;

	if (__builtin_expect(prev == NULL, 0))
		return (NULL);
	if (!lst)
	{
		index = UINT32_MAX;
		*prev = index;
		return (NULL);
	}
	if (index == UINT32_MAX)
		index = lst->start;
	else
		index++;
	if (index > lst->end)
	{
		lst = lst->next;
		if (lst)
			index = lst->start;
		else
			index = UINT32_MAX;
	}
	*prev = index;
	return (lst);
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
