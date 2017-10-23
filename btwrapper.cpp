/*
 * =====================================================================================
 *
 *       Filename: btwrapper.cpp
 *        Created: 10/23/2017 00:56:20
 *  Last Modified: 10/23/2017 01:40:32
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

#include <mutex>
#include <ctime>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cxxabi.h>
#include <sys/time.h>

#include "btwrapper.h"
#include "backtrace.h"
#include "backtrace-supported.h"

// global backtrace functionality handler
// only one initialization allowed at the very beginning
struct backtrace_state *__global_state = nullptr;

static std::string __demangle_function_name(const char *szMangledName)
{
    if(szMangledName){
        int nStatus = 0;
        if(auto pBuf = abi::__cxa_demangle(szMangledName, nullptr, nullptr, &nStatus)){

            // may need to improve it
            // everytime when call this funciton it allocated memory..

            std::string szRet = pBuf;
            std::free(pBuf);
            return szRet;
        }

        // we return the input name
        // if the mangled name provided but demangle failed
        return std::string(szMangledName) + "()";
    }

    // return empty string
    return "";
}

static int __record_handler_default(const char *szLog)
{
    static const auto szFileName = []() -> std::string
    {
        std::string szFullFileName;

        szFullFileName += std::getenv("HOME");
        szFullFileName += "/.backtrace_record.";

        auto stTime      =  std::time(nullptr);
        auto stLocalTime = *std::localtime(&stTime);

        char szTimeBuf[256];
        std::strftime(szTimeBuf, sizeof(szTimeBuf), "%Y.%m.%d.%H.%M.%S", &stLocalTime);
        szFullFileName += szTimeBuf;

        struct timeval stTimeVal;
        gettimeofday(&stTimeVal, nullptr);

        char szUSecBuf[128];
        std::sprintf(szUSecBuf, ".%07ld.", (long)(stTimeVal.tv_usec));
        szFullFileName += szUSecBuf;

        std::srand(std::time(0));
        for(int nIndex = 0; nIndex < 5; ++nIndex){
            szFullFileName += std::to_string(std::rand() % 10);
        }

        if(auto pAddName = std::getenv("BACKTRACE_RECORD_SUFFIX")){
            szFullFileName += ".";
            szFullFileName += pAddName;
        }

        szFullFileName += ".";
        szFullFileName += std::to_string((unsigned long)(getpid()));

        szFullFileName += ".txt";
        return szFullFileName;
    }();

    static std::mutex s_LogLock;
    {
        std::lock_guard<std::mutex> stLockGuard(s_LogLock);
        if(auto fp = std::fopen(szFileName.c_str(), "a+")){
            std::fprintf(fp, "%s\n", szLog);
            std::fclose(fp);

            return 0;
        }
    }
    return -1;
}

static void __backtrace_error_handler(void *pData, const char *szMsg, int nError)
{
    if(pData == nullptr){
        pData = (void *)(__record_handler_default);
    }

    std::string szLogLine;

    szLogLine += " **** ERROR IN BACKTRACE: ";
    szLogLine += szMsg ? szMsg : "??";
    szLogLine += " ";
    szLogLine += std::to_string(nError);

    // if error happens
    // we try to redirect error message to the default log file
    if(((__backtrace_record_handler)(pData))(szLogLine.c_str())){
        std::fprintf(stderr, "Default backtrace log functionality broken: %s\n", szLogLine.c_str());
        std::exit(0);
    }
}

static int __backtrace_full_callback(void *pData, uintptr_t nPC, const char *szFileName, int nLine, const char *szFuncName)
{
    auto szDemangledName = __demangle_function_name(szFuncName);

    char szPC[128];
    std::sprintf(szPC, "%#lX", (unsigned long)(nPC));

    std::string szLogLine;

    szLogLine += szPC;
    szLogLine += " ";

    szLogLine += szDemangledName.empty() ? "??" : szDemangledName;
    szLogLine += " ";

    szLogLine += szFileName ? szFileName : "??";
    szLogLine += " ";

    szLogLine += std::to_string(nLine);

    return ((__backtrace_record_handler)(pData))(szLogLine.c_str());
}

// export to libbtwrapper.so
// record backtrace if callback provided or use default record functionality
int __backtrace_record(__backtrace_record_handler fnCB)
{
    if(fnCB == nullptr){
        fnCB = __record_handler_default;
    }

    // make the log line
    // then feed the record handler

    extern struct backtrace_state *__global_state;
    if(__global_state){
        return backtrace_full(__global_state, 0, __backtrace_full_callback, __backtrace_error_handler, (void *)(fnCB));
    }
    return -1;
}

// export to libbtwrapper.so
// establish the backtrace functionality
int __backtrace_enable(const char *szBinFileName)
{
    if(__global_state){
        auto pErrorLog = "Backtrace functionality is created more than once";
        if(__record_handler_default(pErrorLog)){
            std::fprintf(stderr, "%s\n", pErrorLog);
            std::exit(0);
        }
        return -1;
    }

    // if any error happens
    // call __backtrace_error_handler(nullptr, error_message, errno)

    __global_state = backtrace_create_state(szBinFileName, BACKTRACE_SUPPORTS_THREADS, __backtrace_error_handler, nullptr);
    return __global_state ? 0 : -1;
}
