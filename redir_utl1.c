#include "hsh.h"

/**
 * _unexpected_redir - Message for Syntax error: redirection unexpected
 * @execnt: Command line counter
 * Return: Nothing
 */
void _unexpected_redir(size_t execnt)
{
	char msg[161];

	sprintf(msg, "%s: %ld: Syntax error: redirection unexpected\n",
	"./hsh", execnt);
	write(STDERR_FILENO, msg, _strlen(msg));
}

/**
 * _cannot_create - Message for Syntax error: cannot create
 * @f: The file name
 * @execnt: Command line counter
 * Return: Nothing
 */
void _cannot_create(char *f, size_t execnt)
{
	char msg[161];
	char *errmsg[2] = {"Permission denied", "Directory nonexistent"};
	int  err;

	err = *(__errno_location());
	if (err == 13)
		err = 0;
	else
		err = 1;

	sprintf(msg, "%s: %ld: cannot create %s: %s\n", "./hsh", execnt, f,
	errmsg[err]);
	write(STDERR_FILENO, msg, _strlen(msg));
}

/**
 * _def_flags - Define flags for redirection
 * @line: The command line
 * @fd: The file descriptors array for redirection
 * @cmds: The commands
 * @re: What symbol "> >> < <<"
 * @in: Interactive [0 | 1]
 * @flag: Flags
 * @t: Token to command
 * @f: Token to file name
 * Return: Erase the file [TRUE | FALSE] or ERROR
 */
int _def_flags(char *line, int *fd, char **cmds, char re, int in,
		int *flag, char **t, char **f)
{
	int ret = FALSE, pipefd[2];
	char *opt = "><|", *tmp = NULL;

	tmp = _strdup(line), *t = strtok(tmp, opt), *f = strtok(NULL, opt);
	if (*t != NULL && *f != NULL)	/* Special contitions > file or < file */
		*t = strtok(line, opt), *f = strtok(NULL, opt);
	else
		*f = strtok(line, opt), *t = line, ret = TRUE;
	free(tmp);
	*f = _trim(*f, ' '), *f = _trim(*f, '\t');
	*f = _trim(*f, '\n'), *f = _trim(*f, '\r');

	if (re == GT)			/* Open the file for create ">" */
		*flag = O_CREAT | O_WRONLY | O_TRUNC;
	else if (re == LT)		/* Open the file for read "<" */
		*flag = O_RDONLY;
	else if (re == LT2)		/* Open tmp file for << */
	{	*(fd + LT2_OUT) = _rdheredoc(*f, in), *flag = O_RDONLY, *f = TMP_FILE;
		if (*(fd + LT2_OUT) == ERROR)
			return (ERROR);
	}
	else if (re == GT2)		/* Open the file for append ">>" */
		*flag = O_CREAT | O_WRONLY | O_APPEND;
	else if (re == PIPE)	/* Create the pipe */
	{	cmds[0] = *t, cmds[1] = *f;
		if (pipe(pipefd) == -1)
		{   perror("pipe");
			return (ERROR);
		}	fd[READ_END] = pipefd[READ_END], fd[WRITE_END] = pipefd[WRITE_END];
		return (PIPE);
	}

	return (ret);
}
