# Simple shell v2
This project named "simple shell" and its executable "hsh" is based on the
original sh "Borne shell" functionality, but an small subset.  

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
4. [Authors](#Authors)
5. [License](#License)


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
