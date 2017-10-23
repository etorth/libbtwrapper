/*
 * =====================================================================================
 *
 *       Filename: btwrapper.h
 *        Created: 10/23/2017 00:29:08
 *  Last Modified: 10/23/2017 00:39:16
 *
 *    Description:
 *
 *        Version: 1.0
 *       Revision: none
 *       Compiler: gcc
 *
 *         Author: ANHONG
 *          Email: anhonghe@gmail.com
 *   Organization: USTC
 *
 * =====================================================================================
 */

#ifndef _BTWRAPPER_09652755034365_H_
#define _BTWRAPPER_09652755034365_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

typedef int (*__backtrace_record_handler)(const char *);

EXTERNC int __backtrace_enable(const char *);
EXTERNC int __backtrace_record(__backtrace_record_handler);

#endif
