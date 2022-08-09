/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_pixelop_help.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These functions test the pixel operation functions
 */

#ifndef SRC_IMG_PIXELOP_TEST_H_
#define SRC_IMG_PIXELOP_TEST_H_

#include "img_pixelop_base.h"

/***************************************************************************************************************************************************/
/* SW Test Functions */
/***************************************************************************************************************************************************/

/*! \brief Check if pixelwise function was correct */
template <typename Intype, typename AcType, vx_uint32 PIXELS>
void TestPixel(Intype *inputA, AcType *inputB, AcType *outputA, AcType *outputB, std::string &name) {
    vx_int64 absDifMax = 0;
    for (vx_uint32 i = 0; i < PIXELS; i++) {
        vx_int64 absDif = abs(static_cast<vx_int64>(outputA[i]) - static_cast<vx_int64>(outputB[i]));
        absDifMax = std::max(absDifMax, absDif);
        if (absDif > 256) {
            vx_int64 A = static_cast<vx_int64>(inputA[i]);
            vx_int64 B = static_cast<vx_int64>(inputB[i]);
            vx_int64 C = static_cast<vx_int64>(outputA[i]);
            vx_int64 D = static_cast<vx_int64>(outputB[i]);
            printf("%4d: (%10lld, %10lld) - (%10lld, %10lld) - (%10lld), \n", i, A, B, C, D, absDif);
        }
    }
    std::cout << "    " << name.c_str() << "Max. absolut error: " << absDifMax << std::endl;
}

/*! \brief Check if pixelwise function was correct (also checks for overflow) */
template <typename Intype, typename AcType, vx_uint32 PIXELS, vx_uint32 MAX_VAL>
void TestPixelMax(Intype *inputA, AcType *inputB, AcType *outputA, AcType *outputB, std::string &name) {
    vx_int64 absDifMax = 0;
    for (vx_uint32 i = 0; i < PIXELS; i++) {
        vx_int64 in1 = static_cast<vx_int64>(outputA[i]);
        vx_int64 in2 = static_cast<vx_int64>(outputB[i]);
        vx_int64 absDif = abs(in1 - in2);
        if (absDif > (MAX_VAL / 2))
            absDif = MAX_VAL - absDif;
        absDifMax = MAX(absDifMax, absDif);
        if (absDif > 8) {
            vx_int64 A = static_cast<vx_int64>(inputA[i]);
            vx_int64 B = static_cast<vx_int64>(inputB[i]);
            vx_int64 C = static_cast<vx_int64>(outputA[i]);
            vx_int64 D = static_cast<vx_int64>(outputB[i]);
            printf("%4d: (%10lld, %10lld) - (%10lld, %10lld) - (%10lld), \n", i, A, B, C, D, absDif);
            int a = getchar();
        }
    }
    std::cout << "    " << name.c_str() << "Max. absolut error: " << absDifMax << std::endl;
}

/*! \brief Test pixelwise function */
template <typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE,
          vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE,
          vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_UPPER, ScalarType THRESH_LOWER, vx_uint32 ALPHA,
          vx_uint8 ORIENT_QUANTIZATION>
void TestPixelwiseA(ScalarType *inputA, ScalarType *inputB, ScalarType *outputA, ScalarType *outputB,
                    std::string &name) {

    // Check if signed
    const bool IS_SIGNED = std::numeric_limits<ScalarType>::is_signed;

    // Get (Max and Min)
    const double sat_min = static_cast<double>(ComputeMin<ScalarType>());
    const double sat_max = static_cast<double>(ComputeMax<ScalarType>());

    // Get (Max and Min)
    const vx_int64 MIN_VAL = static_cast<vx_int64>(ComputeMin<ScalarType>());
    const vx_int64 MAX_VAL = static_cast<vx_int64>(ComputeMax<ScalarType>());

    // Compute Software
    for (vx_uint32 i = 0; i < PIXELS; i++) {
        vx_int64 A = static_cast<vx_int64>(inputA[i]);
        vx_int64 B = static_cast<vx_int64>(inputB[i]);
        vx_float64 C = 0.0;
        vx_int64 D = 0, E = 0;

        // Computation including rounding
        if (OPERATION_TYPE == HIFLIPVX::COPY_DATA) {
            D = A;
        } else if (OPERATION_TYPE == HIFLIPVX::MAX) {
            D = std::max(A, B);
        } else if (OPERATION_TYPE == HIFLIPVX::MIN) {
            D = std::min(A, B);
        } else if (OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
            D = llabs(A - B);
        } else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
            D = A + B;
        } else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
            D = A - B;
        } else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {
            vx_uint64 A1 = static_cast<vx_uint64>(abs(A));
            vx_uint64 B1 = static_cast<vx_uint64>(abs(B));
            vx_float64 A2 = static_cast<vx_float64>(A1 * A1);
            vx_float64 B2 = static_cast<vx_float64>(B1 * B1);
            C = sqrt(A2 + B2);
            if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
                C += 0.5;
            D = static_cast<vx_int64>(C);
        } else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
            C = static_cast<vx_float64>(A) * static_cast<vx_float64>(B) * (static_cast<vx_float64>(SCALE) / pow(2, 16));
            if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
                C += 0.5;
            D = static_cast<vx_int64>(C);
        } else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY_CONSTANT) {
            C = static_cast<vx_float64>(A) * (static_cast<vx_float64>(SCALE) / pow(2, 16));
            if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
                C += 0.5;
            D = static_cast<vx_int64>(C);
        } else if (OPERATION_TYPE == HIFLIPVX::THRESHOLD) {
            if (IS_SIGNED == true) {
                if (THRESH_TYPE == VX_THRESHOLD_TYPE_BINARY)
                    D = (A > THRESH_LOWER) ? (-1) : (0);
                else if (THRESH_TYPE == VX_THRESHOLD_TYPE_RANGE)
                    D = ((A > THRESH_UPPER) || (A < THRESH_LOWER)) ? (0) : (-1);
            } else {
                if (THRESH_TYPE == VX_THRESHOLD_TYPE_BINARY)
                    D = (A > THRESH_LOWER) ? (MAX_VAL) : (0);
                else if (THRESH_TYPE == VX_THRESHOLD_TYPE_RANGE)
                    D = ((A > THRESH_UPPER) || (A < THRESH_LOWER)) ? (0) : (MAX_VAL);
            }
        } else if (OPERATION_TYPE == HIFLIPVX::WEIGHTED_AVERAGE) {
            vx_float64 alpha = (vx_float64)ALPHA / (vx_float64)(1 << 16);
            if (ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) {
                C = ((1.0 - alpha) * (vx_float64)B + alpha * (vx_float64)A);
                D = static_cast<vx_int64>(C);
            } else if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
                C = ((1.0 - alpha) * (vx_float64)B + alpha * (vx_float64)A);
                D = static_cast<vx_int64>(C + 0.5);
            }
        } else if (OPERATION_TYPE == HIFLIPVX::PHASE) {
            const vx_int32 ACCURACY = 16;
            const vx_float64 SHIFT = (vx_int32)1 << ACCURACY;
            const vx_float64 TURN_POS = SHIFT / (vx_float64)(2 << (vx_int32)ORIENT_QUANTIZATION);
            const vx_float64 TURN_NEG = SHIFT - TURN_POS;
            const vx_int32 QUANTIZATION = ACCURACY - (vx_int32)ORIENT_QUANTIZATION;
            const vx_float64 ROUNDING = (vx_int32)1 << (QUANTIZATION - (vx_int32)1);
            vx_float64 a1 = atan2CordicAccurate(A, B);                           // Compute angle
            vx_float64 a2 = (a1 / pow(2, (32 - ACCURACY))) - ROUNDING;           // Reduce Precision and round
            vx_float64 a3 = (a2 < 0.0) ? (a2 + SHIFT) : (a2);                    // Convert negative degree to positiv
            vx_float64 a4 = (a3 > TURN_NEG) ? (a3 - TURN_NEG) : (a3 + TURN_POS); // Setting 0 degree to east
            vx_float64 a5 = (a4 >= SHIFT) ? (0.0) : (a4);                        // Setting 360 degree to 0
            D = (vx_int64)((a5) / pow(2, QUANTIZATION));                         // Do quantization
        }

        // Conversion Policy
        if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
            if (D > MAX_VAL)
                E = MAX_VAL;
            else if (D < MIN_VAL)
                E = MIN_VAL;
            else
                E = D;
        } else {
            E = D;
        }
        outputA[i] = static_cast<ScalarType>(E);
    }

    // Compute Hardware
    if (OPERATION_TYPE == HIFLIPVX::COPY_DATA) {
        ImgCopy<ScalarType, VEC_SIZE, PIXELS>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                              (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::MAX) {
        ImgMax<ScalarType, VEC_SIZE, PIXELS>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                             (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                             (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::MIN) {
        ImgMin<ScalarType, VEC_SIZE, PIXELS>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                             (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                             (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
        ImgAbsDiff<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                                              (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                                              (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
        ImgAdd<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                                          (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                                          (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
        ImgSubtract<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                                               (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                                               (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {
        ImgMagnitude<ScalarType, ScalarType, VEC_SIZE, PIXELS, ROUND_POLICY>(
            (vx_image_data<ScalarType, VEC_SIZE> *)inputA, (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
            (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
        ImgMultiply<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY, ROUND_POLICY, SCALE>(
            (vx_image_data<ScalarType, VEC_SIZE> *)inputA, (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
            (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY_CONSTANT) {
        ImgMultiplyConstant<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY, ROUND_POLICY, SCALE>(
            (vx_image_data<ScalarType, VEC_SIZE> *)inputA, (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::THRESHOLD) {
        ImgThreshold<ScalarType, VEC_SIZE, PIXELS, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER>(
            (vx_image_data<ScalarType, VEC_SIZE> *)inputA, (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::WEIGHTED_AVERAGE) {
        ImgWeightedAverage<ScalarType, VEC_SIZE, PIXELS, ALPHA, ROUND_POLICY>(
            (vx_image_data<ScalarType, VEC_SIZE> *)inputA, (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
            (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    } else if (OPERATION_TYPE == HIFLIPVX::PHASE) {
        ImgPhase<ScalarType, VEC_SIZE, PIXELS, ORIENT_QUANTIZATION>((vx_image_data<ScalarType, VEC_SIZE> *)inputA,
                                                                    (vx_image_data<ScalarType, VEC_SIZE> *)inputB,
                                                                    (vx_image_data<ScalarType, VEC_SIZE> *)outputB);
    }

    // Test result
    if (OPERATION_TYPE == HIFLIPVX::PHASE) {
        const vx_uint32 MAX_VAL = 1 << ORIENT_QUANTIZATION;
        TestPixelMax<ScalarType, ScalarType, PIXELS, MAX_VAL>(inputA, inputB, outputA, outputB, name);
    } else {
        TestPixel<ScalarType, ScalarType, PIXELS>(inputA, inputB, outputA, outputB, name);
    }
}

/*! \brief Test all pixelwise functions dependent on policy */
template <typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, vx_convert_policy_e CONV_POLICY,
          vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_UPPER,
          ScalarType THRESH_LOWER, vx_uint32 ALPHA, vx_uint8 ORIENT_QUANTIZATION>
void TestPolicyA(ScalarType *inputA, ScalarType *inputB, ScalarType *outputA, ScalarType *outputB, std::string &name) {

    // Write name of upper test case
    std::cout << "  " << name.c_str() << std::endl;

    // Function test scenarios
    std::string name0 = "Copy: ";
    std::string name1 = "Max:  ";
    std::string name2 = "Min:  ";
    std::string name3 = "Abs:  ";
    std::string name4 = "Add:  ";
    std::string name5 = "Sub:  ";
    std::string name6 = "Mag:  ";
    std::string name7 = "Mul:  ";
    std::string name8 = "Thre: ";
    std::string name9 = "Weig: ";
    std::string name10 = "Orie: ";
    std::string name11 = "MulC: ";

    // Test functions for possible policies
    if ((ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) && (CONV_POLICY == VX_CONVERT_POLICY_WRAP)) {
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::COPY_DATA, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name0);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MAX, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name1);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MIN, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name2);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::THRESHOLD, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name8);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::WEIGHTED_AVERAGE, CONV_POLICY, ROUND_POLICY, SCALE,
                       THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA,
                                                                                            outputB, name9);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::PHASE, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB,
                                                                               name10);
    }
    if (ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) {
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, ROUND_POLICY, SCALE,
                       THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA,
                                                                                            outputB, name3);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, ROUND_POLICY, SCALE,
                       THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA,
                                                                                            outputB, name4);
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, ROUND_POLICY, SCALE,
                       THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA,
                                                                                            outputB, name5);
    }
    if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
        TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MAGNITUDE, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                       THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name6);
    }
    TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE,
                   THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name7);
    TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MULTIPLY_CONSTANT, CONV_POLICY, ROUND_POLICY, SCALE,
                   THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA,
                                                                                        outputB, name11);
}

/*! \brief Test all pixelwise functions for all possible policies for 1 data type */
template <typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE,
          ScalarType THRESH_UPPER, ScalarType THRESH_LOWER, vx_uint32 ALPHA, vx_uint8 ORIENT_QUANTIZATION>
void TestDataTypeA(std::string &name) {

    // Test images
    ScalarType *inputA = new ScalarType[PIXELS];
    ScalarType *inputB = new ScalarType[PIXELS];
    ScalarType *outputA = new ScalarType[PIXELS];
    ScalarType *outputB = new ScalarType[PIXELS];

    // Write test image data
    const vx_int64 MIN_VAL = static_cast<vx_int64>(ComputeMin<ScalarType>());
    const vx_int64 MAX_VAL = static_cast<vx_int64>(ComputeMax<ScalarType>());
    const double max_rand = static_cast<double>(RAND_MAX);
    const double min_val = static_cast<double>(MIN_VAL);
    const double max_val = static_cast<double>(MAX_VAL);
    double A, B, C, D;
    vx_int64 E, F;
    vx_int64 maximun = MAX_VAL;
    vx_int64 minimum = MIN_VAL;

    for (vx_uint32 i = 0; i < PIXELS; i++) {
        A = static_cast<double>(rand());
        B = A / max_rand;
        C = B * (max_val - min_val);
        D = C + min_val;
        E = static_cast<vx_int64>(D + 0.5);
        F = MAX(MIN(E, MAX_VAL), MIN_VAL);
        inputA[i] = static_cast<ScalarType>(F);
        maximun = MAX(F, maximun);
        minimum = MIN(F, minimum);
        A = static_cast<double>(rand());
        B = A / max_rand;
        C = B * (max_val - min_val);
        D = C + min_val;
        E = static_cast<vx_int64>(D + 0.5);
        F = MAX(MIN(E, MAX_VAL), MIN_VAL);
        inputB[i] = static_cast<ScalarType>(F);
        maximun = MAX(F, maximun);
        minimum = MIN(F, minimum);
    }

    // Write name of upper test case
    std::cout << "" << name.c_str();
    std::cout << " | input data range: [" << minimum << ".." << maximun << "]" << std::endl;

    // Policy test scenarios
    std::string name1 = "CONVERT_POLICY_WRAP and ROUND_POLICY_TO_ZERO:             ";
    std::string name2 = "CONVERT_POLICY_WRAP and ROUND_POLICY_TO_NEAREST_EVEN:     ";
    std::string name3 = "CONVERT_POLICY_SATURATE and ROUND_POLICY_TO_ZERO:         ";
    std::string name4 = "CONVERT_POLICY_SATURATE and ROUND_POLICY_TO_NEAREST_EVEN: ";

    // Test all policies for a data type
    TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, SCALE, THRESH_TYPE,
                THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name1);
    TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_NEAREST_EVEN, SCALE,
                THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB,
                                                                                     name2);
    TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_SATURATE, VX_ROUND_POLICY_TO_ZERO, SCALE, THRESH_TYPE,
                THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB, name3);
    TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_SATURATE, VX_ROUND_POLICY_TO_NEAREST_EVEN, SCALE,
                THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA, ORIENT_QUANTIZATION>(inputA, inputB, outputA, outputB,
                                                                                     name4);

    // Delete memory
    delete[] inputA;
    delete[] inputB;
    delete[] outputA;
    delete[] outputB;

    // Newline
    std::cout << std::endl;
}

/*! \brief Calls all Software Test Functions */
void SwTestPixelopMain(void) {

    // Image Size
    const vx_uint32 COLS = 256;
    const vx_uint32 ROWS = 256;
    const vx_uint32 PIXELS = COLS * ROWS;
    const vx_uint8 VEC_SIZE = 1;
    const vx_uint32 SCALE = static_cast<vx_uint32>(256);
    const vx_uint8 SHIFT = static_cast<vx_uint32>(8);     // 0 .. 15
    const vx_uint32 ALPHA = static_cast<vx_uint32>(1000); // 0 .. 65536
    const vx_threshold_type_e THRESH_TYPE =
        VX_THRESHOLD_TYPE_BINARY; // VX_THRESHOLD_TYPE_BINARY, VX_THRESHOLD_TYPE_RANGE
    const vx_uint32 THRESH_UPPER = 100;
    const vx_uint32 THRESH_LOWER = 20;
    const vx_uint8 ORIENT_QUANTIZATION = 8;

    std::cout << "Resolution:   " << COLS << " x " << ROWS << std::endl;
    std::cout << "Vector Size:  " << static_cast<vx_uint16>(VEC_SIZE) << std::endl;
    std::cout << "Scale (mult): " << SCALE << std::endl << std::endl;

    std::string name01 = "UNSIGNED INTEGER 8-Bit:  ";
    std::string name02 = "UNSIGNED INTEGER 16-Bit: ";
    std::string name03 = "UNSIGNED INTEGER 32-Bit: ";
    std::string name04 = "SIGNED INTEGER 8-Bit:    ";
    std::string name05 = "SIGNED INTEGER 16-Bit:   ";
    std::string name06 = "SIGNED INTEGER 32-Bit:   ";

#if !defined(__LAST__) && !defined(__LAST__)
    TestDataTypeA<vx_uint8, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name01);
    TestDataTypeA<vx_uint16, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name02);
    TestDataTypeA<vx_uint32, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name03);
    TestDataTypeA<vx_int8, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name04);
    TestDataTypeA<vx_int16, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name05);
    TestDataTypeA<vx_int32, PIXELS, VEC_SIZE, SCALE, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER, ALPHA,
                  ORIENT_QUANTIZATION>(name06);
#endif
}

#endif /* SRC_PIXELOP_TEST_H_ */
