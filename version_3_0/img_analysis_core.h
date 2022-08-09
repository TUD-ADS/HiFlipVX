/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_analysis_core.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are all core functions needed for the image analysis functions (Do not call functions from here)
 */

#ifndef SRC_IMG_ANALYSIS_CORE_H_
#define SRC_IMG_ANALYSIS_CORE_H_

#include "vx_helper.h"

/***************************************************************************************************************************************************/
/** These features allow for conditional graph flow in OpenVX, via support for a variety of operations between two
 * scalars.
 * @param SrcType           The data type of the input scalars
 * @param DstType           The data type of the output scalar
 * @param scalar_operation  The scalar operation enum
 * @param input1            First scalar operand
 * @param input2            Second scalar operand
 * @param output            Result of the scalar operation (DstType)(A op B)
 */
template <typename SrcType, typename DstType, vx_scalar_operation_e scalar_operation>
void ControlFlow(vx_image_data<SrcType, 1> input1[1], vx_image_data<SrcType, 1> input2[1],
                 vx_image_data<DstType, 1> output[1]) {
#pragma HLS INLINE

    // Get data type information
    const bool is_specialized_src = std::numeric_limits<SrcType>::is_specialized;
    const bool is_specialized_dst = std::numeric_limits<DstType>::is_specialized;
    const bool is_integer_src = std::numeric_limits<SrcType>::is_integer; // Not all operations are possible on floats
    const bool is_integer_dst = std::numeric_limits<DstType>::is_integer;
    const SrcType maximum = std::numeric_limits<SrcType>::max(); // division or modulus not possible

    // Compile time checks
    const bool logical = (scalar_operation == VX_SCALAR_OP_AND) ||   //
                         (scalar_operation == VX_SCALAR_OP_OR) ||    //
                         (scalar_operation == VX_SCALAR_OP_XOR) ||   //
                         (scalar_operation == VX_SCALAR_OP_NAND) ||  //
                         (scalar_operation == VX_SCALAR_OP_MODULUS); //
    const bool logical_float = (logical == true) && (is_integer_src == false);
    STATIC_ASSERT((logical_float == false), logical_operators_and_modulus_are_not_possible_on_float);
    STATIC_ASSERT((is_specialized_src == true) && (is_specialized_dst == true), the_data_type_is_not_known);

    // Read input
    vx_image_data<SrcType, 1> src1 = input1[0];
    vx_image_data<SrcType, 1> src2 = input2[0];
    SrcType A = src1.pixel[0];
    SrcType B = src2.pixel[0];
    SrcType C = 0;
    DstType D = 0;
    vx_image_data<DstType, 1> dst;

    // Compute
    if (scalar_operation == VX_SCALAR_OP_AND) {
        C = (A & B);
    } else if (scalar_operation == VX_SCALAR_OP_OR) {
        C = (A | B);
    } else if (scalar_operation == VX_SCALAR_OP_XOR) {
        C = (A ^ B);
    } else if (scalar_operation == VX_SCALAR_OP_NAND) {
        C = !(A & B);
    } else if (scalar_operation == VX_SCALAR_OP_EQUAL) {
        C = (A == B);
    } else if (scalar_operation == VX_SCALAR_OP_NOTEQUAL) {
        C = (A != B);
    } else if (scalar_operation == VX_SCALAR_OP_LESS) {
        C = (A < B);
    } else if (scalar_operation == VX_SCALAR_OP_LESSEQ) {
        C = (A <= B);
    } else if (scalar_operation == VX_SCALAR_OP_GREATER) {
        C = (A > B);
    } else if (scalar_operation == VX_SCALAR_OP_GREATEREQ) {
        C = (A >= B);
    } else if (scalar_operation == VX_SCALAR_OP_ADD) {
        C = (A + B);
    } else if (scalar_operation == VX_SCALAR_OP_SUBTRACT) {
        C = (A - B);
    } else if (scalar_operation == VX_SCALAR_OP_MULTIPLY) {
        C = (A * B);
    } else if (scalar_operation == VX_SCALAR_OP_DIVIDE) {
        C = (B == 0) ? (maximum) : (A / B);
    } else if (scalar_operation == VX_SCALAR_OP_MODULUS) {
        C = (B == 0) ? (0) : (A % B);
    } else if (scalar_operation == VX_SCALAR_OP_MIN) {
        C = MIN(A, B);
    } else if (scalar_operation == VX_SCALAR_OP_MAX) {
        C = MAX(A, B);
    }

    // integer = float op float (VX_ROUND_POLICY_TO_ZERO, VX_CONVERT_POLICY_SATURATE)
    if ((is_integer_dst == true) && (is_integer_src == false)) {
        const SrcType sat_max = static_cast<SrcType>(std::numeric_limits<DstType>::max());
        const SrcType sat_min = static_cast<SrcType>(std::numeric_limits<DstType>::min());
        D = static_cast<DstType>(MIN(MAX(C, sat_min), sat_max));

        // Otherwise VX_CONVERT_POLICY_WRAP
    } else {
        D = static_cast<DstType>(C);
    }

    // Set user(SOF) & last(EOF)
    GenerateDmaSignal<DstType, 1>(true, true, dst);

    // Write output
    dst.pixel[0] = D;
    output[0] = dst;
}

/***************************************************************************************************************************************************/
// Reset the two histogram entries
template <typename DataType, vx_uint8 VEC_NUM, vx_uint32 BINS> //
void ResetHistograms(DataType LUTS[2 * VEC_NUM][BINS]) {       //
#pragma HLS INLINE

    for (vx_uint32 i = 0; i < BINS; ++i) {
#pragma HLS PIPELINE II = 1
        for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
            LUTS[2 * v][i] = 0;
            LUTS[2 * v + 1][i] = 0;
        }
    }
}
template <typename T, vx_uint32 BINS>             //
void ResetHistogram(T LUTA[BINS], T LUTB[BINS]) { //
#pragma HLS INLINE

    for (vx_uint32 i = 0; i < BINS; i++) {
#pragma HLS PIPELINE II = 1
        LUTA[i] = 0;
        LUTB[i] = 0;
    }
}

/***************************************************************************************************************************************************/
// Cumulative distribution function
template <vx_uint32 IMG_PIXELS, vx_uint32 BINS>
void EqualizeHistDistribution(vx_uint32 LUTA[BINS], vx_uint32 LUTB[BINS], vx_uint32 HIST[BINS], vx_uint8 min_bin) {
#pragma HLS INLINE

    // Constants
    const vx_uint8 MAX_VAL = std::numeric_limits<vx_uint8>::max();
    const vx_uint32 SHIFT = 24;

    // Pre-compute parts of the cdf computation
    const vx_uint32 cdf_min_ = LUTA[min_bin] + LUTB[min_bin];
    const vx_uint32 divisor_ = (IMG_PIXELS - cdf_min_); // TODO: show that IMG_PIXELS > cdf_min_
    const vx_uint32 dividend_ = ((vx_uint32)(MAX_VAL)) << SHIFT;
    const vx_uint64 multiplicant_ = (vx_uint64)(dividend_ / divisor_);

    // Compute Equalized histogram
    for (vx_uint32 i = 0, cdf_i = 0; i < BINS; i++) {
#pragma HLS PIPELINE II = 1

        // Cumulative distribution function
        cdf_i += LUTA[i] + LUTB[i];

        // Compute equalized histogram function
        vx_uint64 cdf = (vx_uint64)(cdf_i - cdf_min_);
        vx_uint32 eqhist_i = (vx_uint32)((cdf * multiplicant_) >> SHIFT);

        // Store result in buffer
        HIST[i] = static_cast<vx_uint32>(eqhist_i);
    }
}

/***************************************************************************************************************************************************/
/** @brief Equalizes the histogram of a grayscale image.
 * @param IMG_PIXELS The amount of pixels for input and output image
 * @param input1     The grayscale input image in VX_DF_IMAGE_U8 (1. read)
 * @param input2     The grayscale input image in VX_DF_IMAGE_U8 (2. read)
 * @param output     The grayscale output image of type vx_uint8 with equalized brightness and contrast and same size as
 * the input image.
 */
template <vx_uint32 IMG_PIXELS>
void EqualizeHist(vx_image_data<vx_uint8, 1> input1[IMG_PIXELS], vx_image_data<vx_uint8, 1> input2[IMG_PIXELS],
                  vx_image_data<vx_uint32, 1> output[IMG_PIXELS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 BINS = 256;

    // Variables
    vx_uint8 pre_bin = 0, cur_bin = 0;
    vx_uint32 pre_val = 0, cur_val = 0;
    vx_uint8 min_bin = std::numeric_limits<vx_uint8>::max();

    // Buffer Histogram in BRAM
    vx_uint32 LUTA[BINS], LUTB[BINS], HIST[BINS];

    // Reset histogram
    ResetHistogram<vx_uint32, BINS>(LUTA, LUTB);

    // Compute Histogram
    for (vx_uint32 i = 0; i < IMG_PIXELS + 1; i++) {
#pragma HLS PIPELINE II = 1

        // Gett the input data
        if (i < IMG_PIXELS) {
            vx_image_data<vx_uint8, 1> src = input1[i];
            cur_bin = src.pixel[0];
        }

        // Compute minimum bin entry
        min_bin = MIN(min_bin, cur_bin);

        // Read current bin & write previous bin
        if (i % 2 == 0) {
            cur_val = LUTA[cur_bin];
            LUTB[pre_bin] = pre_val;
        } else {
            cur_val = LUTB[cur_bin];
            LUTA[pre_bin] = pre_val;
        }

        // Update to store in next iteration
        cur_val++;
        pre_bin = cur_bin;
        pre_val = cur_val;
    }

    // Cumulative distribution function
    EqualizeHistDistribution<IMG_PIXELS, BINS>(LUTA, LUTB, HIST, min_bin);

    // Write equalized image
    for (vx_uint32 i = 0; i < IMG_PIXELS; ++i) {
#pragma HLS PIPELINE II = 1
        vx_image_data<vx_uint32, 1> dst;

        // Read input
        vx_image_data<vx_uint8, 1> src = input2[i];

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<vx_uint32, 1>((i == 0), (i == IMG_PIXELS - 1), dst);

        // Write output
        dst.pixel[0] = HIST[src.pixel[0]];
        output[i] = dst;
    }
}

/***************************************************************************************************************************************************/
/** @brief Equalizes the histogram of a grayscale image.
 * @param IMG_PIXELS The amount of pixels for input and output image
 * @param input1     The grayscale input image in VX_DF_IMAGE_U8 (1. read)
 * @param input2     The grayscale input image in VX_DF_IMAGE_U8 (2. read)
 * @param output     The grayscale output image of type vx_uint8 with equalized brightness and contrast and same size as
 * the input image.
 */
template <vx_uint32 IMG_PIXELS>                                                                             //
void EqualizeHist(vx_uint8 input1[IMG_PIXELS], vx_uint8 input2[IMG_PIXELS], vx_uint32 output[IMG_PIXELS]) { //
#pragma HLS INLINE

    // Constants
    const vx_uint32 BINS = 256;

    // Variables
    vx_uint8 pre_bin = 0, cur_bin = 0;
    vx_uint32 pre_val = 0, cur_val = 0;
    vx_uint8 min_bin = std::numeric_limits<vx_uint8>::max();

    // Buffer Histogram in BRAM
    vx_uint32 LUTA[BINS], LUTB[BINS], HIST[BINS];

    // Reset histogram
    ResetHistogram<vx_uint32, BINS>(LUTA, LUTB);

    // Compute Histogram
    for (vx_uint32 i = 0; i < IMG_PIXELS + 1; i++) {
#pragma HLS PIPELINE II = 1

        // Gett the input data
        if (i < IMG_PIXELS) {
            cur_bin = input1[i];
        }

        // Compute minimum bin entry
        min_bin = MIN(min_bin, cur_bin);

        // Read current bin & write previous bin
        if (i % 2 == 0) {
            cur_val = LUTA[cur_bin];
            LUTB[pre_bin] = pre_val;
        } else {
            cur_val = LUTB[cur_bin];
            LUTA[pre_bin] = pre_val;
        }

        // Update to store in next iteration
        cur_val++;
        pre_bin = cur_bin;
        pre_val = cur_val;
    }

    //
    EqualizeHistDistribution<IMG_PIXELS, BINS>(LUTA, LUTB, HIST, min_bin);

    // Write equalized image
    for (vx_uint32 i = 0; i < IMG_PIXELS; ++i) {
#pragma HLS PIPELINE II = 1
        output[i] = HIST[input2[i]];
    }
}

/***************************************************************************************************************************************************/
// Compile time checks for histogram function
template <typename SrcType, vx_uint32 IMG_ROWS, vx_uint32 IMG_COLS, vx_uint8 VEC_NUM, //
          vx_uint32 DIST_BINS, vx_uint32 DIST_RANGE, SrcType DIST_OFFSET>             //
void HistogramCompileTimeChecks() {
#pragma HLS INLINE

    // Check Datatype, MaximumValue, and that BIN is smaller RANGE
    const vx_type_e TYPE = GET_TYPE(SrcType);
    const bool allowed_type = (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16);
    const vx_uint32 MAX_VAL =
        (TYPE == VX_TYPE_UINT8) ? (static_cast<vx_uint32>(0x100)) : (static_cast<vx_uint32>(0x10000));
    const bool allowed_value1 = ((DIST_BINS <= MAX_VAL) && (DIST_RANGE <= MAX_VAL));
    const bool allowed_value2 = (static_cast<vx_uint32>(DIST_OFFSET) <= (MAX_VAL - static_cast<vx_uint32>(1)));
    const bool allowed_range = DIST_BINS <= DIST_RANGE;
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_histogram);
    STATIC_ASSERT(allowed_value1, BINS_RANGE_value_out_of_range_max_0x100_for_uint8_and_0x10000_for_uint16);
    STATIC_ASSERT(allowed_value2, OFFSET_value_out_of_range_max_0xff_for_uint8_and_0xffff_for_uint16);
    STATIC_ASSERT(allowed_range, RANGE_should_be_smaller_or_equal_to_BINS);
    STATIC_ASSERT(VEC_NUM == 1 || VEC_NUM == 2 || VEC_NUM == 4 || VEC_NUM == 8,
                  ONLY_1_2_4_8_CAN_BE_VECTORIZATION_FACTOR);
    STATIC_ASSERT(IMG_COLS % VEC_NUM == 0, VECTORIZATION_FACTOR_NEEDS_TO_DIVIDE_COLUMNS_WITHOUT_REST);
}

/***************************************************************************************************************************************************/
// Updates an entry of the histogram
template <typename SrcType, vx_uint32 IMG_PIXELS, vx_uint32 DIST_BINS, vx_uint32 DIST_RANGE, SrcType DIST_OFFSET> //
void HistogramUpdateEntry(vx_uint32 i, SrcType pixel, vx_uint32 LUTA[DIST_BINS], vx_uint32 LUTB[DIST_BINS],       //
                          vx_uint16 &pre_bin, vx_uint32 &pre_val, vx_bool ignore) {                               //
#pragma HLS INLINE

    vx_uint16 cur_bin = 0;
    vx_uint32 cur_val = 0;

    // Compute Scale (BINS / RANGE) "approximation"
    const vx_uint16 ACCURACY = 15;
    const vx_uint16 SCALE = static_cast<vx_uint16>((DIST_BINS << ACCURACY) / DIST_RANGE);

    // Subtract offset and compute current bin bin
    if (ignore == vx_false_e) {
        const SrcType index = pixel - DIST_OFFSET;

        if (DIST_BINS != DIST_RANGE)
            cur_bin =
                static_cast<vx_uint16>((static_cast<vx_uint32>(index) * static_cast<vx_uint32>(SCALE)) >> ACCURACY);
        else
            cur_bin = static_cast<vx_uint16>(index);
    }

    // Read current histogram entry & write previous histogram entry
    if (i % 2 == 0) {
        cur_val = LUTA[cur_bin];
        LUTB[pre_bin] = pre_val;
    } else {
        cur_val = LUTB[cur_bin];
        LUTA[pre_bin] = pre_val;
    }

    // Increment histogram value
    if (ignore == vx_false_e)
        ++cur_val;

    // Update to store in next iteration
    pre_bin = cur_bin;
    pre_val = cur_val;
}

/***************************************************************************************************************************************************/
/** @brief Generates a distribution from an image.
 * @param SrcType              The Input Type can be vx_uint8 and vx_uint16
 * @param IMG_PIXELS           The amount of pixels for input and output image
 * @param DIST_BINS    Indicates the number of bins.
 * @param DIST_RANG    Indicates the total number of the consecutive
 * values of the distribution interval.
 * @param DIST_OFFSET  Indicates the start of the values to use
 * (inclusive).
 * @param input                The input image (vx_uint8, vx_uint16)
 * @param output               The output image (vx_uint32)
 */
template <typename SrcType, vx_uint32 IMG_ROWS, vx_uint32 IMG_COLS, vx_uint8 VEC_NUM, //
          vx_uint32 DIST_BINS, vx_uint32 DIST_RANGE, SrcType DIST_OFFSET>             //
void Histogram(vx_image_data<SrcType, VEC_NUM> input[IMG_ROWS * IMG_COLS / VEC_NUM],  //
               vx_image_data<vx_uint32, 1> output[DIST_BINS]) {                       //
#pragma HLS INLINE

    const vx_uint32 IMG_VEC_COLS = IMG_COLS / VEC_NUM;
    const vx_uint32 IMG_VEC_PIXELS = IMG_ROWS * IMG_VEC_COLS;

    // Compile time checks for histogram function
    HistogramCompileTimeChecks<SrcType, IMG_ROWS, IMG_COLS, VEC_NUM, DIST_BINS, DIST_RANGE, DIST_OFFSET>();

    // Global variables
    vx_uint16 pre_bin[VEC_NUM];
#pragma HLS array_partition variable = pre_bin complete
    vx_uint32 pre_val[VEC_NUM];
#pragma HLS array_partition variable = pre_val complete

    // Buffer Histogram in BRAM
    vx_uint32 LUTS[2 * VEC_NUM][DIST_BINS];
#pragma HLS array_partition variable = LUTS complete dim = 1

    // Reset histogram
    ResetHistograms<vx_uint32, VEC_NUM, DIST_BINS>(LUTS);
    for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
        pre_bin[v] = 0;
        pre_val[v] = 0;
    }

    // Compute Histogram
    for (vx_uint32 y = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 xvec = 0; xvec < IMG_VEC_COLS; ++xvec) {
#pragma HLS PIPELINE II = 1

            const vx_uint32 ptr = y * IMG_VEC_COLS + xvec;
            vx_image_data<SrcType, VEC_NUM> src = input[ptr];

            for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
                SrcType pixel = src.pixel[v];

                // Check if bin will be out of range
                vx_bool ignore = vx_true_e;
                if ((DIST_OFFSET <= pixel) && (pixel <= (DIST_OFFSET + DIST_RANGE)) && (ptr < IMG_VEC_PIXELS))
                    ignore = vx_false_e;

                HistogramUpdateEntry<SrcType, IMG_VEC_PIXELS, DIST_BINS, DIST_RANGE, DIST_OFFSET> //
                    (ptr, pixel, LUTS[2 * v], LUTS[2 * v + 1], pre_bin[v], pre_val[v], ignore);   //
            }
        }
    }

    // Write output data
    for (vx_uint32 i = 0; i < DIST_BINS; i++) {
#pragma HLS PIPELINE II = VEC_NUM
        vx_image_data<vx_uint32, 1> dst;

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<vx_uint32, 1>((i == 0), (i == DIST_BINS - 1), dst);

        dst.pixel[0] = 0;

        for (vx_uint8 v = 0; v < VEC_NUM; ++v) {

            dst.pixel[0] += (LUTS[2 * v][i] + LUTS[2 * v + 1][i]);
        }

        output[i] = dst;
    }
}

/***************************************************************************************************************************************************/
/** @brief Generates a distribution from an image.
 * @param SrcType              The Input Type can be vx_uint8 and vx_uint16
 * @param IMG_PIXELS           The amount of pixels for input and output image
 * @param DIST_BINS    Indicates the number of bins.
 * @param DIST_RANG    Indicates the total number of the consecutive
 * values of the distribution interval.
 * @param DIST_OFFSET  Indicates the start of the values to use
 * (inclusive).
 * @param input                The input image (vx_uint8, vx_uint16)
 * @param output               The output image (vx_uint32)
 */
template <typename SrcType, vx_uint32 IMG_ROWS, vx_uint32 IMG_COLS, vx_uint8 VEC_NUM, //
          vx_uint32 DIST_BINS, vx_uint32 DIST_RANGE, SrcType DIST_OFFSET>             //
void Histogram(SrcType input[IMG_ROWS * IMG_COLS], vx_uint32 output[DIST_BINS]) {
#pragma HLS INLINE

    const vx_uint32 IMG_PIXELS = IMG_ROWS * IMG_COLS;

    // Compile time checks for histogram function
    HistogramCompileTimeChecks<SrcType, IMG_ROWS, IMG_COLS, VEC_NUM, DIST_BINS, DIST_RANGE, DIST_OFFSET>();

    // Global variables
    vx_uint16 pre_bin = 0;
    vx_uint32 pre_val = 0;

    // Buffer Histogram in BRAM
    vx_uint32 LUTS[2 * 1][DIST_BINS];
#pragma HLS array_partition variable = LUTS complete dim = 1

    // Reset histogram
    ResetHistograms<vx_uint32, VEC_NUM, DIST_BINS>(LUTS);

    // Compute Histogram
    for (vx_uint32 y = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            //
            const vx_uint32 ptr = y * IMG_COLS + x;
            SrcType pixel = input[ptr];

            // Check if bin will be out of range
            vx_bool ignore = vx_true_e;
            if ((DIST_OFFSET <= pixel) && (pixel <= (DIST_OFFSET + DIST_RANGE)) && (ptr < IMG_PIXELS))
                ignore = vx_false_e;

            HistogramUpdateEntry<SrcType, IMG_PIXELS, DIST_BINS, DIST_RANGE, DIST_OFFSET> //
                (ptr, pixel, LUTS[0], LUTS[1], pre_bin, pre_val, vx_false_e);             //
        }
    }

    // Write output data
    for (vx_uint32 i = 0; i < DIST_BINS; i++) {
#pragma HLS PIPELINE II = VEC_NUM
        for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
            output[i] += (LUTS[2 * v][i] + LUTS[2 * v + 1][i]);
        }
    }
}

/******************************************************************************/
/* Calculate Contrast Factor k */
/******************************************************************************/
template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint8 VEC_NUM, vx_uint16 KCONTRAST_PERCENTILE,
          vx_uint32 BORDER>
void ContrastFactor(DataType input[IMG_COLS * IMG_ROWS], vx_uint16 output[1]) {
#pragma HLS INLINE

    // Constants required for calculations dependant on input type
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool INPUT_IS_UINT8 = (TYPE == VX_TYPE_UINT8);
    const vx_uint16 BINS = (INPUT_IS_UINT8) ? (256) : (512);
    const vx_uint8 SHIFT = (INPUT_IS_UINT8) ? (0) : (7);
    const vx_uint8 DIGITS = (INPUT_IS_UINT8) ? (8) : (16);
    const vx_uint32 RANGE = (1 << 8 * sizeof(DataType));
    const vx_uint32 OFFSET = 0;
    const vx_uint32 IMG_PIXELS = IMG_ROWS * IMG_COLS;

    STATIC_ASSERT(VEC_NUM == 1, USE_THE_VX_IMAGE_DATA_TYPE_FOR_VEC_NUM_BIGGER_1);
    STATIC_ASSERT(TYPE == VX_TYPE_UINT8 || TYPE == VX_TYPE_UINT16, ONLY_AVAILABLE_FOR_UINT8_OR_UINT16_DATA);
    STATIC_ASSERT(BORDER < 2 * IMG_COLS && BORDER < 2 * IMG_ROWS, BORDER_LARGER_THAN_IMAGE);

    // Global variables
    vx_uint16 binthreshold = 0;
    vx_uint32 nthreshold = 0;
    vx_uint32 nelements = 0;
    vx_uint32 npoints = 0;
    vx_uint16 pre_bin = 0;
    vx_uint32 pre_val = 0;

    // Buffer Histogram in BRAM
    vx_uint32 LUTS[2 * 1][BINS];
#pragma HLS array_partition variable = LUTS complete dim = 1

    // Reset histogram
    ResetHistograms<vx_uint32, VEC_NUM, BINS>(LUTS);

    // Create histogram and calculate max value
    for (vx_uint32 y = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1
            DataType pixel = 0;

            // Only handle non-border pixels
            const vx_uint32 ptr = y * IMG_COLS + x;
            if ((x > BORDER && x < IMG_COLS - BORDER) && (y > BORDER && y < IMG_ROWS - BORDER)) {
                pixel = input[ptr];
            }

            // Ignore zero values
            vx_bool ignore = vx_true_e;
            if (pixel != 0) {
                ++npoints;
                ignore = vx_false_e;
            }

            HistogramUpdateEntry<DataType, IMG_PIXELS, BINS, RANGE, OFFSET> //
                (ptr, pixel, LUTS[0], LUTS[1], pre_bin, pre_val, ignore);   //
        }
    }

    nthreshold = ((vx_uint64)npoints * (vx_uint64)KCONTRAST_PERCENTILE) >> 16;

    // Find histogram percentile
    for (vx_uint16 k = 0; k < BINS; ++k) {
#pragma HLS PIPELINE II = 1
        if (nelements < nthreshold) {
            nelements += (LUTS[0][k] + LUTS[1][k]);
            binthreshold = k;
        }
    }
    vx_uint16 contrast = 1966; // 2^16 * 0.03f
    if (nelements >= nthreshold) {
        // -1 factor 2 due to normalization of values to 0.5
        contrast = binthreshold << (SHIFT + 16 - 1 - DIGITS);
    }

    output[0] = contrast;
}

/******************************************************************************/
/* Calculate Contrast Factor k Vectorized */
/******************************************************************************/

template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint8 VEC_NUM,       //
          vx_uint16 KCONTRAST_PERCENTILE, vx_uint32 BORDER>                                  //
void ContrastFactor(vx_image_data<DataType, VEC_NUM> input[IMG_ROWS * (IMG_COLS / VEC_NUM)], //
                    vx_image_data<vx_uint16, 1> output[1]) {                                 //
#pragma HLS INLINE

    // Constants required for calculations dependant on input type
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool INPUT_IS_UINT8 = (TYPE == VX_TYPE_UINT8);
    const vx_uint16 BINS = (INPUT_IS_UINT8) ? (256) : (512);
    const vx_uint8 SHIFT = (INPUT_IS_UINT8) ? (0) : (7);
    const vx_uint8 DIGITS = (INPUT_IS_UINT8) ? (8) : (16);
    const vx_uint32 RANGE = (1 << 8 * sizeof(DataType));
    const vx_uint32 OFFSET = 0;
    const vx_uint32 IMG_VEC_COLS = IMG_COLS / VEC_NUM;
    const vx_uint32 IMG_VEC_PIXELS = IMG_ROWS * IMG_VEC_COLS;

    STATIC_ASSERT(VEC_NUM == 1 || VEC_NUM == 2 || VEC_NUM == 4 || VEC_NUM == 8,
                  ONLY_1_2_4_8_CAN_BE_VECTORIZATION_FACTOR);
    STATIC_ASSERT(IMG_COLS % VEC_NUM == 0, VECTORIZATION_FACTOR_NEEDS_TO_DIVIDE_COLUMNS_WITHOUT_REST);
    STATIC_ASSERT(TYPE == VX_TYPE_UINT8 || TYPE == VX_TYPE_UINT16, ONLY_AVAILABLE_FOR_UINT8_OR_UINT16_DATA);
    STATIC_ASSERT(BORDER < 2 * IMG_COLS && BORDER < 2 * IMG_ROWS, BORDER_LARGER_THAN_IMAGE);

    // Global variables
    vx_uint16 binthreshold = 0;
    vx_uint32 nthreshold = 0;
    vx_uint32 nelements = 0;
    vx_uint32 npoints = 0;
    vx_image_data<vx_uint16, 1> res;

    // Buffer Histogram in BRAM
    vx_uint32 LUTS[2 * VEC_NUM][BINS];
#pragma HLS array_partition variable = LUTS complete dim = 1
    vx_uint16 pre_bin[VEC_NUM];
#pragma HLS array_partition variable = pre_bin complete
    vx_uint32 pre_val[VEC_NUM];
#pragma HLS array_partition variable = pre_val complete
    vx_uint32 points[VEC_NUM];
#pragma HLS array_partition variable = points complete

    // Reset histogram
    ResetHistograms<vx_uint32, VEC_NUM, BINS>(LUTS);
    for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
        pre_bin[v] = 0;
        pre_val[v] = 0;
        points[v] = 0;
    }

    // Create histogram and calculate max value
    for (vx_uint32 y = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 xvec = 0; xvec < IMG_VEC_COLS; ++xvec) {
#pragma HLS PIPELINE II = 1

            // read pixel vector
            const vx_uint32 ptr = y * (IMG_COLS / VEC_NUM) + xvec;
            vx_image_data<DataType, VEC_NUM> src = input[ptr];

            for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
                vx_uint32 x = xvec * VEC_NUM + v;
                DataType pixel = 0;

                // Only handle non-border pixels
                if ((x > BORDER && x < IMG_COLS - BORDER) && (y > BORDER && y < IMG_ROWS - BORDER)) {
                    pixel = src.pixel[v];
                }

                // Ignore zero values
                vx_bool ignore = vx_true_e;
                if (pixel != 0) {
                    ++points[v];
                    ignore = vx_false_e;
                }

                HistogramUpdateEntry<DataType, IMG_VEC_PIXELS, BINS, RANGE, OFFSET>             //
                    (ptr, pixel, LUTS[2 * v], LUTS[2 * v + 1], pre_bin[v], pre_val[v], ignore); //
            }
        }
    }

    for (vx_uint8 v = 0; v < VEC_NUM; ++v) {
#pragma HLS UNROLL
        npoints += points[v];
    }
    nthreshold = ((vx_uint64)npoints * (vx_uint64)KCONTRAST_PERCENTILE) >> 16;

    // Find histogram percentile
    for (vx_uint16 k = 0; k < BINS; ++k) {
#pragma HLS PIPELINE II = VEC_NUM
        for (vx_uint16 v = 0; v < VEC_NUM; ++v) {
            if (nelements < nthreshold) {
                nelements += (LUTS[2 * v][k] + LUTS[2 * v + 1][k]);
                binthreshold = k;
            }
        }
    }

    // 2^16 * 0.03f (TODO: constant)
    vx_uint16 contrast = 1966;
    if (nelements >= nthreshold) {
        // -1 factor 2 due to normalization of values to 0.5
        contrast = binthreshold << (SHIFT + 16 - 1 - DIGITS);
    }

    res.pixel[0] = contrast;

    // Set user(SOF) & last(EOF)
    GenerateDmaSignal<vx_uint16, 1>(true, true, res); // TODO: wieso nicht beides true?

    output[0] = res;
}

/***************************************************************************************************************************************************/
/** @brief Computes the integral image of the input.
 * @param IMG_COLS    The columns of the image
 * @param IMG_ROWS    The rows of the image
 * @param input       The input image
 * @param output      The output image
 */
template <const vx_uint16 IMG_COLS, const vx_uint16 IMG_ROWS>            //
void Integral(vx_image_data<vx_uint8, 1> input[IMG_COLS * IMG_ROWS],     //
              vx_image_data<vx_uint32, 1> output[IMG_COLS * IMG_ROWS]) { //
#pragma HLS INLINE

    // Buffers 1 row of the integral image
    vx_uint32 buffer[IMG_COLS];

    // Compute integral on image
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0, sum_row = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1
            vx_image_data<vx_uint8, 1> src;
            vx_image_data<vx_uint32, 1> dst;

            // Read input
            src = input[y * IMG_COLS + x];
            vx_uint8 pixel = src.pixel[0];

            // Compute the sum of the current row
            sum_row += pixel;

            // Compute the integral pixel
            vx_uint32 sum_area = sum_row;
            if (y > 0)
                sum_area += buffer[x];

            // Buffer the integral pixels of row for next row
            buffer[x] = sum_area;

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<vx_uint32, 1>((x == 0 && y == 0), (x == (IMG_COLS - 1) && y == (IMG_ROWS - 1)), dst);

            // Write output
            dst.pixel[0] = sum_area;
            output[y * IMG_COLS + x] = dst;
        }
    }
}
/***************************************************************************************************************************************************/
/** @brief Computes the integral image of the input.
 * @param IMG_COLS    The columns of the image
 * @param IMG_ROWS    The rows of the image
 * @param input       The input image
 * @param output      The output image
 */
template <const vx_uint16 IMG_COLS, const vx_uint16 IMG_ROWS> //
void Integral(vx_uint8 input[IMG_COLS * IMG_ROWS],            //
              vx_uint32 output[IMG_COLS * IMG_ROWS]) {        //
#pragma HLS INLINE

    // Buffers 1 row of the integral image
    vx_uint32 buffer[IMG_COLS];

    // Compute integral on image
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0, sum_row = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1

            // Read input
            vx_uint8 pixel = input[y * IMG_COLS + x];

            // Compute the sum of the current row
            sum_row += pixel;

            // Compute the integral pixel
            vx_uint32 sum_area = sum_row;
            if (y > 0)
                sum_area += buffer[x];

            // Buffer the integral pixels of row for next row
            buffer[x] = sum_area;

            // Write output
            output[y * IMG_COLS + x] = sum_area;
        }
    }
}

/***************************************************************************************************************************************************/
/** @brief Computes the mean pixel value of the pixels in the input image.
 * @param DataType    The data type of the input, which can be 8-bit/16-bit signed/unsigned
 * @param IMG_COLS    The amount of columns in the image
 * @param IMG_ROWS    The amount of rows in the image
 * @param input       The input image
 * @param mean        The output average pixel value.
 * @param reciprocal  The reciprocal of the image pixel amount.
 */
template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void Mean(vx_image_data<DataType, 1> input[IMG_ROWS * IMG_COLS], vx_float32 &mean, const vx_float32 reciprocal) {
#pragma HLS INLINE

    // Variables
    vx_int64 sum = 0;

    // Compute the sum for the mean value (pipelined)
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1
            vx_image_data<DataType, 1> src;

            // Read Input
            src = input[y * IMG_COLS + x];
            DataType data = src.pixel[0];

            // Compute sum
            sum += static_cast<vx_int64>(data);
        }
    }

    // Write back the mean value
    mean = static_cast<vx_float32>(sum) * reciprocal;
}

/***************************************************************************************************************************************************/
/** @brief Computes the mean pixel value of the pixels in the input image.
 * @param DataType    The data type of the input, which can be 8-bit/16-bit signed/unsigned
 * @param IMG_COLS    The amount of columns in the image
 * @param IMG_ROWS    The amount of rows in the image
 * @param input       The input image
 * @param mean        The output average pixel value.
 * @param reciprocal  The reciprocal of the image pixel amount.
 */
template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void Mean(DataType input[IMG_ROWS * IMG_COLS], vx_float32 &mean, const vx_float32 reciprocal) {
#pragma HLS INLINE

    // Variables
    vx_int64 sum = 0;

    // Compute the sum for the mean value (pipelined)
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1

            // Read Input
            DataType data = input[y * IMG_COLS + x];

            // Compute sum
            sum += static_cast<vx_int64>(data);
        }
    }

    // Write back the mean value
    mean = static_cast<vx_float32>(sum) * reciprocal;
}

/***************************************************************************************************************************************************/
/** @brief Computes thestandard deviation of the pixels in the input image.
 * @param DataType       The data type of the input, which can be 8-bit/16-bit signed/unsigned
 * @param IMG_COLS       The amount of columns in the image
 * @param IMG_ROWS       The amount of rows in the image
 * @param input          The input image
 * @param mean           The input average pixel value.
 * @param stddev         The output standard deviation of the pixel values.
 * @param reciprocal     The reciprocal of the image pixel amount.
 */
template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void StdDev(vx_image_data<DataType, 1> input[IMG_ROWS * IMG_COLS], vx_float32 mean, vx_float32 &stddev,
            const vx_float32 reciprocal) {
#pragma HLS INLINE

    // Variables
    vx_uint64 sum = 0;

    // Compute the sum for the standard deviation value (pipelined)
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1
            vx_image_data<DataType, 1> src;

            // Read Input
            src = input[y * IMG_COLS + x];
            DataType data = src.pixel[0];

            // Compute sum
            const vx_int32 sub = (vx_int32)mean - (vx_int32)data;
            sum += (vx_uint64)((vx_int64)sub * (vx_int64)sub);
        }
    }

    // Write back the standard deviation value
    stddev = sqrtf(static_cast<vx_float32>(sum) * reciprocal);
}

/***************************************************************************************************************************************************/
/** @brief Computes thestandard deviation of the pixels in the input image.
 * @param DataType       The data type of the input, which can be 8-bit/16-bit signed/unsigned
 * @param IMG_COLS       The amount of columns in the image
 * @param IMG_ROWS       The amount of rows in the image
 * @param input          The input image
 * @param mean           The input average pixel value.
 * @param stddev         The output standard deviation of the pixel values.
 * @param reciprocal     The reciprocal of the image pixel amount.
 */
template <typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void StdDev(DataType input[IMG_ROWS * IMG_COLS], vx_float32 mean, vx_float32 &stddev, const vx_float32 reciprocal) {
#pragma HLS INLINE

    // Variables
    vx_uint64 sum = 0;

    // Compute the sum for the standard deviation value (pipelined)
    for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint32 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1

            // Read Input
            DataType data = input[y * IMG_COLS + x];

            // Compute sum
            const vx_int32 sub = (vx_int32)mean - (vx_int32)data;
            sum += (vx_uint64)((vx_int64)sub * (vx_int64)sub);
        }
    }

    // Write back the standard deviation value
    stddev = sqrtf(static_cast<vx_float32>(sum) * reciprocal);
}

/***************************************************************************************************************************************************/
/** @brief Computes the mean pixel value and the standard deviation of the pixels in the input image (which has a
dimension width and height). Use with or without all parameters marked as optional.
* @param DataType       The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS       The amount of columns in the image
* @param IMG_ROWS       The amount of rows in the image
* @param COMPUTE_STDDEV Is true if standard deviation needs to be computed (If [optional] is set)
* @param input1         The input image (read 1. time)
* @param mean           The VX_TYPE_FLOAT32 average pixel value.
* @param input2         [optional] The input image (read 2. time)
* @param stddev         [optional] The VX_TYPE_FLOAT32 standard deviation of the pixel values.
*/
template <typename DataType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_bool COMPUTE_STDDEV>
void MeanStdDev(vx_image_data<DataType, 1> input1[IMG_ROWS * IMG_COLS], vx_image_data<vx_float32, 1> mean[1],
                vx_image_data<DataType, 1> input2[IMG_ROWS * IMG_COLS], vx_image_data<vx_float32, 1> stddev[1]) {
#pragma HLS INLINE

    // Constants
    const vx_float32 reciprocal = 1.0f / (IMG_ROWS * IMG_COLS);

    // Check input
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type =
        (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16) || (TYPE == VX_TYPE_INT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_mean_stddev);

    // Local variables
    vx_float32 mean_t = 0, stddev_t = 0;
    vx_image_data<vx_float32, 1> mean_v;
    vx_image_data<vx_float32, 1> stddev_v;

    // Compute the mean
    Mean<DataType, IMG_COLS, IMG_ROWS>(input1, mean_t, reciprocal);

    // Store mean
    GenerateDmaSignal<vx_float32, 1>(true, true, mean_v);
    mean_v.pixel[0] = mean_t;
    *mean = mean_v;

    // Compute the standard deviation
    if (COMPUTE_STDDEV == true) {
        StdDev<DataType, IMG_COLS, IMG_ROWS>(input2, mean_t + 0.5f, stddev_t, reciprocal);

        // Store standard deviation
        GenerateDmaSignal<vx_float32, 1>(true, true, stddev_v);
        stddev_v.pixel[0] = stddev_t;
        *stddev = stddev_v;
    }
}

/***************************************************************************************************************************************************/
/** @brief Computes the mean pixel value and the standard deviation of the pixels in the input image (which has a
dimension width and height). Use with or without all parameters marked as optional.
* @param DataType       The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS       The amount of columns in the image
* @param IMG_ROWS       The amount of rows in the image
* @param COMPUTE_STDDEV Is true if standard deviation needs to be computed (If [optional] is set)
* @param input1         The input image (read 1. time)
* @param mean           The VX_TYPE_FLOAT32 average pixel value.
* @param input2         [optional] The input image (read 2. time)
* @param stddev         [optional] The VX_TYPE_FLOAT32 standard deviation of the pixel values.
*/
template <typename DataType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_bool COMPUTE_STDDEV>
void MeanStdDev(DataType input1[IMG_ROWS * IMG_COLS], vx_float32 mean[1], DataType input2[IMG_ROWS * IMG_COLS],
                vx_float32 stddev[1]) {
#pragma HLS INLINE

    // Constants
    const vx_float32 reciprocal = 1.0f / (IMG_ROWS * IMG_COLS);

    // Check input
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type =
        (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16) || (TYPE == VX_TYPE_INT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_mean_stddev);

    // Local variables
    vx_float32 mean_t = 0, stddev_t = 0;

    // Compute the mean
    Mean<DataType, IMG_COLS, IMG_ROWS>(input1, mean_t, reciprocal);

    // Store mean
    *mean = mean_t;

    // Compute the standard deviation
    if (COMPUTE_STDDEV == true) {
        StdDev<DataType, IMG_COLS, IMG_ROWS>(input2, mean_t + 0.5f, stddev_t, reciprocal);

        // Store standard deviation
        *stddev = stddev_t;
    }
}

/***************************************************************************************************************************************************/
// Update Min/max location
template <typename DataType, vx_uint16 CAPACITY, vx_bool COMPUTE_LOCATIONS>
void MinMaxUpdateLocation(vx_uint16 y, vx_uint16 x, DataType data, vx_uint16 min_loc_x_buffer[CAPACITY],
                          vx_uint16 min_loc_y_buffer[CAPACITY], vx_uint16 max_loc_x_buffer[CAPACITY],
                          vx_uint16 max_loc_y_buffer[CAPACITY], DataType &min_value, DataType &max_value,
                          vx_uint16 &min_counter, vx_uint16 &max_counter) {
#pragma HLS INLINE

    //
    if (COMPUTE_LOCATIONS == vx_true_e) {

        // Compute minimum/maximum value, locations and counter
        if (data < min_value) {
            min_value = data;
            min_loc_x_buffer[0] = x;
            min_loc_y_buffer[0] = y;
            min_counter = 1;
        } else if ((data == min_value) && (min_counter < CAPACITY)) {
            min_loc_x_buffer[min_counter] = x;
            min_loc_y_buffer[min_counter] = y;
            min_counter++;
        }

        // Compute maximum value, locations and counter
        if (data > max_value) {
            max_value = data;
            max_loc_x_buffer[0] = x;
            max_loc_y_buffer[0] = y;
            max_counter = 1;
        } else if ((data == max_value) && (max_counter < CAPACITY)) {
            max_loc_x_buffer[max_counter] = x;
            max_loc_y_buffer[max_counter] = y;
            max_counter++;
        }
    } else {
        min_value = MIN(data, min_value);
        max_value = MAX(data, max_value);
    }
}

/***************************************************************************************************************************************************/
/** @brief Finds the minimum and maximum values in an image and a location for each. If the input image has several
minimums/maximums, the kernel returns all of them. Use with or without all parameters marked as optional.
* @param DataType The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS   The amount of columns in the image
* @param IMG_ROWS   The amount of rows in the image
* @param CAPACITY   [optional] The capacity of the arrays that store the locations (minLoc , max_loc)
* @param input      The input image
* @param min_val    The minimum value in the image, which corresponds to the type of the input.
* @param max_val    The maximum value in the image, which corresponds to the type of the input.
* @param min_loc    [optional] The minimum x, y locations. If the input image has several minimums,
                               the kernel will return up to the capacity of the array.
* @param max_loc    [optional] The maximum x, y locations. If the input image has several maximums,
                               the kernel will return up to the capacity of the array.
* @param min_count  [optional] The total number of detected minimums in image.
* @param max_count  [optional] The total number of detected maximums in image.
*/
template <typename DataType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 CAPACITY, vx_bool COMPUTE_LOCATIONS>
void MinMaxLocation(vx_image_data<DataType, 1> input[IMG_ROWS * IMG_COLS], vx_image_data<DataType, 1> min_val[1],
                    vx_image_data<DataType, 1> max_val[1], vx_image_data<vx_uint16, 2> min_loc[CAPACITY],
                    vx_image_data<vx_uint16, 2> max_loc[CAPACITY], vx_image_data<vx_uint16, 1> min_count[1],
                    vx_image_data<vx_uint16, 1> max_count[1]) {
#pragma HLS INLINE

    // Check input
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type =
        (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16) || (TYPE == VX_TYPE_INT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_min_max_location);

    // Variables and arrays
    vx_uint16 min_loc_x_buffer[CAPACITY];
    vx_uint16 min_loc_y_buffer[CAPACITY];
    vx_uint16 max_loc_x_buffer[CAPACITY];
    vx_uint16 max_loc_y_buffer[CAPACITY];

    DataType min_value = std::numeric_limits<DataType>::max();
    DataType max_value = std::numeric_limits<DataType>::min();
    vx_uint16 min_counter = 0;
    vx_uint16 max_counter = 0;

    // Computes the minimum and maximum values, including their counters and locations (pipelined)
    for (vx_uint16 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint16 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1
            vx_image_data<DataType, 1> src;

            // Read Input
            src = input[y * IMG_COLS + x];
            DataType data = src.pixel[0];

            // Update Min/max location
            MinMaxUpdateLocation<DataType, CAPACITY, COMPUTE_LOCATIONS>(y, x, data, min_loc_x_buffer, min_loc_y_buffer,
                                                                        max_loc_x_buffer, max_loc_y_buffer, min_value,
                                                                        max_value, min_counter, max_counter);
        }
    }

    {
        // Write output
        vx_image_data<DataType, 1> dst1, dst2;
        GenerateDmaSignal<DataType, 1>(true, true, dst1);
        GenerateDmaSignal<DataType, 1>(true, true, dst2);
        dst1.pixel[0] = min_value;
        dst2.pixel[0] = max_value;
        *min_val = dst1; // TODO: put into previous loop
        *max_val = dst2; // TODO: put into previous loop
    }

    // Write optional output
    if (COMPUTE_LOCATIONS == vx_true_e) {

        // Get the maximum amount of elements between both location buffers
        const vx_uint16 max_elements =
            MIN(MAX(min_counter, max_counter), CAPACITY); // TODO: put as loop bound in next loop

        // Store max location information
        for (vx_uint16 i = 0; i < max_elements; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS loop_tripcount min = 1 max = CAPACITY
            vx_image_data<vx_uint16, 1> dst1, dst2;
            vx_image_data<vx_uint16, 2> dst3, dst4;

            if (i == 0) {
                GenerateDmaSignal<vx_uint16, 1>(true, true, dst1);
                GenerateDmaSignal<vx_uint16, 1>(true, true, dst2);
                dst1.pixel[0] = min_counter;
                dst2.pixel[0] = max_counter;
                *min_count = dst1; // TODO previous/middle loop
                *max_count = dst2; // TODO previous/middle loop
            }
            if (i < min_counter) {
                dst3.pixel[0] = min_loc_x_buffer[i];
                dst3.pixel[1] = min_loc_y_buffer[i];
                GenerateDmaSignal<vx_uint16, 2>((i == 0), (i == min_counter - 1), dst3);
                min_loc[i] = dst3;
            }
            if (i < max_counter) {
                dst4.pixel[0] = max_loc_x_buffer[i];
                dst4.pixel[1] = max_loc_y_buffer[i];
                GenerateDmaSignal<vx_uint16, 2>((i == 0), (i == max_counter - 1), dst4);
                max_loc[i] = dst4;
            }
        }
    }
}

/***************************************************************************************************************************************************/
/** @brief Finds the minimum and maximum values in an image and a location for each. If the input image has several
minimums/maximums, the kernel returns all of them. Use with or without all parameters marked as optional.
* @param DataType The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS   The amount of columns in the image
* @param IMG_ROWS   The amount of rows in the image
* @param CAPACITY   [optional] The capacity of the arrays that store the locations (minLoc , max_loc)
* @param input      The input image
* @param min_val    The minimum value in the image, which corresponds to the type of the input.
* @param max_val    The maximum value in the image, which corresponds to the type of the input.
* @param min_loc    [optional] The minimum x, y locations. If the input image has several minimums,
                               the kernel will return up to the capacity of the array.
* @param max_loc    [optional] The maximum x, y locations. If the input image has several maximums,
                               the kernel will return up to the capacity of the array.
* @param min_count  [optional] The total number of detected minimums in image.
* @param max_count  [optional] The total number of detected maximums in image.
*/
template <typename DataType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 CAPACITY, vx_bool COMPUTE_LOCATIONS>
void MinMaxLocation(DataType input[IMG_ROWS * IMG_COLS], DataType min_val[1], DataType max_val[1],
                    vx_image_data<vx_uint16, 2> min_loc[CAPACITY], vx_image_data<vx_uint16, 2> max_loc[CAPACITY],
                    vx_uint16 min_count[1], vx_uint16 max_count[1]) {
#pragma HLS INLINE

    // Check input
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type =
        (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16) || (TYPE == VX_TYPE_INT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_min_max_location);

    // Variables and arrays
    vx_uint16 min_loc_x_buffer[CAPACITY];
    vx_uint16 min_loc_y_buffer[CAPACITY];
    vx_uint16 max_loc_x_buffer[CAPACITY];
    vx_uint16 max_loc_y_buffer[CAPACITY];

    DataType min_value = ComputeMax<DataType>();
    DataType max_value = ComputeMin<DataType>();
    vx_uint16 min_counter = 0;
    vx_uint16 max_counter = 0;

    // Computes the minimum and maximum values, including their counters and locations (pipelined)
    for (vx_uint16 y = 0; y < IMG_ROWS; y++) {
        for (vx_uint16 x = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II = 1

            // Read Input
            DataType data = input[y * IMG_COLS + x];

            // Update Min/max location
            MinMaxUpdateLocation<DataType, CAPACITY, COMPUTE_LOCATIONS>(y, x, data, min_loc_x_buffer, min_loc_y_buffer,
                                                                        max_loc_x_buffer, max_loc_y_buffer, min_value,
                                                                        max_value, min_counter, max_counter);
        }
    }

    // Write output
    *min_val = min_value; // TODO: in mittlere schleife (pipelinen)
    *max_val = max_value; // TODO: in mittlere schleife (pipelinen)

    // Write optional output
    if (COMPUTE_LOCATIONS == vx_true_e) {

        // Get the maximum amount of elements between both location buffers
        const vx_uint16 max_elements =
            MIN(MAX(min_counter, max_counter), CAPACITY); // TODO: put as loop bound in next loop

        // Store max location information
        for (vx_uint16 i = 0; i < max_elements; i++) {
#pragma HLS PIPELINE II = 1
#pragma HLS loop_tripcount min = 1 max = CAPACITY
            vx_image_data<vx_uint16, 2> dst3, dst4;

            if (i == 0) {
                *min_count = min_counter; // TODO: in mittlere schleife
                *max_count = max_counter; // TODO: in mittlere schleife
            }
            if (i < min_counter) {
                dst3.pixel[0] = min_loc_x_buffer[i];
                dst3.pixel[1] = min_loc_y_buffer[i];
                min_loc[i] = dst3;
            }
            if (i < max_counter) {
                dst4.pixel[0] = max_loc_x_buffer[i];
                dst4.pixel[1] = max_loc_y_buffer[i];
                max_loc[i] = dst4;
            }
        }
    }
}

/***************************************************************************************************************************************************/
/** @brief Implements the Table Lookup Image Kernel.
 * @param DataType   The data type can be vx_uint8 and vx_int16
 * @param IMG_PIXELS  The amount of pixels for input and output image
 * @param LUT_COUNT   Indicates the number of elements in the LUT.
 * @param LUT_OFFSET  Indicates the index of the input value = 0.
 * @param input       The input image (vx_uint8, vx_int16)
 * @param output      The output image (vx_uint8, vx_int16)
 */
template <typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
void TableLookup(vx_image_data<DataType, 1> input[IMG_PIXELS], vx_image_data<DataType, 1> lut[LUT_COUNT],
                 vx_image_data<DataType, 1> output[IMG_PIXELS]) {
#pragma HLS INLINE

    // Check Datatype
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type = (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_histogram);

    // Buffer for lookup table
    DataType table[LUT_COUNT];

    // Read lookup table into buffer
    for (vx_uint32 i = 0; i < LUT_COUNT; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<DataType, 1> src;
        src = lut[i];
        table[i] = src.pixel[0];
    }

    // Perform table lookup
    for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<DataType, 1> src;
        vx_image_data<DataType, 1> dst;

        // Read from Input
        src = input[i];
        DataType data = src.pixel[0];

        // Add Offset
        vx_int32 index = (vx_int32)data + (vx_int32)LUT_OFFSET;

        // Perform Table Lookup and write to output
        if ((index >= 0) && (index < (vx_int32)LUT_COUNT))
            dst.pixel[0] = table[index];
        else
            dst.pixel[0] = 0;

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<DataType, 1>((i == 0), (i == IMG_PIXELS - 1), dst);

        // Write output
        output[i] = dst;
    }
}

/***************************************************************************************************************************************************/
/** @brief Implements the Table Lookup Image Kernel.
 * @param DataType   The data type can be vx_uint8 and vx_int16
 * @param IMG_PIXELS  The amount of pixels for input and output image
 * @param LUT_COUNT   Indicates the number of elements in the LUT.
 * @param LUT_OFFSET  Indicates the index of the input value = 0.
 * @param input       The input image (vx_uint8, vx_int16)
 * @param output      The output image (vx_uint8, vx_int16)
 */
template <typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
void TableLookup(DataType input[IMG_PIXELS], DataType lut[LUT_COUNT], DataType output[IMG_PIXELS]) {
#pragma HLS INLINE

    // Check Datatype
    const vx_type_e TYPE = GET_TYPE(DataType);
    const bool allowed_type = (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_INT16);
    STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_histogram);

    // Buffer for lookup table
    DataType table[LUT_COUNT];

    // Read lookup table into buffer
    for (vx_uint32 i = 0; i < LUT_COUNT; i++) {
#pragma HLS PIPELINE II = 1
        table[i] = lut[i];
    }

    // Perform table lookup
    for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        DataType result = 0;

        // Read from Input
        DataType data = input[i];

        // Add Offset
        vx_int32 index = (vx_int32)data + (vx_int32)LUT_OFFSET;

        // Perform Table Lookup and write to output
        if ((index >= 0) && (index < (vx_int32)LUT_COUNT))
            result = table[index];
        else
            result = 0;

        // Write output
        output[i] = result;
    }
}

#endif /* SRC_IMG_ANALYSIS_CORE_H_ */
