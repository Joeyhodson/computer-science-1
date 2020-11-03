/* COP 3502C Assignment 2
This program is written by: Joseph Hodson 
Due Date: 10/04/2020 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"
//'ground', 'group' & 'queue' are somewhat used interchangeably

typedef struct soldier { //cirular doubly linked list
	int sequenceNumber; //soldier number (for a given queue)
	struct soldier *prev, *next; //prev and next of a node in a queue
} soldier;

typedef struct queue { //grounds of linked list of soldiers
	soldier *front, *back; //front and back of queue
	int nodeCount; //soldier count
	int k; //kill skip variable
	int th; //kill threshold minimum leftover
	int gNumber; //ground number
	char *name; //name of ground
} queue;

int isEmpty(queue* Q) { //checks if given queue is empty
	
	if (Q->front == NULL) {
		return 1; //if the front is empty then the queue is empty or a pointer was improperly assigned or not assigned
	}
	else {
		return 0;
	}
}

soldier* createSoldier(int sequence) { //creates node for circular doubly linked list
//specifically, creates a single instance of a soldier and assigns sequenceNumber to input parameter 'val'
	soldier* newSoldier = (soldier*)malloc(sizeof(soldier));
	newSoldier->sequenceNumber = sequence;
	//newSoldier->next = NULL; initialized in enQueue
	//newSoldier->prev = NULL; initialized in enQueue
	return newSoldier;
}

int enQueue(queue *Q, int val) {

	if(Q == NULL) { //makes sure the queue isn't null to begin with
		return -1;
	}

	soldier* temp = createSoldier(val); //creates a soldier with assigned seq. num.

	if (temp != NULL) { //i.e. if node's mem was properly allocated

		temp->next = NULL; 
		temp->prev = NULL;

		if (isEmpty(Q)) {
			Q->front = temp;
		}

		if (Q->back != NULL) {
			Q->back->next = temp;
			temp->prev = Q->back;
		}
		
		Q->back = temp;
		Q->front->prev = Q->back;
		Q->back->next = Q->front;

		return 1;
	}
	else {
		return 0;
	}
}

queue* createGroup(int soldierCount, int killPattern, int killThreshold, char* groupName, int groundNumber) {

	int gNameLength = strlen(groupName) + 1;
	
	queue* aGroup = (queue*)malloc(sizeof(queue));
	aGroup->name = (char*)malloc(gNameLength*sizeof(char));
	aGroup->front = NULL;
	aGroup->back = NULL;
	aGroup->k = killPattern;
	aGroup->th = killThreshold;
	aGroup->gNumber = groundNumber; //added ground number to struct
	aGroup->nodeCount = soldierCount;
	strcpy(aGroup->name, groupName);

	return aGroup;
}

queue** readGroups(FILE *inFile, int* groupCount) {

	queue** queues = NULL;
	char groundName[51];
	int groundNumber;
	int soldierCount;
	int killThresh;
	int killSkip;

    queues = (queue**)calloc(10, sizeof(queue*)); //using calloc to set empty grounds (queues) to null
    fscanf(inFile, "%d", groupCount);

	for (int x = 0; x < *groupCount; x++) {

		fscanf(inFile, "%d", &groundNumber);
		//printf("++%d++\n", groundNumber); //debug
		fscanf(inFile, " %s", groundName);
		//printf("++%s++\n", groundName); //debug
		fscanf(inFile, "%d", &soldierCount);
		//printf("++%d++\n", soldierCount); //debug
		fscanf(inFile, "%d", &killSkip);
		//printf("++%d++\n", killSkip); //debug
		fscanf(inFile, "%d", &killThresh);
		//printf("++%d++\n", killThresh); //debug

		queues[groundNumber-1] = createGroup(soldierCount, killSkip, killThresh, groundName, groundNumber); //groundNum - 1 is finding proper index since it is shifted by 1
	}
	return queues;
}

void createReverseCircle(queue* Q) {

	if (Q != NULL) {
		for (int x = Q->nodeCount; x > 0; x--) {
			//printf("%d", Q->nodeCount); //debug
			enQueue(Q, x); //populates our queues as long as the ground isn't null
		}
	}
}

void rearrangeCircle(queue* Q) {

	if (Q != NULL) {
		soldier* curr = Q->front; //will be traversing with curr
		soldier* temp; //holds curr->next
		do {
			temp = curr->next; //the three lines in 'do' reverse the pointers of a single node
			curr->next = curr->prev;
			curr->prev = temp;
			curr = temp;
		} while(curr != Q->front); //do while allows us to loop through entire queue since it doesn't check while until after curr has moved on from front

		temp = Q->back; //now assign the queue's pointers
		Q->back = Q->front;
		Q->front = temp;
	}
}

int deQueue(queue* Q) {

	if(Q == NULL || isEmpty(Q)) { //return if queue is null or queue is empty
		return -1;
	}

	soldier* temp;
	int returnVal;

	returnVal = Q->front->sequenceNumber; //returns soldier's number
	temp = Q->front; //holds front so we can free later after reassigning front

	if (Q->front == Q->back) { //if true, then there is only one soldier left in the queue
		Q->front = NULL;
		Q->back = NULL;
	}
	else {
		Q->front = Q->front->next; //if not true, then reassign appropriate pointers
		Q->front->prev = Q->back;
		Q->back->next = Q->front;
	}

	Q->nodeCount-1; //dequeueing removes a soldier from queue
	free(temp); //free so we don't have a leak
	return returnVal;
}

int phase1(FILE* out, queue** Q, int groundCount) { //for future reference, will split up function a bit even though it's fully functional as is
	
	fprintf(out, "\nPhase1 execution\n");
	int val; //our executed soldier
	for(int x = 0; x < groundCount; x++) { //loop through all grounds
		if (Q[x] != NULL) { //checking if ground is a null ground
			
			fprintf(out, "\nLine# %d %s\n", Q[x]->gNumber, Q[x]->name); //print our ground number and name
			
			soldier* curr = Q[x]->front; //traversing node pointer 'current node'
			//add for nodeCount = thresh
			while (Q[x]->nodeCount != Q[x]->th) { //executions meet threshold

				soldier* temp = NULL; //temp variable to hold the node of executed soldier
				soldier* priorVal = NULL; //prior node before executed soldier
				soldier* afterVal = NULL; //the node after executed soldier
				//both priorVal and afterVal will point to each other, overwriting the node in between, the executed soldiers' node
				
				curr = curr->prev; //was set to prev to correct and iteration issue when finding priorVal
				for(int y = 0; y < (Q[x]->k); y++) { //loop will move curr to proper position of executed soldier
					//curr will STAY WHERE IT IS until next for loop is called again, for the same ground, to remove the next soldier
					if (y == (Q[x]->k)-1) {
						temp = curr->next; //this says, where the current position is, the next node will be the executed node
						curr = curr->next;
					}

					if (y == (Q[x]->k)-2) { //this says, where the current position is, the next node will be the prior node to the executed node
						priorVal = curr->next;
					}
					curr = curr->next; // moves our current position in list
				}
	
			 	afterVal = temp->next; //afterVal is simply the node after our temp which we found to be the executed soldier
				priorVal->next = temp->next; //overwrites the prior nodes next pointer to say it's next node should be the node after the executed soldiers'
				afterVal->prev = priorVal; //the node after executed node, points previous to the node before the executed node
				
				if(afterVal == Q[x]->front) {
					//if true, then temp is back
					Q[x]->back = temp->prev;
				}
				if(priorVal == Q[x]->back) {
					//if true, then temp is front
					Q[x]->front = temp->next;
				}
				val = temp->sequenceNumber;
				free(temp); //frees temp after appropriate pointers are pointed to the after and prior 
							//nodes, also checking if these nodes are the front and back of queue
				fprintf(out, "Soldier# %d executed\n", val);
				Q[x]->nodeCount--; //decrease our soldier count after a single execution
				//NOTE: curr is STILL left at where the for loop ended, allowing the next for loop iterations to find the next position
				// curr is only set to NULL until the entire ground executions are over with
			}
		}
	}
}

int totalSoldierCount(queue** Q, int groupCount) { //finds the entire soldier count throughout all grounds

	int count = 0;
	for (int x = 0; x < groupCount; x++) {
		if (Q[x] != NULL) {
			for (int y = 0; y < Q[x]->nodeCount; y++) {
				count++;
			}
		}
	}
	return count;
}

soldier* peek(queue *Q) {

	if(Q != NULL) {

		soldier* temp = Q->front; //will return the front of every non null queue -> used in phase 2
		//int frontSoldier = temp->sequenceNumber;
		//fprintf(out, "%d", temp->sequenceNumber);
		return temp;	
	}
}

int phase2(FILE* out, queue** Q, int groundCount) {

	int soldierCount = totalSoldierCount(Q, groundCount);
	//soldierCount is used to keep track of the total amount of soldiers left
	//printf("%d", soldierCount); //debug tool
	soldier* maxFront; //holds the greates front solider number
	soldier* currFront; //iterates through all fronts to compare against maxFront
	int maxSoldierGround; //keeps position of queue with the greatest fronts' soldier number
	int tempNodeCount; //explained below
	int max;

	fprintf(out, "\nPhase2 execution\n");
	while (soldierCount != 1) {	//leaves our lucky soldier

		max = 0; //start the max front to zero to kick of the conditional check

		for(int x = 0; x < groundCount; x++) { //loops through all grounds
			if(Q[x] != NULL) { //makes sure the grounds' queue isn't null
				if(Q[x]->front != NULL) {
					currFront = peek(Q[x]); //sets every new queue's front to check if it's the max
					if (currFront->sequenceNumber > max) {
						max = currFront->sequenceNumber; //our maxFront is the truly the max if true
						maxSoldierGround = x+1; //used to remember the queue since maxFront is just an element of the queue
					}
				}
			}
		}
		tempNodeCount = Q[maxSoldierGround-1]->nodeCount; //as we dequeue in the for loop below, the queues node count is actually shrinking. 
		//therefore, the for loop's condition of being less than the node count is ever changing.
		for (int y = 0; y < tempNodeCount; y++) {
			fprintf(out, "Executed Soldier %d from line %d\n", deQueue(Q[maxSoldierGround-1]), maxSoldierGround);
			soldierCount--;
			if (soldierCount == 1) { //must be included so the last nested loop doesn't dequeue our last soldier
				fprintf(out, "\nSoldier %d from line %d will survive", deQueue(Q[maxSoldierGround-1]), maxSoldierGround);
				break;
			}
			//soldierCount--;
		}
		//Q[maxSoldierGround-1] = NULL; //sets all the queues to null. --> wont be able to free queue names later so remove line
		//printf("\n....%d...\n", soldierCount); //debug tool
	}
	return 1;
}

int freeAll(queue** qIndex, int groundCount) {

	//soldiers are already freed
	for (int x = 0; x < groundCount; x++) {
		if (qIndex[x] != NULL) {
			
			free(qIndex[x]->name);
		}
		free(qIndex[x]);
	}
	free(qIndex);
}

void* display(FILE* out, queue *Q) {

	if(Q != NULL || Q->front != NULL) {
		
		soldier* temp = Q->front;
		while(temp != Q->back) { //could have implemented a do while just as in rearrangeCircle
			fprintf(out, " %d", temp->sequenceNumber);
			temp = temp->next;
		}
	fprintf(out, " %d", Q->back->sequenceNumber);
	}
}

void* initialList(FILE* out, queue** qIndex, int groundCount) { //displays the initial list with populated queues

	fprintf(out, "Initial nonempty lists status\n");
	for (int x = 0; x < groundCount; x++) {
		if (qIndex[x] != NULL) {
			
			fprintf(out, "%d %s", qIndex[x]->gNumber, qIndex[x]->name);
			display(out, qIndex[x]);
			fprintf(out, "\n");
		}
	}
}

void* orderedList(FILE* out, queue** qIndex, int groundCount) { //displays the ordered list after the queues are reversed

	fprintf(out, "\nAfter ordering nonempty lists status\n");
	for (int x = 0; x < groundCount; x++) {
		if (qIndex[x] != NULL) {
			
			fprintf(out, "%d %s", qIndex[x]->gNumber, qIndex[x]->name);
			display(out, qIndex[x]);
			fprintf(out, "\n");
		}
	}
}

int main(void) { //TODO: Free rest, and devise code in phase 1 for th = 1, k =1.

	atexit(report_mem_leak);
	FILE *inFile;
	FILE *outFile;
	int qCount;
	int groundCount = 10; //as defined in the assignment to be hardcoded

	inFile = fopen("int.txt", "r");
	outFile = fopen("out.txt", "w");
	
	if (inFile == NULL) {
		return -1;
	}

	queue** queueIndex = readGroups(inFile, &qCount); //creates our queueIndex, individual queues, and scans elements in per queue 

	for (int x = 0; x < groundCount; x++) //populates all queues in reverse order
	{ 
		createReverseCircle(queueIndex[x]);
	}
	initialList(outFile, queueIndex, groundCount); //displays initial, nonempty, list
	
	for (int x = 0; x < groundCount; x++) //flips pointers of all queues' nodes in reverse order
	{
		rearrangeCircle(queueIndex[x]);
	}
	orderedList(outFile, queueIndex, groundCount); //same function essentially as initialList, std output slightly different
	
	phase1(outFile, queueIndex, groundCount);
	phase2(outFile, queueIndex, groundCount);
	
	//deQueue(queueIndex[5]); //debug tool
	//display2(queueIndex[5]); //debug tool

	fclose(inFile);
	fclose(outFile);
	
	freeAll(queueIndex, groundCount);
}