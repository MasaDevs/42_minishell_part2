/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   make_lstenvp.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Marai <MasaDevs@gmail.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/14 09:09:09 by keys              #+#    #+#             */
/*   Updated: 2023/02/27 20:41:06 by Marai            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

size_t	env_len(char *envp)
{
	size_t	len;

	len = 0;
	while (envp[len] != '=')
		len++;
	return (len);
}

void	add_env(t_env **env, char *envp, size_t len)
{
	(*env)->key = ft_substr(envp, 0, len);
	if (!env)
		exit(1);
}

void	add_value(t_env **env, char *envp, size_t len)
{
	(*env)->value = ft_substr(envp, len + 1, strlen(&envp[len + 1]));
	if (!(*env)->value)
		exit(1);
}

t_env	*new_lstenv(char *envp)
{
	t_env	*env;
	size_t	len;

	env = malloc(sizeof(t_env));
	if (!env)
		exit(1);
	len = env_len(envp);
	add_env(&env, envp, len);
	add_value(&env, envp, len);
	env->prev = NULL;
	env->next = NULL;
	return (env);
}

t_env	*env_last(t_env *env)
{
	t_env	*tmp;

	tmp = env;
	while (tmp->next)
		tmp = tmp->next;
	return (tmp);
}

void	ft_env_addback(t_env **env, t_env *new)
{
	t_env	*last;

	if (env)
	{
		if (*env)
		{
			last = env_last(*env);
			last->next = new;
			new->prev = last;
		}
		else
			*env = new;
	}
}

void	print_env(t_env *env)
{
	ssize_t num;

	num = 1;
	while (1)
	{
		if (env == NULL)
			break ;
		printf("number %ld\nkey: %s\n value: %s\n\n\n", num, env->key, env->value);
		env = env->next;
		num++;
	}
}

void	make_lstenv(t_env **s_env, char **envp)
{
	int		i;
	t_env	*new;

	i = 0;
	while (envp[i])
	{
		new = new_lstenv(envp[i]);
		ft_env_addback(s_env, new);
		i++;
	}
	print_env(*s_env);
}
