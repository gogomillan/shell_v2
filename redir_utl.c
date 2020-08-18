#include "hsh.h"

/**
 * _split_oper - Splits the getline result into tokens by opers
 * @line: The command line
 * @fd: The file descriptor
 * @cmds: The commands from command line
 * @execnt: the command line counter
 * @inter: Interactive mode [0 | 1]
 * Return: The line without the redirection
 */
char *_split_oper(char *line, int *fd, char **cmds, size_t *execnt, int inter)
{
	int flags, rdf;
	char *opt = "><|", ret, *t = NULL, *f = NULL;

	/* Looking for operators */
	*(fd + STDIN_OUT) = STDOUT_FILENO;
	ret = _find_oper(line, opt[0]);					/* > */
	if (ret == FALSE)
		ret = _find_oper(line, opt[1]);				/* < */
	if (ret == FALSE)
		ret = _find_oper(line, opt[2]);				/* | */

	if (ret == ERROR)
	{	 _unexpected_redir(*execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	else if (ret == FALSE)
	{	*(fd + WRITE_END) = CLOSED;
		return (line);
	}

	rdf = _def_flags(line, fd, cmds, ret, inter, &flags, &t, &f);
	if (rdf == ERROR)
		return (NULL);
	if (rdf == PIPE)
		return (line);

	*(fd + WRITE_END) = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (ret == LT || ret == LT2)
		*(fd + STDIN_OUT) = STDIN_FILENO;
	if (*(fd + WRITE_END) == -1)
	{	_cannot_create(f, *execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	if (rdf == TRUE)
	{	close(*(fd + WRITE_END)), *(fd + WRITE_END) = 0;
		return (NULL);
	}
	return (line);
}

/**
 * _find_oper - Tries to find the derired operator
 * @str: The string to check
 * @op: The operator ['>' | '<' | ';' | '&' | '|']
 * Return: GT, GT2, LT, LT2, PIPE, SC, AND, OR, FALSE, ERROR
 */
char _find_oper(char *str, char op)
{
	char *p = str;

	if (str == NULL)
		return (FALSE);

	while ((p = _strchr(p, op)) != NULL)
	{
		if (p != NULL)
		{
			if (*(p + 1) == op)
			{
				if (*(p + 2) == op)
					return (ERROR);
				else
					return ((op == '>') ? GT2 : ((op == '<') ? LT2 : OR));
			}
			else
			{
				return ((op == '>') ? GT : ((op == '<') ? LT : PIPE));
			}
		}
		p++;
	}

	return (FALSE);
}

/**
 * _trim - Trim characteres at the begging and end of a string
 * @str: String
 * @c: Character to trim
 * Return: A pointer to the new position to start the string
 */
char *_trim(char *str, char c)
{
	char *h = str, *t = str; /* Head and Tail of the string */

	if (str == NULL)
		return (str);
	if (_strlen(str) <= 0)
		return (str);

	while (*t++ != '\0')
		if (*h == c)
			h++;
	t--, t--;
	while (*t == c)
		*t-- = '\0';

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
	{	/* Duplicate the file on the STDOUT stream */
		if (dup2(fd, STDIN_FILENO) == -1)
		{   perror("dup2");
			return (-1);
		}
	}
	return (fd);
}

/**
 * _rdheredoc - read the input for the heredoc redirection
 * @f: Token's name
 * @inter: Intereactive mode [0 | 1]
 * Return: File descripto to the tmp file.
 */
int _rdheredoc(char *f, int inter)
{
	int fd, ret;
	char *buf = NULL;
	size_t len = 0;

	if (f == NULL)
		return (ERROR);
	/* Open the fileOpen the file for tmp */
	fd = open(TMP_FILE, O_CREAT | O_RDWR | O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd == ERROR)
	{   perror("open");
		return (ERROR);
	}
	/* Take the keyboard input */
	do {
		if (inter)
			write(STDOUT_FILENO, "> ", 2);
		ret = getline(&buf, &len, stdin);
		if (ret > 0 && _strncmp(buf, f, _strlen(buf) - 1) != 0)
			if (write(fd, buf, _strlen(buf)) <= 0)
			{   perror("write");
				return (ERROR);
			}
	} while (ret > 0 && _strncmp(buf, f, _strlen(buf) - 1) != 0);
	fflush(stdin), fflush(stdout), free(buf);
	close(fd);
	return (fd);
}
