#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

char src_path[PATH_MAX];
char dst_path[PATH_MAX];
void help(){
    printf("command line instruction\n");
    printf("1) $./star archive <archive file name> <target directory path>\n");
    printf("2) $./star list <archive file name>\n");
    printf("3) $./star extract <archive file name>\n");
    return;
}

void error_check(char *file){
    if(access(file, F_OK) == -1){
            perror(file);
            exit(1);
        }
}

int main(int argc, char **argv){
    if(argc == 1){
        help();
        return 0;
    }

    if(strcmp(argv[1], "archive") == 0 && argc == 4){
        strcpy(dst_path, argv[2]);
        strcpy(src_path, argv[3]);
        error_check(dst_path);
        error_check(src_path);
        
        // function call
    }
    else if(strcmp(argv[1], "list") == 0 && argc == 3){
        strcpy(dst_path, argv[2]);
        error_check(dst_path);

        // function call
    }
    else if(strcmp(argv[1], "extract") == 0 && argc == 3){
        strcpy(dst_path, argv[2]);
        error_check(dst_path);

        // function call
    }
    else{
        fputs("wrong command. How to use command : ./star\n", stderr);
        exit(1);
    }
    return 0;
}