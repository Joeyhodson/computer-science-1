/* COP 3502C Assignment 1
This program is written by: Joey Hodson */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "leak_detector_c.h"

//#define MAX_LENGTH 51; //name & element
//#define MAX_VALUE 1000000; //population


//structure definitions
typedef struct monster {
	//int id;
	char *name;
	char *element;
	int population;
} monster;

typedef struct region {
	char *name;
	int nmonsters;
	int total_population;
	monster **monsters;
} region;

typedef struct itinerary {
	int nregions;
	region **regions;
	int captures;
} itinerary;

typedef struct trainer {
	char *name;
	itinerary *visits;
} trainer;

monster* createMonster(char *tempname, char *tempelement, int population) {
//creates a single instance of a monster
	int mNameLength = strlen(tempname) + 1;
	int mElementLength = strlen(tempelement) + 1;

	monster* aMonster = (monster*)malloc(sizeof(monster));
	aMonster->name = (char*)malloc(mNameLength*sizeof(char));
	aMonster->element = (char*)malloc(mElementLength*sizeof(char));

	strcpy(aMonster->name, tempname);
	strcpy(aMonster->element, tempelement);
	aMonster->population = population;

	return aMonster;
}

monster** readMonsters(FILE* inFile, int* monsterCount) {
//reads from the in file, allocates enough space to hold an array of pointer structs of monsters
	monster** mIndex = NULL;
	char mName[51];
	char mElement[51];
	int mPop;

	if(inFile != NULL) {

		fscanf(inFile, "%d", monsterCount);
		mIndex = (monster**)malloc(*monsterCount*sizeof(monster*));
		fscanf(inFile, "%s", mName); //will be overwritten anyways - accepts "monsters"

		for(int x = 0; x < *monsterCount; x++) {

			fscanf(inFile, " %s", mName);
			// printf("++%s++\n", mName); //debug
			fscanf(inFile, " %s", mElement);
			fscanf(inFile, " %d", &mPop);

			mIndex[x] = createMonster(mName, mElement, mPop);
		}
	}
	return mIndex; //every monster (index)
}

trainer* createTrainer(char* tempname) {
//creates an instance of a single trainer
	int tNameLength = strlen(tempname) + 1;

	trainer* aTrainer = (trainer*)malloc(sizeof(trainer));
	aTrainer->name = (char*)malloc(tNameLength*sizeof(char));
	//aTrainer->visits = malloc(sizeof(itinerary));

	strcpy(aTrainer->name, tempname);

	return aTrainer; 
}

itinerary* createItinerary(int nregions, int captures) {
//creates a single instance of an itinerary, is found within a single trainer (visits)
	itinerary* anItinerary = (itinerary*)malloc(sizeof(itinerary)); //changed sizeof to *
	anItinerary->regions = malloc(nregions*sizeof(region*));
	
	anItinerary->nregions = nregions;
	anItinerary->captures = captures;

	return anItinerary;
}

int findRegionIndex(char* rName, int* regionCount, region** regionAtlas) {
//function matches the read name from readTrainers and matches it with the proper index
// of regionAtlas so it can properly reference it
	int properIndex;
	for(int x = 0; x < *regionCount; x++) {

		int cmp = strcmp(regionAtlas[x]->name, rName);
		if (cmp == 0) {
			properIndex = x;
		}
	}
	return properIndex;
}

trainer** readTrainers(FILE* inFile, int* trainerCount, int* regionCount, region** regionAtlas) {
//read in the trainers and allocates the trainerList. The function calls createTrainor to create 
// specific instances of trainers

	trainer** list = NULL;
	char tName[51];
	char rName[51]; 
	int nregions;
	int captures;

	if(inFile != NULL) {

		fscanf(inFile, " %d", trainerCount);
		fscanf(inFile, " %s", tName); //garbage - will overwrite anyways

		list = (trainer**)malloc(*trainerCount*sizeof(trainer*));

		for(int x = 0; x < *trainerCount; x++) {

			fscanf(inFile, " %s", tName);
			list[x] = createTrainer(tName);
			//printf("%s\n", list[x]->name); //debug
			fscanf(inFile, " %d", &captures);
			//printf("%d\n", captures); //debug
			fscanf(inFile, " %s", tName);
			//printf("%s\n", tName); //debug
			fscanf(inFile, " %d", &nregions);
			//printf("%d\n", nregions); //debug
			fscanf(inFile, " %s", tName);
			//printf("%s\n", tName); //debug
			list[x]->visits = createItinerary(nregions, captures);
			//printf("%d", list[x]->visits->nregions); //debug
			//cant be% visits[y] bc not an array of itin point structs just a solo point
			for(int y = 0; y < nregions; y++) {

				fscanf(inFile, " %s", rName);
				//printf("%s\n", rName);
				int z = findRegionIndex(rName, regionCount, regionAtlas);
				//atlas[x]->monsters[y] = monsterIndex[z]; //reference
				list[x]->visits->regions[y] = regionAtlas[z];
				//printf("%s\n", list[2]->visits->regions[0]->name); //debug	
			}
		}
	}
	//printf("%s\n", list[2]->visits->regions[0]->name); //prints "Aria" - test
	return list;
}

region* createRegion(char* tempname, int nmonsters, int total_population) {
//function creates specific instances of regions and allocates the proper sized memory
	int rNameLength = strlen(tempname) + 1;

	region* aRegion = (region*)malloc(sizeof(region));
	aRegion->name = (char*)malloc(rNameLength*sizeof(char));

	strcpy(aRegion->name, tempname);
	aRegion->nmonsters = nmonsters;
	aRegion->total_population = total_population;

	aRegion->monsters = malloc(nmonsters*sizeof(monster*));

	return aRegion;
}

int findMonsterIndex(char* mName, int* monsterCount, monster** monsterIndex) {//matches region specific monster index with proper index of mIndex
//function matches the read monster name within readRegions and returns an int that matches
// the index of monsterIndex
	int properIndex;
	for (int x = 0; x < *monsterCount; x++) {

		int cmp = strcmp(monsterIndex[x]->name, mName);
		if (cmp == 0) {
			properIndex = x;
		}
	}
	return properIndex;
}

region** readRegions(FILE* inFile, int* regionCount, int* monsterCount, monster** monsterIndex) {
//allocates the atlas including scanning in all the appropriate elements;
// the atlas is an array of pointers of type struct region
	region** atlas = NULL; 
 	char rName[51];
 	char mName[51];
 	int total_population = 0;
	int mCount;

	if(inFile != NULL) {

		fscanf(inFile, " %d", regionCount);
		fscanf(inFile, " %s", rName);

		atlas = (region**)malloc(*regionCount*sizeof(region*)); //DAing atlas

		for(int x = 0; x < *regionCount; x++) { //looping through regions
			
			fscanf(inFile, " %s", rName);
			// printf("%s\n", rName); //debug
			fscanf(inFile, " %d", &mCount);
			// printf("%d\n", mCount); //debug
			atlas[x] = createRegion(rName, mCount, total_population);
			fscanf(inFile, " %s", rName);
			// printf("%d\n", mCount); //debug
			for(int y = 0; y < mCount; y++) {

				fscanf(inFile, " %s", mName);
				// printf("^^%s^^", mName); //debug
				int z = findMonsterIndex(mName, monsterCount, monsterIndex);
				atlas[x]->monsters[y] = monsterIndex[z];
				// printf("^^%s^^", mName); //debug
				//printf("%s\n", atlas[x]->monsters[y]->name); //debug
				atlas[x]->total_population += atlas[x]->monsters[y]->population;
				//printf("%d\n", atlas[x]->total_population); //debug
			}
		//	printf("%d\n", atlas[x]->total_population);//debug
		}
	}
	return atlas;
}

void releaseAll(trainer** trainerList, monster** monsterIndex, region** regionAtlas,
// ran into the problem of releasing allocated memory twice. Discovered this was due to regions & trainers sharing the same 
// monsters & regions respectively. The way this function is set up, we do not run into that problem since we completely 
// free all monsters to begin, then regions, then trainers in an "inner to outter" way.
	int* tCount, int* mCount, int* rCount) {

	for(int x = 0; x < *mCount; x++) {
	
		free(monsterIndex[x]->element);
		free(monsterIndex[x]->name);
		free(monsterIndex[x]);
	}
	free(monsterIndex);

	for(int y = 0; y < *rCount; y++) {

		free(regionAtlas[y]->name);
		free(regionAtlas[y]->monsters);
		free(regionAtlas[y]);
	}
	free(regionAtlas);

	for(int z = 0; z < *tCount; z++) {

		free(trainerList[z]->visits->regions);
		free(trainerList[z]->visits);
		free(trainerList[z]->name);
		free(trainerList[z]);
	}
	free(trainerList);
}


int main (void) {
// main outputs to both stdout and an out file. It first calls the 3 functions needed to create 
// the monster index, region atlas, and trainer list

 	//atexit(report_mem_leak);
	FILE *fp;
	FILE *fp2;
	int mCount, tCount, rCount;

	fp = fopen("in.txt", "r");
	monster** monsterIndex = readMonsters(fp, &mCount);
	region** regionAtlas = readRegions(fp, &rCount, &mCount, monsterIndex);
	trainer** trainerList = readTrainers(fp, &tCount, &rCount, regionAtlas);

	fp2 = fopen("out.txt", "w");
	// to cmd prompt
	for(int x = 0; x < tCount; x++) {
		printf("%s\n", trainerList[x]->name);

		for(int y = 0; y < trainerList[x]->visits->nregions; y++) {
			printf("%s\n", trainerList[x]->visits->regions[y]->name);

			for(int z = 0; z < trainerList[x]->visits->regions[y]->nmonsters; z++) {
				
				double i = (float)(trainerList[x]->visits->captures);
				double j = (float)(trainerList[x]->visits->regions[y]->monsters[z]->population)/
								(trainerList[x]->visits->regions[y]->total_population);
				double k = i*j;
				k = round(k);
				if (k > 0) {
					printf("%.0f %s\n", (double)k, trainerList[x]->visits->regions[y]->monsters[z]->name);
				}
			}
		}
		printf("\n");
	}
	// to out file
	for(int x = 0; x < tCount; x++) {
		fprintf(fp2, "%s\n", trainerList[x]->name);

		for(int y = 0; y < trainerList[x]->visits->nregions; y++) {
			fprintf(fp2, "%s\n", trainerList[x]->visits->regions[y]->name);

			for(int z = 0; z < trainerList[x]->visits->regions[y]->nmonsters; z++) {
				
				double i = (float)(trainerList[x]->visits->captures);
				double j = (float)(trainerList[x]->visits->regions[y]->monsters[z]->population)/
								(trainerList[x]->visits->regions[y]->total_population);
				double k = i*j;
				k = round(k);
				if (k > 0) {
					fprintf(fp2, "%.0f %s\n", (double)k, trainerList[x]->visits->regions[y]->monsters[z]->name);
				}
			}
		}
		fprintf(fp2, "\n");
	}
	fclose(fp);
	fclose(fp2);
	releaseAll(trainerList, monsterIndex, regionAtlas, &tCount, &mCount, &rCount);
}