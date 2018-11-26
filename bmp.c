#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "bmp.h"

static enum read_status read_bmp_header(FILE *file, struct bmp_header *header) {

    if (!file) return READ_INVALID_FILE;

    if (fread(&header->file, sizeof(header->file), 1, file) != 1) return READ_INVALID_FILE;

    if (header->file.bfType != 0x4D42) return READ_INVALID_BITMAP_FILE_HEADER;

    if (header->file.bfReserved) return READ_INVALID_BITMAP_FILE_HEADER;

    if (fread(&header->info, sizeof(header->info), 1, file) != 1) return READ_INVALID_FILE;

    if (header->info.biSize != 0x28) return READ_FILE_UNSUPPORTED_VERSION;

    if (header->info.biWidth < 1 || header->info.biHeight < 1) return READ_INVALID_BITMAP_INFO_HEADER;

    if (header->info.biPlanes != 1) return READ_INVALID_BITMAP_INFO_HEADER;

    if (header->info.biBitCount != 0x18) return READ_FILE_UNSUPPORTED_VERSION;

    if (header->info.biCompression) return READ_FILE_UNSUPPORTED_VERSION;

    return READ_OK;
}

struct pixel *image_get(
        struct image *img,
        uint64_t x,
        uint64_t y
) {
    return img->data + y * img->width + x;
}

struct image image_create(
        uint64_t width,
        uint64_t height
) {
    return (struct image) {
            .width = width,
            .height = height,
            .data = calloc(sizeof(struct pixel), width * height)
    };
}


void image_destroy(struct image *img) {
    img->width = 0;
    img->height = 0;
    free(img->data);
}


static uint64_t bmp_padding(uint64_t width) { return width % 4; }

enum read_status bmp_from_file(FILE *file, struct image *const img) {

    if (!file)
        return READ_NULL_PTR_FILE;

    struct bmp_header header;

    const enum read_status read_header_stat = read_bmp_header(file, &header);
    if (read_header_stat != READ_OK) return read_header_stat;

    fseek(file, header.file.bfOffBits, SEEK_SET);

    *img = image_create(header.info.biWidth, header.info.biHeight);

    const uint64_t padding = bmp_padding(img->width);

    for (uint64_t i = 0; i < img->height; i++)
        if (fread(image_get(img, 0, i),
                  img->width * sizeof(struct pixel),
                  1,
                  file
        )) {
            fseek(file, padding, SEEK_CUR);
        } else {
            image_destroy(img);
            return READ_INVALID_FILE;
        }
// fopen( "w" ); -> "wb"
    return READ_OK;
}

static struct bmp_header bmp_header_generate(
        const struct image *img
) {
    struct bmp_header header;
    header.file = (struct bmp_file_header) {
            .bfType=0x4D42,
            .bfReserved = 0,
            .bfOffBits = sizeof(struct bmp_header),
            .bfSize = (uint32_t) (sizeof(struct bmp_header) + (bmp_padding(img->width) + img->width * 3) * img->height)
    };
    header.info = (struct bmp_info_header) {0};
    header.info.biSizeImage = (uint32_t) ((bmp_padding(img->width) + img->width * 3) * img->height);
    header.info.biSize = 0x28;
    header.info.biWidth = (uint32_t) (img->width);
    header.info.biHeight = (uint32_t) (img->height);
    header.info.biPlanes = 1;
    header.info.biBitCount = 0x18;
    return header;
}

enum write_status bmp_to_file(FILE *file, struct image *const img) {
    if (!file) return WRITE_INVALID_FILE;
    if (!img) return WRITE_NULL_PTR_IMAGE;


    struct bmp_header header = bmp_header_generate(img);
    fwrite(&header, sizeof(header), 1, file);
    const uint64_t padding = bmp_padding(img->width);

    uint64_t stub = 0;

    for (uint64_t i = 0; i < img->height; i++)
        if (!fwrite(image_get(img, 0, i),
                    img->width * sizeof(struct pixel),
                    1,
                    file
        )
            ||
            (padding && !fwrite(&stub, padding, 1, file)))
            return WRITE_INVALID_FILE;

    return WRITE_OK;
}

static void swap_pixel(struct pixel *left, struct pixel *right) {
    struct pixel *temp = malloc(sizeof(struct pixel));
    memcpy(temp, left, sizeof(struct pixel));
    memcpy(left, right, sizeof(struct pixel));
    memcpy(right, temp, sizeof(struct pixel));
    free(temp);
}

int rotate180(struct image *img) {
    if (!img)
        return 0;
    for (uint64_t i = 0; i < img->height / 2; ++i)
        for (uint64_t j = 0; j < img->width; ++j)
            swap_pixel(image_get(img, i, j), image_get(img, img->height - i - 1, img->width - j - 1));
    return 1;
}

int rotate90(struct image *img) {
    if (!img)
        return 0;
    struct pixel *temp = malloc(sizeof(struct pixel));
    for (uint64_t i = 0; i < img->height / 2; ++i)
        for (uint64_t j = 0; j < img->width / 2; ++j) {
            memcpy(temp, image_get(img, i, j), sizeof(struct pixel));
            memcpy(image_get(img, i, j), image_get(img, j, img->height - i - 1), sizeof(struct pixel));
            memcpy(image_get(img, j, img->height - i - 1), image_get(img, img->height - i - 1, img->width - j - 1),
                   sizeof(struct pixel));
            memcpy(image_get(img, img->height - i - 1, img->width - j - 1), image_get(img, img->width - j - 1, i),
                   sizeof(struct pixel));
            memcpy(image_get(img, img->width - j - 1, i), temp, sizeof(struct pixel));
        }
    free(temp);
    return 1;
}

struct image *rotate_corner(struct image *img, int corner) {
    double ob, oh, aboh, dx, dy, new_height, new_width;
    int angle, rotate_angle;
    uint64_t new_x, new_y;
    uint64_t width = img->width;
    uint64_t height = img->height;

    angle = corner;

//приводим его к значению от 0 до 360
    if (angle < 0)
        angle = (angle % 360) + 360;
    else
        angle = angle % 360;

    if (angle <= 180) {
        rotate_angle = angle % 180;
        if (rotate_angle > 90)
            rotate_angle = 180 - rotate_angle;
    } else {
        rotate_angle = abs(angle - 360);
        if (rotate_angle > 90)
            rotate_angle = 180 - rotate_angle;
    }
//новые размеры
    new_width = 2 * (sqrt(pow(width, 2) +
                          pow(height, 2)) / 2 *
                     sin(atan((double) width / height) + rotate_angle * M_PI / 180));
    new_height = 2 * (sqrt(pow(width, 2) +
                           pow(height, 2)) / 2 *
                      sin(atan((double) height / width) + rotate_angle * M_PI / 180));
//сдвиги
    dx = (new_width - width) / 2;
    dy = (new_height - height) / 2;
//сдвигаем координаты элемента
    /*Image1.Left:=ImageLeft-round(dx);
    Image1.Top:=ImageTop-round(dy);*/

    struct image *result = malloc(sizeof(struct image));
    *result = image_create((uint64_t) round(new_width), (uint64_t) round(new_width));
//Важно!!!
    /*new_width = (new_width - 1);
    new_height = (new_height - 1);*/

    for (uint64_t j = 0; j < (uint64_t) round(new_width); ++j) {
        for (uint64_t k = 0; k < (uint64_t) round(new_height); ++k) {
            ob = sqrt(pow(new_width / 2 - j, 2) +
                      pow(new_height / 2 - k, 2));
            oh = new_width / 2 - j;
            if (ob)
                aboh = acos((oh<0?-oh:oh) / ob);
            else
                aboh = 0;
            if (((k >= new_height / 2) && (j < new_width / 2)) ||
                ((k < new_height / 2) && (j >= new_width / 2)))
                aboh *= -1;
            if (oh > 0) {
//новые координаты для четвертей 2 и 3
                new_x = (uint64_t) round(new_width / 2 - cos(aboh + angle * M_PI / 180) * ob - dx);
                new_y = (uint64_t) round(new_height / 2 - sin(aboh + angle * M_PI / 180) * ob - dy);
            } else {
//новые координаты для четвертей 1 и 4
                new_x = (uint64_t) round(new_width / 2 - cos(aboh + angle * M_PI / 180 + M_PI) * ob - dx);
                new_y = (uint64_t) round(new_height / 2 - sin(aboh + angle * M_PI / 180 + M_PI) * ob - dy);
            }
//если полученные координаты попадают в старое изображение...
            if ((new_x >= 0) && (new_y >= 0)
                && (new_x <= width - 1)
                && (new_y <= height - 1))
//...берем из него точку
                memcpy(image_get(result, j, k), image_get(img, new_x, new_y), sizeof(struct pixel));
            /*else
                bm.Canvas.Pixels[j, k]=$f5f5f5;*/
        }
    }
//выводим результат
    //Image1.Picture.Bitmap.Assign(bm);
    image_destroy(img);
    free(img);
    return result;
}

struct image* random_image(struct image* img) {
    srand(time(0));
    for (uint64_t i = 0; i < img->width; ++i)
        for (uint64_t j = 0; j < img->height; ++j) {
            struct pixel* pix = image_get(img, i, j);
            pix->r = rand() % 255;
            pix->g = rand() % 255;
            pix->b = rand() % 255;
        }
    return img;
}

static unsigned char sat( uint64_t x) {
    if (x < 256) return x; return 255;
}
static void sepia_one( struct pixel* const pixel ) {
    static const float c[3][3] = {
            { .393f, .769f, .189f },
            { .349f, .686f, .168f },
            { .272f, .543f, .131f } };
    struct pixel const old = *pixel;
    pixel->r = sat(
            old.r * c[0][0] + old.g * c[0][1] + old.b * c[0][2]
    );
    pixel->g = sat(
            old.r * c[1][0] + old.g * c[1][1] + old.b * c[1][2]
    );
    pixel->b = sat(
            old.r * c[2][0] + old.g * c[2][1] + old.b * c[2][2]
    );
}
void sepia_c_inplace( struct image* img ) {
    uint32_t x,y;
    for( y = 0; y < img->height; y++ )
        for( x = 0; x < img->width; x++ )
            sepia_one( image_get( img, x, y ) );
}
