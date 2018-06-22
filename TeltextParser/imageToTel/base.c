/*My idea comes from the Computer Architecture course. We bulit a Hex8 processor.
I think I could simulate the processor's way to deal with teltest code.
First read code to a "memory"(code array). 
Then use decoder to decode these teltext code, changing them from "Not machine code"
to "machine code"(Actually, it is language C).
The instruct.counter works a program counter which could let the porcessor excute the next
code. 
The My_SDL_Draw fucntion is used to excute and output.

I don't use any 2-d array to store my instructions. Becuase I believe that using one STRUCTURE
to store all the instructions is enough. All the codes don't require us to "go back", so next 
state only be decided by current state and the new code. It is like a Finite State Machine.
I think using a structure to store current state is enough.*/

#include <stdio.h>
#include <stdlib.h>
#include "neillsdl2.h"



int main(int argc, char *argv[]){
    SDL_Simplewin sw;
    Instruction instruct;
    fntrow fontdata[FNTCHARS][FNTHEIGHT];
    unsigned char code[CODENUM];
    if(argc != ARGVSIZE) {
        fprintf(stderr, "%s", "The arguments is wrong\n");
        exit(EXIT_FAILURE);
    }
    
    ReadFile(argv[1], code);
    Neill_SDL_Init(&sw);
    Instruction_Init(&instruct);
    
    Neill_SDL_ReadFont(fontdata, "m7fixed.fnt");
    
    do{
        /*Decoder will change the instruction set*/
        Decoder(&instruct, code);
        /*My_SDL_Draw will excute the instructs and print them*/
        My_SDL_Draw(&sw, fontdata, &instruct);
        Neill_SDL_Events(&sw);
        /*renew instructions after a whole line*/
        if(instruct.counter % LINEWIDTH == LINEWIDTH - 1){
            RenewInstruction(&instruct);
        }
        /*Decoder could cope with the situation when counter is 
        bigger than 999*/
        /*counter works as programcounter*/
        instruct.counter++;
        if(instruct.counter >= CODENUM){
            Neill_SDL_UpdateScreen(&sw);
        }

    }while(!sw.finished);
    

    atexit(SDL_Quit);
    return 0;
}



