    /* COP 3502C Programming Assignment 5
This program is written by: Joey Hodson
            November 30, 2020 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "leak_detector_c.h"

typedef struct trieNode {
    struct trieNode *child[26];
    int freq;
    int sumFreq;
    int currMaxFreq;
} trieNode;

trieNode* createTrieNode() {
    
    trieNode* triNode = (trieNode*)malloc(sizeof(trieNode));
    for (int x = 0; x < 26; x++) {
        triNode->child[x] = NULL;
    }
    triNode->freq = 0;
    triNode->sumFreq = 0;
    triNode->currMaxFreq = 0;

    return triNode;
}

void printNextLetter(FILE* outFile, trieNode* currNode) {

    int x;
    int max = currNode->currMaxFreq;

    for (x = 0; x < 26; x++) {
        if (currNode->child[x] != NULL) {
            if (currNode->child[x]->sumFreq == max) {
                fprintf(outFile, "%c", x + 'a'); //break;
            }
        }
    }
    fprintf(outFile, "\n");
}

void predictLetter(FILE* outFile, char* word, trieNode* currNode) {

    int wordLen = strlen(word);
    char currLett = word[0];

    if (wordLen == 1) {
        if (currNode->child[currLett - 'a']->currMaxFreq > 0) {
            printNextLetter(outFile, currNode->child[currLett - 'a']);
        }
        else {
            fprintf(outFile, "unknown word\n");
        }
        return;
    }

    if (currNode->child[currLett - 'a'] != NULL) {
        predictLetter(outFile, &word[1], currNode->child[currLett - 'a']);
    }
    else {
        fprintf(outFile, "unknown word\n");
    }
}


void updateCurrMaxFreq(trieNode* currNode) {

    if (currNode == NULL) {
        return;
    }

    int max = 0;
    for (int x = 0; x < 26; x++) {
        if (currNode->child[x] != NULL) {
            updateCurrMaxFreq(currNode->child[x]);
            if (currNode->child[x]->sumFreq > max) {
                max = currNode->child[x]->sumFreq;
            }
            //currNode->currMaxFreq = max;
        }
    }
    currNode->currMaxFreq = max;
}

void updateSumFreq(trieNode* currNode) {

    if (currNode == NULL) {
        return;
    }

    currNode->sumFreq = 0;
    for (int x = 0; x < 26; x++) {
        if (currNode->child[x] != NULL) {
            updateSumFreq(currNode->child[x]);
            currNode->sumFreq += currNode->child[x]->sumFreq;
        }
    }

    currNode->sumFreq += currNode->freq;
}

void addLetter(char currLett, int wordCount, trieNode* currNode) {

    currNode->child[currLett - 'a'] = createTrieNode();
}

void addWord(char* word, int wordCount, trieNode* currNode) {

    if (word == NULL || word[0] == '\0') {
        return;
    }

    char currLett = word[0];
    if (currNode->child[currLett - 'a'] == NULL) {
        addLetter(currLett, wordCount, currNode);
    }

    // sets last node of word it's appropriate count
    if (strlen(word) == 1) {
        currNode->child[currLett - 'a']->freq = wordCount;
        //currNode->child[currLett - 'a']->sumFreq = wordCount;
    }

    addWord(&word[1], wordCount, currNode->child[currLett - 'a']);
}

int readInFile(FILE* inFile, FILE* outFile, int commandCount, trieNode* trieRoot) {

    if (inFile == NULL) {
        return 1;
    }

    int commandType = 0;
    int wordCount = 0;
    char word[5];// = malloc(sizeof(char)*5);

    for (int x = 0; x < commandCount; x++) {

        fscanf(inFile, "%d", &commandType);
        if (commandType == 1) {
            fscanf(inFile, "%s", word);
            fscanf(inFile, "%d", &wordCount);

            addWord(word, wordCount, trieRoot);
            // separated the two functions below for clarity
            updateSumFreq(trieRoot);
            updateCurrMaxFreq(trieRoot);
        }
        else if (commandType == 2) {
            fscanf(inFile, "%s", word);
            predictLetter(outFile, word, trieRoot);
        }
        else {
            //free(word);
            return 1;
        }
    }
    //free(word);
    return 0;
}

void freeTrie(trieNode* trieRoot) {

    if (trieRoot == NULL) {
        return;
    }

    for (int x = 0; x < 26; x++) {
        freeTrie(trieRoot->child[x]);
    }

    free(trieRoot);
}

int main(void) {

     //leak detector
     //atexit(report_mem_leak);

     FILE* inFile = fopen("in.txt", "r");
     FILE* outFile = fopen("out.txt", "w");
     int commandCount;
     fscanf(inFile, "%d", &commandCount);
     
     trieNode* trieRoot = createTrieNode();

     int err = readInFile(inFile, outFile, commandCount, trieRoot);
     if (err) {fprintf(outFile, "Error while reading from input.\n");}

     freeTrie(trieRoot);

     fclose(inFile);
     fclose(outFile); 
}