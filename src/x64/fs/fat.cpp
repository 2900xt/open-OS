#include <fonts.hpp>
#include <fs/fat.hpp>
#include <io.hpp>


FAT12_bootSectorData* g_BootSectorData;
FAT12_directory* g_rootDirectory;
byte* g_FAT;
byte rootDirFileCount = 0;

void readBootSector(){
    FDCReadSector_LBA(0, 0);
    g_BootSectorData = NEW(FAT12_bootSectorData);
    memcpy(g_BootSectorData,(void*)0x1000,32);
}

void readFAT(){
    //Copy FAT into Memory
    g_FAT = (byte*)malloc(g_BootSectorData->sectorsPerFat * g_BootSectorData->bytesPerSector);
    for(int currentSector = g_BootSectorData->reservedSectors; currentSector < g_BootSectorData->sectorsPerFat; currentSector++){
        FDCReadSector_LBA(0,currentSector);
        memcpy(g_FAT + (g_BootSectorData->bytesPerSector * (currentSector - g_BootSectorData->reservedSectors)), (void*)0x1000, g_BootSectorData->bytesPerSector);
    }
}

void readRootDirectory(){
    //Read the contents of the root directory into memory
    dword lba =  g_BootSectorData->reservedSectors + g_BootSectorData->FATCount * g_BootSectorData->sectorsPerFat;
    dword size = sizeof(FAT12_directory) * g_BootSectorData->directoryEntryCount;
    dword sectors = (size/g_BootSectorData->bytesPerSector);
    
    if(size % g_BootSectorData->bytesPerSector > 0)
        sectors++;

    g_rootDirectory = (FAT12_directory*)malloc(sectors * g_BootSectorData->bytesPerSector);
    for(int currentSector = lba ; currentSector < (10+lba) ; currentSector++){
        FDCReadSector_LBA(0,currentSector);
        memcpy(g_rootDirectory + (g_BootSectorData->bytesPerSector * (currentSector - lba)), (void*)0x1000, g_BootSectorData->bytesPerSector);
    }
}
