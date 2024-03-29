<div align="center">
  <img width="791" height="223" src="assets/carbon_SimpleShellV_2.png">
</div>

# Simple Shell v 2.0
Simple Shell V 2.0 is a project based on the original sh "Borne shell" functionality,
but a small subset and allows us to understand Unix-like OS architecture. It was
developed on Ubuntu Linux and with C language.

![OS Linux](https://img.shields.io/badge/OS-Linux-blue.svg)
![distro Ubuntu](https://img.shields.io/badge/distro-Ubuntu-orange.svg)
![tecnology C](https://img.shields.io/badge/technology-C-green.svg)

The concepts covered here are:
- Who designed and implemented the original Unix operating system
- Who wrote the first version of the Unix shell
- How does a shell work
- What are a pid and a ppid
- How to manipulate the environment of the current process
- What is the difference between a function and a system call
- How to create processes
- What are the three prototypes of main
- How does the shell use the PATH to find the programs
- How to execute another program with the execve system call
- How to suspend the execution of a process until one of its children terminates
- What is EOF / end-of-file?
- How to redirect the content of a file into another file
- What is a pipeline
- How to setup a pipe
- How to setup a pipe between two processes


## Table of Contents
1. [Requirements](#Requirements)
2. [Installation](#Installation)
3. [Testing](#Testing)
4. [Examples](#Examples)
5. [Authors](#Authors)
6. [License](#License)


## Requirements
- Linux Ubuntu 14.04 LTS
- gcc (Ubuntu 4.8.4-2ubuntu1~14.04.4) 4.8.4
- C language understanding


## Installation
1. Clone the repo in your local machine:

**Example:**
```bash
vagrant@~$
vagrant@~$ git clone https://github.com/gogomillan/shell_v2
vagrant@~$
```
2. Go to the directory and compile the task you want:

**Example:**
```bash
vagrant@~$
vagrant@~$ cd shell_v2
vagrant@~$ make all
vagrant@~$ ./hsh
($) ls
addhist.c        check_variables.c  error_handler.c  _help.c     _hlphistory.c  intfuncs.c          myexec.c    _setenv.c
alias.c          currhist.c         _exit.c          _history.c  hsh            loadhist.c          path.c      sizehist.c
AUTHORS          dockerfile         _getenv.c        _hlpcd.c    hsh.h          main.c              README.md   string_funcs.c
_cd.c            _env.c             _getline.c       _hlpexit.c  inputfile.c    Makefile            savehist.c  _strtok.c
check_builtin.c  env_funcs.c        handsigint.c     _hlphelp.c  interact.c     man_1_simple_shell  script.sh   _unsetenv.c
($) exit
vagrant@~$
```


## Testing

This shell work like this in interactive mode:

**Example:**
```bash
vagrant@~$ ./hsh
($) /bin/ls
hsh main.c shell.c
($)
($) exit
vagrant@~$
```

But also in non-interactive mode:

**Example:**
```bash
vagrant@~$
vagrant@~$ echo "/bin/ls" | ./hsh
hsh main.c shell.c test_ls_2
vagrant@~$
vagrant@~$ cat test_ls_2
/bin/ls
/bin/ls
vagrant@~$
vagrant@~$ cat test_ls_2 | ./hsh
hsh main.c shell.c test_ls_2
hsh main.c shell.c test_ls_2
vagrant@~$
```
## Examples

#### hsh 1.0 +
- Handle the right stream redirection ">"
- Usage: "command > output_file"
```
gonza-juan-pc@~$ echo Holberton School > test
gonza-juan-pc@~$ cat -e test
Holberton School$
gonza-juan-pc@~$ 
```

#### hsh 1.1 +
- Handle the right stream redirection ">"
- Usage: "command n> output_file"
```
gonza-juan-pc@~$ echo Holberton School > test
gonza-juan-pc@~$ cat -e test
Holberton School$
gonza-juan-pc@~$ echo Holberton School 1> test
gonza-juan-pc@~$ cat -e test
Holberton School$
gonza-juan-pc@~$ echo Holberton School 2> test
gonza-juan-pc@~$ cat -e test
gonza-juan-pc@~$ ls /hbtn 2> test
gonza-juan-pc@~$ cat -e test
ls: cannot access /hbtn: No such file or directory$
gonza-juan-pc@~$
```

#### hsh 1.1 +
- Handle the double right stream redirection ">>"
- Usage: "command >> output_file"
```
gonza-juan-pc@~$ rm -f test
gonza-juan-pc@~$ echo Holberton School >> test
gonza-juan-pc@~$ cat -e test
Holberton School$
gonza-juan-pc@~$ echo Holberton School >> test
gonza-juan-pc@~$ cat -e test
Holberton School$
Holberton School$
gonza-juan-pc@~$ 
```

#### hsh 1.2 +
- Handle the left stream redirection "<"
- Usage: "command < output_file"
```
gonza-juan-pc@~$ cat -e small_file 
Holberton$
Second line$
gonza-juan-pc@~$ rev < small_file
notrebloH
enil dnoceS
gonza-juan-pc@~$
```

#### hsh 1.3 +
- Handle the double left stream redirection << (also known as heredoc)
- Usage: "command << delimiter"
```
gonza-juan-pc@~$ cat -e << HOLBERTON
> qwertyuiop
> ls -l                          
> cat -e small_file
> HOLBERTONnope
> nopeHOLBERTON
> HOLBERTON 
> HOLBERTON
qwertyuiop$
ls -l$
cat -e small_file$
HOLBERTONnope$
nopeHOLBERTON$
HOLBERTON $
gonza-juan-pc@~$
```

#### hsh 1.4 +
- Handle the pipe stream redirection "|"
- Usage: "command1 | command2"
```
gonza-juan-pc@~$ ls /var | rev
spukcab
ehcac
hsarc
bil
lacol
kcol
gol
liam
scirtem
tpo
nur
loops
pmt
gonza-juan-pc@~$ ls -lr /var | cat -e
total 44$
drwxrwxrwt  2 root root     4096 Jul  5 10:26 tmp$
drwxr-xr-x  9 root root     4096 Feb 17  2016 spool$
lrwxrwxrwx  1 root root        4 Nov 19  2016 run -> /run$
drwxr-xr-x  2 root root     4096 Feb 17  2016 opt$
drwxrwsrwt  2 root whoopsie 4096 Feb 17  2016 metrics$
drwxrwsr-x  2 root mail     4096 Feb 17  2016 mail$
drwxrwxr-x 14 root syslog   4096 Jul 11 09:34 log$
lrwxrwxrwx  1 root root        9 Nov 19  2016 lock -> /run/lock$
drwxrwsr-x  2 root staff    4096 Apr 10  2014 local$
drwxr-xr-x 70 root root     4096 Mar 21 13:06 lib$
drwxrwsrwt  2 root whoopsie 4096 Jul 11 14:54 crash$
drwxr-xr-x 17 root root     4096 Feb 17  2016 cache$
drwxr-xr-x  2 root root     4096 Jul 11 09:33 backups$
gonza-juan-pc@~$ echo "Holberton" | wc -
10
gonza-juan-pc@~$ 
```

#### hsh 1.5 +
- Handle the commands separator ";"
- Usage: "command1 ; command2"
```
gonza-juan-pc@~$ ls /var ; ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-pc@~$ ls /hbtn ; ls /var
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-pc@~$ ls /var ; ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
gonza-juan-pc@~$ ls /var ; ls /hbtn ; ls /var ; ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-pc@~$
```

#### hsh 1.6 +
- Handle the "&&" logical operator
```
gonza-juan-pc@~$ ls /var && ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-pc@~$ ls /hbtn && ls /var
ls: cannot access /hbtn: No such file or directory
gonza-juan-pc@~$ ls /var && ls /var && ls /var && ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
gonza-juan-pc@~$ ls /var && ls /var && ls /var && ls /hbtn && ls /hbtn
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
ls: cannot access /hbtn: No such file or directory
gonza-juan-pc@~$
```

#### hsh 1.7 +
- Handle the "||" logical operator
```
gonza-juan-p@~$ ls /var || ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-p@~$ ls /hbtn || ls /var
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-p@~$ ls /hbtn || ls /hbtn || ls /hbtn || ls /var
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-p@~$ ls /hbtn || ls /hbtn || ls /hbtn || ls /var || ls /var
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
ls: cannot access /hbtn: No such file or directory
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  spool  tmp
gonza-juan-p@~$
```

#### hsh 1.8 +
- Implement the setenv and unsetenv builtin commands

    + setenv
        + Initialize a new environment variable, or modify an existing one
        + Command syntax: setenv VARIABLE VALUE
        + Should print something on stderr on failure
    + unsetenv
        + Remove a environment variable
        + Command syntax: unsetenv VARIABLE
        + Should print something on stderr on failure

#### hsh 1.0 +
- Handle Ctrl+C: your shell should not quit when the user inputs ^C
```
gonza-juan-pc@ubuntu:~/shell$ ./hsh
$ ^C
$ ^C
$ ^C
$ ls /var
backups  cache  crash  lib  local  lock  log  mail  metrics  opt  run  snap  spool  tmp
$ ^C
$ ^C
$ ^C
$ exit
gonza-juan-pc@ubuntu:~/shell$ 
```

#### hsh 1.0 +
- Handle Ctrl+C: your shell should not quit when the user inputs ^C

    + Implement the alias builtin command
    + Usage: alias [name[='value'] ...]
        + alias: Prints a list of all aliases, one per line, in the form name='value'
        + alias name [name2 ...]: Prints the aliases name, name2, etc 1 per line, in the form name='value'
        + alias name='value' [...]: Defines an alias for each name whose value is given. If name is already an alias, replaces its value with value

## Authors

**Juan Marcos Cabezas**
- Twitter: [@juanmarcoscabezas](https://twitter.com/juanmarcoscabezas)
- Github: [@juanmarcoscabezas](https://github.com/juanmarcoscabezas)
- LinkedIn: [@juanmarcoscabezas](https://linkedin.com/in/juanmarcoscabezas)

**Gonzalo Gomez Millan**
- Twitter: [@gogomillan](https://twitter.com/gogomillan)
- Github: [@gogomillan](https://github.com/gogomillan)
- LinkedIn: [@gogomillan](https://linkedin.com/in/gogomillan)


## License
MIT License [LICENSE](LICENSE)
