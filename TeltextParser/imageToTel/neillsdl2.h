#include <SDL2/SDL.h>
#include <math.h>


#define WWIDTH 800
#define WHEIGHT 600

/* Font stuff */
typedef unsigned short fntrow;
#define FNTWIDTH (sizeof(fntrow)*8)
#define FNTHEIGHT 18
#define FNTCHARS 96
#define FNT1STCHAR 32

#define SDL_8BITCOLOUR 256

#define R 0
#define G 1
#define B 2
#define RGB 3
#define COLOURMAX 255
#define COLOURMIN 0


#define TRUE 1
#define FALSE 0

/*offset to ascii code*/
#define OFFEST 32

#define SPACE 32

#define COLOURNUM 8

#define CODENUM 1000

#define DOUBLEHEIGHT 2
#define SINGLEHEIGHT 1

#define ARGVSIZE 2
#define CODENUM 1000
#define LINEWIDTH 40

#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif

#define WIDTH 640
#define HEIGHT 450

#define HASHSIZE FNTCHARS * 3
#define CODEW 40
#define CODEH 25

#define PLINE 16
#define COLOURNUM 8
#define GRAPHBIT 6
/*the number of small square in vertical direction is 3*/
#define GRAPHVNUM 3
#define GRAPHHNUM 2

#define CODESTART 0x80
#define DATASTART 0xa0
#define BLASTSTART 0xc0
#define BLASTEND 0xdf

#define WSSQUARE 8
#define HSSQUARE 6
#define NUMSV 3


enum Colour{red = 0, green, yellow, blue, magenta, cyan, white, black};
typedef enum Colour Colour;

enum Character_color_code{redchcode = 0x81, greenchcode, yellowchcode, bluechcode, megentachcode,
cyanchcode, whitechcode};

enum Height_code{singlehcode = 0x8c, doublehcode};

enum Graph_Colour_code{redgracode = 0x91, greengracode, yellowgracode, bluegracode, megentagracode,
cyangracode, whitegracode};

enum Seperate_Code{nosepcode = 0x99, sepcode};

enum mix_Code{blackbackcode = 0x9c, newbackcode, holdcode, releasecode};

/* All info required for windows / renderer & event loop */
struct SDL_Simplewin {
   SDL_bool finished;
   SDL_Window *win;
   SDL_Renderer *renderer;
   SDL_Texture *display;
};
typedef struct SDL_Simplewin SDL_Simplewin;

struct Instruction {
    Colour backcolour;
    Colour forecolour;
    int counter;
    char data;
    int fonthigh;
    int islowpart;
    int ischar;
    int isseperate;
    int ishold;
    
    int backcolchng;
    int backblackchng;
    int forecolchng_char;
    int heightchng;
    int graphchng;
    int seperatechng;
    int holdchng;
};
typedef struct Instruction Instruction;


void Neill_SDL_Init(SDL_Simplewin *sw);
void Neill_SDL_Events(SDL_Simplewin *sw);
void Neill_SDL_SetDrawColour(SDL_Simplewin *sw, Uint8 r, Uint8 g, Uint8 b);
void Neill_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], unsigned char chr, int ox, int oy);
void Neill_SDL_ReadFont(fntrow fontdata[FNTCHARS][FNTHEIGHT], char *fname);
void Neill_SDL_UpdateScreen(SDL_Simplewin *sw);

void Instruction_Init(Instruction *instruct);
void ReadFile(char name[], unsigned char code[]);
void RenewInstruction(Instruction *instruct);
void Get_Colour(Colour colour, int *cred, int *cgreen, int *cblue);
int IsForBackColour(unsigned char code[], int currentCounter);
void Decoder(Instruction *instruct, unsigned char code[]);


void My_SDL_Draw(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], 
Instruction *instruct);
void My_SDL_DrawChar(SDL_Simplewin *sw, fntrow fontdata[FNTCHARS][FNTHEIGHT], Instruction *instruct);
void My_SDL_DrawGraph(SDL_Simplewin *sw, Instruction *instruct);
void Draw_Square(SDL_Simplewin *sw,Instruction *instruct, int ox, int oy);
void DrawBackground(SDL_Simplewin *sw, Instruction *instruct, int ox, int oy);


/**************************************************************/
/*extension***extension***extension***extension***extension***/
/**************************************************************/
void Initialize_Code(int code[CODEH][CODEW]);

void Get_Colourset(SDL_Surface *image, int red_l[][WIDTH], int green_l[][WIDTH], int blue_l[][WIDTH], int color_set[][WIDTH]);

void Write_IntoFile(char name[], int code_set[][CODEW]);

void Write_Code(Instruction *instruct, int code_set[][CODEW]);

void Put_Controlcode_Spaceslot(int code_set[][CODEW], int code, int x, int y);

void Get_InsFromGraph(Instruction *instruct, int font_set_hash[], int color_set[HEIGHT][WIDTH]);

int Get_data(Instruction *instruct, int ox, int oy, int color_set[][WIDTH], int font_set_hash[]);

int Check_Holding(Instruction *instruct, int ox, int oy, int color_set[][WIDTH]);

int Control_CodeForHold(Instruction *instruct, int first_colour, int second_colour);

int Check_If_Char(int hash_value, int font_set_hash[], int *i);

int Get_Hash_Value(int ox, int oy, int color_set[][WIDTH], int *second_colour);

void Check_BackColourChange(Instruction *instruct, int first_colour);

int Check_If_Contain_Colour1(int color_set[][WIDTH], int colour1, int ox, int oy);

int Get_Data_From_Graph(Instruction *instruct, int ox, int oy, int color_set[][WIDTH],
    int colour_1, int colour_2);
    
void Check_HeightChange(Instruction *instruct, int order);

void Check_CharColourChange(Instruction *instruct, int forecl);


Uint32 GetPixel(SDL_Surface *surface, int x, int y);

void PrintCode(int code_set[][CODEW]);

void Get_FontHash(fntrow fontdata[FNTCHARS][FNTHEIGHT], int font_set_hash[HASHSIZE]);



