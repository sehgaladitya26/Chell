# Shell Features and walkthrough:
### All the built-in commands created:
- cd
- pwd
- echo
- I/O redirection
- pipe
- pinfo
- discover
- history
- continue
- clear
- exit

### Files explanation:
shell.c
> It is the main file which orchestrates all the functions which includes
> - Shell prompt
> - User input
> - Initial setup of directories, structs and signal handler.
> - Segregation and tokenization of foreground and background commands
> - Calling of in-built, external and other helper functions

getName.c
> It finds the username and hostname and sends it back to the shell.c.

cd.c
> It is a replica of actual cd command with most of the commands implemented in it.<br/>
> It performs following jobs to do:
> - Initializing home, current and previous working directory
> - parsing input command and changing directory
> - Keeping note of current directory and printing '~' respectively

echo.c
> It simply prints the input string with spaces, tabs, newlines (etc) delimited.

ls.c
> It prints all the directories, files and executables in the target directory and color code them respectively.<br/>
> Multiple flags can be called at once i.e. '-l', '-a', '-al' or '-la' with their usual meaning as in the actual ls command.<br/>

execute.c
> It deals with process creation either in foreground or background and keeps its track in a single linked list.<br/>
> It allows foreground process to take control of the terminal and to get it back once it is terminated by changing process group ID of child process to it's process ID and changing it's descriptor to STDERR_FILENO.<br/>
> It prints the time foreground process was executed for.<br/>
> It prints the name, PID and exit status of a background process when it ends.<br/>
NOTE: When the background process ends, it still takes current input under the exit-prompt although, it doesn't shows the shell prompt.<br/>
> Job function prints all the process, their name, id, and job number <br/>
> sig [job number] [signal ] can be used to send a signal to any processes.
> fg [job number] brings the stated stopped process running in foreground
> bg [job number] brings the stated stopped process runnoing in background<br/>

pinfo.c
> It prints the process ID, virtual memory, status and executable path of the target process.<br/>
> If no process ID is given, it prints the stats for the shell by default.<br/>

discover.c
> It finds and print the target file or directory inside the given directory hierarchy. <br/>
> Flag '-f' can be used to filter files from the directory while searching. <br/>
> Similarly Flag '-d' filter out directories from the files.<br/>
> If only discover command or 'discover -f -d "(optional)file/directory name" ' is given then it will print all the files and directories in that hierarchy.

history.c
> It stores all the last 20 commands in file named 'history.txt'. <br/>
> Once called, it will print last 10 commands in the terminal. <br/>
> It can be invoked from any instance of ./shell process and all the commands can be seen on either shells. 

signal.c
> It initialise a global struct containing information about shell's process ID, process group ID and sets it's descriptor to STDERR_FILENO for terminal access switching as discussed in execute.c. <br/>
> Ctrl+Z, Ctrl+C are disabled for the shell and can only be used on the processes invoked from the shell.<br/>
> To log out of the shell, enter Ctrl+D.<br/>

io_red.c
> It parses and runs a command containing '<', '>' or '>>'. in background, foreground (overwrite) and foreground (append) respectively<br/>

pipe.c
> It joins two or more commands inputs and output together by breaking it on the basis of '|'<br/>
> For now, I/O redirection is not available in pipelines.<br/>

# Assumptions taken:

- A max of 1024 commands can only be taken for now.<br/>
  Each command can have a max length of 256 characters. <br/>

- cd takes a single arguments. 
  More than one can result in "multiple argument error" prompt <br/>

- Flags and quotes are not considered in echo.
  Although, any spaces or tabs would be delimited. <br/>

- pwd commands prints current directory from /home <br/>

- chaining of multiple background, foreground and built-in commands are possible using '&' and ';' for background and foreground respectively. <br/>

- All the built-in commands would run in foreground even though invoked with '&'. <br/>

- When multiple foreground process exits, shell prints time-taken for all the individual processes.<br/>
> - For ex: If input is: sleep 5; sleep 20; shell will print:<br/>
>  sleep executed for 5 seconds<br/>
>  sleep executed for 20 seconds<br/>

- Discover command ignores hidden file. <br/>

- History is stored in a "history.txt" file in the source folder<br/>

