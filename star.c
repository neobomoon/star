#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>

char src_path[PATH_MAX];
char dst_path[PATH_MAX];


typedef struct header{
    char type;
    unsigned int path_len;
    unsigned int data_size;
}Header;

void
*path_cat(char *path, char *dir){
    char *new_path = (char *) malloc(sizeof(char) * (strlen(path) + strlen(dir) + 1));
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, dir);
    return new_path;
}

void
write(char *buff, size_t size, FILE *new_fp){
    if(fwrite(buff, 1, size, new_fp) != size){
        perror("fail to write");
        exit(1);
    }
}
void
archive_write(char *f_name, Header *header, FILE *new_fp){
    char *path = path_cat(src_path, f_name);

    char buff[512];
    size_t size = 0;
    //헤더부터 저장
    if(fwrite(header, sizeof(Header), 1, new_fp) != sizeof(Header){
        perror("fail to write header");
        exit(1);
    }
    //path name 저장
    write(path, strlen(path), new_fp);

    //데이터 저장
    if(header->type == 1){
        FILE *fp = fopen(path, "rb");
        if(fp == NULL){
            perror(path);
            exit(1);
        }

        while(feof(fp) == 0){
            size = fread(buff, 1, sizeof(buff), fp);
            write(buff, size, new_fp); //hex으로 write하기
        }
        fclose(fp);

    }

    free(path);
    return;
}

void
header_update(char *path, Header* header, char type){
    struct stat s_file;
    if(stat(path, &s_file) == -1){
        perror("stat error");
        exit(1);
    }
    header->type = type;
    header->path_len = strlen(path);
    header->data_size = type == 1 ? s_file.st_size : 0; 
    return;
}

void
archive(char *dir, FILE *new_fp){
    DIR *dp;
    struct dirent *ep;
    Hearder *header = malloc(sizeof(Hearder));
    
    char *path = path_cat(src_path, dir);
    dp = opendir(path);
    if(dp == NULL){
        perror(path);
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
            header_update(path, header, 1);
            archive_write(sub_file, header, new_fp);
            free(sub_file);
        }
        if(ep->d_type == DT_DIR){
            char *sub_path = path_cat(dir, ep->d_name);
            header_update(path, header, 0);
            archive_write(sub_path, header, new_fp);
            archive(sub_path, new_fp);
            free(sub_path);
        }
    } // for end
    closedir(dp);
    free(path);
    fre(header);
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

void
help(){
    printf("command line instruction\n");
    printf("1) $./star archive <archive file name> <target directory path>\n");
    printf("2) $./star list <archive file name>\n");
    printf("3) $./star extract <archive file name>\n");
    return;
}

void
error_check(char *file){
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
        FILE *new_fp = fopen(dst_path, "w");
        error_check(dst_path);

        strcpy(src_path, argv[3]);
        error_check(src_path);
        char *dir = find_dir(src_path);
        src_path[strlen(src_path) - strlen(dir)] = '\0';

        archive(dir, new_fp);

        free(dir);
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

    fclose(new_fp);
    free(dir);
    return 0;
    
    strncpy(dst_path, argv[1], strlen(argv[1]) - 1); 
    strncpy(src_path, argv[2], strlen(argv[2]) - 1);
    printf("%s %s\n", dst_path, src_path);

    char *dir = find_dir(src_path);
    src_path[strlen(src_path) - strlen(dir)] = '\0';
    printf("%s %s\n", src_path, dir);


    d_copy(dir);
    return 0;
}