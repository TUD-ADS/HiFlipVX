/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_filter_core.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are the core functions used for the hardware accelerated image processing functions
 */

#ifndef SRC_IMG_FILTER_CORE_H_
#define SRC_IMG_FILTER_CORE_H_

#include "img_filter_window.h"

/*********************************************************************************************************************/
/* SEPARABLE FILTER FUNCTIONS */
/*********************************************************************************************************************/

/** @brief  Checks the input parameters of the convolution filter
@param InType       Data type of the input image (8-bit, 16-bit, 32-bit)
@param OutType      Data type of the output image (8-bit, 16-bit, 32-bit)
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image width
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
*/
template <typename InType, typename OutType, vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, vx_uint16 IMG_COLS,
          vx_uint16 KERN_SIZE> //
void CheckFilterParameters() { //
#pragma HLS INLINE
    // Check function parameters/types
    const vx_uint16 src_size = sizeof(InType);
    const vx_uint16 dst_size = sizeof(OutType);
    const bool allowed_kernel_size =
        (KERN_SIZE == 3) || (KERN_SIZE == 5) || (KERN_SIZE == 7) || (KERN_SIZE == 9) || (KERN_SIZE == 11);
    const bool allowed_vector_size = (VEC_NUM == 1) || (VEC_NUM == 2) || (VEC_NUM == 4) || (VEC_NUM == 8);
    const bool allowed_data_type = (src_size == 1) || (src_size == 2) || (src_size == 4);
    const bool allowed_kernel_type = (src_size == 1) || (src_size == 2) || (src_size == 4);
    const bool allowed_kernel_num = (KERN_NUM == 1) || (KERN_NUM == 2);
    STATIC_ASSERT(allowed_kernel_size, kernel_size_must_be_3_5_7_9_11);
    STATIC_ASSERT((IMG_COLS % VEC_NUM == 0), image_colums_are_not_multiple_of_vector_size);
    STATIC_ASSERT((src_size == dst_size), size_of_in_and_out_type_must_be_equal);
    STATIC_ASSERT(allowed_vector_size, vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(allowed_data_type, data_type_must_be_8_16_32_bit);
    STATIC_ASSERT(allowed_kernel_type, kernel_data_type_must_be_8_16_32_bit);
    STATIC_ASSERT(allowed_kernel_num, kernel_num_can_only_be_1__or_2);
}

/** @brief Selects and computes a 1d horizontal filter
@param ScalarType   Data type of the input/output image
@param KernelType   Data type of the kernel
@param KERN_SIZE    Filter kernel size
@param WIN_COLS     Columns of the sliding window
@param VEC_NUM      Vector size
@param window       The sliding window
@param kernel       The convolution kernels
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
@param output       The results (vector) (per clock cycle)
*/
template <typename ScalarType, typename KernelType, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE,
          vx_uint16 WIN_COLS, vx_uint16 VEC_NUM>
void ComputeFilterHorizontal(ScalarType window[WIN_COLS], KernelType kernel[KERN_SIZE], const vx_uint64 kernel_mult,
                             const vx_uint32 kernel_shift, ScalarType output[VEC_NUM]) {
#pragma HLS INLINE

    for (vx_uint16 vecId = 0; vecId < VEC_NUM; vecId++) {
#pragma HLS unroll

        // Window for single vector element
        ScalarType kernel_window[KERN_SIZE];
#pragma HLS array_partition variable = kernel_window complete dim = 0

        // Get window for single vector element
        for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
            kernel_window[i] = window[i + vecId];
        }

        // Compute filter for single vector element
        ScalarType result = 0;
        switch (KERN_TYPE) {
        case HIFLIPVX::GAUSSIAN_FILTER:
            result =
                ComputeGaussian1d<ScalarType, KernelType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::BOX_FILTER:
            result = ComputeBox1d<ScalarType, KERN_SIZE>(kernel_window, kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::ERODE_IMAGE:
            result = ComputeErode1d<ScalarType, KERN_SIZE>(kernel_window);
            break;
        case HIFLIPVX::DILATE_IMAGE:
            result = ComputeDilate1d<ScalarType, KERN_SIZE>(kernel_window);
            break;
        default:
            break;
        }
        output[vecId] = result;
    }
}

/** @brief Selects and computes a 1d vertical filter
@param ScalarType   Data type of the input/output image
@param KernelType   Data type of the kernel
@param KERN_SIZE    Filter kernel size
@param VEC_NUM      Vector size
@param window       The sliding window
@param kernel       The convolution kernels
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
@param output       The results (vector) (per clock cycle)
*/
template <typename ScalarType, typename KernelType, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE,
          vx_uint16 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
void ComputeFilterVertical(ScalarType window[KERN_SIZE][VEC_NUM], KernelType kernel[KERN_SIZE],
                           const vx_uint64 kernel_mult, const vx_uint32 kernel_shift,
                           vx_image_data<ScalarType, VEC_NUM> &output, vx_uint16 x, vx_uint16 y) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 WIN_COLS = 2 * KERN_RAD + VEC_NUM + (VEC_NUM - (KERN_RAD % VEC_NUM)) % VEC_NUM;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;

    // Output array of the different filter (scalar)
    vx_image_data<ScalarType, VEC_NUM> output_array;

    for (vx_uint16 vecId = 0; vecId < VEC_NUM; vecId++) {
#pragma HLS unroll

        // Window for single vector element
        ScalarType kernel_window[KERN_SIZE];
#pragma HLS array_partition variable = kernel_window complete dim = 0

        // Get window for single vector element
        for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
            kernel_window[i] = window[i][vecId];
        }

        // Compute filter for single vector element
        ScalarType result = 0;
        switch (KERN_TYPE) {
        case HIFLIPVX::GAUSSIAN_FILTER:
            result =
                ComputeGaussian1d<ScalarType, KernelType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::BOX_FILTER:
            result = ComputeBox1d<ScalarType, KERN_SIZE>(kernel_window, kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::ERODE_IMAGE:
            result = ComputeErode1d<ScalarType, KERN_SIZE>(kernel_window);
            break;
        case HIFLIPVX::DILATE_IMAGE:
            result = ComputeDilate1d<ScalarType, KERN_SIZE>(kernel_window);
            break;
        default:
            break;
        }
        output_array.pixel[vecId] = result;
    }

    // Set user(SOF) & last(EOF)
    const bool sof = ((y == KERN_RAD) && (x == OHD_COLS));
    const bool eof = ((y == (IMG_ROWS + KERN_RAD - 1)) && (x == (VEC_COLS + OHD_COLS - 1)));
    GenerateDmaSignal<ScalarType, VEC_NUM>(sof, eof, output_array);

    // Write output
    output = output_array;
}

/** @brief  Computes multiple 1d convolution filter of kernel_type type
@param ScalarType   Data type of the input/output image (8-bit, 16-bit, 32-bit)
@param KernelType   Data type of the kernel (8-bit, 16-bit, 32-bit)
@param BufferType   (sizeof(InType) * VEC_NUM * (KERN_SIZE - 1)) == (sizeof(BufferType) * BUFFER_NUM)
@param BUFFER_NUM   (sizeof(InType) * VEC_NUM * (KERN_SIZE - 1)) == (sizeof(BufferType) * BUFFER_NUM)
@param VEC_PIX      The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image width
@param IMG_ROWS     Image height
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param input        Input image
@param output       Output image
@param kernel       The convolution kernel
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
*/
template <typename ScalarType, typename KernelType, /*typename BufferType, vx_uint16 BUFFER_NUM, */ vx_uint32 VEC_PIX,
          vx_uint16 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE,
          HIFLIPVX::FilterOperation KERN_TYPE, vx_border_e BORDER_TYPE>
void ComputeFilter1dFunc(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],
                         vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX], KernelType kernel[KERN_SIZE],
                         const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 WIN_COLS = 2 * KERN_RAD + VEC_NUM + (VEC_NUM - (KERN_RAD % VEC_NUM)) % VEC_NUM;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;

    // Linebuffer
    vx_data_pack<ScalarType, (KERN_SIZE - 1) * VEC_NUM> linebuffer[VEC_COLS];
    HIFLIPVX_DATAPACK(linebuffer);

    // Sliding window for complete vector
    ScalarType window_hor[1][WIN_COLS];
#pragma HLS array_partition variable = window_hor complete dim = 0

    ScalarType window_ver[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable = window_ver complete dim = 0

    // Compute the filter (pipelined)
    vx_uint32 ptr_src = 0, ptr_dst = 0;
    for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; y++) {
        for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; x++) {
#pragma HLS PIPELINE II = 1

            // Variables (Registers)
            ScalarType input_buffer[VEC_NUM];
#pragma HLS array_partition variable = input_buffer complete dim = 0
            ScalarType buffer[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable = buffer complete dim = 0
            ScalarType internal_buffer[VEC_NUM];
#pragma HLS array_partition variable = internal_buffer complete dim = 0
            vx_image_data<ScalarType, VEC_NUM> input_data_2d;
            vx_image_data<ScalarType, VEC_NUM> output_data;

            // Read input data from global memory
            if ((y < IMG_ROWS) && (x < VEC_COLS)) {
                input_data_2d = input[ptr_src];
                ptr_src++;
            }
            for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS unroll
                input_buffer[v] = input_data_2d.pixel[v];
            }

            // Compute Horizontal Filter including sliding window
            SlidingWindowHorizontal<ScalarType, KERN_RAD, VEC_COLS, VEC_NUM, WIN_COLS, BORDER_TYPE>(input_buffer,
                                                                                                    window_hor, x);
            ComputeFilterHorizontal<ScalarType, KernelType, KERN_SIZE, KERN_TYPE, WIN_COLS, VEC_NUM>(
                window_hor[0], kernel, kernel_mult, kernel_shift, internal_buffer);

            // Compute linebuffer
            vx_int16 xs = x - OHD_COLS;
            if (xs >= 0) {
                ReadFromLineBuffer_<ScalarType, VEC_NUM, KERN_SIZE, VEC_COLS>(internal_buffer, linebuffer, buffer, xs);
                WriteToLineBuffer_<ScalarType, VEC_NUM, KERN_SIZE, VEC_COLS>(buffer, linebuffer, xs);
            }

            // Compute Vertical Filter including sliding window
            SlidingWindowVertical<ScalarType, IMG_ROWS, KERN_RAD, VEC_NUM, KERN_SIZE, BORDER_TYPE>(buffer, window_ver,
                                                                                                   y);
            ComputeFilterVertical<ScalarType, KernelType, KERN_SIZE, KERN_TYPE, VEC_NUM, IMG_COLS, IMG_ROWS>(
                window_ver, kernel, kernel_mult, kernel_shift, output_data, x, y);

            // Write output data to global memory
            if ((y >= KERN_RAD) && (x >= OHD_COLS)) {
                output[ptr_dst] = output_data;
                ptr_dst++;
            }
        }
    }
}

/** @brief  Computes multiple 1d convolution filter of kernel_type type
@param ScalarType   Data type of the input/output image (8-bit, 16-bit, 32-bit)
@param KernType     Data type of the kernel (8-bit, 16-bit, 32-bit)
@param VEC_PIX      The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param IMG_COLS     Image width
@param IMG_ROWS     Image height
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param input        Input image
@param output       Output image
@param kernel       The convolution kernel
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
*/
template <typename ScalarType, typename KernelType, vx_uint32 VEC_PIX, vx_uint16 VEC_NUM, vx_uint16 IMG_COLS,
          vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE, vx_border_e BORDER_TYPE>
void ComputeFilter1d(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],
                     vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX], KernelType kernel[KERN_SIZE],
                     const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Check parameter input
    CheckFilterParameters<ScalarType, ScalarType, VEC_NUM, 1, IMG_COLS, KERN_SIZE>();

    ComputeFilter1dFunc<ScalarType, KernelType, VEC_PIX, VEC_NUM, IMG_COLS, IMG_ROWS, KERN_SIZE, KERN_TYPE,
                        BORDER_TYPE>(input, output, kernel, kernel_mult, kernel_shift);
}

/*********************************************************************************************************************/
/* FILTER FUNCTIONS (2D) */
/*********************************************************************************************************************/

/**
 * @brief Compute scaled image (filles gaps with zeros)
 * @tparam KernType Data type of the kernel
 * @param kernel_in Input kernel
 * @param kernel_out Upscaled kernel
 */
template <typename KernType, vx_uint16 KERN_SIZE, vx_uint16 SCALE>
void ComputeScaledKernel(KernType kernel_in[KERN_SIZE][KERN_SIZE],
                         KernType kernel_out[SCALE_KERNEL(KERN_SIZE, SCALE)][SCALE_KERNEL(KERN_SIZE, SCALE)]) {
#pragma HLS INLINE

    const vx_uint8 SCALED_KERNEL_SIZE = SCALE_KERNEL(KERN_SIZE, SCALE);
    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint8 SCALED_KERN_RAD = KERN_RAD * SCALE;
    const vx_uint8 GAP_SIZE = SCALE - 1;
    const vx_uint8 GAP_CHECK = GAP_SIZE + 1;

    for (vx_uint8 y = 0; y < SCALED_KERNEL_SIZE; y++) {
#pragma HLS UNROLL
        const vx_bool is_zero_y = ((y % GAP_CHECK) != 0);
        const vx_uint8 map_y = y / SCALE;

        for (vx_uint8 x = 0; x < SCALED_KERNEL_SIZE; x++) {
#pragma HLS UNROLL
            const vx_bool is_zero_x = ((x % GAP_CHECK) != 0);
            const vx_uint8 map_x = x / SCALE;

            kernel_out[y][x] = (is_zero_y | is_zero_x) ? (0) : (kernel_in[map_y][map_x]);
        }
    }
}

/** @brief Selects and computes a filter
@param InType       Data type of the input image
@param OutType      Data type of the output image
@param KernType     Data type of the kernel
@param VEC_NUM     Vector size
@param KERN_NUM     Amount of filter kernel
@param KERN_SIZE    Filter kernel size
@param WIN_COLS     Columns of the sliding window
@param window       The sliding window
@param kernel       The convolution kernels
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
@param output       The results (vector) (per clock cycle)
*/
template <typename InType, typename OutType, typename KernType, typename KernModType, vx_uint16 VEC_NUM_IN,
          vx_uint16 VEC_NUM_OUT, vx_uint16 KERN_NUM, HIFLIPVX::FilterOperation KERN_TYPE, vx_hint_e PRECISION,
          vx_uint16 KERN_SIZE, vx_uint16 STEP, vx_uint16 WIN_COLS, InType THRESH_LOWER, InType THRESH_UPPER,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 IN_NUM, vx_uint16 OUT_ID, vx_uint32 QUANTIZATION>
void ComputeFilter(InType window[IN_NUM][KERN_SIZE][WIN_COLS], KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE],
                   const vx_uint64 kernel_mult, const vx_uint32 kernel_shift, const KernModType kernel_modifier,
                   vx_image_data<OutType, VEC_NUM_OUT> &output, const vx_uint16 x, const vx_uint16 y) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;

    // Output
    vx_image_data<OutType, VEC_NUM_OUT> output_vector;

    // Compute KERN_NUM different filter with VEC_NUM elements per clock cycle
    for (vx_uint16 vecId = 0; vecId < VEC_NUM_OUT; vecId++) {
#pragma HLS unroll

        // Window for single vector element
        InType kernel_window[IN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_window complete dim = 0

        // Get window for single vector element
        for (vx_uint16 inId = 0; inId < IN_NUM; inId++) {
#pragma HLS unroll
            for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
                for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
                    kernel_window[inId][i][j] = window[inId][i][j + vecId * (STEP + 1)];
                }
            }
        }

        // Compute filter for single vector element
        OutType result = 0;

        switch (KERN_TYPE) {
        case HIFLIPVX::GAUSSIAN_FILTER:
            result = ComputeGaussian2d<InType, OutType, KernType, KERN_SIZE>(kernel[OUT_ID], kernel_window[0],
                                                                             kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::DERIVATIVE_X:
            result = ComputeDerivativeX<InType, OutType, KernType, KERN_SIZE>(kernel[OUT_ID], kernel_window[0],
                                                                              kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::DERIVATIVE_Y:
            result = ComputeDerivativeY<InType, OutType, KernType, KERN_SIZE>(kernel[OUT_ID], kernel_window[0],
                                                                              kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::CUSTOM_CONVOLUTION_U:
            result = ComputeConvolve2d<InType, vx_uint64, OutType, KernType, KERN_SIZE>(
                kernel[OUT_ID], kernel_window[0], kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::CUSTOM_CONVOLUTION_S:
            result = ComputeConvolve2d<InType, vx_int64, OutType, KernType, KERN_SIZE>(kernel[OUT_ID], kernel_window[0],
                                                                                       kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::BOX_FILTER:
            result = ComputeBox2d<InType, OutType, KERN_SIZE>(kernel_window[0], kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::MEDIAN_FILTER_NxN:
            result = ComputeMedian<InType, OutType, KERN_SIZE>(kernel_window[0]);
            break;
        case HIFLIPVX::MEDIAN_FILTER_3x3:
            result = ComputeMedian3x3<InType, OutType, KERN_SIZE>(kernel_window[0]);
            break;
        case HIFLIPVX::ERODE_IMAGE:
            result = 0;
            break;
        case HIFLIPVX::DILATE_IMAGE:
            result = 0;
            break;
        case HIFLIPVX::NON_MAX_SUPPRESSION:
            result = NonMaxSuppression<InType, OutType, KernType, KERN_SIZE>(kernel[OUT_ID], kernel_window[0]);
            break;
        case HIFLIPVX::SEGMENT_TEST_DETECTOR:
            result = SegmentTestDetector<InType, OutType, KERN_SIZE>(kernel_window[0]);
            break;
        case HIFLIPVX::HYSTERESIS:
            result = Hysteresis<InType, OutType, KERN_SIZE, THRESH_LOWER, THRESH_UPPER>(kernel_window[0]);
            break;
        case HIFLIPVX::ORIENTED_NON_MAX_SUPPRESSION:
            result = ComputeOrientedNonMaxSuppression<InType, OutType, KERN_SIZE, QUANTIZATION>(kernel_window[0],
                                                                                                kernel_window[1]);
            break;
        case HIFLIPVX::FED:
            result = ComputeFED<InType, KERN_SIZE, QUANTIZATION>(kernel_window[0], kernel_window[1]);
            break;
        case HIFLIPVX::DOH:
            result = ComputeDoH<InType, OutType, KernType, KERN_NUM, KERN_SIZE>(kernel_window[0], kernel_window[1],
                                                                                kernel, kernel_mult, kernel_shift);
            break;
        case HIFLIPVX::PERONA_MALIK_WIDE_REGION:
            result = ComputePeronaMalikWideRegion<InType, KernType, KERN_NUM, KERN_SIZE, PRECISION>(
                kernel_window[0], kernel, kernel_mult, kernel_shift, kernel_modifier);
            break;
        case HIFLIPVX::FORWARD1:
            result = ForwardImageData<InType, KERN_SIZE>(kernel_window[0]);
            break;
        case HIFLIPVX::FORWARD2:
            result = ForwardImageData<InType, KERN_SIZE>(kernel_window[1]);
            break;
        default:
            result = 0;
            break;
        }

        output_vector.pixel[vecId] = result;
    }

    // Set user(SOF) & last(EOF) (stride has also to be considered for EOF
    const vx_bool sof = ((y == KERN_RAD) && (x == OHD_COLS));
    const vx_bool eof = ((y == (IMG_ROWS + KERN_RAD - 1 - STEP)) && (x == (VEC_COLS + OHD_COLS - 1 - STEP)));
    GenerateDmaSignal<OutType, VEC_NUM_OUT>(sof, eof, output_vector);

    // Write Output
    output = output_vector;
}

/** @brief  Computes multiple 2d convolution filter of kernel_type type
@param InType       Data type of the input image (8-bit, 16-bit, 32-bit)
@param OutType      Data type of the output image (8-bit, 16-bit, 32-bit)
@param KernType     Data type of the kernel (8-bit, 16-bit, 32-bit)
@param BufferType   (sizeof(InType) * VEC_NUM * (KERN_SIZE - 1)) == (sizeof(BufferType) * BUFFER_NUM)
@param BUFFER_NUM   (sizeof(InType) * VEC_NUM * (KERN_SIZE - 1)) == (sizeof(BufferType) * BUFFER_NUM)
@param VEC_PIX      The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image IMG_COLS
@param IMG_ROWS     Image height
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param input        Input image
@param output1      1. Output image
@param output2      2. Output image
@param kernel       The convolution kernel
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
*/
template <typename InType, typename OutType, typename KernType, typename KernModType, typename BufferType,
          vx_uint16 BUFFER_NUM, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT, vx_uint16 VEC_NUM_IN,
          vx_uint16 VEC_NUM_OUT, vx_uint16 KERN_NUM, HIFLIPVX::FilterOperation KERN_TYPE1,
          HIFLIPVX::FilterOperation KERN_TYPE2, vx_hint_e PRECISION, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint16 KERN_SIZE, vx_uint16 STEP, vx_border_e BORDER_TYPE, InType THRESH_LOWER, InType THRESH_UPPER,
          vx_uint16 IN_NUM, vx_uint16 OUT_NUM, vx_uint32 QUANTIZATION>
void ComputeFilter2dFunc(vx_image_data<InType, VEC_NUM_IN> input1[VEC_PIX_IN],
                         vx_image_data<InType, VEC_NUM_IN> input2[VEC_PIX_IN],
                         vx_image_data<OutType, VEC_NUM_OUT> output1[VEC_PIX_OUT],
                         vx_image_data<OutType, VEC_NUM_OUT> output2[VEC_PIX_OUT],
                         KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult,
                         const vx_uint32 kernel_shift, const KernModType kernel_modifier) {
#pragma HLS INLINE

    // Use only unsigned data types for buffers, because linebuffer produces errors with signed data types.
    typedef typename to_unsigned<InType>::DataType bufT;

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 WIN_COLS = (KERN_RAD << 1) + VEC_NUM_IN + (VEC_NUM_IN - (KERN_RAD % VEC_NUM_IN)) % VEC_NUM_IN;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;
    const vx_uint16 STEP_VEC_OUT = ((STEP + 1) > VEC_NUM_IN) ? ((STEP + 1) / VEC_NUM_IN) : (1);

    // Linebuffer
    BufferType linebuffer[IN_NUM][BUFFER_NUM][VEC_COLS];
    HIFLIPVX_DATAPACK(linebuffer);

    // Sliding window for complete vector
    bufT window[IN_NUM][KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable = window complete dim = 0

    vx_uint32 ptr_src = 0;
    vx_uint32 ptr_dst = 0;

    // Compute the filter (pipelined)
    for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; ++y) {
        for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            bufT input_buffer[IN_NUM][VEC_NUM_IN];
#pragma HLS array_partition variable = input_buffer complete dim = 0

            bufT buffer[IN_NUM][KERN_SIZE][VEC_NUM_IN];
#pragma HLS array_partition variable = buffer complete dim = 0

            // Input & Output
            vx_image_data<InType, VEC_NUM_IN> input_data[IN_NUM];
            vx_image_data<OutType, VEC_NUM_OUT> vec_output_data[OUT_NUM];

            // Read input data from global memory
            if ((y < IMG_ROWS) && (x < VEC_COLS)) {
                if (IN_NUM > 0)
                    input_data[0] = input1[ptr_src];
                if (IN_NUM > 1)
                    input_data[1] = input2[ptr_src];
                ++ptr_src;
            }

            for (vx_uint16 inId = 0; inId < IN_NUM; inId++) {
#pragma HLS unroll
                for (vx_uint16 v = 0; v < VEC_NUM_IN; v++) {
#pragma HLS unroll

                    input_buffer[inId][v] = static_cast<bufT>(input_data[inId].pixel[v]);
                }
            }

            for (vx_uint16 inId = 0; inId < IN_NUM; inId++) {
#pragma HLS unroll

                // Read data from line_buffer to buffer
                ReadFromLineBuffer<bufT, BufferType, BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(
                    input_buffer[inId], linebuffer[inId], buffer[inId], x);

                // Write data from buffer to line_buffer
                WriteToLineBuffer<bufT, BufferType, BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(buffer[inId],
                                                                                                 linebuffer[inId], x);

                // Move sliding window with replicated/constant border
                SlidingWindow<bufT, VEC_NUM_IN, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE>(
                    buffer[inId], window[inId], x, y);
            }

            // Interpret the data with the original input data type.
            InType(&filter_window)[IN_NUM][KERN_SIZE][WIN_COLS] =
                *reinterpret_cast<InType(*)[IN_NUM][KERN_SIZE][WIN_COLS]>(&window);

            if (OUT_NUM > 0) {
                ComputeFilter<InType, OutType, KernType, KernModType, VEC_NUM_IN, VEC_NUM_OUT, KERN_NUM, KERN_TYPE1,
                              PRECISION, KERN_SIZE, STEP, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS,
                              IN_NUM, 0, QUANTIZATION>(filter_window, kernel, kernel_mult, kernel_shift,
                                                       kernel_modifier, vec_output_data[0], x, y);
            }

            if (OUT_NUM > 1) {
                ComputeFilter<InType, OutType, KernType, KernModType, VEC_NUM_IN, VEC_NUM_OUT, KERN_NUM, KERN_TYPE2,
                              PRECISION, KERN_SIZE, STEP, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS,
                              IN_NUM, 1, QUANTIZATION>(filter_window, kernel, kernel_mult, kernel_shift,
                                                       kernel_modifier, vec_output_data[1], x, y);
            }

            // Write output data to global memory
            if ((y >= KERN_RAD) && (x >= OHD_COLS) &&     //
                (((y - KERN_RAD) % (STEP + 1)) == 0) &&   //
                (((x - OHD_COLS) % STEP_VEC_OUT) == 0)) { //
                if (OUT_NUM > 0)
                    output1[ptr_dst] = vec_output_data[0];
                if (OUT_NUM > 1)
                    output2[ptr_dst] = vec_output_data[1];
                ++ptr_dst;
            }
        }
    }
}

/*
 *
 */
template <typename InType, typename OutType, typename KernType, typename KernModType, vx_uint32 VEC_PIX_IN,
          vx_uint32 VEC_PIX_OUT, vx_uint16 VEC_NUM_IN, vx_uint16 VEC_NUM_OUT, vx_uint16 KERN_NUM,
          HIFLIPVX::FilterOperation KERN_TYPE1, HIFLIPVX::FilterOperation KERN_TYPE2, vx_hint_e PRECISION,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, vx_uint16 STEP, vx_border_e BORDER_TYPE,
          InType THRESH_LOWER, InType THRESH_UPPER, vx_uint16 IN_NUM, vx_uint16 OUT_NUM, vx_uint32 QUANTIZATION>
void ComputeFilter2dFunc_(vx_image_data<InType, VEC_NUM_IN> input1[VEC_PIX_IN],
                          vx_image_data<InType, VEC_NUM_IN> input2[VEC_PIX_IN],
                          vx_image_data<OutType, VEC_NUM_OUT> output1[VEC_PIX_OUT],
                          vx_image_data<OutType, VEC_NUM_OUT> output2[VEC_PIX_OUT],
                          KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult,
                          const vx_uint32 kernel_shift, const KernModType kernel_modifier) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 STEP_VEC_OUT = ((STEP + 1) > VEC_NUM_IN) ? ((STEP + 1) / VEC_NUM_IN) : (1);
    const vx_uint16 WIN_COLS = (KERN_RAD << 1) + VEC_NUM_IN + (VEC_NUM_IN - (KERN_RAD % VEC_NUM_IN)) % VEC_NUM_IN;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;

    // Linebuffer (all inputs)
    vx_data_pack<InType, (KERN_SIZE - 1) * VEC_NUM_IN> linebuffer[IN_NUM][VEC_COLS];
    HIFLIPVX_DATAPACK(linebuffer);

    // Sliding window (all inputs)
    InType window[IN_NUM][KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable = window complete dim = 0

    vx_uint32 ptr_src = 0;
    vx_uint32 ptr_dst = 0;

    // Compute the filter (pipelined)
    for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; ++y) {
        for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // Input & Output vector
            vx_image_data<InType, VEC_NUM_IN> input_data[IN_NUM];
            vx_image_data<OutType, VEC_NUM_OUT> output_data[OUT_NUM];

            // Read input data from global memory
            if ((y < IMG_ROWS) && (x < VEC_COLS)) {
                if (IN_NUM > 0)
                    input_data[0] = input1[ptr_src];
                if (IN_NUM > 1)
                    input_data[1] = input2[ptr_src];
                ++ptr_src;
            }

            // linebuffer and sliding window
            for (vx_uint16 inId = 0; inId < IN_NUM; ++inId) {
#pragma HLS unroll

                //
                InType input_buffer[VEC_NUM_IN];
#pragma HLS array_partition variable = input_buffer complete dim = 0
                InType buffer[KERN_SIZE][VEC_NUM_IN];
#pragma HLS array_partition variable = buffer complete dim = 0

                //
                for (vx_uint16 v = 0; v < VEC_NUM_IN; ++v) {
#pragma HLS unroll
                    input_buffer[v] = input_data[inId].pixel[v];
                }

                ReadFromLineBuffer_<InType, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(input_buffer, linebuffer[inId], buffer, x);
                WriteToLineBuffer_<InType, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(buffer, linebuffer[inId], x);
                SlidingWindow<InType, VEC_NUM_IN, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE>(
                    buffer, window[inId], x, y);
            }

            // Compute filter for 1 or 2 Outputs
            for (vx_uint16 outId = 0; outId < OUT_NUM; ++outId) {
#pragma HLS unroll
                if (OUT_NUM > 0 && outId == 0) {
                    ComputeFilter<InType, OutType, KernType, KernModType, VEC_NUM_IN, VEC_NUM_OUT, KERN_NUM, KERN_TYPE1,
                                  PRECISION, KERN_SIZE, STEP, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS,
                                  IN_NUM, 0, QUANTIZATION>(window, kernel, kernel_mult, kernel_shift, kernel_modifier,
                                                           output_data[0], x, y);
                }
                if (OUT_NUM > 1 && outId == 1) {
                    ComputeFilter<InType, OutType, KernType, KernModType, VEC_NUM_IN, VEC_NUM_OUT, KERN_NUM, KERN_TYPE2,
                                  PRECISION, KERN_SIZE, STEP, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS,
                                  IN_NUM, 1, QUANTIZATION>(window, kernel, kernel_mult, kernel_shift, kernel_modifier,
                                                           output_data[1], x, y);
                }
            }

            // Write output data to global memory
            if ((y >= KERN_RAD) && (x >= OHD_COLS) &&     //
                (((y - KERN_RAD) % (STEP + 1)) == 0) &&   //
                (((x - OHD_COLS) % STEP_VEC_OUT) == 0)) { //
                if (OUT_NUM > 0)
                    output1[ptr_dst] = output_data[0];
                if (OUT_NUM > 1)
                    output2[ptr_dst] = output_data[1];
                ++ptr_dst;
            }
        }
    }
}

/** @brief  Computes multiple 2d convolution filter of kernel_type type
@param InType       Data type of the input image (8-bit, 16-bit, 32-bit)
@param OutType      Data type of the output image (8-bit, 16-bit, 32-bit)
@param KernType     Data type of the kernel (8-bit, 16-bit, 32-bit)
@param VEC_PIX      The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image IMG_COLS
@param IMG_ROWS     Image height
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param input        Input image
@param output1      1. Output image
@param output2      2. Output image
@param kernel       The convolution kernel
@param kernel_mult  Resulting pixels are 1. multiplied by this amount
@param kernel_shift Resulting pixels are 2. shifted to the right by this amount
@param kernel_type  The type of convolution
*/
template <typename InType, typename OutType, typename KernType, typename KernModType, vx_uint32 VEC_PIX_IN,
          vx_uint32 VEC_PIX_OUT, vx_uint16 VEC_NUM_IN, vx_uint16 VEC_NUM_OUT, vx_uint16 KERN_NUM,
          HIFLIPVX::FilterOperation KERN_TYPE1, HIFLIPVX::FilterOperation KERN_TYPE2, vx_hint_e PRECISION,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, vx_uint16 KERNEL_SCALE, vx_uint16 STEP,
          vx_border_e BORDER_TYPE, InType THRESH_LOWER, InType THRESH_UPPER, vx_uint16 IN_NUM, vx_uint16 OUT_NUM,
          vx_uint32 QUANTIZATION>
void ComputeFilter2d(vx_image_data<InType, VEC_NUM_IN> input1[VEC_PIX_IN],
                     vx_image_data<InType, VEC_NUM_IN> input2[VEC_PIX_IN],
                     vx_image_data<OutType, VEC_NUM_OUT> output1[VEC_PIX_OUT],
                     vx_image_data<OutType, VEC_NUM_OUT> output2[VEC_PIX_OUT],
                     KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult,
                     const vx_uint32 kernel_shift, const KernModType kernel_modifier) {
#pragma HLS INLINE

    const vx_uint16 SCALED_KERNEL_SIZE = SCALE_KERNEL(KERN_SIZE, KERNEL_SCALE);

    const vx_bool supported_scale_kernel_1 =
        (KERN_TYPE1 == HIFLIPVX::DOH) || (KERN_TYPE1 == HIFLIPVX::GAUSSIAN_FILTER) ||
        (KERN_TYPE1 == HIFLIPVX::DERIVATIVE_X) || (KERN_TYPE1 == HIFLIPVX::DERIVATIVE_Y) ||
        (KERN_TYPE1 == HIFLIPVX::CUSTOM_CONVOLUTION_U) || (KERN_TYPE1 == HIFLIPVX::CUSTOM_CONVOLUTION_S) ||
        (KERN_TYPE1 == HIFLIPVX::NON_MAX_SUPPRESSION);
    const vx_bool supported_scale_kernel_2 =
        (KERN_TYPE2 == HIFLIPVX::DOH) || (KERN_TYPE2 == HIFLIPVX::GAUSSIAN_FILTER) ||
        (KERN_TYPE2 == HIFLIPVX::DERIVATIVE_X) || (KERN_TYPE2 == HIFLIPVX::DERIVATIVE_Y) ||
        (KERN_TYPE2 == HIFLIPVX::CUSTOM_CONVOLUTION_U) || (KERN_TYPE1 == HIFLIPVX::CUSTOM_CONVOLUTION_S) ||
        (KERN_TYPE2 == HIFLIPVX::NON_MAX_SUPPRESSION);
    const vx_bool supported_kernel_sizes = (SCALED_KERNEL_SIZE == 3) || (SCALED_KERNEL_SIZE == 5) ||
                                           (SCALED_KERNEL_SIZE == 7) || (SCALED_KERNEL_SIZE == 9) ||
                                           (SCALED_KERNEL_SIZE == 11) || (SCALED_KERNEL_SIZE == 13) ||
                                           (SCALED_KERNEL_SIZE == 15) || (SCALED_KERNEL_SIZE == 17);
    const vx_bool supported_step =
        (((STEP + 1) % VEC_NUM_IN == STEP + 1) &&
         (VEC_NUM_IN % (STEP + 1) == 0)) || // vector size is multiple of step + 1
        (((STEP + 1) % VEC_NUM_IN == 0) &&
         (VEC_NUM_IN % (STEP + 1) == VEC_NUM_IN)) ||                        // step + 1 is multiple of vector size
        (((STEP + 1) % VEC_NUM_IN == 0) && (VEC_NUM_IN % (STEP + 1) == 0)); // step + 1 equal vector size
    const vx_bool supported_vec_out =
        (VEC_NUM_OUT == 1) || (VEC_NUM_OUT == 2) || (VEC_NUM_OUT == 4) || (VEC_NUM_OUT == 8);

    STATIC_ASSERT(KERNEL_SCALE > 0, scale_cannot_be_zero);
    // check if scaling for given kernel is supported
    STATIC_ASSERT((KERNEL_SCALE == 1) || supported_scale_kernel_1, scaling_for_first_kernel_type_is_not_supported);
    STATIC_ASSERT((KERNEL_SCALE == 1) || supported_scale_kernel_2, scaling_for_second_kernel_type_is_not_supported);
    // check if resulting kernel exceeds maximum supported kernel size
    STATIC_ASSERT(supported_kernel_sizes, scaled_kernel_size_is_not_supported);
    STATIC_ASSERT(supported_vec_out, output_vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(supported_step, step_plus_1_has_to_be_a_multiple_of_input_vector_size_or_vise_versa);

    // scale kernel
    KernType scaled_kernel[KERN_NUM][SCALED_KERNEL_SIZE][SCALED_KERNEL_SIZE] = {};

    for (vx_uint8 i = 0; i < KERN_NUM; i++) {
#pragma HLS UNROLL
        ComputeScaledKernel<KernType, KERN_SIZE, KERNEL_SCALE>(kernel[i], scaled_kernel[i]);
    }

    // Check function parameters/types
    const vx_uint16 kern_size = sizeof(KernType);
    const vx_uint16 src_size = sizeof(InType);
    const vx_uint16 dst_size = sizeof(OutType);
    const bool allowed_kernel_size =
        (KERN_SIZE == 3) || (KERN_SIZE == 5) || (KERN_SIZE == 7) || (KERN_SIZE == 9) || (KERN_SIZE == 11);
    const bool allowed_vector_size = (VEC_NUM_IN == 1) || (VEC_NUM_IN == 2) || (VEC_NUM_IN == 4) || (VEC_NUM_IN == 8);
    const bool allowed_data_type = (src_size == 1) || (src_size == 2) || (src_size == 4);
    const bool allowed_kernel_type = (kern_size == 1) || (kern_size == 2);
    const bool allowed_kernel_num = (KERN_NUM == 1) || (KERN_NUM == 2) || (KERN_NUM == 3);
    STATIC_ASSERT(allowed_kernel_size, kernel_size_must_be_3_5_7_9_11);
    STATIC_ASSERT((IMG_COLS % VEC_NUM_IN == 0), image_colums_are_not_multiple_of_vector_size);
    STATIC_ASSERT((src_size <= dst_size), size_of_dst_cannot_be_smaller_than_src);
    STATIC_ASSERT(allowed_vector_size, vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(allowed_data_type, data_type_must_be_8_16_32_bit);
    STATIC_ASSERT(allowed_kernel_type, kernel_data_type_must_be_8_16_bit);
    STATIC_ASSERT(allowed_kernel_num, kernel_num_can_only_be_1_2_or_3);

    // Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
    ComputeFilter2dFunc_<InType, OutType, KernType, KernModType, VEC_PIX_IN, VEC_PIX_OUT, VEC_NUM_IN, VEC_NUM_OUT,
                         KERN_NUM, KERN_TYPE1, KERN_TYPE2, PRECISION, IMG_COLS, IMG_ROWS, SCALED_KERNEL_SIZE, STEP,
                         BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>(
        input1, input2, output1, output2, scaled_kernel, kernel_mult, kernel_shift, kernel_modifier);
}

template <typename InType, typename OutType, typename KernType, typename KernModType, vx_uint32 VEC_PIX,
          vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, HIFLIPVX::FilterOperation KERN_TYPE1,
          HIFLIPVX::FilterOperation KERN_TYPE2, vx_hint_e PRECISION, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint16 KERN_SIZE, vx_border_e BORDER_TYPE, InType THRESH_LOWER, InType THRESH_UPPER, vx_uint16 IN_NUM,
          vx_uint16 OUT_NUM, vx_uint32 QUANTIZATION>
void ComputeFilter2d_(vx_image_data<InType, VEC_NUM> input1[VEC_PIX], vx_image_data<InType, VEC_NUM> input2[VEC_PIX],
                      vx_image_data<OutType, VEC_NUM> output1[VEC_PIX],
                      vx_image_data<OutType, VEC_NUM> output2[VEC_PIX], KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE],
                      const vx_uint64 kernel_mult, const vx_uint32 kernel_shift, const KernModType kernel_modifier) {
#pragma HLS INLINE

    // Check function parameters/types
    const vx_uint16 kern_size = sizeof(KernType);
    const vx_uint16 src_size = sizeof(InType);
    const vx_uint16 dst_size = sizeof(OutType);
    const bool allowed_kernel_size =
        (KERN_SIZE == 3) || (KERN_SIZE == 5) || (KERN_SIZE == 7) || (KERN_SIZE == 9) || (KERN_SIZE == 11);
    const bool allowed_vector_size = (VEC_NUM == 1) || (VEC_NUM == 2) || (VEC_NUM == 4) || (VEC_NUM == 8);
    const bool allowed_data_type = (src_size == 1) || (src_size == 2) || (src_size == 4);
    const bool allowed_kernel_type = (kern_size == 1) || (kern_size == 2) || (kern_size == 4);
    const bool allowed_kernel_num = (KERN_NUM == 1) || (KERN_NUM == 2);
    STATIC_ASSERT(allowed_kernel_size, kernel_size_must_be_3_5_7_9_11);
    STATIC_ASSERT((IMG_COLS % VEC_NUM == 0), image_colums_are_not_multiple_of_vector_size);
    STATIC_ASSERT((src_size == dst_size), size_of_in_and_out_type_must_be_equal);
    STATIC_ASSERT(allowed_vector_size, vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(allowed_data_type, data_type_must_be_8_16_32_bit);
    STATIC_ASSERT(allowed_kernel_type, kernel_data_type_must_be_8_16_32_bit);
    STATIC_ASSERT(allowed_kernel_num, kernel_num_can_only_be_1__or_2);

    // Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
    ComputeFilter2dFunc_<InType, OutType, KernType, KernModType, VEC_PIX, VEC_PIX, VEC_NUM, VEC_NUM, KERN_NUM,
                         KERN_TYPE1, KERN_TYPE2, PRECISION, IMG_COLS, IMG_ROWS, KERN_SIZE, 0, BORDER_TYPE, THRESH_LOWER,
                         THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>(input1, input2, output1, output2, kernel,
                                                                      kernel_mult, kernel_shift, kernel_modifier);
}
/***************************************************************************************************************************************************/
/* Filter Function Definition */
/***************************************************************************************************************************************************/

/*********************************************************************************************************************/
/** @brief  Computes a Box filter over a window of the input image.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param SEPARABLE   Is true, if separable filter should be used
@param input       Input image
@param output      Output image
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH,        //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE> //
void HwBox(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                               //
           vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {                            //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

    // Kernel Type
    typedef vx_uint16 KernelType;

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    if (SEPARABLE == vx_false_e) {

        // Function Input
        KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

        // Compute kernel and normalization (Compile-Time)
        for (vx_uint8 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
            for (vx_uint8 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
                kernel_vector[0][y][x] = 1;
            }
        }
        ComputeNormalization2d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

        // Compute Filter
        ComputeFilter2d_<ScalarType, ScalarType, KernelType, vx_uint8, VEC_PIX, VEC_NUM,                           //
                         KERN_NUM, HIFLIPVX::BOX_FILTER, HIFLIPVX::BOX_FILTER, VX_HINT_PERFORMANCE_DEFAULT, WIDTH, //
                         HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,              //
                         QUANTIZATION>(input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift,        //
                                       kernel_modifier);                                                           //

    } else {

        // Function Input
        KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

        // Compute kernel and normalization (Compile-Time)
        for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
            kernel_vector[i] = 1;
        }
        ComputeNormalization1d<KernelType, KERN_SIZE>(kernel_vector, kernel_mult, kernel_shift);

        // Compute Filter
        ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::BOX_FILTER, //
                        BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);                    //
    }
}

/*********************************************************************************************************************/
/** @brief  Convolves the input with the client supplied convolution matrix.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param conv        The custom convolution kernel
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>             //
void HwConvolve(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                   //
                vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX],                  //
                const ScalarType conv[KERN_SIZE][KERN_SIZE]) {                       //
#pragma HLS INLINE

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    // Check, if input or output is signed
    const HIFLIPVX::FilterOperation operation = (std::numeric_limits<ScalarType>::is_signed)
                                                    ? (HIFLIPVX::CUSTOM_CONVOLUTION_S)
                                                    : (HIFLIPVX::CUSTOM_CONVOLUTION_U);

    // Function Input
    ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute kernel and normalization (Compile-Time)
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            kernel_vector[0][i][j] = conv[i][j];
        }
    }
    ComputeNormalization2d<ScalarType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

    // Compute Filter
    ComputeFilter2d_<ScalarType, ScalarType, ScalarType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,          //
                     operation, operation, VX_HINT_PERFORMANCE_DEFAULT, WIDTH,                          //
                     HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,       //
                     QUANTIZATION>(input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift, //
                                   kernel_modifier);                                                    //
}

/*********************************************************************************************************************/
/** @brief  Implements Dilation, which grows the white space in a Boolean image.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>             //
void HwDilate(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                     //
              vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {                  //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

    // Kernel Parameters
    typedef vx_uint16 KernelType;

    // Function Input
    KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Kernel normalization and type
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;

    // Compute Filter
    ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::DILATE_IMAGE, //
                    BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);                      //
}

/*********************************************************************************************************************/
/** @brief  Implements Erosion, which shrinks the white space in a Boolean image.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>             //
void HwErode(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                      //
             vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {                   //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

    // Kernel Parameters
    typedef vx_uint16 KernelType;

    // Function Input
    KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Kernel normalization and type
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;

    // Compute Filter
    ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::ERODE_IMAGE, //
                    BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);                     //
}

/*********************************************************************************************************************/
/** @brief  Computes a Gaussian filter over a window of the input image.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param SEPARABLE   Is true, if separable filter should be used
@param input       Input image
@param output      Output image
@param output_fw   forward input image
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,  //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE, vx_bool FORWARD_COEFFICIENTS> //
void HwGaussian(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                                      //
                vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX],                                     //
                vx_image_data<ScalarType, VEC_NUM> output_fw[VEC_PIX]) {                                //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

    // Kernel Type
    typedef vx_uint16 KernelType;

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1 + FORWARD_COEFFICIENTS;

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    if (SEPARABLE == vx_false_e || FORWARD_COEFFICIENTS == vx_true_e) {

        // Function Input
        KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

        // Compute kernel and normalization (Compile-Time)
        ComputeGaussianKernels<KernelType, KERN_SIZE>(kernel_vector[0]);
        ComputeNormalization2d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

        // Compute Filter
        ComputeFilter2d_<ScalarType, ScalarType, KernelType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,                  //
                         HIFLIPVX::GAUSSIAN_FILTER, HIFLIPVX::FORWARD1, VX_HINT_PERFORMANCE_DEFAULT, WIDTH, HEIGHT, //
                         KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,                       //
                         QUANTIZATION>(input, NULL, output, output_fw, kernel_vector, kernel_mult, kernel_shift,    //
                                       kernel_modifier);                                                            //

    } else {

        // Function Input
        KernelType kernel_vector[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

        // Compute kernel and normalization (Compile-Time)
        ComputeGaussianKernels<KernelType, KERN_SIZE>(kernel_vector);
        ComputeNormalization1d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

        // Compute Filter
        ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, //
                        HIFLIPVX::GAUSSIAN_FILTER, BORDER_TYPE>                             //
            (input, output, kernel_vector[0], kernel_mult, kernel_shift);                   //
    }
}

/*********************************************************************************************************************/
/** @brief  Computes a median pixel value over a window of the input image.
The output image dimensions should be the same as the dimensions of the input image.
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>             //
void HwMedian(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                     //
              vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {                  //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

    // Kernel Type
    typedef vx_uint16 KernelType;

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    const HIFLIPVX::FilterOperation operation =
        (KERN_SIZE == 3) ? (HIFLIPVX::MEDIAN_FILTER_3x3) : (HIFLIPVX::MEDIAN_FILTER_NxN);

    // Function Input
    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d_<ScalarType, ScalarType, KernelType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,          //
                     operation, operation, VX_HINT_PERFORMANCE_DEFAULT, WIDTH, HEIGHT,                  //
                     KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION> //
        (input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift, kernel_modifier);         //
}

/*********************************************************************************************************************/
/** @brief  Implements the Sobel Image Filter Kernel.
The output images dimensions should be the same as the dimensions of the input image.
@param SrcType     Data type of the input image (unsigned)(8-bit, 16-bit, 32-bit)
@param DstType     Data type of the output image (signed)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       The input image
@param output1     The output image (x derivative)
@param output2     The output image (y derivative)
*/
template <typename InType, typename OutType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM,         //
          vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE> //
void HwSobel(vx_image_data<InType, VEC_NUM> input[VEC_PIX],                               //
             vx_image_data<OutType, VEC_NUM> output1[VEC_PIX],                            //
             vx_image_data<OutType, VEC_NUM> output2[VEC_PIX]) {                          //
#pragma HLS INLINE

    // Check function parameters/types
    STATIC_ASSERT(std::numeric_limits<InType>::is_signed == false, input_must_be_unsigned);
    STATIC_ASSERT(std::numeric_limits<OutType>::is_signed == true, output_must_be_signed);

    // Kernel Type
    typedef vx_int16 KernelType;

    // template Parameters
    const vx_uint8 KERN_NUM = 2;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const InType THRESH_LOWER = 0;
    const InType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 2;

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    // Function Input
    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute kernel and normalization (Compile-Time)
    ComputeSobelKernels<KernelType, KERN_SIZE>(kernel_vector[0], kernel_vector[1]);
    ComputeNormalization2d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

    // Compute Filter
    ComputeFilter2d_<InType, OutType, KernelType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,                          //
                     HIFLIPVX::DERIVATIVE_X, HIFLIPVX::DERIVATIVE_Y, VX_HINT_PERFORMANCE_DEFAULT, WIDTH, HEIGHT, //
                     KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>          //
        (input, NULL, output1, output2, kernel_vector, kernel_mult, kernel_shift, kernel_modifier);              //
}

/*********************************************************************************************************************/
/** @brief  Find local maxima in an image, or otherwise suppress pixels that are not local maxima.
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   The size of window over which to perform the localized non-maxima suppression (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param mask        Constrict suppression to a ROI.
*/
template <typename DataType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, //
          vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>           //
void HwNonMaxSuppression(vx_image_data<DataType, VEC_NUM> input[VEC_PIX],          //
                         vx_image_data<DataType, VEC_NUM> output[VEC_PIX],         //
                         const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {              //
#pragma HLS INLINE

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const DataType THRESH_LOWER = 0;
    const DataType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    // Function Input
    vx_uint8 mask_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = mask_vector complete dim = 0

    // Compute kernel and normalization (Compile-Time)
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            mask_vector[0][i][j] = mask[i][j];
        }
    }

    // Compute Filter
    ComputeFilter2d_<DataType, DataType, vx_uint8, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,                        //
                     HIFLIPVX::NON_MAX_SUPPRESSION, HIFLIPVX::NON_MAX_SUPPRESSION, VX_HINT_PERFORMANCE_DEFAULT, //
                     WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,        //
                     QUANTIZATION>(input, NULL, output, NULL, mask_vector, kernel_mult, kernel_shift,           //
                                   kernel_modifier);                                                            //
}

/*********************************************************************************************************************/
/*
 * Segment Test Detector
 */
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM,              //
          vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>            //
void HwSegmentTestDetector(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],    //
                           vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) { //
#pragma HLS INLINE

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 KERN_SIZE = 7;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    // Function Input
    ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d_<ScalarType, ScalarType, ScalarType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,                      //
                     HIFLIPVX::SEGMENT_TEST_DETECTOR, HIFLIPVX::SEGMENT_TEST_DETECTOR, VX_HINT_PERFORMANCE_DEFAULT, //
                     WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,            //
                     QUANTIZATION>(input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift,             //
                                   kernel_modifier);                                                                //
}

/*********************************************************************************************************************/
/*
 * Hysteresis
 */
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,   //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, ScalarType THRESH_LOWER, ScalarType THRESH_UPPER> //
void HwHysteresis(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],                                     //
                  vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {                                  //
#pragma HLS INLINE

    // template Parameters
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint64 kernel_mult = 1;
    const vx_uint32 kernel_shift = 0;
    const vx_uint8 kernel_modifier = 0;

    // Function Input
    ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d_<ScalarType, ScalarType, ScalarType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM,               //
                     HIFLIPVX::HYSTERESIS, HIFLIPVX::HYSTERESIS, VX_HINT_PERFORMANCE_DEFAULT, WIDTH, HEIGHT, //
                     KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>      //
        (input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift, kernel_modifier);              //
}

/*********************************************************************************************************************/
/*
 * Conductivity function
 */
template <typename DataType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, //
          vx_border_e BORDER_TYPE, HIFLIPVX::FilterOperation COEFFICIENT, vx_hint_e PRECISION>            //
void HwConductivity(vx_image_data<DataType, VEC_NUM> input[VEC_PIX],                                      //
                    vx_image_data<DataType, VEC_NUM> output[VEC_PIX],                                     //
                    vx_image_data<contrastSquareT, 1> csquare[1]) {                                       //
#pragma HLS INLINE

    const vx_bool is_conduction_coefficient = (COEFFICIENT == HIFLIPVX::PERONA_MALIK_WIDE_REGION);

    STATIC_ASSERT(is_conduction_coefficient, filter_operation_is_not_a_conduction_coefficent_function);
    STATIC_ASSERT(std::numeric_limits<DataType>::is_signed == false, input_data_type_must_be_unsigned);

    // template parameter
    typedef vx_int8 KernelType;
    const vx_uint8 KERN_SIZE = 3;
    const vx_uint8 KERN_NUM = 2;
    const DataType THRESH_LOWER = 0;
    const DataType THRESH_UPPER = 0;
    const vx_uint8 KERNEL_SCALE = 1;
    const vx_uint8 STEP = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 5;

    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE] = SCHARR_XY_3x3;
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d_<DataType, DataType, KernelType, contrastSquareT, VEC_PIX, VEC_NUM, KERN_NUM,   //
                     COEFFICIENT, COEFFICIENT, PRECISION, IMG_COLS, IMG_ROWS, KERN_SIZE,            //
                     BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>        //
        (input, NULL, output, NULL, kernel_vector, kernel_mult, kernel_shift, csquare[0].pixel[0]); //
}

/*********************************************************************************************************************/
/*
 * Fast Explicit Diffusion
 */
template <typename DataType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, //
          vx_uint32 STEP_SIZE, vx_border_e BORDER_TYPE, vx_bool FORWARD_COEFFICIENTS>                     //
void HwFED(vx_image_data<DataType, VEC_NUM> input_lt[VEC_PIX],                                            //
           vx_image_data<DataType, VEC_NUM> input_coeff[VEC_PIX],                                         //
           vx_image_data<DataType, VEC_NUM> output_lt[VEC_PIX],                                           //
           vx_image_data<DataType, VEC_NUM> output_fw_coeff[VEC_PIX]) {                                   //
#pragma HLS INLINE

    STATIC_ASSERT(std::numeric_limits<DataType>::is_signed == false, data_type_must_be_unsigned);

    // template parameter
    typedef vx_int8 KernelType;
    typedef vx_uint8 KernelModifier;
    const vx_uint8 KERN_SIZE = 3;
    const vx_uint8 KERN_NUM = 1;
    const DataType THRESH_LOWER = 0;
    const DataType THRESH_UPPER = 0;
    const vx_uint8 KERNEL_SCALE = 1;
    const vx_uint8 STEP = 0;
    const vx_uint16 IN_NUM = 2;
    const vx_uint16 OUT_NUM = 1 + FORWARD_COEFFICIENTS; // add one output if coefficent shall be forwarded

    // function arguments
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;
    vx_uint8 kernel_modifier = 0;

    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE] = {};
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d_<DataType, DataType, KernelType, KernelModifier, VEC_PIX, VEC_NUM, KERN_NUM,                    //
                     HIFLIPVX::FED, HIFLIPVX::FORWARD2, VX_HINT_PERFORMANCE_DEFAULT, IMG_COLS, IMG_ROWS, KERN_SIZE, //
                     BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, STEP_SIZE>                           //
        (input_lt, input_coeff, output_lt, output_fw_coeff, kernel_vector, kernel_mult, kernel_shift, kernel_modifier);
}

/*********************************************************************************************************************/

/** @brief  Implements a scalable 3x3 Scharr Image Filter Kernel.
The output images dimensions should be the same as the dimensions of the input image.
@param SrcType     Data type of the input image (unsigned)(8-bit, 16-bit, 32-bit)
@param DstType     Data type of the output image (signed)(8-bit, 16-bit, 32-bit)
@param VEC_PIX     The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       The input image
@param output1     The output image (x derivative)
@param output2     The output image (y derivative)
*/
template <typename InType, typename OutType, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT,    //
          vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, //
          vx_uint8 KERNEL_SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                     //
void HwScalableScharr3x3(vx_image_data<InType, VEC_NUM_IN> input[VEC_PIX_IN],                //
                         vx_image_data<OutType, VEC_NUM_OUT> output_lx[VEC_PIX_OUT],         //
                         vx_image_data<OutType, VEC_NUM_OUT> output_ly[VEC_PIX_OUT]) {       //
#pragma HLS INLINE

    STATIC_ASSERT(std::numeric_limits<InType>::is_signed == false, input_must_be_unsigned);
    STATIC_ASSERT(std::numeric_limits<OutType>::is_signed == true, output_must_be_signed);

    // template parameter
    typedef vx_int8 KernelType;
    typedef vx_uint8 KernModType;
    const vx_uint8 KERN_NUM = 2;
    const vx_uint8 KERN_SIZE = 3;
    const InType THRESH_LOWER = 0;
    const InType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 1;
    const vx_uint16 OUT_NUM = 2;

    // function arguments
    const vx_uint8 kernel_mult = 1;
    const vx_uint8 kernel_shift = 5;
    const vx_uint8 kernel_modifier = 0;

    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE] = SCHARR_XY_3x3;
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    // Compute Filter
    ComputeFilter2d<InType, OutType, KernelType, KernModType, VEC_PIX_IN, VEC_PIX_OUT, VEC_NUM_IN, VEC_NUM_OUT,   //
                    KERN_NUM, HIFLIPVX::DERIVATIVE_X, HIFLIPVX::DERIVATIVE_Y, VX_HINT_PERFORMANCE_DEFAULT,        //
                    IMG_COLS, IMG_ROWS, KERN_SIZE, KERNEL_SCALE, STEP, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER,   //
                    IN_NUM, OUT_NUM, QUANTIZATION>(input, NULL, output_lx, output_ly, kernel_vector, kernel_mult, //
                                                   kernel_shift, kernel_modifier);                                //
}

/*********************************************************************************************************************/
/*
 * Determinant of the Hessian
 */
template <typename InType, typename OutType, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT,    //
          vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, //
          vx_uint8 KERNEL_SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE, vx_bool WITH_FIRST_ORDER_DERIVATION>
void HwDetHessian(vx_image_data<InType, VEC_NUM_IN> input_lx[VEC_PIX_IN],
                  vx_image_data<InType, VEC_NUM_IN> input_ly[VEC_PIX_IN],
                  vx_image_data<OutType, VEC_NUM_OUT> output[VEC_PIX_OUT]) {
#pragma HLS INLINE

    // input: "signed" if computing 1st and 2nd order derivative || "unsigned" if computinf 2nd order derivative
    STATIC_ASSERT(std::numeric_limits<InType>::is_signed != WITH_FIRST_ORDER_DERIVATION, input_wrong_signedness);
    STATIC_ASSERT(std::numeric_limits<OutType>::is_signed == true, output_must_be_signed);

    // template parameter
    typedef vx_int16 KernelType;
    typedef vx_uint8 KernModType;

    const vx_uint8 KERN_NUM = 2 + WITH_FIRST_ORDER_DERIVATION;
    const vx_uint8 KERN_SIZE = 3 + (WITH_FIRST_ORDER_DERIVATION << 1);
    const InType THRESH_LOWER = 0;
    const InType THRESH_UPPER = 0;
    const vx_uint8 QUANTIZATION = 0;
    const vx_uint16 IN_NUM = 2 - WITH_FIRST_ORDER_DERIVATION;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint8 kernel_mult = 1;
    // normally shift by 5 (without first order derivation) or 10 (with first order derivation),
    // but to reduce lost of precision the final result of the Doh does a post shifting
    const vx_uint8 kernel_shift = (WITH_FIRST_ORDER_DERIVATION) ? (5) : (0);
    const vx_uint8 kernel_modifier = 0;

    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE] = SCHARR_XY_3x3;
#pragma HLS array_partition variable = kernel_vector complete dim = 0

    if (WITH_FIRST_ORDER_DERIVATION) {
        const KernelType scharr_x[3][3] = SCHARR_X_3x3;
        const KernelType scharr_y[3][3] = SCHARR_Y_3x3;
        // reinterpretation of data type to support KERN_SIZE=3 compilation,
        // in case the dimensions are wrong this branch is not accessed and therefore cannot cause any harm
        ConvolveKernel<KernelType, KernelType, 3, 3>(scharr_x, scharr_x,
                                                     reinterpret_cast<KernelType(*)[5]>(kernel_vector[DOH_XX]));
        ConvolveKernel<KernelType, KernelType, 3, 3>(scharr_x, scharr_y,
                                                     reinterpret_cast<KernelType(*)[5]>(kernel_vector[DOH_XY]));
        ConvolveKernel<KernelType, KernelType, 3, 3>(scharr_y, scharr_y,
                                                     reinterpret_cast<KernelType(*)[5]>(kernel_vector[DOH_YY]));
    }

    ComputeFilter2d<InType, OutType, KernelType, KernModType, VEC_PIX_IN, VEC_PIX_OUT, VEC_NUM_IN, VEC_NUM_OUT, //
                    KERN_NUM, HIFLIPVX::DOH, HIFLIPVX::DOH, VX_HINT_PERFORMANCE_DEFAULT, IMG_COLS, IMG_ROWS,    //
                    KERN_SIZE, KERNEL_SCALE, STEP, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM,    //
                    QUANTIZATION>(input_lx, input_ly, output, NULL, kernel_vector, kernel_mult, kernel_shift,   //
                                  kernel_modifier);
}

/*********************************************************************************************************************/
/*
 * Non Maximum Suppression with Orientation
 */
template <typename ScalarType, vx_uint32 VEC_PIX, vx_uint32 VEC_NUM, vx_uint32 IMG_COLS,      //
          vx_uint32 IMG_ROWS, vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>                //
void HwOrientedNonMaxSuppression(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],           //
                                 vx_image_data<ScalarType, VEC_NUM> input_no_window[VEC_PIX], //
                                 vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {        //
#pragma HLS INLINE

    STATIC_ASSERT(QUANTIZATION >= 3, quantization_of_minimum_3_needed);

    // template parameter
    typedef vx_int8 KernelType;
    typedef vx_uint8 KernModType;
    const vx_uint8 KERN_NUM = 1;
    const vx_uint8 KERN_SIZE = 3;
    const vx_uint8 SCALE = 1;
    const vx_uint8 STEP = 0;
    const ScalarType THRESH_LOWER = 0;
    const ScalarType THRESH_UPPER = 0;
    const vx_uint16 IN_NUM = 2;
    const vx_uint16 OUT_NUM = 1;

    // function arguments
    const vx_uint8 kernel_mult = 1;
    const vx_uint8 kernel_shift = 5;
    const KernModType kernel_modifier = 0;

    KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE] = {};

    // Compute Filter
    ComputeFilter2d_<ScalarType, ScalarType, KernelType, KernModType, VEC_PIX, VEC_NUM, KERN_NUM,          //
                     HIFLIPVX::ORIENTED_NON_MAX_SUPPRESSION, HIFLIPVX::ORIENTED_NON_MAX_SUPPRESSION,       //
                     VX_HINT_PERFORMANCE_DEFAULT, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE,              //
                     THRESH_LOWER, THRESH_UPPER, IN_NUM, OUT_NUM, QUANTIZATION>                            //
        (input, input_no_window, output, NULL, kernel_vector, kernel_mult, kernel_shift, kernel_modifier); //
}

#endif /* SRC_IMG_FILTER_CORE_H_ */
