#include <stdio.h>
#include "bmp.h"
#include <stdlib.h>
#include <time.h>

int main() {
    time_t start = time(0);
    FILE *f;
    struct image *img = malloc(sizeof(struct image));
    f = fopen("test.bmp", "rb");
    printf("%d", bmp_from_file(f, img));
    sepia_c_inplace(img);
    if (f)
        fclose(f);
    f = fopen("test.bmp", "wb");
    printf("%d\n", bmp_to_file(f, img));
    if (f)
        fclose(f);
    image_destroy(img);
    free(img);
    printf("%luc",time(0)-start);
    return 0;
}