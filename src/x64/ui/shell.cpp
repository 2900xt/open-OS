#include <ui/shell.hpp>

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

int debug_CMD(PROCESS_T* proc){

    char* out;

    if(*(commandBuffer + 6))
        out = commandBuffer+6;
    else{
        TTY1.printf("%c%s%c Enter Message: ",LRED, proc->name, WHITE);
        out = getLine();
    }
    debugPrint(out);
    TTY1.printf("%c%s%c Wrote %c%s%c to port %c0xE9%c\n",LRED, proc->name, WHITE, LGREEN, out, WHITE, CYAN, WHITE);
    return 0;
}

int time_CMD(PROCESS_T* proc){
    TIME_T currentTime;
    readRTC(&currentTime);
    TTY1.printf("\n%c%s%c Current Time: \n%d:%d:%d\n\n%c%s%c Date:\n%d/%d/%d\n",LRED,proc->name,WHITE,currentTime.hour,currentTime.minute,currentTime.second,LRED,proc->name,WHITE,currentTime.day,currentTime.month,currentTime.year);
    return 0;
}

int version_CMD(PROCESS_T* proc){
    TTY1.printf("\n%c%s%c OPEN-OS %s%c",LRED, proc->name, PURPLE, OPENOS_VERSION, WHITE );
}

bool cmdCheck(const char* a, const char* b){
    for(int j = -1; b[++j] != 0;){
        if(a[j] != b[j])
            return false;
    }
    return true;
}


int OpenOS_proc_shell(PROCESS_T* proc){

    int exitCode = 0;
    PROCESS_T* CURRENT_COMMAND = nullptr;

    getCommand:

    for(int i = 0; i < 80; i++)
        commandBuffer[i] = 0;

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

    runCommand:
    
    if(CURRENT_COMMAND != nullptr){
        exitCode = runProc(CURRENT_COMMAND);
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
