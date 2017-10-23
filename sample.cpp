/*
 * =====================================================================================
 *
 *       Filename: sample.cpp
 *        Created: 10/23/2017 01:13:20
 *  Last Modified: 10/23/2017 01:14:33
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

#include "btwrapper.h"
void f()
{
    __backtrace_record(nullptr);
}

int main(int argc, char *argv[])
{
    __backtrace_enable(argv[0]);

    f();
    return 0;
}
