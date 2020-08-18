#include "hsh.h"

/**
 * myexec - Exececutes a command
 * @argc: Quantity of parameters
 * @argv: The end NULL list of parameters
 * @lenv: The variables from the environment list
 * @execnt: the command line counter
 * @fd: the file descriptor for output
 * Return: Always 0
 */
int myexec(int argc, char **argv, lenv_s **lenv, size_t *execnt, int *fd)
{
	pid_t pid;
	int status, ret = 0, es = 0, isnull = 0; /* ret = return, es = exit status */
	char msg[80], *sentence = NULL, *pathos, *tmp = NULL, **env = menv(lenv);
	struct stat st;

	st.st_mode = 0, (void) argc, pathos = _getenv("PATH", lenv);
	if (_strncmp(pathos, ":", 1) == 0 || pathos == NULL)
	{	tmp = _strdup(argv[1]), sentence = tmp;
		if (access(sentence, F_OK | R_OK | X_OK) == -1)
			free(tmp), sentence = NULL;
	}
	else
		sentence = path(argv[1], lenv);
	sentence != NULL ? stat(sentence, &st) : (isnull = 1);
	if (((st.st_mode & S_IFMT) == S_IFDIR) && isnull == 0)
	{	sprintf(msg, "%s: %ld: %s: Permission denied\n", argv[0], *execnt, argv[1]);
		write(STDERR_FILENO, &msg, _strlen(msg)), free(env), free(sentence);
		return (126);
	}
	if (sentence == NULL)
	{	sprintf(msg, "%s: %ld: %s: not found\n", argv[0], *execnt, argv[1]);
		write(STDERR_FILENO, &msg, _strlen(msg)), free(env);
		return ((*(fd + WRITE_END) == CLOSED) ? 127 : 0);
		/* return (127);  return (0); */
	} pid = fork();
	if (pid == -1)
	{	perror("Error:");
		return (1);
	}
	else if (pid == 0)
	{	ret = (fd[WRITE_END] != CLOSED) ? _dup(fd[WRITE_END], fd[STDIN_OUT]) : ret;
		if (execve(sentence, (argv + 1), env) == -1)
			exit(127);
	} else
	{	wait(&status);
		if (WIFEXITED(status))
			es = WEXITSTATUS(status);
	} free(sentence), free(env), (fd[LT2_OUT] != CLOSED) ? unlink(TMP_FILE) : es;
	fd[LT2_OUT] = CLOSED;
	return (es);
}
