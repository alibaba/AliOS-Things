/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_DEBUG_H
#define AOS_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#define SHORT_FILE __FILENAME__

#define debug_print_assert(A,B,C,D,E,F)

#if (!defined(unlikely))
#define unlikely(EXPRESSSION) !!(EXPRESSSION)
#endif

/*
 * Check that an expression is true (non-zero).
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method.
 *
 * @param[in]  X  expression to be evaluated.
 */
#if (!defined(check))
#define check(X)                                                                            \
        do {                                                                                \
            if (unlikely(!(X))) {                                                           \
                debug_print_assert(0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
            }                                                                               \
        } while(1 == 0)
#endif

/*
 * Check that an expression is true (non-zero) with an explanation.
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method.
 *
 * @param[in]  X       expression to be evaluated.
 * @param[in]  STR     If the expression evaluate to false, custom string to print.
 */
#if (!defined(check_string))
#define check_string(X, STR)                                                               \
        do {                                                                               \
            if (unlikely(!(X))) {                                                          \
                debug_print_assert(0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                AOS_ASSERTION_FAIL_ACTION();                                               \
            }                                                                              \
        } while(1 == 0)
#endif

/*
 * Requires that an expression evaluate to true.
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method then jumps to a label.
 *
 * @param[in]  X      expression to be evalulated.
 * @param[in]  LABEL  if expression evaluate to false,jumps to the LABEL.
 */
#if (!defined(require))
#define require(X, LABEL)                                                                     \
        do {                                                                                  \
            if (unlikely(!(X))) {                                                             \
                debug_print_assert( 0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__ ); \
                goto LABEL;                                                                   \
            }                                                                                 \
        } while(1 == 0)
#endif

/*
 * Requires that an expression evaluate to true with an explanation.
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) and a custom explanation string using the default debugging output method then jumps to a label.
 *
 * @param[in]  X      expression to be evalulated.
 * @param[in]  LABEL  if expression evaluate to false,jumps to the LABEL.
 * @param[in]  STR    if expression evaluate to false,custom explanation string to print.
 */
#if (!defined(require_string))
#define require_string(X, LABEL, STR)                                                      \
        do {                                                                               \
            if (unlikely(!(X))) {                                                          \
                debug_print_assert(0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                goto LABEL;                                                                \
            }                                                                              \
        } while(1 == 0)
#endif

/*
 * Requires that an expression evaluate to true.
 * If expression evalulates to false, this jumps to a label. No debugging information is printed.
 *
 * @param[in]  X      expression to be evalulated
 * @param[in]  LABEL  if expression evaluate to false,jumps to the LABEL.
 */
#if (!defined(require_quiet))
#define require_quiet(X, LABEL)   \
        do {                      \
            if (unlikely(!(X))) { \
                goto LABEL;       \
            }                     \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0).
 * If the error code is non-0, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method then jumps to a label.
 *
 * @param[in]  ERR    error to be evaluated
 * @param[in]  LABEL  If the error code is non-0,jumps to the LABEL.
 */
#if (!defined(require_noerr))
#define require_noerr(ERR, LABEL)                                                                    \
        do {                                                                                         \
            int localErr;                                                                            \
                                                                                                     \
            localErr = (int)(ERR);                                                                   \
            if (unlikely(localErr != 0)) {                                                           \
                debug_print_assert(localErr, NULL, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                goto LABEL;                                                                          \
            }                                                                                        \
                                                                                                     \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0) with an explanation.
 * If the error code is non-0, this prints debugging information (actual expression string, file, line number,
 * function name, etc.), and a custom explanation string using the default debugging output method using the
 * default debugging output method then jumps to a label.
 *
 * @param[in]  ERR    error to be evaluated
 * @param[in]  LABEL  If the error code is non-0, jumps to the LABEL.
 * @param[in]  STR    If the error code is non-0, custom explanation string to print
 */
#if (!defined(require_noerr_string))
#define require_noerr_string(ERR, LABEL, STR)                                                       \
        do {                                                                                        \
            int localErr;                                                                           \
                                                                                                    \
            localErr = (int)(ERR);                                                                  \
            if (unlikely(localErr != 0)) {                                                          \
                debug_print_assert(localErr, NULL, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                goto LABEL;                                                                         \
            }                                                                                       \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0)  with an explanation and action to execute otherwise.
 * If the error code is non-0, this prints debugging information (actual expression string, file, line number,
 * function name, etc.), and a custom explanation string using the default debugging output method using the
 * default debugging output method then executes an action and jumps to a label.
 *
 * @param[in]  ERR     error to be evaluated.
 * @param[in]  LABEL   If the error code is non-0, jumps to the LABEL.
 * @param[in]  ACTION  If the error code is non-0, custom code to executes.
 * @param[in]  STR     If the error code is non-0, custom explanation string to print.
 */
#if (!defined(require_noerr_action_string))
#define require_noerr_action_string(ERR, LABEL, ACTION, STR)                                        \
        do {                                                                                        \
            int localErr;                                                                           \
                                                                                                    \
            localErr = (int)(ERR);                                                                  \
            if (unlikely(localErr != 0)) {                                                          \
                debug_print_assert(localErr, NULL, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                { ACTION; }                                                                         \
                goto LABEL;                                                                         \
            }                                                                                       \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0).
 * If the error code is non-0, this jumps to a label. No debugging information is printed.
 *
 * @param[in]  ERR    error to be evaluated.
 * @param[in]  LABEL  If the error code is non-0, jumps to the LABEL.
 */
#if (!defined(require_noerr_quiet))
#define require_noerr_quiet(ERR, LABEL) \
        do {                            \
            if (unlikely((ERR) != 0)) { \
                goto LABEL;             \
            }                           \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0) with an action to execute otherwise.
 * If the error code is non-0, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method then executes an action and jumps to a label.
 *
 * @param[in]  ERR     error to be evaluated.
 * @param[in]  LABEL   If the error code is non-0, jumps to the LABEL.
 * @param[in]  ACTION  If the error code is non-0, custom code to executes.
 */
#if (!defined(require_noerr_action))
#define require_noerr_action(ERR, LABEL, ACTION)                                                     \
        do {                                                                                         \
            int localErr;                                                                            \
                                                                                                     \
            localErr = (int)(ERR);                                                                   \
            if (unlikely(localErr != 0)) {                                                           \
                debug_print_assert(localErr, NULL, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                { ACTION; }                                                                          \
                goto LABEL;                                                                          \
            }                                                                                        \
        } while(1 == 0)
#endif

/*
 * Require that an error code is noErr (0) with an action to execute otherwise.
 * If the error code is non-0, this executes an action and jumps to a label. No debugging information is printed.
 *
 * @param[in]  ERR     error to be evaluated.
 * @param[in]  LABEL   If the error code is non-0, jumps to the LABEL.
 * @param[in]  ACTION  If the error code is non-0, custom code to executes.
 */
#if (!defined(require_noerr_action_quiet))
#define require_noerr_action_quiet(ERR, LABEL, ACTION) \
        do {                                           \
            if (unlikely((ERR) != 0)) {                \
                { ACTION; }                            \
                goto LABEL;                            \
            }                                          \
        } while(1 == 0)
#endif

/*
 * Requires that an expression evaluate to true with an action to execute otherwise.
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) using the default debugging output method then executes an action and jumps to a label.
 *
 * @param[in]  X       expression to be evaluated.
 * @param[in]  LABEL   If the expression evaluate to false, jumps to the LABEL.
 * @param[in]  ACTION  If the expression evaluate to false, custom code to executes.
 */
#if (!defined(require_action))
#define require_action(X, LABEL, ACTION)                                                    \
        do {                                                                                \
            if (unlikely(!(X))) {                                                           \
                debug_print_assert(0, #X, NULL, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                { ACTION; }                                                                 \
                goto LABEL;                                                                 \
            }                                                                               \
        } while (1 == 0)
#endif

/*
 * Requires that an expression evaluate to true with an explanation and action to execute otherwise.
 * If expression evalulates to false, this prints debugging information (actual expression string, file, line number,
 * function name, etc.) and a custom explanation string using the default debugging output method then executes an
 * action and jumps to a label.
 *
 * @param[in]  X       expression to be evaluated.
 * @param[in]  LABEL   If the expression evaluate to false, jumps to the LABEL.
 * @param[in]  ACTION  If the expression evaluate to false, custom code to executes.
 * @param[in]  STR     If the expression evaluate to false, custom string to print.
 */
#if (!defined(require_action_string))
#define require_action_string(X, LABEL, ACTION, STR)                                       \
        do {                                                                               \
            if (unlikely(!(X))) {                                                          \
                debug_print_assert(0, #X, STR, SHORT_FILE, __LINE__, __PRETTY_FUNCTION__); \
                { ACTION; }                                                                \
                goto LABEL;                                                                \
            }                                                                              \
        } while (1 == 0)
#endif

/*
 * Requires that an expression evaluate to true with an action to execute otherwise.
 * If expression evalulates to false, this executes an action and jumps to a label.
 * No debugging information is printed.
 *
 * @param[in]  X       expression to be evaluated.
 * @param[in]  LABEL   If the expression evaluate to false, jumps to the LABEL.
 * @param[in]  ACTION  If the expression evaluate to false, custom code to executes.
 */
#if (!defined(require_action_quiet))
#define require_action_quiet(X, LABEL, ACTION) \
        do {                                   \
            if (unlikely(!(X))) {              \
                { ACTION; }                    \
                goto LABEL;                    \
            }                                  \
                                               \
        } while(1 == 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* AOS_DEBUG_H */

