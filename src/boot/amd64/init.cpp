#include <io.hpp>
#include <fonts.hpp>
#include <int.hpp>
#include <fs/fat.hpp>

extern "C" void setup64(){
    x64IDT_INIT();
    
    heap_init(0x100000,0x100000);

    FDCInitialize();

    loadRoot();

    void (*start)() = (void(*)()) loadFile("KERNEL  BIN");

    start();
}                                                                                            
