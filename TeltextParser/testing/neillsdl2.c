#include "neillsdl2.h"



void Instruction_Init(Instruction *instruct){
    instruct->counter = 0;
    instruct->data = SPACE;
    instruct->forecolour = white;
    instruct->backcolour = black;
    instruct->fonthigh = SINGLEHEIGHT;
    instruct->islowpart = FALSE;
    instruct->ischar = TRUE;
    instruct->isseperate = FALSE;
    instruct->ishold = FALSE;

}

void ReadFile(char name[], unsigned char code[]){
    FILE *fp;
    int i = 0;
    fp = fopen(name, "rb");
    if(!fp){
        fprintf(stderr, "%s", "Can't open'\n");
        exit(EXIT_FAILURE);
    }
    while(fscanf(fp, "%c", &code[i++]) != EOF){
    }
    fclose(fp);
}




void Decoder(Instruction *instruct, unsigned char code[]){
    unsigned char order = code[instruct->counter];
    /*this array is used to the character's hight*/
    static int highrecord[LINEWIDTH];
    /*If the programm counter is bigger than 999, then print the 
    last graphic/character/background repeatedly*/
    if(instruct->counter >= CODENUM){
        return;
    }
    
    if(order >= redchcode && order <= whitechcode){
        instruct->data = SPACE;
        /*check the colour is for background or character*/
        /*if it is for background, change background's colour
         and also don't change ischar status'*/
        if(IsForBackColour(code, instruct->counter) == FALSE){
            /*This is not for background, so it is used to change character.
            The code need to make sure that is is in character model */
            instruct->ischar = TRUE;
            instruct->isseperate = FALSE;
        }
        instruct->forecolour = order - redchcode;
        /*changing background and character model will release the holding*/
        instruct->ishold = FALSE;
    }
    /*change hight, singlehcode is 0x8c*/
    else if(order == singlehcode){
        instruct->data = SPACE;
        instruct->fonthigh = SINGLEHEIGHT;
        instruct->ischar = TRUE;
        instruct->ishold = FALSE;
        instruct->isseperate = FALSE;
    }
    else if(order == doublehcode){
        instruct->data = SPACE;
        instruct->fonthigh = DOUBLEHEIGHT;
        instruct->ischar = TRUE;
        instruct->ishold = FALSE;
        instruct->isseperate = FALSE;
    }
    /*change colour and shape*/
    else if(order >= redgracode && order <= whitegracode){
        instruct->ischar = FALSE;
        instruct->forecolour = order - redgracode;
        instruct->fonthigh = SINGLEHEIGHT;
        /*If in hold model, data will not change*/
        /*If not in hold model, changing graph colour will
        make the graph contiguous*/
        if(instruct->ishold == FALSE){
            instruct->data = SPACE;
            instruct->isseperate = FALSE;
        }
    }
    else if(order == nosepcode){
        instruct->ischar = FALSE;
        instruct->isseperate = FALSE;
        instruct->fonthigh = SINGLEHEIGHT;
        instruct->data = SPACE;
        instruct->ishold = FALSE;
    }
    else if(order == sepcode){
        instruct->ischar = FALSE;
        instruct->isseperate = TRUE;
        instruct->fonthigh = SINGLEHEIGHT;
        instruct->data = SPACE;
        instruct->ishold = FALSE;
    }
    else if (order == blackbackcode){
        instruct->backcolour = black;
        instruct->data = SPACE;
        /*changing background will stop holding*/
        instruct->ishold = FALSE;
    }
    else if(order == newbackcode){
        instruct->data = SPACE;
        instruct->backcolour = instruct->forecolour;
        /*changing background will stop holding*/
        instruct->ishold = FALSE;
    }
    /*since is is held, then data could not be changed*/
    else if(order == holdcode){
    /*check invalid code*/
        instruct->ishold = TRUE;
    }
    else if(order == releasecode){
        instruct->data = SPACE;
        instruct->ishold = FALSE;
    }
    else if(order == DATASTART){
        instruct->data = SPACE;
        instruct->ishold = FALSE;
    }
    else if(order > DATASTART || (order >= DATASTART - CODESTART && order < CODESTART)){
        instruct->data = order < CODESTART ? order : order - CODESTART;
        if(instruct->fonthigh == DOUBLEHEIGHT){
            instruct->islowpart = highrecord[instruct->counter % LINEWIDTH]
             == DOUBLEHEIGHT ? TRUE : FALSE;
        }
        else{
            instruct->islowpart = FALSE;
        }
        highrecord[instruct->counter % LINEWIDTH] = instruct->fonthigh;
    }
    else{
    /*
        fprintf(stderr, "%s", "You are not allowed to use this code!\n");
        exit(EXIT_FAILURE);
        */
        printf("code %x is not allowed\n", code[instruct->counter]);
    }   
}



void RenewInstruction(Instruction *instruct){
    instruct->data = SPACE;
    instruct->forecolour = white;
    instruct->backcolour = black;
        /*wrong code*/
    instruct->ischar = TRUE;
    instruct->fonthigh = SINGLEHEIGHT;
    instruct->islowpart = FALSE;
    instruct->isseperate = FALSE;
    instruct->ishold = FALSE;

}

/*Check if next next code is 0x9d. If it is 0x9d, the colour code is for backcolour */
int IsForBackColour(unsigned char code[], int counter){
    return (counter < CODENUM - 1 && code[counter + 1] == newbackcode)? TRUE : FALSE;
}


void My_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], Instruction *instruct){
    unsigned x, y;
    int ox, oy;
    unsigned chr = instruct->data;
    int forered, foregreen, foreblue, backred, backgreen, backblue;
    int h = instruct->fonthigh, islow = instruct->islowpart;
    Colour fcolour = instruct->forecolour;
    Colour bcolour = instruct->backcolour;
    Get_Colour(fcolour, &forered, &foregreen, &foreblue);
    Get_Colour(bcolour, &backred, &backgreen, &backblue);
    ox = instruct->counter % LINEWIDTH * FNTWIDTH;
    oy = instruct->counter / LINEWIDTH * FNTHEIGHT;
    for(y = 0; y < FNTHEIGHT; y++){
        for(x = 0; x < FNTWIDTH; x++){
        /*if h == 1, then [y / h + (h - 1) * islow * FNTHEIGHT /2] is [y])*/
            if(fontdata[chr-FNT1STCHAR][y / h + (h - 1) * islow * FNTHEIGHT /2]
             >> (FNTWIDTH - 1 - x) & 1){
            Neill_SDL_SetDrawColour(sw, forered, foregreen, foreblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y + oy);
        }
        else{
            Neill_SDL_SetDrawColour(sw, backred, backgreen, backblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y + oy);
        }
        }
    }
}


void My_SDL_Draw(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], Instruction *instruct) {
    int chr = instruct->data;
    /*draw blast through text and text*/
    if(instruct->ischar == TRUE || (chr >= BLASTSTART - CODESTART && chr <= BLASTEND - CODESTART)){
        My_SDL_DrawChar(sw, fontdata, instruct);
    }
    else{
        My_SDL_DrawGraph(sw, instruct);
    }
}


void DrawBackground(SDL_Simplewin *sw, Instruction *instruct, int ox, int oy){
    unsigned x, y;
    int backred, backgreen, backblue;
    Colour bcolour = instruct->backcolour;
    Get_Colour(bcolour, &backred, &backgreen, &backblue);
    for(y = 0; y < FNTHEIGHT ; y++){
        for(x = 0; x < FNTWIDTH; x++){
            Neill_SDL_SetDrawColour(sw, backred, backgreen, backblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
        }
    }
}


void My_SDL_DrawGraph(SDL_Simplewin *sw, Instruction *instruct){
    unsigned x, y;
    int i = 0;
    /*here bitnum is the sum of 1 2 4 8 16 or 64*/
    int bitnum = instruct->data - OFFEST;
    int ox = instruct->counter % LINEWIDTH * FNTWIDTH;
    int oy = instruct->counter / LINEWIDTH * FNTHEIGHT;
    DrawBackground(sw, instruct, ox, oy);
    /*Because the order is the result of addition of 1, 2, 4, 8, 16, 64,
    the addition in binary form is meaningful. For example: the number
    1011111 is the result of 1+2+4+8+16+64. If I let the number do right 
    shift and check whether the last bit is 1. If it is 1, then I now there
    is a small square. I also calculate how many right-shifts that I have 
    used, so it is easy for me to know the coordinate.*/
    while(bitnum){
        if(bitnum & 1){
            y = i / 2;
            x = i % 2;
            Draw_Square(sw, instruct, ox + x * WSSQUARE, oy + y * HSSQUARE);
        }
        i += i >= GRAPHBIT - 1 ? 0 : 1;
        bitnum = bitnum >> 1;
    }
    
}

void Draw_Square(SDL_Simplewin *sw, Instruction *instruct, int ox, int oy){
    unsigned x, y;
    unsigned border = instruct->isseperate;
    int forered, foregreen, foreblue;
    Colour fcolour = instruct->forecolour;
    Get_Colour(fcolour, &forered, &foregreen, &foreblue);
    for(y = 0 + border; y < FNTHEIGHT / GRAPHVNUM - border; y++){
        for(x = 0 + border; x < FNTWIDTH / GRAPHHNUM - border; x++){
            Neill_SDL_SetDrawColour(sw, forered, foregreen, foreblue);
            SDL_RenderDrawPoint(sw->renderer, x + ox, y+oy);
        }
    }
}

/*enum Colour{red = 0, green, yellow, blue, magenta, cyan, white, black};
My colour sequence is the same as TELTEXT control code. If the colour's number
is added 1, then red=0b001, green=0b010, yellow=0b011, blue=0b100, etc. 
As a result, the number in binary form is ox_isblue_isgreen_isred.
If isblue==TRUE, then it will be 1*/

void Get_Colour(Colour colour, int *cred, int *cgreen, int *cblue){
    int colour_code = (int)colour + 1;
    *cred = (colour_code & 1) * COLOURMAX;
    *cgreen = (colour_code >> 1 & 1) * COLOURMAX;
    *cblue = (colour_code >> 2 & 1) * COLOURMAX;
}




void Neill_SDL_Init(SDL_Simplewin *sw)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    } 

    sw->finished = 0;
   
    sw->win= SDL_CreateWindow("SDL Window",
                           SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED,
                           WWIDTH, WHEIGHT,
                           SDL_WINDOW_SHOWN);
    if(sw->win == NULL){
       fprintf(stderr, "\nUnable to initialize SDL Window:  %s\n", SDL_GetError());
       SDL_Quit();
       exit(EXIT_FAILURE);
    }

    sw->renderer = SDL_CreateRenderer(sw->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    if(sw->renderer == NULL){
        fprintf(stderr, "\nUnable to initialize SDL Renderer:  %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(sw->renderer,SDL_BLENDMODE_BLEND);

    /* Create texture for display */
    sw->display = SDL_CreateTexture(sw->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WWIDTH, WHEIGHT);
    if(sw->display == NULL){
        fprintf(stderr, "\nUnable to initialize SDL texture:  %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderTarget(sw->renderer, sw->display);

    /* Clear screen, & set draw colour to black */
    SDL_SetRenderDrawColor(sw->renderer, COLOURMIN, COLOURMIN, COLOURMIN, COLOURMAX);
    SDL_RenderClear(sw->renderer);

}

/* Housekeeping to do with the render buffer & updating the screen */
void Neill_SDL_UpdateScreen(SDL_Simplewin *sw)
{
    SDL_SetRenderTarget(sw->renderer, NULL);
    SDL_RenderCopy(sw->renderer, sw->display, NULL, NULL);
    SDL_RenderPresent(sw->renderer);
    SDL_SetRenderTarget(sw->renderer, sw->display);
}

/* Gobble all events & ignore most */
void Neill_SDL_Events(SDL_Simplewin *sw)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {      
         switch (event.type){
             case SDL_QUIT:
             case SDL_MOUSEBUTTONDOWN:
             case SDL_KEYDOWN:
                 sw->finished = 1;
         }
    }
}


/* Trivial wrapper to avoid complexities of renderer & alpha channels */
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetRenderDrawColor(sw->renderer, r, g, b, SDL_ALPHA_OPAQUE);
}



void Neill_SDL_ReadFont(fntrow fontdata[FNTCHARS][FNTHEIGHT], char *fname)
{
    FILE *fp = fopen(fname, "rb");
    size_t itms;
    if(!fp){
        fprintf(stderr, "Can't open Font file %s\n", fname);
        exit(EXIT_FAILURE);
    }
    itms = fread(fontdata, sizeof(fntrow), FNTCHARS*FNTHEIGHT, fp);
    if(itms != FNTCHARS*FNTHEIGHT){
        fprintf(stderr, "Can't read all Font file %s (%d) \n", fname, (int)itms);
        exit(EXIT_FAILURE);
    }
    fclose(fp);
}

