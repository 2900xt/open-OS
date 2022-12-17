#include <types.h>
#include <stdlib.h>
#include <stdout.h>

#define FLOPPY_144_SECTORS_PER_TRACK    18
#define FLOPPY_144_BYTES_PER_SECTOR     2

struct CHS
{
    byte cyl, head, sector;
};

struct sense_interrupt_data
{
    byte st0;
    byte cyl;
};

struct operation_data
{
    byte st0;
    byte st1;
    byte st2;
    byte cyl;
    byte head;
    byte sector;
};

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


void setWriteDMA(){
    outb(0x0a, 0x06);
    outb(0x0b, 0x5a);
    outb(0x0a, 0x02);
}

void setReadDMA(){
    outb(0x0a, 0x06);
    outb(0x0b, 0x56);
    outb(0x0a, 0x02);
}

void convertToCHS(word lba, CHS* out){
    out->cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    out->head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    out->sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

bool isFIFOAvalible(){
    return (inb(MAIN_STATUS_REGISTER) & MSR_FIFO_ENABLE);
}

bool isDataAvalible(){
    return (inb(MAIN_STATUS_REGISTER) & (MSR_FIFO_ENABLE | MSR_DATA_OUT));
}

void FDWriteDOR(byte data){
    outb(DIGITAL_OUTPUT_REGISTER, data);
}

void FDSendData(byte val){
    while(!isFIFOAvalible());
    outb(DATA_FIFO,val);
}

byte FDGetData(){
    while(!isDataAvalible());
    return inb(DATA_FIFO);
}

void waitForIRQ(){
    while(!isrs[6]);
    isrs[6] = false;
}

void writeCCR(CCR_FLAGS data){
    outb(CONFIGURATION_CONTROL_REGISTER, data);
}


void FDCSpecifyDrive(dword stepr, dword loadt, dword unloadt, bool dmaEnable){
    FDSendData(CMD_SPECIFY);
    FDSendData(((stepr & 0xf) << 4 ) | (unloadt & 0xf));
    FDSendData((loadt << 1) | (dmaEnable ? 1 : 0));
}

void FDSenseInterrupt(sense_interrupt_data* data){
    FDSendData(CMD_SENSE_INTERRUPT);
    
    data->st0 = FDGetData();
    data->cyl = FDGetData();
}


void FDSelectDrive(byte drive){
    drive = drive & 0b00000011;
    byte dor = inb(DIGITAL_OUTPUT_REGISTER) & 0b11111100;
    FDWriteDOR(dor | drive);
}

void FDEnableMotor(byte drive){
    drive = drive & 0b00000011;
    byte dor = inb(DIGITAL_OUTPUT_REGISTER) & 0b00001111;

    switch (drive){
        case 0:
            FDWriteDOR( dor | DOR_MOT0);
            break;
        case 1:
            FDWriteDOR( dor | DOR_MOT1);
            break;
        case 2:
            FDWriteDOR( dor | DOR_MOT2);
            break;
        case 3:
            FDWriteDOR( dor | DOR_MOT3);
            break;
    }
}

void FDDisableMotor(byte drive){
    drive = drive & 0b00000011;
    byte dor = inb(DIGITAL_OUTPUT_REGISTER) & 0b00001111;

    switch (drive){
        case 0:
            FDWriteDOR( dor & ~DOR_MOT0);
            break;
        case 1:
            FDWriteDOR( dor & ~DOR_MOT1);
            break;
        case 2:
            FDWriteDOR( dor & ~DOR_MOT2);
            break;
        case 3:
            FDWriteDOR( dor & ~DOR_MOT3);
            break;
    }
}


bool FDCalibrate(byte drive){
    sense_interrupt_data data;
    drive = drive & 0b00000011;
    FDEnableMotor(drive);
    

    for(int i = 0; i < 10; i++){
        FDSendData(CMD_RECALIBRATE);
        FDSendData(drive);

        waitForIRQ();

        FDSenseInterrupt(&data);

        if(!data.cyl){
            FDDisableMotor(drive);
            return true;
        }

    }

    FDDisableMotor(drive);
    return false;

}

bool FDSeek(byte cyl, byte head, byte drive){
    sense_interrupt_data data;
    drive = drive & 0b00000011;

    FDEnableMotor(drive);

    for(int i = 0; i < 10 ; i++){
        FDSendData(CMD_SEEK);
        FDSendData((head << 2) | drive);
        FDSendData(cyl);

        waitForIRQ();
        FDSenseInterrupt(&data);

        if(!data.cyl){
            FDDisableMotor(drive);
            return true;
        }
    }

    FDDisableMotor(drive);
    return false;

}


void FDCReset(byte drive){
    FDWriteDOR(0);
    FDWriteDOR(DOR_DMA_ENABLE | DOR_RESET);

    waitForIRQ();
    sense_interrupt_data data;
    for(int i = 0; i < 4 ; i++)
        FDSenseInterrupt(&data);
    
    writeCCR(_500K);

    FDSelectDrive(drive);
    FDCSpecifyDrive(3,16,240,false);
    FDCalibrate(drive);
}

void FDCReadSector_CHS(byte drive ,byte cyl, byte head, byte sector, operation_data* out = new operation_data){
    sense_interrupt_data data;

    setReadDMA();

    FDSeek(cyl,head,drive);

    FDEnableMotor(drive);

    FDSendData(CMD_READ_DATA | CMD_EXT_MULTITRACK | CMD_EXT_DENSITY | CMD_EXT_SKIP);
    FDSendData(head << 2 | (drive & 0b00000011));
    FDSendData(cyl);
    FDSendData(head);
    FDSendData(sector);
    FDSendData(FLOPPY_144_BYTES_PER_SECTOR);
    FDSendData(( ( sector + 1 ) >= FLOPPY_144_SECTORS_PER_TRACK ) ? FLOPPY_144_SECTORS_PER_TRACK : sector + 1 );
    FDSendData(GAP3_3_5);
    FDSendData(0xFF);

    waitForIRQ();

    out->st0 = FDGetData();
    out->st1 = FDGetData();
    out->st2 = FDGetData();
    out->cyl = FDGetData();
    out->head = FDGetData();
    out->sector = FDGetData();
    FDGetData();

    FDSenseInterrupt(&data);

    FDDisableMotor(drive);
    return;
}