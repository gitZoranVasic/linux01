#include <unistd.h>
#define UTIL_IMPLEMENTATION
#include "utils.h"
#include <errno.h>
#include <string.h>

#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>

uint64_t seed;

/*int rand() {
	if(seed == 0) seed = (times(NULL) * 1367);	
	int i,j = 36000;
	int check = 1;
	
	while(j > 0) {
		j--;
}


    //slow down the program for times to cathe up

	i = (times(NULL) * 1367) + seed;
	i = i + '!';	
	i = i%('~'-'!');

	if(i < 0) i = -i;
    vardump(i);
	//return the value in the first 128 of ascii characters as int
	seed = i;
	return i;
}*/

int rand(void)
{
    seed = 345234523462347623*seed + 1;
    int i = seed>>33;
    i = i%122; //necu zagrade
    if(i < 33) i = i + 34;
    if(i > 91 && i < 96 || i > 58 && i < 64) i = i + 10; //necu karaktere!
    return i;

}

int main(char* args) {

	int choice = 0;
	
	int argNum = get_argc(args);

	if(argNum != 2) {
		printerr("Broj parametara nije ispravan! \n");
		_exit(-1);
	}


	choice = atoi(get_argv(args, 1));

	if(choice < 1 || choice > 3) {
		printerr("Parametar nije ispravan!\n");
		_exit(-1);
	}



    int i, j = 2;

	for(i = 0; i < choice; i++) {
		j = j * 2;
	}

	char c[j];
	//j mi prestavlja za sledecu petlju koliko treba slova da generisem! 
	
	seed = (times(NULL) * 1367) - 1;
	for(i = 0; i < j; i++) {
		c[i] = rand();
	}

	println(c);

 	_exit(-1);
}
