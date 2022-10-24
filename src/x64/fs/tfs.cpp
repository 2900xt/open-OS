#include <fs/tfs.hpp>


void ROOT_INIT(FileObjectDescriptor* OPENOS_ROOT){

    OPENOS_ROOT->objectName = "/";
    OPENOS_ROOT->objectType = DIR_TYPE;
    OPENOS_ROOT->objectData = NULL;
    OPENOS_ROOT->objectInUse = true;
    OPENOS_ROOT->objectSub = NULL;
    OPENOS_ROOT->objectParent = NULL;

    debugPrint("Initialized TFS\n\r");

}

FileObjectDescriptor* makeNewFile(FileObjectDescriptor* directory, const char* name, fileTypes_t type, void* data){
    if(directory->objectType != DIR_TYPE)
        return NULL;
    FileObjectDescriptor* newFile = NEW(FileObjectDescriptor);
    newFile->objectName = name;
    newFile->objectType = type;
    newFile->objectSub = NULL;
    newFile->objectData = data;
    newFile->objectParent = directory;
    newFile->objectInUse = false;
    
    directory->objectSub = newFile;

    return newFile;
}
