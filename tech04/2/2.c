extern void normalize_path(char *path)
{
    char* ptr_read = path;
    char* ptr_write = path - 1;
    char* last_slash = ptr_write;
    char *pre_last_slash = ptr_write;
    for (; *ptr_read; ++ptr_read)
    {
        if (ptr_read != path && *ptr_read == '/')
        {
            if (*ptr_write == '/')
                continue;
            else if (*ptr_write == '.')
            {
                if (ptr_write - 1 < path || *(ptr_write - 1) == '/')
                {
                    --ptr_write;
                    continue;
                }
                else if (ptr_write - 1 >= path && *(ptr_write - 1) == '.' && *(ptr_write - 2) == '/')
                {
                    ptr_write = pre_last_slash;
                    continue;
                }
            }
        }
        *++ptr_write = *ptr_read;
        if (*ptr_write == '/')
        {
            pre_last_slash = last_slash;
            last_slash = ptr_write;
        }
    }
    *++ptr_write = '\0';
}
