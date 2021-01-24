       /* COP 3502C Assignment 4
This program was written by: Joey Hodson 
       Date: November 14, 2020 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "leak_detector_c.h"

#define MAXLEN 30
FILE* outFile;

// sub tree
typedef struct itemNode {
    char name[MAXLEN];
    int count;
    struct itemNode *left, *right;
} itemNode;

// main tree, holds sub trees
typedef struct treeNameNode {
    char treeName[MAXLEN];
    struct treeNameNode *left, *right;
    itemNode *theTree;
} treeNameNode;

// creates a node for the name tree
treeNameNode* createTreeNameNode(char* name) {

    treeNameNode* nameNode = (treeNameNode*)malloc(sizeof(treeNameNode));
    strcpy(nameNode->treeName, name);
    nameNode->left = NULL;
    nameNode->right = NULL;
    nameNode->theTree = NULL;

    return nameNode;
}

// creates a node for an item tree
itemNode* createItemNode(char* name, int count) {

    itemNode* node = (itemNode*)malloc(sizeof(itemNode));
    strcpy(node->name, name);
    node->count = count;
    node->left = NULL;
    node->right = NULL;

    return node;
}

// inserts a name node to the name tree
treeNameNode* insertNameNode(treeNameNode* root, treeNameNode* newNode) {

    if (root == NULL) {
        return newNode;
    }

    // root name > newNode name
    if (strcmp(root->treeName, newNode->treeName) > 0) {
        // if an element exists to the left of 'root' then call again
        if (root->left != NULL) {
            root->left = insertNameNode(root->left, newNode);
        }
        else {
            root->left = newNode;
        }
    }
    // root name < newNode name OR they are equal strings
    else {
        // if an element exists to the right of 'root' then call again
        if (root->right != NULL) {
            root->right = insertNameNode(root->right, newNode);
        }
        else {
            root->right = newNode;
        }
    }
    return root;
}

// inserts an item node into an item tree
itemNode* insertItemNode(itemNode* treeRoot, itemNode* newNode) {

    if (treeRoot == NULL) {
        return newNode;
    }

    // treeRoot name > newNode name
    if (strcmp(treeRoot->name, newNode->name) > 0) {
        // if an element exists to the left of it's 'root' then call again
        if (treeRoot->left != NULL) {
            treeRoot->left = insertItemNode(treeRoot->left, newNode);
        }
        else {
            treeRoot->left = newNode;
        }
    }
    // treeRoot name < newNode name OR they are equal strings
    else {
        // if an element exists to the right of it's 'root' then call again
        if (treeRoot->right != NULL) {
            treeRoot->right = insertItemNode(treeRoot->right, newNode);
        }
        else {
            treeRoot->right = newNode;
        }
    }

    return treeRoot;
}

// builds the name tree
treeNameNode* buildNameTree(FILE* inFile, int N) {

    if (inFile == NULL) {
        return NULL;
    }

    treeNameNode* nameRoot = NULL;
    char name[MAXLEN];
    for (int x = 0; x < N; x++) {
        fscanf(inFile, "%s", name);
        treeNameNode* newNameNode = createTreeNameNode(name);
        nameRoot = insertNameNode(nameRoot, newNameNode);
    }

    return nameRoot;
}

// finds and returns a node in the name tree given the node's name
treeNameNode* searchNameNode(treeNameNode* nameRoot, char treeName[MAXLEN]) {

    if (nameRoot == NULL) {
        return NULL;
    }

    if (strcmp(nameRoot->treeName, treeName) == 0) {
        return nameRoot;
    }
    else if (strcmp(nameRoot->treeName, treeName) > 0) {
        return searchNameNode(nameRoot->left, treeName);
    }
    else {
        return searchNameNode(nameRoot->right, treeName);
    }
}

// finds and returns a node in an item tree given the node's name
itemNode* searchItemNode(itemNode* root, char itemName[MAXLEN]) {

    if(root == NULL) {
        return NULL;
    }

    if(strcmp(root->name, itemName) == 0) {
        return root;
    }
    else if(strcmp(root->name, itemName) > 0) {
        return searchItemNode(root->left, itemName);
    }
    else {
        return searchItemNode(root->right, itemName);
    }
}

// scans in and populates all the item trees with their nodes
void populateTrees(FILE* inFile, treeNameNode* nameRoot, int I) {

    if (inFile == NULL) {
        return;
    }

    int count;
    char treeName[MAXLEN];
    char itemName[MAXLEN];

    for (int x = 0; x < I; x++) {
        fscanf(inFile, "%s %s %d", treeName, itemName, &count);
        itemNode* newItemNode = createItemNode(itemName, count);
        treeNameNode* treeNode = searchNameNode(nameRoot, treeName);
        treeNode->theTree = insertItemNode(treeNode->theTree, newItemNode);
    }
}

// displays the node names, in order, of the name tree
void displayInOrderNameTree(treeNameNode* root) {

    if(root == NULL) {
        return;
    }

    displayInOrderNameTree(root->left);
    fprintf(outFile, "%s ", root->treeName);
    displayInOrderNameTree(root->right);
}

// traverses and prints the names of an item tree
void traverseSubTree(itemNode* root) {
    
    if (root == NULL) {
        return;
    }
 
    traverseSubTree(root->left);
    fprintf(outFile, "%s ", root->name);
    traverseSubTree(root->right);
}

// traverses the name tree, which subsequently traverses each item tree within
void traverse_in_traverse(treeNameNode* root) {

    if (root == NULL) {
        return;
    }

    traverse_in_traverse(root->left);
    fprintf(outFile, "\n===%s===\n", root->treeName);
    traverseSubTree(root->theTree);
    //fprintf(outFile, "\b");
    traverse_in_traverse(root->right);
}

// frees an item tree of all nodes in which are malloc'd
void freeTree(itemNode* root) {

    if(root == NULL) {
        return;
    }

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// utilizes post order to free, if not, the root would be freed before other nodes could be freed (leak).
void freeAll(treeNameNode* root) {

    if(root == NULL) {
        return;
    }

    freeAll(root->left);
    freeAll(root->right);
    freeTree(root->theTree);
    free(root);
}

// counts the number of nodes that are alphabetically before a given node
int countItemsBefore(itemNode* currentNode, char itemName[MAXLEN]) {

    if (currentNode == NULL) {
        return 0;
    }

    if (strcmp(itemName, currentNode->name) <= 0) {
        return countItemsBefore(currentNode->left, itemName);
    }
    else {
        return 1 + countItemsBefore(currentNode->left, itemName) + countItemsBefore(currentNode->right, itemName);
    }
}

// prints the found count of nodes before a node
void printItemCountBefore(int count, char itemName[MAXLEN]) {

    if (count == 0) {
        fprintf(outFile, "Not found\n");
        return;
    }

    fprintf(outFile, "item before %s: %d\n", itemName, count);
}

// finds the total count of items in an item tree
int totalTreeCount(itemNode* root) {

    if (root == NULL) {
        return 0;
    }

    int leftCount = totalTreeCount(root->left);
    int rightCount = totalTreeCount(root->right);
    return root->count + leftCount + rightCount;
}

// checks whether an item node is a leaf node
int isItemLeaf(itemNode* node) {

    if (node->left == NULL && node->right == NULL) {
        return 1;
    }

    return 0;
}

// checks whether a name node is a leaf node
int isNameLeaf(treeNameNode* node) {

    if (node->left == NULL && node->right == NULL) {
        return 1;
    }

    return 0;
}

// finds and returns the parent to an item node
itemNode* itemParent(itemNode* root, itemNode* child) {

    if (root->left == child || root->right == child) {
        return root;
    }

    if (strcmp(child->name, root->name) > 0) {
        return itemParent(root->right, child);
    }
    else {
        return itemParent(root->left, child);
    }
    // extraneous cases
    return NULL;
}

// finds and returns the parent of a name node
treeNameNode* nameParent(treeNameNode* root, treeNameNode* child) {

    if (root->left == child || root->right == child) {
        return root;
    }

    if (strcmp(child->treeName, root->treeName) > 0) {
        return nameParent(root->right, child);
    }
    else {
        return nameParent(root->left, child);
    }
    // extraneous cases
    return NULL;
}

// checks whether an item node has only a left child
int itemHasOnlyLeftChild(itemNode* node) {

    if (node->left != NULL && node->right == NULL) {
        return 1;
    }

    return 0;
}

// checks whether an item node has only a right child
int itemHasOnlyRightChild(itemNode* node) {

    if (node->left == NULL && node->right != NULL) {
        return 1;
    }

    return 0;
}

// checks whether a name node has only a left child
int nameHasOnlyLeftChild(treeNameNode* node) {

    if (node->left != NULL && node->right == NULL) {
        return 1;
    }

    return 0;
}

// checks whether a name node has only a right child
int nameHasOnlyRightChild(treeNameNode* node) {

    if (node->left == NULL && node->right != NULL) {
        return 1;
    }

    return 0;
}

// finds and returns the minimum node of an item tree
itemNode* minItemNode(itemNode* root) {

    // can only be possible on the first call in the call stack
    if (root == NULL) {
        return NULL;
    }

    if (root->left == NULL) {
        return root;
    }

    return minItemNode(root->left);
}

// finds and returns the maximum node of the name tree
treeNameNode* minNameNode(treeNameNode* nameRoot) {

    // can only be possible on the first call in the call stack
    if (nameRoot == NULL) {
        return NULL;
    }

    if (nameRoot->left == NULL) {
        return nameRoot;
    }

    return minNameNode(nameRoot->left);
}

// finds and returns the maximum node of an item tree
itemNode* maxItemNode(itemNode* root) {

    // can only be possible on the first call in the call stack
    if (root == NULL) {
        return NULL;
    }

    if (root->right == NULL) {
        return root;
    }

    return maxItemNode(root->right);
}

// finds and returns the maximum node of the name tree
treeNameNode* maxNameNode(treeNameNode* nameRoot) {

    // can only be possible on the first call in the call stack
    if (nameRoot == NULL) {
        return NULL;
    }

    if (nameRoot->right == NULL) {
        return nameRoot;
    }

    return minNameNode(nameRoot->right);
}

// if a node with two children is to be deleted, this function takes the contents of the mininimum node
// of the nodeToBeDeleted->right sub tree and copies it's content into the node that is needed to be "deleted". 
// Then the function deletes the minimum node that is found. The node that's "deleted" is actually not deleted but it's values
// are that of the minimum node to the right sub tree of the node that needs to be "deleted". 
void replaceItemContents(itemNode* nodeToFill, itemNode* nodeToFree, itemNode* parentOfNodeToFree) {

    strcpy(nodeToFill->name, nodeToFree->name);
    nodeToFill->count = nodeToFree->count;

    // since we are using minItemNode, this holds true
    if (parentOfNodeToFree == nodeToFill) {
        parentOfNodeToFree->right = NULL;
    }
    else {
        parentOfNodeToFree->left = NULL;
    }

    free(nodeToFree);
    return;
}

// same concept as replaceItemContents() but applied to a name node with two children that needs to be deleted.
// There is slight derivation in the fact that the name node's sub, item tree needs to be freed as well.
void replaceNameContents(treeNameNode* nodeToFill, treeNameNode* nodeToFree, treeNameNode* parentOfNodeToFree) {

    strcpy(nodeToFill->treeName, nodeToFree->treeName);
    freeTree(nodeToFill->theTree);
    nodeToFill->theTree = nodeToFree->theTree;

    // since we are using minNameNode, this holds true
    if (parentOfNodeToFree == nodeToFill) {
        parentOfNodeToFree->right = NULL;
    }
    else {
        parentOfNodeToFree->left = NULL;
    }

    free(nodeToFree);
    return;
}

// "deletes" the root of an item tree
void removeItemRoot(itemNode* root) {
    
    // has no children (floating node)
    if (isItemLeaf(root)) {
        free(root);
        return;
    }
    // has only a left child
    else if (itemHasOnlyLeftChild(root)) {
        itemNode* temp = root;
        root = root->left;
        free(temp);
        return;
    }
    // has only a right child
    else if (itemHasOnlyRightChild(root)) {
        itemNode* temp = root;
        root = root->right;
        free(temp);
        return;
    }
    // has two children
    else {
        itemNode* minNode = minItemNode(root->right);
        itemNode* parentMinNode = itemParent(root, minNode);
        replaceItemContents(root, minNode, parentMinNode);
        return;
    }
}

// "deletes" the the root of the name tree
void removeNameRoot(treeNameNode* nameRoot, treeNameNode* nameNode) {
    
    // has no children (floating node)
    if (isNameLeaf(nameRoot)) {
        freeAll(nameRoot);
        return;
    }
    // has only a left child
    else if (nameHasOnlyLeftChild(nameRoot)) {
        treeNameNode* temp = nameRoot;
        nameRoot = nameRoot->left;
        freeTree(temp->theTree);
        free(temp);
        return;
    }
    // has only a right child
    else if (nameHasOnlyRightChild(nameRoot)) {
        treeNameNode* temp = nameRoot;
        nameRoot = nameRoot->right;
        freeTree(temp->theTree);
        free(temp);
        return;
    }
    // has two children
    else {
        treeNameNode* minNode = minNameNode(nameRoot->right);
        treeNameNode* parentMinNode = nameParent(nameRoot, minNode);
        replaceNameContents(nameRoot, minNode, parentMinNode);
        return;
    }
}

// already given node to delete - deletes a node from an item tree
void deleteItemNode(itemNode* root, itemNode* toDeleteNode, treeNameNode* nameNode) {

    if (root == NULL || toDeleteNode == NULL) {
        fprintf(outFile, "%s was not deleted from %s\n", toDeleteNode->name, nameNode->treeName);
        return;
    }

    fprintf(outFile, "%s deleted from %s\n", toDeleteNode->name, nameNode->treeName);

    // parent would be NULL if this is true.
    if (root == toDeleteNode) {
        removeItemRoot(root);
        return;
    }

    itemNode* parent = itemParent(root, toDeleteNode);
    // has no children
    if (isItemLeaf(toDeleteNode)) {
        free(toDeleteNode);
        // must have parents point to null once freed or will potentially try to free twice
        if (parent->left == toDeleteNode) {
            parent->left = NULL;
        }
        parent->right = NULL;
        return;
    }
    // has only left child
    else if (itemHasOnlyLeftChild(toDeleteNode)) {
        if (parent->left == toDeleteNode) {
            parent->left = toDeleteNode->left;
        }
        else {
            parent->right = toDeleteNode->left;
        }
        free(toDeleteNode);
        return;
    }
    // has only right child
    else if (itemHasOnlyRightChild(toDeleteNode)) {
        if (parent->left == toDeleteNode) {
            parent->left = toDeleteNode->right;
        }
        else {
            parent->right = toDeleteNode->right;
        }
        free(toDeleteNode);
        return;
    }
    // has both children
    else {
        itemNode* minNode = minItemNode(toDeleteNode->right);
        itemNode* parentMinNode = itemParent(root, minNode);
        replaceItemContents(toDeleteNode, minNode, parentMinNode);
        return;
    }
}

// already given node to delete - deletes a node from the main tree
void deleteNameNode(treeNameNode* nameRoot, treeNameNode* toDeleteNode) {

    if (nameRoot == NULL || toDeleteNode == NULL) {
        fprintf(outFile, "%s was not deleted from the name tree\n", toDeleteNode->treeName);
        return;
    }

    fprintf(outFile, "%s deleted\n", toDeleteNode->treeName);
    
    if (toDeleteNode == nameRoot) {
        removeNameRoot(nameRoot, toDeleteNode);
        return;
    }

    treeNameNode* parent = nameParent(nameRoot, toDeleteNode);
    // has no children
    if (isNameLeaf(toDeleteNode)) {
        freeTree(toDeleteNode->theTree);
        free(toDeleteNode);
        // must have parents point to null once freed or will potentially try to free twice
        if (parent->left == toDeleteNode) {
            parent->left = NULL;
        }
        parent->right = NULL;
        return;
    }
    // has only left child
    else if (nameHasOnlyLeftChild(toDeleteNode)) {
        if (parent->left == toDeleteNode) {
            parent->left = toDeleteNode->left;
        }
        else {
            parent->right = toDeleteNode->left;
        }
        freeTree(toDeleteNode->theTree);
        free(toDeleteNode);
        return;
    }
    // has only right child
    else if (nameHasOnlyRightChild(toDeleteNode)) {
        if (parent->left == toDeleteNode) {
            parent->left = toDeleteNode->right;
        }
        else {
            parent->right = toDeleteNode->right;
        }
        freeTree(toDeleteNode->theTree);
        free(toDeleteNode);
        return;
    }
    // has both children
    else {
        treeNameNode* minNode = minNameNode(toDeleteNode->right);
        treeNameNode* parentMinNode = nameParent(nameRoot, minNode);
        replaceNameContents(toDeleteNode, minNode, parentMinNode);
        return;
    }
}

// reduces the count of a given node in an item tree
void reduceCount(itemNode* root, char itemName[MAXLEN], int reduction, treeNameNode* nameNode) {

    itemNode* reducedNode = searchItemNode(root, itemName);
    if (reducedNode == NULL) {
        return;
    }

    reducedNode->count -= reduction;
    if (reducedNode->count <= 0) {
        deleteItemNode(root, reducedNode, nameNode);
        return;
    }

    fprintf(outFile, "%s reduced\n", itemName);
}

// checks whether a name node exists, if it doesn't, it reads in the rest of the instructions on the line in the input file
int nameNodeDoesNotExist(FILE* inFile, treeNameNode* node, char instruction[MAXLEN]) {
    
    char bypassItemName[MAXLEN];
    int bypassReduction;
    if (node != NULL) {
        return 0;
    }

    if (strcmp(instruction, "search") == 0 || strcmp(instruction, "item_before") == 0 || strcmp(instruction, "delete") == 0) {
        fscanf(inFile, "%s", bypassItemName);
    }
    if (strcmp(instruction, "reduce") == 0) {
        fscanf(inFile, "%s %d", bypassItemName, &bypassReduction);
    }

    return 1;
}

// returns the height of an item tree
int treeHeight(itemNode* root) {

    if (root == NULL) {
        return -1;
    }

    int leftHeight = treeHeight(root->left);
    int rightHeight = treeHeight(root->right);

    if (leftHeight > rightHeight) {
        return 1 + leftHeight;
    }
    else {
        return 1 + rightHeight;
    }
}

// checks whether or not any tree is balanced (name tree or an item tree)
void heightBalance(int leftHeight, int rightHeight, char treeName[MAXLEN]) {

    int difference = rightHeight - leftHeight;
    char* status = "balanced";
    if (difference > 1) {
        status = "not balanced";
    }

    fprintf(outFile, "%s: left height %d, right height %d, difference %d, %s\n", treeName, leftHeight, rightHeight, difference, status);
}

// prints the count of items in a node
void printItemNodeCount(itemNode* item, char itemName[MAXLEN], treeNameNode* nameNode) {
    
    if (item == NULL) {
        fprintf(outFile, "%s not found in %s\n", itemName, nameNode->treeName);
        return;
    }

    fprintf(outFile, "%d %s found in %s\n", item->count, item->name, nameNode->treeName);
}

// processes each line of commands as they are needed to be. 
void processQueries(FILE* inFile, treeNameNode* nameRoot, int Q) {

    if (inFile == NULL) {
        return;
    }

    fprintf(outFile, "\n");
    char instruction[MAXLEN];
    char treeName[MAXLEN];
    char itemName[MAXLEN];
    // reduction count for reduceCount()
    int reduction = 0;

    // loops through queries and executes commands
    for (int x = 0; x < Q; x++) {

        fscanf(inFile, "%s %s ", instruction, treeName);
        treeNameNode* nameNode = searchNameNode(nameRoot, treeName);

        if (nameNodeDoesNotExist(inFile, nameNode, instruction)) {
            fprintf(outFile, "%s does not exist\n", treeName);
            continue;
        }
        itemNode* itemRoot = nameNode->theTree;
        
        if (strcmp(instruction, "search") == 0) {
            fscanf(inFile,"%s", itemName);
            itemNode* item = searchItemNode(itemRoot, itemName);
            printItemNodeCount(item, itemName, nameNode);
        }

        else if(strcmp(instruction, "item_before") == 0) {
            fscanf(inFile,"%s", itemName);
            int count = countItemsBefore(itemRoot, itemName);
            printItemCountBefore(count, itemName);
        }
    
        else if(strcmp(instruction, "height_balance") == 0) {
            int leftHeight = treeHeight(itemRoot->left);
            int rightHeight = treeHeight(itemRoot->right);
            heightBalance(leftHeight, rightHeight, treeName);
        }

        else if(strcmp(instruction, "count") == 0) {
            fprintf(outFile, "%s count %d\n", treeName, totalTreeCount(itemRoot));
        }

        else if(strcmp(instruction, "reduce") == 0) {
            fscanf(inFile,"%s %d", itemName, &reduction);
            reduceCount(itemRoot, itemName, reduction, nameNode);
        }
        
        else if(strcmp(instruction, "delete") == 0) {
            fscanf(inFile,"%s", itemName);
            itemNode* item = searchItemNode(itemRoot, itemName);
            deleteItemNode(itemRoot, item, nameNode);
        }
        
        else if(strcmp(instruction, "delete_name") == 0) {
            deleteNameNode(nameRoot, nameNode);
        }

        else {
            continue;
        } 
    }    
}

int main(void) {

    // leak detector
    atexit(report_mem_leak);

    int N, I, Q;
    FILE* inFile = fopen("in.txt", "r");
    outFile = fopen("out.txt", "w");

    fscanf(inFile, "%d %d %d", &N, &I, &Q);
    treeNameNode* nameRoot = buildNameTree(inFile, N);
    populateTrees(inFile, nameRoot, I);
    
    displayInOrderNameTree(nameRoot);
    traverse_in_traverse(nameRoot);
    
    processQueries(inFile, nameRoot, Q);

    freeAll(nameRoot);

    fclose(inFile);
    fclose(outFile);
}