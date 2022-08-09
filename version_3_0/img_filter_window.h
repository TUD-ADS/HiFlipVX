/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_filter_window.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are the sliding window and line buffer functions used for the hardware accelerated image processing functions
 */

#ifndef SRC_IMG_FILTER_WINDOW_H_
#define SRC_IMG_FILTER_WINDOW_H_

#include "img_filter_function.h"

/*********************************************************************************************************************/
/* Linebuffer Functions */
/*********************************************************************************************************************/

/** @brief Reads data from line buffers
@param VecType     1 vector is 1 linebuffer element
@param KERN_SIZE   The size of the filter kernel
@param VEC_COLS    The amount of columns in the vectorized image
@param input       New image data to be stored into the linebuffer
@param linebuffer  Linebuffers to store (KERN_SIZE-1) image rows
@param output      Data at x coordinates in the linebuffers
@param x           The x coordinate in the vectorized image
*/
template <typename InType, typename BufferType, const vx_uint16 BUFFER_NUM, const vx_uint8 VEC_NUM,
          const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void ReadFromLineBuffer(InType input[VEC_NUM], BufferType linebuffer[BUFFER_NUM][VEC_COLS],
                        InType output[KERN_SIZE][VEC_NUM], const vx_uint16 x) {
#pragma HLS INLINE

    // For data type conversion to decrease BRAM usage: FACTOR*BUFFER_NUM = (KERN_SIZE-1)*VEC_NUM
    const vx_uint16 FACTOR = (sizeof(BufferType) < sizeof(InType)) ? (1) : (sizeof(BufferType) / sizeof(InType));

    // Buffer for data type conversion
    BufferType buffer1[BUFFER_NUM];
#pragma HLS array_partition variable = buffer1 complete dim = 0
    InType buffer2[BUFFER_NUM * FACTOR];
#pragma HLS array_partition variable = buffer2 complete dim = 0

    // Check linebuffer border
    if (x < VEC_COLS) {

        // Read data from linebuffer
        for (vx_uint16 i = 0; i < BUFFER_NUM; i++) {
#pragma HLS unroll
            buffer1[i] = linebuffer[i][x];
        }

        // Unpack data
        for (vx_uint16 i = 0; i < BUFFER_NUM; i++) {
#pragma HLS unroll
            for (vx_uint16 j = 0; j < FACTOR; j++) {
#pragma HLS unroll
                vx_uint16 shift = j * static_cast<vx_uint16>(sizeof(InType)) * static_cast<vx_uint16>(8);
                buffer2[i * FACTOR + j] = static_cast<InType>(buffer1[i] >> shift);
            }
        }

        // Pack data for output
        for (vx_uint16 i = 0; i < KERN_SIZE - 1; i++) {
#pragma HLS unroll
            for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
                output[i][j] = buffer2[i * VEC_NUM + j];
            }
        }
        for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS UNROLL
            output[KERN_SIZE - 1][j] = input[j];
        }
    }
}
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void ReadFromLineBuffer_(InType input[VEC_NUM],                                                //
                         vx_data_pack<InType, (KERN_SIZE - 1) * VEC_NUM> linebuffer[VEC_COLS], //
                         InType output[KERN_SIZE][VEC_NUM],                                    //
                         const vx_uint16 x) {                                                  //
#pragma HLS INLINE

    // Buffer for data type conversion
    vx_data_pack<InType, (KERN_SIZE - 1) * VEC_NUM> buffer1;

    // Check linebuffer border
    if (x < VEC_COLS) {

        // Read data from linebuffer
        buffer1 = linebuffer[x];

        // Pack data for output
        for (vx_uint16 i = 0; i < KERN_SIZE - 1; i++) {
#pragma HLS unroll
            for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
                output[i][j] = buffer1.pixel[i * VEC_NUM + j];
            }
        }
        for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS UNROLL
            output[KERN_SIZE - 1][j] = input[j];
        }
    }
}

/** @brief Writes data to line buffers
@param VecType     1 vector is 1 linebuffer element
@param KERN_SIZE   The size of the filter kernel
@param VEC_COLS    The amount of columns in the vectorized image
@param input       Stores data at the x coordinates of the linebuffers
@param linebuffer  Linebuffers to store (KERN_SIZE-1) image rows
@param x           The x coordinate in the vectorized image
*/
template <typename InType, typename BufferType, const vx_uint16 BUFFER_NUM, const vx_uint8 VEC_NUM,
          const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void WriteToLineBuffer(InType input[KERN_SIZE][VEC_NUM], BufferType linebuffer[BUFFER_NUM][VEC_COLS],
                       const vx_uint16 x) {
#pragma HLS INLINE

    // For data type conversion to decrease BRAM usage: FACTOR*BUFFER_NUM = (KERN_SIZE-1)*VEC_NUM
    const vx_uint16 FACTOR = (sizeof(BufferType) < sizeof(InType)) ? (1) : (sizeof(BufferType) / sizeof(InType));

    // Buffer for data type conversion
    InType buffer1[(KERN_SIZE - 1) * VEC_NUM];
#pragma HLS array_partition variable = buffer1 complete dim = 0
    BufferType buffer2[BUFFER_NUM];
#pragma HLS array_partition variable = buffer2 complete dim = 0

    // Check linebuffer border
    if (x < VEC_COLS) {

        // Unpack data from input
        for (vx_uint16 i = 0; i < KERN_SIZE - 1; i++) {
#pragma HLS unroll
            for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
                buffer1[i * VEC_NUM + j] = input[i + 1][j];
            }
        }

        // Pack data for linebuffer
        for (vx_uint16 i = 0; i < BUFFER_NUM; i++) {
#pragma HLS unroll
            BufferType data = 0;
            for (vx_uint16 j = 0; j < FACTOR; j++) {
#pragma HLS unroll
                vx_uint16 shift = j * static_cast<vx_uint16>(sizeof(InType)) * static_cast<vx_uint16>(8);
                data |= (static_cast<BufferType>(buffer1[i * FACTOR + j])) << shift;
            }
            buffer2[i] = data;
        }

        // Write to linebuffer
        for (vx_uint16 i = 0; i < BUFFER_NUM; i++) {
#pragma HLS unroll
            linebuffer[i][x] = buffer2[i];
        }
    }
}

template <typename InType, const vx_uint8 VEC_NUM, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void WriteToLineBuffer_(InType input[KERN_SIZE][VEC_NUM],                                     //
                        vx_data_pack<InType, (KERN_SIZE - 1) * VEC_NUM> linebuffer[VEC_COLS], //
                        const vx_uint16 x) {                                                  //
#pragma HLS INLINE

    // Buffer for data type conversion
    vx_data_pack<InType, (KERN_SIZE - 1) * VEC_NUM> buffer1;

    // Check linebuffer border
    if (x < VEC_COLS) {

        // Unpack data from input
        for (vx_uint16 i = 0; i < (KERN_SIZE - 1); i++) {
#pragma HLS unroll
            for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
                buffer1.pixel[i * VEC_NUM + j] = input[i + 1][j];
            }
        }

        // Write to linebuffer
        linebuffer[x] = buffer1;
    }
}

/*********************************************************************************************************************/
/* Sliding Window Functions for Different Border Types */
/*********************************************************************************************************************/

/** @brief Replicates the y borders if needed for the sliding window
@param VecType     1 vector is 1 linebuffer element
@param IMG_ROWS    Amount of rows in the image
@param KERN_RAD    Radius of the filter kernel
@param KERN_SIZE   Total amount of rows in the sliding window
@param input       Output of the linebuffers
@param output      Input with replicated borders if needed
@param y           y coordinate of the image
*/
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD,
          const vx_uint16 KERN_SIZE>
void SlidingWindowReplicatedY(const InType input[KERN_SIZE][VEC_NUM], InType output[KERN_SIZE][VEC_NUM],
                              const vx_uint16 y) {
#pragma HLS INLINE

    // Get upper pixels and check y border
    if (KERN_RAD > 0) {
        for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
            output[KERN_RAD - 1][v] = (y > KERN_RAD) ? (input[KERN_RAD - 1][v]) : (input[KERN_RAD][v]);
        }
        for (vx_int32 i = KERN_RAD - 2; i >= 0; i--) {
#pragma HLS UNROLL
            for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
                output[i][v] = (y > static_cast<vx_uint16>(KERN_SIZE - 2 - i)) ? (input[i][v]) : (output[i + 1][v]);
            }
        }
    }

    // Pass through observed pixel in the image
    for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
        output[KERN_RAD][v] = input[KERN_RAD][v];
    }

    // Get lower pixels and check y border
    if (KERN_RAD > 0) {
        for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
            output[KERN_RAD + 1][v] = (y < IMG_ROWS + KERN_RAD - 1) ? (input[KERN_RAD + 1][v]) : (input[KERN_RAD][v]);
        }
        for (vx_uint16 i = KERN_RAD + 2; i < KERN_SIZE; i++) {
#pragma HLS UNROLL
            for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
                output[i][v] =
                    (y < static_cast<vx_uint16>(IMG_ROWS + KERN_SIZE - 1 - i)) ? (input[i][v]) : (output[i - 1][v]);
            }
        }
    }
}

/** @brief Moves sliding window and a replicated border in x direction
@param ScalarType  The data type of the image elements
@param KERN_SIZE   Total amount of rows in the sliding window
@param VEC_COLS    The amount of columns in the vectorized image
@param VEC_SIZE    The number of elements in a vector
@param WIN_BORD_A  Internal vertical border for the sliding window
@param WIN_BORD_B  Internal vertical border for the sliding window
@param WIN_COLS    The number of columns in a row
@param input       An array of input data for each row
@param window      The sliding window
@param x           x coordinate of the vectorized image
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE,
          const vx_uint16 WIN_BORD_A, const vx_uint16 WIN_BORD_B, const vx_uint16 WIN_COLS>
void SlidingWindowReplicatedX(ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][WIN_COLS],
                              const vx_uint16 x) {
#pragma HLS INLINE

    // Move sliding window and check x border
    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll

        // Move sliding window
        for (vx_uint16 j = 0; j < WIN_BORD_A; j++) {
#pragma HLS unroll
            window[i][j] = window[i][j + VEC_SIZE];
        }

        // Move pixel in sliding window and get/check left x border
        for (vx_uint16 j = WIN_BORD_A; j < WIN_BORD_B; j++) {
#pragma HLS unroll
            if (x == 0)
                window[i][j] = input[i][0];
            else
                window[i][j] = window[i][j + VEC_SIZE];
        }

        // Get new pixel array and check right x border
        for (vx_uint16 j = WIN_BORD_B; j < WIN_COLS; j++) {
#pragma HLS unroll
            if (x >= VEC_COLS) {
                window[i][j] = window[i][WIN_BORD_B - 1];
            } else {
                window[i][j] = input[i][j - WIN_BORD_B];
            }
        }
    }
}

/** @brief Sets a constant border of 0 for the sliding window
@param VecType     1 vector is 1 linebuffer element
@param IMG_ROWS    Amount of rows in the image
@param KERN_RAD    Radius of the filter kernel
@param KERN_SIZE   Total amount of rows in the sliding window
@param input       Output of the linebuffers
@param output      Input with replicated borders if needed
@param y           y coordinate of the image
*/
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD,
          const vx_uint16 KERN_SIZE>
void SlidingWindowConstantY(const InType input[KERN_SIZE][VEC_NUM], InType output[KERN_SIZE][VEC_NUM],
                            const vx_uint16 y) {
#pragma HLS INLINE

    // Get upper pixels and check y border
    for (vx_uint16 i = 0; i < KERN_RAD; i++) {
#pragma HLS unroll
        for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
            output[i][v] = (y > KERN_SIZE - 2 - i) ? (input[i][v]) : (0);
        }
    }

    // Pass through observed pixel in the image
    for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
        output[KERN_RAD][v] = input[KERN_RAD][v];
    }

    // Get lower pixels and check y border
    for (vx_uint16 i = KERN_RAD + 1; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
            output[i][v] = (y < IMG_ROWS + KERN_SIZE - 1 - i) ? (input[i][v]) : (0);
        }
    }
}

/** @brief Moves sliding window and a constant border in x direction
@param ScalarType  The data type of the image elements
@param KERN_SIZE   Total amount of rows in the sliding window
@param VEC_COLS    The amount of columns in the vectorized image
@param VEC_SIZE    The number of elements in a vector
@param WIN_BORD_A  Internal vertical border for the sliding window
@param WIN_BORD_B  Internal vertical border for the sliding window
@param WIN_COLS    The number of columns in a row
@param input       An array of input data for each row
@param window      The sliding window
@param x           x coordinate of the vectorized image
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE,
          const vx_uint16 WIN_BORD_A, const vx_uint16 WIN_BORD_B, const vx_uint16 WIN_COLS>
void SlidingWindowConstantX(ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][WIN_COLS],
                            const vx_uint16 x) {
#pragma HLS INLINE

    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll

        // Move sliding window
        for (vx_uint16 j = 0; j < WIN_BORD_A; j++) {
#pragma HLS unroll
            window[i][j] = window[i][j + VEC_SIZE];
        }

        // Move pixel in sliding window and get/check left x border
        for (vx_uint16 j = WIN_BORD_A; j < WIN_BORD_B; j++) {
#pragma HLS unroll
            if (x == 0)
                window[i][j] = 0;
            else
                window[i][j] = window[i][j + VEC_SIZE];
        }

        // Get new pixel vector and check right x border
        for (vx_uint16 j = WIN_BORD_B; j < WIN_COLS; j++) {
#pragma HLS unroll
            if (x >= VEC_COLS)
                window[i][j] = 0;
            else
                window[i][j] = input[i][j - WIN_BORD_B];
        }
    }
}

/** @brief Moves sliding window without considering borders
@param ScalarType  The data type of the image elements
@param KERN_SIZE   Total amount of rows in the sliding window
@param VEC_SIZE    The number of elements in a vector
@param WIN_BORD_B  Internal vertical border for the sliding window
@param WIN_COLS    The number of columns in a row
@param input       An array of input data for each row
@param window      The sliding window
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_SIZE, const vx_uint16 WIN_BORD_B,
          const vx_uint16 WIN_COLS>
void SlidingWindowUnchanged(const ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][WIN_COLS]) {
#pragma HLS INLINE

    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll

        // Move sliding window
        for (vx_uint16 j = 0; j < WIN_BORD_B; j++) {
#pragma HLS unroll
            window[i][j] = window[i][j + VEC_SIZE];
        }

        // Get new pixel vector
        for (vx_uint16 j = WIN_BORD_B; j < WIN_COLS; j++) {
#pragma HLS unroll
            window[i][j] = input[i][j - WIN_BORD_B];
        }
    }
}

/*********************************************************************************************************************/
/* Main sliding Window Functions */
/*********************************************************************************************************************/

/** @brief Sliding window horizontal main function / considers different border types
@param VecType     1 vector is processed in each clock cycle
@param ScalarType  1 vector contains N scalar elements
@param KERN_RAD    Radius of the filter kernel
@param VEC_COLS    The amount of columns in the vectorized image
@param VEC_SIZE    The number of elements in a vector
@param WIN_COLS    The number of columns in a row
@param BORDER_TYPE The border type that is considered in the sliding window
@param input       The input buffer
@param window      The output sliding window
@param x           The x current coordinate
*/
template <typename ScalarType, const vx_uint16 KERN_RAD, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE,
          const vx_uint16 WIN_COLS, const vx_border_e BORDER_TYPE>
void SlidingWindowHorizontal(const ScalarType input[VEC_SIZE], ScalarType window[1][WIN_COLS], const vx_uint16 x) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 WIN_BORD_A = WIN_COLS - VEC_SIZE - KERN_RAD;
    const vx_uint16 WIN_BORD_B = WIN_COLS - VEC_SIZE;

    // Input data rows in scalar representation (after considering y border)
    ScalarType input_vector[1][VEC_SIZE];
#pragma HLS array_partition variable = input_vector complete dim = 0

    for (vx_uint16 i = 0; i < VEC_SIZE; i++) {
#pragma HLS unroll
        input_vector[0][i] = input[i];
    }

    // REPLICATED: replicates the border values when exceeding borders
    if (BORDER_TYPE == VX_BORDER_REPLICATE) {

        // Sets sliding window and replicated x borders
        SlidingWindowReplicatedX<ScalarType, 1, VEC_COLS, VEC_SIZE, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(input_vector,
                                                                                                      window, x);

        // CONSTANT: creates a constant border of zeros around the image
    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {

        // Sets sliding window and constant x borders
        SlidingWindowConstantX<ScalarType, 1, VEC_COLS, VEC_SIZE, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(input_vector,
                                                                                                    window, x);

        // UNCHANGED: filters exceeding the borders are invalid
    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {

        // Sets sliding window and does not create borders
        SlidingWindowUnchanged<ScalarType, 1, VEC_SIZE, WIN_BORD_B, WIN_COLS>(input_vector, window);
    }
}

/** @brief Sliding window main function / considers different border types
@param VecType     1 vector is processed in each clock cycle
@param ScalarType  1 vector contains N scalar elements
@param IMG_ROWS    Amount of rows in the image
@param KERN_RAD    Radius of the filter kernel
@param VEC_SIZE    The number of elements in a vector
@param KERN_SIZE   Total amount of rows in the sliding window
@param BORDER_TYPE The border type that is considered in the sliding window
@param input       The input buffer
@param window      The output sliding window
@param y           The y current coordinate
*/
template <typename ScalarType, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD, const vx_uint16 VEC_SIZE,
          const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SlidingWindowVertical(const ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][VEC_SIZE],
                           const vx_uint16 y) {
#pragma HLS INLINE

    // Input data rows in vector representation (after considering y border)
    ScalarType buffer_vector[KERN_SIZE][VEC_SIZE];
#pragma HLS array_partition variable = buffer_vector complete dim = 0

    // REPLICATED: replicates the border values when exceeding borders
    if (BORDER_TYPE == VX_BORDER_REPLICATE) {

        // Replicated y borders
        SlidingWindowReplicatedY<ScalarType, VEC_SIZE, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);

        // CONSTANT: creates a constant border of zeros around the image
    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {

        // Constant y borders
        SlidingWindowConstantY<ScalarType, VEC_SIZE, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);
    }

    // Convert from vector to scalar type
    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS UNROLL
        for (vx_uint16 j = 0; j < VEC_SIZE; j++) {
#pragma HLS UNROLL
            if (BORDER_TYPE == VX_BORDER_REPLICATE || BORDER_TYPE == VX_BORDER_CONSTANT)
                window[i][j] = buffer_vector[i][j];
            else if (BORDER_TYPE == VX_BORDER_UNDEFINED)
                window[i][j] = input[i][j];
        }
    }
}

/** @brief Sliding window main function / considers different border types
@param VecType     1 vector is processed in each clock cycle
@param ScalarType  1 vector contains N scalar elements
@param IMG_ROWS    Amount of rows in the image
@param KERN_RAD    Radius of the filter kernel
@param VEC_COLS    The amount of columns in the vectorized image
@param VEC_SIZE    The number of elements in a vector
@param WIN_COLS    The number of columns in a row
@param KERN_SIZE   Total amount of rows in the sliding window
@param BORDER_TYPE The border type that is considered in the sliding window
@param input       The input buffer
@param window      The output sliding window
@param x           The x current coordinate
@param y           The y current coordinate
*/
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD,
          const vx_uint16 VEC_COLS, const vx_uint16 WIN_COLS, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SlidingWindow(const InType input[KERN_SIZE][VEC_NUM], InType window[KERN_SIZE][WIN_COLS], const vx_uint16 x,
                   const vx_uint16 y) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 WIN_BORD_A = WIN_COLS - VEC_NUM - KERN_RAD;
    const vx_uint16 A = KERN_RAD + (KERN_RAD % VEC_NUM);
    const vx_uint16 WIN_BORD_B = WIN_COLS - VEC_NUM;
    const vx_uint16 B = 2 * KERN_RAD + (KERN_RAD % VEC_NUM);

    // Input data rows in vector representation (after considering y border)
    InType buffer_vector[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable = buffer_vector complete dim = 0

    // REPLICATED: replicates the border values when exceeding borders
    if (BORDER_TYPE == VX_BORDER_REPLICATE) {

        // Replicated y borders
        SlidingWindowReplicatedY<InType, VEC_NUM, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);

        // Sets sliding window and replicated x borders
        SlidingWindowReplicatedX<InType, KERN_SIZE, VEC_COLS, VEC_NUM, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(buffer_vector,
                                                                                                         window, x);

        // CONSTANT: creates a constant border of zeros around the image
    } else if (BORDER_TYPE == VX_BORDER_CONSTANT) {

        // Constant y borders
        SlidingWindowConstantY<InType, VEC_NUM, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);

        // Sets sliding window and constant x borders
        SlidingWindowConstantX<InType, KERN_SIZE, VEC_COLS, VEC_NUM, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(buffer_vector,
                                                                                                       window, x);

        // UNCHANGED: filters exceeding the borders are invalid
    } else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {

        // Sets sliding window and does not create borders
        SlidingWindowUnchanged<InType, KERN_SIZE, VEC_NUM, WIN_BORD_B, WIN_COLS>(input, window);
    }
}

#endif /* SRC_IMG_FILTER_WINDOW_H_ */
