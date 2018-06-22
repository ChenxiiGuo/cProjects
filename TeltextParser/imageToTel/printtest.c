#include <stdio.h>
#include <stdlib.h>
#include <SDL_image.h>

#include "neillsdl2.h"


#define ARGVNUM 3
#define DEFAULT 32
#define SCROFFESET 150

#define RM 0xff000000
#define GM 0x00ff0000
#define BM 0x0000ff00
#define AM 0x000000ff
/*My method to test my print code is pirnt the picture and save them
as testgraph.BMP. Then reload the picture and compare the pixels' color with
standard graphs. If they are same, then pass.*/

void print_code(unsigned char code[], fntrow fontdata[FNTCHARS][FNTHEIGHT], SDL_Surface *sshot, SDL_Simplewin *sw);
void invert_image(int pitch, int height, void* image_pixels);
void Save_Picture(SDL_Simplewin *sw, SDL_Rect *rect, SDL_Surface *sshot);
Uint32 GetPixel(SDL_Surface *surface, int x, int y);

/*get the surface's RGB information'*/
void Get_Colourset_1(SDL_Surface *image, int color_set[][WIDTH]);

/*compare current with standard picture*/
int Comp_SavePicture_Standard(char *pname);

/*compare saved picture with standard picture*/
int Comp_Surface_Standard(SDL_Surface *surf, char *pname);

void Srcect_Init(SDL_Rect *rect);

int main(int argc, char *argv[]){
    SDL_Simplewin sw;
    Instruction instruct;
    fntrow fontdata[FNTCHARS][FNTHEIGHT];
    unsigned char code[CODENUM];
    SDL_Rect rect;
    SDL_Surface *sshot = SDL_CreateRGBSurface(1, WIDTH, HEIGHT, DEFAULT, RM, GM, BM, AM);
    int wrong_surface, wrong_sshot;
    if(argc != ARGVNUM) {
        fprintf(stderr, "%s", "The arguments is wrong\n");
        exit(EXIT_FAILURE);
    }
    
    Srcect_Init(&rect);
    ReadFile(argv[1], code);
    Neill_SDL_Init(&sw);
    Instruction_Init(&instruct);
    Neill_SDL_ReadFont(fontdata, "m7fixed.fnt");
    
    do{
        Decoder(&instruct, code);
        My_SDL_Draw(&sw, fontdata, &instruct);
        Neill_SDL_Events(&sw);
        if(instruct.counter % LINEWIDTH == LINEWIDTH - 1){
            RenewInstruction(&instruct);
        }
        instruct.counter++;

    }while(instruct.counter < CODENUM);

    Save_Picture(&sw, &rect, sshot);

    wrong_surface = Comp_Surface_Standard(sshot, argv[2]);
    wrong_sshot = Comp_SavePicture_Standard(argv[2]);
    
    printf("The number of wrong pixel is %d in screen shot surface\n", wrong_surface);
    printf("the number of wrong pixel is %d in saved picture of screen shot\n", wrong_sshot);

    SDL_FreeSurface(sshot); 
    atexit(SDL_Quit);
    return 0;
}

int Comp_SavePicture_Standard(char *pname){
    SDL_Surface *savep;
    savep = IMG_Load("test.BMP");
    return Comp_Surface_Standard(savep, pname); 
}

int Comp_Surface_Standard(SDL_Surface *surf, char *pname){
    int surf_colour[HEIGHT][WIDTH];
    int standd_colour[HEIGHT][WIDTH];
    int i, j, num_wrong = 0;
    SDL_Surface *standard;
    SDL_LockSurface(surf);
    Get_Colourset_1(surf, surf_colour);
    SDL_UnlockSurface(surf);
    standard = IMG_Load(pname);
    Get_Colourset_1(standard, standd_colour);
    for(i = 0; i < HEIGHT; i++){
        for(j = 0; j < WIDTH; j ++){
            num_wrong += surf_colour[i][j] != standd_colour[i][j];
        }
    }
    return num_wrong;
    
}


void Save_Picture(SDL_Simplewin *sw, SDL_Rect *rect, SDL_Surface *sshot){
    SDL_RenderReadPixels(sw->renderer, rect, SDL_PIXELFORMAT_RGBA8888, sshot->pixels, sshot->pitch);
    invert_image(sshot->pitch, sshot->h , sshot->pixels);
    SDL_SaveBMP(sshot, "test.BMP");
}


void Srcect_Init(SDL_Rect *rect){
    rect->x = 0,
    rect->y = SCROFFESET;
    rect->h = HEIGHT;
    rect->w = WIDTH;
}





void Get_Colourset_1(SDL_Surface *image, int color_set[][WIDTH]){
    SDL_PixelFormat *fmt;
    Uint32 x, y;
    Uint32 p;
    Uint8 red, green, blue;
    fmt = image->format;
    for(y = 0; y < HEIGHT; y ++){
        for(x = 0; x < WIDTH; x++){
            p = GetPixel(image, x, y);
            SDL_GetRGB(p, fmt, &red, &green, &blue);
            /*
            red_l[y][x] = red;
            green_l[y][x] = green;
            blue_l[y][x] = blue;*/
            /*red = 0, green = 1, yellow = 2......*/
            color_set[y][x] = red/COLOURMAX * 1 + green / COLOURMAX* (1 << 1) + 
            blue / COLOURMAX * (1 << 2) - 1;
            color_set[y][x] = color_set[y][x] < 0 ? (COLOURNUM - 1) : color_set[y][x];
        }
    }
    

}

/*IMPORTANT DECLARE : These two are not my code. I get them
from internet. My original code can't deal with BMP. Because the 
saved picture is always upside down.  Also my original getpixel code
can't get RGB from BMP'*/
void invert_image(int pitch, int height, void* image_pixels)
{
   int index;
   void* temp_row;
   int height_div_2;

   temp_row = (void *)malloc(pitch);
   if(NULL == temp_row)
   {
      SDL_SetError("Not enough memory for image inversion\n");
   }

   height_div_2 = (int) (height * .5);
   for(index = 0; index < height_div_2; index++)    {

      memcpy((Uint8 *)temp_row,(Uint8 *)(image_pixels) +
         pitch * index,pitch);

      memcpy((Uint8 *)(image_pixels) + pitch * index, (Uint8 *)(image_pixels) +
         pitch * (height - index- 1), pitch);
      memcpy( (Uint8 *)(image_pixels) + pitch * (height - index - 1),
         temp_row, pitch);
   }
   free(temp_row);
} 

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
