/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_tree.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 15:27:23 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 17:52:48 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BINARY_TREE_H
# define BINARY_TREE_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>

typedef struct s_binary_tree
{
	uint64_t				key;
	void					*data;
	struct s_binary_tree	*left;
	struct s_binary_tree	*right;
}	t_binary_tree;

t_binary_tree	*btree_create(uint64_t key, void *data);
void			btree_destroy(t_binary_tree *btree, void (*del)(void *));
void			*btree_search(t_binary_tree *btree, uint64_t key);
t_binary_tree	*btree_insert(t_binary_tree *btree, uint64_t key, void *data);
t_binary_tree	*btree_remove(t_binary_tree *btree, uint64_t key, void (*del)(void *));
bool			btree_isleaf(t_binary_tree *btree);

#endif
