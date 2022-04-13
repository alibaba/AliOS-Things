/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <das/platform.h>
#include <das/hardware.h>
#include <das/service.h>

#ifdef DAS_PLATFORM_LINUX
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>

#if (DAS_SERVICE_FSCAN_ENABLED)

static const char *default_protected_paths[] = {
    // library paths
    "/lib",
    "/lib64",
    "/usr/lib",
    "/usr/lib64",
    "/system/lib",
    "/system/lib64",

    // binary paths
    "/bin",
    "/sbin",
    "/usr/bin",
    "/usr/sbin",
    "/usr/libexec",
    "/system/bin",
    "/system/xbin",
    "/system/das/bin",
    "/system/sbin",

    // config files
    "/etc/init.d",
    "/etc/systemd",
    "/etc/inittab",
    "/etc/sysconfig",
    "/etc/sysctl.d",

    // the end.
    NULL,
};

static int protected_array_index = 0;

#define ATTR_MAX                128
#define SHA1_CHECKSUM_STR_MAX   (DAS_SHA1_LENGTH * 2 + 1)
#define MIN_ATTRIBUTE_STR       "mode:x|size:y|type:z"

typedef enum {
    FILE_TYPE_ERROR = -1,
    FILE_TYPE_FILE = 0,
    FILE_TYPE_LIBRARY = 1,
    FILE_TYPE_EXECUTABLE = 2,
    FILE_TYPE_SCRIPT = 3,
    FILE_TYPE_MAX = 255,
} att_file_type_t;

#define EI_MAG0		0		/* File identification byte 0 index */
#define ELFMAG0		0x7f		/* Magic number byte 0 */

#define EI_MAG1		1		/* File identification byte 1 index */
#define ELFMAG1		'E'		/* Magic number byte 1 */

#define EI_MAG2		2		/* File identification byte 2 index */
#define ELFMAG2		'L'		/* Magic number byte 2 */

#define EI_MAG3		3		/* File identification byte 3 index */
#define ELFMAG3		'F'		/* Magic number byte 3 */

#define PROC_SUPER_MAGIC      0x9fa0     /* /proc FS */
#define SYSFS_MAGIC           0x62656572
#define TMPFS_MAGIC           0x01021994

/* size of f_type differs from platform */
#if defined(__aarch64__) || defined(__x86_64__)
uint64_t g_special_fstype[] = {
#elif defined(__mips__) && defined(__LP64__) /* 64-bit MIPS. */
uint64_t g_special_fstype[] = {
#elif defined(__mips__) /* 32-bit MIPS (corresponds to the kernel's statfs64 type). */
uint32_t g_special_fstype[] = {
#else /* 32-bit ARM or x86 (corresponds to the kernel's statfs64 type). */
uint32_t g_special_fstype[] = {
#endif
    PROC_SUPER_MAGIC, // /proc
    SYSFS_MAGIC,      // /sys
    TMPFS_MAGIC,      // /dev, /run
};

#define SPECIAL_FSTYPE_NUM    (sizeof(g_special_fstype)/sizeof(g_special_fstype[0]))

typedef struct _fscan_context_t {
    char **dirs;
    uint32_t dir_index;
    uint32_t dir_count;
    
    struct dirent **files;
    uint32_t file_index;
    uint32_t file_count;

    uint32_t file_scanned_count;
} fscan_context_t;

static fscan_context_t fscan_ctx = {0};

static char **scan_paths = NULL;
static uint32_t scan_paths_max_cnt = 200;
static uint32_t scan_paths_cnt = 0;

extern int alphasort(const struct dirent** a, const struct dirent** b);

static bool _is_script_file(const char* image_path) {
    int path_len = strlen(image_path);
    if(path_len > 3 &&
        image_path[path_len - 1] == 'h' &&
        image_path[path_len - 2] == 's' &&
        image_path[path_len - 3] == '.') {
        return true;
    }
    return false;
}

// x.so || x.ko
static bool _is_library_file(const char* image_path) {
    int path_len = strlen(image_path);
    if(path_len > 3 &&
        image_path[path_len - 1] == 'o' &&
        (image_path[path_len - 2] == 's' || image_path[path_len - 2] == 'k') &&
        image_path[path_len - 3] == '.') {
        return true;
    }
    return false;
}

static int _get_file_type(const char* image_path, bool is_elf) {
    att_file_type_t file_type = FILE_TYPE_FILE;

    if(NULL == image_path || strlen(image_path) == 0) {
        DAS_LOG("image path is empty.\n");
        return file_type;
    }

    if(false == is_elf) {
        if (_is_script_file(image_path)) {
            file_type = FILE_TYPE_SCRIPT;
        } else {
            file_type = FILE_TYPE_FILE;
        }
    } else {
        if (_is_library_file(image_path)) {
            file_type = FILE_TYPE_LIBRARY;
        } else {
            file_type = FILE_TYPE_EXECUTABLE;
        }
    }

    return file_type;
}

static int _get_file_attr(const char *image_path,
                         char *attr,
                         int attr_buf_size, 
                         bool is_elf) {
    struct stat st;
    att_file_type_t file_type = FILE_TYPE_ERROR;
    
    if (NULL == image_path) {
        DAS_LOG("image path is empty.\n");
        return -1;
    }

    if (NULL == attr || attr_buf_size < 10) {
        DAS_LOG("attribute buffer is empty or too small.\n");
        return -1;
    }

    // get mode from stat
    if (stat(image_path, &st) == -1) {
        DAS_LOG("stat failure, errno=%d\n", errno);
        return -1;
    }

    file_type = _get_file_type(image_path, is_elf);

    snprintf(attr, attr_buf_size, 
        "mode:%o|size:%lld|type:%d", 
        st.st_mode, (long long)st.st_size, file_type);

    return 0;
}

static long _get_file_size(const char* file_path) {
    struct stat st;
    if(stat(file_path, &st) == 0) {
        return (st.st_size);
    } 
    return -1;
}

static int _update_file_checksum(const char* image_path,
                                bool *is_elf,
                                das_sum_context_t *sum_context,
                                das_sum_context_t *mac_context) {
    int ret = -1;
    unsigned char* data = MAP_FAILED;
    int file_fd = -1;
    long file_size = -1;

    if (image_path == NULL || strlen(image_path) == 0 || sum_context == NULL) {
        DAS_LOG("_update_file_checksum, args error.\n");
        goto exit;
    }

    file_fd = open(image_path, O_RDONLY);
    if (file_fd == -1) {
        DAS_LOG("_update_file_checksum, open %s error, %s\n", image_path, strerror(errno));
        goto exit;
    }

    file_size = _get_file_size(image_path);
    if (file_size == -1) {
        DAS_LOG("_update_file_checksum, _get_file_size %s error.\n", image_path);
        goto exit;
    }

    data = (unsigned char *)mmap(NULL, file_size, PROT_READ, MAP_SHARED, file_fd, 0);
    if (data == MAP_FAILED) {
        DAS_LOG("_update_file_checksum, mmap %s error.\n", image_path);
        goto exit;
    }

    if (is_elf) {
        *is_elf = false;

        if (file_size > 10 &&
            ((char*)data)[EI_MAG0] == ELFMAG0 &&
            ((char*)data)[EI_MAG1] == ELFMAG1 &&
            ((char*)data)[EI_MAG2] == ELFMAG2 &&
            ((char*)data)[EI_MAG3] == ELFMAG3) {
            *is_elf = true;
        }
    }

    if (sum_context) {
        das_sum_update(sum_context, data, file_size);
        ret = 0;
    }

    if (mac_context) {
        das_sum_update(mac_context, data, file_size);
        ret = 0;
    }

exit:
    if (file_fd != -1) {
      close(file_fd);
    }
    
    if (data != MAP_FAILED) {
        munmap(data, file_size);
    }

  return ret;
}

static bool _is_normal_fs_type(const char *path) {
    struct statfs fsinfo;
    unsigned long i;
    bool ret = true;
    
    if (0 == statfs(path, &fsinfo)) {
        for (i = 0; i < SPECIAL_FSTYPE_NUM; i++) {
            if (fsinfo.f_type == g_special_fstype[i]) {
                ret = false;
                break;
            }
        }
    } else {
        DAS_LOG("statfs : %s, errno = %d\n", path, errno);
        ret = false;
    }

    return ret;
}

static void _init_file_context() {
    protected_array_index = 0;

    memset(&fscan_ctx, 0, sizeof(fscan_context_t));

    scan_paths_cnt = 0;
    memset(scan_paths, 0, sizeof(char *) * scan_paths_max_cnt);
}

static int _fscan_init(void) {
    scan_paths_cnt = 0;
    scan_paths_max_cnt = 200;
    scan_paths = (char **)malloc(sizeof(char *) * scan_paths_max_cnt);
    if (scan_paths == NULL) {
        return -1;
    }

    _init_file_context();

    return 0;
}

static int _only_dirs(const struct dirent *d) {
    return (strcmp(d->d_name, "..") && strcmp(d->d_name, ".") && (d->d_type == DT_DIR));
}

static int _add_scan_path(const char* path, int cnt) {
    int ret = -1;
    size_t len = 0;

    if (path == NULL || (len = strlen(path)) == 0 || cnt == 0) {
        return ret;
    }

    if (scan_paths_cnt + cnt >= scan_paths_max_cnt) {
        scan_paths_max_cnt *= 2;
        scan_paths_max_cnt += cnt;
        char **tmp = malloc(scan_paths_max_cnt * sizeof(char *));
        if (tmp) {
            memcpy(tmp, scan_paths, scan_paths_cnt * sizeof(char *));
            free(scan_paths);
            scan_paths = tmp;
        } else {
            DAS_LOG("no memory, malloc scan_paths error.\n");
        }
    }

    if (scan_paths_cnt < scan_paths_max_cnt) {
        scan_paths[scan_paths_cnt] = malloc(sizeof(char) * (len + 1));
        if (scan_paths[scan_paths_cnt]) {
            strcpy(scan_paths[scan_paths_cnt], path);
            scan_paths[scan_paths_cnt][len] = '\0';
            scan_paths_cnt++;
            ret = 0;
        }
    }

    return ret;
}
static int _extract_dir(const char *path) {
    int ret = -1;
    int n = 0;
    struct dirent **namelist = NULL;

    if (path == NULL || strlen(path) == 0) {
        goto __exit;
    }
    
    if(!_is_normal_fs_type(path)) {
        goto __exit;
    }

    // add current directory.
    _add_scan_path(path, 1);

    // then add sub-directories.
    n = scandir(path, &namelist, _only_dirs, (void*)alphasort);
    if (n <= 0 || namelist == NULL) {
        goto __exit;
    }

    while(n--) {
        char path_tmp[PATH_MAX] = {0};

        strcpy(path_tmp, path);
        if (path[strlen(path) -1] != '/') {
            strcat(path_tmp, "/");
        }
        strcat(path_tmp, namelist[n]->d_name);
        
        _add_scan_path(path_tmp, 1);

        free(namelist[n]);
        namelist[n] = NULL;

        _extract_dir(path_tmp); 
    }
    
    ret = 0;

__exit:
    if (namelist) {
        free(namelist);
    }

    return ret;
}

static int _only_files(const struct dirent *d) {
    return (strcmp(d->d_name, "..") && strcmp(d->d_name, ".") && (d->d_type == DT_REG));
}

struct dirent **_get_dir_files(const char *path, int *cnt) {
    struct dirent **file_list = NULL;
    int n = 0;

    if (path == NULL || strlen(path) == 0) {
        goto __exit;
    }

    n = scandir(path, &file_list, _only_files, (void*)alphasort);
    if (n <= 0) {
        goto __exit;
    }

__exit:
    if (cnt) {
        *cnt = n;
    }

    return file_list;
}

static das_result_t _get_report_file_item(char *path, 
                                        size_t size, 
                                        das_service_state_t *state) {
    das_result_t result = DAS_ERROR_BAD_PARAMETERS;
    
    if (path == NULL || size == 0 || state == NULL) {
        return result;
    }

    if (state->status == DAS_SRV_STATUS_START) {
        _init_file_context();
    }

    if (fscan_ctx.dirs == NULL || fscan_ctx.dir_count == 0) {
        const char *dir = NULL;
        while ((dir = default_protected_paths[protected_array_index++]) != NULL) {
            struct stat s_buf;

            DAS_LOG("scan protected path: %s\n", dir);

            if (access(dir, F_OK) != 0) {
                DAS_LOG("dir %s is not exist.\n", path);
                continue;
            }

            if (stat(dir, &s_buf) != 0) {
                DAS_LOG("stat path %s failed, error: %s\n", dir, strerror(errno));
                continue;
            }

            if (S_ISDIR(s_buf.st_mode)) {
                scan_paths_cnt = 0;
                memset(scan_paths, 0, scan_paths_max_cnt * sizeof(char *));

                _extract_dir(dir);

                if (scan_paths_cnt == 0) {
                    continue;
                }

                DAS_LOG("scan protected path: %s, sub dirs: %d\n", dir, scan_paths_cnt);

                fscan_ctx.dirs = scan_paths;
                fscan_ctx.dir_count = scan_paths_cnt;
                fscan_ctx.dir_index = 0;
                break;
            } else if (S_ISREG(s_buf.st_mode)) {
                // publish
                strncpy(path, dir, size);
                state->status = DAS_SRV_STATUS_PUBLISH;
                break;
            }
        }

        if (dir == NULL) {
            _init_file_context();
            state->status = DAS_SRV_STATUS_FINISH;
        }
    }

    if (fscan_ctx.dirs && fscan_ctx.dir_count && 
        fscan_ctx.file_count == 0 && fscan_ctx.files == NULL) {
        while (fscan_ctx.dir_index < fscan_ctx.dir_count) {
            int cnt = 0;
            const char *dir = fscan_ctx.dirs[fscan_ctx.dir_index];
            struct dirent **files = _get_dir_files(dir, &cnt);
            if (files == NULL || cnt <= 0) {
                fscan_ctx.dir_index++;
                continue;
            } else {
                DAS_LOG("scan sub dir: %s, sub files: %d\n", dir, cnt);
                fscan_ctx.file_index = 0;
                fscan_ctx.file_count = cnt;
                fscan_ctx.files = files;
                break;
            }
        }

        if (fscan_ctx.dir_index >= fscan_ctx.dir_count) {
            memset(&fscan_ctx, 0, sizeof(fscan_context_t));

            scan_paths_cnt = 0;
            memset(scan_paths, 0, sizeof(char *) * scan_paths_max_cnt);

            state->status = DAS_SRV_STATUS_NEXT;
        }
    }

    if (fscan_ctx.file_count && 
        fscan_ctx.files && 
        fscan_ctx.dirs && 
        fscan_ctx.dir_index < fscan_ctx.dir_count) {
        if (fscan_ctx.file_index < fscan_ctx.file_count) {
            const char *dir = fscan_ctx.dirs[fscan_ctx.dir_index];
            const char *name = fscan_ctx.files[fscan_ctx.file_index]->d_name;

            // publish
            strncpy(path, dir, size);
            if (path[strlen(path) - 1] != '/') {
                strncat(path, "/", size - strlen(path) -1);
            }
            strncat(path, name, size - strlen(path) -1);

            free(fscan_ctx.files[fscan_ctx.file_index]);

            fscan_ctx.file_index++;

            state->status = DAS_SRV_STATUS_PUBLISH;
        } else {
            free(fscan_ctx.dirs[fscan_ctx.dir_index]);
            fscan_ctx.dirs[fscan_ctx.dir_index] = NULL;

            fscan_ctx.dir_index++;

            free(fscan_ctx.files);
            fscan_ctx.files = NULL;

            fscan_ctx.file_index = 0;
            fscan_ctx.file_count = 0;

            state->status = DAS_SRV_STATUS_NEXT;
        }
    }

    result = DAS_SUCCESS;
    return result;
}

/* 
    path:/lib/modules/4.14.81/bluetooth.ko,
    attribute:mode:100644|size:464280|type:1,
    signature:4c66afa86e373d07662686c4ba1a3ca84e51f4f6
*/
static das_result_t _fscan_attest(char *path,
                                size_t path_size,
                                char *attribute,
                                size_t attribute_size,
                                das_sum_context_t *sum_context,
                                das_service_state_t *state) {
    das_result_t result = DAS_ERROR_BAD_PARAMETERS;
    char publish_file_path[PATH_MAX] = {0};

    if (path == NULL ||
        sum_context == NULL || state == NULL ||
        attribute == NULL || attribute_size < strlen(MIN_ATTRIBUTE_STR)) {
        return result;
    }

    result = _get_report_file_item(publish_file_path, PATH_MAX, state);
    if (result != DAS_SUCCESS) {
        DAS_LOG("_get_report_file_item error.\n");
        return result;
    }

    if (strlen(publish_file_path)) {
        char file_attr[ATTR_MAX] = { 0 };
        bool is_elf = false;
        int ret = 0;

        // get file hash
        ret = _update_file_checksum(publish_file_path, 
                                &is_elf,
                                sum_context,
                                NULL);
        if (ret == -1) {
            DAS_LOG("_update_file_checksum failed: %s\n", publish_file_path);
            state->status = DAS_SRV_STATUS_NEXT;
            return result;
        }

        // get file attributes
        ret = _get_file_attr(publish_file_path, file_attr, ATTR_MAX, is_elf);
        if (ret == -1) {
            DAS_LOG("_get_file_attr failed: %s\n", publish_file_path);
            state->status = DAS_SRV_STATUS_NEXT;
            return result;
        }

        // report
        DAS_LOG("report path:%s, %s\n", publish_file_path, file_attr);
        
        size_t len = attribute_size > strlen(file_attr) ? strlen(file_attr) : (attribute_size - 1);
        memcpy(attribute, file_attr, len * sizeof(char));
        attribute[len] = '\0';

        len = path_size > strlen(publish_file_path) ? strlen(publish_file_path) : (path_size - 1);
        memcpy(path, publish_file_path, len * sizeof(char));
        path[len] = '\0';

        fscan_ctx.file_scanned_count++;
    } else {
        DAS_LOG("publish_file_path is null.\n");
    }
  
    return result;
}

static das_result_t _fscan_measure(das_sum_context_t *sum_context,
                                das_mac_context_t *mac_context,
                                das_service_state_t *state) {
    das_result_t result = DAS_ERROR_BAD_PARAMETERS;
    char publish_file_path[PATH_MAX] = {0};

    if (sum_context == NULL || state == NULL) {
        return result;
    }

    result = _get_report_file_item(publish_file_path, PATH_MAX, state);
    if (result != DAS_SUCCESS) {
        DAS_LOG("_get_report_file_item error.\n");
        return result;
    }

    if (strlen(publish_file_path)) {
        int ret = _update_file_checksum(publish_file_path, 
                                NULL,
                                sum_context, 
                                mac_context);
        if (ret == -1) {
            DAS_LOG("_update_file_checksum failed: %s\n", publish_file_path);
            state->status = DAS_SRV_STATUS_NEXT;
            return result;
        }

        DAS_LOG("measure file: %s.\n", publish_file_path);
        fscan_ctx.file_scanned_count++;
    } else {
        DAS_LOG("publish_file_path is null.\n");
    }

    return result;
}

das_service_t service_fscan = {
    .name = "FSCAN",
    .init = _fscan_init,
    .info = NULL,
    .attest = _fscan_attest,
    .measure = _fscan_measure,
};

#endif // end DAS_SERVICE_FSCAN_ENABLED

#endif // DAS_PLATFORM_LINUX