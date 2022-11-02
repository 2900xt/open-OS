#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>
#include <gsl/string.hpp>
#include <fs/tfs.hpp>
#include <proc.hpp>
#include <time.hpp>

void * __gxx_personality_v0=0;
void * _Unwind_Resume =0;

int init(PROCESS_T* proc){
    x64IDT_INIT();
    heap_init(0x100000,0x100000);       //Sets up heap from 0x100000 - 0x1F0000
    initTTY(&TTY1);
    
    FileObjectDescriptor* OPENOS_ROOT = NEW(FileObjectDescriptor);

    initializeRoot(OPENOS_ROOT);

    TTY1.printf("%c%s%c Booting from%c %x...%c\n",RED ,proc->name , WHITE, GREEN, 0x7c00, WHITE);
    printMemoryInformation(proc->name);
    FDCInitialize();

    TIME_T currentTime;
    readRTC(&currentTime);

    TTY1.printf("\nCurrent Time: \n%d:%d:%d\n\nDate:\n%d/%d/%d\n->",currentTime.hour,currentTime.minute,currentTime.second,currentTime.day,currentTime.month,currentTime.year);


    for(;;){
        TTY1.putChar(getChar());
    }
}

extern "C" void setup64(){
    PROCESS_T INIT = { "[INIT]" , (qword)init ,nullptr , nullptr , PROCESS_PERMISSIONS::ROOT};
    runProc(&INIT);
}