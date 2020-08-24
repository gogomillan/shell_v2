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
	write(STDERR_FILENO, &msg, _strlen(msg));
}

/**
 * _cannot_create - Message for Syntax error: cannot create
 * @ret: The operator
 * @f: The fine name
 * @execnt: Command line counter
 * Return: Nothing
 */
void _cannot_create(char ret, char *f, size_t execnt)
{
	char msg[161];
	char *errmsg[3] = {"Permission denied", "Directory nonexistent",
						"No such file"};
	char *o_c_msg[2] = {"cannot create", "cannot open"};
	int err;

	err = *(__errno_location());
	if (err == 13)
		err = 0;	/* Permission denied */
	else
		err = 1;	/* Directory nonexistent */
	if (ret == LT)
		err = 2, ret = 1;	/* Error msg for < */
	else
		ret = 0;			/* Error msg for > or >> */
	sprintf(msg, "%s: %ld: %s %s: %s\n", "./hsh", execnt, o_c_msg[(int)ret],
	f, errmsg[err]);
	write(STDERR_FILENO, &msg, _strlen(msg));
}

/**
 * _def_flags - Define flags for redirection
 * @line: The command line
 * @fd: The file descriptors array for redirection
 * @re: What symbol code for "> >> < << | || && ; #"
 * @sym: The symbol as a single char
 * @in: Interactive [0 | 1]
 * @flag: Flags
 * @t: Token to command
 * @f: Token to file name
 * Return: Erase the file [TRUE | FALSE] or ERROR
 */
int _def_flags(char *line, int *fd, char re, char sym, int in,
		int *flag, char **t, char **f)
{
	int pipefd[2], ret = FALSE;
	char opt[] = {'\0', '\0'}, *tmp = NULL;

	opt[0] = sym, _hide_char(line, sym, '\v'), tmp = _strdup(line);
	*t = strtok(tmp, opt), *f = strtok(NULL, opt);
	/* Special contitions > file or < file */
	if (*t != NULL && *f != NULL)
		*t = strtok(line, opt), *f = strtok(NULL, opt);	/* cmd oper cmd */
	else
		*f = strtok(line, opt), *t = line, ret = TRUE;	/* cases: < file or > file */
	/* Trimp chars from beginning and end for file name or next command */
	free(tmp), _hide_char(*t, '\v', sym), _hide_char(*f, '\v', sym);
	*f = _trim(*f, ' '), *f = _trim(*f, '\t');
	*f = _trim(*f, '\n'), *f = _trim(*f, '\r');
	/* Evaluate the flags needed */
	if (re == GT)			/* Open the file for create ">" */
		*flag = O_CREAT | O_WRONLY | O_TRUNC;
	else if (re == LT)		/* Open the file for read "<" */
		*flag = O_RDONLY;
	else if (re == LT2)		/*  Open tmp file for << */
	{	*(fd + LT2_OUT) = _rdheredoc(*f, in), *flag = O_RDONLY, *f = TMP_FILE;
		if (*(fd + LT2_OUT) == ERROR)
			return (ERROR);
	}
	else if (re == GT2)		/* Open the file for append ">>" */
		*flag = O_CREAT | O_WRONLY | O_APPEND;
	else if (re == COMM)	/* For # */
		*flag = O_COMM;
	else if (re == PIPE || re == SC || re == OR || re == AND)	/* For pipe "|" */
	{
		if (pipe(pipefd) == -1)
		{   perror("pipe");
			return (ERROR);
		}
		*(fd + READ_END) = pipefd[READ_END], *(fd + WRITE_END) = pipefd[WRITE_END];
	}
	else					/* For "||" or "&&" */
		*flag = NO_OTHER;
	/* This reurns let's know if the redirection file is a tmp */
	return (ret);
}

/**
 * _trim - Trim characteres at the beginning and end of a string
 * @str: String
 * @c: Character to trim
 * Return: A pointer to the new position to start the string
 */
char *_trim(char *str, char c)
{
	char *h = str, *t = str; /* Head and Tail of the string */

	if (str == NULL)		/* No string */
		return (str);
	if (_strlen(str) <= 0)	/* Empty string */
		return (str);

	while (*t++ != '\0')	/* Trim the char at the beginning */
		if (*h == c)
			h++;
	t--, t--;
	while (*t == c)			/* Trim the char at the end */
		*t-- = '\0';
	/* Return a pointer to the beginning */
	return (h);
}

/**
 * _dup - Duplicate the input or output
 * @fd: File descriptor
 * @inout: [STDIN_FILENO | STDOUT_FILENO]
 * Return: Nothing
 */
int _dup(int fd, char inout)
{
	if (inout == STDOUT_FILENO)
	{	/* Duplicate the file on the STDOUT stream */
		if (dup2(fd, STDOUT_FILENO) == -1)
		{   perror("dup2");
			return (-1);
		}
	}
	else if (inout == STDIN_FILENO)
	{	/* Duplicate the file on the STDIN stream */
		if (dup2(fd, STDIN_FILENO) == -1)
		{   perror("dup2");
			return (-1);
		}
	}
	return (fd);
}
