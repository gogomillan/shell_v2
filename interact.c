#include "hsh.h"

char **askmem(int argc, char *line);
int _cmdln(char *line, char **ml, char **tm, char ***ar, int *ac, char **av);

/**
 * interact - Exececutes a command
 * @av: Parameters for the program
 * @lenv: The variables from the environment list
 * @execnt: the counter
 * Return: Always 0
 */
int interact(char **av, lenv_s **lenv, size_t *execnt)
{
	size_t len = 0;
	int read = 1, j, argc = 0, inter = 1, (*f)() = NULL, builtin;
	int ret = 0, fd[4] = {CLOSED, CLOSED, STDOUT_FILENO, CLOSED};
	char **argv = NULL, *line = NULL, *tmp = NULL, *myline = NULL;

	isatty(STDIN_FILENO) == 0 ? inter = 0 : inter;
	do {
		inter == 1 ?  write(STDOUT_FILENO, "($) ", 4) : inter, fflush(stdin);
		read = getline(&line, &len, stdin);
		if (read == -1)
		{	read == -1 && inter == 1 ? write(1, "\n", 1) : read, free(line);
			return (ret);
		}
		if (_split_oper(line, fd, execnt, inter) == NULL) /* Redirections > >>< << */
		{	ret = fd[WRITE_END], addhist(argv), (*execnt)++, fd[WRITE_END] = CLOSED;
			continue;
		}
		j = _cmdln(line, &myline, &tmp, &argv, &argc, av); /* Stack for the cmdline*/
		if (j == ERROR)
			return (ERROR);
		f = check_builtin(myline);
		if (f != NULL)
		{	builtin = f(argv, lenv, execnt);
			if (_strncmp(myline, "exit", 4) == 0)
			{	free(tmp), free(myline), free(line), free(argv);
				return ((builtin == -1) ? ret : builtin);
			} ret = builtin;
		}
		else
			argc > 2 ? ret = myexec(j, argv, lenv, execnt, fd) : argc;
		addhist(argv), free(argv), free(tmp), free(myline), (*execnt)++;
		argv = NULL, tmp = NULL, myline = NULL;
		if ((ret == 127 || ret == 126 || ret == 2) && inter == 0)
		{	free(line);
			return (ret);
		}
	} while (1);
	free(myline), free(line);
	return (ret);
}

/**
 * _cmdln - Command line stack
 * @line: The command line
 * @ml: The myline (base for stack)
 * @tm: The tmp line
 * @ar: The pointers to the tokens of command line
 * @ac: Number of tokens
 * @av: argv from main
 * Return: The arguments
 */
int _cmdln(char *line, char **ml, char **tm, char ***ar, int *ac, char **av)
{
	char *str1, *t, **argv = NULL, *tmp = NULL, *myline = NULL;
	int j, argc;

	myline = _strdup(line), tmp = _strdup(myline);
	for (argc = 1, str1 = tmp; (t = strtok(str1, " \t\n")); argc++, str1 = NULL)
		if (t == NULL)
			break;
	argv = askmem(++argc + 2, myline);
	if (argv == NULL)
		return (-1);
	argv[0] = av[0];
	for (j = 1, str1 = myline; ; j++, str1 = NULL)
	{	t = strtok(str1, " \t\n"), argv[j] = t;
		if (t == NULL)
			break;
	}
	*ml = myline, *tm = tmp, *ar = argv, *ac = argc;
	return (j);
}

/**
 * askmem - Allocates memory
 * @argc: Amount of memory to allocate
 * @myline: The pointer to line readed
 * Return: A pointer to the new memory area or NULL
 */
char **askmem(int argc, char *myline)
{
char **argv;

	argv = malloc((argc) * sizeof(char **));
	if (argv == NULL)
	{
		free(myline);
		perror("Error: ");
		return (NULL);
	}
	return (argv);
}
