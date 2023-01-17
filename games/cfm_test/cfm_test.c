#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

uint64_t cfm_store_call(char const *fun, uint64_t *address);
void cfm_verify_hash(char const *fun, uint64_t *address, uint64_t hash);

uint64_t cfm_store_call(char const *fun, uint64_t *address) {
    printf("Storing call to %s(), ret addr = %p\n", fun, address);
    return 0x1234567891234567; 
};

void cfm_verify_hash(char const *fun, uint64_t *address, uint64_t hash) {
    printf("Verifying return from %s(), ret addr = %p, hash = %lx\n", fun, address, hash);
    if(hash == 0x1234567891234567) {
        return;
    } else {
        fprintf(stderr, "Detected address violation, Exiting...\n");
        exit(EXIT_FAILURE);
    }
}

// The variable will automatically push the hash on the stack
#define STORE uint64_t hash = cfm_store_call(__FUNCTION__, __builtin_return_address(0));

// We can directly use the local variable on the current stackframe
#define VERIFY cfm_verify_hash(__FUNCTION__,  __builtin_return_address(0), hash);

void print_pwd(void) {
    printf("My password is admin\n");
}

void foo(char *input) {   
    char buffer[64];
    strcpy(buffer, input);
    printf("My name is %s\n", buffer);
    return;
}

int main(int argc, char *argv[]) {
    foo(argv[1]);
    return 0; 
}




