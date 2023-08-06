#include <stdio.h>
#include <string.h>
#include "history.h"
#include "shell.h"

#define MAX_LINES 21
#define MAX_CHARS 256

void read_history(char *last)
{
    char cmd[256];
    strcpy(cmd, last);
    strcat(cmd, "\n");

    char arr[MAX_LINES][MAX_CHARS] = {{""}};
    int n = 0;
    FILE *fp = fopen(history_dir, "r");
    FILE *st = fopen(temp_dir, "a");
    while (n < MAX_LINES && fgets(arr[n], MAX_CHARS, fp))
    {
        n++;
    }

    if (n == 20 && strcmp(cmd, arr[n-1]) != 0)
    {
        for (int i = 1; i < n; i++)
        {
            fputs(arr[i], st);
        }
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            fputs(arr[i], st);
        }
    }
    if (strcmp(cmd, arr[n - 1]) != 0)
    {
        fputs(cmd, st);
    }
    fclose(fp);
    fclose(st);
    remove(history_dir);
    rename(temp_dir, history_dir);
    return;
}

void history()
{

    char arr[MAX_LINES][MAX_CHARS] = {{""}};
    int n = 0;
    FILE *fp = fopen(history_dir, "r");
    while (n < MAX_LINES && fgets(arr[n], MAX_CHARS, fp))
    {
        n++;
    }

    if (fp != stdin)
        fclose(fp);
    // for history command
    if (n <= 10)
        for (int i = 0; i < n; i++)
            printf("%s", arr[i]);
    else
    {
        for (int i = n - 10; i < n; i++)
            printf("%s", arr[i]);
    }
    return;
}