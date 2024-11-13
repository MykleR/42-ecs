/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:03:10 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/12 21:14:27 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UINT_LIST_H
# define UINT_LIST_H

# include <stdlib.h>
# include <stdint.h>

typedef struct s_list
{
	struct s_list	*next;
	struct s_list	*prev;
	uint32_t		val;
}					t_list;

void				list_addfront(t_list **lst, uint32_t val);
void				list_clear(t_list **lst);
void				list_remove(t_list **lst, uint32_t val);
uint32_t			list_popfront(t_list **lst);

#endif
