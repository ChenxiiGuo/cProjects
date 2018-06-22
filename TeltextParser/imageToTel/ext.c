#include <stdio.h>
#include <stdlib.h>
#include "neillsdl2.h"
#include <SDL_image.h>
#include <SDL.h>

int main(int argc, char *argv[]){
    SDL_Simplewin sw;
    SDL_Surface *image;
    fntrow fontdata[FNTCHARS][FNTHEIGHT];
    Instruction instruct;
    /*the color_set will have the color information. I use 0~7 to store
     8 different colour*/
    int color_set[HEIGHT][WIDTH];
    /*I will temperarily put my teltext codes in this array.
    Finally, write them into a m7 file*/
    int code_set[CODEH][CODEW];
    /*the hash value of every character's structure including double height */
    int font_set_hash[HASHSIZE];
    int red_l[HEIGHT][WIDTH], green_l[HEIGHT][WIDTH], blue_l[HEIGHT][WIDTH];
    
    if(argc != ARGVSIZE) {
        fprintf(stderr, "%s", "The arguments is wrong\n");
        exit(EXIT_FAILURE);
    }
    
    Neill_SDL_ReadFont(fontdata, "m7fixed.fnt");
    /*intialize the code array, give them 0xa0*/
    Initialize_Code(code_set);
    
    Neill_SDL_Init(&sw);
    
    image = IMG_Load(argv[1]);
    
    SDL_LockSurface(image);
    /*get the colour information from picture*/
    Get_Colourset(image, red_l, green_l, blue_l, color_set);
    
    SDL_UnlockSurface(image);
    
    Instruction_Init(&instruct);
    
    Get_FontHash(fontdata, font_set_hash);
    
    
    while(instruct.counter < CODENUM){
        Get_InsFromGraph(&instruct, font_set_hash, color_set);
        Write_Code(&instruct, code_set);
        instruct.counter++;
    }
    
    PrintCode(code_set);
    
    Write_IntoFile("rev.m7", code_set);
    atexit(SDL_Quit);

    return 0;
}

void Initialize_Code(int code[CODEH][CODEW]){
    int i, j;
    for(i = 0; i < CODEH; i++){
        for(j = 0; j < CODEW; j++){
            code[i][j] = DATASTART;
        }
    }
}

void Get_Colourset(SDL_Surface *image, int red_l[][WIDTH], int green_l[][WIDTH], int blue_l[][WIDTH], int color_set[][WIDTH]){
    SDL_PixelFormat *fmt;
    Uint32 x, y;
    Uint32 p;
    Uint8 red, green, blue;
    fmt = image->format;
    for(y = 0; y < HEIGHT; y ++){
        for(x = 0; x < WIDTH; x++){
            p = GetPixel(image, x, y);
            SDL_GetRGB(p, fmt, &red, &green, &blue);
            red_l[y][x] = red;
            green_l[y][x] = green;
            blue_l[y][x] = blue;
            /*red = 0, green = 1, yellow = 2......*/
            color_set[y][x] = red/COLOURMAX * 1 + green / COLOURMAX* (1 << 1) + 
            blue / COLOURMAX * (1 << 2) - 1;
            color_set[y][x] = color_set[y][x] < 0 ? (COLOURNUM - 1) : color_set[y][x];
        }
    }
    

}

void Get_InsFromGraph(Instruction *instruct, int font_set_hash[], int color_set[HEIGHT][WIDTH]){
    int x, y;
    y = instruct->counter / CODEW * FNTHEIGHT;
    x = instruct->counter % CODEW * FNTWIDTH;

    if(x == 0){
        RenewInstruction(instruct);
    }
    /*the get_data function's return value is the ascii code, not control code.
    But during the process, the control code could be achieved*/
    instruct->data = Get_data(instruct, x, y, color_set, font_set_hash);
}


int Get_data(Instruction *instruct, int ox, int oy, int color_set[][WIDTH], int font_set_hash[]){

    int hash_value, next_x, next_y, i = 0;
    
    /*make an assumption that the top-left is always the background colour.
    In graph model, it is not always right. But in graphic model I will check it.
    So in character model, first_colour is background. In graphic, it is not.*/
    int first_colour = color_set[oy][ox];
    int second_colour = instruct->forecolour;
    
    /*get a small cell's hash value'*/
    hash_value = Get_Hash_Value(ox, oy, color_set, &second_colour);
    
    /*next cell's coordinate is used to guess whehter it is a 0xff in graphic model
    or changing background*/
    next_y = (instruct->counter + 1) / CODEW * FNTHEIGHT;
    next_x = (instruct->counter + 1) % CODEW * FNTWIDTH;
    
    /*if in graphic model, I will check if the holding model should be changed*/
    if(instruct->ischar == FALSE && Check_Holding(instruct, ox, oy, color_set) == TRUE){
        return Control_CodeForHold(instruct, first_colour, second_colour);
    }

    /*compare the hash_value of current with the library. Check whehter it is a char.
    But it doesn't mean that it is in character model, because SPACE and blast-through
    test should be considered'*/
    if(Check_If_Char(hash_value, font_set_hash, &i) == TRUE){
       
        Check_CharColourChange(instruct, second_colour);
        
        Check_HeightChange(instruct, i);
        
        /*blast-through should be considered*/
        if(i && instruct->ischar == FALSE && (i % FNTCHARS + DATASTART < BLASTSTART || 
        i % FNTCHARS + DATASTART > BLASTEND)){
            instruct->ischar = TRUE;
            instruct->graphchng = TRUE;
        }
        /*if hash value is 0, then it should not be in seperated model*/
        instruct->isseperate = i == 0 ? instruct->isseperate : FALSE;
        
        /* check 0xff in graph model*/
        if(i == 0 && first_colour == (int)instruct->forecolour && instruct->ischar == FALSE){
            return Get_Data_From_Graph(instruct,ox, oy, color_set, first_colour, second_colour);
        }

        if(i == 0 && ox / FNTWIDTH < WIDTH - 1 && ox > 0){
        /*It is very hard to tell the difference between background change and oxFF
        My method is checking whehter current background color appears in next cell. If it appears,
        this means that it is oxFF rather than changing background. This method is not a perfect one.
        */
            if(Check_If_Contain_Colour1(color_set, instruct->backcolour, next_x, next_y) == TRUE){
                if(first_colour != (int)instruct->backcolour){
                    return Get_Data_From_Graph(instruct,ox, oy, color_set, first_colour, second_colour);
                }
                return SPACE;
            }
            Check_BackColourChange(instruct, first_colour);
            
        }
        return i % FNTCHARS + OFFEST;
    }
    /*if not found*/
    return Get_Data_From_Graph(instruct, ox, oy, color_set, first_colour, second_colour);
}

/*if the graph has features that show it need to be held, I will check whehter I need give 
    a change instruction (0x9e or ox9f). If not, I will just give a forecolour (0x91~0x97)*/
int Control_CodeForHold(Instruction *instruct, int first_colour, int second_colour){
    if(instruct->holdchng == TRUE){
            instruct->holdchng = FALSE;
            return instruct->ishold == TRUE ? holdcode - CODESTART : releasecode - CODESTART;
    }
    else {
        if(first_colour != (int)instruct->backcolour){
            return first_colour + redgracode - CODESTART;
        }
        else{
            return second_colour + redgracode - CODESTART;
        }
    }
}

/*my hash algorithm is like the fontfile. Each line's number is multiplied by
    the line number. Then add them together.*/

int Get_Hash_Value(int ox, int oy, int color_set[][WIDTH], int *second_colour){
    int x, y, sum_line = 0, hash_value = 0;
    int first_colour = color_set[oy][ox];
    for(y = 0; y < FNTHEIGHT; y++){
        for(x = 0; x < (int)FNTWIDTH; x++){
            if(color_set[oy + y][ox + x] != first_colour){
                *second_colour = color_set [oy + y][ox + x];
                sum_line = sum_line | (1 << (FNTWIDTH - 1 - x));
            }
        }
        hash_value += sum_line * (y + 1);
        sum_line = 0;
    }
    return hash_value;
}


/*Check holding is partly based on guess :)
 First check the hash value of current cell, previous cell and next cell. If they are same and
 the colour continunously change, then it is holding. They will only be done in graphic model,
  so don't worry about test like "aaaaa"*/
int Check_Holding(Instruction *instruct, int ox, int oy, int color_set[][WIDTH]){
    int next_x = ox + FNTWIDTH;
    int prev_x = ox - FNTWIDTH;
    int second_colour = color_set[oy][ox];
    int next_second_colour = color_set[oy][next_x];
    int prev_second_colour = color_set[oy][prev_x];
    int current_hashvalue = Get_Hash_Value(ox, oy, color_set, &second_colour);
    int next_hashvalue = Get_Hash_Value(next_x, oy, color_set, &next_second_colour);
    int prev_hashvalue = Get_Hash_Value(prev_x, oy, color_set, &prev_second_colour);
    if(next_x >= WIDTH || prev_x < 0){
        instruct->ishold = FALSE;
        return FALSE;
    }
    if(current_hashvalue != next_hashvalue || current_hashvalue != prev_hashvalue){
        return FALSE;
    }
        /*in case 0xf3 and 0xea, although two picture has same hash value, they are different*/
    if(current_hashvalue && color_set[oy][next_x] != color_set[oy][ox]){
        if(color_set[oy][ox] == next_second_colour){
            return FALSE;
        }
    }
    if(second_colour == next_second_colour ){
        return FALSE;
    }
    if(instruct->ishold == FALSE){
        instruct->holdchng = TRUE;
        instruct->ishold = TRUE;
    }
    return TRUE;
}

void Check_CharColourChange(Instruction *instruct, int forecl){
    if(instruct->forecolour != (unsigned int)forecl){
        instruct->forecolour = (Colour)forecl;
        instruct->forecolchng_char = TRUE;
    }
    else{
        instruct->forecolchng_char = FALSE;
    }
}

void Get_FontHash(fntrow fontdata[FNTCHARS][FNTHEIGHT], int font_set_hash[HASHSIZE]){
    int i, y = 0;

    for(i = 0; i < HASHSIZE; i++){
        for(y = 0; y < FNTHEIGHT; y++){
            if(i < FNTCHARS){
                font_set_hash[i] += fontdata[i%FNTCHARS][y] * (y + 1);
            }
            else if(i >= FNTCHARS && i < FNTCHARS * 2){
                font_set_hash[i] += fontdata[i%FNTCHARS][y / 2] * (y + 1);
            }
            else{
                font_set_hash[i] += fontdata[i%FNTCHARS][FNTHEIGHT / 2 + y / 2] * (y + 1);
            }
        }

    }
}

void PrintCode(int code_set[][CODEW]){
    int i, j, k = 0;
    for(i = 0; i < CODEH; i++){
        for(j = 0; j < CODEW; j++){
            printf("%x ", code_set[i][j]);
            if(k % PLINE == PLINE - 1){
                printf("**\n");
            }
            k++;
        }
    }
}

/*Declare: this function is from internet*/
Uint32 GetPixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;
    }
}



void Check_HeightChange(Instruction *instruct, int order){
    int current_high = SINGLEHEIGHT;
    int data = order % FNTCHARS + OFFEST;
    if(data == SPACE){
        return;
    }
    instruct->heightchng = FALSE;
    if(order >= FNTCHARS){
        current_high = DOUBLEHEIGHT;
    }
    if(current_high != instruct->fonthigh){
        instruct->heightchng = TRUE;
    }
    instruct->fonthigh = current_high;
}



int Get_Data_From_Graph(Instruction *instruct, int ox, int oy, int color_set[][WIDTH],
                       int colour_1, int colour_2){
    int x, y, x_edge, y_edge, res = 0, i = 0, is_seperate = TRUE;
    /*need to be improved*/
    int fore_colour = (colour_1 == (int)instruct->backcolour ? colour_2 : colour_1);
    instruct->forecolour = fore_colour;
    if(instruct->ischar == TRUE){
        instruct->ischar = FALSE;
        instruct->graphchng = TRUE;
    }
    /*GRAPHBIT is 6*/
    while(i < GRAPHBIT){
        /*x, y is the coordinate of each small square's middle point*/
        x = (i % 2) * FNTWIDTH / 2 + FNTWIDTH / 2 / 2;
        y = (i / 2) * FNTHEIGHT / GRAPHVNUM + FNTHEIGHT / GRAPHVNUM / 2;
        x_edge = i % 2 == 0 ? 0 : FNTWIDTH - 1;
        y_edge = (i / 2) * FNTHEIGHT / GRAPHVNUM + FNTHEIGHT / GRAPHVNUM / 2;
        /*check each small square's edge. if all squares' edge is back_colour,
        it means that this is seperated graph(space has been dealed with )*/
        if(color_set[oy + y_edge][ox + x_edge] == fore_colour){
            is_seperate = FALSE;
        }
        if(color_set[oy + y][ox + x] == fore_colour){
            res += (i == GRAPHBIT - 1 ? 1 << GRAPHBIT : 1 << i); 
        } 
        i++;
    }
    if(instruct->isseperate == FALSE){
        if(is_seperate == TRUE){
            instruct->seperatechng = TRUE;
            instruct->isseperate = TRUE;
        }
    }
    if(instruct->isseperate == TRUE){
        if(is_seperate == FALSE){
            instruct->seperatechng = TRUE;
            instruct->isseperate = FALSE;
        }
    }

    instruct->ischar = FALSE;
    return  res + OFFEST;
}

int Check_If_Contain_Colour1(int color_set[][WIDTH], int colour1, int ox, int oy){
    int x, y;
    for(y = 0; y < FNTHEIGHT; y++){
        for(x = 0; x < (int)FNTWIDTH; x++){
            if(colour1 == color_set[oy + y][ox + x]){
                return TRUE;
            }
        }
    }
    return FALSE;
}

void Check_BackColourChange(Instruction *instruct, int first_colour){
    if(instruct->backcolour != black && first_colour == black){
        instruct->backblackchng = TRUE;
        instruct->backcolour = black;
        return;
    }
/*
    if(instruct->backcolour== black && first_colour != black && (int)instruct->forecolour != first_colour){*/
    /*if(instruct->backcolour== black && first_colour != black ){*/
    if(instruct->backcolour != (unsigned int)first_colour){
        instruct->backcolchng = TRUE;
        instruct->backcolour= first_colour;
    }

}



/*check if this is char*/
int Check_If_Char(int hash_value, int font_set_hash[], int *i){
    /*the data is not too much, so I think linear search is acceptable*/
    for(*i = 0; *i < HASHSIZE; (*i)++){
        if(font_set_hash[*i] == hash_value){
            return TRUE;
        }
    }
    return FALSE;
}






void Put_Controlcode_Spaceslot(int code_set[][CODEW], int code, int x, int y){
    int i = x - 1;
    while(code_set[y][i] != DATASTART && i > 0){
        i--;
    }
    i = i < 0 ? 0 : i;
    code_set[y][i] = code;
}

void Write_Code(Instruction *instruct, int code_set[][CODEW]){
    int x, y;
    /*avoid set character colour twice */
    int flag_colchng = FALSE;
    y = instruct->counter / CODEW;
    x = instruct->counter % CODEW;
    
    code_set[y][x] = instruct->data + CODESTART;
    /*check color change*/
    if(instruct->forecolchng_char == TRUE){
        flag_colchng = TRUE;
        Put_Controlcode_Spaceslot(code_set, redchcode + instruct->forecolour, x, y);
    }
    if(instruct->heightchng){
        if(instruct->fonthigh == SINGLEHEIGHT){
            Put_Controlcode_Spaceslot(code_set, singlehcode, x, y);
        }
        else{
            Put_Controlcode_Spaceslot(code_set, doublehcode, x, y);
        }
        instruct->heightchng = FALSE;
    }
    /*sequence is important 9a is superior than others*/
    if(instruct->seperatechng){
        if(instruct->isseperate == TRUE){
            Put_Controlcode_Spaceslot(code_set, sepcode, x, y);
        }
        if(instruct->isseperate == FALSE){
            Put_Controlcode_Spaceslot(code_set, redgracode + instruct-> forecolour, x, y);
        }
        instruct->seperatechng = FALSE;
    }
    if(instruct->graphchng){
        if(instruct->ischar == FALSE){
            Put_Controlcode_Spaceslot(code_set, redgracode + instruct->forecolour, x, y);
        }
        else{
        /*if character's colour has been changed, don't change it again'*/
            if(flag_colchng == FALSE){
                Put_Controlcode_Spaceslot(code_set, redchcode + instruct->forecolour, x, y);
            }
        }
        instruct->graphchng = FALSE;
    }
    if(instruct->backcolchng){
        code_set[y][x] = newbackcode;
        Put_Controlcode_Spaceslot(code_set, redchcode + instruct->backcolour, x, y);
        instruct->backcolchng = FALSE;
    }
    if(instruct->backblackchng){
        code_set[y][x] = blackbackcode;
        instruct->backblackchng = FALSE;
    }
}


void Write_IntoFile(char name[], int code_set[][CODEW]){
    FILE *fp;
    int i, j;
    fp = fopen(name, "w");
    for(i = 0; i < CODEH; i++){
        for(j = 0; j < CODEW; j++){
            fprintf(fp, "%c", code_set[i][j]);
        }
    }
    fclose(fp);       
}

