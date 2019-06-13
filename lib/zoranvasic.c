#define __LIBRARY__
#include <unistd.h>

_syscall1(int, keyset,char*,passphrase)
_syscall1(time_t, times, struct tms *, tbuf)
_syscall2(int,encr,int,fd,int,mode)
_syscall0(int, activate_timer)
_syscall1(int, setflag, int, flag)