#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
char src_path[PATH_MAX];
char dst_path[PATH_MAX];


void
*path_cat(char *path, char *dir){
    char *new_path = (char *) malloc(sizeof(char) * (strlen(path) + strlen(dir) + 1));
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, dir);
    return new_path;
}
void
f_copy(char *f_name){
    char *path = path_cat(src_path, f_name);
    char *new_path = path_cat(dst_path, f_name);
    FILE *fp = fopen(path, "rb");
    FILE *new_fp = fopen(new_path, "wb");
    if(fp == NULL){
        perror(path);
        exit(1);
    }
    if(new_fp == NULL){
        perror(dst_path);
        exit(1);
    }

    char buff[512];
    size_t size = 0;
    while(feof(fp) == 0){
        size = fread(buff, 1, sizeof(buff), fp);
        if(fwrite(buff, 1, size, new_fp) != size){
            perror("fail to write\n");
            exit(1);
        }
    }
    fclose(fp);
    fclose(new_fp);
    return;
}

void
d_copy(char *dir){
    DIR *dp;
    struct dirent *ep;
    
    char *path = path_cat(src_path, dir);
    dp = opendir(path);
    if(dp == NULL){
        perror(path);
        exit(1);
    }

    char *new_path = path_cat(dst_path, dir);
    if(mkdir(new_path, 0755) == -1){
        perror(new_path);
        exit(1);
    }

    for(; ep = readdir(dp); ){
        if(strcmp(ep->d_name, ".") == 0){
            continue;
        }
        if(strcmp(ep->d_name, "..") == 0){
            continue;
        }
        if(ep->d_type == DT_LNK){
            continue;
        }
        if(ep->d_type == DT_REG){
            char *sub_file = path_cat(dir, ep->d_name);
            f_copy(sub_file);
            free(sub_file);
        }
        if(ep->d_type == DT_DIR){
            char *sub_path = path_cat(dir, ep->d_name);
            d_copy(sub_path);
            free(sub_path);
        }
    } // for
    closedir(dp);
    free(path);
    free(new_path);
    return;
}

char *
find_dir(char *path){
    char *new_path;

    if(path[0] == '/'){ // 절대경로일 때
        int ind;
        for(ind = strlen(path) - 1; path[ind] != '/'; ind--);
        new_path = strdup(path + ind + 1);
    }
    else if((path[0] == '.' && path[1] == '/')){ // 상대경로일 때
        int ind;
        for(ind = strlen(path) - 1; path[ind] != '/'; ind--);
        new_path = strdup(path + ind + 1);
    }

    return new_path;
}


int main(int argc, char **argv){
    if(argc != 3){
        perror("wrong argument.\n");
        exit(1);
    }
    
    strncpy(dst_path, argv[1], strlen(argv[1]) - 1); 
    strncpy(src_path, argv[2], strlen(argv[2]) - 1);
    printf("%s %s\n", dst_path, src_path);

    char *dir = find_dir(src_path);
    src_path[strlen(src_path) - strlen(dir)] = '\0';
    printf("%s %s\n", src_path, dir);


    d_copy(dir);

    free(dir);
    return 0;
}