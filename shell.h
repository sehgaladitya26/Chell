#ifndef SHELL_H
#define SHELL_H

char history_dir[256];
char temp_dir[256];
int inbuilt_cmd_parser(char **args);
char **cmd_parse(char *cmd);
int tokenize_cmd(char *cmd_line, char **commands, char *delim);
#endif