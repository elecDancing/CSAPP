#define _GNU_SOURCE
#include "cachelab.h"
#include "stdlib.h"
#include <stdio.h>
#include "getopt.h"
#include <string.h>
#define addrLen 8
#include <stdbool.h> //In C program use bool need stdbool.h head file

static int S;
static int E;
static int B;
static int hits = 0;
static int misses = 0;
static int evictions = 0;
static int totalSet; //set number
bool verbose = false;

typedef struct _Node{
    unsigned tag;
    struct _Node* next;
    struct _Node* prev;
}Node;

typedef struct _LRU{
    Node* head;
    Node* tail;
    int* size;
}LRU;

static LRU* lru;

void initializeLRU(int i){
    lru[i].head = malloc(sizeof(struct _Node));
    lru[i].tail = malloc(sizeof(Node));

    lru[i].head->next = lru[i].tail;
    lru[i].tail->prev = lru[i].head;
    (lru[i].size)     = (int* )malloc(sizeof(int));
    *(lru[i].size)      = 0;
}

/**
 *
 * @param lru the lru we manupilate
 * @param pos the node position to be deleted usually 0 or 1
 *              0 means the first one
 *              1 means the last one
 */
void deleteElement(unsigned set, Node* nodeToDel, LRU* curLru){
    nodeToDel->next->prev = nodeToDel->prev;
    nodeToDel->prev->next = nodeToDel->next;
    *(curLru->size) = *(curLru->size) - 1;
}

void evict(unsigned set, LRU* curLru){
    deleteElement(set, curLru->tail->prev, curLru);
}

void addFirst(unsigned set, Node* node, LRU* curLru){
    node->next = curLru->head->next;
    node->prev = curLru->head;

    curLru->head->next->prev = node;
    curLru->head->next       = node;

    *(curLru->size) = *(curLru->size) + 1;
}

void print_help(){
    printf("How Dare You! please use google! Now!!!!\n");
}

void parseOption(int argc, char* const argv[], char** fileName){
    int option;
    while( (option = getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch (option) {
            case 'h':
                print_help();
                exit(0);
            case 'v':
                verbose = true; //set flag to output trace info
                break;
            break;
            case 's':
                S = atoi(optarg);
            case 'E':
                E = atoi(optarg);
            case 'b':
                B = atoi(optarg);
            case 't':
                strcpy(*fileName, optarg);
            // default:
            //     print_help();
            //     exit(-1);
        }
    }
    totalSet = 1 << S;
}

void update(unsigned address){
    unsigned mask = 0xFFFFFFFF;
    unsigned maskSet = mask >> (32 - S);
    unsigned targetSet = ((maskSet) & (address >> B));
    unsigned targetTag = address >> (S + B);

    LRU curLru = lru[targetSet];
    
    // to find if we have one
    Node* cur = curLru.head->next;
    int found = 0;
    while(cur != curLru.tail){ //search for the whole LinkedList
    //Can I use hash map to replace it?
        if(cur->tag == targetTag){
            found = 1;
	    break;
        }

        cur = cur->next;
    }

    
    if(found){ 
        hits++;
        deleteElement(targetSet, cur, &curLru);
        addFirst(targetSet, cur, &curLru);
        if(verbose)
	        printf("hit!   set -> %d \n", targetSet);
    }else{

        Node* newNode = malloc(sizeof(Node));
        newNode->tag = targetTag;
        if(*(curLru.size) == E){ // full, need to evict
            deleteElement(targetSet, curLru.tail->prev, &curLru);
            addFirst(targetSet, newNode, &curLru);

            evictions++;
            misses++;
            if(verbose)
	            printf("evic + miss    set -> %d\n", targetSet);
        }else{
            misses++;
            addFirst(targetSet, newNode, &curLru);
            if(verbose)
            printf("only miss    set  -> %d\n", targetSet);
        }    
    }
}




void cacheSimulateWhole(char* fileName){
    // step1: new lru with s sets
    lru = malloc(totalSet * sizeof(*lru));
    for(int i = 0; i < totalSet; i++)
        initializeLRU(i);

    FILE* file = fopen(fileName, "r");
    char op;
    unsigned address;
    int size;
    // L 10, 1
    while(fscanf(file, " %c %x,%d", &op, &address, &size) > 0){
        printf("%c, %x, %d\n", op, address, size);
        
	switch (op) {
            case 'L':
                update(address);
                break;
            case 'M':
                update(address);
            case 'S':
                update(address);
                break;
        }
    printf("-------------------------------------\n");
    }
 
}

int main(int argc, char* const argv[])
{
    char* fileName = malloc(100 * sizeof(char));

    // step1: parse option
    parseOption(argc, argv, &fileName);

    // step2: read all of the lines and analyze it
    cacheSimulateWhole(fileName);

    printSummary(hits, misses, evictions);
    return 0;
}