#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"
#include <errno.h>


int main(char* args) {

	int argNum = get_argc(args);

	if(argNum != 1) {
		printerr("Broj parametara nije ispravan! \n");
		_exit(-1);
	}
	int key = keyset("");

	printstr("Kljic obrisan!");
	
	_exit(-1);
 
}
