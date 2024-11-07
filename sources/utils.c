/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mrouves <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 20:18:07 by mrouves           #+#    #+#             */
/*   Updated: 2024/11/07 16:33:45 by mrouves          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	unsigned char	*d;
	unsigned char	*s;

	if (!dst && !src)
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
	unsigned char	val;

	mem = (unsigned char *)s;
	val = (unsigned char)c;
	while (n--)
		*mem++ = val;
	return (s);
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*result;
	size_t	fsize;

	fsize = size * nmemb;
	if (nmemb && fsize / nmemb != size)
		return (NULL);
	result = malloc(fsize);
	if (result)
		ft_memset(result, 0, fsize);
	return (result);
}
