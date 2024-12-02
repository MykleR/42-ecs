/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uint_list.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 19:03:10 by mrouves           #+#    #+#             */
/*   Updated: 2024/12/02 20:02:12 by mykle            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UINT_LIST_H
# define UINT_LIST_H

# include <stdlib.h>
# include <stdint.h>
# include <stdbool.h>

# define ECS_ULIST_INIT_CAP 32

typedef struct s_ecs_ulist
{
	uint32_t	*values;
	uint32_t	cap;
	uint32_t	len;
}					t_ecs_ulist;

bool			list_create(t_ecs_ulist *lst);
void			list_destroy(t_ecs_ulist *lst);
uint32_t		list_popfront(t_ecs_ulist *lst);
void			list_add(t_ecs_ulist *lst, uint32_t id);
void			list_remove(t_ecs_ulist *lst, uint32_t id);
#endif
