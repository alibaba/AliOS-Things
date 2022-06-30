#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>

extern struct _reent *__getreent(void);
extern int _open_r(struct _reent *reent, const char *pathname, int flags, int mode);

int open(const char *pathname, int flags, ...)
{
    mode_t mode;

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode = va_arg(ap, mode_t);
        va_end(ap);
    } else {
        mode = 0;
    }

    return _open_r(__getreent(), pathname, flags, mode);
}

extern int _close_r(struct _reent *reent, int fd);

int close(int fd)
{
    return _close_r(__getreent(), fd);
}

extern ssize_t _read_r(struct _reent *reent, int fd, void *buf, size_t count);

ssize_t read(int fd, void *buf, size_t count)
{
    return _read_r(__getreent(), fd, buf, count);
}

extern ssize_t _write_r(struct _reent *reent, int fd, const void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count)
{
    return _write_r(__getreent(), fd, buf, count);
}

extern off_t _lseek_r(struct _reent *reent, int fd, off_t offset, int whence);

off_t lseek(int fd, off_t offset, int whence)
{
    return _lseek_r(__getreent(), fd, offset, whence);
}

extern int _unlink_r(struct _reent *reent, const char *pathname);

int unlink(const char *pathname)
{
    return _unlink_r(__getreent(), pathname);
}

extern int _stat_r(struct _reent *reent, const char *pathname, struct stat *buf);

int stat(const char *pathname, struct stat *buf)
{
    return _stat_r(__getreent(), pathname, buf);
}

extern int _fstat_r(struct _reent *reent, int fd, struct stat *buf);

int fstat(int fd, struct stat *buf)
{
    return _fstat_r(__getreent(), fd, buf);
}

extern int _getpid_r(struct _reent *reent);

pid_t getpid(void)
{
    return _getpid_r(__getreent());
}
