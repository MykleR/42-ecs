/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/21 23:16:05 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"
#include "utils.h"

t_ecs_ulist	*list_create(uint32_t cap)
{
	t_ecs_ulist	*lst;

	lst = malloc(sizeof(t_ecs_ulist));
	if (__builtin_expect(!lst, 0))
		return (NULL);
	lst->cap = cap;
	lst->len = 0;
	lst->values = ft_calloc(sizeof(uint32_t), cap);
	if (lst->values)
		return (lst);
	free(lst);
	return (NULL);
}

void	list_destroy(t_ecs_ulist *lst)
{
	if (__builtin_expect(!lst, 0))
		return ;
	free(lst->values);
	free(lst);
}

uint32_t	list_popfront(t_ecs_ulist *lst)
{
	uint32_t	val;

	if (__builtin_expect(!lst || !lst->len, 0))
		return (0);
	lst->len--;
	val = *(lst->values);
	*(lst->values) = *(lst->values + lst->len);
	return (val);
}

void	list_add(t_ecs_ulist *lst, uint32_t id)
{
	if (__builtin_expect(!lst, 0))
		return ;
	if (__builtin_expect(lst->len >= lst->cap, 0))
	{
		lst->values = ft_realloc(lst->values, lst->cap * sizeof(uint32_t),
				lst->cap * sizeof(uint32_t) << 1);
		lst->cap <<= 1;
	}
	lst->values[lst->len++] = id;
}

void	list_remove(t_ecs_ulist *lst, uint32_t id)
{
	uint32_t	i;

	if (__builtin_expect(!lst || !lst->len, 0))
		return ;
	i = 0;
	while (i < lst->len && lst->values[i] != id)
		i++;
	if (i == lst->len)
		return ;
	lst->values[i] = lst->values[--lst->len];
}
