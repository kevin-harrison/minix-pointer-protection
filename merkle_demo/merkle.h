#ifndef MERKLE_H
#define MERKLE_H


typedef struct merkle_tree_node merkle_tree_node;
struct merkle_tree_node { 
    char *hash;
    void **from_address;    // the address of the protected pointer
    void *to_address;       // the value of the protected pointer
    merkle_tree_node *left;
    merkle_tree_node *right;
};

typedef char* (*Hash_Function) (merkle_tree_node*);

typedef struct {
    size_t hash_size;
    size_t data_block_size;
    Hash_Function hash_function;
    merkle_tree_node *root;
} merkle_tree;


void addPointer(void **from);
void removePointer(void **from);
int verifyPointer(void **from);

// Macros to inject into user programs
#define STORE_POINTER(ptr) addPointer(&(ptr));
#define REMOVE_POINTER(ptr) removePointer(&(ptr));
#define VERIFY_POINTER(ptr) if(verifyPointer(&(ptr)) != 1) { printf("Tried to access unverified pointer. Killing process.\n"); exit(EXIT_FAILURE);}
 
#endif 