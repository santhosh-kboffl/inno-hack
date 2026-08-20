#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "onnxruntime_c_api.h"

typedef const OrtApiBase *
(ORT_API_CALL *OrtGetApiBaseFunction)(void);

int main(void)
{
    printf("=====================================\n");
    printf("       ONNX RUNTIME C API TEST\n");
    printf("=====================================\n\n");

    const char *dll_path =
        "third_party/onnxruntime/bin/onnxruntime.dll";

    /* STEP 1: Load DLL */
    printf("[1] Loading ONNX Runtime DLL...\n");

    HMODULE dll =
        LoadLibraryA(dll_path);

    if (dll == NULL)
    {
        printf("FAIL: Could not load DLL.\n");
        printf("Windows Error: %lu\n", GetLastError());

        return EXIT_FAILURE;
    }

    printf("PASS: DLL loaded successfully.\n\n");


    /* STEP 2: Find OrtGetApiBase */
    printf("[2] Finding OrtGetApiBase...\n");

    OrtGetApiBaseFunction get_api_base =
        (OrtGetApiBaseFunction)
        GetProcAddress(
            dll,
            "OrtGetApiBase"
        );

    if (get_api_base == NULL)
    {
        printf("FAIL: OrtGetApiBase not found.\n");

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }

    printf("PASS: OrtGetApiBase found.\n\n");


    /* STEP 3: Access API base */
    printf("[3] Accessing ONNX Runtime API...\n");

    const OrtApiBase *api_base =
        get_api_base();

    if (api_base == NULL)
    {
        printf("FAIL: OrtApiBase is NULL.\n");

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }

    printf("PASS: ONNX Runtime API accessible.\n\n");


    /* STEP 4: Print version */
    printf("ONNX Runtime Version:\n");
    printf("%s\n\n", api_base->GetVersionString());


    printf("=====================================\n");
    printf("          JOB 3 API TEST PASSED\n");
    printf("=====================================\n");

    printf("DLL Loading        : PASS\n");
    printf("OrtGetApiBase      : PASS\n");
    printf("ONNX Runtime API   : PASS\n");

    FreeLibrary(dll);

    return EXIT_SUCCESS;
}