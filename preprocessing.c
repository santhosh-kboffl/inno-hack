#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

#include "preprocessing.h"


/* Bilinear image resizing for grayscale images */
unsigned char *resize_bilinear(
    const unsigned char *src,
    int src_width,
    int src_height,
    int dst_width,
    int dst_height
)
{
    unsigned char *dst =
        (unsigned char *)malloc(dst_width * dst_height);

    if (dst == NULL)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    float x_ratio = (float)(src_width - 1) / dst_width;
    float y_ratio = (float)(src_height - 1) / dst_height;

    for (int y = 0; y < dst_height; y++)
    {
        for (int x = 0; x < dst_width; x++)
        {
            float gx = x * x_ratio;
            float gy = y * y_ratio;

            int x1 = (int)gx;
            int y1 = (int)gy;

            int x2 = x1 + 1;
            int y2 = y1 + 1;

            if (x2 >= src_width)
                x2 = src_width - 1;

            if (y2 >= src_height)
                y2 = src_height - 1;

            float dx = gx - x1;
            float dy = gy - y1;

            int p1 = src[y1 * src_width + x1];
            int p2 = src[y1 * src_width + x2];
            int p3 = src[y2 * src_width + x1];
            int p4 = src[y2 * src_width + x2];

            float value =
                p1 * (1 - dx) * (1 - dy) +
                p2 * dx * (1 - dy) +
                p3 * (1 - dx) * dy +
                p4 * dx * dy;

            dst[y * dst_width + x] =
                (unsigned char)value;
        }
    }

    return dst;
}


int create_lr_hr_pair(
    const char *input_path,
    const char *hr_path,
    const char *lr_path,
    int hr_size,
    int scale
)
{
    int width;
    int height;
    int channels;

    /*
        Force image loading as grayscale.
        channels requested = 1
    */
    unsigned char *image =
        stbi_load(
            input_path,
            &width,
            &height,
            &channels,
            1
        );

    if (image == NULL)
    {
        printf("ERROR: Could not load image.\n");
        printf("Path: %s\n", input_path);

        return 0;
    }

    printf("Original image: %d x %d\n",
           width,
           height);

    /*
        Find largest centered square.
    */
    int crop_size =
        width < height ? width : height;

    int start_x =
        (width - crop_size) / 2;

    int start_y =
        (height - crop_size) / 2;

    unsigned char *cropped =
        (unsigned char *)malloc(
            crop_size * crop_size
        );

    if (cropped == NULL)
    {
        printf("Memory allocation failed.\n");

        stbi_image_free(image);

        return 0;
    }

    /*
        Center crop.
    */
    for (int y = 0; y < crop_size; y++)
    {
        for (int x = 0; x < crop_size; x++)
        {
            int source_index =
                (start_y + y) * width +
                (start_x + x);

            int destination_index =
                y * crop_size + x;

            cropped[destination_index] =
                image[source_index];
        }
    }

    /*
        Resize cropped image → 512x512 HR.
    */
    unsigned char *hr =
        resize_bilinear(
            cropped,
            crop_size,
            crop_size,
            hr_size,
            hr_size
        );

    if (hr == NULL)
    {
        free(cropped);
        stbi_image_free(image);

        return 0;
    }

    /*
        Create LR image.
        Example:

        HR = 512
        scale = 4
        LR = 128
    */
    int lr_size =
        hr_size / scale;

    unsigned char *lr =
        resize_bilinear(
            hr,
            hr_size,
            hr_size,
            lr_size,
            lr_size
        );

    if (lr == NULL)
    {
        free(hr);
        free(cropped);
        stbi_image_free(image);

        return 0;
    }

    /*
        Save HR image.
    */
    if (!stbi_write_png(
            hr_path,
            hr_size,
            hr_size,
            1,
            hr,
            hr_size))
    {
        printf("Failed to save HR image.\n");
    }

    /*
        Save LR image.
    */
    if (!stbi_write_png(
            lr_path,
            lr_size,
            lr_size,
            1,
            lr,
            lr_size))
    {
        printf("Failed to save LR image.\n");
    }

    printf("\nDataset pair created successfully!\n");

    printf("HR image: %d x %d\n",
           hr_size,
           hr_size);

    printf("LR image: %d x %d\n",
           lr_size,
           lr_size);

    /*
        Clean memory.
    */
    free(lr);
    free(hr);
    free(cropped);

    stbi_image_free(image);

    return 1;
}