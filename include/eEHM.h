// Copyright [2014] <cuizhaohui>
/**
 * \file eEHM.h
 * \author CUI Zhao-hui, zhh.cui@gmail.com
 * \date 2014-07-07
 * \brief Enhanced exception handling method (eEHM).
 */

#include <csignal>
#include <csetjmp>
#include <exception>
#include <string>
#include <execinfo.h>
#include <cstdlib>
#include <cstdio>

void user_fpe(int signalId);
void user_segv(int signalId);
void user_int(int signalId);
void user_term(int signalId);
void user_abrt(int signalId);
void user_ill(int signalId);

class eEHM {
 public:
    eEHM();
    ~eEHM();
    void SetUserHandler(int signalID);
    bool isValid(void);

 private:
    std::string GetTrace(void);
    __sighandler_t sigfpe_handler;
    __sighandler_t sigsegv_handler;
    __sighandler_t sigint_handler;
    __sighandler_t sigterm_handler;
    __sighandler_t sigabrt_handler;
    __sighandler_t sigill_handler;
    static int counter;
};

class signal_error : public std::exception {
 public:
    signal_error(int code = 0, std::string desc = "It seems ok.");
    ~signal_error() throw() {};
    const char * what() const throw();
 private:
    int code;
    std::string desc;
};



