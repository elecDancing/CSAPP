/*
 * @Descripttion: 
 * @version: 
 * @Author: xp.Zhang
 * @Date: 2023-01-17 00:00:11
 * @LastEditors: xp.Zhang
 * @LastEditTime: 2024-02-04 22:02:28
 */
#define _GNU_SOURCE
#include "stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "cachelab.h"
#include "getopt.h"
#define addrLen 8

//need S E B
static int s = -1;
static int E = -1;
static int b = -1;

//need record hits, misses, evictions frequency
static int hits = 0;
static int misses = 0;
static int evictions = 0;

//need record Setnumber, usual S = 2^s
static int totalSet = -1;

//flag to output trace info detailed
bool verbose = false;

//dataStruct of LRU doubleD LinkedList
//!typedef (strust _Node) Alias (Node)
typedef struct _Node{
    unsigned tag;
    struct _Node* next;
    struct _Node* prev;   
}Node;
typedef struct _LRU{
    Node* head; //!head and tail dont store data
    Node* tail;
    int* size;
}LRU;

// LRY to be used
// every set has a lru 
//e.g.   set0  ->    lru[0];
static LRU* lru;

// init lru
void initializeLRU(int i){
    lru[i].head = malloc(sizeof(Node));
    lru[i].tail = malloc(sizeof(Node));

    lru[i].head->next = lru[i].tail;
    lru[i].tail->prev = lru[i].head;
    (lru[i].size)     = (int* )malloc(sizeof(int));
    *(lru[i].size)    = 0;
}

// LRU process function
void deleteElement(unsigned set, Node* nodeToDel, LRU* curLru){
    nodeToDel->next->prev = nodeToDel->prev;
    nodeToDel->prev->next = nodeToDel->next;
    //free(nodeToDel); 
    // cant free space beacuse we give Lru a space, if we free it ,the space of lru decrease;
    *curLru->size = *curLru->size - 1;
}

void evict(unsigned set, LRU* curLru){
    deleteElement(set, curLru->tail->prev, curLru);
}

void insert(unsigned set, Node* node, LRU* curLru){
    node->next = curLru->head->next;
    node->prev = curLru->head;

    curLru->head->next->prev = node;
    curLru->head->next       = node;

    *curLru->size = *curLru->size + 1;
}

void print_help(){
    printf("\nUsage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n\n");
    printf("For more help, please visit : \n");
    printf("\n\n");
    printf("http://www.xpzhang.fun:8090/archives/csapp-cachelab-shi-yan-ji-lu-part-a\n\n");
}

// parse Option process
void parseOption(int argc, char* const argv[], char* fileName[]){
    int option;
    while((option = getopt(argc, argv, "hvs:E:b:t:")) != -1){
        switch (option)
        {
            case 'h':
                print_help();
                exit(0);
            case 'v':
                verbose = true; //set flag to output trace info
                break;
            break;
            case 's': 
                s = atoi(optarg);//! optarg is also a pointer
            case 'E':
                E = atoi(optarg);
            case 'b':
                b = atoi(optarg);
            case 't':
                strcpy(*fileName, optarg);            
        }
    }
    totalSet = 1 << s;
}

void update(unsigned address){
    unsigned mask = 0xFFFFFFFF;
    unsigned maskSet = mask >> (32 - s);
    unsigned targetSet = ((maskSet) & (address >> b));// find setIndex In address
    unsigned targetTag = address >> (s +b);// find tag in address

    LRU curLru = lru[targetSet]; // define the curLRU for targetSet
    
    //
    Node* cur = curLru.head->next;
    bool found = false;
    while(cur != curLru.tail){
        if(cur->tag == targetTag){
            found = true;
            break;
        }
        cur = cur->next;
    }

    if(found){
        hits++;
        deleteElement(targetSet, cur, &curLru);//update the one to least
        insert(targetSet, cur, &curLru);
        if(verbose)
            printf("hit!   set -> %d \n", targetSet);
    }
    else{ //!not in the cache, need to insert
        Node* newNode = malloc(sizeof(Node));
        newNode->tag = targetTag;
        if(*curLru.size == E){ //reach max capcity
            deleteElement(targetSet, curLru.tail->prev, &curLru);
            insert(targetSet, newNode, &curLru);

            evictions++;
            misses++;
            if(verbose)
	            printf("evic + miss    set -> %d\n", targetSet);
        }
        else{
            insert(targetSet, newNode, &curLru);
            misses++;
            if (verbose)
                printf("miss no evic   set  -> %d\n", targetSet);    
        }
    }
}

void cacheSimulater(char* fileName){
    // s sets and s number new lru
    // one set -> one lru
    // need init every lru!
    lru = malloc(totalSet * sizeof(*lru));
    for(int i = 0; i < totalSet; ++i){
        initializeLRU(i);
    }
    //load trace file
    FILE* file = fopen(fileName, "r");
    char option;
    unsigned address;
    int size;
    //L 0x49083899, 1
    while(fscanf(file, " %c %x, %d", &option, &address, &size) > 0){//!每个“ m”，“ l”和“ s”前面都有一个空格
        printf("%c, %x, %d\n", option, address, size);
        switch (option){
            case 'L':
                update(address);
                break;
            case 'M':
                update(address); //M
            case 'S':
                update(address);
                break;
        }
        printf("-------------------------------------\n");
    }
}

int main(int argc, char* const argv[]){
    char* fileName = malloc(100 * sizeof(char));

    // step1: parse option
    parseOption(argc, argv, &fileName);

    // step2: read all of the lines and analyze it
    cacheSimulater(fileName);

    printSummary(hits, misses, evictions);
    return 0;
}