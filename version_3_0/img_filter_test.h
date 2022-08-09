/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_filter_test.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These functions have been implemented to test the functionality and accuracy of the filter functions
 */

#ifndef SRC_IMG_FILTER_TEST_H_
#define SRC_IMG_FILTER_TEST_H_

#include "img_filter_base.h"

/*********************************************************************************************************************/
struct deviation {
    float pos_max = 0.0f;
    float neg_max = 0.0f;
    float abs_max = 0.0f;

    inline void update(const deviation d) {
        pos_max = MAX(d.pos_max, pos_max);
        neg_max = MAX(d.neg_max, neg_max);
        abs_max = MAX(d.pos_max, abs_max);
    }
};

/*********************************************************************************************************************/
template <vx_uint64 IMG_COLS, vx_uint64 IMG_ROWS, vx_uint16 TEST_CYCLES, vx_uint16 NUMBER_OF_BINS> //
void printDifBin(vx_uint64 *DifBin, const float diff_limit) {                                      //

    const float bin_size = diff_limit / static_cast<float>(NUMBER_OF_BINS - 1);

    const vx_uint64 NUMBER_OF_PIXELS = IMG_COLS * IMG_ROWS * TEST_CYCLES;

    std::cout << "Number of computed values: " << NUMBER_OF_PIXELS << "\n";
    std::cout << "Deviation\tAmount\tPercent\n";

    for (vx_uint16 i = 0; i < NUMBER_OF_BINS - 1; i++) {
        const vx_uint64 n = DifBin[i];
        const double p = 100.0f * static_cast<double>(n) / static_cast<double>(NUMBER_OF_PIXELS);
        std::cout << "< " << (i + 1) * bin_size << ": " << n << " " << p << "\n";
    }

    const vx_uint64 n = DifBin[NUMBER_OF_BINS - 1];
    const double p = 100.0f * static_cast<double>(n) / static_cast<double>(NUMBER_OF_PIXELS);
    std::cout << ">= " << (NUMBER_OF_BINS - 1) * bin_size << ": " << n << " " << p << "\n";
}

/*********************************************************************************************************************/
// copied from img_filter_test.h and extended by bool value to print just maximum absolute difference
template <typename Type, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint32 BORDER_RADIUS, vx_uint16 NUMBER_OF_BINS> //
deviation CheckError(float *output_sw, Type *output_hw, vx_uint64 *DifBin, const float diff_limit,
                     const vx_bool print_max_only) { //

    const float bin_size = diff_limit / static_cast<float>(NUMBER_OF_BINS - 1);
    const float bin_mul = 1 / bin_size;

    float absDifMax = 0;
    float PosDifMax = 0;
    float NegDifMax = 0;
    vx_uint16 fy = 0;
    vx_uint16 fx = 0;

    for (vx_uint32 y = BORDER_RADIUS; y < IMG_ROWS - BORDER_RADIUS; y++) {
        for (vx_uint32 x = BORDER_RADIUS; x < IMG_COLS - BORDER_RADIUS; x++) {
            vx_uint32 ptr = y * IMG_COLS + x;
            const float A = output_sw[ptr];
            const float B = fractionToFloat<Type>(output_hw[ptr]);
            const float dif = B - A;
            const float absDif = ABS(dif);

            fy = (absDif > absDifMax) ? (y) : (fy);
            fx = (absDif > absDifMax) ? (x) : (fx);

            absDifMax = MAX(absDifMax, absDif);
            PosDifMax = (dif > 0) ? MAX(PosDifMax, absDif) : (PosDifMax);
            NegDifMax = (dif < 0) ? MAX(NegDifMax, absDif) : (NegDifMax);

            if (DifBin != NULL) {

                const vx_uint16 bin_index = (absDif < diff_limit) ? (static_cast<vx_uint16>(absDif * bin_mul))
                                                                  : (static_cast<vx_uint16>(NUMBER_OF_BINS - 1));

                DifBin[bin_index]++;
            }
            if (absDif >= diff_limit && print_max_only == vx_false_e) {
                printf("%4d%4d:\t %f\t%f\t%f\n", y, x, A, B, absDif);
            }
        }
    }
    printf("Max. absolut error: %0.8f LAST_ENTRY_YX(%i, %i)\n", absDifMax, fy, fx);

    return deviation{PosDifMax, NegDifMax, absDifMax};
}

/**********************************************************************************************************************/
/** @brief SW: Computes the maximum absolute error between two images
 */
template <typename Type, const vx_uint32 IMG_COLS, const vx_uint32 IMG_ROWS, vx_uint8 VEC_NUM, vx_uint16 FILTER_SIZE,
          vx_border_e BORDER_TYPE>
void CheckError(Type *output_sw, Type *output_hw, std::string &name) {

    float absDifMax = 0;
    vx_uint32 BORDER_RADIUS = FILTER_SIZE / 2;

    for (vx_uint32 y = BORDER_RADIUS; y < IMG_ROWS - BORDER_RADIUS; y++) {
        for (vx_uint32 x = BORDER_RADIUS; x < IMG_COLS - BORDER_RADIUS; x++) {
            vx_uint32 ptr = y * IMG_COLS + x;
            float A = static_cast<float>(output_sw[ptr]);
            float B = static_cast<float>(output_hw[ptr]);
            float absDif = ABS(A - B);
            absDifMax = MAX(absDifMax, absDif);

            if (absDif > 4) {
                printf("%4d%4d:\t %f\t%f\t%f\n", y, x, A, B, absDif);
                // getchar();
            }
        }
    }

    if (absDifMax >= 0) {
        std::cout << name.c_str();
        printf("Max. absolut error: %0.2f", absDifMax);
        printf(" | VecSize: %d | KernSize: %d | DataSize: %d | BorderType: %d\n", VEC_NUM, FILTER_SIZE, sizeof(Type),
               BORDER_TYPE);
    }
}

/**********************************************************************************************************************/
template <vx_uint16 WIDTH, vx_uint16 HEIGHT>
void SwFED(const float *inputLt, const float *inputLf, float *output, const float stepsize) {

    for (vx_uint16 ym = 0; ym < HEIGHT; ym++) {

        // Y pointer
        vx_uint32 yl = std::max(ym - 1, 0);
        vx_uint32 yh = std::min(ym + 1, HEIGHT - 1);
        const float *Lf_l = &inputLf[yl * WIDTH];
        const float *Lf_m = &inputLf[ym * WIDTH];
        const float *Lf_h = &inputLf[yh * WIDTH];
        const float *Lt_l = &inputLt[yl * WIDTH];
        const float *Lt_m = &inputLt[ym * WIDTH];
        const float *Lt_h = &inputLt[yh * WIDTH];
        float *Lstep_dst = &output[ym * WIDTH];

        for (vx_uint16 xm = 0; xm < WIDTH; xm++) {

            // X pointer
            vx_uint32 xl = std::max(xm - 1, 0);
            vx_uint32 xh = std::min(xm + 1, WIDTH - 1);

            // Compute FED
            float LfCenter = Lf_m[xm];
            float LtCenter = Lt_m[xm];
            float xpos = (Lf_m[xh] + LfCenter) * (Lt_m[xh] - LtCenter);
            float xneg = (LfCenter + Lf_m[xl]) * (LtCenter - Lt_m[xl]);
            float ypos = (Lf_h[xm] + LfCenter) * (Lt_h[xm] - LtCenter);
            float yneg = (LfCenter + Lf_l[xm]) * (LtCenter - Lt_l[xm]);
            float result = LtCenter + 0.5f * stepsize * (xpos - xneg + ypos - yneg);

            Lstep_dst[xm] = result;
        }
    }
}

/**********************************************************************************************************************/
template <vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint16 VEC_NUM, vx_uint16 TEST_CYCLES>
vx_bool TestFED(const float diff_limit_8, const float diff_limit_16) {

    const vx_uint16 NUMBER_OF_BINS = 5;

    vx_uint64 *DifBin8 = new vx_uint64[NUMBER_OF_BINS]();
    vx_uint64 *DifBin16 = new vx_uint64[NUMBER_OF_BINS]();

    // constants
    const vx_uint16 KERN_SIZE = 3; // supported: 3, 5, 7, 9 and 11
    const vx_uint8 OMAX = 3;
    const vx_uint8 NSUBLEVELS = 4;
    const vx_uint8 FED_MAX_NSTEPS = 16;
    const vx_uint8 EVOLUTIONS = OMAX * NSUBLEVELS;

    const float TAU_SW = 0.0643982589f;
    const vx_uint32 TAU_HW = vx_uint32(0.0643982589f * (1 << 16));

    const vx_border_e BORDER_TYPE = VX_BORDER_REPLICATE; // VX_BORDER_REPLICATE, VX_BORDER_CONSTANT, VX_BORDER_UNDEFINED

    const vx_uint32 BORDER_RADIUS = (BORDER_TYPE == VX_BORDER_UNDEFINED) ? (KERN_SIZE / 2) : (0);

    const vx_uint32 NUMBER_OF_PIXELS = HEIGHT * WIDTH;

    deviation difMax_8;
    deviation difMax_16;

    vx_bool forward8 = vx_true_e;
    vx_bool forward16 = vx_true_e;

    vx_uint8 *input8_hw[2];
    float *input8_sw[2];
    float *output8_sw;
    vx_uint8 *output8_hw[2];

    vx_uint16 *input16_hw[2];
    float *input16_sw[2];
    float *output16_sw;
    vx_uint16 *output16_hw[2];

    /******** Allocate memory ***********************************************************************/

    input8_sw[0] = new float[NUMBER_OF_PIXELS];
    input8_sw[1] = new float[NUMBER_OF_PIXELS];
    output8_sw = new float[NUMBER_OF_PIXELS];

    input16_sw[0] = new float[NUMBER_OF_PIXELS];
    input16_sw[1] = new float[NUMBER_OF_PIXELS];
    output16_sw = new float[NUMBER_OF_PIXELS];

    input8_hw[0] = new vx_uint8[NUMBER_OF_PIXELS];
    input8_hw[1] = new vx_uint8[NUMBER_OF_PIXELS];
    output8_hw[0] = new vx_uint8[NUMBER_OF_PIXELS];
    output8_hw[1] = new vx_uint8[NUMBER_OF_PIXELS];

    input16_hw[0] = new vx_uint16[NUMBER_OF_PIXELS];
    input16_hw[1] = new vx_uint16[NUMBER_OF_PIXELS];
    output16_hw[0] = new vx_uint16[NUMBER_OF_PIXELS];
    output16_hw[1] = new vx_uint16[NUMBER_OF_PIXELS];

    /******** Create image ******************************************************************/

    std::cout << "Test of FED function\n"
              << "--------------------\n";
    std::cout << "Resolution: " << WIDTH << "x" << HEIGHT << "\n";
    std::cout << "Vector size: " << VEC_NUM << "\n";

    for (vx_uint16 i = 0; i < TEST_CYCLES; i++) {

        std::cout << "Cycle " << i + 1 << " of " << TEST_CYCLES << "\n";

        deviation absDif;

        for (vx_uint32 i = 0; i < NUMBER_OF_PIXELS; i++) {
            const vx_uint8 pixel_1 = rand() % 256;
            const vx_uint8 pixel_2 = rand() % 256;
            input8_hw[0][i] = pixel_1;
            input8_sw[0][i] = fractionToFloat<vx_uint8>(pixel_1);
            input8_hw[1][i] = pixel_2;
            input8_sw[1][i] = fractionToFloat<vx_uint8>(pixel_2);
        }

        for (vx_uint32 i = 0; i < NUMBER_OF_PIXELS; i++) {
            const vx_uint16 pixel_1 = rand() % 65536;
            const vx_uint16 pixel_2 = rand() % 65536;
            input16_hw[0][i] = pixel_1;
            input16_sw[0][i] = fractionToFloat<vx_uint16>(pixel_1);
            input16_hw[1][i] = pixel_2;
            input16_sw[1][i] = fractionToFloat<vx_uint16>(pixel_2);
        }

        std::cout << "8 Bit: ";

        SwFED<WIDTH, HEIGHT>(input8_sw[0], input8_sw[1], output8_sw, TAU_SW);
        ImgFastExplicitDiffusion<vx_uint8, VEC_NUM, WIDTH, HEIGHT, TAU_HW, BORDER_TYPE>(input8_hw[0], input8_hw[1],
                                                                                        output8_hw[0], output8_hw[1]);

        absDif = CheckError<vx_uint8, WIDTH, HEIGHT, BORDER_RADIUS, NUMBER_OF_BINS>(output8_sw, output8_hw[0], DifBin8,
                                                                                    diff_limit_8, vx_true_e);
        difMax_8.update(absDif);

        // check if forwarding works for 8 bit in-/output
        if (memcmp(input8_hw[1], output8_hw[1], sizeof(vx_uint8) * NUMBER_OF_PIXELS) != 0) {
            std::cout << "Error in forwarding of 8 bit conduction coefficient matrix!\n";
            forward8 = vx_false_e;
        }

        std::cout << "16 Bit: ";

        SwFED<WIDTH, HEIGHT>(input16_sw[0], input16_sw[1], output16_sw, TAU_SW);
        ImgFastExplicitDiffusion<vx_uint16, VEC_NUM, WIDTH, HEIGHT, TAU_HW, BORDER_TYPE>(
            input16_hw[0], input16_hw[1], output16_hw[0], output16_hw[1]);

        absDif = CheckError<vx_uint16, WIDTH, HEIGHT, BORDER_RADIUS, NUMBER_OF_BINS>(
            output16_sw, output16_hw[0], DifBin16, diff_limit_16, vx_true_e);
        difMax_16.update(absDif);

        // check if forwarding works for 16 bit in-/output
        if (memcmp(input16_hw[1], output16_hw[1], sizeof(vx_uint8) * NUMBER_OF_PIXELS) != 0) {
            std::cout << "Error in forwarding of 16 bit conduction coefficient matrix!\n";
            forward16 = vx_false_e;
        }
    }

    std::cout << "------------------------------------\n"
              << "Absolute difference over all cycles.\n"
              << "------------------------------------\n";
    std::cout << "8 Bit: " << std::fixed << std::setprecision(8) << difMax_8.abs_max << "\n";
    std::cout << "Positive deviation: " << difMax_8.pos_max << "\n";
    std::cout << "Negative deviation: " << difMax_8.neg_max << "\n";
    printDifBin<WIDTH, HEIGHT, TEST_CYCLES, NUMBER_OF_BINS>(DifBin8, diff_limit_8);
    std::cout << "16 Bit: " << difMax_16.abs_max << "\n";
    std::cout << "Positive deviation: " << difMax_16.pos_max << "\n";
    std::cout << "Negative deviation: " << difMax_16.neg_max << "\n";
    printDifBin<WIDTH, HEIGHT, TEST_CYCLES, NUMBER_OF_BINS>(DifBin16, diff_limit_16);

    const vx_bool success =
        (difMax_8.abs_max <= diff_limit_8) && (difMax_16.abs_max <= diff_limit_16) && forward8 && forward16;

    // Free Memory
    delete[] input8_sw[0];
    delete[] input8_hw[0];
    delete[] input8_sw[1];
    delete[] input8_hw[1];
    delete[] output8_sw;
    delete[] output8_hw[0];
    delete[] output8_hw[1];

    delete[] input16_sw[0];
    delete[] input16_hw[0];
    delete[] input16_sw[1];
    delete[] input16_hw[1];
    delete[] output16_sw;
    delete[] output16_hw[0];
    delete[] output16_hw[1];

    delete[] DifBin8;
    delete[] DifBin16;

    return success;
}

/**********************************************************************************************************************/
void sw_test_fed() {

    // test configurations
    const vx_uint16 TEST_WIDTH = 1920;
    const vx_uint16 TEST_HEIGHT = 1080;
    const vx_uint16 TEST_CYCLES = 10;

    // maximum threshold for deviation if this threshold is reached than does the test function return false and stops
    // the whole testing process
    const float max_deviation_8_fed = 0.0078125;          // 1/2^7
    const float max_deviation_16_fed = 3.0517578125e-05f; // 1/2^15

    checkSuccess(TestFED<TEST_WIDTH, TEST_HEIGHT, 1, TEST_CYCLES>(max_deviation_8_fed, max_deviation_16_fed));
    checkSuccess(TestFED<TEST_WIDTH, TEST_HEIGHT, 2, TEST_CYCLES>(max_deviation_8_fed, max_deviation_16_fed));
    checkSuccess(TestFED<TEST_WIDTH, TEST_HEIGHT, 4, TEST_CYCLES>(max_deviation_8_fed, max_deviation_16_fed));
    checkSuccess(TestFED<TEST_WIDTH, TEST_HEIGHT, 8, TEST_CYCLES>(max_deviation_8_fed, max_deviation_16_fed));
}

/**********************************************************************************************************************/
template <typename Type, const vx_uint32 WIDTH, const vx_uint32 HEIGHT, const vx_uint32 KERN_SIZE,
          const vx_border_e BORDER_TYPE, Type THRESH_LOWER, Type THRESH_UPPER>
void SwHysteresis(Type *src, Type *dst) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;
    const Type max_value = ComputeMax<Type>();

    // Compute  function
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    dst[y * WIDTH + x] = src[y * WIDTH + x];
                    continue;
                }
            }

            // Variables
            vx_int32 yi = 0, xj = 0;
            vx_int64 input = 0;

            // Input
            vx_int64 pixel = src[y * WIDTH + x];

            // Check the strength of the input
            bool strong = (pixel >= THRESH_UPPER);
            bool weak = (pixel >= THRESH_LOWER);

            // Compute  value
            for (vx_int32 i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

                    // Update  value
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        input = src[yi * WIDTH + xj];
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        yi = i + y;
                        xj = j + x;
                        input = src[yi * WIDTH + xj];
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        yi = i + y;
                        xj = j + x;
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            input = src[yi * WIDTH + xj];
                        } else {
                            input = 0;
                        }
                    }

                    if (!(y == 0 && x == 0)) {
                        if ((input >= THRESH_UPPER) && (weak == true)) {
                            strong = true;
                        }
                    }
                }
            }

            // Write back result
            dst[y * WIDTH + x] = (strong == true) ? (max_value) : ((Type)42);
        }
    }
}

/**********************************************************************************************************************/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_border_e BORDER_TYPE>
void SwSegmentTestDetector(Type *src, Type *dst) {

    // Constants
    const vx_int32 KERN_RAD = 3;
    const Type max_value = std::numeric_limits<Type>::max();
    const Type min_value = std::numeric_limits<Type>::min();

    // Bresham circle
    Type circle[16] = {0};
    const vx_int8 coordinates[16][2] = {{-3, 0}, {-3, 1}, {-2, 2}, {-1, 3}, {0, 3},  {1, 3},   {2, 2},   {3, 1},
                                        {3, 0},  {3, -1}, {2, -2}, {1, -3}, {0, -3}, {-1, -3}, {-2, -2}, {-3, -1}};

    // Compute  function
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    dst[y * WIDTH + x] = src[y * WIDTH + x];
                    continue;
                }
            }

            // input/output
            vx_float32 pixel = (vx_float32)src[y * WIDTH + x];
            vx_float32 response = 0;

            // Get input data
            for (vx_int32 i = 0; i < 16; ++i) {
                vx_int32 yi = coordinates[i][0] + y;
                vx_int32 xj = coordinates[i][1] + x;
                Type value = 0;

                if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                    yi = MIN(MAX(yi, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                    xj = MIN(MAX(xj, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                    value = src[yi * WIDTH + xj];
                } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                    value = src[yi * WIDTH + xj];
                } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                    if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                        value = src[yi * WIDTH + xj];
                    } else {
                        value = 0;
                    }
                }
                circle[i] = value;
            }

            // Compute
            for (vx_int32 i = 0; i < 16; ++i) {
                vx_float32 min_diff = FLT_MAX;
                bool bigger = true;
                bool smaller = true;
                for (vx_int32 j = i; j < (i + 9); ++j) {
                    vx_uint8 ptr = j % 16;
                    vx_float32 value = (vx_float32)circle[ptr];
                    vx_float32 diff = ABS(value - pixel);
                    min_diff = MIN(diff, min_diff);
                    if (pixel <= value)
                        bigger = false;
                    if (pixel >= value)
                        smaller = false;
                }
                if (bigger == true || smaller == true)
                    response = MAX(response, min_diff);
            }

            // Write back result
            dst[y * WIDTH + x] = (Type)response; // MAX(MIN(response, max_value), min_value);
        }
    }
}

/**********************************************************************************************************************/
/*! \brief Compute the Non-max supression for 1 pixel */
template <typename Type, vx_int32 KERN_RAD> bool CheckNonMax(vx_int32 y, vx_int32 x, Type pixel, Type input) {

    bool maximum = false;

    // Check top left
    if ((y < KERN_RAD) || ((y == KERN_RAD) && (x < KERN_RAD))) {
        if (input < pixel)
            maximum = false;

        // Check bottom right
    } else if ((y > KERN_RAD) || ((y == KERN_RAD) && (x > KERN_RAD))) {
        if (input <= pixel)
            maximum = false;
    }

    return maximum;
}

/**********************************************************************************************************************/
template <typename Type, const vx_int32 WIDTH, const vx_int32 HEIGHT, const vx_int32 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwNonMaximaSuppression(Type *src, Type *dst) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;
    const Type min_value = ComputeMin<Type>();

    // Compute  function
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    dst[y * WIDTH + x] = src[y * WIDTH + x];
                    continue;
                }
            }

            // Variables
            vx_int32 yi = 0, xj = 0;
            Type input = 0;
            bool maximum = true;

            // Input
            Type pixel = src[y * WIDTH + x];

            // Compute  value
            for (vx_int32 i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

                    // Update  value
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        input = src[yi * WIDTH + xj];
                        maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);

                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        yi = i + y;
                        xj = j + x;
                        input = src[yi * WIDTH + xj];
                        maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);

                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        yi = i + y;
                        xj = j + x;
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            input = src[yi * WIDTH + xj];
                            maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);
                        } else {
                            maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, (Type)0);
                        }
                    }
                }
            }

            // Write back result
            dst[y * WIDTH + x] = (maximum == false) ? (min_value) : (input);
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Implements the Sobel Image Filter Kernel.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output_x     The output x image
@param output_y     The output y image
*/
template <typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwSobel(InType *input, OutType *output_x, OutType *output_y) {

    // Kernel Type
    typedef vx_int16 KernelTypeT;

    // Kernel Parameters
    const vx_uint16 KERN_NUM = 2;
    const vx_int32 KERN_RAD = KERN_SIZE / 2;

    // Normalization
    float norm = 0.0;

    // Function Input
    KernelTypeT kernel[KERN_NUM][KERN_SIZE][KERN_SIZE];

    // Compute kernel and normalization (Compile-Time)
    ComputeSobelKernels<KernelTypeT, KERN_SIZE>(kernel[0], kernel[1]);

    // Normalization
    for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
        for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
            norm += static_cast<float>(abs(kernel[0][y][x]));
        }
    }

    // Compute Sobel filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output_x[y * WIDTH + x] = input[y * WIDTH + x];
                    output_y[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single custom filter
            float sum_x = 0.0f, sum_y = 0.0f;
            ;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                        float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                        sum_x += (A * B);
                        sum_y += (A * C);
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
                            float A = static_cast<float>(input[yi * WIDTH + xj]);
                            float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                            float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                            sum_x += (A * B);
                            sum_y += (A * C);
                        }
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                        float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                        sum_x += (A * B);
                        sum_y += (A * C);
                    }
                }
            }

            // Store result
            output_x[y * WIDTH + x] = static_cast<OutType>((sum_x / norm) + 0.5f);
            output_y[y * WIDTH + x] = static_cast<OutType>((sum_y / norm) + 0.5f);
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Implements the Scharr Image Filter Kernel.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output_x     The output x image
@param output_y     The output y image
*/
template <typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT,
          const vx_border_e BORDER_TYPE>
void SwScharr3x3(InType *input, OutType *output_x, OutType *output_y) {

    // Kernel Type
    typedef vx_int16 KernelTypeT;

    // Kernel Parameters
    const vx_uint16 KERN_NUM = 2;
    const vx_uint16 KERN_SIZE = 3;
    const vx_int32 KERN_RAD = KERN_SIZE / 2;
    const float NORM = 32;

    // Function Input
    const KernelTypeT kernel[KERN_NUM][KERN_SIZE][KERN_SIZE] = {{
                                                                    {-3, 0, 3},
                                                                    {-10, 0, 10},
                                                                    {-3, 0, 3},
                                                                },
                                                                {
                                                                    {-3, -10, -3},
                                                                    {0, 0, 0},
                                                                    {3, 10, 3},
                                                                }};

    // Compute Scharr filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output_x[y * WIDTH + x] = input[y * WIDTH + x];
                    output_y[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single custom filter
            float sum_x = 0.0f, sum_y = 0.0f;
            ;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                        float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                        sum_x += (A * B);
                        sum_y += (A * C);
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
                            float A = static_cast<float>(input[yi * WIDTH + xj]);
                            float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                            float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                            sum_x += (A * B);
                            sum_y += (A * C);
                        }
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
                        float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
                        sum_x += (A * B);
                        sum_y += (A * C);
                    }
                }
            }

            // Store result
            output_x[y * WIDTH + x] = static_cast<OutType>(sum_x / NORM + 0.5f);
            output_y[y * WIDTH + x] = static_cast<OutType>(sum_y / NORM + 0.5f);
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Computes a median pixel value over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwMedian(Type *input, Type *output) {

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE / 2;
    const vx_uint16 WIN_SIZE = KERN_SIZE * KERN_SIZE;
    const vx_uint16 MEDIAN = WIN_SIZE / 2;

    // Window
    Type data[WIN_SIZE];

    // Compute Median filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single Median filter
            vx_uint16 ptr = 0;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        data[ptr] = input[yi * WIDTH + xj];
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT))
                            data[ptr] = input[yi * WIDTH + xj];
                        else
                            data[ptr] = 0;
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        data[ptr] = input[yi * WIDTH + xj];
                    }
                    ptr++;
                }
            }
            std::sort(data, data + WIN_SIZE);

            // Store result
            output[y * WIDTH + x] = data[MEDIAN];
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Computes a Gaussian filter over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwGaussian(Type *input, Type *output) {

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE / 2;

    // Kernel Type
    typedef vx_uint16 KernelTypeT;

    // Normalization
    float norm = 0.0;

    // Kernel
    KernelTypeT kernel[KERN_SIZE][KERN_SIZE];

    // Compute kernel and normalization (Compile-Time)
    ComputeGaussianKernels<KernelTypeT, KERN_SIZE>(kernel);

    // Normalization
    for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
        for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
            norm += static_cast<float>(kernel[y][x]);
        }
    }

    // Compute gaussian filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single gaussian filter
            float sum = 0.0f;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                        sum += (A * B);
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
                            float A = static_cast<float>(input[yi * WIDTH + xj]);
                            float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                            sum += (A * B);
                        }
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                        sum += (A * B);
                    }
                }
            }

            // Store result
            output[y * WIDTH + x] = static_cast<Type>(sum / norm + 0.5f);
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Implements Dilation, which grows the white space in an image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwDilate(Type *input, Type *output) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;

    // Compute Dilate function
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Single Result
            Type result = std::numeric_limits<Type>::min();

            // Compute Dilate value
            for (int i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
                for (int j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

                    // Get kernel coordinates
                    int yi = i + y;
                    int xj = j + x;

                    // Update dilate value
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            Type value = input[yi * WIDTH + xj];
                            result = MAX(result, value);
                        }
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        Type value = input[yi * WIDTH + xj];
                        result = MAX(result, value);
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            Type value = input[yi * WIDTH + xj];
                            result = MAX(result, value);
                        } else {
                            result = MAX(result, (Type)0);
                        }
                    }
                }
            }

            // Write back result
            output[y * WIDTH + x] = result;
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Implements Erosion, which shrinks the white space in an image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwErode(Type *input, Type *output) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;

    // Compute Dilate function
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Single Result
            Type result = std::numeric_limits<Type>::max();

            // Compute Dilate value
            for (int i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
                for (int j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

                    // Get kernel coordinates
                    int yi = i + y;
                    int xj = j + x;

                    // Update dilate value
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            Type value = input[yi * WIDTH + xj];
                            result = MIN(result, value);
                        }
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        Type value = input[yi * WIDTH + xj];
                        result = MIN(result, value);
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
                            Type value = input[yi * WIDTH + xj];
                            result = MIN(result, value);
                        } else {
                            result = MIN(result, (Type)0);
                        }
                    }
                }
            }

            // Write back result
            output[y * WIDTH + x] = result;
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Convolves the input with the client supplied convolution matrix.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
@param output       The convolve kernel
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwConvolve(Type *input, Type *output, const Type kernel[KERN_SIZE][KERN_SIZE]) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;

    float NORM = 0.0f;
    for (vx_int32 i = 0; i < KERN_SIZE; i++) {
        for (vx_int32 j = 0; j < KERN_SIZE; j++) {
            NORM += kernel[i][j];
        }
    }
    NORM = 1.0f / NORM;

    // Compute custom filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single custom filter
            float sum = 0.0f;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

                    // Replicated border
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                        sum += (A * B);

                        // Constant border
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
                            float A = static_cast<float>(input[yi * WIDTH + xj]);
                            float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                            sum += (A * B);
                        }

                        // Data is passed through if filter exceeds border
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        float A = static_cast<float>(input[yi * WIDTH + xj]);
                        float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
                        sum += (A * B);
                    }
                }
            }

            // Store result
            output[y * WIDTH + x] = static_cast<Type>(sum * NORM + 0.5f);
        }
    }
}

/**********************************************************************************************************************/
/**
@brief  Computes a Box filter over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template <typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE,
          const vx_border_e BORDER_TYPE>
void SwBox(Type *input, Type *output) {

    // Constants
    const vx_int32 KERN_RAD = KERN_SIZE / 2;
    const float NORM = 1.0f / powf(KERN_SIZE, 2);

    // Compute box filter
    for (vx_int32 y = 0; y < HEIGHT; y++) {
        for (vx_int32 x = 0; x < WIDTH; x++) {

            // Data is passed through if filter exceeds border
            if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
                    output[y * WIDTH + x] = input[y * WIDTH + x];
                    continue;
                }
            }

            // Compute single box filter
            float sum = 0.0f;
            for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
                for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

                    // Replicated border
                    if (BORDER_TYPE == VX_BORDER_REPLICATE) {
                        vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
                        vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
                        sum += static_cast<float>(input[yi * WIDTH + xj]);

                        // Constant border
                    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT))
                            sum += static_cast<float>(input[yi * WIDTH + xj]);

                        // Data is passed through if filter exceeds border
                    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
                        vx_int32 yi = y + i;
                        vx_int32 xj = x + j;
                        sum += static_cast<float>(input[yi * WIDTH + xj]);
                    }
                }
            }

            // Store result
            output[y * WIDTH + x] = static_cast<Type>(sum * NORM + 0.5f);
        }
    }
}

/**********************************************************************************************************************/
/*! \brief Test all filter functions */
template <typename TypeUint, typename TypeInt, vx_uint8 VEC_NUM, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD,
          vx_uint16 FILTER_SIZE, vx_border_e BORDER_TYPE, TypeUint THRESH_LOWER, TypeUint THRESH_UPPER>
void SwFilters(TypeUint convolve_kernel[FILTER_SIZE][FILTER_SIZE], TypeUint *input, TypeUint *output_sw,
               TypeUint *output_hw, TypeInt *output_sw_x, TypeInt *output_sw_y, TypeInt *output_hw_x,
               TypeInt *output_hw_y) {

    const vx_uint32 BORDER_RADIUS = (BORDER_TYPE == VX_BORDER_UNDEFINED) ? (FILTER_SIZE / 2) : (0);
    const vx_uint32 BORDER_RADIUS_FAST = (BORDER_TYPE == VX_BORDER_UNDEFINED) ? (7 / 2) : (0);

    std::string name01 = "Box Filter:            ";
    std::string name02 = "Box Filter (sep):      ";
    std::string name03 = "Convolve Filter:       ";
    std::string name04 = "Erode Filter:          ";
    std::string name05 = "Dilate Filter:         ";
    std::string name06 = "Gaussian Filter:       ";
    std::string name07 = "Gaussian Filter (sep): ";
    std::string name08 = "Median Filter:         ";
    std::string name09 = "Scharr3x3 Filter (x):  ";
    std::string name10 = "Scharr3x3 Filter (y):  ";
    std::string name11 = "Sobel Filter (x):      ";
    std::string name12 = "Sobel Filter (y):      ";
    std::string name13 = "Non-max Suppression:   ";
    std::string name14 = "Segment Test Detector: ";
    std::string name15 = "Hysteresis:            ";

    // Non-max Suppression Mask
    vx_uint8 nms_mask[FILTER_SIZE][FILTER_SIZE];
    for (vx_int32 i = 0; i < FILTER_SIZE; ++i) {
        for (vx_int32 j = 0; j < FILTER_SIZE; ++j) {
            nms_mask[i][j] = 0;
        }
    }

    SwBox<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    ImgBox<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_false_e>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name01);
    ImgBox<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_true_e>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name02);

    if (sizeof(TypeUint) == 1 || sizeof(TypeUint) == 2) {
        SwConvolve<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw, convolve_kernel);
        ImgConvolve<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
            (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw, convolve_kernel);
        CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name03);
    }

    SwErode<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    ImgErode<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name04);

    SwDilate<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    ImgDilate<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name05);

    SwGaussian<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    ImgGaussian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_false_e>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name06);
    ImgGaussian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_true_e>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name07);

    SwMedian<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    ImgMedian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name08);

    if (FILTER_SIZE == 3) {
        SwScharr3x3<TypeUint, TypeInt, COLS_FHD, ROWS_FHD, BORDER_TYPE>(input, output_sw_x, output_sw_y);
        ImgScharr3x3<TypeUint, TypeInt, VEC_NUM, COLS_FHD, ROWS_FHD, (vx_uint8)1, (vx_uint8)0, BORDER_TYPE>(
            (vx_image_data<TypeUint, VEC_NUM> *)input,
            (vx_image_data<TypeInt, STEP_VEC_NUM_OUT(VEC_NUM, 0)> *)output_hw_x,
            (vx_image_data<TypeInt, STEP_VEC_NUM_OUT(VEC_NUM, 0)> *)output_hw_y);
        CheckError<TypeInt, COLS_FHD, ROWS_FHD, VEC_NUM, 3, BORDER_TYPE>(output_sw_x, output_hw_x, name09);
        CheckError<TypeInt, COLS_FHD, ROWS_FHD, VEC_NUM, 3, BORDER_TYPE>(output_sw_y, output_hw_y, name10);
    }

    SwSobel<TypeUint, TypeInt, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw_x, output_sw_y);
    ImgSobel<TypeUint, TypeInt, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeInt, VEC_NUM> *)output_hw_x,
        (vx_image_data<TypeInt, VEC_NUM> *)output_hw_y);
    CheckError<TypeInt, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw_x, output_hw_x, name11);
    CheckError<TypeInt, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw_y, output_hw_y, name12);

    ImgNonMaxSuppression<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw, nms_mask);
    SwNonMaximaSuppression<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name13);

    if (FILTER_SIZE == 7) {
        ImgSegmentTestDetector<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, BORDER_TYPE>(
            (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
        SwSegmentTestDetector<TypeUint, COLS_FHD, ROWS_FHD, BORDER_TYPE>(input, output_sw);
        CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, 7, BORDER_TYPE>(output_sw, output_hw, name14);
    }

    ImgHysteresis<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(
        (vx_image_data<TypeUint, VEC_NUM> *)input, (vx_image_data<TypeUint, VEC_NUM> *)output_hw);
    SwHysteresis<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output_sw);

    CheckError<TypeUint, COLS_FHD, ROWS_FHD, VEC_NUM, FILTER_SIZE, BORDER_TYPE>(output_sw, output_hw, name15);
}

/**********************************************************************************************************************/
template <typename imgUintT, typename imgIntT, vx_uint8 VEC_NUM, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD,
          vx_uint16 FILTER_SIZE, imgUintT THRESH_LOWER, imgUintT THRESH_UPPER>
void TestFilterBorder(imgUintT convolve_kernel[FILTER_SIZE][FILTER_SIZE], imgUintT *input, imgUintT *output_sw,
                      imgUintT *output_hw, imgIntT *output_sw_x, imgIntT *output_sw_y, imgIntT *output_hw_x,
                      imgIntT *output_hw_y) {

    SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_UNDEFINED, THRESH_LOWER,
              THRESH_UPPER>(convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x,
                            output_hw_y);
    SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_CONSTANT, THRESH_LOWER,
              THRESH_UPPER>(convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x,
                            output_hw_y);
    SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_REPLICATE, THRESH_LOWER,
              THRESH_UPPER>(convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x,
                            output_hw_y);
}

/**********************************************************************************************************************/
template <typename imgUintT, typename imgIntT, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD, vx_uint16 FILTER_SIZE,
          imgUintT THRESH_LOWER, imgUintT THRESH_UPPER>
void TestFilterVectorSize(imgUintT *input, imgUintT *output_sw, imgUintT *output_hw, imgIntT *output_sw_x,
                          imgIntT *output_sw_y, imgIntT *output_hw_x, imgIntT *output_hw_y) {

    // Convolution kernel for the HwConvolve filter
    imgUintT convolve_kernel[FILTER_SIZE][FILTER_SIZE];
    for (vx_uint16 i = 0; i < FILTER_SIZE; i++) {
        for (vx_uint16 j = 0; j < FILTER_SIZE; j++) {
            convolve_kernel[i][j] = static_cast<imgUintT>(rand() % 256);
        }
    }

    TestFilterBorder<imgUintT, imgIntT, 1, COLS_FHD, ROWS_FHD, FILTER_SIZE, THRESH_LOWER, THRESH_UPPER>(
        convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    TestFilterBorder<imgUintT, imgIntT, 2, COLS_FHD, ROWS_FHD, FILTER_SIZE, THRESH_LOWER, THRESH_UPPER>(
        convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    TestFilterBorder<imgUintT, imgIntT, 4, COLS_FHD, ROWS_FHD, FILTER_SIZE, THRESH_LOWER, THRESH_UPPER>(
        convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    // TestFilterBorder<imgUintT, imgIntT, 8, COLS_FHD, ROWS_FHD, FILTER_SIZE, THRESH_LOWER, THRESH_UPPER>(
    //    convolve_kernel, input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
}

/**********************************************************************************************************************/
template <typename imgUintT, typename imgIntT, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD, imgUintT THRESH_LOWER,
          imgUintT THRESH_UPPER>
void TestFilterKernelSize(imgUintT *input, imgUintT *output_sw, imgUintT *output_hw, imgIntT *output_sw_x,
                          imgIntT *output_sw_y, imgIntT *output_hw_x, imgIntT *output_hw_y) {

    TestFilterVectorSize<imgUintT, imgIntT, COLS_FHD, ROWS_FHD, 3, THRESH_LOWER, THRESH_UPPER>(
        input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    TestFilterVectorSize<imgUintT, imgIntT, COLS_FHD, ROWS_FHD, 5, THRESH_LOWER, THRESH_UPPER>(
        input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    TestFilterVectorSize<imgUintT, imgIntT, COLS_FHD, ROWS_FHD, 7, THRESH_LOWER, THRESH_UPPER>(
        input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
    // TestFilterVectorSize<imgUintT, imgIntT, COLS_FHD, ROWS_FHD, 9, THRESH_LOWER, THRESH_UPPER>(
    //    input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
}

/**********************************************************************************************************************/
template <typename imgUintT, typename imgIntT, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD> //
void TestFilterDatatype() {

    const vx_uint32 PIXELS_FHD = COLS_FHD * ROWS_FHD;
    const imgUintT THRESH_LOWER = 20;
    const imgUintT THRESH_UPPER = 40;

    imgUintT *input = new imgUintT[PIXELS_FHD];
    imgUintT *output_sw = new imgUintT[PIXELS_FHD];
    imgUintT *output_hw = new imgUintT[PIXELS_FHD];
    imgIntT *output_sw_x = new imgIntT[PIXELS_FHD];
    imgIntT *output_sw_y = new imgIntT[PIXELS_FHD];
    imgIntT *output_hw_x = new imgIntT[PIXELS_FHD];
    imgIntT *output_hw_y = new imgIntT[PIXELS_FHD];

    for (vx_uint32 i = 0; i < PIXELS_FHD; i++)
        input[i] = rand() % 256;

    TestFilterKernelSize<imgUintT, imgIntT, COLS_FHD, ROWS_FHD, THRESH_LOWER, THRESH_UPPER>(
        input, output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);

    // Free Memory
    delete[] input;
    delete[] output_sw;
    delete[] output_hw;
    delete[] output_sw_x;
    delete[] output_sw_y;
    delete[] output_hw_x;
    delete[] output_hw_y;
}

/**********************************************************************************************************************/
/*! \brief Test all filter functions */
void TestFilterMain(void) {

    /******** Constants to test the functions ********************************************************************/

    // Test parameters
    const vx_uint16 COLS_FHD = 640;
    const vx_uint16 ROWS_FHD = 480;

    /******** Test accelerated functions ************************************************************/
#if !defined(__LAST__) && !defined(__LAST__)

#pragma omp parallel sections
    {
#pragma omp section
        { TestFilterDatatype<vx_uint8, vx_int8, COLS_FHD, ROWS_FHD>(); }
#pragma omp section
        { TestFilterDatatype<vx_uint16, vx_int16, COLS_FHD, ROWS_FHD>(); }
#pragma omp section
        { TestFilterDatatype<vx_uint32, vx_int32, COLS_FHD, ROWS_FHD>(); }
    }

    sw_test_fed();
#endif
}

#endif /* SRC_IMG_FILTER_TEST_H_ */
