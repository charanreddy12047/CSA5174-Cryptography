#include <stdio.h>
#include <string.h>
#include <openssl/aes.h>
#include <openssl/rand.h>

// AES block size is 16 bytes (128 bits)
#define AES_BLOCK_SIZE 16

// Function to XOR two blocks of AES_BLOCK_SIZE length
void xor_blocks(unsigned char *block1, unsigned char *block2, unsigned char *out) {
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        out[i] = block1[i] ^ block2[i];
    }
}

// Function to compute CBC-MAC for a message X using AES
void cbc_mac(unsigned char *key, unsigned char *message, int msg_len, unsigned char *mac) {
    AES_KEY aes_key;
    unsigned char iv[AES_BLOCK_SIZE] = {0};  // Initialization vector (IV) is zeroed
    unsigned char block[AES_BLOCK_SIZE];
    unsigned char prev_block[AES_BLOCK_SIZE];
    
    // Set the AES key for encryption
    AES_set_encrypt_key(key, 128, &aes_key);
    
    // Initialize the previous block with the IV (all zeros initially)
    memcpy(prev_block, iv, AES_BLOCK_SIZE);
    
    // Process each block of the message
    for (int i = 0; i < msg_len; i += AES_BLOCK_SIZE) {
        // Copy the current block
        memcpy(block, message + i, AES_BLOCK_SIZE);
        
        // XOR the block with the previous block (or IV for the first block)
        xor_blocks(block, prev_block, block);
        
        // Encrypt the XORed block
        AES_encrypt(block, block, &aes_key);
        
        // Set the current block as the previous block for the next iteration
        memcpy(prev_block, block, AES_BLOCK_SIZE);
    }
    
    // The last block of the encrypted data is the CBC-MAC
    memcpy(mac, prev_block, AES_BLOCK_SIZE);
}

// Function to print a block in hexadecimal format
void print_hex(unsigned char *data, int len) {
    for (int i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

int main() {
    // Sample message X (a one-block message)
    unsigned char message[] = "This is a one-block message.";  // Message X
    int msg_len = strlen((char *)message);  // Length of the message
    
    // AES key (128-bit)
    unsigned char key[AES_BLOCK_SIZE] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x97, 0x75, 0x46, 0x61, 0x30, 0x8b};
    
    // Compute CBC-MAC for the message X
    unsigned char mac[AES_BLOCK_SIZE];
    cbc_mac(key, message, msg_len, mac);

    printf("Original message CBC-MAC: ");
    print_hex(mac, AES_BLOCK_SIZE);
    
    // Adversary appends MAC to message X to form the new message X || (X || T)
    unsigned char adversary_message[msg_len + AES_BLOCK_SIZE];
    memcpy(adversary_message, message, msg_len);
    memcpy(adversary_message + msg_len, mac, AES_BLOCK_SIZE);  // Append MAC to the message

    // Compute CBC-MAC for the new message X || (X || T)
    unsigned char adversary_mac[AES_BLOCK_SIZE];
    cbc_mac(key, adversary_message, msg_len + AES_BLOCK_SIZE, adversary_mac);

    printf("Adversary message CBC-MAC: ");
    print_hex(adversary_mac, AES_BLOCK_SIZE);
    
    // Output:
    // The adversary MAC should be the same as the original MAC.
    if (memcmp(mac, adversary_mac, AES_BLOCK_SIZE) == 0) {
        printf("The MACs are equal. The adversary successfully forged the MAC!\n");
    } else {
        printf("The MACs are different.\n");
    }

    return 0;
}

