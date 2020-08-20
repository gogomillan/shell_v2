#include "hsh.h"

/**
 * _split_oper - Splits the getline result into tokens by opers
 * @line: The command line
 * @fd: The file descriptor
 * @execnt: the command line counter
 * @inter: Interactive mode [0 | 1]
 * @cmd2: Second command
 * Return: The line without the redirection
 */
char *_split_oper(char *line, int *fd, size_t *execnt, int inter, char *cmd2)
{
	int flags, cf, op = 0;
	char *opt = "><|&;";
	char ret = FALSE, *t = NULL, *f = NULL;

	*(fd + STDIN_OUT) = STDOUT_FILENO, (void)cmd2;
	for (op = 0; op < 5 && ret == FALSE; op++)
		ret = _find_oper(line, opt[op]);

	if (ret == ERROR)
	{	 _unexpected_redir(*execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	else if (ret == FALSE)
	{	*(fd + WRITE_END) = CLOSED;
		return (line);
	}

	cf = _def_flags(line, fd, ret, inter, &flags, &t, &f), cmd2 = f;
	if (cf == ERROR)
		return (NULL);

	if (ret == PIPE || ret == OR || ret == AND || ret == SC)
		*(fd + WRITE_END) = STDOUT_FILENO;
	else
		*(fd + WRITE_END) = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (ret == LT || ret == LT2)
		*(fd + STDIN_OUT) = STDIN_FILENO;
	if (*(fd + WRITE_END) == -1)
	{	_cannot_create(ret, f, *execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	if (cf)
	{	close(*(fd + WRITE_END)), *(fd + WRITE_END) = 0;
		return (NULL);
	}
	return (line);
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

	/* Validate the no string option */
	if (str == NULL)
		return (FALSE);
	/* Find out for the oper in the string */
	while ((p = _strchr(p, oper)) != NULL)
	{
		if (p != NULL)							/* Oper found */
		{
			if (*(p + 1) == oper)			/* Double oper found */
			{
				if (*(p + 2) == oper)	/* Triple oper found */
					return (ERROR);
				else					/* If not return double oper */
					return ((oper == '>') ? GT2 : ((oper == '<') ? LT2 :
							((oper == '|') ? OR : ((oper == '&') ? AND : FALSE))));
			}
			else							/* If not return sigle oper */
			{
				return ((oper == '>') ? GT : ((oper == '<') ? LT :
						((oper == '|') ? PIPE : ((oper == ';') ? SC : FALSE))));
			}
		}
		p++;	/* Move the pointer if necessary */
	}
	/* Oper not found */
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
	char *buf = NULL, save;
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
		if (inter)			/* When interactive prompt */
			write(STDOUT_FILENO, "> ", 2);
		ret = getline(&buf, &len, stdin);
		save = buf[_strlen(buf) - 1];	/* Deal with the \n */
		buf[_strlen(buf) - 1] = '\0';
		if (ret > 0 && _strcmp(buf, f) != 0)
		{	/* Write in the tmp file if isn't the TOKEN */
			buf[_strlen(buf)] = save;
			if (write(fd, buf, _strlen(buf)) <= 0)
			{   perror("write");
				return (ERROR);
			}
			buf[_strlen(buf) - 1] = '\0';
		}
	} while (ret > 0 && _strcmp(buf, f) != 0);
	fflush(stdin), fflush(stdout), free(buf), close(fd);	/* Cleaning */
	/* Return the file descriptor to say we had input */
	return (fd);
}
