#include <stdio.h>
#include <stdint.h>
#include <string.h>

// SHA-3 (Keccak) state matrix size
#define WIDTH 5
#define LANESIZE 64
#define STATE_SIZE 1600 


uint64_t state[WIDTH][WIDTH];

// Function to initialize the state matrix
void initialize_state() {
    
    memset(state, 0, sizeof(state));

    
    state[0][0] = 0x1234567890abcdef; 
    state[1][0] = 0xabcdef0123456789; 
    
}


int all_capacity_lanes_nonzero() {
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (state[i][j] == 0) {
                return 0; 
            }
        }
    }
    return 1; 
}


void update_state() {
    // This is a very simplified simulation of the state update,
    // which would be the main part of the SHA-3 algorithm.
    // For this exercise, we just rotate some bits to make the state "change".
    
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            // Simulate some change in the state; simple shift for example
            state[i][j] ^= (state[i][j] >> 1);
            state[i][j] ^= (state[i][j] << 1);
        }
    }
}

int main() {
    int steps = 0;

    // Initialize the state (starting with the capacity lanes as zeros)
    initialize_state();

    // Continue updating the state until all capacity lanes have non-zero bits
    while (!all_capacity_lanes_nonzero()) {
        steps++;
        update_state();
    }

    // Print how many steps it took to make all capacity lanes non-zero
    printf("It took %d steps to make all capacity lanes non-zero.\n", steps);

    return 0;
}

