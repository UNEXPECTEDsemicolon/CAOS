#include <stdio.h>
#include <stdlib.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>


int main(int argc, char** argv)
{
    uint8_t raw_salt[16];
    if (0 == fread(raw_salt, sizeof(uint8_t), sizeof(raw_salt), stdin))
    {
        fprintf(stderr, "Failed to read salt\n");
        exit(1);
    }
    uint8_t* salt = raw_salt + 8;

    uint8_t* password = argv[1];

    uint8_t key[EVP_MAX_KEY_LENGTH];
    uint8_t iv[EVP_MAX_IV_LENGTH];

    if (0 == EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha256(), salt, password, strlen(password), 1, key, iv))
    {
        fprintf(stderr, "Failed to calculate key from salt and password\n");
        exit(1);
    }
    

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    {
        fprintf(stderr, "Failed to initialize aes_256_cbc decryptor\n");
        exit(1);
    }

    const size_t buff_size = 4096;
    u_int8_t buffer[buff_size];
    size_t size;
    u_int8_t result[4 * buff_size];
    int len; 
    while (size = fread(buffer, sizeof(u_int8_t), sizeof(buffer), stdin))
    {
        if (1 != EVP_DecryptUpdate(ctx, result, &len, buffer, size))
        {
            fprintf(stderr, "Failed to decrypt\n");
            ERR_print_errors(NULL);
            exit(1);
        }
        fwrite(result, sizeof(uint8_t), len, stdout);
    }
    if (1 != EVP_DecryptFinal_ex(ctx, result, &len))
    {
        fprintf(stderr, "Failed to finalize decryption\n");
        exit(1);
    }
    fwrite(result, sizeof(uint8_t), len, stdout);
}
