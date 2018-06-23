#include "mybst.h"
#define CSEC (double)(CLOCKS_PER_SEC)

#define NUM 500


/*using threaded tree to search the tree.
 Morris and Joseph M invented this method */
void bst_getordered_morris(bst* b, void *v);

char* myprintint(const void* v);
int myintcmp(const void* a, const void* b);

int main(void){
    clock_t c1, c2, c3;
    bst *b = bst_init(sizeof(int), myintcmp, myprintint);
    int *arr = (int*)calloc(NUM, sizeof(int));
    int *arr_morris = (int*)calloc(NUM, sizeof(int));
    int *arr_common = (int*)calloc(NUM, sizeof(int));
    arr_init(arr, NUM);
    b->top = get_balance_tree(b, arr, 0, NUM - 1);
    c1 = clock();
    bst_getordered_morris(b, arr_morris);
    c2 = clock();
    bst_getordered(b, arr_common);
    c3 = clock();
    /*In my computer, Morris takes 0.000968s and recursion takes 0.000463s*/
    /*The reason I guess is that Morris's method needs the pre-pointer to
     go through the whole tree and the current pointer also needs to go through
     the tree. But when using recursion, we have a inexplicit "stack" which needs a lot
     of memory. So the space complexity for Morris' method is O(1), and the space 
     complexity for recursion is O(n)*/
    printf("Morris' method takes %fs\n",(double)(c2 - c1)/CSEC);
    printf("Recursion takes %fs\n",(double)(c3 - c2)/CSEC);

    free(arr_common);
    arr_common = NULL;
    free(arr);
    arr = NULL;
    free(arr_morris);
    arr_morris = NULL;
    bst_free(&b);
    return 0;
    
    
}



void bst_getordered_morris(bst* b, void* v){
    bstnode *root = b->top, *pre = NULL;
    int i = 0;
    int *p = &i;
    while(root) {
        if(root->left == NULL) {
            memcpy((char*)v + *p, root->data, b->elsz);
            root = root->right;
        }
        else{
            pre = root->left;
            while(pre->right && pre->right != root){
                pre = pre->right;
            }
            if(pre->right == NULL){
                /*this step is important will help to find the root from leaves*/
                /*this will change the structure of this tree*/
                pre->right = root;
                root = root->left;
            }
            else{
                /*recover the structure of this tree*/
                pre->right = NULL;
                memcpy((char*)v + *p, root->data, b->elsz);
                root = root->right;
            }
        
        }
    }
    
}




char* myprintint(const void* v)
{
    static char str[100];
    sprintf(str, "%d", *(int*)v);
    return str;
}


int myintcmp(const void* a, const void* b)
{
    return *(int*)a - *(int*)b;
}
