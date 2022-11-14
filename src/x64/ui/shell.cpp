#include <ui/shell.hpp>
#include <fs/fat.hpp>

char inputBuffer[80];
char commandBuffer[80];

char* getLine(){
    for(int i = 0; i < 80; i++)
        inputBuffer[i] = 0;
    
    for(int i = 0; i < 80; i++){
        inputBuffer[i] = getChar();
        if(inputBuffer[i] == '\n'){
            inputBuffer[i] = '\0';
            TTY1.printf("\n");
            return inputBuffer;
        }
        TTY1.putChar(inputBuffer[i]);
    }
}

int debug_CMD(PROCESS_T* proc, int argc, char** argv){

    char* out;

    if(*(commandBuffer + 6))
        out = commandBuffer+6;
    else{
        TTY1.printf("%c%s%c Enter Message: ",LRED, proc->name, WHITE);
        out = getLine();
    }
    debugPrint(out);
    debugPrint("\n\r");
    TTY1.printf("%c%s%c Wrote %c%s%c to port %c0xE9%c\n",LRED, proc->name, WHITE, LGREEN, out, WHITE, CYAN, WHITE);
    return 0;
}

int time_CMD(PROCESS_T* proc, int argc, char** argv){
    TIME_T currentTime;
    readRTC(&currentTime);
    TTY1.printf("\n%c%s%c Current Time: \n%d:%d:%d\n\n%c%s%c Date:\n%d/%d/%d\n",LRED,proc->name,WHITE,currentTime.hour,currentTime.minute,currentTime.second,LRED,proc->name,WHITE,currentTime.day,currentTime.month,currentTime.year);
    return 0;
}

int version_CMD(PROCESS_T* proc, int argc, char** argv){
    TTY1.printf("\n%c%s%c OPEN-OS %s%c",LRED, proc->name, PURPLE, OPENOS_VERSION, WHITE);
}

int ls_CMD(PROCESS_T* proc, int argc, char** argv){
    //Display files in current DIR to TTY1
    int i = 0;
    int size = 0;
    TTY1.printf("%x\n",g_rootDirectory);
    TTY1.printf("Volume in drive : %c%s%c\n",LBLUE,g_rootDirectory[i++].name,WHITE);
    while(i < sizeof(FAT12_directory) * g_BootSectorData->directoryEntryCount){
        if(g_rootDirectory[i].size == 0){
            break;
        }
        TTY1.printf("%s\n",g_rootDirectory[i].name);
        size += g_rootDirectory[i++].size;
    }
    TTY1.printf("%d Files Total , %d bytes",i-1,size);
    return 0;
}

int uptime_CMD(PROCESS_T* proc, int argc, char** argv){
    TTY1.printf("%c%s%c %d%c seconds of uptime\n",LRED,proc->name,GREEN,seconds,WHITE);
}

int break_CMD(PROCESS_T* proc, int argc, char** argv){
    TTY1.printf("%c%s%c Break!\n",LRED, proc->name, WHITE);
    MagicBreak();
}

bool cmdCheck(const char* a, const char* b){
    for(int j = -1; b[++j] != 0 || a[j] != 0;){
        if(a[j] != b[j])
            return false;
    }
    return true;
}

//Commands in **argv will be executed

int OpenOS_proc_shell(PROCESS_T* proc, int argc, char** argv){

    for(int i = 0; i < 80; i++)
        commandBuffer[i] = 0;

    int bufferPtr = 0;
    int exitCode = 0;
    PROCESS_T* CURRENT_COMMAND = nullptr;

    if(argv == nullptr)
        goto getCommand;

    if(argc && strlen(*argv) < 80){
        memcpy(commandBuffer, *argv, strlen(*argv));
        bufferPtr += strlen(commandBuffer) + 1;
    }

    goto processCommand;

    getCommand:

    TTY1.printf("\n[%c%d%c]->",(exitCode ? RED : GREEN), exitCode, WHITE);

    for(int i = 0; i < 80; i++){
        commandBuffer[i] = getChar();
        if(commandBuffer[i] == '\n'){
            commandBuffer[i] = '\0';
            TTY1.printf("\n");
            goto processCommand;
        }else if(commandBuffer[i] == ' '){
            commandBuffer[i] = '\0';
        }
        else if(commandBuffer[i] == '\r'){
            commandBuffer[i--] = 0;
            commandBuffer[i--] = 0;
            TTY1.backspace();
            continue;
        }
        TTY1.putChar(commandBuffer[i]);
    }

    processCommand:

    if(cmdCheck("debug", commandBuffer)){
        CURRENT_COMMAND = createProc(proc,"[debug]",proc->permissions,debug_CMD);
    }
    else if(cmdCheck("time", commandBuffer)){
        CURRENT_COMMAND = createProc(proc,"[time]",proc->permissions,time_CMD);
    }
    else if(cmdCheck("version", commandBuffer)){
        CURRENT_COMMAND = createProc(proc,"[version]",proc->permissions, version_CMD);
    }
    else if(cmdCheck("ls", commandBuffer)){
        CURRENT_COMMAND = createProc(proc,"[fs]",proc->permissions, ls_CMD);
    }
    else if(cmdCheck("uptime", commandBuffer)){
        CURRENT_COMMAND = createProc(proc, "[INIT]", proc->permissions, uptime_CMD);
    }
    else if(cmdCheck("break", commandBuffer)){
        CURRENT_COMMAND = createProc(proc, "[INIT]", proc->permissions, break_CMD);
    }
    else if(cmdCheck("shutdown", commandBuffer)){
        outw( 0xB004, 0x0 | 0x2000 );
    }

    runCommand:
    
    if(CURRENT_COMMAND != nullptr){
        exitCode = runProc(CURRENT_COMMAND, 0, nullptr);
        killProc(CURRENT_COMMAND);
        CURRENT_COMMAND = nullptr;
    }
    else{
        TTY1.printf("%c%s%c Command Not Found : %s\n",LRED, proc->name,WHITE, commandBuffer);
        exitCode = -127;
    }

    goto getCommand;

    return 0;
}
