#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "merkle.h"
#include "md5.h"
//#include "syslib.h"
#include "lib.h"


merkle_tree tree = {NULL};


// ###########################################################################################################################################
// ############################################################ UTILITY FUNCTIONS ############################################################
// ###########################################################################################################################################

/*
static void printBytes(unsigned char *address, int size) {
    int count;
    for (count = 0; count < size; count++){
        printf("%.2x:", address[count]);
    }
}

static void printNode(merkle_tree_node *node)
{
    printf("===%p===\n", node);
    if (node->hash == NULL) {
        printf("Hash: (nil)\n");
    }
    else {
        printf("Hash: ");
        printBytes((unsigned char*)(node->hash), 16);
        printf("\n");
    }
    printf("From: %p\nTo:   %p\nLeft: %p\nRight %p\n\n", node->from_address, node->to_address, node->left, node->right);
}


static void print2DUtil(merkle_tree_node *node, int space)
{
    // Base case
    if (node == NULL)
        return;
 
    // Increase distance between levels
    space += 40;
 
    // Process right child first
    print2DUtil(node->right, space);
 
    // Print current node after space
    printf("\n");
    for (int i = 40; i < space; i++)
        printf(" ");
    printBytes((unsigned char*)node->hash, MD5_DIGEST_LENGTH);
    printf(": %p->%p\n", node->from_address, node->to_address);
 
    // Process left child
    print2DUtil(node->left, space);
}
 
// Wrapper over print2DUtil()
static void print2D(merkle_tree_node *root)
{
    // Pass initial space count as 0
    print2DUtil(root, 0);
}
*/

// ###########################################################################################################################################
// ############################################################ PRIVATE FUNCTIONS ############################################################
// ###########################################################################################################################################

 /*
static char* naiveHash(merkle_tree_node *node)
{
    // Empty node hashes to empty string
    if (node == NULL) {
        char *hash = malloc(1);
        *hash = 0;
        return hash;
    }

    int left_length = (node->left == NULL) ? 0 : strlen(node->left->hash);
    int right_length = (node->right == NULL) ? 0 : strlen(node->right->hash);
    int length = snprintf(NULL, 0, "%zu", (size_t)node->to_address);
    
    // Create to_address string   
    char* address_str = malloc(length+1);
    snprintf(address_str, length+1, "%zu", (size_t)node->to_address);
   
    // Concat the strings
    char *hash = malloc(left_length + length + right_length + 1);
    *hash = 0;
    if (node->left != NULL) strncat(hash, node->left->hash, left_length);
    strncat(hash, address_str, length);
    if (node->right != NULL) strncat(hash, node->right->hash, right_length);

    free(address_str);
    return hash;
}
*/


// Creates a new merkle tree node from pointer to and from
static merkle_tree_node* createNode(void **from, void *to)
{
    merkle_tree_node *node = malloc(sizeof(merkle_tree_node));
    node->from_address = from;
    node->to_address = to;
    node->left = NULL;
    node->right = NULL;
    node->hash = NULL;
    return node;
}


static int cfm_pass_message(message *m, int type)
{
	int r;

	r = _syscall(CFM_PROC_NR, type, m);

	return r;
}

static int cfm_get_merkle_root(char* dest_buffer) {

	message m;
	memset(&m, 0, sizeof(m));

	if(cfm_pass_message(&m, CFM_GET_MERKLE_ROOT) != 0) {
		return -1;
	}

	memcpy(dest_buffer, m.m_cfm_sendrecv.data, 16);

	return 0;
}


static int cfm_update_merkle_root(char* src_buffer) {

	message m;
	memset(&m, 0, sizeof(m));
	memcpy(m.m_cfm_sendrecv.data, src_buffer, 16);

	if(cfm_pass_message(&m, CFM_UPDATE_MERKLE_ROOT) != 0) {
		return -1;
	}

	return 0;
}


// Get the current root node's hash from the CFM server
static void getCFMRoot(char* hash_buffer)
{
    cfm_get_merkle_root(hash_buffer);
    return;
}


// Update the root node's hash in the CFM server
static void updateCFMRoot()
{
    if (tree.root == NULL){
        char default_hash[16];
        memset(default_hash, 0, 16);
        cfm_update_merkle_root(default_hash);
    }
    else cfm_update_merkle_root(tree.root->hash);
    return;
}


/* Calculates the hash of a node.
 * The hash is = hash(left child's hash || from_address || to_address || right child's hash)
 * The hash of an empty node is defined to be all 0 bytes.
*/
static char* hashNode(merkle_tree_node *node)
{
    // Hash of empty node is 0
    char *hash = malloc(MD5_DIGEST_LENGTH);
    if (node == NULL) {   
        memset(hash, 0, MD5_DIGEST_LENGTH);
        return hash;
    }
    
    // Create input for MD5(left_hash, from, to, right_hash)
    size_t input_size = MD5_DIGEST_LENGTH + sizeof(node->from_address) + sizeof(node->to_address) + MD5_DIGEST_LENGTH;
    char input[input_size];
    if (node->left != NULL) memcpy(input, node->left->hash, MD5_DIGEST_LENGTH);
    else                    memset(input, 0, MD5_DIGEST_LENGTH);
    memcpy(&input[MD5_DIGEST_LENGTH], &node->from_address, 8);
    memcpy(&input[MD5_DIGEST_LENGTH+sizeof(node->from_address)], &node->to_address, sizeof(node->to_address));
    if (node->right != NULL) memcpy(&input[MD5_DIGEST_LENGTH+sizeof(node->from_address)+sizeof(node->to_address)], node->right->hash, MD5_DIGEST_LENGTH);
    else                     memset(&input[MD5_DIGEST_LENGTH+sizeof(node->from_address)+sizeof(node->to_address)], 0, MD5_DIGEST_LENGTH);
    
    // Hash with MD5
    MD5One((unsigned char *)input, input_size, (unsigned char *)hash);
    return hash;
}


/* Populates {path} with the search path of a search for a node with key {key}.
 * Returns the index in path of the found node. If the node at this index is NULL
 * there was no node with key {key}
*/
static int getSearchPath(void **key, merkle_tree_node *path[100])
{
    path[0] = tree.root;
    int i = 0;
    while (path[i] != NULL) {
        if (path[i]->from_address > key) path[i+1] = path[i]->left;        // Advance left
        else if (path[i]->from_address < key) path[i+1] = path[i]->right;  // Advance right
        else break;                                                        // Key match
        i++;
    }
    return i;
}


/* Returns 1 if the nodes from {path[0]} to {path[last_index]} are a valid Merkle proof and
 * the root matches the CFM server root. Otherwise returns 0. Also confirms that the nodes
 * in the path are not on the stack. If they were, the flow of the program may be hijacked when
 * we later dereference and write to these nodes. For ensuring a node is on the heap see:
 * https://devblogs.microsoft.com/oldnewthing/20170927-00/?p=97095
*/
static int validatePath(merkle_tree_node *path[100], int last_index)
{
    // Validate root node
    char *root_hash = hashNode(tree.root);
    char *cfm_root  = malloc(MD5_DIGEST_LENGTH);
    getCFMRoot(cfm_root);
    if (memcmp(root_hash, cfm_root, MD5_DIGEST_LENGTH) != 0) return 0;
    free(root_hash);
    free(cfm_root);

    // Validate path
    int i = 1;
    if ((size_t)&(path[0]) <= (size_t)sbrk(0)) return 0; // Confirm that path[0] is on the heap
    while (i <= last_index) {
        if ((size_t)&(path[i]) <= (size_t)sbrk(0)) return 0; // Confirm that path[i] is on the heap
        char *hash = hashNode(path[i]);
        if (path[i] == NULL) {
            const char DEFAULT_HASH[MD5_DIGEST_LENGTH] = {0};
            if (memcmp(hash, DEFAULT_HASH, MD5_DIGEST_LENGTH) != 0) return 0;
        }
        else if (memcmp(hash, path[i]->hash, MD5_DIGEST_LENGTH) != 0) return 0;  
        free(hash);
        i++;
    }
    return 1;
}

// ###########################################################################################################################################
// ############################################################ PUBLIC FUNCTIONS #############################################################
// ###########################################################################################################################################

/* Adds node with a payload of a pointer to the global merkle tree.
 * Cannot trust the state of memory when this function is called so the part of the merkle tree
 * we interact with (the search path) must first be validated with the CFM server and then after
 * adding the pointer the state of the tree must be cached in the CFM server.
*/
void addPointer(void **from)
{
    void *to = *(from);

    // Find the path to the node that needs inserting/updating
    merkle_tree_node *path[100];
    int i = getSearchPath(from, path);

    // Validate search path
    validatePath(path, i);

    // Update a node
    if (path[i] != NULL) {
        if (path[i]->to_address == to) return; // Nothing to update
        path[i]->to_address = to;
        free(path[i]->hash);
        path[i]->hash = hashNode(path[i]);
    }
    // Insert a node
    else {
        merkle_tree_node *new_node = createNode(from, to);        
        new_node->hash = hashNode(new_node);
        if (i == 0) tree.root = new_node;
        else {
            if (path[i-1]->from_address > from) path[i-1]->left = new_node;
            else if (path[i-1]->from_address < from) path[i-1]->right = new_node;
        }
    }

    // Propagate hash changes up the tree
    while (i > 0) {
        free(path[i-1]->hash);
        path[i-1]->hash = hashNode(path[i-1]);
        i--;
    }
    updateCFMRoot();
}


/* Searches for a node with from_address={from} and removes it from the global merkle tree.
 * Cannot trust the state of memory when this function is called so the part of the merkle tree
 * we interact with (the search path) must first be validated with the CFM server and then after
 * removing the pointer the state of the tree must be cached in the CFM server.
*/
void removePointer(void **from) 
{
    if (tree.root == NULL) return;

    // Find the path to the node that needs removing
    merkle_tree_node *path[100];
    int i = getSearchPath(from, path);

    // Validate path
    validatePath(path, i);

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
            path[i+1] = path[i]->left;  // Advance left
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
        path[i]->hash = hashNode(path[i]);
    }
    updateCFMRoot();
}


/* Verifies that the pointer at address {from} points to the same address is did when it was added to the merkle tree.
 * Returns 1 if pointer is verified and 0 if not.
 * The pointer is verified if its address and current value are in tree, the hashes along the search path in the tree
 * are valid, and the root of the tree equals the merkle root in the CFM server.
*/
int validatePointer(void **from)
{   
    // Search for node in tree
    merkle_tree_node *path[100];
    int found_node = getSearchPath(from, path);
    if ((path[found_node] == NULL) || (path[found_node]->to_address != *from)) return 0; // (key,value)=(from,*from) wasn't in tree

    // Validate root node
    return validatePath(path, found_node);
}

/*
int main()
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

    validatePointer(&p4);
    removePointer(&p4);
    
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

    removePointer(&p4);
    print2D(tree.root);

    printf("%d\n", validatePointer(&p1));
    printf("%d\n", validatePointer(&p2));
    printf("%d\n", validatePointer(&p3));
    printf("%d\n", validatePointer(&p4));
    printf("%d\n", validatePointer(&p5));
    printf("%d\n", validatePointer(&p6));
    printf("%d\n", validatePointer(&p7)); 
    printf("%d\n", validatePointer(&p8)); 
    printf("%d\n", validatePointer(&p9));

    return 0;
}
*/

// TODO: Allow other types of payloads besides only pointers
// TODO: Switch to self-balancing tree