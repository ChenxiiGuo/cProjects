
#include "mybst.h"
#define CSEC (double)(CLOCKS_PER_SEC)
#define STRSIZE 30
#define PRIME 619207
#define REHASHSTEP 7
#define BSTTIME 0.5
int BKDRhash(char *str);
void get_hashtable(char (*words)[STRSIZE], char filename[]);
void get_wrong_words(char filename[], char (*words_dic)[STRSIZE],char (*words_check)[STRSIZE]);
void put_into_hash(char (*words)[STRSIZE],char str[]);
int check_hashtable(char (*words)[STRSIZE], char str[]);
void free_2_D_arry(char **, int row);

int main(){
    
    clock_t c1, c2, c3;
    /*Although words_dic[][] is more convenient. Since there are so many
     words, the stack is not big enough, I could only use the heap. just 
     calloc or malloc the memory*/
    char (*words_dic)[STRSIZE] = (char(*)[STRSIZE])calloc(PRIME, STRSIZE);
    char (*words_check)[STRSIZE] = (char(*)[STRSIZE])calloc(PRIME, STRSIZE);
    char file[2][STRSIZE] ={"eng_370k_shuffle.txt", "heart_darkness.txt"};
    double length_1, length_2;
    
    c1 = clock();
    get_hashtable(words_dic, file[0]);
    c2 = clock();
    get_wrong_words(file[1], words_dic, words_check);
    c3 = clock();
    length_1 = (c3 - c2)/CSEC;
    length_2 = (c3 - c1)/CSEC;
    printf("The searching time for hashing is %f. \n",length_1);
    printf("This is %d times quicker than BST.\n",(int)(BSTTIME/length_1));
    printf("If including the building time for hashtable,\n");
    printf("then the total time for hashing is %f.\n",length_2);
    printf("This is %d times quicker than BST.\n",(int)(BSTTIME/length_2));

    free(words_dic);
    free(words_check);

    return 0;

}



void get_wrong_words(char filename[],char (*words_dic)[STRSIZE],char (*words_check)[STRSIZE]){
    FILE *fp;
    char str[STRSIZE];
    fp = fopen(filename,"r");
    if(!fp){
        ON_ERROR("Can't open the file!\n");
    }
    while(fscanf(fp, "%s", str) != EOF){
        if(check_hashtable(words_dic, str) == 0){
            if(check_hashtable(words_check, str) == 0){
                put_into_hash(words_check, str);
                printf("%s\n", str);
            }
        }
    }
    
}
int check_hashtable(char (*words)[STRSIZE], char str[]){
    int hash_value = BKDRhash(str);
    int new_hash_value;
    while(words[hash_value][0]){
        if(strcmp(words[hash_value], str) == 0){
            return 1;
        }
        new_hash_value = hash_value + REHASHSTEP;
        hash_value = new_hash_value % PRIME;
        
    }
    return 0;
    
}


void put_into_hash(char (*words)[STRSIZE], char str[]){
    int hash_value = BKDRhash(str);
    int new_hash_value;
    if(words[hash_value]){
        strcpy(words[hash_value], str);
    }
    else{
        while(words[hash_value][0] && strcmp(words[hash_value], str) != 0){
            new_hash_value = hash_value + REHASHSTEP;
            hash_value = new_hash_value % PRIME;
        }
        strcpy(words[hash_value], str);
    }
    
}



int BKDRhash(char *str){
    unsigned int seed = 131;
    unsigned int hash = 0;
    while (*str)
    {
        hash = hash * seed + (*str++);
    }
    return (hash & 0x7FFFFFFF) % PRIME;

}

void get_hashtable(char (*words)[STRSIZE], char filename[]){
    FILE *fp;
    char str[STRSIZE];
    int total = 0;
    int hash_value = 0;
    int new_hash_value;
    fp = fopen(filename,"r");
    if(!fp){
        ON_ERROR("Can't open the file!\n");
    }
    while(fscanf(fp, "%s", str) != EOF){
        total++;
        hash_value = BKDRhash(str);
        if(words[hash_value] == 0){
            strcpy(words[hash_value], str);
        }
        else{
            while(words[hash_value][0] && strcmp(words[hash_value], str) != 0){
                new_hash_value = hash_value + REHASHSTEP;
                hash_value = new_hash_value % PRIME;
            }
            strcpy(words[hash_value], str);
        }
    }
    fclose(fp);
}

