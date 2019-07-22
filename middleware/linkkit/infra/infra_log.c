#include "infra_config.h"
#include <string.h>
#include <stdarg.h>
#include "infra_compat.h"
#include "infra_log.h"
#include "wrappers.h"
#include "ulog/ulog.h"

#if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED)

void IOT_SetLogLevel(IOT_LogLevel level)
{
    int             lvl = (int)level;
    if (level == LOG_NONE_LEVEL) {
        aos_set_log_level((aos_log_level_t)lvl);
    } else if (level > LOG_NONE_LEVEL && level < LOG_INFO_LEVEL) {
        aos_set_log_level((aos_log_level_t)(lvl + 1));
    } else if (level >= LOG_INFO_LEVEL) {
        aos_set_log_level((aos_log_level_t)(lvl + 2));
    }
}
#else
void IOT_SetLogLevel(IOT_LogLevel level)
{
    return;
}
#endif  /* #if defined(INFRA_LOG) && !defined(INFRA_LOG_ALL_MUTED) */

