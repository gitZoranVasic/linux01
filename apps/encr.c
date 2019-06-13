#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


int main(char* args) {

        int mode = 1;
        int argc = get_argc(args);
        if (argc < 2) {
            printstr("Nedovoljno argumenata\n");
            _exit(1);
        }

        int fd = open(get_argv(args, 1), O_RDWR);
        if(errno == 21) {
            fd = open(get_argv(args, 1), O_RDONLY);
        }

/*        struct stat path_stat;
        stat(get_argv(args, 1), &path_stat);
        int isDir = isFile(get_argv(args, 1));*/

/*

    printstr("isDir \n");
    printstr("isDir \n");
    printstr("isDir \n");

    vardump(fd);

    printstr("isDir \n");
    printstr("isDir \n");
*/




    if (fd < 0){
            printstr("Fajl ne postoji, poklusaj ponovo! \n");
            _exit(1);
        }

        encr(fd,mode);
        close(fd);

        _exit(0);

}
