# Simple Shell in C

## Description of Various Files

- **main.c** - Contains the main function, generates the prompt, takes the input command and calls the various functions according to the command entered.
- **ls.c**- Contains all the functions required to execute the ls command (including the flags). 
- **args.c**- Contains the function to split the command according to semi colons.Contains simple functions like execvp etc.
- **pinfo.c** - Contains all the functions required to implement the pinfo function.
- **clock.c** - Contains all the functions required to implement the timer function.
- **headers.h** - Header file for including all the required libraries.
- **makefile** - Make File .

### Features
- Piping
- I/O redirection
- Foreground/Background Process Handling
- Signal Handling
- Exectution of all bash commands and custom implementation of some of the commands.

### List of Commands which were implemented without using execvp
- cd: change directory
- ls: list all the files and directories inside the current directory
- echo: output whatever was the input
- cat: output the contenets of the file
- jobs: list all the running jobs and their PIDs
- kjob: kill a job
- fg: bring a job to foreground
- bg: send a job to background 10.time: show time
- remindme: set a reminder

