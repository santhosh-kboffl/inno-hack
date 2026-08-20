#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "onnxruntime_c_api.h"


#define DLL_PATH \
    "third_party/onnxruntime/bin/onnxruntime.dll"


/*
    On Windows ONNX Runtime uses ORTCHAR_T
    for model paths.

    Therefore we use a wide string: L"..."
*/
#define MODEL_PATH \
    L"models/realesr-general-x4v3.onnx"


typedef const OrtApiBase *
(ORT_API_CALL *OrtGetApiBaseFunction)(void);


/* ---------------------------------------
   Error handling helper
   --------------------------------------- */
static int check_status(
    const OrtApi *api,
    OrtStatus *status,
    const char *operation
)
{
    if (status == NULL)
    {
        return 1;
    }

    printf("\nERROR during %s\n", operation);

    printf(
        "ONNX Runtime error: %s\n",
        api->GetErrorMessage(status)
    );

    api->ReleaseStatus(status);

    return 0;
}


int main(void)
{
    printf("=========================================\n");
    printf("      ONNX SR MODEL LOAD TEST\n");
    printf("=========================================\n\n");


    /* =====================================
       STEP 1: Load ONNX Runtime DLL
       ===================================== */

    printf("[1] Loading ONNX Runtime DLL...\n");

    HMODULE dll =
        LoadLibraryA(DLL_PATH);

    if (dll == NULL)
    {
        printf("FAIL: Could not load DLL.\n");
        printf("Windows Error: %lu\n", GetLastError());

        return EXIT_FAILURE;
    }

    printf("PASS: DLL loaded.\n\n");


    /* =====================================
       STEP 2: Find OrtGetApiBase
       ===================================== */

    printf("[2] Accessing ONNX Runtime API...\n");

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


    const OrtApiBase *api_base =
        get_api_base();

    const OrtApi *api =
        api_base->GetApi(
            ORT_API_VERSION
        );

    if (api == NULL)
    {
        printf("FAIL: Could not obtain OrtApi.\n");

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }


    printf("PASS: ONNX Runtime API accessible.\n");

    printf(
        "Version: %s\n\n",
        api_base->GetVersionString()
    );


    /* =====================================
       STEP 3: Create environment
       ===================================== */

    printf("[3] Creating ONNX environment...\n");

    OrtEnv *env = NULL;

    OrtStatus *status =
        api->CreateEnv(
            ORT_LOGGING_LEVEL_WARNING,
            "LunarSR",
            &env
        );

    if (!check_status(
            api,
            status,
            "CreateEnv"))
    {
        FreeLibrary(dll);

        return EXIT_FAILURE;
    }

    printf("PASS: Environment created.\n\n");


    /* =====================================
       STEP 4: Create session options
       ===================================== */

    printf("[4] Creating session options...\n");

    OrtSessionOptions *session_options =
        NULL;

    status =
        api->CreateSessionOptions(
            &session_options
        );

    if (!check_status(
            api,
            status,
            "CreateSessionOptions"))
    {
        api->ReleaseEnv(env);

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }


    printf("PASS: Session options created.\n\n");


    /* =====================================
       STEP 5: Load SR model
       ===================================== */

    printf("[5] Loading Super-Resolution model...\n");

    printf(
        "Model: models/realesr-general-x4v3.onnx\n"
    );


    OrtSession *session = NULL;


    status =
        api->CreateSession(
            env,
            MODEL_PATH,
            session_options,
            &session
        );


    if (!check_status(
            api,
            status,
            "CreateSession"))
    {
        api->ReleaseSessionOptions(
            session_options
        );

        api->ReleaseEnv(env);

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }


    printf(
        "PASS: Super-Resolution model loaded!\n\n"
    );


    /* =====================================
       STEP 6: Count inputs
       ===================================== */

    printf("[6] Reading model structure...\n");

    size_t input_count = 0;

    size_t output_count = 0;


    status =
        api->SessionGetInputCount(
            session,
            &input_count
        );


    if (!check_status(
            api,
            status,
            "SessionGetInputCount"))
    {
        api->ReleaseSession(session);

        api->ReleaseSessionOptions(
            session_options
        );

        api->ReleaseEnv(env);

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }


    status =
        api->SessionGetOutputCount(
            session,
            &output_count
        );


    if (!check_status(
            api,
            status,
            "SessionGetOutputCount"))
    {
        api->ReleaseSession(session);

        api->ReleaseSessionOptions(
            session_options
        );

        api->ReleaseEnv(env);

        FreeLibrary(dll);

        return EXIT_FAILURE;
    }


    printf("\nMODEL INFORMATION\n");
    printf("-------------------------\n");

    printf(
        "Number of inputs  : %zu\n",
        input_count
    );

    printf(
        "Number of outputs : %zu\n",
        output_count
    );


    /* =====================================
       Final result
       ===================================== */

    printf("\n=========================================\n");
    printf("             JOB 4 PASSED\n");
    printf("=========================================\n");

    printf("ONNX Runtime       : PASS\n");
    printf("SR Model Loading   : PASS\n");
    printf("Session Creation   : PASS\n");
    printf("Input Detection    : PASS\n");
    printf("Output Detection   : PASS\n");

    printf(
        "\nThe 4x Super-Resolution model\n"
        "can now be accessed from C.\n"
    );


    /* =====================================
       Cleanup
       ===================================== */

    api->ReleaseSession(session);

    api->ReleaseSessionOptions(
        session_options
    );

    api->ReleaseEnv(env);

    FreeLibrary(dll);


    return EXIT_SUCCESS;
}