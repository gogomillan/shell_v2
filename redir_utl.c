#include "hsh.h"

/**
 * _split_oper - Splits the getline result into tokens by opers
 * @line: The command line
 * @fd: The file descriptor
 * @execnt: the command line counter
 * Return: The line without the redirection
 */
char *_split_oper(char *line, int *fd, size_t *execnt)
{
	int err, flags;
	char *errmsg[2] = {"Permission denied", "Directory nonexistent"}, *opt = "><";
	char msg[80], ret, *t, *f;

	ret = _find_oper(line, opt[0]), *(fd + STDIN_OUT) = STDOUT_FILENO;
	if (ret == FALSE)
		ret = _find_oper(line, opt[1]);
	if (ret == ERROR)
	{	 _unexpected_redir(*execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	else if (ret == FALSE)
	{	*(fd + WRITE_END) = CLOSED;
		return (line);
	}
	t = strtok(line, opt), f = strtok(NULL, opt), f = _trim(f, ' ');
	f = _trim(f, '\t'), f = _trim(f, '\n'), f = _trim(f, '\r');
	if (ret == GT)	/* Open the fileOpen the file for create ">" */
		flags = O_CREAT | O_WRONLY | O_TRUNC;
	else if (ret == LT)
		flags = O_RDONLY;
	else if (ret == LT2)
	{	*(fd + LT2_OUT) = _rdheredoc(f), flags = O_RDONLY, f = TMP_FILE;
		if (*(fd + LT2_OUT) == -1)
			return (NULL);
	}
	else			/* Open the fileOpen the file for append ">>" */
		flags = O_CREAT | O_WRONLY | O_APPEND;
	*(fd + WRITE_END) = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (ret == LT || ret == LT2)
		*(fd + STDIN_OUT) = STDIN_FILENO;
	if (*(fd + WRITE_END) == -1)
	{	err = (*(__errno_location()) == 13) ? 0 : 1, *(fd + WRITE_END) = 2;
		sprintf(msg, "%s: %ld: cannot create %s: %s\n",
		"./hsh", *execnt, f, errmsg[err]);
		write(STDERR_FILENO, &msg, _strlen(msg));
		return (NULL);
	}
	return (t);
}

/**
 * _find_oper - Tries to find the derired operator
 * @str: The string to check
 * @oper: The operator ['>' | '<' | ';' | '&' | '|']
 * Return: GT, GT2, LT, LT2, PIPE, SC, AND, OR, FALSE, ERROR
 */
char _find_oper(char *str, char oper)
{
	char *p = str;

	if (str == NULL)
		return (FALSE);

	while ((p = _strchr(p, oper)) != NULL)
	{
		if (p != NULL)
		{
			if (*(p + 1) == oper)
			{
				if (*(p + 2) == oper)
					return (ERROR);
				else
					return ((oper == '>') ? GT2 : LT2);
			}
			else
			{
				return ((oper == '>') ? GT : LT);
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
 * Return: File descripto to the tmp file.
 */
int _rdheredoc(char *f)
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
		printf("> ");
		ret = getline(&buf, &len, stdin);
		if (ret > 0 && _strncmp(buf, f, strlen(buf) - 1) != 0)
			if (write(fd, buf, strlen(buf)) <= 0)
			{   perror("write");
				return (ERROR);
			}
	} while (ret > 0 && strncmp(buf, f, strlen(buf) - 1) != 0);
	free(buf);
	close(fd);
	return (fd);
}
