#ifndef _INTERFACE_H_
#define _INTERFACE_H_

extern char *__env[];
extern char **environ;

typedef struct {
   const char *name;
   int  (*open_r  )( struct _reent *r, const char *path, int flags, int mode );
   int  (*close_r )( struct _reent *r, int fd );
   long (*write_r )( struct _reent *r, int fd, const char *ptr, int len );
   long (*read_r  )( struct _reent *r, int fd, char *ptr, int len );

} devoptab_t;

int  open_r(struct _reent *r, const char *path, int flags, int mode );
int  close_r(struct _reent *r, int fd );
long write_r(struct _reent *r, int fd, const char *ptr, int len );
long read_r(struct _reent *r, int fd, char *ptr, int len );

int fork_r(struct _reent *r);
int execve_r(struct _reent *r, char *name, char **argv, char **env);
int kill(int pid, int sig);
// wait_r
int getpid_r(struct _reent *r);

int isatty_r(struct _reent *r, int file);

int stat_r (struct _reent *_r, const char *file, struct stat *pstat);
int fstat_r (struct _reent *_r, int fd, struct stat *pstat );

int link_r ( struct _reent *_r, const char *oldname, const char *newname );
int unlink_r ( struct _reent *_r, const char *name );
off_t lseek_r( struct _reent *_r, int fd, off_t pos, int whence );

int times_r( struct _reent *r, struct tms *tmsbuf);

caddr_t sbrk_r(int incr);

#endif