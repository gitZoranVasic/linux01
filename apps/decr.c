#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>



int main(char* args) {

    int argc = get_argc(args);
    if (argc < 2) {
        printstr("Nedovoljno argumenata\n");
        _exit(1);
    }

    int fd = open(get_argv(args, 1), O_RDWR);
    if(errno == 21) {
        fd = open(get_argv(args, 1), O_RDONLY);
    }

    if (fd < 0){
        printstr("Fajl ne postoji, poklusaj ponovo! \n");
        _exit(1);
    }

    encr(fd,2);
    close(fd);

    _exit(0);

}
