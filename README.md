#  A simple UNIX shell.
A C program that provides the basic operation of a command line shell 

## Shell commands implemented

* exit

Exit the program. Does not matter how many arguments the user enters; they are all ignored.

* pwd

Display the current working directory. Use `getcwd()` function. Run man getcwd for more. Ignore any extra arguments passed by the user.

* cd 

Use `chdir()` function. Pass `chdir()` the first argument the user enters (it will accept absolute and relative paths). Ignore any extra arguments passed by the user.

* history

Displays the ten most recent commands executed in the shell. If there are not yet ten commands entered, display all the commands entered so far (<10).

* ! command 

Command `!n` runs command number `n`, such as `!11` will re-run the eleventh command entered this session. 

* !! command

 Runs the previous command

## Signal

display the list of history commands when the user presses `ctrl-c` (which is the `SIGINT` signal)

Note that when another child process is running, `ctrl-c` will likely cancel/kill that process. Therefore displaying the history with `ctrl-c` will only be tested when there are no child processes running.
