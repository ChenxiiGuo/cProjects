
/*there are some teltext code. I didn't change them to define or enum. becuase I want to test the
original code*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "neillsdl2.h"

#define DHCN 14
#define DSCN 10
#define DHC 7
#define DCN 7


void test_ReadFile(char filename[], int *errorNum);
void test_RenewInstruction(int *perrorNum);
void test_Instruction_Init(int *perrorNum);
void test_Get_Colour(int *perrorNum);
void test_Decoder(int *perrorNum);

void make_Test_File(char filename[]);
void test_Decoder_Colour(int *perrorNum);
void test_Decoder_G_C_Change(int *perrorNum);
void test_Decoder_Height_Change(int *perrorNum);
void test_Decoder_Seperate_Change(int *perrorNum);
void test_Decoder_Hold_Change(int *perrorNum);

int main(void) {
    int errorNum = 0;
    printf("Test......start\n");
    test_ReadFile("testfile.m7", &errorNum);
    test_Instruction_Init(&errorNum);
    test_RenewInstruction(&errorNum);
    test_Get_Colour(&errorNum);
    test_Decoder(&errorNum); 
    printf("Totally there are %d errors\n", errorNum);
    printf("Test......finished\n");
    return 0;
}

/*make a file contains 0x80~0xff continuously*/
void test_ReadFile(char filename[], int *perrorNum) {
    int old_errorNum = *perrorNum;
    int error_This = 0;
    int i;
    unsigned char code[CODENUM];
    make_Test_File(filename);
    ReadFile(filename, code);
    for(i = 0; i < CODENUM; i++){
        if(code[i] != (0x80 + i) % (0xff + 1)){
            (*perrorNum)++;
        }
    }
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in ReadFile.\n", error_This);
}

void test_Instruction_Init(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct;
    Instruction_Init(&instruct);
    (*perrorNum) += instruct.counter != 0;
    (*perrorNum) += instruct.data != SPACE;
    (*perrorNum) += instruct.forecolour != white;
    (*perrorNum) += instruct.backcolour != black;
    (*perrorNum) += instruct.islowpart != FALSE ;
    (*perrorNum) += instruct.ischar != TRUE ;
    (*perrorNum) += instruct.fonthigh != SINGLEHEIGHT ;
    (*perrorNum) += instruct.isseperate != FALSE;
    (*perrorNum) += instruct.ishold != FALSE;
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Instruction_Init.\n", error_This);
    /*Because the Init function should also be used. If it has bug, then, we can't
    test the code'*/
    if(error_This > 0){
        fprintf(stderr, "%s", "Instruction init test faild, can't test the code'\n");
        exit(EXIT_FAILURE);
    }

}

void make_Test_File(char filename[]) {
    FILE *fp;
    int i, num;
    fp = fopen(filename, "w");
    if(!fp){
        fprintf(stderr, "%s", "Can't open file'\n");
    }
    for(i = 0; i < CODENUM; i++) {
        num = (0x80 + i) % (0xff + 1);
        fprintf(fp, "%c", num);
    }
    fclose(fp);
}


void test_RenewInstruction(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct;
    RenewInstruction(&instruct);
    (*perrorNum) += instruct.data != SPACE;
    (*perrorNum) += instruct.forecolour != white;
    (*perrorNum) += instruct.backcolour != black;
    (*perrorNum) += instruct.ischar != TRUE;
    (*perrorNum) += instruct.islowpart != FALSE;
    (*perrorNum) += instruct.fonthigh != SINGLEHEIGHT;
    (*perrorNum) += instruct.isseperate != FALSE;
    (*perrorNum) += instruct.ishold != FALSE;
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in RenewInstruction.\n", error_This);
}


void test_Decoder(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    test_Decoder_Colour(perrorNum);
    /*G_C means graph and character*/
    test_Decoder_G_C_Change(perrorNum);
    test_Decoder_Height_Change(perrorNum);
    test_Decoder_Seperate_Change(perrorNum);
    test_Decoder_Hold_Change(perrorNum);

    
    
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder.\n", error_This);
}


void test_Decoder_Hold_Change(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct_turn, instruct_hold;
    /*In my design, only instructions 0x91~0x97 will not disturb the hold model*/
    unsigned char code_turn[DHCN] = {0x9e, 0x9f, 0x9e, 0x81, 0x9e, 0x9c, 0x9e, 0x9d,
    0x9e, 0x9c, 0x9e, 0x99, 0x9e, 0x9a};
    unsigned char code_hold[DHCN] = {0x9e, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
    0x9e, 0x9e, 0x91, 0x9e, 0x94, 0x9e};
    Instruction_Init(&instruct_turn);
    Instruction_Init(&instruct_hold);
    do {
        Decoder(&instruct_turn, code_turn);
        Decoder(&instruct_hold, code_hold);
        /*if counter is a even number, the height should be normal.
        if counter is an odd number, the height should be double*/
        if(instruct_turn.counter % 2 == 0) {
            (*perrorNum) += instruct_turn.ishold != TRUE;
        }
        else {
            (*perrorNum) += instruct_turn.ishold != FALSE;
        }
        (*perrorNum) += instruct_hold.ishold!= TRUE;
        error_This = *perrorNum - old_errorNum;
        instruct_turn.counter++;
        instruct_hold.counter++;

    }while(instruct_turn.counter < DHCN);
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder_hold change.\n", error_This);
}



void test_Decoder_Seperate_Change(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct_turn, instruct_hold;
    /*If counter is a even number, the isseperate should be true.
    If counter is a odd numberm the isseperate should be false.
    I test alphanumeric change, single height and double height.
    Also if not in holding model, changing graphics colour also makes
    isseperate be false*/
    unsigned char code_turn[DSCN] = {0x9a, 0x99, 0x9a, 0x81, 0x9a, 0x8c, 0x9a, 
    0x8d, 0x9a, 0x91};
    /*In this test code, the isseperate should always be ture.
    0x81 0x9d, in my design, if they are combined together, they are
    used to change background. It will not influence the isseperate's value*/
    unsigned char code_hold[DSCN] = {0x9a, 0x9e, 0x91, 0x92, 0x92, 0x93, 0x97, 
    0x9c, 0x81, 0x9d};
    Instruction_Init(&instruct_turn);
    Instruction_Init(&instruct_hold);
    do {
        Decoder(&instruct_turn, code_turn);
        Decoder(&instruct_hold, code_hold);
        /*if counter is a even number, the height should be normal.
        if counter is an odd number, the height should be double*/
        if(instruct_turn.counter % 2 == 0) {
            (*perrorNum) += instruct_turn.isseperate != TRUE;
        }
        else {
            (*perrorNum) += instruct_turn.isseperate != FALSE;
        }
        (*perrorNum) += instruct_hold.isseperate!= TRUE;
        error_This = *perrorNum - old_errorNum;
        instruct_turn.counter++;
        instruct_hold.counter++;
    }while(instruct_turn.counter < DSCN);
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder_seperate change.\n", error_This);
}


void test_Decoder_Height_Change(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct;
    /*randomly choose a graphic colour changing code and all other code that
    need to change the height of character*/
    unsigned char code[] = {0x8c, 0x8d, 0x91, 0x8d, 0x99, 0x8d, 0x9a};
    Instruction_Init(&instruct);
    do {
        Decoder(&instruct, code);
        /*if counter is a even number, the height should be normal.
        if counter is an odd number, the height should be double*/
        if(instruct.counter % 2 == 0) {
            (*perrorNum) += instruct.fonthigh != SINGLEHEIGHT;
        }
        else {
            (*perrorNum) += instruct.fonthigh != DOUBLEHEIGHT;
        }
        instruct.counter++;
    }while(instruct.counter < DHC); 
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder_height change.\n", error_This);
}

void test_Decoder_G_C_Change(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct;
    /*Uing instruct character colour, background colour, single height, continuous
    graph, double height and seperated graph. The character and graph model should 
    take turns to appear */
    unsigned char code[6] = {0x81, 0x92, 0x8c, 0x99, 0x8d, 0x9a};
    /*If instruction_init has problems, in test_Decoder_Colour the program will exit*/
    Instruction_Init(&instruct);
    do {
        Decoder(&instruct, code);
        /*if counter is a even number, the ischar should be ture.
        As a result, if it is false, *perrorNum will be added one*/
        if(instruct.counter % 2 == 0) {
        
            (*perrorNum) += instruct.ischar == FALSE;
        }
        else {
            (*perrorNum) += instruct.ischar == TRUE;
        }
        instruct.counter++;
    }while(instruct.counter < 6);
    
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder_Graph_Char change.\n", error_This);
}


void test_Decoder_Colour(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    Instruction instruct_fore, instruct_back;
    unsigned char char_forecolour_code[DCN] = {0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87};
    unsigned char char_backcolour_code[DCN] = {0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97};
    test_Instruction_Init(perrorNum);
    Instruction_Init(&instruct_fore);
    Instruction_Init(&instruct_back);
    
    do {
        Decoder(&instruct_fore, char_forecolour_code);
        Decoder(&instruct_back, char_backcolour_code);
        /*enum Colour{red = 0, green, yellow, blue, magenta, cyan, white, black};
        and colour has been tested in test_Get_Colour. As a result, using red +
        instruct->counter is acceptable*/
        (*perrorNum) += (int)instruct_fore.forecolour != red + instruct_fore.counter;
        (*perrorNum) += (int)instruct_back.forecolour != red + instruct_back.counter;
        instruct_fore.counter++;
        instruct_back.counter++;
    }
    while(instruct_back.counter < 0x87 - 0x81 + 1);
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Decoder colour part.\n", error_This);
}

/*test whether Get_Colour can get right RGB*/
void test_Get_Colour(int *perrorNum){
    int old_errorNum = *perrorNum;
    int error_This = 0;
    int cred, cgreen, cblue;
    
    Get_Colour(red, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 255 && cgreen == 0 && cblue == 0) == FALSE;
    
    Get_Colour(green, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 0 && cgreen == 255 && cblue == 0) == FALSE; 
    
    Get_Colour(yellow, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 255 && cgreen == 255 && cblue == 0) == FALSE; 
    
    Get_Colour(blue, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 0 && cgreen == 0 && cblue == 255) == FALSE; 
    
    Get_Colour(magenta, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 255 && cgreen == 0 && cblue == 255) == FALSE;
    
    Get_Colour(cyan, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 0 && cgreen == 255 && cblue == 255) == FALSE;  
    
    Get_Colour(white, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 255 && cgreen == 255 && cblue == 255) == FALSE;
    
    Get_Colour(black, &cred, &cgreen, &cblue);
    (*perrorNum) += (cred == 0 && cgreen == 0 && cblue == 0) == FALSE;          
    
    error_This = *perrorNum - old_errorNum;
    printf("There are totally %d errors in Get_Colour.\n", error_This);
}





