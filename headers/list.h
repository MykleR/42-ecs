/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   list.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:03:10 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/08 16:44:48 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIST_H
# define LIST_H

# include <stdlib.h>

typedef struct s_list
{
	void			*content;
	struct s_list	*next;
	struct s_list	*prev;
}					t_list;

t_list				*list_create(void *content);
t_list				*list_last(t_list *lst);
t_list				*list_addback(t_list *lst, t_list *node);
void				list_clear(t_list **lst, void (*del)(void *));
void				list_delone(t_list	*lst, void (*del)(void*));
t_list				*list_find(t_list *lst, void *content);

#endif
