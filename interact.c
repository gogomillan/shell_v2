#include "hsh.h"

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
	char **argv = NULL, *line = NULL, *myln = NULL, *cmds[3] = {NULL, NULL, NULL};

	isatty(STDIN_FILENO) == 0 ? inter = 0 : inter;
	do {										/* If interact then prompt */
		inter == 1 ?  write(STDOUT_FILENO, "($) ", 4) : inter, fflush(stdin);
		read = getline(&line, &len, stdin);		/* Get command line */
		if (read == -1)
		{	read == -1 && inter == 1 ? write(1, "\n", 1) : read, free(line);
			return (ret);						/* Return when EOF */
		}
		if (_split_oper(line, fd, cmds, execnt, inter) == NULL) /* Find operators */
		{	ret = fd[WRITE_END], addhist(argv), (*execnt)++, fd[WRITE_END] = CLOSED;
			continue;
		}
		j = _bld_exec_stack(line, &myln, &argc, &argv, av[0]); /* Build the args*/
		if (j == ERROR)
			return (ERROR);
		f = check_builtin(line);		/* Check for a built-in function */
		if (f != NULL)
		{	builtin = f(argv, lenv, execnt);
			if (_strncmp(line, "exit", 4) == 0 && builtin >= 0)
			{	free(line), free(argv), free(myln);
				if (argv[2] != NULL)
					return (builtin);
				return (ret);
			} ret = builtin;
		} else							/* Try external command */
			argc > 2 ? ret = myexec(j, argv, lenv, execnt, fd) : argc;
		addhist(argv), free(argv), free(myln), (*execnt)++, argv = NULL; /* Historical & clean */
		if ((ret == 127 || ret == 126 || ret == 2) && inter == 0)
		{	free(line);										/* When these exit status */
			return (ret);
		}
	} while (1);
	return (ret);
}
/**
 * _bld_exec_stack - Build the execution stack for command line
 * @line: The command line
 * @ml: The command line structure
 * @argc: The argument count
 * @argv: The arguments
 * @av_0: The program name
 * Return: EXIT_SUCCESFUL or ERROR
 */
int _bld_exec_stack(char *line, char **ml, int *argc, char ***argv, char *av_0)
{
	char *t, *str1 = NULL, *tmp = NULL, **av = NULL, *myline = NULL;
	int ac = 0, j;

	myline = _strdup(line), tmp = _strdup(myline);
	/* Count how many tokens there are */
	for (ac = 1, str1 = tmp; (t = strtok(str1, " \t\n")); ac++, str1 = NULL)
		if (t == NULL)
			break;
	/* Ask for memory */
	av = askmem(++ac + 2, myline);
	if (av == NULL)
	{	free(tmp), free(myline); 
		return (ERROR);
	}
	/* Store the program name */
	av[0] = av_0;
	/* Store the arguments */
	for (j = 1, str1 = myline; ; j++, str1 = NULL)
	{
		t = strtok(str1, " \t\n"), av[j] = t;
		if (t == NULL)
			break;
	}
	/* Save memory addreses */
	*argv = av, *argc = ac;
	/* Cleaning */
	free(tmp);
	*ml = myline; 

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
	int iter;

	argv = malloc((argc) * sizeof(char **));
	if (argv == NULL)
	{
		free(myline);
		perror("Error: ");
		return (NULL);
	}
	for (iter = 0; iter < argc; iter++)
		*(argv + iter) = NULL;
	return (argv);
}
