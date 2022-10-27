#include <stdio.h>
#include <openssl/sha.h>
#include <unistd.h>

int main()
{
    SHA512_CTX sha512;
    SHA512_Init(&sha512);
    const size_t buff_size = 4096;
    char buffer[buff_size];
    size_t size;
    while (size = fread(buffer, sizeof(char), sizeof(buffer), stdin))
    {
        SHA512_Update(&sha512, buffer, size);
    }
    unsigned char hash[SHA512_DIGEST_LENGTH];
    SHA512_Final(hash, &sha512);
    printf("0x");
    for (size_t i = 0; i < SHA512_DIGEST_LENGTH; i++)
    {
        printf("%02x", (int)hash[i]);
    }
    printf("\n");
}
