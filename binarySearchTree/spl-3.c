#include "bst.h"

#define CSEC (double)(CLOCKS_PER_SEC)
#define ARGVSIZE 3
#define STRSIZE 30

int mystrcmp(const void* a, const void* b);
char* myprintstr(const void* v);
void get_tree_from_file(char name[], bst* b);
void get_misspelt_tree(char name[], bst* dic, bst* mis);

int main(int argc, char *argv[]) {
    bst* dictionary = bst_init(STRSIZE, mystrcmp, myprintstr);
    bst* mispell = bst_init(STRSIZE, mystrcmp, myprintstr);
    
    
    if(argc != ARGVSIZE) {
        ON_ERROR("The arguments is wrong!\n");
    }
    
    get_tree_from_file(argv[1], dictionary);
    get_misspelt_tree(argv[2], dictionary, mispell);
    

    bst_free(&dictionary);
    bst_free(&mispell);
    
    return 0;
}

void get_tree_from_file(char name[], bst *b) {
    FILE *fp;
    char str[STRSIZE];
    fp = fopen(name, "r");
    if(!fp) {
        ON_ERROR("Can't open the file!\n");
    }
    while(fscanf(fp, "%s", str) != EOF) {
        bst_insert(b, str);
    }
    fclose(fp);
}

void get_misspelt_tree(char name[],bst* dic, bst* mis){
    FILE *fp;
    char str[STRSIZE];
    fp = fopen(name, "r");
    if(!fp) {
        ON_ERROR("Can't open the file!\n");
    }
    while(fscanf(fp, "%s", str) != EOF) {
        if(bst_isin(dic, str) == false && bst_isin(mis, str) == false){
            printf("%s\n", str);
            bst_insert(mis, str);
        }
    }
    fclose(fp);
}

char* myprintstr(const void* v)
{
    return (char*)v;
}

int mystrcmp(const void* a, const void* b)
{
    return strcmp(a, b);
}





