#include <fonts.hpp>
#include <fs/fat.hpp>
#include <io.hpp>


FAT12_bootSectorData* g_BootSectorData;
FAT12_directory* g_rootDirectory;
byte* g_FAT;

void readBootSector(){
    FDCReadSector_LBA(0, 0);
    g_BootSectorData = NEW(FAT12_bootSectorData);
    memcpy(g_BootSectorData,(void*)0x1000,32);

    TTY1.printf("\nVENDORID: %s",g_BootSectorData->vendorID);
}

void readFAT(){
    //Copy FAT into Memory
    int fatSectorsEnd = g_BootSectorData->sectorsPerFat * g_BootSectorData->FATCount + g_BootSectorData->reservedSectors;
    g_FAT = (byte*)malloc((fatSectorsEnd - g_BootSectorData->reservedSectors) * g_BootSectorData->bytesPerSector);
    for(int currentSector = g_BootSectorData->reservedSectors; currentSector < fatSectorsEnd; currentSector++){
        FDCReadSector_LBA(0,currentSector);
        memcpy(g_FAT + (g_BootSectorData->bytesPerSector * (currentSector - g_BootSectorData->reservedSectors)), (void*)0x1000, g_BootSectorData->bytesPerSector);
    }
    TTY1.printf("\nFAT Table at:%x",g_FAT);
}

void readRootDirectory(){
    //Read the contents of the root directory into memory
    dword lba = g_BootSectorData->sectorsPerFat * g_BootSectorData->FATCount + g_BootSectorData->reservedSectors;
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