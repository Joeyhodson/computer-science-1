         /* COP 3502C Assignment 3
This program was written by: Joseph Hodson 
            Date: 10/27/2020 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
//#include "leak_detector_c.h"

typedef struct monster {
    int id; // optional?
    //char name[64]; //on PDF
    char* name;
    //char element[64]; //on PDF
    char* element;
    int population;
    double weight;
} monster;

typedef struct {
    long long int compares;
    long long int copies;
} sort_results;

// function is given
void print_clocks(clock_t clocks) {

    printf("Total time taken %lfs\n", ((double) clocks) /CLOCKS_PER_SEC);
}

void printAndClearResults(sort_results* results) {
    
    printf("Total number of comparisons: %lld\n", results->compares);
    printf("Total number of copy operations: %lld\n\n", results->copies);
    results->compares = 0;
    results->copies = 0;
}

int compareTo(monster* m1, monster* m2, int criteria) {

    // if m1 < m2 return -1
    // if m1 > m2 return 1
    // if m1 == m2 return 0
    // criteria = 1 (names), 2 (weights), 3 (names & weights [names prioritized])
    
    //compare names
    if (criteria == 1) {
        if (strcmp(m1->name, m2->name) == 0) {
            //names equal
            return 0;
        }
        if (strcmp(m1->name, m2->name) < 0) {
            //m1 name > m2 name
            return 1;
        }
        else
            //m1 name < m2 name
            return -1;
    }
    //compare weights
    if (criteria == 2) {
        if (m1->weight == m2->weight) {
            //weights equal
            return 0;
        }
        if (m1->weight > m2->weight) {
            //m1 weight > m2 weight
            return 1;
        }
        else
            //m1 weight < m2 weight
            return -1;
    }
    //compare names & weights together
    if (criteria == 3) {
        if (strcmp(m1->name, m2->name) == 0) { 
            //names equal
            if (m1->weight == m2->weight) {
                //weights equal
                return 0;
            }
            if (m1->weight > m2->weight) {
                //m1 weight > m2 weight
                return 1;
            }
            else
                //m1 weight < m2 weight
                return -1;
        }
        if (strcmp(m1->name, m2->name) < 0) {
            //m1 name > m2 name
            return 1;
        }
        else
            //m1 name < m2 name
            return -1;
    }
    else {
        //improper criteria
        return 4;
    }
}

int isSorted(monster **m, int length, int criteria) {
    
    // if sorted return 1;
    // if not sorted return 0;
    // criteria = 1 (names), 2 (weights), 3 (names & weights [names prioritized])

    //compare names
    if (criteria == 1) {

        int kill;
        for (int x = 0; x < length; x++) {
            if (x == length-1)
                return 1;
            kill = strcmp(m[x]->name, m[x+1]->name);
            if (kill >= 0)
                continue;
            else
                return 0;
        }
    }
    //compare weights
    if (criteria == 2) {
        
        for (int x = 0; x < length; x++) {
            if (x == length-1)
                return 1;
            if (m[x]->weight <= m[x+1]->weight)
                continue;
            else
                return 0;
        }
    }
    //compare names & weights together
    if (criteria == 3) {

        int kill;
        for (int x = 0; x < length; x++) {
            if (x == length-1)
                return 1;

            kill = strcmp(m[x]->name, m[x+1]->name);
            if (kill > 0) {
                continue;
            }
            if (kill == 0) {
                if (m[x]->weight <= m[x+1]->weight) {
                    continue;
                }
                else
                    return 0;
            }
            else
                return 0;
        }
    }
    else {
        //improper criteria
        return 4;
    }
}

void swap(monster** m1, monster** m2, sort_results* results) {

    monster* temp = *m1;
    results->copies++;
    *m1 = *m2;
    results->copies++;
    *m2 = temp;
    results->copies++;
}

void selectionSortCore(monster** monsters, int left, int right, int criteria, sort_results* results) {

    int min_index;
    
    // loops through 1 file array and holds index where min_index will go
    for (int x = left; x < right; x++) {
        min_index = x;

        // loops through 1 file array, each time finding the new smallest index
        for (int y = x+1; y < right; y++) {

            // returns 1 if m1->criteria > m2->criteria
            int compare = compareTo(monsters[min_index], monsters[y], criteria);
            results->compares++;

            // if mIndex[i][min_index]->criteria > mIndex[i][y]->criteria 
            if (compare == 1) {
                min_index = y;
            }
        }
        // swap pointed locations
        swap(&monsters[min_index], &monsters[x], results);
    }
}

void selectionSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {
    selectionSortCore(monsters, 0, monsterCount, criteria, results);
}

// Chooses an element in the array and places all items smaller to the left and all items larger to the right.
// The pivot is guaranteed to be in the correct position, other elements are not.
int partition(monster** monsters, int left, int right, int criteria, sort_results* results) {

    int pivotIndex = right; // I don't need to make a whole new monster
    
    int i = left + rand()%(right-left+1);
    swap(&monsters[i], &monsters[pivotIndex], results);

    int nextInsert = left;
    for(int x = left; x < right; x++) {

        int compare = compareTo(monsters[x], monsters[pivotIndex], criteria);
        results->compares++;

        if (compare != 1) {
            swap(&monsters[x], &monsters[nextInsert], results);
            nextInsert++;
        }        
    }
    swap(&monsters[nextInsert], &monsters[pivotIndex], results);
    
    return nextInsert;
}

void quickSortCore(monster** monsters, int left, int right, int criteria, sort_results* results) {
// both left and right are BOTH indexes

    if (left < right) {

        int partitionIndex = partition(monsters, left, right, criteria, results);
    
        quickSortCore(monsters, left, partitionIndex-1, criteria, results);
        quickSortCore(monsters, partitionIndex+1, right, criteria, results);
    }
}

void quickSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {
    quickSortCore(monsters, 0, monsterCount-1, criteria, results);
}

void bubbleSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {
    
    for (int x = 0; x < monsterCount; x++) {
        for (int y = 0; y < monsterCount-x-1; y++) {
            int compare = compareTo(monsters[y], monsters[y+1], criteria);
            results->compares++;

            // if mIndex[i][y]->criteria > mIndex[i][y+1]->criteria
            if (compare == 1) {
                swap(&monsters[y], &monsters[y+1], results);
            }
        }
    }
}

void insertionSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {

    int y;
    monster* hand;

    for (int x = 1; x < monsterCount; x++) {
        
        hand = monsters[x]; // we will think of this monster as our hand
        results->copies++;

        for (y = x-1; y >= 0; y--) {
            int compare = compareTo(monsters[y], hand, criteria);
            results->compares++;

            // if monsters[y]->criteria > hand->criteria
            if (compare == 1) {
                monsters[y+1] = monsters[y];
            }
            else {
                break;
            }
        }
        monsters[y+1] = hand;
        results->copies++;
    }
} 

void merge(monster** monsters, int left1, int right1, int left2, int right2, int criteria, sort_results* results) {

    int length = right2 - left1;
    monster** temp = (monster**)malloc(length*sizeof(monster*));

    int p1 = left1;

    int p2 = left2;
    int x = 0;

    while (p1 != right1 && p2 != right2) {

        int compare = compareTo(monsters[p1], monsters[p2], criteria);
        results->compares++;

        // if m[p1] < m[p2]
        if (compare == -1) {
            temp[x++] = monsters[p1++];
            results->copies++;
        }
        else if (compare == 1) {
            temp[x++] = monsters[p2++];
            results->copies++;
        }
        else { // compare == 0
            temp[x++] = monsters[p1++];
            results->copies++;
            temp[x++] = monsters[p2++];
            results->copies++;
        }
    }
    
    while (p1 != right1) {
        temp[x++] = monsters[p1++];
        results->copies++;
    }

    while (p2 != right2) {
        temp[x++] = monsters[p2++];
        results->copies++;
    }
    
    for (int i = 0; i < length; i++) {
        monsters[i+left1] = temp[i];
        results->copies++;
    }

    free(temp);
}

void mergeSortCore(monster** monsters, int left, int right, int criteria, sort_results* results) {
    
    int length = right - left;

    if (length == 1) {
        return;
    }

    int middle = (left + right)/2;
    mergeSortCore(monsters, left, middle, criteria, results);
    mergeSortCore(monsters, middle, right, criteria, results);
    
    merge(monsters, left, middle, middle, right, criteria, results);
}

void mergeSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {
    mergeSortCore(monsters, 0, monsterCount, criteria, results);
}

void modifiedMergeSortCore(monster** monsters, int left, int right, int criteria, sort_results* results) {

    int length = right - left;

    if (length <= 25) {
        selectionSortCore(monsters, left, right, criteria, results);
        return;
    }

    int middle = (left + right)/2;
    modifiedMergeSortCore(monsters, left, middle, criteria, results);
    modifiedMergeSortCore(monsters, middle, right, criteria, results);
    
    merge(monsters, left, middle, middle, right, criteria, results);
}

void modifiedMergeSort(monster** monsters, int monsterCount, int criteria, sort_results* results) {
    modifiedMergeSortCore(monsters, 0, monsterCount, criteria, results);
}

monster* createMonster(/*int id,*/char* name, char* element, int pop, double weight) {

    int nameLength = strlen(name) + 1;
    int elementLength = strlen(element) + 1;
    monster* aMonster = (monster*)malloc(sizeof(monster));
    //aMonster->id = id;
    aMonster->population = pop;

    // simply allocates appropriate space for name length
    aMonster->name = (char*)malloc(nameLength*sizeof(char));
    // simply allocates appropriate space for element length
    aMonster->element = (char*)malloc(elementLength*sizeof(char));

    aMonster->weight = weight;
    // copies name to our monster struct member (name)
    strcpy(aMonster->name, name);
    // copies element to our monster struct member (element)
    strcpy(aMonster->element, element);

    return aMonster;
}

monster*** readMonsters(FILE **inFiles, int n) {

    monster*** mIndex = NULL;
    int monsterCount;
    char mName[11];
    char mElement[11];
    int id; // omitted for now
    int mPop;
    double mWeight;

    // checks if input file array is pointing to something
    if (inFiles != NULL) {
        
        // allocates space for the entire index
        mIndex = (monster***)malloc(n*sizeof(monster**));

        // for n files
        for(int x = 0; x < n; x++) {
            
            // monster count per file is (x+1) * 10,000
            monsterCount = (x+1);//*10000;
            // allocates space for 1 file of count of monsters
            mIndex[x] = (monster**)malloc(monsterCount*sizeof(monster*));
            
            // loops through count of monsters per file
            for(int y = 0; y < monsterCount; y++) {
                fscanf(inFiles[x], " %s", mName);
                //printf("----%s-----\n", mName); //debug
                fscanf(inFiles[x], " %s", mElement);
                //printf("----%s-----\n", mElement); //debug
                fscanf(inFiles[x], " %d", &mPop);
                //printf("----%d-----\n", mPop); //debug
                fscanf(inFiles[x], " %lf", &mWeight);
                //printf("----%lf-----\n", mWeight); //debug

                // 2D array -> mIndex[file#][monster#]
                mIndex[x][y] = createMonster(mName, mElement, mPop, mWeight);
            }
        }
    }
    return mIndex;
}

// frees our monster index
void freeIndex(monster*** mIndex, int n) {

    int monsterCount;
    for (int x = 0; x < n; x++) {
        monsterCount = (x+1);//*10000;
        for (int y = 0; y < monsterCount; y++) {
            free(mIndex[x][y]->name);
            free(mIndex[x][y]->element);
            free(mIndex[x][y]);
        }
        free(mIndex[x]);
    }
    free(mIndex);
}

// opens txt file to corresponding file array index
FILE** createInFileArray(FILE** inFiles, int n) {

    char buffer[20];
    for (int x = 0; x < n; x++) {

        snprintf(buffer, 20, "%d0K.txt", x+1);
        inFiles[x] = fopen(buffer, "r");
    
    }
    return inFiles;
}

// opens csv file to corresponding file array index
FILE** createOutFileArray(FILE** outFiles, int m) {

    char buffer[20];
    for (int x = 0; x < m; x++) {

        snprintf(buffer, 20, "criteria_%d.csv", x+1);
        outFiles[x] = fopen(buffer, "w");

        fprintf(outFiles[x], "DataSize,SelectionSortCompare,SelectionSortCopy,SelectionSortTime,BubbleSortCompare,BubbleSortCopy,BubbleSortTime,InsertionSortCompare,InsertionSortCopy,InsertionSortTime,MergeSortCompare,MergeSortCopy,MergeSortTime,Merge_InsertionSortCompare,Merge_InsertionSortCopy,Merge_InsertionSortTime,QuickSortCompare,QuickSortCopy,QuickSortTime");
    }
    return outFiles;
}

// closes all files in file array
void closeFiles(FILE** files, int n) {

    for (int x = 0; x < n; x++) {
        fclose(files[x]);
    }
}

monster** copyArray(monster** monsters, int monsterCount) {

    monster** monstersCopy = (monster**)malloc(monsterCount*sizeof(monster*));
    for (int x = 0; x < monsterCount; x++) {
        monstersCopy[x] = monsters[x];
    }
    return monstersCopy;
}

void printSortStatus(int isSorted, int criteria, char* algName, char* beforeOrAfter) {

    char* criteriaName;

    if (criteria == 1) {
        criteriaName = "weight";
    }
    else if (criteria == 2) {
        criteriaName = "name";
    }
    else {
        criteriaName = "name and weight";
    }

    if(isSorted == 1) {
        printf("Array status: sorted by %s %s calling %s sort\n", criteriaName, beforeOrAfter, algName);
    }
    else {
        printf("Array status: not sorted by %s %s calling %s sort\n", criteriaName, beforeOrAfter, algName);
    }
}

// TODO - Exports data to the CSV represented by outputFile
void writeToCSV(FILE* outputFile, sort_results* results, clock_t clocks) {

    // printf("Total time taken %lfs\n", ((double) clocks) /CLOCKS_PER_SEC)
    fprintf(outputFile, "%lld, %lld, %lf", results->compares, results->copies, ((double) clocks) /CLOCKS_PER_SEC);

}

void SortArrayByAlgorithm(monster** monsters, int monsterCount, int criteria, sort_results* results, FILE* outputFile, void (*sortingAlgorithm)(monster**, int, int, sort_results*), char* algName) 
{
    // Make a copy of the current monsters array before sorting it.
    monster** monstersCopy = copyArray(monsters, monsterCount);

    clock_t end_cpu, start_cpu;

    // Print the check to see if it is sorted
    printSortStatus(isSorted(monstersCopy, monsterCount, criteria), criteria, algName, "before");

    start_cpu = clock();
    // Sort using algorithm and criteria
    (*sortingAlgorithm)(monstersCopy, monsterCount, criteria, results);

    end_cpu = clock();

    // Print the check to see if it is sorted
    printSortStatus(isSorted(monstersCopy, monsterCount, criteria), criteria, algName, "after");

    // Print total time taken to sort array by criteria & given alg
    print_clocks(end_cpu - start_cpu);

    // Export data to csv
    writeToCSV(outputFile, results, end_cpu - start_cpu);

    // Print comparisons and copies and clear result attributes
    printAndClearResults(results);
    
    // Free the locally allocated memory.
    free(monstersCopy);
}

void sortArrayByCriteria(monster** monsters, int monsterCount, int criteria, sort_results* results, FILE* outputFile)
{   
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &selectionSort, "selection");
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &bubbleSort, "bubble");
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &insertionSort, "insertion");
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &mergeSort, "merge");
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &modifiedMergeSort, "modified merge");
    SortArrayByAlgorithm(monsters, monsterCount, criteria, results, outputFile, &quickSort, "quick");
}

void sortIndexByCriteria(monster*** monsterIndex, int n, int criteria, sort_results* results, FILE* outputFile)
{
    for (int i = 0; i < n; i++) {
        int monsterCount = (i+1);//*10000;
        sortArrayByCriteria(monsterIndex[i], monsterCount, criteria, results, outputFile);
        fprintf(outputFile, "\n");
    }
}

int main(void) {

    // TODO memory leak file
    
    // create our sort_results structure to record data
    sort_results* results = (sort_results*)malloc(sizeof(sort_results));
    results->compares = 0;
    results->copies = 0;
    
    // number of input files
    int n = 6;
    // number of output files
    int m = 3;

    // allocate mem for array of file pointers
    FILE** inputFiles = (FILE**)malloc(sizeof(FILE*)*n);
    FILE** outputFiles = (FILE**)malloc(sizeof(FILE*)*m);
    
    // creates our file ptr arrays
    inputFiles = createInFileArray(inputFiles, n);
    outputFiles = createOutFileArray(outputFiles, m);
    
    // creates our monster index and reads in data from files
    monster*** monsterIndex = readMonsters(inputFiles, n);
    
    for (int x = 0; x < m; x++) {
        sortIndexByCriteria(monsterIndex, n, x+1, results, outputFiles[x]);
    }

    // frees our 2D array of monster structures
    freeIndex(monsterIndex, n);

    // closes all files
    closeFiles(inputFiles, n);
    closeFiles(outputFiles, m);

    // free results
    free(results);

    // free allocated array of file ptrs
    free(inputFiles);
    free(outputFiles);
}

// TODO
// ========
// Processing Criteria x and file x0K.txt
// ========