#include <stdio.h>
#include "bmp.h"
#include <stdlib.h>
#include <time.h>

int main() {
    time_t start = clock();
    FILE *f;
    struct image *img = malloc(sizeof(struct image));
    f = fopen("test.bmp", "rb");
    printf("%d", bmp_from_file(f, img));
    sepia_c_inplace(img);
    if (f)
        fclose(f);
    f = fopen("test1.bmp", "wb");
    printf("%d\n", bmp_to_file(f, img));
    if (f)
        fclose(f);
    image_destroy(img);
    free(img);
    printf("%fmc\n",(double)(clock()-start) * 1000 / CLOCKS_PER_SEC);
    {
        time_t start_asm = clock();
        FILE *f_asm;
        struct image *img_asm = malloc(sizeof(struct image));
        f_asm = fopen("test.bmp", "rb");
        printf("%d", bmp_from_file(f_asm, img_asm));
        extern void sepia_asm_inplace(struct pixel*,size_t size,struct pixel*);
        sepia_asm_inplace(img_asm->data,img_asm->height*img_asm->width,img_asm->data);
        if (f_asm)
            fclose(f_asm);
        f_asm = fopen("test2.bmp", "wb");
        printf("%d\n", bmp_to_file(f_asm, img_asm));
        if (f_asm)
            fclose(f_asm);
        image_destroy(img_asm);
        free(img_asm);
        printf("%fmc\n",(double)(clock()-start_asm) * 1000 / CLOCKS_PER_SEC);
    }
    return 0;
}