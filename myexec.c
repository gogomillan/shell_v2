#include "hsh.h"

int _test_cmd(char *sentence, char **av, char **env, size_t *execnt, int *fd);
char *_path_cmd(char **argv, lenv_s **lenv, char *pathos);
int _cmdln(char *line, char **ml, char **tm, char ***ar, int *ac, char **av);

/**
 * myexec - Exececutes a command
 * @argv: The end NULL list of parameters
 * @lenv: The variables from the environment list
 * @execnt: the command line counter
 * @fd: the file descriptor for output
 * @cmd2: Second sentence from command line
 * Return: Always 0
 */
int myexec(char **argv, lenv_s **lenv, size_t *execnt, int *fd, char *cmd2)
{
	pid_t pid1, pid2;
	int argc, j, status, ret = 0, es = 0; /* ret = return, es = exit status */
	char *sentence = NULL, *pathos, **env = menv(lenv);
	char *sntc = NULL, *myline, *tmp, **av2;

	pathos = _getenv("PATH", lenv);						/* Get the PATH */
	/* Make sentence with path */
	sentence = _path_cmd(argv, lenv, pathos);			/* Full sentence */
	ret = _test_cmd(sentence, argv, env, execnt, fd);	/* Is correct? */
	if (ret != NO_OTHER)
		return (ret);
	/* If a second sentence from command line */
	if (fd[READ_END] != CLOSED && cmd2 != NULL)
	{
		j = _cmdln(cmd2, &myline, &tmp, &av2, &argc, argv);	/* Make stack for execv*/
		for (argc = 0; av2[argc] != NULL; argc++)
			printf("{%s}", av2[argc]);
		printf("\n");
		sntc = _path_cmd(av2, lenv, pathos);				/* Full sentence */
		ret = _test_cmd(sntc, av2, env, execnt, fd);		/* Is correct? */
		if (ret != NO_OTHER)
			return (ret);
	}
	/* gogomillan - printf("To fork -> [%s][%s]\n", sentence, cmd2); */
	/* Create a child process */
	pid1 = fork();
	if (pid1 == -1)					/* If any error from fork */
	{	perror("Error:");
		return (1);
	}
	else if (pid1 == 0)				/* Execute the command in the child */
	{
		if (fd[READ_END] != CLOSED && cmd2 != NULL)	/* There is a second sentence */
		{	_dup(fd[READ_END], STDIN_FILENO);
			if (execve(sntc, (av2 + 1), env) == -1)
				exit(127);
		}
		else
		{	ret = (fd[WRITE_END] != CLOSED) ? _dup(fd[WRITE_END], fd[STDIN_OUT]) : ret;
			if (execve(sentence, (argv + 1), env) == -1)
				exit(127);
		}
	} else							/* The parent wait for the child */
	{
		if (fd[READ_END] != CLOSED && cmd2 != NULL)	/* There is a second sentence */
		{	pid2 = fork();					/* Create a child process */
			if (pid2 == -1)					/* If any error from fork */
			{	perror("Error:");
				return (1);
			}
			else if (pid1 == 0)				/* Execute the command in the child */
			{	_dup(fd[WRITE_END], STDOUT_FILENO);
				if (execve(sentence, (argv + 1), env) == -1)
					exit(127);
			} else
			{	/* Wait for the writter child */
				wait(&status);
				if (WIFEXITED(status))
					es = WEXITSTATUS(status);
				/* Close both sides of pipe to release the consumer process */
				close(fd[READ_END]), close(fd[WRITE_END]);
				/* Wait for the reader child */
				wait(&status);
				if (WIFEXITED(status))
					es = WEXITSTATUS(status);
			}
		} else
		{	wait(&status);
			if (WIFEXITED(status))
				es = WEXITSTATUS(status);
		}
	}
	free(sentence), free(env);
	if (fd[READ_END] != CLOSED && cmd2 != NULL)
		free(av2), free(tmp), free(myline);
	(fd[LT2_OUT] != CLOSED) ? unlink(TMP_FILE) : j;
	fd[LT2_OUT] = fd[READ_END] = fd[WRITE_END] = CLOSED;
	return (es);
}

/**
 * _path_cmd - Find a path for the command
 * @argv: The double pounter to the arguments to the execv
 * @lenv: The pointer to the environment structure
 * @pathos: The path from OS
 * Return: The pointer to the sentence
 */
char *_path_cmd(char **argv, lenv_s **lenv, char *pathos)
{
	char *sentence = NULL, *tmp = NULL;

	if (_strncmp(pathos, ":", 1) == 0 || pathos == NULL) /* if bad PATH */
	{
		tmp = _strdup(argv[1]), sentence = tmp;
		if (access(sentence, F_OK | R_OK | X_OK) == ERROR)
			free(tmp), sentence = NULL;
	}
	else											/* Correct PATH */
		sentence = path(argv[1], lenv);

	return (sentence);
}

/**
 * _test_cmd - Verify the command
 * @sentence: The command sentence
 * @argv: The cmmand arguments
 * @env: The environment
 * @execnt: The execution command line counter
 * @fd: The file descriptors
 * Return: Exit code on error or NO_OTHER
 */
int _test_cmd(char *sentence, char **argv, char **env, size_t *execnt, int *fd)
{
	char msg[160];
	int isnull = FALSE;
	struct stat st;

	/* Evaluate if there is a sentence */
	st.st_mode = 0;
	if (sentence != NULL)
		stat(sentence, &st);
	else
		isnull = TRUE;
	/* Verify the command isn't a directory */
	if (isnull == FALSE && ((st.st_mode & S_IFMT) == S_IFDIR))
	{
		sprintf(msg, "%s: %ld: %s: Permission denied\n", argv[0], *execnt, argv[1]);
		write(STDERR_FILENO, &msg, _strlen(msg));
		free(env), free(sentence);
		return (126);
	}
	/* If no sentence */
	if (sentence == NULL)
	{
		sprintf(msg, "%s: %ld: %s: not found\n", argv[0], *execnt, argv[1]);
		write(STDERR_FILENO, &msg, _strlen(msg));
		free(env);
		if (*(fd + WRITE_END) == CLOSED)
			return (127);
		else
			return (0);
	}
	/* no problem */
	return (NO_OTHER);
}
