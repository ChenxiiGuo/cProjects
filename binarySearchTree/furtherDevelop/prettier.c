#include "mybst.h"
#include <math.h>
#define NUM 15
#define WORDS 15
#define STRSIZE 15
#define TWOSON 4
#define RIGHTSON 3
#define LEFTSON 2
#define NOSON 1

int mystrcmp(const void* a, const void* b);
int mychrcmp(const void* a, const void* b);
int myintcmp(const void* a, const void* b);
char* myprintstr(const void* v);
char* myprintchr(const void* v);
char* myprintint(const void* v);

void print_space(int num);
void print_dot(int num);
void print_link(int **lable, int line, int width, int widthofbiggest);


bst* get_string_tree(void);
bst* get_int_tree(void);
bst* get_char_tree(void);


void print_tree(bst *b);

/*print the tree*/
void print_tree_helper(bst* b, int **lable, int **seque, void *data,
                       int depth, int width, int widthofword);
/*store the data in an array*/
void bst_get_pre_order(bst* b, void* v);
/*This function is used to get the shape and the widdest */
void search_tree(bstnode *root, int**lable, int **sque, int depth, int begin,
                 int end, int *index);

void init_2_array(int**arr, int width, int depth);
/*find which word when printed is the widest one*/
void find_widest_word(bst* b, bstnode *root, int *width);
void free_2_array(int**arr, int depth);


int main(void){
    bst* int_tree = get_int_tree();
    bst* string_tree = get_string_tree();
    bst* char_tree = get_char_tree();
    printf("Test of int type is :\n");
    print_tree(int_tree);
    printf("\n\n");
    printf("Test of string type is :\n");
    print_tree(string_tree);
    printf("\n\n");
    printf("Test of char type is :\n");
    print_tree(char_tree);
    bst_free(&int_tree);
    bst_free(&string_tree);
    bst_free(&char_tree);
    return 0;
    
}

void print_tree(bst *b){
    /*index is used to remeber the each element's access sequence*/
    int index = 0;
    int depth = bst_maxdepth(b);
    int width = pow(2, depth) - 1;
    /*lable is used to remeber every node's children status*/
    int **lable = (int**)calloc(depth, sizeof(int*));
    /*sque is used to remeber the each element's access sequence*/
    int **seque = (int**)calloc(depth, sizeof(int*));
    
    int widthofword = 0;
    int size = bst_size(b);
    /*data is used to store each element's data(preorder sequence)*/
    void *data = (void*)calloc(size, b->elsz);
    
    if(b == NULL){
        ON_ERROR("There is no tree!\n");
    }
    
    init_2_array(lable, width, depth);
    init_2_array(seque, width, depth);
    
    bst_get_pre_order(b, data);
    
    find_widest_word(b, b->top, &widthofword);
    
    search_tree(b->top, lable, seque, 0, 0, width - 1, &index);
    
    print_tree_helper(b, lable, seque, data, depth, width, widthofword);
    /*Clear*/
    free_2_array(lable, depth);
    free_2_array(seque, depth);
    free(data);
    data = NULL;
    free(lable);
    lable = NULL;
    free(seque);
    seque = NULL;
}

void print_tree_helper(bst* b, int **lable, int **seque, void *data,
                       int depth, int width, int widthofword){
    int i, j, k;
    char *str = (char*)calloc(1, widthofword);
    char *anchor = str;
    for(i = 0; i < depth; i++){
        for(j = 0; j < width; j++){
            if(lable[i][j] > 0){
                k = sprintf(str, "%s", b->prntnode((char*)data + (b->elsz) * seque[i][j]));
                printf("%s", str);
                print_space(widthofword - k);
            }
            else{
                print_space(widthofword);
            }
        }
        printf("\n");
        if(i != depth - 1){
            print_link(lable, i, width, widthofword);
            printf("\n");
        }
        memset(anchor, 0, widthofword);
    }
    free(anchor);
    anchor = NULL;
}

/*I must admit that in this part, my method may make people feel confused*/
void print_link(int **lable, int line, int width, int widthofbiggest){
    int i = 0;
    int flag_right = 0;
    int flag_left = 0;
    /*char *str = (char*)calloc(width, widthofbiggest);*/
    while(i < width){
    /*check each lable[i][j]'s content.*/
        if(lable[line][i] > 0){
    /*If a node doesn't have son, V should not be printed*/
            if(lable[line][i] != NOSON){
                printf("V");
            }
            else{
                printf(" ");
            }
            flag_left = false;
            print_dot(widthofbiggest - 1);
            flag_right = lable[line][i] >= LEFTSON ? true : false;
        }
        else if(lable[line + 1][i] > 0){
            printf("V");
            if(flag_right == false){
                flag_left = true;
            }
            if(flag_left){
                print_dot(widthofbiggest - 1);
            }
            else{
                print_space(widthofbiggest - 1);
            }
            flag_right = false;
            
        }
        else if(flag_right || flag_left){
            print_dot(widthofbiggest);
        }
        else{
            print_space(widthofbiggest);
        }
        i++;
    }
}





void search_tree(bstnode *root, int**lable, int **seque, int depth, int begin,
                 int end, int *index){
    if(root == NULL){
        return;
    }
    
    /*lable array stores the root's children's condition*/
    if(root->left && root->right){
        lable[depth][(begin + end) / 2] = TWOSON;
    }
    else if(root->right) {
        lable[depth][(begin + end) / 2] = RIGHTSON;
    }
    else if(root->left) {
        lable[depth][(begin + end) / 2] = LEFTSON;
    }
    else{
        lable[depth][(begin + end) / 2] = NOSON;
    }
    seque[depth][(begin + end) / 2] = (*index);
    (*index)++;
    depth ++;
    search_tree(root->left, lable, seque, depth, begin, (begin + end) / 2 - 1, index);
    search_tree(root->left, lable, seque, depth, (begin + end) / 2 + 1, end, index);
}


void find_widest_word(bst* b, bstnode *root, int *width){
    int length;
    if(root == NULL){
        return;
    }
    length = strlen(b->prntnode(root->data)) + 1;
    *width = length > *width ? length : *width;
    find_widest_word(b, root->left, width);
    find_widest_word(b, root->right, width);
}




void init_2_array(int**arr, int width, int depth){
    int i;
    for(i = 0; i < depth; i++){
        arr[i] = (int*)calloc(width, sizeof(int));
    }
}




bst* get_string_tree(void){
    bst *b = bst_init(STRSIZE, mystrcmp, myprintstr);
    bst *rb;
    char words1[WORDS][STRSIZE] = {"apple", "bag", "card", "dental", "electronic", "favourite", "government",  "hope", "intelligent", "jog", "kilometer", "love", "man", "next", "option"};
    bst_insertarray(b, words1, WORDS);
    rb = bst_rebalance(b);
    bst_free(&b);
    return rb;
}

bst* get_int_tree(void){
    int i;
    bst* b = bst_init(sizeof(int), myintcmp, myprintint);
    bst* rb;
    for(i = 0; i < NUM; i++){
        bst_insert(b, &i);
    }
    rb = bst_rebalance(b);
    bst_free(&b);
    return rb;
}

bst* get_char_tree(void){
    bst *b = bst_init(sizeof(char), mychrcmp, myprintchr);
    bst *rb;
    char str[WORDS] = "ABCDEFGHIJKLMNO";
    bst_insertarray(b, str, WORDS);
    rb = bst_rebalance(b);
    bst_free(&b);
    return rb;
}





void bst_get_pre_ordered_helper(bst* b,bstnode* root, void* v, int *pnum){
    if(!root){
        return;
    }
    memcpy((char*)v + *pnum, root->data, b->elsz);
    *pnum += b->elsz;
    bst_get_pre_ordered_helper(b, root->left, v, pnum);
    bst_get_pre_ordered_helper(b, root->right, v, pnum);
}

void bst_get_pre_order(bst* b, void* v){
    int i = 0;
    if(!b || !b->top){
        return;
    }
    bst_get_pre_ordered_helper(b, b->top, v, &i);
}

void print_space(int num){
    while(num--){
        printf(" ");
    }
}


void print_dot(int num){
    while(num--){
        printf(".");
    }
}

void free_2_array(int**arr, int depth){
    int i;
    for(i = 0; i < depth; i++){
        free(arr[i]);
        arr[i] = NULL;
    }
}



char* myprintstr(const void* v)
{
    return (char*)v;
}

char* myprintchr(const void* v)
{
    static char str[100];
    sprintf(str, "%c", *(char*)v);
    return str;
}

char* myprintint(const void* v)
{
    static char str[100];
    sprintf(str, "%d", *(int*)v);
    return str;
}

int mystrcmp(const void* a, const void* b)
{
    return strcmp(a, b);
}

int mychrcmp(const void* a, const void* b)
{
    return *(char*)a - *(char*)b;
}

int myintcmp(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}
