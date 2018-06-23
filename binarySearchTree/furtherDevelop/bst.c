#include "mybst.h"
#include <assert.h>
#define BRACE 3
#define STRSIZE 30
#define WORDS 23


int max(int a, int b);
void bst_insert_helper(bstnode* root, void* v, bst* b);
int bst_size_helper(bstnode* root);
int bst_maxdepth_helper(bstnode* root);
bool bst_isin_helper(bstnode* root, void* v, bst* b);
void bst_free_helper(bstnode *root);
void bst_getordered_helper(bst* b,bstnode* root, void* v, int* pnum);

bst* bst_init(int sz,
              int(*comp)(const void* a, const void* b),
              char*(*prnt)(const void* a)  ){
    bst *binary_search_tree = (bst*)calloc(1, sizeof(bst));
    binary_search_tree->top = NULL;
    binary_search_tree->elsz = sz;
    binary_search_tree->compare = comp;
    binary_search_tree->prntnode = prnt;
    return binary_search_tree;
}


void bst_insert(bst* b, void* v){
    bstnode *root = b->top;
    if (!b->top) {
        b->top = bstnode_init(v, b->elsz);
        return;
    }
    bst_insert_helper(root, v, b);
}
void bst_insert_helper(bstnode* root, void* v, bst* b){
    if (b->compare(root->data, v) == 0){
        return;
    }
    if (b->compare(root->data, v) < 0) {
        if(!root->right){
            root->right = bstnode_init(v, b->elsz);
            return;
        }
        bst_insert_helper(root->right, v, b);
    }
    else {
        if(!root->left){
            root->left = bstnode_init(v, b->elsz);
            return;
        }
        bst_insert_helper(root->left, v, b);
    }

}


int bst_size(bst* b){
    if(b->top == NULL){
        return 0;
    }
    return bst_size_helper(b->top);
}

int bst_size_helper(bstnode* root){
    if(!root){
        return 0;
    }
    return 1 + bst_size_helper(root->left) + bst_size_helper(root->right);
}

int bst_maxdepth(bst *b){
    if(!b->top){
        return 0;
    }
    return bst_maxdepth_helper(b->top);
}

int bst_maxdepth_helper(bstnode* root){
    if(!root){
        return 0;
    }
    return 1+max(bst_maxdepth_helper(root->left), bst_maxdepth_helper(root->right));
}


bool bst_isin(bst* b, void* v){
    if(!b->top){
        return false;
    }
    return bst_isin_helper(b->top, v, b);
}

bool bst_isin_helper(bstnode* root, void* v, bst* b){
    if(!root){
        return false;
    }
    if(b->compare(root->data, v) == 0){
        return true;
    }
    if(b->compare(root->data, v) < 0){
        return bst_isin_helper(root->right, v, b);
    }
    else{
        return bst_isin_helper(root->left, v, b);
    }
}

void bst_insertarray(bst* b, void* v, int n){
    int i;
    for(i = 0; i < n; i++){
        bst_insert(b, (char *)v + i * b->elsz);
    }
}

void bst_free(bst** p){
    bst *a;
    if(p == NULL||*p == NULL){
        return;
    }
    a = *p;

    bst_free_helper(a->top);
    free(a);
    *p = NULL;
}

void bst_free_helper(bstnode *root){
    if(!root){
        return;
    }
    bst_free_helper(root->left);
    bst_free_helper(root->right);
    
    free(root->data);
    root->data = NULL;
    root->left = NULL;
    root->right = NULL;
    free(root);
    root = NULL;
}

char* bst_print_helper(bst *b, bstnode *root) {
    char *str, *str_left, *str_right;
    int length_left, length_right, length_data;
    
    if(root == NULL) {
        str = (char*)calloc(1, sizeof(char));
        return str;
    }
    
    str_left = bst_print_helper(b, root->left);
    str_right = bst_print_helper(b, root->right);

    length_left = strlen(str_left);
    length_right = strlen(str_right);
    length_data = strlen(b->prntnode(root->data));
    /*BRACE is 3, because two braces and one \0*/
    str = (char*)calloc((length_data + length_left + length_right + BRACE), sizeof(char));
    sprintf(str, "(%s%s%s)", b->prntnode(root->data), str_left, str_right);
    
    free(str_left);
    free(str_right);

    return str;
}

char* bst_print(bst *b) {
    char *str = NULL;
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    str = bst_print_helper(b, b->top);
    return str;
}




void bst_getordered(bst* b, void* v){
    int i = 0;
    if(!b || !b->top){
        return;
    }
    bst_getordered_helper(b, b->top, v, &i);
}
void bst_getordered_helper(bst* b,bstnode* root, void* v, int *pnum){
    if(!root){
        return;
    }
    bst_getordered_helper(b, root->left, v, pnum);
    memcpy((char*)v + *pnum, root->data, b->elsz);
    *pnum += b->elsz;
    bst_getordered_helper(b, root->right, v, pnum);
}

/*Two way to realize the rebalance function.
 First one doesn't use insert function, but it is a faster,
 when using 20480 numbers. The first one spends 0.0029s, the second one use
 0.0042s, maybe because the the complexity for first one is O(n), for the second
 one is n(log(n)). The time complexity for bst_insert is O(log(n))*/
bst* bst_rebalance(bst* b){
    bst *res = bst_init(b->elsz, b->compare, b->prntnode);
    int size = bst_size(b);
    void *arr = (void*)calloc(size, b->elsz);
    int i = 0, j = size - 1;
    bst_getordered(b, arr);
    res->top = get_balance_tree(b, arr, i, j);
    free(arr);
    arr = NULL;
    return res;
}

bstnode* get_balance_tree(bst* b, void *arr, int start, int end){
    bstnode *root = NULL;
    if(start > end){
        return root;
    }
    root = bstnode_init((char*)arr + (start + end) / 2 * (b->elsz), b->elsz);
    root->left = get_balance_tree(b, arr, start, (start + end) / 2 - 1);
    root->right = get_balance_tree(b, arr, (start + end) / 2 + 1, end);
    return root;
}
/*
void bst_rebalance_help(bst* res, void* arr, int start, int end){
    if(start > end){
        return;
    }
    bst_insert(res, (char*)arr + (start + end) / 2 * (res->elsz));
    bst_rebalance_help(res, arr, start, (start + end) / 2 - 1);
    bst_rebalance_help(res, arr, (start + end)/2 + 1, end);
    
}

bst* bst_rebalance(bst* b){
    bst *res = bst_init(b->elsz, b->compare, b->prntnode);
    int size = bst_size(b);
    void *arr = (void*)calloc(size, b->elsz);
    int i = 0, j = size - 1;
    bst_getordered(b, arr);
    bst_rebalance_help(res, arr, i, j);
    free(arr);
    return res;
}*/





bstnode* bstnode_init(void *pdata, int data_size){
    bstnode* newnode = (bstnode*)calloc(1, sizeof(bstnode));
    if(pdata == NULL){
        newnode->data = NULL;
    }
    else{
        newnode->data = (void*)calloc(1, data_size);
        memcpy(newnode->data, pdata, data_size);
    }
    newnode->left = NULL;
    newnode->right = NULL;
    return newnode;
}

int max(int a, int b){
    return a > b ? a : b;
}

void arr_init(int *arr, int size){
    int i;
    for(i = 0; i < size; i++) {
        arr[i] = i;
    }
}















