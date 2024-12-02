/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:06:16 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/02 20:04:21 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "uint_list.h"
#include "utils.h"

bool	list_create(t_ecs_ulist *lst)
{
	if (__builtin_expect(!lst, 0))
		return (false);
	lst->len = 0;
	lst->cap = ECS_ULIST_INIT_CAP;
	lst->values = ft_calloc(sizeof(uint32_t), lst->cap);
	return (lst->values != NULL);
}

void	list_destroy(t_ecs_ulist *lst)
{
	if (__builtin_expect(!lst, 0))
		return ;
	free(lst->values);
	lst->values = NULL;
	lst->len = 0;
	lst->cap = 0;
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
	if (i < lst->len)
		lst->values[i] = lst->values[--lst->len];
}
