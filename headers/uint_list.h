/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:03:10 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/19 20:12:54 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UINT_LIST_H
# define UINT_LIST_H

# include <stdlib.h>
# include <stdint.h>

typedef struct s_ecs_ulist
{
	struct s_ecs_ulist	*next;
	uint32_t			start;
	uint32_t			end;
}					t_ecs_ulist;

t_ecs_ulist		*list_create(uint32_t start, uint32_t end, t_ecs_ulist *next);
void			list_insert(t_ecs_ulist	**lst, uint32_t val);
void			list_clear(t_ecs_ulist **lst);
void			list_remove(t_ecs_ulist **lst, uint32_t val);
uint32_t		list_popfront(t_ecs_ulist **lst);
t_ecs_ulist		*list_iter(t_ecs_ulist *lst, uint32_t	*prev);
void			list_delone(t_ecs_ulist **lst);

#endif
