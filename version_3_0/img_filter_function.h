/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_filter_function.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>, Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are the functions that compute on an input window and output a single value.
 */

#ifndef SRC_IMG_FILTER_FUNCTION_H_
#define SRC_IMG_FILTER_FUNCTION_H_

#include "img_mt_helper.h"

/*********************************************************************************************************************/
/* Filter Sub-Functions */
/*********************************************************************************************************************/

/** @brief Compares and swaps 2 elements
@param InType  Data type of the image pixels
@param A       1. element to be compared
@param B       2. element to be compared
@param H       smaller element
@param L       bigger element
*/
template <typename InType>                                      //
void CompareAndSwap(InType A, InType B, InType &L, InType &H) { //
#pragma HLS INLINE

    // Compare and Swap
    if (A > B) {
        L = B;
        H = A;
    } else {
        L = A;
        H = B;
    }
}

/*********************************************************************************************************************/
/* Filter Functions */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/** @brief Computes a 1d box filter
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the box filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE>                                                          //
ScalarType ComputeBox1d(ScalarType window[KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) { //
#pragma HLS INLINE

    // Variable
    vx_uint64 result = 0;

    // Sum all input data
    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        result += static_cast<vx_uint64>(window[i]);
    }

    // Normalize
    const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<ScalarType>(norm);
}

/*********************************************************************************************************************/
/** @brief Computes a 2d box filter
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the box filter
*/
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE> //
OutType ComputeBox2d(InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult,
                     const vx_uint32 kernel_shift) { //
#pragma HLS INLINE

    // Variable
    vx_uint64 result = 0;

    // Sum all input data
    for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
        for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
            result += static_cast<vx_uint64>(window[y][x]);
        }
    }

    // Normalize
    const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/** @brief Computes a 2d convolution filter
@param InType        The input scalar data type
@param CompType      Data type for internal computation (uint64 or int64)
@param OutType       The output scalar data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel
@param inKernel      The costum kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the convolution filter
*/
template <typename InType, typename CompType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE> //
OutType ComputeConvolve2d(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],         //
                          const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {                        //
#pragma HLS INLINE

    // Variables
    CompType result = 0;

    // Compute the costum filter
    for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            CompType kernelData = static_cast<CompType>(kernel[i][j]);
            CompType windowData = static_cast<CompType>(window[i][j]);
            result += (kernelData * windowData);
        }
    }

    // Normalize
    const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/** @brief Computes a 1d dilate filter
@param ScalarType    The Scalar Data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the dilate filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE>  //
ScalarType ComputeDilate1d(ScalarType window[KERN_SIZE]) { //
#pragma HLS INLINE

    // Compute dilate
    ScalarType result = window[0];
    for (vx_uint16 i = 1; i < KERN_SIZE; i++) {
#pragma HLS unroll
        result = MAX(result, window[i]);
    }

    // Return result
    return result;
}

/*********************************************************************************************************************/
/** @brief Computes a 1d erode filter
@param ScalarType    The Scalar Data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the erode filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE> //
ScalarType ComputeErode1d(ScalarType window[KERN_SIZE]) { //
#pragma HLS INLINE

    // Compute erode
    ScalarType result = window[0];
    for (vx_uint16 i = 1; i < KERN_SIZE; i++) {
#pragma HLS unroll
        result = MIN(result, window[i]);
    }

    // Return result
    return result;
}

/*********************************************************************************************************************/
/** @brief Computes a gaussian 1d filter (optimized to the kernel symmetry)
@param ScalarType    The input/output data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel
@param kernel        The gaussian kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the gaussian convolution
*/
template <typename ScalarType, typename KernelType, const vx_uint16 KERN_SIZE>            //
ScalarType ComputeGaussian1d(KernelType kernel[KERN_SIZE], ScalarType window[KERN_SIZE],  //
                             const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) { //
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;

    // Get middle pixel result
    vx_uint64 result = static_cast<vx_uint64>(kernel[KERN_RAD]) * static_cast<vx_uint64>(window[KERN_RAD]);

    // Add all other pixel to result
    for (vx_uint16 i = 0; i < KERN_RAD; i++) {
#pragma HLS unroll
        vx_uint64 A = static_cast<vx_uint64>(kernel[i]);
        vx_uint64 B0 = static_cast<vx_uint64>(window[i]);
        vx_uint64 B1 = static_cast<vx_uint64>(window[KERN_SIZE - i - 1]);
        result += A * (B0 + B1);
    }

    // Normalize
    const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<ScalarType>(norm);
}

/*********************************************************************************************************************/
/** @brief Computes a gaussian 2d filter (optimized to the kernel symmetry)
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel
@param kernel        The gaussian kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the gaussian convolution
*/
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>            //
OutType ComputeGaussian2d(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE], //
                          const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {                //
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE / 2;
    const vx_uint16 KERN_RNG = KERN_SIZE - 1;

    // Store result of the gaussian filter (scalar)
    vx_uint64 result = 0;

    // if: (y == KERN_RAD) && (x == KERN_RAD)
    {
        vx_uint64 sum = static_cast<vx_uint64>(window[KERN_RAD][KERN_RAD]);
        result = (static_cast<vx_uint64>(kernel[KERN_RAD][KERN_RAD]) * sum);
    }

    // if: (y == KERN_RAD) && (x < KERN_RAD)
    for (vx_uint16 x = 0; x < KERN_RAD; x++) {
#pragma HLS unroll
        const vx_uint16 RADIUS = KERN_RAD - x;
        vx_uint64 sum = 0;
        sum += static_cast<vx_uint64>(window[KERN_RAD][x]);
        sum += static_cast<vx_uint64>(window[KERN_RAD][KERN_RNG - x]);
        sum += static_cast<vx_uint64>(window[KERN_RAD + RADIUS][x + RADIUS]);
        sum += static_cast<vx_uint64>(window[KERN_RAD - RADIUS][x + RADIUS]);
        result += (static_cast<vx_uint64>(kernel[KERN_RAD][x]) * sum);
    }

    // if: (y == x) && (x < KERN_RAD)
    for (vx_uint16 y = 0; y < KERN_RAD; y++) {
#pragma HLS unroll
        vx_uint64 sum = 0;
        sum += static_cast<vx_uint64>(window[y][y]);
        sum += static_cast<vx_uint64>(window[y][KERN_RNG - y]);
        sum += static_cast<vx_uint64>(window[KERN_RNG - y][y]);
        sum += static_cast<vx_uint64>(window[KERN_RNG - y][KERN_RNG - y]);
        result += (static_cast<vx_uint64>(kernel[y][y]) * sum);
    }

    // if: (y > 0) && (y < KERN_RAD) && (y > x)
    for (vx_uint16 x = 0; x <= KERN_RAD; x++) {
#pragma HLS unroll
        for (vx_uint16 y = x + 1; y < KERN_RAD; y++) {
#pragma HLS unroll
            vx_int64 sum = 0;
            sum += static_cast<vx_uint64>(window[y][x]);
            sum += static_cast<vx_uint64>(window[x][y]);
            sum += static_cast<vx_uint64>(window[KERN_RNG - y][x]);
            sum += static_cast<vx_uint64>(window[x][KERN_RNG - y]);
            sum += static_cast<vx_uint64>(window[KERN_RNG - x][y]);
            sum += static_cast<vx_uint64>(window[y][KERN_RNG - x]);
            sum += static_cast<vx_uint64>(window[KERN_RNG - x][KERN_RNG - y]);
            sum += static_cast<vx_uint64>(window[KERN_RNG - y][KERN_RNG - x]);
            result += (static_cast<vx_uint64>(kernel[y][x]) * sum);
        }
    }

    // Normalize
    const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/** @brief Computes a median filter
@details Algorithm taken from the the following paper:
A fault-tolerant implementation of the median filter
from L. A. Aranda and P. Reviriego and J. A. Maestro
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the median filter
*/
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE> //
OutType ComputeMedian3x3(InType window[KERN_SIZE][KERN_SIZE]) {         //
#pragma HLS INLINE

    // Variables
    InType B0, B1, B3, B4, B6, B7;
    InType C1, C2, C4, C5, C7, C8;
    InType D0, D1, D3, D4, D6, D7;
    InType E0, E1, E3, E4, E7, E8;
    InType F1, F2, F4, F5, F6, F7;
    InType G3, G4, H3, H4, I4, I5, J3, J4;

    // Input pixel
    InType A0 = window[0][0], A1 = window[0][1], A2 = window[0][2];
    InType A3 = window[1][0], A4 = window[1][1], A5 = window[1][2];
    InType A6 = window[2][0], A7 = window[2][1], A8 = window[2][2];

    // Get median with 19 compare and swap units
    CompareAndSwap(A0, A1, B0, B1);
    CompareAndSwap(A3, A4, B3, B4);
    CompareAndSwap(A6, A7, B6, B7);
    CompareAndSwap(B1, A2, C1, C2);
    CompareAndSwap(B4, A5, C4, C5);
    CompareAndSwap(B7, A8, C7, C8);
    CompareAndSwap(B0, C1, D0, D1);
    CompareAndSwap(B3, C4, D3, D4);
    CompareAndSwap(B6, C7, D6, D7);
    CompareAndSwap(D0, D3, E0, E1);
    CompareAndSwap(D1, D4, E3, E4);
    CompareAndSwap(C5, C8, E7, E8);
    CompareAndSwap(E1, D6, F1, F2);
    CompareAndSwap(E4, D7, F4, F5);
    CompareAndSwap(C2, E7, F6, F7);
    CompareAndSwap(E3, F4, G3, G4);
    CompareAndSwap(F2, G4, H3, H4);
    CompareAndSwap(H4, F6, I4, I5);
    CompareAndSwap(H3, I4, J3, J4);

    // Take median value
    return static_cast<OutType>(J4);
}

/*********************************************************************************************************************/
/** @brief Computes a median filter
@details Algorithm taken from the the following book:
Knuth in The Art of Computer Programming, vol 3 (algorithm 5.2.2M)
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the median filter
*/
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE> //
OutType ComputeMedian(InType window[KERN_SIZE][KERN_SIZE]) {            //
#pragma HLS INLINE

    // Constants and variables
    const vx_uint16 BUFFER_SIZE = KERN_SIZE * KERN_SIZE;
    const vx_uint16 BUFFER_RAD = BUFFER_SIZE >> 1;
    vx_uint16 START = 1;
    vx_uint16 row_ptr = 0;
    vx_uint16 SCALE = BUFFER_SIZE - 1;
    vx_uint16 MSB = 0;

    // Register stages for pipelining of the sorting
    InType buffer[BUFFER_SIZE + 1][BUFFER_SIZE];
#pragma HLS array_partition variable = buffer complete dim = 0

    // Pre-computation
    for (vx_uint16 i = 0; i < sizeof(vx_uint16) * 8; i++) {
#pragma HLS unroll
        if ((SCALE & static_cast<vx_uint16>(1 << i)) != 0)
            MSB = i;
    }
    for (vx_uint16 i = 0; i < MSB; i++) {
#pragma HLS unroll
        START *= 2;
    }

    // 2d to 1d input window conversion
    for (vx_uint16 i = 0, ptr = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            buffer[0][ptr] = window[i][j];
            ptr++;
        }
    }

    // Sort array using odd-even mergesort
    for (vx_uint16 p = START; p > 0; p >>= 1) {
#pragma HLS unroll
        for (vx_uint16 q = START, r = 0, d = p; d > 0; d = q - p, q >>= 1, r = p) {
#pragma HLS unroll
            for (vx_uint16 i = 0; i < BUFFER_SIZE; i++) {
#pragma HLS unroll
                buffer[row_ptr + 1][i] = buffer[row_ptr][i];
            }
            for (vx_uint16 i = 0; i < BUFFER_SIZE - d; i++) {
#pragma HLS unroll
                if ((i & p) == r)
                    CompareAndSwap<InType>(buffer[row_ptr][i], buffer[row_ptr][i + d], buffer[row_ptr + 1][i],
                                           buffer[row_ptr + 1][i + d]);
            }
            row_ptr++;
        }
    }

    // Take median value
    return static_cast<OutType>(buffer[row_ptr][BUFFER_RAD]);
}

/*********************************************************************************************************************/
/** @brief Computes the y derivative (optimized for kernel symmetry)
@param InType        The input data type
@param OutType       The output data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel/window
@param kernel        The suppression mask  (if mask pixel is 0, check suppression)
@param window        The sliding window of input pixels
@return              The result of the Non-Maxima Suppression
*/
template <typename InType, typename OutType, typename KernType, vx_uint16 KERN_SIZE>
OutType NonMaxSuppression(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    // Constants
    const vx_type_e data_type = GET_TYPE(InType);
    const InType min_value = ComputeMin<InType>();
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;

    // Is true, if input is maximum
    bool maximum = true;

    // Input pixel
    InType input = window[KERN_RAD][KERN_RAD];

    // Check if pixel is maxima in mask area
    for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
        for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll

            // Pixel to compare
            InType pixel = window[y][x];

            // If mask is zero, check suppresion
            if (kernel[y][x] == 0) {

                // Check top left
                if ((y < KERN_RAD) || ((y == KERN_RAD) && (x < KERN_RAD))) {
                    if (input < pixel)
                        maximum = false;

                    // Check bottom right
                } else if ((y > KERN_RAD) || ((y == KERN_RAD) && (x > KERN_RAD))) {
                    if (input <= pixel)
                        maximum = false;
                }
            }
        }
    }

    // Suppress input to minimum possible value, if it is not a maxima
    InType output = (maximum == false) ? (min_value) : (input);
    return static_cast<OutType>(output);
}

/*********************************************************************************************************************/
template <typename InType, typename OutType, vx_uint16 KERN_SIZE>  //
OutType SegmentTestDetector(InType window[KERN_SIZE][KERN_SIZE]) { //
#pragma HLS INLINE

    // Variables
    InType circle[16] = {0};
#pragma HLS array_partition variable = circle complete dim = 0
    InType difference[16];
#pragma HLS array_partition variable = difference complete dim = 0
    vx_uint8 direction[16];
#pragma HLS array_partition variable = direction complete dim = 0

    // Input and output
    InType input = 0;
    InType response = 0;

    // Buffer Bresenham circle
    if (KERN_SIZE == 7) {
        input = window[3][3];
        circle[0] = window[0][3];
        circle[1] = window[0][4];
        circle[2] = window[1][5];
        circle[3] = window[2][6];
        circle[4] = window[3][6];
        circle[5] = window[4][6];
        circle[6] = window[5][5];
        circle[7] = window[6][4];
        circle[8] = window[6][3];
        circle[9] = window[6][2];
        circle[10] = window[5][1];
        circle[11] = window[4][0];
        circle[12] = window[3][0];
        circle[13] = window[2][0];
        circle[14] = window[1][1];
        circle[15] = window[0][2];
    }

    // Compute diffs and compare if pixel is dark or bright
    for (vx_uint8 i = 0; i < 16; ++i) {
#pragma HLS unroll
        if (circle[i] > input) {
            difference[i] = circle[i] - input;
            direction[i] = 1;
        } else {
            difference[i] = input - circle[i];
            direction[i] = 2;
        }
    }

    // Compute response of all possible corners
    for (vx_uint8 i = 0; i < 16; ++i) {
#pragma HLS unroll
        InType minimum = std::numeric_limits<InType>::max();
        vx_uint8 c1 = 3;

        // Compute response of 1 possible corner
        // Response is ">0" if all other pixels are small or bigger
        for (vx_uint8 j = i; j < (i + 9); ++j) {
#pragma HLS unroll
            vx_uint8 ptr = j % 16;
            if (minimum > difference[ptr])
                minimum = difference[ptr];
            c1 = c1 & direction[ptr];
        }
        InType val = (c1 == 0) ? (0) : (minimum);

        // Get highest response
        if (response < val)
            response = val;
    }

    // Output final response
    return response;
}

/*********************************************************************************************************************/
template <typename InType, typename OutType, vx_uint16 KERN_SIZE, InType THRESH_LOWER, InType THRESH_UPPER> //
OutType Hysteresis(InType window[KERN_SIZE][KERN_SIZE]) {                                                   //
#pragma HLS INLINE

    // Constants
    vx_uint32 KERN_RAD = KERN_SIZE / 2;
    OutType MAX_VAL = std::numeric_limits<OutType>::max();

    // Input variable
    InType input = window[KERN_RAD][KERN_RAD];

    // Check the strength of the input
    bool strong = (input >= THRESH_UPPER);
    bool weak = (input >= THRESH_LOWER);

    // If variable weak and neighbour strong, set to strong
    for (vx_uint32 y = 0; y < KERN_SIZE; ++y) {
#pragma HLS unroll
        for (vx_uint32 x = 0; x < KERN_SIZE; ++x) {
#pragma HLS unroll
            //
            if (!(y == KERN_RAD && x == KERN_RAD)) {
                InType neighbour = window[y][x];
                if ((neighbour >= THRESH_UPPER) && (weak == true)) {
                    strong = true;
                }
            }
        }
    }
    OutType result = (strong == true) ? (MAX_VAL) : ((OutType)42);

    // Return result
    return result;
}

/*********************************************************************************************************************/
template <typename InType, typename OutType, vx_uint32 KERN_SIZE, vx_uint32 QUANTIZATION> //
OutType ComputeOrientedNonMaxSuppression(InType window1[KERN_SIZE][KERN_SIZE],
                                         InType window2[KERN_SIZE][KERN_SIZE]) { //
#pragma HLS INLINE

    // Constants
    const InType max_val = std::numeric_limits<InType>::max();
    const InType min_val = std::numeric_limits<InType>::min();
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;

    // Input pixel
    InType input = window1[KERN_RAD][KERN_RAD];
    InType orientation = window2[KERN_RAD][KERN_RAD];

    // Downscale quantization
    if (QUANTIZATION > 3)
        orientation = orientation >> (QUANTIZATION - 3);

    // Get the pixels in perpendicular lines
    InType a = max_val, b = max_val;
    if (orientation == 0 || orientation == 4) {
        a = window1[0][1]; // north
        b = window1[2][1]; // south
    } else if (orientation == 1 || orientation == 5) {
        a = window1[0][0]; // north/west
        b = window1[2][2]; // south/east
    } else if (orientation == 2 || orientation == 6) {
        a = window1[1][0]; // west
        b = window1[1][2]; // east
    } else if (orientation == 3 || orientation == 7) {
        a = window1[2][0]; // south/west
        b = window1[0][2]; // north/east
    }

    // Suppress input to minimum possible value, if it is not a maxima
    InType output = ((input >= a) || (input >= b)) ? (input) : (min_val);
    return static_cast<OutType>(output);
}

/*********************************************************************************************************************/
/**
 * @brief Optimized function which computes the second order derivatives in respect to x and y by exploding the
 * symmetric properties of them (e.g. the convolution of a 3x3 Scharr filter kernel with respect to x with a 3x3 Scharr
 * filter kernel in respect to y).
 * @tparam InType Input data type (either 8 or 16 bit image).
 * @tparam OutType Output data type (either 8 or 16 bit image).
 * @tparam KernType Data type of the kernel.
 * @tparam KERN_SIZE Size a the quadratic kernel in one dimension.
 * @param kernel Kernel which is used to compute the second order derivatives in respect to x and y.
 * @param window Sliding window applied on the image to convolve it with the kernel.
 * @param kernel_mult Multiplier for normalization.
 * @param kernel_shift Shifter for normalization.
 * @return The result of the x and y derivatives computation.
 */
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeXY(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],
                            const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 KERN_RANGE = KERN_SIZE - 1;

    // Compute the filter result
    vx_int64 result = 0;

    for (vx_uint16 y = 0; y < KERN_RAD; y++) {
#pragma HLS unroll
        for (vx_uint16 x = 0; x < KERN_RAD; x++) {
#pragma HLS unroll

            // Constants
            const vx_uint16 yn = y + KERN_RANGE - y * 2;
            const vx_uint16 xn = x + KERN_RANGE - x * 2;

            // Compute mul
            vx_int64 mul = ABS(static_cast<vx_int64>(kernel[y][x]));

            // compute sum
            vx_int64 sum = static_cast<vx_int64>(window[y][xn]) + static_cast<vx_int64>(window[yn][x]) -
                           static_cast<vx_int64>(window[y][x]) - static_cast<vx_int64>(window[yn][xn]);

            // Add to result
            result += sum * mul;
        }
    }

    // Normalize
    const vx_uint64 rounding = (kernel_shift == 0) ? (0) : static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/**
 * @brief Optimized function which computes the second order derivatives in respect to x by exploding the symmetric
 * properties of them (e.g. the convolution of a 3x3 Scharr filter kernel with respect to x with itself).
 * @tparam InType Input data type (either 8 or 16 bit image).
 * @tparam OutType Output data type (either 8 or 16 bit image).
 * @tparam KernType Data type of the kernel.
 * @tparam KERN_SIZE Size a the quadratic kernel in one dimension.
 * @param kernel Kernel which is used to compute the second order derivatives in respect to x.
 * @param window Sliding window applied on the image to convolve it with the kernel.
 * @param kernel_mult Multiplier for normalization.
 * @param kernel_shift Shifter for normalization.
 * @return The result of the second order derivatives computation in respect to x.
 */
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeXX(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],
                            const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 KERN_RANGE = KERN_SIZE - 1;

    // Compute the filter result
    vx_int64 result = 0;

    for (vx_uint16 y = 0; y < KERN_RAD + 1; y++) {
#pragma HLS unroll
        for (vx_uint16 x = 0; x < KERN_RAD; x++) {
#pragma HLS unroll

            // Constants
            const vx_uint16 yn = y + KERN_RANGE - y * 2;
            const vx_uint16 xn = x + KERN_RANGE - x * 2;

            // Compute mul
            vx_int64 mul = ABS(static_cast<vx_int64>(kernel[y][x]));

            // compute sum
            vx_int64 sum;
            if (y < KERN_RAD) {
                sum =
                    ((static_cast<vx_int64>(window[y][KERN_RAD]) + static_cast<vx_int64>(window[yn][KERN_RAD])) << 1) -
                    static_cast<vx_int64>(window[y][xn]) - static_cast<vx_int64>(window[yn][xn]) -
                    static_cast<vx_int64>(window[y][x]) - static_cast<vx_int64>(window[yn][x]);
            } else {
                sum = (static_cast<vx_int64>(window[y][KERN_RAD]) << 1) - static_cast<vx_int64>(window[y][xn]) -
                      static_cast<vx_int64>(window[y][x]);
            }

            // Add to result
            result += sum * mul;
        }
    }

    // Normalize
    const vx_uint64 rounding = (kernel_shift == 0) ? (0) : static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/**
 * @brief Optimized function which computes the second order derivatives in respect to y by exploding the symmetric
 * properties of them (e.g. the convolution of a 3x3 Scharr filter kernel with respect to y with itself).
 * @tparam InType Input data type (either 8 or 16 bit image).
 * @tparam OutType Output data type (either 8 or 16 bit image).
 * @tparam KernType Data type of the kernel.
 * @tparam KERN_SIZE Size a the quadratic kernel in one dimension.
 * @param kernel Kernel which is used to compute the second order derivatives in respect to y.
 * @param window Sliding window applied on the image to convolve it with the kernel.
 * @param kernel_mult Multiplier for normalization.
 * @param kernel_shift Shifter for normalization.
 * @return The result of the second order derivatives computation in respect to y.
 */
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeYY(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],
                            const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 KERN_RANGE = KERN_SIZE - 1;

    // Compute the filter result
    vx_int64 result = 0;

    for (vx_uint16 x = 0; x < KERN_RAD + 1; x++) {
#pragma HLS unroll
        for (vx_uint16 y = 0; y < KERN_RAD; y++) {
#pragma HLS unroll

            // Constants
            const vx_uint16 yn = y + KERN_RANGE - y * 2;
            const vx_uint16 xn = x + KERN_RANGE - x * 2;

            // Compute mul
            vx_int64 mul = ABS(static_cast<vx_int64>(kernel[y][x]));

            // compute sum
            vx_int64 sum;
            if (x < KERN_RAD) {
                sum =
                    ((static_cast<vx_int64>(window[KERN_RAD][x]) + static_cast<vx_int64>(window[KERN_RAD][xn])) << 1) -
                    static_cast<vx_int64>(window[yn][x]) - static_cast<vx_int64>(window[yn][xn]) -
                    static_cast<vx_int64>(window[y][x]) - static_cast<vx_int64>(window[y][xn]);
            } else {
                sum = (static_cast<vx_int64>(window[KERN_RAD][x]) << 1) - static_cast<vx_int64>(window[yn][x]) -
                      static_cast<vx_int64>(window[y][x]);
            }

            // Add to result
            result += sum * mul;
        }
    }

    // Normalize
    const vx_uint64 rounding = (kernel_shift == 0) ? (0) : static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/*
 * copied from img_filter_core
 * added check for kernel_shift = 0 in for rounding
 * add _ to the end of the name to distinguish between modified and original version
 */
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeX(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],
                           const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 KERN_RANGE = KERN_SIZE - 1;

    // Compute the filter result
    vx_int64 result = 0;

    for (vx_uint16 y = 0; y < KERN_RAD + 1; y++) {
#pragma HLS unroll
        for (vx_uint16 x = 0; x < KERN_RAD; x++) {
#pragma HLS unroll

            // Constants
            const vx_uint16 yn = y + KERN_RANGE - y * 2;
            const vx_uint16 xn = x + KERN_RANGE - x * 2;

            // Compute mul
            vx_int64 mul = ABS(static_cast<vx_int64>(kernel[y][x]));

            // compute sum
            vx_int64 sum;
            if (y < KERN_RAD) {
                sum = static_cast<vx_int64>(window[y][xn]) + static_cast<vx_int64>(window[yn][xn]) -
                      static_cast<vx_int64>(window[y][x]) - static_cast<vx_int64>(window[yn][x]);
            } else {
                sum = static_cast<vx_int64>(window[y][xn]) - static_cast<vx_int64>(window[y][x]);
            }

            // Add to result
            result += sum * mul;
        }
    }

    // Normalize
    const vx_uint64 rounding = (kernel_shift == 0) ? (0) : static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeY(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE],
                           const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 KERN_RANGE = KERN_SIZE - 1;

    // Compute the filter result
    vx_int64 result = 0;

    for (vx_uint16 x = 0; x < KERN_RAD + 1; x++) {
#pragma HLS unroll
        for (vx_uint16 y = 0; y < KERN_RAD; y++) {
#pragma HLS unroll

            // Constants
            const vx_uint16 yn = y + KERN_RANGE - y * 2;
            const vx_uint16 xn = x + KERN_RANGE - x * 2;

            // Compute mul
            vx_int64 mul = ABS(static_cast<vx_int64>(kernel[y][x]));

            // compute sum
            vx_int64 sum;
            if (x < KERN_RAD) {
                sum = static_cast<vx_int64>(window[yn][x]) + static_cast<vx_int64>(window[yn][xn]) -
                      static_cast<vx_int64>(window[y][x]) - static_cast<vx_int64>(window[y][xn]);
            } else {
                sum = static_cast<vx_int64>(window[yn][x]) - static_cast<vx_int64>(window[y][x]);
            }

            // Add to result
            result += sum * mul;
        }
    }

    // Normalize
    const vx_uint64 rounding = (kernel_shift == 0) ? (0) : static_cast<vx_uint64>(1) << (kernel_shift - 1);
    const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

    // Return result
    return static_cast<OutType>(norm);
}

/*********************************************************************************************************************/
/**
 * @brief Computes a conduction coefficient using the Perona Malik diffusion for wide regions
 * @tparam DataType Data type of the input / output image(unsigned)(8 - bit, 16 - bit)
 * @tparam KernType Data type of the kernel
 * @tparam KERN_NUM Number of input kernels
 * @tparam KERN_SIZE Kernel window size
 * @tparam PRECISION Level of precision to compute the conduction coefficents
 * @param window Window of pixel to be convolute with kernel
 * @param kernel Kernels used for convolution
 * @param kernel_mult   Multiplier for normalization
 * @param kernel_shift  Shifter For normalization
 * @param csquare Square of the contrast factor
 * @return Conduction coefficient
 */
template <typename DataType, typename KernType, vx_uint8 KERN_NUM, vx_uint8 KERN_SIZE, vx_hint_e PRECISION> //
DataType ComputePeronaMalikWideRegion(                                                                      //
    DataType window[KERN_SIZE][KERN_SIZE], KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE],                 //
    const vx_uint8 kernel_mult, const vx_uint8 kernel_shift, const contrastSquareT csquare) {               //
#pragma HLS INLINE

    // with normalization before computing the square (low)
    typedef typename to_signed<DataType>::DataType derivNormSignedT; // 7 or 15 bits (+1 sign bit)

    typedef typename next_larger_unsigned<DataType>::DataType derivNormSquareUnsignedT; // 16 or 32 bits
    typedef typename next_larger_signed<DataType>::DataType derivNormSquareSignedT;     // 15 or 31 bits (+1 sign bit)

    typedef derivNormSquareUnsignedT denT; // for low
    typedef vx_uint32 denMedT;             // 18 bits for medium
    typedef vx_uint32 denHighT;            // 24 bit for high

    typedef vx_uint32 numT;     // 32 bits for low and medium
    typedef vx_uint64 numHighT; // 48 bits for high

    typedef vx_uint16 resMedT; // 14 bits for medium

    const vx_uint8 KERNEL_SHIFT = 5;
    const vx_uint8 KERNEL_SHIFT_SQUARE = KERNEL_SHIFT << 1;

    // common constants for all precisions
    const vx_uint8 BITS_INPUT = BYTES_IN_BITS(sizeof(DataType)); // 8 or 16 bits
    const vx_uint8 BITS_OUTPUT = BITS_INPUT;                     // 8 or 16 bits
    const vx_uint8 BITS_LX_LY = BITS_INPUT + 4;                  // 12 or 20 bits

    // bias of 1 for the denominator to avoid division by zero
    const vx_uint8 EPSILON = 1;

    /*
     *  + 1 because rounding can cause to reach value 0.5
     *  e.g.	sum						4|8 = 	   1111 1111 0000
     *  		rounding: 1 << (5-1)		 	   0000 0001 0000
     *  		intermediate result		5|8 = 0001 0000 0000 0000
     *  		normalized: >> 5		0|8	=           1000 0000
     */
    const vx_uint8 BITS_LX_LY_NORM = BITS_LX_LY - KERNEL_SHIFT + 1;
    const vx_uint8 BITS_LX_LY_SQUARE = (BITS_LX_LY_NORM << 1) - 1;

    // Further research showed that k² is at most 356/1024 = 0,34765625. Meaning the highest bit of constrastSquareT is
    // never set, therefore -1.
    const vx_uint8 BITS_CSQUARE = BYTES_IN_BITS(sizeof(contrastSquareT)) - 1; // 31 bits
    const vx_uint8 BITS_FLOAT = 24;

    /*
     * Syntax: Fraction bits | Highest fraction bit set
     * contrastSquare is assumed of type vx_uint32
     * variable		16 bit		8 bit		range
     * --------------------------------------------------------------------
     * lx & ly		16|16		8|8			[-0.5, 0.5]
     * lxu & lyu	16|16		8|8			[0, 0.5]
     * lxs & lys	32|31		16|15		[0, 0.25]
     */

    // had lowest resource requirements
    const vx_ap_uint<BITS_CSQUARE> prec_csquare = csquare;

    const vx_ap_int<BITS_LX_LY_NORM + 1> lx = ComputeDerivativeX<DataType, derivNormSignedT, KernType, KERN_SIZE>(
        kernel[SCHARR_X], window, kernel_mult, kernel_shift);
    const vx_ap_int<BITS_LX_LY_NORM + 1> ly = ComputeDerivativeY<DataType, derivNormSignedT, KernType, KERN_SIZE>(
        kernel[SCHARR_Y], window, kernel_mult, kernel_shift);

    const vx_ap_uint<BITS_LX_LY_SQUARE> lxs_tmp = lx * lx;
    const vx_ap_uint<BITS_LX_LY_SQUARE> lys_tmp = ly * ly;

    const derivNormSquareUnsignedT lxs = lxs_tmp.value;
    const derivNormSquareUnsignedT lys = lys_tmp.value;

    DataType output;

    if (PRECISION == VX_HINT_PERFORMANCE_LOW_POWER) {

        /*
         * Syntax: Fraction bits | Highest fraction bit set
         * contrastSquare is assumed of type vx_uint32
         * variable				16 bit		8 bit		range
         * --------------------------------------------------------------------
         * num_csquare			32|31		20|19	(0, 0.34765625]
         * den_csquare			17|16		12|11	(0, 0.34765625]
         * den_lxs_lys			17|16		12|11	[0, 0.34765625]
         * den					17|17		12|12	(0, 0.6953125]
         * result				15|15		8|8		(0, 1)
         *
         * Even if for 16 bit the last two bits are always zero when they are shift to 16 bits for output,
         * test showed that the deviation was lower than in case of dividing the 32 bit numerator with a
         * 16 bit denominator. The bit width of the denominator of 17 was chose experimentally. With a
         * denominator greater and smaller than 17 the maximum deviation of the test results got worse.
         */

        /*
         * - 1 because square can at most 0.25
         * e.g.		norm_sum	0|8		1000 0000
         * 			sum_square	0|16	0100 0000 0000 0000
         */
        const vx_uint8 BITS_NUM_CSQUARE = (BITS_INPUT >= 16) ? (BITS_CSQUARE) : (19);
        const vx_uint8 BITS_DEN = (BITS_INPUT >= 16) ? (17) : (BITS_INPUT + 4);
        const vx_uint8 BITS_DEN_CSQUARE = BITS_DEN - 1;
        const vx_uint8 BITS_DEN_lXS_LYS = BITS_DEN - 1;
        const vx_uint8 BITS_RESULT = (BITS_INPUT >= 16) ? (BITS_INPUT - 1) : (BITS_INPUT);

        const vx_ap_uint<BITS_NUM_CSQUARE> num_csquare = prec_csquare.template shift_to<BITS_NUM_CSQUARE>();

        const vx_ap_uint<BITS_DEN_CSQUARE> den_csquare = prec_csquare.template shift_to<BITS_DEN_CSQUARE>();

        const vx_ap_uint<BITS_DEN_lXS_LYS> den_lxs = lxs_tmp.template shift_to_right_only<BITS_DEN_lXS_LYS>();
        const vx_ap_uint<BITS_DEN_lXS_LYS> den_lys = lys_tmp.template shift_to_right_only<BITS_DEN_lXS_LYS>();

        const vx_ap_uint<BITS_RESULT> result = num_csquare / (EPSILON + den_csquare + den_lxs + den_lys);
        // shift result to output data type
        output = static_cast<DataType>((result.template shift_to<BITS_OUTPUT>()).value);

    } else if (PRECISION == VX_HINT_PERFORMANCE_DEFAULT) {

        /*
         * Syntax: Fraction bits | Highest fraction bit set
         * csquare is assumed of type vx_uint32
         * variable				16 bit		8 bit		range
         * -----------------------------------------------------------------------------------
         * lxs & lys			32|31		16|15		[0, 0.25]
         * den_csquare			18|17		18|17		(0, 0.34765625]
         * den_lxs & den_lys	18|17		18|17		[0, 0.25)
         * den_lxs_lys			18|17		18|17		[0, 0.34765625)
         * den					18|18		18|18		(0, 0.6953125]
         * result				14|14		14|14		(0, 1)
         *
         * The bit width of the denominator of 18 was chose experimentally.
         * Even if for 16 bit the last two bits are always zero when they are shift to 16 bits for output,
         * test showed that the deviation was lower than in case of dividing the 32 bit numerator with a
         * 16 bit denominator. With a denominator greater and smaller than 18 the maximum deviation of the test results
         * got worse.
         */

        const vx_uint8 BITS_DEN = 18;
        const vx_uint8 BITS_DEN_CSQUARE = BITS_DEN - 1;
        const vx_uint8 BITS_DEN_lXS_LYS = BITS_DEN - 1;
        const vx_uint8 BITS_RESULT = 14;

        const vx_ap_uint<BITS_DEN_CSQUARE> den_csquare = prec_csquare.template shift_to<BITS_DEN_CSQUARE>();
        const vx_ap_uint<BITS_DEN_lXS_LYS> den_lxs = lxs_tmp.template shift_to<BITS_DEN_lXS_LYS>();
        const vx_ap_uint<BITS_DEN_lXS_LYS> den_lys = lys_tmp.template shift_to<BITS_DEN_lXS_LYS>();

        const vx_ap_uint<BITS_RESULT> result = prec_csquare / (EPSILON + den_csquare + den_lxs + den_lys);

        // shift result to output data type
        output = static_cast<DataType>((result.template shift_to<BITS_OUTPUT>()).value);

    } else {

        /*
         * Syntax: Fraction bits | Highest fraction bit set
         * csquare is assumed of type vx_uint32
         * variable / bits		16			8			range
         * -----------------------------------------------------------------------------------
         * prec_square			32|31		32|31		(0, 0.34765625)
         * lxs & lys			32|31		16|15		[0, 0.25]
         * den_lxs & den_lys	24|23		16|15		[0, 0.25]
         * den_lxs_lys			24|23		16|15		[0, 0.34765625)
         * den_csquare			24|23		16|15		(0, 0.34765625)
         * den					24|24		16|16		(0, 0.6953125)
         * num_csquare			48|47		40|39		(0, 0.34765625)
         * result				24|24		24|24		(0, 1)
         */

#if false // TODO try to improve

		const vx_uint8 BITS_NUM_CSQUARE = (BITS_LX_LY_SQUARE + BITS_FLOAT > 2 * BITS_FLOAT) ? (2 * BITS_FLOAT - 1) : (BITS_LX_LY_SQUARE + BITS_FLOAT);
		const vx_uint8 BITS_DEN = (BITS_LX_LY_SQUARE > BITS_FLOAT - 1) ? (BITS_FLOAT - 1) : (BITS_LX_LY_SQUARE);
		const vx_uint8 BITS_DEN_SUM = BITS_DEN + 1;
		const vx_uint8 BITS_RESULT = BITS_FLOAT;

		const vx_ap_uint<BITS_DEN> den_lxs = lxs_tmp.template shift_to<BITS_DEN>();
		const vx_ap_uint<BITS_DEN> den_lys = lys_tmp.template shift_to<BITS_DEN>();
		const vx_ap_uint<BITS_DEN> den_csquare = prec_csquare.template shift_to<BITS_DEN>();
		const vx_ap_uint<BITS_NUM_CSQUARE> num_csquare = prec_csquare.template shift_to_left_only<BITS_NUM_CSQUARE>();

		const vx_ap_uint<BITS_RESULT> result = num_csquare / (EPSILON + den_csquare + den_lxs + den_lys);

		//shift result to output data type
		output = static_cast<DataType>((result.template shift_to<BITS_OUTPUT>()).value);
#else

        const vx_uint8 BITS_DEN = (BITS_INPUT >= 16) ? (BITS_FLOAT - 1) : (15);
        const vx_uint8 BITS_NUM = BITS_DEN + BITS_FLOAT;
        const vx_uint8 BITS_RESULT = BITS_FLOAT;

        const vx_uint8 BIT_SHIFT_NUM = BITS_NUM - BITS_CSQUARE;
        const vx_uint8 BIT_SHIFT_LXS_LYS = BITS_LX_LY_SQUARE - BITS_DEN;
        const vx_uint8 BIT_SHIFT_DEN_CSQUARE = BITS_CSQUARE - BITS_DEN;
        const vx_uint8 BIT_SHIFT_OUTPUT = BITS_RESULT - BITS_OUTPUT;

        const vx_uint64 num_csquare = static_cast<vx_uint64>(csquare) << BIT_SHIFT_NUM;
        const vx_uint64 den_csquare = csquare >> BIT_SHIFT_DEN_CSQUARE;
        const vx_uint64 den_lxs = lxs >> BIT_SHIFT_LXS_LYS;
        const vx_uint64 den_lys = lys >> BIT_SHIFT_LXS_LYS;

#ifdef __XILINX__
        const ap_uint<BITS_NUM> ap_num = num_csquare;
        const ap_uint<BITS_NUM> ap_den = EPSILON + den_csquare + den_lxs + den_lys;
        const vx_uint64 result = static_cast<vx_uint64>(hls::divide(ap_num, ap_den).to_uint());
#else
        const vx_uint64 result = num_csquare / (EPSILON + den_csquare + den_lxs + den_lys);
#endif

        output = static_cast<DataType>(result >> BIT_SHIFT_OUTPUT);

#endif
    }

    return output;
}

/*********************************************************************************************************************/
/**
 * @brief Computes Computes a FED step for given step size.
 * @tparam DataType Data type of the input / output image(unsigned)(8 - bit, 16 - bit)
 * @tparam KERN_SIZE Kernel window size
 * @param window_input_coeff Matrix of conduction coefficents
 * @param window_input_lt Input image
 * @return Diffused pixel
 */
template <typename DataType, vx_uint8 KERN_SIZE, vx_uint32 STEP_SIZE>
DataType ComputeFED(DataType window_input_lt[KERN_SIZE][KERN_SIZE], DataType window_input_coeff[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    const vx_uint8 FRACTION_BITS = 16;

    const DataType ONE = std::numeric_limits<DataType>::max();

    const vx_uint8 BIT_SHIFT_DIFF =
        BYTES_IN_BITS(2 * sizeof(DataType)) - FRACTION_BITS; // shift back to 16 bit fraction if necessary
    const vx_uint8 BIT_SHIFT_DIFF_OVER_TIME =
        2 * FRACTION_BITS - BYTES_IN_BITS(sizeof(DataType)); // shift back to input data type level if necessary

    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint8 HWI = KERN_SIZE - 1;
    const vx_uint8 LWI = 0;

    const vx_uint64 CoeffCenter = static_cast<vx_uint64>(window_input_coeff[KERN_RAD][KERN_RAD]);
    const vx_int64 LtCenter = static_cast<vx_int64>(window_input_lt[KERN_RAD][KERN_RAD]);

    /*
     * Bits: DataType + 1
     * Integer|Fraction: 1|8 or 1|16
     */
    const vx_uint64 xpos_1 = window_input_coeff[KERN_RAD][HWI] + CoeffCenter;
    const vx_uint64 xneg_1 = CoeffCenter + window_input_coeff[KERN_RAD][LWI];
    const vx_uint64 ypos_1 = window_input_coeff[HWI][KERN_RAD] + CoeffCenter;
    const vx_uint64 yneg_1 = CoeffCenter + window_input_coeff[LWI][KERN_RAD];

    /*
     * Bits: DataType
     * Integer|Fraction: 0|8 or 0|16
     */
    const vx_int64 xpos_2 = window_input_lt[KERN_RAD][HWI] - LtCenter;
    const vx_int64 xneg_2 = LtCenter - window_input_lt[KERN_RAD][LWI];
    const vx_int64 ypos_2 = window_input_lt[HWI][KERN_RAD] - LtCenter;
    const vx_int64 yneg_2 = LtCenter - window_input_lt[LWI][KERN_RAD];

    /*
     * Bits: 2 * DataType + 1
     * Integer|Fraction: 1|16 or 1|32
     */
    const vx_int64 xpos = static_cast<vx_int64>(xpos_1) * static_cast<vx_int64>(xpos_2);
    const vx_int64 xneg = static_cast<vx_int64>(xneg_1) * static_cast<vx_int64>(xneg_2);
    const vx_int64 ypos = static_cast<vx_int64>(ypos_1) * static_cast<vx_int64>(ypos_2);
    const vx_int64 yneg = static_cast<vx_int64>(yneg_1) * static_cast<vx_int64>(yneg_2);

    /*
     * Bits 2 * DataType + 4
     * Integer|Fraction: 4|16 or 4|32
     * Shift back to 16 fraction part if necessary
     */
    const vx_int64 diff = xpos - xneg + ypos - yneg;
    // TODO
    // const vx_int64 rounding_diff = (BIT_SHIFT_DIFF == 0) ? (0) : (static_cast<vx_uint64>(1) << (BIT_SHIFT_DIFF - 1));
    // const vx_int64 diff_shift = signedRightShift<vx_int64>(diff + rounding_diff, BIT_SHIFT_DIFF);
    const vx_int64 diff_shift = signedRightShift<vx_int64>(diff, BIT_SHIFT_DIFF);

    /*
     * Fraction (2 * 16) + Integer (4 + STEP_SIZE integer bits)
     * For step size the last 16 bits are fraction and the first 16 bits are reserved for integer
     */
    const vx_int64 diff_over_time = (STEP_SIZE >> 1) * diff_shift;
    // TODO
    // const vx_int64 rounding_diff_over_time = (BIT_SHIFT_DIFF_OVER_TIME == 0) ? (0) : (static_cast<vx_int64>(1) <<
    // (BIT_SHIFT_DIFF_OVER_TIME - 1)); const vx_int64 diff_over_time_shift = signedRightShift<vx_int64>(diff_over_time
    // + rounding_diff_over_time, BIT_SHIFT_DIFF_OVER_TIME);
    const vx_int64 diff_over_time_shift = signedRightShift<vx_int64>(diff_over_time, BIT_SHIFT_DIFF_OVER_TIME);

    const vx_int64 result = LtCenter + diff_over_time_shift;

    /*
     * Test in software showed that saturated values did not changed the overall results.
     */
    const DataType output = (result < 0) ? (0) : ((result > ONE) ? (ONE) : (result));

    return output;
}

/*********************************************************************************************************************/
/**
 * @brief Computes the determinant of the Hessian matrix
 * @tparam InType Data type of the input image(signed)(8 - bit, 16 - bit)
 * @tparam OutType Data type of the output image(signed)(8 - bit, 16 - bit)
 * @tparam KernType Data type of the kernel
 * @tparam KERN_NUM Number of input kernels
 * @tparam KERN_SIZE Kernel window size
 * @param input_lx First order derivatives in respect to x
 * @param input_ly First order derivatives in respect to y
 * @param kernel_vector Kernels used for convolution
 * @param kernel_mult Multiplier for normalization
 * @param kernel_shift Shifter For normalization
 * @return Determinant of the Hessian
 */

template <typename InType, typename OutType, typename KernType, vx_uint8 KERN_NUM, vx_uint8 KERN_SIZE>
OutType ComputeDoH(InType input_lx[KERN_SIZE][KERN_SIZE], InType input_ly[KERN_SIZE][KERN_SIZE],
                   KernType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE], const vx_uint8 kernel_mult,
                   const vx_uint8 kernel_shift) {
#pragma HLS INLINE

    typedef typename next_larger_signed<InType>::DataType hesT;
    typedef typename next_larger_signed<hesT>::DataType resT;
    const vx_uint8 KERNEL_SHIFT =
        (KERN_NUM == 3) ? (10)
                        : (5); // if three kernels are given as input it is assumed that these are the 5x5 convolved
                               // scharr kernels, else it is assumed that both 3x3 scharr kernels are used
    const vx_uint8 post_kernel_shift = (KERNEL_SHIFT - kernel_shift) << 1;

    const vx_uint8 BIT_SHIFT_OUT_RIGHT =
        (sizeof(OutType) <= sizeof(hesT)) ? (BYTES_IN_BITS(sizeof(hesT) - sizeof(OutType)) + post_kernel_shift) : (0);
    const vx_uint8 BIT_SHIFT_OUT_LEFT =
        (sizeof(OutType) <= sizeof(hesT)) ? (0) : (BYTES_IN_BITS(sizeof(OutType) - sizeof(hesT)) - post_kernel_shift);

    /*
     * Syntax: With signed bit|Fraction bits|Highest fraction bit set
     * Input bit width - 1 for fraction bits because input is signed
     * No kernel shift when applying the Scharr filter to keep precision.
     * Post kernel shift by 10 after computing the result. But not in case
     * of 8 bit input and 32 bit output to keep maximum precision of 26 bit in this case.
     * Meaning the highest fraction bit in this case is 25th bit.
     * variable 		input	16 bit		8 bit		8 bit		range
     * 					output	16/32 bit	16 bit		32 bit
     * ------------------------------------------------------------------------------
     * lx & ly					16|15|15	8|7|7		8|7|7		[-0.5, 0.5]
     * lxx & lyy & lxy			21|20|19	13|12|11	13|12|11	[-0.25, 0.25]
     * lxx*lyy 					42|41|39	26|25|23	26|25|23	[-0.125, 0.125]
     * lxy*lxy					42|41|39	26|25|23	26|25|23	[0, 0.125]
     * result					42|41|40	26|25|24	26|25|24	[-0.25, 0.125]
     * out_result				32|31|30	16|15|14	32|31|30	[-0.25, 0.125]
     */

    hesT lxx, lxy, lyy;

    if (KERN_NUM == 3) {
        lxx = ComputeDerivativeXX<InType, hesT, KernType, KERN_SIZE>(kernel_vector[DOH_XX], input_lx, kernel_mult,
                                                                     kernel_shift);
        lxy = ComputeDerivativeXY<InType, hesT, KernType, KERN_SIZE>(kernel_vector[DOH_XY], input_lx, kernel_mult,
                                                                     kernel_shift);
        lyy = ComputeDerivativeYY<InType, hesT, KernType, KERN_SIZE>(kernel_vector[DOH_YY], input_lx, kernel_mult,
                                                                     kernel_shift);
    } else {
        lxx = ComputeDerivativeX<InType, hesT, KernType, KERN_SIZE>(kernel_vector[SCHARR_X], input_lx, kernel_mult,
                                                                     kernel_shift);
        lxy = ComputeDerivativeY<InType, hesT, KernType, KERN_SIZE>(kernel_vector[SCHARR_Y], input_lx, kernel_mult,
                                                                     kernel_shift);
        lyy = ComputeDerivativeY<InType, hesT, KernType, KERN_SIZE>(kernel_vector[SCHARR_Y], input_ly, kernel_mult,
                                                                     kernel_shift);
    }

    const resT result =
        static_cast<resT>(lxx) * static_cast<resT>(lyy) - static_cast<resT>(lxy) * static_cast<resT>(lxy);
    const OutType out_result = signedRightShift<resT>(result, BIT_SHIFT_OUT_RIGHT) << BIT_SHIFT_OUT_LEFT;
    return out_result;
}

/*********************************************************************************************************************/
/**
 * @brief Simple forwading function which is used togheter with another kernel function to synchronously fordward the
 * image data.
 * @tparam DataType Data type of the input
 * @tparam KERN_SIZE Kernel window size in one dimenesion.
 * @param kernel_window Kernel window which shall be forworded.
 * @return Forwared pixel (center element of the kernel window).
 */
template <typename DataType, vx_uint8 KERN_SIZE>
DataType ForwardImageData(DataType kernel_window[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE
    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;
    return kernel_window[KERN_RAD][KERN_RAD];
}

#endif /* SRC_IMG_FILTER_FUNCTION_H_ */
