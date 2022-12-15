#include <io.hpp>
#include <int.hpp>
#include <fs/fat.hpp>

extern "C" void setup64(){
    x64IDT_INIT();

    FDCInitialize();

    loadRoot();


    void (*start)() = (void(*)()) loadFile("KERNEL  BIN");

    start();
}                                                                                            