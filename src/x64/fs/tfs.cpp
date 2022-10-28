#include <fs/tfs.hpp>

FileObjectDescriptor* currentDirectory;

void initializeRoot(FileObjectDescriptor* OPENOS_ROOT){

    OPENOS_ROOT->objectName = "/";
    OPENOS_ROOT->objectType = DIR_TYPE;
    OPENOS_ROOT->objectData = NULL;
    OPENOS_ROOT->objectInUse = true;
    OPENOS_ROOT->objectSub = NULL;
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
    newFile->objectName = name;
    newFile->objectType = DIR_TYPE;
    newFile->objectSub = NULL;
    newFile->objectData = (void*)FSlevel;
    newFile->objectParent = parent;
    newFile->objectInUse = false;
    
    parent->objectSub = newFile;

    return newFile;
}

void changeCurrentDirectory(FileObjectDescriptor* newDirectory){

}