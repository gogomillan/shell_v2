#include "hsh.h"

int _test_cmd(char *sentence, char **av, char **env, size_t *execnt, int *fd);
char *_path_cmd(char **argv, lenv_s **lenv, char *pathos);

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
	int status, ret = 0, es = 0; /* ret = return, es = exit status */
	char *sentence = NULL, *pathos, **env = menv(lenv);

	/* Make sentence with path */
	(void) argc, pathos = _getenv("PATH", lenv);
	sentence = _path_cmd(argv, lenv, pathos);
	/*Is this sentence correct?*/
	ret = _test_cmd(sentence, argv, env, execnt, fd);
	if (ret != NO_OTHER)
		return (ret);
	/* Create a child process */
	pid = fork();
	if (pid == -1)									/* If any error from fork */
	{	perror("Error:");
		return (1);
	}
	else if (pid == 0)								/* Execute the command in the child */
	{	ret = (fd[WRITE_END] != CLOSED) ? _dup(fd[WRITE_END], fd[STDIN_OUT]) : ret;
		if (execve(sentence, (argv + 1), env) == -1)
			exit(127);
	}
	else											/* The parent wait for the child */
	{	wait(&status);
		if (WIFEXITED(status))
			es = WEXITSTATUS(status);
	}
	free(sentence), free(env);
	(fd[LT2_OUT] != CLOSED) ? unlink(TMP_FILE) : es;
	fd[LT2_OUT] = CLOSED;
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
