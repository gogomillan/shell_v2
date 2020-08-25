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
	int flags = NO_OTHER, cf, op = 0;
	char *opt = "><|&;#";
	char ret = FALSE, *t = NULL, *f = NULL;

	/* Set clue info */
	*(fd + STDIN_OUT) = STDOUT_FILENO, (void)cmd2, *(fd + WRITE_END) = CLOSED;
	*(_strchr(line, '\n')) = '\0';
	if (_is_err_syntax(line, ";|&", *execnt) != FALSE)		/* Verify Syntax */
	{	*(fd + WRITE_END) = 2;
		return (NULL);
	}
	for (op = 0; op < _strlen(opt) && ret == FALSE; op++)	/*Look for an operator*/
		ret = _find_oper(line, opt[op]);
	if (ret == ERROR)
	{	 _unexpected_redir(*execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	else if (ret == FALSE)
		return (line);
	cf = _def_flags(line, fd, ret, opt[--op], inter, &flags, &t, &f);	/*Get flag*/
	*cmd2 = f, ret = fd[OPER];
	if (cf == ERROR)
		return (NULL);
	if (ret == FALSE || ret == PIPE || ret == OR || ret == AND || ret == SC ||
		ret == COMM)															/* When pipe */
		*(fd + WRITE_END) = (ret == COMM) ? STDOUT_FILENO : *(fd + WRITE_END);
	else																		/* When redirction */
		*(fd + WRITE_END) = open(f, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (ret == LT || ret == LT2)												/* Control for < */
		*(fd + STDIN_OUT) = STDIN_FILENO;
	if (*(fd + WRITE_END) == ERROR)												/* If fail openning */
	{	_cannot_create(ret, f, *execnt), *(fd + WRITE_END) = 2;
		return (NULL);
	}
	if (ret != FALSE && cf == TRUE)				/* Close file if line starts with < or > */
	{	(flags == NO_OTHER) ? close(*(fd + WRITE_END)) : flags;
		*(fd + WRITE_END) = 0;
		return (NULL);
	}
	return (line);
}

/**
 * _is_err_syntax - Look for an unexpected char in a string
 * @s: The string
 * @c: A string with the chars
 * @execnt: Command line counter
 * Return: [TRUE | FALSE | ERROR]
 */
int _is_err_syntax(char *s, char *c, size_t execnt)
{
	char *before, *after, *tmp, buf[2] = {'\0', '\0'};

	/* Verify the string */
	if (s == NULL)
		return (ERROR);
	if (_strlen(s) == 0)
		return (FALSE);
	/* Look for a single special char */
	tmp = _strdup(s);
	while (*c++ != '\0')
	{
		buf[0] = *(c - 1);
		before = strtok(tmp, buf);
		after = strtok(NULL, buf);
		/* ($) ;[ENTER] or ($) |[ENTER] or ($) & or[ENTER] ($) || or[ENTER] */
		/* ($) &&[ENTER] become NULL both sides after strtok */
		if (before == NULL && after == NULL)
		{
			free(tmp);
			_unexpected_char(execnt, buf[0]);
			return (TRUE);
		}
	}
	free(tmp);
	return (FALSE);
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
