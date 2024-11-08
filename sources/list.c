/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 16:44:51 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "list.h"

t_list	*list_create(void *content)
{
	t_list	*result;

	result = malloc(sizeof(t_list));
	if (__builtin_expect(result == NULL , 0))
		return (NULL);
	result->content = content;
	result->next = NULL;
	result->prev = NULL;
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
	if (__builtin_expect(new != NULL, 1))
		new->prev = lst;
	if (__builtin_expect(lst != NULL, 1))
		list_last(lst)->next = new;
	return (lst);
}

void	list_delone(t_list	*lst, void (*del)(void*))
{
	if (__builtin_expect(lst == NULL, 0))
		return ;
	if (del)
		del(lst->content);
	if (lst->prev)
		lst->prev->next = lst->next;
	free(lst);
}

void	list_clear(t_list **lst, void (*del)(void*))
{
	t_list	*next;

	if (__builtin_expect(lst == NULL, 0))
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

t_list	*list_find(t_list *lst, void *content)
{
	while (lst && lst->content != content)
		lst = lst->next;
	return (lst);
}
