#ifndef __FS_ADAPT_H__
#define __FS_ADAPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

    void fs_adapt_init(void);
    int flush(int fd);
    int filesize(int fd);
    int relax(int fd, int size);
    int open(const char *pathname, int flags, ...);
    uint32_t read(int fd, void *buf, size_t count);
    uint32_t write(int fd, const void *buf, size_t nbyte);
    int lseek(int fildes, int offset, int whence);
    //int _is_virtual_path(const char *path);
    int close(int fd);
#ifdef __cplusplus
}
#endif

#endif /* __FS_ADAPT_H__ */