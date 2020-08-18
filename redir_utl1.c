#include "hsh.h"

/**
 * _unexpected_redir - Message for Syntax error: redirection unexpected\n"
 * @execnt: Command line counter
 * Return: Nothing
 */
void _unexpected_redir(size_t execnt)
{
	char msg[161];

	sprintf(msg, "%s: %ld: Syntax error: redirection unexpected\n",
	"./hsh", execnt);
	write(STDERR_FILENO, &msg, _strlen(msg));
}

/**
 * _def_flags - Define flags for redirection
 * @line: The command line
 * @fd: The file descriptors array for redirection
 * @re: What symbol "> >> < <<"
 * @in: Interactive [0 | 1]
 * @flag: Flags
 * @t: Token to command
 * @f: Token to file name
 * Return: Erase the file [TRUE | FALSE] or ERROR
 */
int _def_flags(char *line, int *fd, char re, int in,
		int *flag, char **t, char **f)
{
	int ret = FALSE;
	char *opt = "><", *tmp = NULL;

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
	else if (re == LT2)	/*  Open tmp file for << */
	{	*(fd + LT2_OUT) = _rdheredoc(*f, in), *flag = O_RDONLY, *f = TMP_FILE;
		if (*(fd + LT2_OUT) == ERROR)
			return (ERROR);
	}
	else					/* Open the file for append ">>" */
		*flag = O_CREAT | O_WRONLY | O_APPEND;

	return (ret);
}
