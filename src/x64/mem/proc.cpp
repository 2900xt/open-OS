#include <proc.hpp>

PROCESS_T* createProc(PROCESS_T* parent, const char* name , PROCESS_PERMISSIONS permissions, int (code)(PROCESS_T*)){
    if(!((int)parent->permissions & (int)PROCESS_PERMISSIONS::CHILD))
        return nullptr;

    parent->child = NEW(PROCESS_T);
    if(parent->permissions < permissions)
        return nullptr;
    
    parent->child->file = nullptr;
    parent->child->permissions = (PROCESS_PERMISSIONS)((int)permissions & (int)parent->permissions);
    parent->child->code = (qword)code;
    parent->child->name = name;
    return parent->child;
}

int runProc(PROCESS_T* proc)
{
    proc->state = PROCESS_STATES::FOREGROUND;
    int (*run)(PROCESS_T*)  = (int (*)(PROCESS_T*))proc->code;
    int code = run(proc);
    proc->state = PROCESS_STATES::STOPPED;
    return code;
}

bool killProc(PROCESS_T* proc){
    if(proc->state != PROCESS_STATES::STOPPED)
        return false;
    
    free(proc);
    return true;
}

void* openFile(PROCESS_T* proc,FileObjectDescriptor* file){
    if(proc->permissions <= PROCESS_PERMISSIONS::FS && proc->state == PROCESS_STATES::FOREGROUND)
        return nullptr;
    
    if(!proc->file)
        return nullptr;
    
    proc->file = file;

    return proc->file->objectData;
}