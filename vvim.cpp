#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

using namespace std;


int mk(const char* path, mode_t mode){
    mode_t old_mask = umask(0);
    int ans = mkdir(path, 0);
    if(ans==0) chmod(path, S_IRWXU | S_IRWXG | S_IRWXO);
    return ans;
}
int mkdirs(const char* path, mode_t mode){
    char tmpBuffer[2048];
    const char* tmp = path;
    int len = 0;
    int ret;

    if(path == NULL || strlen(path) >= 2048) return -1;
    if(tmp[0] == '/') tmp++;

    while((tmp = strchr(tmp, '/')) != NULL){
        len = tmp - path;
        tmp++;

        if(len == 0) continue;
        strncpy(tmpBuffer, path, len);
        tmpBuffer[len] = 0;

        if((ret=mk(tmpBuffer, mode)) == -1){
//            printf("f: %s %s %d\n", tmpBuffer, path, len);
            if(errno != EEXIST) return -1;
        }
    }
    int totalLen = strlen(path);
    if(path[totalLen-1] == '/') return mk(path, mode);    
    return 0;
}
static const char * VIRTUAL_CONFIG_PATH = "/etc/vvim/";
static const char * VIRTUAL_DIRECTORY = "/usr/local/vvim_dir/";

void init(){
    if(access(VIRTUAL_CONFIG_PATH, F_OK) == -1){
        printf("[SYSTEM] First init: make file %s state: %d\n", VIRTUAL_CONFIG_PATH, mkdirs(VIRTUAL_CONFIG_PATH, 0776));
    }
    if(access(VIRTUAL_DIRECTORY, F_OK) == -1){
        printf("[SYSTEM] First init: make file %s state: %d\n", VIRTUAL_DIRECTORY, mkdirs(VIRTUAL_DIRECTORY, 0776));
    }
}
char command[4096];
int main(int argv, char** args){
    init();
    char work_directory[2048];
    errno = 0;
    if(getcwd(work_directory, 2048) == NULL){
        if(errno == ERANGE){
            printf("[SYSTEM] Error: pathname length exceeds buffer size %d\n", 2048);
        }else perror("getcwd");
        exit(EXIT_FAILURE);
    }

    int vd_len = strlen(VIRTUAL_DIRECTORY);
    int wd_len = strlen(work_directory);
    for (int i = 1; i < argv; i++)
    {
        char org[4096];
        char dst[4096];
        char res[4096];
        char* org_ptr = org;
        strcpy(org_ptr, work_directory);
        org_ptr += wd_len;
        *org_ptr++ = '/';
        strcpy(org_ptr, args[i]);

        char* dst_p = &dst[0];
        strcpy(dst_p, VIRTUAL_DIRECTORY);
        dst_p += vd_len;
        strcpy(dst_p, org);

        printf("[SYSTEM] src path: %s\n", org);
        printf("[SYSTEM] dst path: %s\n", dst);

        if(mkdirs(dst, 0776)==-1){
            printf("mkdir fail\n");
            continue;
        }
        
        if(access(dst, F_OK) == -1){
            int src_fd, dst_fd;
            if ((src_fd = open(org, O_RDONLY)) == -1)
            {
                printf("%s open fail\n", org);
                continue;
            }
            if ((dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
            {
                printf("%s open fail\n", dst);
                continue;
            }
            char contents[1024];
            int stat = 0;
            while ((stat = read(src_fd, contents, 1024)) > 0)
            {
                write(dst_fd, contents, stat);
            }
            close(src_fd), close(dst_fd);
        }

        sprintf(res, "vim %s", dst);
        system(res);
    }
}
 