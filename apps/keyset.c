#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"
#include <errno.h>


int main(char* args) {

	int argNum = get_argc(args);

	if(argNum < 2) {

        char buf[1024];

        setflag(1);
        read(0, buf, 1024);
        buf[strlen(buf) - 1] = '\0';
        setflag(0);


        activate_timer();

        int key = keyset(buf);

        if(key < 0) {
            if(errno == EINVAL)
                printerr("Kljuc nije ispravan. Mora biti stepen dvojke!\n");
            else printerr("Nepoznata greska!\n");
        }

	}else {
        printerr("Broj parametara nije ispravan! \n");
        _exit(-1);
	}


	
	_exit(-1);

}
