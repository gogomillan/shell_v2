#include "hsh.h"

int _test_cmd(char *sentence, char **av, lenv_s **lenv, size_t *execnt);
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
	int argc, j, status = 0, ret = 0, ret1 = 0, es = 0; /* es = exit status */
	char *sentence = NULL, *pathos, **env = menv(lenv);
	char *sntc = NULL, *myline, *tmp, **av2, buf[10];

	pathos = _getenv("PATH", lenv);						/* Get the PATH */
	/* Make sentence with path */
	sentence = _path_cmd(argv, lenv, pathos);			/* Full sentence */
	ret = _test_cmd(sentence, argv, lenv, execnt);		/* Is correct? */
	if (fd[OPER] != PIPE && fd[OPER] != OR && fd[OPER] != SC)
		if (ret != NO_OTHER)
		{	free(env);
			return ((*(fd + WRITE_END) != CLOSED) ? 0 : ret);
		}
	/* If a second sentence from command line */
	if (fd[OPER] == SC || fd[OPER] == PIPE || fd[OPER] == AND || fd[OPER] == OR)
	{
		j = _cmdln(cmd2, &myline, &tmp, &av2, &argc, argv);	/* Make stack for execv*/
		sntc = _path_cmd(av2, lenv, pathos);				/* Full sentence */
		ret1 = _test_cmd(sntc, av2, lenv, execnt);			/* Is correct? */
		if (ret1 != NO_OTHER && fd[OPER] != OR && fd[OPER] != SC)
		{	free(env), free(sntc), free(myline), free(tmp), free(av2), free(sentence);
			return (ret1);
		}
	}
	if (fd[OPER] == OR)		/* Exit code when both sides have error */
		if (ret == 127 && ret1 == 127)
			return (127);
	/* Create a child process */
	pid1 = fork();
	if (pid1 == ERROR)					/* If any error from fork */
	{	perror("Error:");
		return (1);
	}
	else if (pid1 == 0)				/* Execute the command in the child */
	{
		/* If there is a second sentence */
		if (fd[OPER] == PIPE || fd[OPER] == SC || fd[OPER] == OR || fd[OPER] == AND)
		{
			/**
			 * This point to evaluate if there is a next command
			 * If there is then
			 * - Create a pipe
			 * - Fork
			 * - Call the myexec for that command and next if exist (Resursive)
			 * Wait for the child and exit
			 */
			close(fd[WRITE_END]);
			if (fd[OPER] == PIPE && fd[READ_END] != CLOSED && cmd2 != NULL)	/* PIPE */
				_dup(fd[READ_END], STDIN_FILENO);			/* redir stdin */
			else															/* Others */
			{	read(fd[READ_END], buf, 2), buf[2] = '\0';	/* Read msg */
				close(fd[READ_END]);
				if (_strcmp(buf, "NO") == 0 || ret1 != NO_OTHER)
					sntc = "/bin/true", *(av2 + 1) = "true", *(av2 + 2) = NULL;
			}
			if (execve(sntc, (av2 + 1), env) == ERROR)	/* Exec the second sentence */
				exit(127);
		} else		/* No second sentence */
		{	ret = (fd[WRITE_END] != CLOSED) ? _dup(fd[WRITE_END], fd[STDIN_OUT]) : ret;
			if (execve(sentence, (argv + 1), env) == ERROR)
				exit(127);
		}
	} else							/* The parent wait for the child */
	{
		/* If there is a second sentence */
		if (fd[OPER] == PIPE || fd[OPER] == SC || fd[OPER] == OR || fd[OPER] == AND)
		{	pid2 = fork();					/* Create a child process */
			if (pid2 == ERROR)					/* If any error from fork */
			{	perror("Error:");
				return (1);
			}
			else if (pid2 == 0)				/* Execute the command in the child */
			{	close(fd[READ_END]);
				if (fd[OPER] == PIPE && fd[READ_END] != CLOSED && cmd2 != NULL) /* PIPE */
					_dup(fd[WRITE_END], STDOUT_FILENO);
				if (ret != NO_OTHER)
					sentence = "/bin/false", *(av2 + 1) = "false", *(av2 + 2) = NULL;
				if (execve(sentence, (argv + 1), env) == ERROR)
					exit(127);
			} else
			{	/* Wait for the writter child */
				wait(&status);
				if (WIFEXITED(status))
					es = WEXITSTATUS(status);
				if (fd[OPER] != PIPE)	/* Msg for first child */
				{
					if (fd[OPER] == OR && es == 0)
						write(fd[WRITE_END], "NO", 2);
					else
						write(fd[WRITE_END], "GO", 2);
				}
				/* Close both sides of pipe to release the consumer process */
				close(fd[READ_END]), close(fd[WRITE_END]);
				/* Wait for the reader child */
				wait(&status);
				if (WIFEXITED(status))
					es = WEXITSTATUS(status);
			}
		} else		/* No second sentence */
		{
			wait(&status);
			if (WIFEXITED(status))
				es = WEXITSTATUS(status);
		}
	}
	free(sentence), free(env);
	if (fd[READ_END] != CLOSED && cmd2 != NULL)
	{
		free(av2), free(tmp), free(myline), free(sntc);
	}
	(fd[LT2_OUT] != CLOSED) ? unlink(TMP_FILE) : j;
	fd[LT2_OUT] = fd[READ_END] = fd[WRITE_END] = CLOSED;
	fd[OPER] = FALSE;
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

	if (argv == NULL)
		return (NULL);
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
 * @lenv: The environment
 * @execnt: The execution command line counter
 * Return: Exit code on error or NO_OTHER
 */
int _test_cmd(char *sentence, char **argv, lenv_s **lenv, size_t *execnt)
{
	char msg[160], **env = menv(lenv);
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
		return (127);
	}
	/* no problem */
	free(env);
	return (NO_OTHER);
}
