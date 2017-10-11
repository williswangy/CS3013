# CS3013 
# Assignment 1
# Yuan Wang, ywang19@wpi.edu

## Compiling 
- Compilation can be done with the make command.
- Simply “cd” the directory
- Type <code> make </code>
- Use <code> make clean </code> to clean old builds

## doit

- doit is a basic shell that supports background tasks.
- doit can be run into two modes.
- First Mode
- Simply calling <code>./doit [command] </code> with a command will execute the command and then exit with the statistics following the instructions.
- Second Mode
- run as a basic shell.
- executing <code>./doit</code> will bring up a standard-looking command prompt.
- NOTE: Instead of using a default prompt string of “==>”
-       I want the program to look like a standard shell prompt
-       Therefore, this extracts the username, the hostname, and currently running path and uses those to construct the prompt.
-       DO NOT DEDUCT SCORES FROM THIS PART. I JUST MAKE SOME CHANGES.

- doit will run any command and allows backgrounding of those commands as well by adding an ampersand(&) to the end of a command line.
- <code> jobs </code> command => get list of currently executing jobs
- when a job completes, the command line will show the statistics for the completed job which working for both foreground and background jobs.

- to change directory in the shell, use the <code>cd</code> command.
- Tp exit the shell, use the <code>exit</code> command.

