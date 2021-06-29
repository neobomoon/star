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
char dst_file[PATH_MAX];


typedef struct header{
    char type;
    unsigned int path_len;
    unsigned int data_size;
    char *path_name;
}Header;

void
get_option(int argc, char **argv, char *opt){
    if(argc == 2){
        goto exit_err ;
    }

    if(strcmp(argv[1], "archive") == 0 && argc == 4){ // archive인 경우
        strcpy(dst_file, argv[2]) ;
        strcpy(src_path, argv[3]) ;

        if(access(src_path, F_OK) == -1)
            goto exit_err ;
        *opt = 'a';
        return ;
    }
    if(strcmp(argv[1], "list") == 0 && argc == 3){
        strcpy(dst_file, argv[2]) ;

        if(access(dst_file, F_OK) == -1)
            goto exit_err ;

        *opt = 'l' ;
        return ;
    }
    if(strcmp(argv[1], "extract") == 0 && argc == 3){
        strcpy(dst_file, argv[2]) ;

        if(access(dst_file, F_OK) == -1)
            goto exit_err ;

        *opt = 'e' ;
        return ;
    }

    goto exit_err ;

exit_err:
    perror("wrong option") ;
    exit(1) ;
}

void
*path_cat(char *path, char *dir){
    char *new_path = (char *) malloc(sizeof(char) * (strlen(path) + strlen(dir) + 1));
    strcpy(new_path, path);
    strcat(new_path, "/");
    strcat(new_path, dir);
    return new_path;
}

void
header_set(char type, int path_len, size_t data_size, char *path_name, Header *header){
    header->type = type ;
    header->path_len = path_len ;
    header->data_size = data_size ;
    header->path_name = (char *) malloc(sizeof(char) * strlen(path_name)) ;
    strcpy(header->path_name, path_name) ;
    return ;
}

void
header_write(char type, int path_len, size_t data_size, char *path_name, FILE *new_fp, Header *header){
    header_set(type, path_len, data_size, path_name, header) ; 

    size_t size = 9 + strlen(header->path_name) ;
    
    if(fwrite(header, 1, size, new_fp) != size)
        goto exit_fwrite ;

    free(header->path_name) ;
    return ;

exit_fwrite:
    perror("fail to fwrite");
    exit(1);
}

void
data_write(char type, char *path, FILE *new_fp){
    if(type = 0)
        return ;

    FILE *fp = fopen(path, "rb") ; 
    if(fp == NULL)
        goto exit_fopen ;
    
    char buff[512] ;
    size_t size ;
    while(feof(fp) == 0){
        size = fread(buff, 1, sizeof(buff), fp) ; 
        if(fwrite(buff, 1, size, new_fp) != size)
            goto exit_fwrite ;
    }

    fclose(fp) ;
    return ;

exit_fopen:
    perror("fail to fopen") ;
    exit(1) ;

exit_fwrite:
    perror("fail to fwrite") ;
    exit(1) ;
}

void
archive_write(FILE *new_fp, Header *header, char type, char *file){
    struct stat *s_file ;
    char *path = path_cat(src_path, file) ;
    if(stat(path, s_file) == -1)
        goto exit_stat ;
    
    header_write(type, strlen(file), type ? s_file->st_size : 0, file, new_fp, header) ;
    data_write(type, path, new_fp) ; 

    free(path);
    return ;

exit_stat:
    perror("fail to stat") ;
    exit(1) ;
}

//헤더, 데이터를 적어야 함.
void
archive(char *path, FILE *new_fp, Header *header){

    DIR *dp = opendir(path) ;
    if(dp == NULL)
        goto exit_opendir ; 
    
    struct dirent *dep ;
    for(; dep = readdir(dp); ){
        if(strcmp(dep->d_name, ".") == 0)
            continue ;
        if(strcmp(dep->d_name, "..") == 0)
            continue ;
        if(dep->d_type == DT_LNK)
            continue ;
        if(dep->d_type == DT_REG)
            archive_write(new_fp, header, 1, dep->d_name) ; // 데이터 적기
        if(dep->d_type == DT_DIR){
            archive_write(new_fp, header, 0, dep->d_name) ;
            //여러번 돌 때 돌리기
        }
    } // for end

    closedir(dp) ;
    return ;

exit_opendir:
    perror("fail to opendir") ;
    exit(1) ;
}

int
main(int argc, char **argv){
    char opt ;
    get_option(argc, argv, &opt) ;

    FILE *new_fp = fopen(dst_file, "wb") ;

    if(new_fp == NULL)
        goto exit_fopen ;

    if(opt == 'a'){
        Header *header ;
        archive(src_path, new_fp, header) ;
    }
    if(opt == 'l'){
        //list() ;
    }
    if(opt == 'e'){
        //extract() ;
    }

    fclose(new_fp) ;
    return 0 ;

exit_fopen:
    perror("fail to open file") ;
    exit(1) ;
}