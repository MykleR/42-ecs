/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 20:18:07 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/13 13:53:17 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

size_t	next_pow2(size_t n)
{
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	return (++n);
}

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	if (__builtin_expect(!src || !dst, 0))
		return (NULL);
	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	while (n--)
		*d++ = *s++;
	return (dst);
}

void	*ft_memset(void *s, int c, size_t n)
{
	unsigned char	*mem;

	if (__builtin_expect(!s, 0))
		return (NULL);
	mem = (unsigned char *)s;
	while (n--)
		*mem++ = (unsigned char)c;
	return (s);
}

void	*ft_realloc(void *ptr, size_t old_size, size_t new_size)
{
	void	*result;

	if (__builtin_expect(!ptr, 0))
		return (malloc(new_size));
	if (__builtin_expect(old_size >= new_size, 0))
		return (ptr);
	result = malloc(new_size);
	if (__builtin_expect(result != NULL, 1))
		ft_memcpy(result, ptr, old_size);
	free(ptr);
	return (result);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*result;
	size_t	fsize;

	fsize = size * nmemb;
	if (__builtin_expect(nmemb && fsize / nmemb != size, 0))
		return (NULL);
	result = malloc(fsize);
	if (__builtin_expect(result != NULL, 1))
		ft_memset(result, 0, fsize);
	return (result);
}
