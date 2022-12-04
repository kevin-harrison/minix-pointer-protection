#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "merkle.h"


char* naiveHash(merkle_tree_node *node)
{
    int left_length = (node->left == NULL) ? 0 : strlen(node->left->hash);
    int right_length = (node->right == NULL) ? 0 : strlen(node->right->hash);
    int length = snprintf(NULL, 0, "%p", node->to_address);
    
    // Create to_address string   
    char* address_str = malloc(length+1);
    snprintf(address_str, length+1, "%p", node->to_address);
   
    // Concat the strings
    char *hash = malloc(left_length + length + right_length + 1);
    *hash = 0;
    if (node->left != NULL) strncat(hash, node->left->hash, left_length);
    strncat(hash, address_str, length);
    if (node->right != NULL) strncat(hash, node->right->hash, right_length);

    free(address_str);
    return hash;
}


#define HASH_SIZE 16 
#define NODE_SIZE sizeof(merkle_tree_node)
#define HASH_FUNCTION naiveHash
merkle_tree tree = {HASH_SIZE, NODE_SIZE, HASH_FUNCTION, NULL};



void printNode(merkle_tree_node *node)
{
    printf("===%p===\nHash: %s\nFrom: %p\nTo:   %p\nLeft: %p\nRight %p\n\n", node, node->hash, node->from_address, node->to_address, node->left, node->right);
}


void print2DUtil(merkle_tree_node *node, int space)
{
    // Base case
    if (node == NULL)
        return;
 
    // Increase distance between levels
    space += 25;
 
    // Process right child first
    print2DUtil(node->right, space);
 
    // Print current node after space
    // count
    printf("\n");
    for (int i = 25; i < space; i++)
        printf(" ");
    printf("%s: %p->%p\n", node->hash, node->from_address, node->to_address);
 
    // Process left child
    print2DUtil(node->left, space);
}
 
// Wrapper over print2DUtil()
void print2D(merkle_tree_node *root)
{
    // Pass initial space count as 0
    print2DUtil(root, 0);
}


// Creates a new merkle tree node from pointer to and from
merkle_tree_node* createNode(void **from, void *to)
{
    merkle_tree_node *node = malloc(sizeof(merkle_tree_node));
    node->from_address = from;
    node->to_address = to;
    node->left = NULL;
    node->right = NULL;
    node->hash = NULL;
    return node;
}


// TODO: get root node from CFM server
char* getCFMRoot()
{
    return tree.root->hash; // temporary solution
}

// TODO: update root node in CFM server
void updateCFMRoot(char* root_hash)
{
    //memcpy hash and send to CFM
    return;
}


// Adds node with a payload of a pointer to the global merkle tree 
void addPointer(void **from)
{
    void *to = *(from);

    // If tree is empty assign new root node
    if (tree.root == NULL) {
        merkle_tree_node *new_node = createNode(from, to);
        new_node->hash = tree.hash_function(new_node);
        tree.root = new_node;
        return;
    }

    // Find the path to the node that needs inserting/updating
    merkle_tree_node *path[100];
    path[0] = tree.root;
    int i = 0;
    while (path[i] != NULL) {
        if (path[i]->from_address > from) path[i+1] = path[i]->left;        // Advance left
        else if (path[i]->from_address < from) path[i+1] = path[i]->right;  // Advance right
        else {                                                              // Key match
            if (path[i]->to_address != to) break;
            else return; // Nothing to update
        }
        i++;
    }

    // Update a node
    if (path[i] != NULL) {
        path[i]->to_address = to;
        free(path[i]->hash);
        path[i]->hash = tree.hash_function(path[i]);
    }
    // Insert a node
    else {
        merkle_tree_node *new_node = createNode(from, to);
        if (i != 0) {
            if (path[i-1]->from_address > from) path[i-1]->left = new_node;
            else if (path[i-1]->from_address < from) path[i-1]->right = new_node;
        }
        new_node->hash = tree.hash_function(new_node);
    }
    
    // Propagate hash changes up the tree
    while (i >=0) {
        i--;
        free(path[i]->hash);
        path[i]->hash = tree.hash_function(path[i]);
    }
    updateCFMRoot(path[0]->hash);
}


// Searches for a node with from_address={from} and removes it from the global merkle tree 
void removePointer(void **from) 
{
    if (tree.root == NULL) return;

    // Find the path to the node that needs removing
    merkle_tree_node *path[100];
    path[0] = tree.root;
    int i = 0;
    while (path[i] != NULL) {
        if (path[i]->from_address > from) path[i+1] = path[i]->left ;         // Advance left
        else if (path[i]->from_address < from) path[i+1] = path[i]->right;    // Advance right
        else break;                                                           // Key match at path[i]
        i++;
    }
    if (path[i] == NULL) return; // No node to remove

    // Get the predecessors pointer to the node we want to remove    
    merkle_tree_node **pointer_to_me;
    if (i > 0 && path[i-1]->from_address > from) pointer_to_me = i > 0 ? &(path[i-1]->left) : &(tree.root);
    else                                         pointer_to_me = i > 0 ? &(path[i-1]->right) : &(tree.root);

    // No or only right child
    if (path[i]->left == NULL) {
        *pointer_to_me = path[i]->right;
        free(path[i]);
    }
    // No or only left child
    else if (path[i]->right == NULL) {
        *pointer_to_me = path[i]->left;
        free(path[i]);
    }
    // Two children 
    else {
        // need to replace with the smallest value node in subtree to the right
        int to_replace = i;
        
        path[i+1] = path[i]->right;
        i++;
        while (path[i]->left != NULL) {
            path[i+1] = path[i]->left;          // Advance left
            i++;
        }
        
        // Move node by fixing up pointers and path
        *pointer_to_me = path[i];
        path[i]->left = path[to_replace]->left;
        path[i]->right = (i == to_replace+1) ? NULL : path[to_replace]->right;
        path[i-1]->left = NULL;
        free(path[to_replace]);
        path[to_replace] = path[i];
    }

    // Propagate hash changes up the tree
    while (i > 0) {
        i--;
        free(path[i]->hash);
        path[i]->hash = tree.hash_function(path[i]);
    }
}


/* Verifies that the pointer at address {from} points to the same address is did when it was added to the merkle tree.
 * Returns 1 if pointer is verified and 0 if not.
 * The pointer is verified if its address and current value are in tree, the hashes along the search path in the tree
 * are valid, and the root of the tree equals the merkle root in the CFM server.
*/
int verifyPointer(void **from)
{   
    char *root_hash = tree.hash_function(tree.root);
    if (strcmp(root_hash, getCFMRoot()) != 0) return 0;  // Root didn't match CFM root!
    free(root_hash);

    // Follow the path to the node
    merkle_tree_node *path[100];
    path[0] = tree.root;
    int i = 0;    
    while (path[i] != NULL) {
        if (i != 0) {
            // Check if hash makes sense
            char *hash = tree.hash_function(path[i]);
            if (strcmp(hash, path[i]->hash) != 0) return 0;  
            free(hash);
        }
        if (path[i]->from_address > from) path[i+1] = path[i]->left ;               // Advance left
        else if (path[i]->from_address < from) path[i+1] = path[i]->right;          // Advance right
        else break;                                                                 // Key match at path[i]
        i++;
    }
    if ((path[i] == NULL) || (path[i]->to_address != *from)) return 0;              // Pointer wasn't in tree

    // Pointer was verified!
    return 1;
}


/* int main()
{
    void *p1 = (void*)1;
    void *p2 = (void*)2;
    void *p3 = (void*)3;
    void *p4 = (void*)4;
    void *p5 = (void*)5;
    void *p6 = (void*)6;
    void *p7 = (void*)7;
    void *p8 = (void*)8;
    void *p9 = (void*)9;

    //printf("%p->%p\n%p->%p\n%p->%p\n%p->%p\n", &p1, p1, &p2, p2, &p3, p3, &p4, p4);
    
    addPointer(&p4);
    removePointer(&p4);
    addPointer(&p4);
    addPointer(&p1);
    addPointer(&p6);
    addPointer(&p5);
    addPointer(&p9);
    addPointer(&p8);
    print2D(tree.root);
    printf("------------------------------------------------------------------------------------------------\n");

    //printNode(tree.root);
    //printNode(tree.root->left);


    removePointer(&p4);
    //removePointer(&p8);
    //removePointer(&p1);
    print2D(tree.root);

    printf("%d\n", verifyPointer(&p1));
    printf("%d\n", verifyPointer(&p2));
    printf("%d\n", verifyPointer(&p3));
    printf("%d\n", verifyPointer(&p4));
    printf("%d\n", verifyPointer(&p5));
    printf("%d\n", verifyPointer(&p6));
    printf("%d\n", verifyPointer(&p7));

    return 0;
} */

// TODO: Allow other types of payloads besides only pointers
// TODO: Switch to self-balancing tree