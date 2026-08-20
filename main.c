#include <stdio.h>

#include "src/preprocessing.h"


int main()
{
    printf("==============================\n");
    printf(" Trust-Aware Lunar SR System\n");
    printf("==============================\n\n");

    const char *input =
        "dataset/original/lunar_01.png";

    const char *hr =
        "dataset/hr/lunar_01_hr.png";

    const char *lr =
        "dataset/lr/lunar_01_lr.png";

    int success =
        create_lr_hr_pair(
            input,
            hr,
            lr,
            512,
            4
        );

    if (success)
    {
        printf("\nPreprocessing completed.\n");
    }
    else
    {
        printf("\nPreprocessing failed.\n");
    }

    return 0;
}