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

//typedef char* (*Hash_Function) (unsigned char, unsigned int, unsigned char);
//typedef char* (*Hash_Function) (merkle_tree_node*);

typedef struct {
    merkle_tree_node *root;
} merkle_tree;


void addPointer(void **from);
void removePointer(void **from);
int validatePointer(void **from);

// Macros to inject into user programs
#define STORE_POINTER(ptr) addPointer((void**)&(ptr));
#define REMOVE_POINTER(ptr) removePointer((void**)&(ptr));
#define VALIDATE_POINTER(ptr) if(validatePointer((void**)&(ptr)) != 1) { printf("Tried to access unverified pointer. Killing process.\n"); while(1); exit(EXIT_FAILURE);} // TODO: remove while when libsys-bug is fixed
 
#endif 