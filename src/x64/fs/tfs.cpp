#include <fs/tfs.hpp>

FileObjectDescriptor* currentDirectory;

void initializeRoot(FileObjectDescriptor* OPENOS_ROOT){

    OPENOS_ROOT->objectPath = "/";
    OPENOS_ROOT->objectType = DIR_TYPE;
    OPENOS_ROOT->objectData = NULL;
    OPENOS_ROOT->objectInUse = true;
    OPENOS_ROOT->objectParent = NULL;

    currentDirectory = OPENOS_ROOT;

    debugPrint("Initialized TFS\n\r");

}

FileObjectDescriptor* makeNewDir(FileObjectDescriptor* parent, const char* name){

    int FSlevel = 0;
    while(parent->objectParent){
        parent = parent->objectParent;
        FSlevel++;
    }

    FileObjectDescriptor* newFile = NEW(FileObjectDescriptor);
    newFile->objectPath = name;
    newFile->objectType = DIR_TYPE;
    newFile->objectData = (void*)FSlevel;
    newFile->objectParent = parent;
    newFile->objectInUse = false;
    newFile->objectSubCount = 0;
    newFile->objectSub = nullptr;
    parent->objectSub[parent->objectSubCount++] = newFile;

    return newFile;
}

FileObjectDescriptor* makeNewFile(FileObjectDescriptor* parent, const char* name, fileTypes_t type){
    FileObjectDescriptor* newFile = NEW(FileObjectDescriptor);
    newFile->objectParent = parent;
    newFile->objectInUse = false;
    newFile->objectType = type;
    newFile->objectPath = name;
}
