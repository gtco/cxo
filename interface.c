#include "interface.h"

char *__env[1] = { 0 };
char **environ = __env;

/* devoptab for an example stream device called "com1" */
const devoptab_t devoptab_com1 = { "com1",
                                   com1_open_r,
                                   com1_close_r,
                                   com1_write_r,
                                   com1_read_r };

const devoptab_t *devoptab_list[] = {
   &devoptab_com1,  /* standard input */
   &devoptab_com1,  /* standard output */
   &devoptab_com1,  /* standard error */
   &devoptab_com1,  /* another device */
//   ... ,         /* and so on... */
   0             /* terminates the list */
};

int close(int file)
{
    return -1;
}

int execve(char *name, char **argv, char **env)
{
  errno=ENOMEM;
  return -1;
}

int fork() {
  errno=EAGAIN;
  return -1;
}

int fstat(int file, struct stat *st)
{
  st->st_mode = S_IFCHR;
  return 0;
}

int getpid()
{
  return 1;
}

// Query whether output stream is a terminal. For consistency with the other minimal implementations,
// which only support output to stdout, this minimal implementation is suggested:
int isatty(int file)
{
   return 1;
}

// Send a signal. Minimal implementation:
int kill(int pid, int sig)
{
  errno=EINVAL;
  return(-1);
}

// Establish a new name for an existing file. Minimal implementation:
int link(char *old, char *new)
{
  errno=EMLINK;
  return -1;
}

//Set position in a file. Minimal implementation:
int lseek(int file, int ptr, int dir)
{
    return 0;
}

//Open a file. Minimal implementation:
int open(const char *name, int flags, int mode)
{
    return -1;
}

// Read from a file. Minimal implementation:
int read(int file, char *ptr, int len)
{
    return 0;
}

// Increase program data space. As malloc and related functions depend on this,
// it is useful to have a working implementation. The following suffices for a
// standalone system; it exploits the symbol end automatically defined by the GNU linker.
caddr_t sbrk(int incr)
{
  extern char end;		/* Defined by the linker */
  static char *heap_end;
  char *prev_heap_end;

  if (heap_end == 0) {
    heap_end = &end;
  }
  prev_heap_end = heap_end;
  if (heap_end + incr > stack_ptr)
    {
      _write (1, "Heap and stack collision\n", 25);
      abort ();
    }

  heap_end += incr;
  return (caddr_t) prev_heap_end;
}







