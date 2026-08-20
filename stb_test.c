#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

int main(void)
{
    printf("=====================================\n");
    printf("          STB IMAGE TEST\n");
    printf("=====================================\n\n");

    const char *input =
    "dataset/original/lunar1.jpg";

    const char *output =
        "stb_test_output.jpg";

    int width;
    int height;
    int channels;

    printf("Loading image...\n");

    unsigned char *image =
        stbi_load(
            input,
            &width,
            &height,
            &channels,
            0
        );

    if (image == NULL)
    {
        printf("ERROR: Could not load image.\n");
        printf("Path: %s\n", input);

        printf(
            "STB error: %s\n",
            stbi_failure_reason()
        );

        return 1;
    }

    printf("\nImage loaded successfully!\n");

    printf("Width    : %d\n", width);
    printf("Height   : %d\n", height);
    printf("Channels : %d\n\n", channels);

    printf("Saving image copy...\n");

    int success =
        stbi_write_png(
            output,
            width,
            height,
            channels,
            image,
            width * channels
        );

    if (!success)
    {
        printf("ERROR: Could not save image.\n");

        stbi_image_free(image);

        return 1;
    }

    printf("Image saved successfully!\n");

    stbi_image_free(image);

    printf("\n=====================================\n");
    printf("          STB TEST PASSED\n");
    printf("=====================================\n");

    printf("stb_image.h       : PASS\n");
    printf("stb_image_write.h : PASS\n");
    printf("stbi_load()       : PASS\n");
    printf("stbi_write_png()  : PASS\n");

    printf("\nSANthosh JOB 1 COMPLETE!\n");

    return 0;
}