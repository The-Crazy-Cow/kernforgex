// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

#include <stdio.h>

/**
 * @brief Log levels used by the logging system.
 *
 * Each log level is represented by a bit flag and can be combined
 * using bitwise operations with dbg_loglevel define in include/debug.c.
 *
 * By default, all log levels are enabled in dbg_loglevel.
 *
 * @note The pr_foo debug log levels are enabled only when DEBUG is defined.
 */

#define DBG_LOGLEVEL_FATAL 0x01
#define DBG_LOGLEVEL_WARN 0x02
#define DBG_LOGLEVEL_ERROR 0x04
#define DBG_LOGLEVEL_INFO 0x08
#define DBG_LOGLEVEL_DEBUG 0x10

/**
 * @brief Functions for managing the logging level mask.
 *
 * These functions allow retrieving, setting, enabling, and disabling
 * individual or multiple log levels using bitwise operations.
 *
 * @note Each log level is represented by a bit flag.
 */
unsigned int get_debug_loglevel(void);
unsigned int set_debug_loglevel(const unsigned int);
unsigned int disable_debug_loglevel(const unsigned int);
unsigned int enable_debug_loglevel(const unsigned int);

#ifdef DEBUG
#define dbg_log_debug(fmt, ...)                                                \
    do {                                                                       \
        fprintf(                                                               \
            stderr,                                                            \
            "\n%s:%s:%d--|" fmt "\n",                                          \
            __func__,                                                          \
            __FILE__,                                                          \
            __LINE__,                                                          \
            ##__VA_ARGS__);                                                    \
        fflush(stderr);                                                        \
    } while (0)
#else
#define dbg_log_debug(...) ((void)0)
#endif /*DEBUG*/

#define pr_fatal(fmt, ...)                                                     \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_FATAL)                         \
            dbg_log_debug("[FATAL] " fmt, ##__VA_ARGS__);                      \
    } while (0)

#define pr_warn(fmt, ...)                                                      \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_WARN)                          \
            dbg_log_debug("[WARN] " fmt, ##__VA_ARGS__);                       \
    } while (0)

#define pr_error(fmt, ...)                                                     \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_ERROR)                         \
            dbg_log_debug("[ERR] " fmt, ##__VA_ARGS__);                        \
    } while (0)

#define pr_info(fmt, ...)                                                      \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_INFO)                          \
            dbg_log_debug("[INFO] " fmt, ##__VA_ARGS__);                       \
    } while (0)

#define pr_debug(fmt, ...)                                                     \
    do {                                                                       \
        if ((get_debug_loglevel()) & DBG_LOGLEVEL_DEBUG)                       \
            dbg_log_debug("[DEBUG] " fmt, ##__VA_ARGS__);                      \
    } while (0)

#endif /*INCLUDE_DEBUG_H*/
// SPDX-License-Identifier: GPL-2.0
//
// vim: set ts=8 sw=8 noet tw=80 cc=80 fo+=t :

#ifndef INCLUDE_DEBUG_H
#define INCLUDE_DEBUG_H

#include <stdio.h>

/**
 * @brief Log levels used by the logging system.
 *
 * Each log level is represented by a bit flag and can be combined
 * using bitwise operations with dbg_loglevel define in include/debug.c.
 *
 * By default, all log levels are enabled in dbg_loglevel.
 *
 * @note The pr_foo debug log levels are enabled only when DEBUG is defined.
 */

#define DBG_LOGLEVEL_FATAL 0x01
#define DBG_LOGLEVEL_WARN 0x02
#define DBG_LOGLEVEL_ERROR 0x04
#define DBG_LOGLEVEL_INFO 0x08
#define DBG_LOGLEVEL_DEBUG 0x10

/**
 * @brief Functions for managing the logging level mask.
 *
 * These functions allow retrieving, setting, enabling, and disabling
 * individual or multiple log levels using bitwise operations.
 *
 * @note Each log level is represented by a bit flag.
 */
unsigned int get_debug_loglevel(void);
unsigned int set_debug_loglevel(const unsigned int);
unsigned int disable_debug_loglevel(const unsigned int);
unsigned int enable_debug_loglevel(const unsigned int);

#ifdef DEBUG
#define dbg_log_debug(fmt, ...)                                                    \
    do {                                                                       \
        fprintf(                                                               \
            stderr,                                                            \
            "\n%s:%s:%d--|" fmt "\n",                                          \
            __func__,                                                          \
            __FILE__,                                                          \
            __LINE__,                                                          \
            ##__VA_ARGS__);                                                    \
        fflush(stderr);                                                        \
    } while (0)
#else
#define dbg_log_debug(...) ((void)0)
#endif /*DEBUG*/

#define pr_fatal(fmt, ...)                                                     \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_FATAL)                                   \
            dbg_log_debug("[FATAL] " fmt, ##__VA_ARGS__);                          \
    } while (0)

#define pr_warn(fmt, ...)                                                      \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_WARN)                                    \
            dbg_log_debug("[WARN] " fmt, ##__VA_ARGS__);                           \
    } while (0)

#define pr_error(fmt, ...)                                                     \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_ERROR)                                   \
            dbg_log_debug("[ERR] " fmt, ##__VA_ARGS__);                            \
    } while (0)

#define pr_info(fmt, ...)                                                      \
    do {                                                                       \
        if (get_debug_loglevel() & DBG_LOGLEVEL_INFO)                                    \
            dbg_log_debug("[INFO] " fmt, ##__VA_ARGS__);                           \
    } while (0)

#define pr_debug(fmt, ...)                                                     \
    do {                                                                       \
        if ((get_debug_loglevel()) & DBG_LOGLEVEL_DEBUG)                                 \
            dbg_log_debug("[DEBUG] " fmt, ##__VA_ARGS__);                          \
    } while (0)

#endif /*INCLUDE_DEBUG_H*/