#ifndef TFS_HPP
#define TFS_HPP

#include <io.hpp>
#include <gsl/string.hpp>

enum fileTypes_t{
    DEV_TYPE,
    PROC_TYPE,
    DATA_TYPE,
    CODE_TYPE,
    MOUNT_TYPE,
    LINK_TYPE,
    DIR_TYPE
};


struct FileObjectDescriptor{
    GSL::String             objectName;
    fileTypes_t             objectType;
    void*                   objectData;
    bool                    objectInUse;
    int                     objectSubCount;
    FileObjectDescriptor*   objectParent;
};

void initializeRoot(FileObjectDescriptor* OPENOS_ROOT);
GSL::String* returnFilePath(FileObjectDescriptor* file);

FileObjectDescriptor* makeNewDir(FileObjectDescriptor* directory, const char* name); 


byte* FDCInitialize();
#endif