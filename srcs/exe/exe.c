/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Marai <MasaDevs@gmail.com>                 +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/06 14:58:36 by keys              #+#    #+#             */
/*   Updated: 2023/02/27 19:58:04 by marai            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <err.h>
#include <errno.h>

#define WRITE 1
#define READ 0

int	exec_si(t_node *node)
{
	char		**argv;
	extern char	**environ;
	pid_t		pid;
	char		*cmd_path;
	int			waitstatus;

	if (node == NULL)
		return (-1);
	argv = make_arr(node);
	//_redirect(node);
	//here_documents(node);
	pid = fork();
	if (pid < 0)
		_err("fork");
	else if (pid == 0)
	{
		if (access(argv[0], X_OK) == 0)
			execve(argv[0], argv, environ);
		else
		{
			cmd_path = exec_filename(argv[0]);
			if (cmd_path != NULL)
				execve(cmd_path, argv, environ);
		}
	}
	else
	{
		wait(&waitstatus);
		//restore_fd(node);
		free(argv);
		return (WEXITSTATUS(waitstatus));
	}
	return (0);
}

int	t_escape_fd(int fd)
{
	int	newfd;

	newfd = fcntl(fd, F_DUPFD, 10);
	fflush(stdout);
	if (newfd < 0)
		_err("fcntl");
	if (close(fd) < 0)
		_err("close");
	return (newfd);
}

int	exec(t_node *node, int k3)
{
	char		**argv;
	extern char	**environ;
	pid_t		pid;
	char		*cmd_path;
	int			rw[2];

	if (!node)
		return (0);
	pipe(rw);
	// _redirect(node->left);
	// here_documents(node);
	pid = fork();
	if (pid < 0)
		_err("fork");
	else if (pid == 0)
	{
		if (node->next == NULL)
		{
			close(rw[0]);
			close(rw[1]);
			dup2(k3, 1);
			close(k3);
		}
		else
		{
			close(rw[0]);
			dup2(rw[1], 1);
			close(rw[1]);
		}
		//_redirect(node);
		argv = make_arr(node);
		if (access(argv[0], X_OK) == 0)
			execve(argv[0], argv, environ);
		else
		{
			cmd_path = exec_filename(argv[0]);
			if (cmd_path != NULL)
				execve(cmd_path, argv, environ);
			else
				_err("command not found");
		}
		exit(0);
	}
	else
	{
		close(rw[1]);
		dup2(rw[0], 0);
		close(rw[0]);
	}
	// restore_fd(node);
	return (pid);
}

void	print_nodes(t_node *node)
{
	while (node)
	{
		printf("-------\n");
		printf("now %p\n", node);
		printf("next %p\n", node->next);
		node = node->next;
	}
}

int	wait_pipeline(pid_t last_pid)
{
	pid_t	wait_result;
	int		status;
	int		wstatus;

	while (1)
	{
		wait_result = wait(&wstatus);
		if (wait_result == last_pid)
			status = WEXITSTATUS(wstatus);
		else if (wait_result < 0)
		{
			if (errno == ECHILD)
				break ;
		}
	}
	return (status);
}

int	exec_tree(t_node *node)
{
	pid_t	pid;
	int		k2;
	int		k3;

	//int		status;
	k2 = fcntl(0, F_DUPFD, 10);
	k3 = fcntl(1, F_DUPFD, 10);
	add_node(node);
	while (node && node->line->type == PIPE)
		node = node->left;
	//print_nodes(node);
	while (node != NULL)
	{
		pid = exec(node, k3);
		node = node->next;
	}
	wait_pipeline(pid);
	dup2(k2, 0);
	close(k2);
	return (0);
}

int	exe_(t_node *node)
{
	if (node->line->type != PIPE)
		exec_si(node);
	else
	{
		exec_tree(node);
	}
	return (0);
}
