#ifndef _BMP_H_
#include <stdint.h>
#include <stdio.h>
#define _BMP_H_

#pragma pack(push, 2)
struct bmp_file_header {
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t bfReserved;
    uint32_t bfOffBits;
};
struct bmp_info_header {
    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelsPerMeter;
    uint32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
struct bmp_header {
    struct bmp_file_header file;
    struct bmp_info_header info;
};

struct pixel {
    uint8_t r, g, b;
};

#pragma pack(pop)


struct image {
    uint64_t width, height;
    struct pixel *data;
};


enum read_status {
    READ_OK = 0,
    READ_INVALID_BITMAP_FILE_HEADER,
    READ_INVALID_BITMAP_INFO_HEADER,
    READ_INVALID_FILE,
    READ_FILE_UNSUPPORTED_VERSION,
    READ_NULL_PTR_FILE
};

enum write_status {
    WRITE_OK = 0,
    WRITE_INVALID_FILE,
    WRITE_NULL_PTR_IMAGE
};

struct pixel *image_get(
        struct image *img,
        uint64_t x,
        uint64_t y
);

struct image image_create(
        uint64_t width,
        uint64_t height
);

void image_destroy(struct image *img);

enum read_status bmp_from_file(FILE *file, struct image *const img);

enum write_status bmp_to_file(FILE *file, struct image *const img);

int rotate180(struct image * img);

int rotate90(struct image * img);

void image_destroy(struct image *img);

struct image* rotate_corner(struct image * img, int corner);

struct image* random_image(struct image * img);

void sepia_c_inplace( struct image* img );

#endif
