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
char *_split_oper(char *line, int *fd, size_t *execnt, int inter, char **cmd2)
{
	int flags, cf, op = 0;
	char *opt = "><|&;#";
	char ret = FALSE, *t = NULL, *f = NULL;

	/* Looking for an operator */
	*(fd + STDIN_OUT) = STDOUT_FILENO, (void)cmd2;
	for (op = 0; op < _strlen(opt) && ret == FALSE; op++)
		ret = _find_oper(line, opt[op]);
	/* Verify error or false result */
	if (ret == ERROR)
	{	 _unexpected_redir(*execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	else if (ret == FALSE)
	{	*(fd + WRITE_END) = CLOSED;
		return (line);
	}
	/* Get flags for redirections and see for command conectors */
	cf = _def_flags(line, fd, ret, opt[--op], inter, &flags, &t, &f), *cmd2 = f;
	if (cf == ERROR)
		return (NULL);
	/* If necessary open files for redirection */
	if (ret == PIPE || ret == OR || ret == AND || ret == SC || ret == COMM)
		*(fd + WRITE_END) = (ret != PIPE) ? STDOUT_FILENO : *(fd + WRITE_END);
	else
		*(fd + WRITE_END) = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (ret == LT || ret == LT2)	/* Control for < */
		*(fd + STDIN_OUT) = STDIN_FILENO;
	if (*(fd + WRITE_END) == ERROR)	/* If fail openning */
	{	_cannot_create(ret, f, *execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	if (cf == TRUE)					/* Close file when cmd line starts with < or > */
	{	(flags == NO_OTHER) ? close(*(fd + WRITE_END)) : flags;
		*(fd + WRITE_END) = 0;
		return (NULL);
	}
	return (line);
}

/**
 * _find_oper - Tries to find the derired operator
 * @str: The string to check
 * @oper: The operator ['>' | '<' | ';' | '&' | '|' | '#' ]
 * Return: GT, GT2, LT, LT2, PIPE, SC, AND, OR, COMM, FALSE, ERROR
 */
char _find_oper(char *str, char oper)
{
	char *p = str;

	/* Validate the no string option */
	if (str == NULL)
		return (FALSE);
	/* Encode oper character when double quotes */
	_hide_char(str, oper, '\v');
	/* Find out for the oper in the string */
	while ((p = _strchr(p, oper)) != NULL)
	{
		if (p != NULL)							/* Oper found */
		{
			if (oper == '#')	/* Find out comment special case*/
			{	_hide_char(str, '\v', oper);
				return (_cmm_case(str));	/* Decode and return */
			}
			if (*(p + 1) == oper)			/* Double oper found */
			{
				if (*(p + 2) == oper)	/* Triple oper found */
				{	_hide_char(str, '\v', oper);
					return (ERROR);			/* Decode and return */
				}
				else					/* If not return double oper */
				{	_hide_char(str, '\v', oper);
					return ((oper == '>') ? GT2 : ((oper == '<') ? LT2 :
							((oper == '|') ? OR : ((oper == '&') ? AND :
							((oper == '#') ? COMM : FALSE)))));	/* Decode and return */
				}
			} else							/* If not return sigle oper */
			{	_hide_char(str, '\v', oper);
				return ((oper == '>') ? GT : ((oper == '<') ? LT :
						((oper == '|') ? PIPE : ((oper == ';') ? SC :
						((oper == '#') ? COMM : FALSE)))));		/* decode and return */
			}
		}
		p++;	/* Move the pointer if necessary */
	}
	/* Oper not found -> decode and return */
	_hide_char(str, '\v', oper);
	return (FALSE);
}

/**
 * _hide_char - Change a character from ctoh to cio if there is a double quote
 * @str: The string
 * @ctoh: Char to hide
 * @cio: Char instead of ctoh
 */
void _hide_char(char *str, char ctoh, char cio)
{
	char *p1 = str, *p2 = NULL;

	/* Verify if a correct string */
	if (str == NULL)
		return;
	/* Look for an opening double quote */
	p1 = _strchr(p1, '"');
	if (p1 == NULL)
		return;
	/* Look for a closing double quote */
	p2 = _strchr((p1 + 1), '"');
	if (p2 == NULL)
		return;
	/* Look for the char to hide (ctoh) inside the quoting text */
	while (p1 != p2)
	{
		if (*p1 == ctoh)
			*p1 = cio;
		p1++;
	}
}

/**
 * _cmm_case - Special case for comments
 * @line: The command line
 * Return: [ COMM | FALSE ]
 */
int _cmm_case(char *line)
{
	char *p;

	if (line == NULL)
		return (FALSE);
	/* if find the # and in at the beginning or after a blank, then OK */
	p = _strchr(line, '#');
	if (p == line || *(p - 1) == ' ' || *(p - 1) == '\t')
		return (COMM);
	/* Else isn't cosidered a comment */
	return (FALSE);
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
