
#include "bst.h"
#define BRACE 3
#define MID 2

/*This function could be used to initialize the node*/
bstnode* bstnode_init(void *pdata, int data_size) {
    bstnode* newnode = (bstnode*)calloc(1, sizeof(bstnode));
    if(pdata == NULL) {
        newnode->data = NULL;
    }
    else {
        newnode->data = (void*)calloc(1, data_size);
        memcpy(newnode->data, pdata, data_size);
    }
    newnode->left = NULL;
    newnode->right = NULL;
    return newnode;
}



bst* bst_init(int sz,
              int(*comp)(const void* a, const void* b),
              char*(*prnt)(const void* a)  ) {
    bst *binary_search_tree = (bst*)calloc(1, sizeof(bst));
    binary_search_tree->top = NULL;
    binary_search_tree->elsz = sz;
    binary_search_tree->compare = comp;
    binary_search_tree->prntnode = prnt;
    return binary_search_tree;
}



int max(int a, int b) {
    return a > b ? a : b;
}



/*bst_insert_helper only belongs to bst_insert, so the argument
 root will never be NULL*/
void bst_insert_helper(bstnode* root, void* v, bst* b) {
    if (b->compare(root->data, v) == 0) {
        return;
    }
    if (b->compare(root->data, v) < 0) {
        if(root->right == NULL){
            root->right = bstnode_init(v, b->elsz);
            return;
        }
        bst_insert_helper(root->right, v, b);
    }
    else {
        if(root->left == NULL) {
            root->left = bstnode_init(v, b->elsz);
            return;
        }
        bst_insert_helper(root->left, v, b);
    }    
}

void bst_insert(bst* b, void* v) {
    if(b == NULL) {
        ON_ERROR("Can't insert into a NULL pointer!\n");
    }
    if (b->top == NULL) {
        b->top = bstnode_init(v, b->elsz);
        return;
    }
    bst_insert_helper(b->top, v, b);
}



int bst_size_helper(bstnode* root) {
    if(root == NULL) {
        return 0;
    }
    return 1 + bst_size_helper(root->left) + bst_size_helper(root->right);
}

int bst_size(bst* b) {
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    return bst_size_helper(b->top);
}



int bst_maxdepth_helper(bstnode* root) {
    if(!root) {
        return 0;
    }
    return 1+max(bst_maxdepth_helper(root->left), bst_maxdepth_helper(root->right));
}

int bst_maxdepth(bst *b) {
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    return bst_maxdepth_helper(b->top);
}



bool bst_isin_helper(bstnode* root, void* v, bst* b) { 
    if(root == NULL) {
        return false;
    }
    /*If found return true*/
    if(b->compare(root->data, v) == 0) {
        return true;
    }
    if(b->compare(root->data, v) < 0){
        return bst_isin_helper(root->right, v, b);
    }
    else {
        return bst_isin_helper(root->left, v, b);
    }
}

bool bst_isin(bst* b, void* v) {
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    return bst_isin_helper(b->top, v, b);
}




void bst_insertarray(bst* b, void* v, int n) {
    int i;
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    for(i = 0; i < n; i++) {
         /*forcely change the type of void pointer, so the pointer could be added.
         The reason choosing char pointer is that it is the "shortest pointer".
         I mean that if it is added 1, the step is shortest*/
        bst_insert(b, (char *)v + i * b->elsz);
    }
}



void bst_free_helper(bstnode **root) {
    bstnode *a;
    
    if(root == NULL || *root == NULL) {
        return;
    }
    a = *root;
    
    bst_free_helper(&a->left);
    bst_free_helper(&a->right);
    
    free(a->data);
    a->data = NULL;
    a->left = NULL;
    a->right = NULL;
    free(a);
    *root = NULL;
}

void bst_free(bst** p) {
    bst *a;
    if(p == NULL||*p == NULL) {
        return;
    }
    a = *p;
    bst_free_helper(&(a->top));
    free(a);
    a = NULL;
    *p = NULL;
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



/*inorder traversal should be used*/
void bst_getordered_helper(bst* b,bstnode* root, void* v, int *pnum) {
    if(root == NULL) {
        return;
    }
    bst_getordered_helper(b, root->left, v, pnum);
    memcpy((char*)v + *pnum, root->data, b->elsz);
    *pnum += b->elsz;
    bst_getordered_helper(b, root->right, v, pnum);
}


void bst_getordered(bst* b, void* v) {
    int i = 0;
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    bst_getordered_helper(b, b->top, v, &i);
}





/*There are two methods to realize the rebalance function.
 First one doesn't use insert function, but it is a faster,
 when using 20480 numbers. The first one spends 0.0029s, the second one use
 0.0042s, maybe because the the complexity for first one is O(n), for the second
 one is n(log(n)). The time complexity for bst_insert is O(log(n)).
 Personally, I prefer the first one.*/

bstnode* bst_rebalance_helper(bst* b, void *arr, int start, int end) {
    bstnode *root = NULL;
    if(start > end) {
        return root;
    }
    root = bstnode_init((char*)arr + (start + end) / MID * (b->elsz), b->elsz);
    root->left = bst_rebalance_helper(b, arr, start, (start + end) / MID - 1);
    root->right = bst_rebalance_helper(b, arr, (start + end) / MID + 1, end);
    return root;
}

bst* bst_rebalance(bst* b) {
    bst *res = bst_init(b->elsz, b->compare, b->prntnode);
    int size = bst_size(b);
    void *arr = (void*)calloc(size, b->elsz);
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    bst_getordered(b, arr);
    res->top = bst_rebalance_helper(b, arr, 0, size - 1);
    free(arr);
    arr = NULL;
    return res;
}

/*
void bst_rebalance_help(bst* res, void* arr, int start, int end) {
    if(start > end) {
        return;
    }
    bst_insert(res, (char*)arr + (start + end) / 2 * (res->elsz));
    bst_rebalance_help(res, arr, start, (start + end) / 2 - 1);
    bst_rebalance_help(res, arr, (start + end) / 2 + 1, end);   
}

bst* bst_rebalance(bst* b) {
    bst *res = bst_init(b->elsz, b->compare, b->prntnode);
    int size = bst_size(b);
    void *arr = (void*)calloc(size, b->elsz);
    int i = 0, j = size - 1;
    if(b == NULL) {
        ON_ERROR("There is no tree!\n");
    }
    bst_getordered(b, arr);
    bst_rebalance_help(res, arr, i, j);
    free(arr);
    return res;
}
*/




















