#pragma once

enum FDTypes
{
    noDrive = 0,
    _0360_525,
    _1200_525,
    _0720_350,
    _1440_350,
    _2880_350
};

enum FD_REGS
{
   STATUS_REGISTER_A                = 0x3F0,
   STATUS_REGISTER_B                = 0x3F1, 
   DIGITAL_OUTPUT_REGISTER          = 0x3F2,
   TAPE_DRIVE_REGISTER              = 0x3F3,
   MAIN_STATUS_REGISTER             = 0x3F4,
   DATARATE_SELECT_REGISTER         = 0x3F4, 
   DATA_FIFO                        = 0x3F5,
   DIGITAL_INPUT_REGISTER           = 0x3F7,
   CONFIGURATION_CONTROL_REGISTER   = 0x3F7
};

enum DOR_FLAGS
{
    DOR_DRIVE0      = 0,
    DOR_DRIVE1      = 1,
    DOR_DRIVE2      = 2,
    DOR_DRIVE3      = 3,
    DOR_RESET       = 4,
    DOR_DMA_ENABLE  = 8,
    DOR_MOT0        = 0x10,
    DOR_MOT1        = 0x20,
    DOR_MOT2        = 0x40,
    DOR_MOT3        = 0x80
};

enum MSR_FLAGS
{
    MSR_SEEK0       = 1,
    MSR_SEEK1       = 2,
    MSR_SEEK2       = 4,
    MSR_SEEK3       = 8,
    MSR_BUSY        = 16,
    MSR_PIO_ENABLE  = 32,
    MSR_DATA_OUT    = 64,
    MSR_FIFO_ENABLE = 128
};

enum CCR_FLAGS
{
    _1000K  = 3,
    _500K   = 0,
    _300K   = 2,
    _250K   = 1
};

enum FDC_COMMANDS
{
    CMD_SPECIFY         = 3,
    CMD_SENSE_STATUS    = 4,
    CMD_WRITE_DATA      = 5,
    CMD_READ_DATA       = 6,
    CMD_RECALIBRATE     = 7,
    CMD_SENSE_INTERRUPT = 8,
    CMD_WRITE_DELETED   = 9,
    CMD_READ_ID         = 10,
    CMD_READ_DELETED    = 12,
    CMD_FORMAT_TRACK    = 13,
    CMD_DUMPREG         = 14,
    CMD_SEEK            = 15,
    CMD_VERSION         = 16,
    CMD_SCAN_EQUAL      = 17,
    CMD_PERPENDICULAR   = 18,
    CMD_CONFIGURE       = 19,
    CMD_LOCK            = 20,
    CMD_VERIFY          = 22,
    CMD_SCAN_LOW        = 25,
    CMD_SCAN_HIGH       = 29
};

enum FDC_COMMANDS_EXT
{
    CMD_EXT_SKIP            = 0x20,
    CMD_EXT_DENSITY         = 0x40,
    CMD_EXT_MULTITRACK      = 0x80
};

enum FD_GAP3
{
    GAP3_STD    = 42,
    GAP3_5_25   = 32,
    GAP3_3_5    = 27
};

struct CHS{
    byte cyl;
    byte head;
    byte sector;
};

struct sense_interrupt_data{
    byte st0;
    byte cyl;
};

struct operation_data{
    byte st0;
    byte st1;
    byte st2;
    byte cyl;
    byte head;
    byte sector;
};

struct FAT12_bootSectorData{
    byte nop[3];
    byte vendorID[8];
    word bytesPerSector;
    byte clusterSize;
    word reservedSectors;
    byte FATCount;
    word directoryEntryCount;
    word sectorCount;
    byte mediaType;
    word sectorsPerFat;
    word sectorsPerTrack;
    word headCount;
}__attribute__((packed));


struct FAT12_directory{
    byte name[11];
    byte attributes;
    byte reserved;
    byte createdTimeTenths;
    word createdTime;       //Multiply Seconds by 2
    word createdDate;
    word lastAccessedDate;
    word firstClusterHigh;
    word lastModifiedTime;
    word lastModifiedDate;
    word firstClusterLow;
    dword size;
}__attribute__((packed));


extern FAT12_bootSectorData* g_BootSectorData;
extern FAT12_directory* g_rootDirectory;
extern byte* g_FAT;

byte* FDCInitialize();
void FDCReadSector_LBA(byte drive, word lba);
void readBootSector();
void readFAT();
void readRootDirectory();
void readFile(FAT12_directory* fileEntry, void* buffer);
