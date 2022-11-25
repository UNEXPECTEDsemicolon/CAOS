#include <zip.h>

int main()
{
    zip_t *z = zip_open("src.zip", ZIP_RDONLY, NULL);
    zip_file_t *file = zip_fopen(z, "/src/dir/", 0);
    printf("%d\n", (int)file);
    zip_stat_t st;
    zip_stat_init(&st);
    int rc = zip_stat_index(z, 0, 0, &st);
    // zip_error_t* err = zip_get_error(z);
    printf("%s\n", zip_strerror(z));
    // zip_stat_index(z, 1, 0, &st);
    while (1);
}