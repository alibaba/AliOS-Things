#ifndef __SDK_IMPL_INTERNAL_H__
#define __SDK_IMPL_INTERNAL_H__

#define sdk_emerg(...)          log_emerg("sdk", __VA_ARGS__)
#define sdk_crit(...)           log_crit("sdk", __VA_ARGS__)
#define sdk_err(...)            log_err("sdk", __VA_ARGS__)
#define sdk_warning(...)        log_warning("sdk", __VA_ARGS__)
#define sdk_info(...)           log_info("sdk", __VA_ARGS__)
#define sdk_debug(...)          log_debug("sdk", __VA_ARGS__)

#endif  /* __SDK_IMPL_INTERNAL_H__ */
