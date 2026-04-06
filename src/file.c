#include "vanilla.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int loadFile(FileView *file, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return -1;
    }

    fseek(f, 0, SEEK_END);
    file->size = ftell(f);
    rewind(f);
    file->buf = malloc(file->size);
    fread(file->buf, 1, file->size, f);
    fclose(f);
    return 0;
}
