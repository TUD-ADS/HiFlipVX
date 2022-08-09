/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_conversion_scale.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  Functions needed for the scale image function (FAST AREA INTERPOLATION).
 */

#ifndef __SRC_IMG_MT_CONVERSION_SCALE_H__
#define __SRC_IMG_MT_CONVERSION_SCALE_H__

#include "img_filter_window.h"

/**********************************************************************************************************************/
template <typename DataType, vx_uint8 SCALE_FACTOR>
DataType ScaleDownMean(const DataType window[SCALE_FACTOR][SCALE_FACTOR], const vx_uint16 x_dst, const vx_uint16 y_dst,
                       vx_uint16 &x_src, vx_uint16 &y_src) {
#pragma HLS INLINE

    typedef typename next_larger<DataType>::DataType sumT;

    // compute number of shifts at compile time, to replace division.
    const vx_uint8 LD_SCALE_FACTOR = int_floor_log2(SCALE_FACTOR);
    const vx_uint8 BIT_SHIFT = LD_SCALE_FACTOR + LD_SCALE_FACTOR;

    sumT sum = 0;

    for (vx_uint16 y = 0; y < SCALE_FACTOR; y++) {
#pragma HLS UNROLL
        for (vx_uint16 x = 0; x < SCALE_FACTOR; x++) {
#pragma HLS UNROLL
            sum += window[y][x];
        }
    }

    x_src = ((x_dst + 1) << LD_SCALE_FACTOR) - 1;
    y_src = ((y_dst + 1) << LD_SCALE_FACTOR) - 1;
    return static_cast<DataType>(sum >> BIT_SHIFT);
}

/**********************************************************************************************************************/
template <typename DataType, vx_uint16 COLS_SCALE, vx_uint16 ROWS_SCALE, vx_uint8 ACCURACY>
DataType ScaleDownNearest(const DataType element, const vx_uint16 x_dst, const vx_uint16 y_dst, vx_uint16 &x_src,
                          vx_uint16 &y_src) {
#pragma HLS INLINE
    x_src = (((((x_dst << 1) + 1) * COLS_SCALE) >> ACCURACY) - 1) >> 1;
    y_src = (((((y_dst << 1) + 1) * ROWS_SCALE) >> ACCURACY) - 1) >> 1;
    return element;
}

/**********************************************************************************************************************/
template <typename DataType, vx_uint16 COLS_SCALE, vx_uint16 ROWS_SCALE, vx_uint8 KERN_SIZE, vx_uint8 ACCURACY>
DataType ScaleDownBilinear(const DataType window[KERN_SIZE][KERN_SIZE], const vx_uint16 x_dst, const vx_uint16 y_dst,
                           vx_uint16 &x_src, vx_uint16 &y_src) {
#pragma HLS INLINE

    // constants
    const vx_uint32 X_SRC_STRT = (COLS_SCALE << 1) - 1;
    const vx_uint32 Y_SRC_STRT = (ROWS_SCALE << 1) - 1;
    const vx_uint32 X_SRC_STEP = COLS_SCALE * 4;
    const vx_uint32 Y_SRC_STEP = ROWS_SCALE * 4;
    const vx_uint32 MASK_FRAKTION = (1 << (ACCURACY + 2)) - 1;
    const vx_uint32 ONE = (1 << ACCURACY);

    const vx_uint8 TOP = 0;
    const vx_uint8 BOTTOM = 1;
    const vx_uint8 LEFT = 0;
    const vx_uint8 RIGHT = 1;

    const vx_uint32 x_fract = (x_src & MASK_FRAKTION) >> 2;
    const vx_uint32 y_fract = (y_src & MASK_FRAKTION) >> 2;

    x_src = ((X_SRC_STRT + x_dst * X_SRC_STEP) >> (ACCURACY + 2)) + 1;
    y_src = ((Y_SRC_STRT + y_dst * Y_SRC_STEP) >> (ACCURACY + 2)) + 1;

    const vx_uint32 tl = window[TOP][LEFT];
    const vx_uint32 tr = window[TOP][RIGHT];
    const vx_uint32 bl = window[BOTTOM][LEFT];
    const vx_uint32 br = window[BOTTOM][RIGHT];

    // Compute the Bilinear Interpolation (Saturated)
    const vx_uint16 tl_part = (vx_uint16)(((((ONE - x_fract) * (ONE - y_fract)) >> 10) * tl) >> 8);
    const vx_uint16 tr_part = (vx_uint16)(((((x_fract) * (ONE - y_fract)) >> 10) * tr) >> 8);
    const vx_uint16 bl_part = (vx_uint16)(((((ONE - x_fract) * (y_fract)) >> 10) * bl) >> 8);
    const vx_uint16 br_part = (vx_uint16)(((((x_fract) * (y_fract)) >> 10) * br) >> 8);
    const vx_uint16 sum = (tl_part + tr_part + bl_part + br_part) >> (2 * ACCURACY - 10 - 8);
    const vx_uint8 result = (vx_uint8)(MIN(sum, (vx_uint16)(255)));

    return result;
}

/**********************************************************************************************************************/
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS_IN, vx_uint16 IMG_ROWS_IN,
          vx_uint16 IMG_COLS_OUT, vx_uint16 IMG_ROWS_OUT, vx_uint16 KERN_SIZE, vx_uint16 WIN_COLS, vx_uint8 ACCURACY,
          vx_interpolation_type_e SCALE_TYPE>
void ScaleDown(DataType window[KERN_SIZE][WIN_COLS], vx_image_data<DataType, VEC_NUM_OUT> &output,
               const vx_uint16 x_dst, const vx_uint16 y_dst, vx_uint16 &x_src, vx_uint16 &y_src) {
#pragma HLS INLINE
    // Constants
    const vx_uint32 COLS_SCALE =
        (vx_uint32)(((vx_float64)IMG_COLS_IN / (vx_float64)IMG_COLS_OUT) * std::pow(2, ACCURACY) + 0.5);
    const vx_uint32 ROWS_SCALE =
        (vx_uint32)(((vx_float64)IMG_ROWS_IN / (vx_float64)IMG_ROWS_OUT) * std::pow(2, ACCURACY) + 0.5);

    const vx_uint16 VEC_COLS = IMG_COLS_IN / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;

    const vx_uint8 VEC_RATIO = VEC_NUM_IN / VEC_NUM_OUT;

    // Output
    vx_image_data<DataType, VEC_NUM_OUT> output_vector;

    // Compute KERN_NUM different filter with VEC_NUM elements per clock cycle
    for (vx_uint16 vecOutId = 0; vecOutId < VEC_NUM_OUT; vecOutId++) {
#pragma HLS unroll

        // Window for single vector element
        DataType kernel_window[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = kernel_window complete dim = 0

        vx_uint16 x_src_vec;
        vx_uint16 y_src_vec;

        // compute correct dst coordinate for vectorization
        const vx_uint16 x_dst_vec = x_dst + vecOutId;

        for (vx_uint16 vecRatId = 0; vecRatId < VEC_RATIO; vecRatId++) {
#pragma HLS unroll
            // Get window for single vector element
            for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
                for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
                    kernel_window[i][j] = window[i][j + vecOutId * KERN_SIZE];
                }
            }
        }

        // Compute filter for single vector element
        DataType result = 0;
        if (SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR) {
            // TODO result = ScaleDownNearest<DataType, COLS_SCALE, ROWS_SCALE, ACCURACY>(kernel_window[0][0],
            // x_dst_vec, y_dst, x_src_vec, y_src_vec);
        } else if (SCALE_TYPE == VX_INTERPOLATION_BILINEAR) {
            // TODO result = ScaleDownBilinear<DataType, COLS_SCALE, ROWS_SCALE, KERN_SIZE, ACCURACY>(kernel_window,
            // x_dst_vec, y_dst, x_src_vec, y_src_vec);
        } else if (SCALE_TYPE == VX_INTERPOLATION_AREA_FAST || SCALE_TYPE == VX_INTERPOLATION_AREA) {
            result = ScaleDownMean<DataType, KERN_SIZE>(kernel_window, x_dst_vec, y_dst, x_src_vec, y_src_vec);
        }
        output_vector.pixel[vecOutId] = result;

        // update src_x and src_y only for the last vector element
        if (vecOutId == VEC_NUM_OUT - 1) {
            x_src = x_src_vec;
            y_src = y_src_vec;
        }
    }

    // Set user(SOF) & last(EOF) (stride has also to be considered for EOF
    const vx_bool sof = ((y_dst == 0) && (x_dst == 0));
    const vx_bool eof = ((y_dst + VEC_NUM_OUT == IMG_ROWS_OUT) && (x_dst + VEC_NUM_OUT == IMG_COLS_OUT));
    GenerateDmaSignal<DataType, VEC_NUM_OUT>(sof, eof, output_vector);

    // Write Output
    output = output_vector;
}

/**********************************************************************************************************************/
template <typename DataType, typename BufferType, vx_uint16 BUFFER_NUM, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT,
          vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS_IN, vx_uint16 IMG_ROWS_IN,
          vx_uint16 IMG_COLS_OUT, vx_uint16 IMG_ROWS_OUT, vx_uint16 KERN_SIZE, vx_uint8 ACCURACY,
          vx_border_e BORDER_TYPE, vx_interpolation_type_e SCALE_TYPE>
void ScaleDownImageFunc(vx_image_data<DataType, VEC_NUM_IN> input[VEC_PIX_IN],
                        vx_image_data<DataType, VEC_NUM_OUT> output[VEC_PIX_OUT]) {
#pragma HLS INLINE

    // Constants
    const vx_bool EVEN_KERN_SIZE = (KERN_SIZE & 0x1) == 0;
    const vx_uint16 VEC_COLS = IMG_COLS_IN / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 WIN_COLS =
        (EVEN_KERN_SIZE) ? ((KERN_SIZE > VEC_NUM_IN) ? (KERN_SIZE) : (VEC_NUM_IN))
                         : ((KERN_RAD << 1) + VEC_NUM_IN + (VEC_NUM_IN - (KERN_RAD % VEC_NUM_IN)) % VEC_NUM_IN);
    const vx_uint16 OHD_COLS =
        (EVEN_KERN_SIZE) ? (IMG_COLS_IN % KERN_SIZE) / VEC_NUM_IN : (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;
    const vx_uint16 LINE_BUFFER_IMG_COLS = VEC_NUM_IN * (KERN_SIZE - 1);
    const vx_uint16 SCALE_VEC_ITER_OUT = (KERN_SIZE > VEC_NUM_IN) ? (KERN_SIZE / VEC_NUM_IN) : (1);

    // TODO new
    const vx_uint16 READ_VEC_PIXELS_X = IMG_COLS_OUT * KERN_SIZE / VEC_NUM_IN;
    const vx_uint16 READ_PIXELS_Y = IMG_ROWS_OUT * KERN_SIZE;

    // Linebuffer
    BufferType linebuffer[BUFFER_NUM][VEC_COLS];

    // Sliding window for complete vector
    DataType window[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable = window complete dim = 0

    vx_uint32 ptr_src = 0;
    vx_uint32 ptr_dst = 0;

    vx_bool y_dst_next = vx_false_e;

    // Compute the filter (pipelined)
    for (vx_uint16 y = 0, y_dst = 0, x_src = 0; y < IMG_ROWS_IN; ++y) {
        for (vx_uint16 x = 0, x_dst = 0, y_src = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            DataType input_buffer[VEC_NUM_IN];
#pragma HLS array_partition variable = input_buffer complete dim = 0

            DataType buffer[KERN_SIZE][VEC_NUM_IN];
#pragma HLS array_partition variable = buffer complete dim = 0

            // Input & Output
            vx_image_data<DataType, VEC_NUM_IN> input_data;
            vx_image_data<DataType, VEC_NUM_OUT> vec_output_data;

            // Read input data from global memory
            if (x < VEC_COLS) {
                input_data = input[ptr_src];
                ++ptr_src;
            }

            for (vx_uint16 v = 0; v < VEC_NUM_IN; v++) {
#pragma HLS unroll
                input_buffer[v] = input_data.pixel[v];
            }

            // Read data from line_buffer to buffer
            ReadFromLineBuffer<DataType, BufferType, BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(
                input_buffer, linebuffer, buffer, x);

            // Write data from buffer to line_buffer
            WriteToLineBuffer<DataType, BufferType, BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(buffer, linebuffer, x);

            // Move sliding window with replicated/constant border

            SlidingWindow<DataType, VEC_NUM_IN, IMG_ROWS_IN, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE>(
                buffer, window, x, y);

            ScaleDown<DataType, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS_IN, IMG_ROWS_IN, IMG_COLS_OUT, IMG_ROWS_OUT,
                      KERN_SIZE, WIN_COLS, ACCURACY, SCALE_TYPE>(window, vec_output_data, x_dst, y_dst, x_src, y_src);

            if (((x + 1) * VEC_NUM_IN - 1 == x_src) && (x < READ_VEC_PIXELS_X) && (y == y_src) && (y < READ_PIXELS_Y)) {

                output[ptr_dst] = vec_output_data;
                ++ptr_dst;
                x_dst += VEC_NUM_OUT;
                y_dst_next = vx_true_e;
            }

            if (y_dst_next && x_dst == IMG_COLS_OUT) {
                ++y_dst;
                y_dst_next = vx_false_e;
            }
        }
    }
}

/**********************************************************************************************************************/
template <typename DataType, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT, vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT,
          vx_uint16 IMG_COLS_IN, vx_uint16 IMG_ROWS_IN, vx_uint16 IMG_COLS_OUT, vx_uint16 IMG_ROWS_OUT,
          vx_uint16 KERN_SIZE, vx_uint8 ACCURACY, vx_border_e BORDER_TYPE, vx_interpolation_type_e SCALE_TYPE>
void ScaleDownImage(vx_image_data<DataType, VEC_NUM_IN> input[VEC_PIX_IN],
                    vx_image_data<DataType, VEC_NUM_OUT> output[VEC_PIX_OUT]) {
#pragma HLS INLINE

    const vx_bool allowed_vector_size_in =
        (VEC_NUM_IN == 1) || (VEC_NUM_IN == 2) || (VEC_NUM_IN == 4) || (VEC_NUM_IN == 8);
    const vx_bool allowed_vector_size_out =
        (VEC_NUM_OUT == 1) || (VEC_NUM_OUT == 2) || (VEC_NUM_OUT == 4) || (VEC_NUM_OUT == 8);
    const vx_bool vec_in_larger_equal_vec_out = (VEC_NUM_IN >= VEC_NUM_OUT);

    STATIC_ASSERT(IMG_COLS_IN >= IMG_COLS_OUT, only_down_scale_of_image_supported);
    STATIC_ASSERT(IMG_ROWS_IN >= IMG_ROWS_OUT, only_down_scale_of_image_supported);
    STATIC_ASSERT(SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR || SCALE_TYPE == VX_INTERPOLATION_BILINEAR ||
                      SCALE_TYPE == VX_INTERPOLATION_AREA_FAST,
                  scale_type_not_supported);
    STATIC_ASSERT(IMG_COLS_IN % VEC_NUM_IN == 0, image_colums_are_not_multiple_of_vector_size);

    STATIC_ASSERT(allowed_vector_size_in, input_vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(allowed_vector_size_out, output_vector_size_must_be_1_2_4_8);
    STATIC_ASSERT(vec_in_larger_equal_vec_out, input_vector_size_must_be_larger_or_equal_output_vector_size);

    // Constants
    const vx_uint16 DATA_IMG_COLS = sizeof(DataType) * VEC_NUM_IN * (KERN_SIZE - 1);
    const vx_uint16 BUFFER_UINT32 = DATA_IMG_COLS % 4;
    const vx_uint16 BUFFER_UINT16 = DATA_IMG_COLS % 2;
    const vx_uint16 BUFFER_NUM =
        (BUFFER_UINT32 == 0) ? (DATA_IMG_COLS >> 2) : ((BUFFER_UINT16 == 0) ? (DATA_IMG_COLS >> 1) : (DATA_IMG_COLS));
    const vx_uint16 BUFFER_IMG_COLS = (BUFFER_UINT32 == 0) ? (4) : ((BUFFER_UINT16 == 0) ? (2) : (1));

    // Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used

    ScaleDownImageFunc<DataType, typename select_buffer_data_type<BUFFER_IMG_COLS>::DataType, BUFFER_NUM, VEC_PIX_IN,
                       VEC_PIX_OUT, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS_IN, IMG_ROWS_IN, IMG_COLS_OUT, IMG_ROWS_OUT,
                       KERN_SIZE, ACCURACY, BORDER_TYPE, SCALE_TYPE>(input, output);
}

/**********************************************************************************************************************/
/**
 * @brief
 * @tparam DataType
 * @param input
 * @param output
 */
template <typename DataType, vx_uint32 VEC_PIX_IN, vx_uint32 VEC_PIX_OUT, vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 SCALE_FACTOR>
void HwScaleDownMean(vx_image_data<DataType, VEC_NUM_IN> input[VEC_PIX_IN],
                     vx_image_data<DataType, VEC_NUM_OUT> output[VEC_PIX_OUT]) {
#pragma HLS INLINE

    const vx_uint8 ACCURACY = 0;
    const vx_interpolation_type_e SCALE_TYPE = VX_INTERPOLATION_AREA_FAST;
    const vx_uint16 IMG_COLS_OUT = IMG_COLS / SCALE_FACTOR;
    const vx_uint16 IMG_ROWS_OUT = IMG_ROWS / SCALE_FACTOR;

    const vx_bool multiple_IMG_COLS = ((IMG_COLS % SCALE_FACTOR) == 0);
    const vx_bool multiple_height = ((IMG_ROWS % SCALE_FACTOR) == 0);
    const vx_bool supported_scale_factor = (SCALE_FACTOR == 2) || (SCALE_FACTOR == 4) || (SCALE_FACTOR == 8);

    // TODO STATIC_ASSERT(multiple_IMG_COLS, width_has_to_be_a_multiple_of_scale_factor);
    // TODO STATIC_ASSERT(multiple_height, height_has_to_be_a_multiple_of_scale_factor);
    STATIC_ASSERT(supported_scale_factor, supported_scale_factor_are_2_4_8);

    ScaleDownImage<DataType, VEC_PIX_IN, VEC_PIX_OUT, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS, IMG_ROWS, IMG_COLS_OUT,
                   IMG_ROWS_OUT, SCALE_FACTOR, ACCURACY, VX_BORDER_UNDEFINED, SCALE_TYPE>(input, output);
}

#endif /* __SRC_IMG_MT_CONVERSION_SCALE_H__ */
