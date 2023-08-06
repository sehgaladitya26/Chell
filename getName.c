#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>

#include "getName.h"
//#include "cd.h"

#define fgGreen "\e[32;1m" //green color

char alias[256];

char* txtColor(char a[]){
    printf("%s",a);
}

char* getusrName() {
    struct passwd* usr;
    char sysName[256];
    char* usrName;
    
    int valid_sysName = gethostname(sysName, sizeof(sysName));
    if(valid_sysName == -1) {perror("User Error:");}

    if((usr = getpwuid(getuid())) != NULL) 
    {
        usrName = usr->pw_name;
    }
    else usrName = "?";

    strcat(usrName,"@");
    strcat(usrName,sysName); // add all these to cd
    //currentDir = parse_cd("~/hello world");
    //printf("\e[32;1m%s\e[0;37m:\e[1;94m%s\e[0;97m$\n",usrName,currentDir); // change \n to string
    sprintf(alias,"\e[32;1m%s\e[0;37m:",usrName);
    return alias;
}

// int main()
// {   
//     setup_routine();
//     char* currentDir;

//     char sysName[256];
//     char* usrName;
    
//     int valid_sysName = gethostname(sysName, sizeof(sysName));
//     if(valid_sysName == -1) {perror("User Error:");}

//     usrName = get_usrName();

//     strcat(usrName,"@");
//     strcat(usrName,sysName); // add all these to cd
//     currentDir = parse_cd("~/hello world");
//     printf("\e[32;1m%s\e[0;37m:\e[1;94m%s\e[0;97m$\n",usrName,currentDir); // change \n to string
//     currentDir = parse_cd("-");
//     printf("\e[32;1m%s\e[0;37m:\e[1;94m%s\e[0;97m$\n",usrName,currentDir);
//     return 0;
// }