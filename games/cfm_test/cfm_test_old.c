#include <stdio.h>
#include <stdint.h>
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

uint64_t* current_ip() {
    return __builtin_return_address(0);
}

void bar() {   

    // bar stuff
    // int a = 1;
    // int b = 2;
    // int c = 3;

    uint64_t *rbp; 
    asm ("mov %%rbp, %0" : "=r" (rbp) );
    cfm_verify_hash("bar", __builtin_return_address(0), *(rbp+3));
    return;
}

void foo() {
    uint64_t ip = (uint64_t) current_ip();
    ip += 45;
    uint64_t hash = cfm_store_call("bar", (uint64_t*) ip);

    bar();

    return;
}

int main(void) {
    foo();
    fprintf(stdout, "Exited gracefully...\n");
    return 0; 
}
