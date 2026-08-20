#include <stdio.h>

#include "src/super_resolution.h"

int main(void)
{
    int success =
        run_super_resolution(
            "dataset/lr/lunar_02_lr.png",
            "outputs/enhanced/lunar_02_enhanced.png",
            "models/realesr-general-x4v3.onnx"
        );

    if (success)
    {
        printf("\nSR TEST SUCCESSFUL.\n");
        return 0;
    }

    printf("\nSR TEST FAILED.\n");

    return 1;
}