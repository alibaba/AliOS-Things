/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <pthread.h>
#include <enviro.h>
#include <aos/kernel.h>

pthread_mutex_t g_enviro_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_environ_t *g_penviron = NULL;

struct _envval {
    const char *valname;
    struct _envval *next;
};

static struct _envval *g_penvval_deprecated = NULL;
static int envval_deprecat(const char *valname);

static pthread_environ_t *env_new(const char *envname, const char *envval)
{
    pthread_environ_t *penv = NULL;
    int envname_len, envval_len;

    /* malloc pthread_environ_t */
    penv = malloc(sizeof(pthread_environ_t));
    if (penv == NULL) {
        return NULL;
    }

    /* malloc envname and copy the envname */
    envname_len = strlen(envname);
    penv->envname = malloc(envname_len + 1);
    if (penv->envname == NULL) {
        free(penv);
        return NULL;
    }

    strncpy(penv->envname, envname, envname_len);
    penv->envname[envname_len] = '\0';

    /* malloc envval and copy the envval */
    envval_len = strlen(envval);
    penv->envval = malloc(envval_len + 1);
    if (penv->envval == NULL) {
        free(penv->envname);
        free(penv);
        return NULL;
    }

    strncpy(penv->envval, envval, envval_len);
    penv->envval[envval_len] = '\0';

    penv->next = NULL;

    return penv;
}

static void env_free(pthread_environ_t *penv)
{
    if (penv == NULL) {
        return;
    }

    if (penv->envname != NULL) {
        free(penv->envname);
    }

    if (penv->envval != NULL) {
        envval_deprecat(penv->envval);
    }

    free(penv);
}

static int envval_deprecat(const char *valname)
{
    struct _envval *ev;

    ev = malloc(sizeof(struct _envval));
    if (ev == NULL) {
        return -1;
    }

    ev->valname = valname;
    ev->next = NULL;

    if (g_penvval_deprecated == NULL) {
        g_penvval_deprecated = ev;
        return 0;
    }

    ev->next = g_penvval_deprecated;
    g_penvval_deprecated = ev;

    return 0;
}

int setenv(const char *envname, const char *envval, int overwrite)
{
    pthread_environ_t *penv     = NULL;
    pthread_environ_t *penv_pre = NULL;
    int envval_len;

    int ret = -1;

    if ((envname == NULL) || (envval == NULL)) {
        return -1;
    }

    ret = pthread_mutex_lock(&g_enviro_mutex);
    if (ret != 0) {
        return -1;
    }

    /* if no environ in tcb, create the first one */
    if (g_penviron == NULL) {
        penv = env_new(envname, envval);
        if (penv == NULL) {
            pthread_mutex_unlock(&g_enviro_mutex);
            return -1;
        }

        g_penviron = penv;

        pthread_mutex_unlock(&g_enviro_mutex);
        return 0;
    }

    /* search the environ list to find the match item */
    penv = g_penviron;
    while (penv != NULL) {
        if (strcmp(penv->envname, envname) == 0) {
            /* if the environment variable named by envname already exists and the value of overwrite is non-zero,
               the function shall return success and the environment shall be updated */
            if (overwrite != 0) {
                /* add the deprecated val in deprecated list */
                if (!envval_deprecat(penv->envval)) {
                    envval_len = strlen(envval);
                    penv->envval = malloc(envval_len + 1);
                    strncpy(penv->envval, envval, envval_len);
                    penv->envval[envval_len] = '\0';
                    pthread_mutex_unlock(&g_enviro_mutex);
                    return 0;
                } else {
                    pthread_mutex_unlock(&g_enviro_mutex);
                    return -1;
                }

            } else {
            /* If the environment variable named by envname already exists and the value of overwrite is zero, the
               function shall return success and the environment shall remain unchanged */
                pthread_mutex_unlock(&g_enviro_mutex);
                return 0;
            }
        }

        penv_pre = penv;
        penv = penv->next;
    }

    /* if no match item create one and add to the end of list */
    penv = env_new(envname, envval);
    if (penv == NULL) {
        pthread_mutex_unlock(&g_enviro_mutex);
        return -1;
    }

    penv_pre->next = penv;

    pthread_mutex_unlock(&g_enviro_mutex);

    return 0;
}

char *getenv(const char *name)
{
    int   ret;
    char *val = NULL;
    pthread_environ_t *penv = NULL;

    if (name == NULL) {
        return NULL;
    }

    penv = g_penviron;
    if (penv == NULL) {
        return NULL;
    }

    ret = pthread_mutex_lock(&g_enviro_mutex);
    if (ret != 0) {
        return NULL;
    }

    /* search the environ list to find the match item */
    while (penv != NULL) {
        if (strcmp(penv->envname, name) == 0) {
            val = penv->envval;
            pthread_mutex_unlock(&g_enviro_mutex);
            return val;
        }

        penv = penv->next;
    }

    pthread_mutex_unlock(&g_enviro_mutex);
    return NULL;
}

int unsetenv(const char *name)
{
    pthread_environ_t *penv     = NULL;
    pthread_environ_t *penv_pre = NULL;

    int ret = -1;

    if (name == NULL) {
        return -1;
    }

    penv = g_penviron;
    if (penv == NULL) {
        return -1;
    }

    ret = pthread_mutex_lock(&g_enviro_mutex);
    if (ret != 0) {
        return -1;
    }

    /* search the environ list to find the match item and free it */
    while (penv != NULL) {
        if (strcmp(penv->envname, name) == 0) {
            if (penv_pre == NULL) {
                g_penviron = penv->next;
            } else {
                penv_pre->next = penv->next;
            }

            pthread_mutex_unlock(&g_enviro_mutex);

            /* free the pthread_environ_t data */
            env_free(penv);

            return 0;
        }

        penv_pre = penv;
        penv = penv->next;
    }

    pthread_mutex_unlock(&g_enviro_mutex);

    return -1;
}

int putenv(char *string)
{
    int   pos     = 0;
    char *envname = NULL;
    char *envval  = NULL;
    int   ret     = -1;

    for (pos = 0; pos < strlen(string); pos++) {
        if (string[pos] == '=') {
            envval = &string[pos + 1];

            /* malloc a memory to save envname */
            envname = malloc(pos + 1);
            if (envname == NULL) {
                return -1;
            }

            /* copy envname */
            strncpy(envname, string, pos);
            envname[pos] = '\0';

            ret = setenv(envname, envval, 1);

            /* free envname */
            free(envname);

            return ret;
        }
    }

    return -1;
}

int clearenv(void)
{
    pthread_environ_t *env;
    pthread_environ_t *next;
    struct _envval *envval, *envval_next;

    env = g_penviron;
    while (env != NULL) {
        next= env->next;
        if (env->envname != NULL) {
            free(env->envname);
        }

        if (env->envval != NULL) {
            free(env->envval);
        }

        free(env);
        env = next;
    }

    g_penviron = NULL;

    envval = g_penvval_deprecated;
    while (envval != NULL) {
        envval_next = envval->next;
        free((void*)envval->valname);
        free(envval);
        envval = envval_next;
    }

    g_penvval_deprecated = NULL;

    return 0;
}

int uname(struct utsname *name)
{
    const char *os      = "AliOS Things";

    if (name == NULL) {
        return -1;
    }

    memset(name, 0, sizeof(struct utsname));

    aos_version_str_get(name->version, _UTSNAME_VERSION_LENGTH - 1);

    strncpy(name->sysname, os, _UTSNAME_SYSNAME_LENGTH - 1);

    return 0;
}

long sysconf(int name)
{
    long val = 0;

    switch (name) {
    case _SC_JOB_CONTROL :
        val = _POSIX_JOB_CONTROL;
        break;
    case _SC_SAVED_IDS :
        val = _POSIX_SAVED_IDS;
        break;
    case _SC_VERSION :
        val = _POSIX_VERSION;
        break;
    case _SC_ASYNCHRONOUS_IO :
        val = _POSIX_ASYNCHRONOUS_IO;
        break;
    case _SC_FSYNC :
        val = _POSIX_FSYNC;
        break;
    case _SC_MAPPED_FILES :
        val = _POSIX_MAPPED_FILES;
        break;
    case _SC_MEMLOCK :
        val = _POSIX_MEMLOCK;
        break;
    case _SC_MEMLOCK_RANGE :
        val = _POSIX_MEMLOCK_RANGE;
        break;
    case _SC_MEMORY_PROTECTION :
        val = _POSIX_MEMORY_PROTECTION;
        break;
    case _SC_MESSAGE_PASSING :
        val = _POSIX_MESSAGE_PASSING;
        break;
    case _SC_PRIORITIZED_IO :
        val = _POSIX_PRIORITIZED_IO;
        break;
    case _SC_REALTIME_SIGNALS :
#if (_POSIX_REALTIME_SIGNALS > 0)
            val = 1;
#else
            val = 0;
#endif
        break;
    case _SC_SEMAPHORES :
        val = _POSIX_SEMAPHORES;
        break;
    case _SC_SYNCHRONIZED_IO :
        val = _POSIX_SYNCHRONIZED_IO;
        break;
    case _SC_TIMERS :
        val = _POSIX_TIMERS;
        break;
    case _SC_BARRIERS :
        val = _POSIX_BARRIERS;
        break;
    case _SC_READER_WRITER_LOCKS :
        val = _POSIX_READER_WRITER_LOCKS;
        break;
    case _SC_SPIN_LOCKS :
        val = _POSIX_SPIN_LOCKS;
        break;
    case _SC_THREADS :
        val = _POSIX_THREADS;
        break;
    case _SC_THREAD_ATTR_STACKADDR :
        val = _POSIX_THREAD_ATTR_STACKADDR;
        break;
    case _SC_THREAD_ATTR_STACKSIZE :
        val = _POSIX_THREAD_ATTR_STACKSIZE;
        break;
    case _SC_THREAD_PRIORITY_SCHEDULING :
        val = _POSIX_THREAD_PRIORITY_SCHEDULING;
        break;
    case _SC_THREAD_PRIO_INHERIT :
        val = _POSIX_THREAD_PRIO_INHERIT;
        break;
    case _SC_THREAD_PRIO_PROTECT :
        val = _POSIX_THREAD_PRIO_PROTECT;
        break;
    case _SC_THREAD_PROCESS_SHARED :
        val = _POSIX_THREAD_PROCESS_SHARED;
        break;
    case _SC_THREAD_SAFE_FUNCTIONS :
        val = _POSIX_THREAD_SAFE_FUNCTIONS;
        break;
    case _SC_SPAWN :
        val = _POSIX_SPAWN;
        break;
    case _SC_TIMEOUTS :
        val = _POSIX_TIMEOUTS;
        break;
    case _SC_CPUTIME :
        val = _POSIX_CPUTIME;
        break;
    case _SC_THREAD_CPUTIME :
        val = _POSIX_THREAD_CPUTIME;
        break;
    case _SC_ADVISORY_INFO :
        val = _POSIX_ADVISORY_INFO;
        break;
    default:
        val = -1;
        break;
    }

    return val;
}

size_t confstr(int name, char *buf, size_t len)
{
    int len_real = 0;

    if (name == _CS_GNU_LIBC_VERSION) {
        len_real = strlen(_POSIX_GNU_LIBC_VERSION);

        if (len < len_real) {
            return 0;
        }

        strncpy(buf, _POSIX_GNU_LIBC_VERSION, len);

        return len_real;
    } else if (name == _CS_GNU_LIBPTHREAD_VERSION) {
        len_real = strlen(_POSIX_GNU_LIBPTHREAD_VERSION);

        if (len < len_real) {
            return 0;
        }

        strncpy(buf, _POSIX_GNU_LIBPTHREAD_VERSION, len);

        return len_real;
    } else {
        return 0;
    }
}
