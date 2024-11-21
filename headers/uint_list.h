/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:03:10 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/21 23:19:03 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UINT_LIST_H
# define UINT_LIST_H

# include <stdlib.h>
# include <stdint.h>

typedef struct s_ecs_ulist
{
	uint32_t	*values;
	uint32_t	cap;
	uint32_t	len;
}					t_ecs_ulist;

t_ecs_ulist		*list_create(uint32_t cap);
void			list_add(t_ecs_ulist *lst, uint32_t id);
void			list_remove(t_ecs_ulist *lst, uint32_t id);
uint32_t		list_popfront(t_ecs_ulist *lst);
void			list_destroy(t_ecs_ulist *lst);
#endif
