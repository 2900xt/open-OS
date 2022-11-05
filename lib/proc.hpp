#ifndef PROC_HPP
#define PROC_HPP

#include <fonts.hpp>
#include <io.hpp>
#include <fs/tfs.hpp>

#define OPENOS_VERSION "0.001 INDEV"

enum class PROCESS_PERMISSIONS
{
    NONE         = 0,
    CHILD        = 1,
    TTY          = 2,
    FS           = 4,
    MEM          = 8,
    DISK         = 16,
    IO           = 32,
    ROOT         = 0xFF
};  

enum class PROCESS_STATES
{
    STOPPED     = 0,
    SUSPENDED   = 1,
    BACKGROUND  = 2,
    FOREGROUND  = 4
};

struct PROCESS_T
{   
    const char* name;
    qword code;
    PROCESS_T* parent;
    PROCESS_T* child;
    FileObjectDescriptor* file;
    PROCESS_PERMISSIONS permissions;
    PROCESS_STATES state;
};


PROCESS_T* createProc(PROCESS_T* parent, const char* name , PROCESS_PERMISSIONS permissions, int (code)(PROCESS_T*));
int runProc(PROCESS_T* proc);
bool killProc(PROCESS_T* proc);
#endif