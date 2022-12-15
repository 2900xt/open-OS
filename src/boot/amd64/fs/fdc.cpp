#include <io.hpp>
#include <int.hpp>

#include <fs/fat.hpp>

#define FLOPPY_144_SECTORS_PER_TRACK    18
#define FLOPPY_144_BYTES_PER_SECTOR     2

void DMAInitialize(){
    outb(0x0a, 0x06);
    outb(0xd8, 0xff);

    outb(0x04, 0x00);
    outb(0x04, 0x10);
    outb(0xd8, 0xff);

    outb(0x05, 0xff);
    outb(0x05, 0x23);
    outb(0x80, 0x00);
    outb(0x0a, 0x02);

}

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

void lba_2_chs(word lba, CHS* sect)
{
    sect->cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
    sect->head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
    sect->sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
}

FDTypes detectMasterFD(){
    outb(0x70, (0 << 7) | (0x10));
    return (FDTypes)(inb(0x71)>>4);
}

FDTypes detectSlaveFD(){
    outb(0x70, (0 << 7) | (0x10));
    return (FDTypes)(inb(0x71) & 0x0F);
}

void waitForIRQ(){
    while(!irq6);
    irq6 = false;
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

void writeCCR(CCR_FLAGS data){
    outb(CONFIGURATION_CONTROL_REGISTER, data);
}

void resetFDC(){
    outb(DIGITAL_OUTPUT_REGISTER, 0);
    outb(DIGITAL_OUTPUT_REGISTER, 0b00011100);
    waitForIRQ();
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

void FDCReadSector_LBA(byte drive, word lba){
    operation_data out;
    CHS location;
    lba_2_chs(lba, &location);
    FDCReadSector_CHS(drive, location.cyl, location.head, location.sector,&out);
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


byte* FDCInitialize(){

    FDTypes masterFDType = detectMasterFD();
    FDTypes slaveFDType = detectSlaveFD();

    if(!masterFDType)
        return nullptr;

    DMAInitialize();

    FDCReset(0);
}

