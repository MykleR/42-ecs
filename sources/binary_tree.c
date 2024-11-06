/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   binary_tree.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <mrouves@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 16:03:22 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/06 18:00:06 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "binary_tree.h"

t_binary_tree	*btree_create(uint64_t key, void *data)
{
	t_binary_tree	*node;

	node = malloc(sizeof(t_binary_tree));
	if (!node)
		return (NULL);
	node->key = key;
	node->data = data;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

void	btree_destroy(t_binary_tree *btree, void (*del)(void *))
{
	if (!btree)
		return ;
	btree_destroy(btree->left, del);
	btree_destroy(btree->right, del);
	if (del)
		del(btree->data);
	free(btree);
}

void	*btree_search(t_binary_tree *btree, uint64_t key)
{
	if (!btree)
		return (NULL);
	if (key < btree->key)
		return (btree_search(btree->left, key));
	if (key > btree->key)
		return (btree_search(btree->right, key));
	return (btree->data);
}

t_binary_tree	*btree_insert(t_binary_tree *btree,
	uint64_t key, void *data)
{
	if (!btree)
		return (btree_create(key, data));
	if (key > btree->key)
		btree->right = btree_insert(btree->right, key, data);
	else if (key < btree->key)
		btree->left = btree_insert(btree->left, key, data);
	return (btree);
}

bool	btree_isleaf(t_binary_tree *btree)
{
	return (btree && !btree->left && !btree->right);
}

static t_binary_tree	*btree_min(t_binary_tree *btree)
{
	if (!btree)
		return (NULL);
    if (btree->left != NULL)
        return (btree_min(btree->left));
    return (btree);
}

static t_binary_tree	*btree_reorganize(t_binary_tree *btree, void (*del)(void *))
{
	t_binary_tree	*tmp;

	tmp = NULL;
	if (btree_isleaf(btree))
		btree_destroy(btree, del);
	else if (btree->left && btree->right)
	{
		tmp = btree_min(btree->right);
		btree->key = tmp->key;
		btree->right = btree_remove(btree->right, tmp->key, del);
		tmp = btree;
    }
	else
	{
		tmp = btree->right;
        if (btree->left)
			tmp = btree->left;
		free(btree);
	}
	return (tmp);
}

t_binary_tree	*btree_remove(t_binary_tree *btree, uint64_t key,  void (*del)(void *))
{
	if (!btree)
		return (NULL);
	if (key > btree->key)
        btree->right = btree_remove(btree->right, key, del);
	else if (key < btree->key)
        btree->left = btree_remove(btree->left, key, del);
	else
		return (btree_reorganize(btree, del));
	return (btree);
}
