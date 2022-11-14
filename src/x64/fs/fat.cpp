#include <fonts.hpp>
#include <fs/fat.hpp>
#include <io.hpp>


FAT12_bootSectorData* g_BootSectorData;
FAT12_directory* g_rootDirectory;

void readBootSector()
{
    FDCReadSector_LBA(0, 0);                            //Read the Bootsector
    g_BootSectorData = NEW(FAT12_bootSectorData);       //Copy the BPB into memory
    memcpy(g_BootSectorData,(void*)0x1000,32);
}

void loadRoot()
{
    readBootSector();
    int rootSectors = (sizeof(FAT12_directory) * g_BootSectorData->directoryEntryCount) / g_BootSectorData->bytesPerSector;     
    g_rootDirectory = (FAT12_directory*)calloc(rootSectors * g_BootSectorData->bytesPerSector);

    int StartingLBA = g_BootSectorData->FATCount * g_BootSectorData->sectorsPerFat + g_BootSectorData->reservedSectors;
    int FinishingLBA = StartingLBA + rootSectors - 2;

    for(int sector = StartingLBA; sector < FinishingLBA; sector++){
        FDCReadSector_LBA(0, sector);
        memcpy((void*)(g_rootDirectory + ((sector - StartingLBA)* g_BootSectorData->bytesPerSector)), (void*)0x1000, g_BootSectorData->bytesPerSector);
    }
}

bool cmpFile(char* s1, char* s2){
    for(int i = 0; i < 11; i++){
        if(s1[i] != s2[i])
            return false;
    }
    return true;
}

void* loadFile(char* filename){

    int i;
    for(i = 0; i < g_BootSectorData->directoryEntryCount; i++){
        if(cmpFile(g_rootDirectory[i].name, filename)){
            goto fileFound;
        }
    }

    return nullptr;

    fileFound:
    
    int fileSize = g_rootDirectory[i].size;
    int cluster = g_rootDirectory[i].firstClusterLow;
    int fatSize = g_BootSectorData->FATCount * g_BootSectorData->sectorsPerFat;

    void* FAT = malloc(fatSize * g_BootSectorData->bytesPerSector);

    for(int sector = 0; sector < fatSize; sector++){
        FDCReadSector_LBA(0, sector + 1);
        memcpy(FAT + (sector * g_BootSectorData->bytesPerSector), (void*)0x1000, g_BootSectorData->bytesPerSector);
    }

    void* buffer = malloc(fileSize + 512);
    int bufferPtr = 0;

    do {
        uint32_t lba = 33 + (cluster - 2) * g_BootSectorData->clusterSize;
        FDCReadSector_LBA(0,lba);
        memcpy(buffer + bufferPtr, (void*)0x1000, 512);
        bufferPtr += g_BootSectorData->clusterSize * g_BootSectorData->bytesPerSector;

        uint32_t fatIndex = cluster * 3 / 2;
        if (cluster % 2 == 0)
            cluster = (*(uint16_t*)(FAT + fatIndex)) & 0x0FFF;
        else
            cluster = (*(uint16_t*)(FAT + fatIndex)) >> 4;

    } while (cluster < 0x0FF8);

    return buffer;



}
