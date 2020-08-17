#ifndef H_SHELL
#define H_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>

#define HISTORY_FILE ".simple_shell_history"

/* Operator definitions */
#define ERROR -1
#define FALSE 0
#define TRUE  1
#define GT    2
#define GT2   3
#define LT    4
#define LT2   5
#define PIPE  6
#define SC    7
#define AND   8
#define OR    9
/* Redirections */
#define CLOSED    -1
#define READ_END   0
#define WRITE_END  1
#define STDIN_OUT  2
#define LT2_OUT    3
#define TMP_FILE "mytemp.XXXXXX.JMGG.Holbies"

extern char **environ;


/**
 * struct commhist - Structure for the command history
 * @id: Id for the node
 * @cmd: The command
 * @next: The next node
 */
typedef struct commhist
{
	int id;
	char *cmd;
	struct commhist *next;
} commhist_s;

/**
 * struct lenv_s - singly linked list
 * @str: variable
 * @next: points to the next node
 * @var: enviroment variables.
 * Description: singly linked list node to store the env
 */
typedef struct lenv_s
{
	char *var;
	struct lenv_s *next;
} lenv_s;

/**
 * struct builtin - Structure for builtins
 * @s: String to compare
 * @f: Function to execute
 */
typedef struct builtin
{
	char *s;
	int (*f)();
} builtin_s;

int inputfile(int argc, char **argv, lenv_s **lenv, size_t *execnt);
int interact(char **av, lenv_s **lenv, size_t *execnt);
int myexec(int argc, char **argv, lenv_s **lenv, size_t *execnt, int *fd);
void handsigint(int sign);
char *path(char *name, lenv_s **env);
int currhist(commhist_s **h, commhist_s **t);
int addhist(char **argv);
int savehist(lenv_s **lenv);
int sizehist(lenv_s **lenv);
int loadhist(lenv_s **lenv);

/* Function related with Built-ins */
int (*check_builtin(char *line))();
int _env(char **argv, lenv_s **lenv, unsigned int *execnt);
int _ex(char **argv, lenv_s **lenv, unsigned int *execnt);
int _cd(char **argv, lenv_s **lenv, unsigned int *execnt);
int _setenv(char **argv, lenv_s **lenv, unsigned int *execnt);
int _unsetenv(char **argv, lenv_s **lenv, unsigned int *execnt);
int _history(char **argv, lenv_s **lenv, unsigned int *execnt);
int _help(char **argv, lenv_s **lenv, unsigned int *execnt);
int _hlphelp(char **av, lenv_s **lenv, unsigned int *e);
int _hlpcd(char **av, lenv_s **lenv, unsigned int *e);
int _hlpexit(char **av, lenv_s **lenv, unsigned int *e);
int _hlphistory(char **av, lenv_s **lenv, unsigned int *e);

/* Functions related with redirection */
char *_split_oper(char *line, int *fd, size_t *execnt, int inter);
char _find_oper(char *str, char oper);
char *_trim(char *str, char c);
int _dup(int fd, char inout);
void _unexpected_redir(size_t execnt);
int _rdheredoc(char *f, int inter);

/* strings functions */
int _strlen(char *s);
char *_strdup(char *str);
int _strcmp(char *s1, char *s2);
int _strncmp(char *s1, char *s2, int n);
char *_strchr(char *s, int c);

/* enviroment funcs */
size_t print_list(lenv_s **head);
int delete_node(lenv_s **head, unsigned int index);
char **menv(lenv_s **head);
lenv_s *add_node(lenv_s **head, char *str);
lenv_s *cenv(char **env);
void free_list(lenv_s **head);
char *_getenv(char *name, lenv_s **lenv);


/* variables */
char **check_variable(char **argv, lenv_s **lenv);


/* Int funcs */
int _atoi(char *s);
int _isdigit(char *s);

#endif /* H_SHELL */
