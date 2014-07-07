// Copyright [2014] <cuizhaohui>
/**
 * \file eEHM.cpp
 * \author CUI Zhao-hui, zhh.cui@gmail.com
 * \date 2014-07-07
 * \brief Enhanced exception handling method (eEHM).
 */

#include "../include/eEHM.h"

jmp_buf fpe_buf;
jmp_buf segv_buf;
jmp_buf int_buf;
jmp_buf term_buf;
jmp_buf abrt_buf;
jmp_buf ill_buf;

int eEHM::counter = 0;

void user_fpe(int signalId) {
    longjmp(fpe_buf, SIGFPE);
}

void user_segv(int signalId) {
    longjmp(segv_buf, SIGSEGV);
}

void user_int(int signalId) {
    longjmp(int_buf, SIGINT);
}

void user_term(int signalId) {
    longjmp(term_buf, SIGTERM);
}

void user_abrt(int signalId) {
    longjmp(abrt_buf, SIGABRT);
}

void user_ill(int signalId) {
    longjmp(ill_buf, SIGILL);
}

eEHM::eEHM() {
    if (counter) {
        throw signal_error(0, "You already have this.");
    }
    sigfpe_handler = NULL;
    sigsegv_handler = NULL;
    sigint_handler = NULL;
    sigterm_handler = NULL;
    sigabrt_handler = NULL;
    sigill_handler = NULL;
    ++counter;
}

eEHM::~eEHM() {
    --counter;
    __sighandler_t reset_handler;
    if (sigfpe_handler) {
        reset_handler = signal(SIGFPE, sigfpe_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGFPE failed.");
        }
    }
    if (sigsegv_handler) {
        reset_handler = signal(SIGSEGV, sigsegv_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGSEGV failed.");
        }
    }
    if (sigint_handler) {
        reset_handler = signal(SIGINT, sigint_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGINT failed.");
        }
    }
    if (sigterm_handler) {
        reset_handler = signal(SIGTERM, sigterm_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGTERM failed.");
        }
    }
    if (sigabrt_handler) {
        reset_handler = signal(SIGABRT, sigabrt_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGABRT failed.");
        }
    }
    if (sigill_handler) {
        reset_handler = signal(SIGILL, sigill_handler);
        if (SIG_ERR == reset_handler) {
            throw signal_error(0, "Reset handler for SIGILL failed.");
        }
    }
}

void eEHM::SetUserHandler(int signalID) {
    switch (signalID) {
    case SIGFPE:
        sigfpe_handler = signal(SIGFPE, user_fpe);
        if (SIG_ERR == sigfpe_handler) {
            throw signal_error(0, "Set user handler for SIGFPE failed.");
        }
        if (setjmp(fpe_buf)) {
            throw signal_error(SIGFPE, std::string("SIGFPE: ") + GetTrace());
        }
        break;
    case SIGSEGV:
        sigsegv_handler = signal(SIGSEGV, user_segv);
        if (SIG_ERR == sigsegv_handler) {
            throw signal_error(0, "Set user handler for SIGSEGV failed.");
        }
        if (setjmp(segv_buf)) {
            throw signal_error(SIGSEGV, std::string("SIGSEGV: ") + GetTrace());
        }
        break;
    case SIGINT:
        sigint_handler = signal(SIGINT, user_int);
        if (SIG_ERR == sigint_handler) {
            throw signal_error(0, "Set user handler for SIGINT failed.");
        }
        if (setjmp(int_buf)) {
            throw signal_error(SIGINT, std::string("SIGINT: ") + GetTrace());
        }
        break;
    case SIGTERM:
        sigterm_handler = signal(SIGTERM, user_term);
        if (SIG_ERR == sigterm_handler) {
            throw signal_error(0, "Set user handler for SIGTERM failed.");
        }
        if (setjmp(term_buf)) {
            throw signal_error(SIGTERM, std::string("SIGTERM: ") + GetTrace());
        }
        break;
    case SIGABRT:
        sigabrt_handler = signal(SIGABRT, user_abrt);
        if (SIG_ERR == sigabrt_handler) {
            throw signal_error(0, "Set user handler for SIGABRT failed.");
        }
        if (setjmp(abrt_buf)) {
            throw signal_error(SIGABRT, std::string("SIGABRT: ") + GetTrace());
        }
        break;
    case SIGILL:
        sigill_handler = signal(SIGILL, user_ill);
        if (SIG_ERR == sigill_handler) {
            throw signal_error(0, "Set user handler for SIGILL failed.");
        }
        if (setjmp(ill_buf)) {
            throw signal_error(SIGILL, std::string("SIGILL: ") + GetTrace());
        }
        break;
    default:
        throw signal_error(0, "Unknown signalID.");
        break;
    }
}

bool eEHM::isValid(void) {
    return counter ? true : false;
}

std::string eEHM::GetTrace(void) {
    const size_t TRACE_DEPTH = 15;
    void *records[TRACE_DEPTH];
    size_t valid_depth;
    char **contents;
    char msg[255];
    std::string result;

    valid_depth = backtrace(records, TRACE_DEPTH);
    snprintf(msg, 255, "backtrace %ld records:\n", valid_depth);
    result.assign(msg);
    if (!valid_depth) {
        return result;
    }

    contents = backtrace_symbols(records, valid_depth);
    std::string record;
    for (size_t i = 2; i < valid_depth; ++i) {
        snprintf(msg, 255, "%3ld:", i);

        record.assign(contents[i]);

        std::string::size_type begin = record.find('(');
        if (std::string::npos == begin) {
            result += msg + record + "\n";
            continue;
        }
        std::string exec_file = record.substr(0, begin);

        begin = record.find("libc.so");
        if (std::string::npos != begin) {
            break;
        }

        begin = record.find("[0x");
        std::string::size_type end = record.find(']', begin);
        if (std::string::npos == begin || std::string::npos == end) {
            continue;
        }
        std::string address = record.substr(begin + 1, end - begin - 1);

        result += msg + record + "\n";

        record.assign("addr2line -f -s -i -C -p -e ");
        record += exec_file;
        record.append(" ");
        record += address;

        FILE *tmp = popen(record.c_str(), "r");
        if (NULL == tmp) {
            continue;
        }
        fgets(msg, 255, tmp);
        pclose(tmp);

        result += std::string("    ") + msg;
    }

    free(contents);
    return result;
}

signal_error::signal_error(int code, std::string desc) {
    this->code = code;
    this->desc = desc;
}

const char * signal_error::what() const throw() {
    return desc.c_str();
}

