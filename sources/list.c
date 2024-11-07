/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/07 14:14:02 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "list.h"

t_list	*list_create(void *content)
{
	t_list	*result;

	result = malloc(sizeof(t_list));
	if (!result)
		return (0);
	result->content = content;
	result->next = NULL;
	return (result);
}

t_list	*list_last(t_list *lst)
{
	t_list	*last;

	last = NULL;
	while (lst)
	{
		last = lst;
		lst = lst->next;
	}
	return (last);
}

t_list	*list_addback(t_list *lst, t_list *new)
{
	if (lst)
		list_last(lst)->next = new;
	return (lst);
}

void	list_clear(t_list **lst, void (*del)(void*))
{
	t_list	*next;

	if (!lst)
		return ;
	while (*lst)
	{
		next = (*lst)->next;
		if (del)
			del((*lst)->content);
		free(*lst);
		*lst = next;
	}
}

void	list_iter(t_list *lst, void (*f)(void *))
{
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}
