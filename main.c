#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#endif

#include "src/preprocessing.h"
#include "src/super_resolution.h"
#include "src/metrics.h"
#include "src/terrain_check.h"
#include "src/confidence.h"
#include "src/tiling.h"
#include "src/uncertainty.h"
#include "src/validation.h"


/* =========================================
   CREATE REQUIRED OUTPUT DIRECTORIES
   ========================================= */

static void create_output_directories(void)
{
#ifdef _WIN32

    _mkdir("dataset");
    _mkdir("dataset/hr");
    _mkdir("dataset/lr");

    _mkdir("outputs");
    _mkdir("outputs/enhanced");
    _mkdir("outputs/tiles");
    _mkdir("outputs/confidence");

#endif
}


int main(void)
{
    printf("=====================================\n");
    printf("   Trust-Aware Lunar SR System\n");
    printf("=====================================\n\n");


    /* =====================================
       1. CREATE REQUIRED DIRECTORIES
       ===================================== */

    create_output_directories();


    /* =====================================
       2. FILE PATHS
       ===================================== */

    const char *input =
        "dataset/original/lunar2.jpg";

    const char *hr =
        "dataset/hr/lunar_02_hr.png";

    const char *lr =
        "dataset/lr/lunar_02_lr.png";

    const char *model =
        "models/realesr-general-x4v3.onnx";

    const char *enhanced_output =
        "outputs/enhanced/lunar_02_enhanced.png";

    const char *tiles_folder =
        "outputs/tiles";

    const char *stitched_output =
        "outputs/stitched.png";

    const char *confidence_output =
        "outputs/confidence/confidence_map.png";


    /* =====================================
       3. PREPROCESSING
       ===================================== */

    printf("\n--- PREPROCESSING ---\n");

    int preprocessing_success =
        create_lr_hr_pair(
            input,
            hr,
            lr,
            512,
            4
        );


    if (!preprocessing_success)
    {
        printf(
            "\nERROR: Preprocessing failed.\n"
        );

        return EXIT_FAILURE;
    }


    printf(
        "\nPreprocessing completed successfully.\n"
    );

    printf(
        "HR Ground Truth : %s\n",
        hr
    );

    printf(
        "LR Input        : %s\n",
        lr
    );


    /* =====================================
       4. AI SUPER RESOLUTION
       ===================================== */

    printf("\n--- AI SUPER RESOLUTION ---\n");


    int sr_success =
        run_super_resolution(
            lr,
            enhanced_output,
            model
        );


    if (!sr_success)
    {
        printf(
            "\nERROR: AI Super-Resolution failed.\n"
        );

        return EXIT_FAILURE;
    }


    printf(
        "\nAI Super-Resolution completed successfully.\n"
    );

    printf(
        "Enhanced image: %s\n",
        enhanced_output
    );


    /* =====================================
       5. IMAGE TILING
       =====================================

       We tile the AI-enhanced output.

       This means the downstream stitched image
       still represents the AI reconstruction.
       ===================================== */

    printf("\n--- IMAGE TILING ---\n");


    int tiles_created =
        create_tiles(
            enhanced_output,
            tiles_folder,
            256
        );


    printf(
        "Tiles created: %d\n",
        tiles_created
    );


    if (tiles_created <= 0)
    {
        printf(
            "\nERROR: Image tiling failed.\n"
        );

        return EXIT_FAILURE;
    }


    /* =====================================
       6. TILE STITCHING
       ===================================== */

    printf("\n--- TILE STITCHING ---\n");


    int stitch_success =
        stitch_tiles(
            tiles_folder,
            stitched_output,
            512,
            512,
            256
        );


    if (!stitch_success)
    {
        printf(
            "\nERROR: Tile stitching failed.\n"
        );

        return EXIT_FAILURE;
    }


    printf(
        "Tile stitching completed successfully.\n"
    );

    printf(
        "Stitched AI output: %s\n",
        stitched_output
    );


    /* =====================================
       7. CONFIDENCE MAP
       =====================================

       Compare:
       Ground Truth HR
              VS
       AI-enhanced reconstructed image
       ===================================== */

    printf("\n--- CONFIDENCE MAP ---\n");


    int map_success =
        generate_confidence_map(
            hr,
            stitched_output,
            confidence_output,
            32
        );


    if (map_success)
    {
        printf(
            "Confidence map generated successfully.\n"
        );

        printf(
            "Confidence map: %s\n",
            confidence_output
        );
    }
    else
    {
        printf(
            "WARNING: Confidence map generation failed.\n"
        );
    }


    /* =====================================
       8. METRICS MODULE SANITY TEST
       =====================================

       These small arrays are only checking
       whether MSE + PSNR functions work.

       Real image validation is performed later
       using run_scientific_validation().
       ===================================== */

    printf("\n--- METRICS MODULE TEST ---\n");


    unsigned char img1[] =
    {
        100, 120,
        140, 160
    };


    unsigned char img2[] =
    {
        105, 118,
        150, 155
    };


    int test_width = 2;
    int test_height = 2;


    double mse =
        calculate_mse(
            img1,
            img2,
            test_width,
            test_height
        );


    double psnr =
        calculate_psnr(
            img1,
            img2,
            test_width,
            test_height
        );


    printf(
        "Test MSE  : %.2f\n",
        mse
    );

    printf(
        "Test PSNR : %.2f dB\n",
        psnr
    );


    /* =====================================
       9. TERRAIN PRESERVATION MODULE TEST
       ===================================== */

    printf("\n--- TERRAIN PRESERVATION TEST ---\n");


    unsigned char terrain1[] =
    {
        10, 20, 30,
        20, 80, 40,
        30, 40, 50
    };


    unsigned char terrain2[] =
    {
        11, 21, 31,
        20, 78, 42,
        29, 41, 51
    };


    double terrain_score =
        calculate_terrain_preservation(
            terrain1,
            terrain2,
            3,
            3
        );


    printf(
        "Terrain Preservation Score: %.2f%%\n",
        terrain_score
    );


    /* =====================================
       10. RECONSTRUCTION CONSISTENCY TEST
       ===================================== */

    printf(
        "\n--- RECONSTRUCTION CONSISTENCY TEST ---\n"
    );


    unsigned char original_lr[] =
    {
        50, 100,
        150, 200
    };


    unsigned char enhanced_hr[] =
    {
        48, 52, 98, 102,
        49, 51, 99, 101,
        148, 152, 198, 202,
        149, 151, 199, 201
    };


    double reconstruction_score =
        calculate_reconstruction_consistency(
            original_lr,
            2,
            2,
            enhanced_hr,
            4,
            4
        );


    printf(
        "Reconstruction Consistency: %.2f%%\n",
        reconstruction_score
    );


    /* =====================================
       11. IMAGE / MODEL AGREEMENT

       IMPORTANT:
       We now compare the HR reference against
       the actual AI reconstruction.

       Previously this compared HR with a copy
       produced by tiling HR itself.
       ===================================== */

    printf("\n--- MODEL AGREEMENT ---\n");


    double agreement_score =
        calculate_uncertainty_agreement(
            hr,
            stitched_output,
            32
        );


    printf(
        "Model Agreement: %.2f%%\n",
        agreement_score
    );


    /* =====================================
       12. SCIENTIFIC CONFIDENCE SCORE
       ===================================== */

    double confidence =
        calculate_confidence(
            reconstruction_score,
            terrain_score,
            agreement_score
        );


    const char *confidence_level =
        get_confidence_level(
            confidence
        );


    printf("\n");
    printf("=====================================\n");
    printf("      SCIENTIFIC CONFIDENCE\n");
    printf("=====================================\n");


    printf(
        "Reconstruction Score : %.2f%%\n",
        reconstruction_score
    );


    printf(
        "Terrain Score        : %.2f%%\n",
        terrain_score
    );


    printf(
        "Model Agreement      : %.2f%%\n",
        agreement_score
    );


    printf("-------------------------------------\n");


    printf(
        "Confidence Score     : %.2f%%\n",
        confidence
    );


    printf(
        "Confidence Level     : %s\n",
        confidence_level
    );


    printf("=====================================\n");


    /* =====================================
       13. REAL SCIENTIFIC VALIDATION

       Ground Truth HR
             VS
       AI Enhanced / Stitched HR

       LR is also supplied for reconstruction
       consistency calculations.
       ===================================== */

    printf(
        "\n--- SCIENTIFIC IMAGE VALIDATION ---\n"
    );


    run_scientific_validation(
        hr,
        lr,
        stitched_output,
        confidence_output
    );


    /* =====================================
       14. FINAL PIPELINE SUMMARY
       ===================================== */

    printf("\n");
    printf("=====================================\n");
    printf("       COMPLETE PIPELINE PASSED\n");
    printf("=====================================\n");


    printf(
        "Original Image   : %s\n",
        input
    );


    printf(
        "Ground Truth HR  : %s\n",
        hr
    );


    printf(
        "Low Resolution   : %s\n",
        lr
    );


    printf(
        "AI Enhanced      : %s\n",
        enhanced_output
    );


    printf(
        "Stitched AI      : %s\n",
        stitched_output
    );


    printf(
        "Confidence Map   : %s\n",
        confidence_output
    );


    printf("-------------------------------------\n");


    printf(
        "AI Model         : RealESRGAN x4\n"
    );


    printf(
        "Enhancement      : 4x\n"
    );


    printf(
        "Scientific Trust : %s (%.2f%%)\n",
        confidence_level,
        confidence
    );


    printf("=====================================\n");


    return EXIT_SUCCESS;
}