/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_filter_core.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These are the core functions used for the hardware accelerated image processing functions (Do not call functions from here)
*/

#ifndef SRC_IMG_FILTER_CORE_H_
#define SRC_IMG_FILTER_CORE_H_

#include "vx_helper.h"
#include <iostream>
using namespace std;

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
template <typename InType, typename BufferType, const vx_uint16 BUFFER_NUM, const vx_uint8 VEC_NUM, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void ReadFromLineBuffer(InType input[VEC_NUM], BufferType linebuffer[BUFFER_NUM][VEC_COLS], InType output[KERN_SIZE][VEC_NUM], const vx_uint16 x) {
#pragma HLS INLINE

	// For data type conversion to decrease BRAM usage: FACTOR*BUFFER_NUM = (KERN_SIZE-1)*VEC_NUM
	const vx_uint16 FACTOR = sizeof(BufferType) / sizeof(InType);

	// Buffer for data type conversion
	BufferType buffer1[BUFFER_NUM];
#pragma HLS array_partition variable=buffer1 complete dim=0
	InType buffer2[BUFFER_NUM*FACTOR];
#pragma HLS array_partition variable=buffer2 complete dim=0

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
				buffer2[i*FACTOR + j] = static_cast<InType>(buffer1[i] >> shift);
			}
		}

		// Pack data for output
		for (vx_uint16 i = 0; i < KERN_SIZE-1; i++) {
#pragma HLS unroll
			for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
				output[i][j] = buffer2[i*VEC_NUM + j];
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
template <typename InType, typename BufferType, const vx_uint16 BUFFER_NUM, const vx_uint8 VEC_NUM, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS>
void WriteToLineBuffer(InType input[KERN_SIZE][VEC_NUM], BufferType linebuffer[BUFFER_NUM][VEC_COLS], const vx_uint16 x) {
#pragma HLS INLINE

	// For data type conversion to decrease BRAM usage: FACTOR*BUFFER_NUM = (KERN_SIZE-1)*VEC_NUM
	const vx_uint16 FACTOR = sizeof(BufferType) / sizeof(InType);

	// Buffer for data type conversion
	InType buffer1[(KERN_SIZE - 1)*VEC_NUM];
#pragma HLS array_partition variable=buffer1 complete dim=0
	BufferType buffer2[BUFFER_NUM];
#pragma HLS array_partition variable=buffer2 complete dim=0

	// Check linebuffer border
	if (x < VEC_COLS) {

		// Unpack data from input
		for (vx_uint16 i = 0; i < KERN_SIZE - 1; i++) {
#pragma HLS unroll
			for (vx_uint16 j = 0; j < VEC_NUM; j++) {
#pragma HLS unroll
				buffer1[i*VEC_NUM + j] = input[i + 1][j];
			}
		}

		// Pack data for linebuffer
		for (vx_uint16 i = 0; i < BUFFER_NUM; i++) {
#pragma HLS unroll
			BufferType data = 0;
			for (vx_uint16 j = 0; j < FACTOR; j++) {
#pragma HLS unroll
				vx_uint16 shift = j * static_cast<vx_uint16>(sizeof(InType)) * static_cast<vx_uint16>(8);
				data |= (static_cast<BufferType>(buffer1[i*FACTOR + j])) << shift;
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
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD, const vx_uint16 KERN_SIZE>
void SlidingWindowReplicatedY(const InType input[KERN_SIZE][VEC_NUM], InType output[KERN_SIZE][VEC_NUM], const vx_uint16 y) {
#pragma HLS INLINE

	// Get upper pixels and check y border
	if (KERN_RAD > 0) {
		for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS UNROLL
			output[KERN_RAD - 1][v] = (y > KERN_RAD) ? (input[KERN_RAD - 1][v]) : (input[KERN_RAD][v]);
		}
		//output[KERN_RAD - 1] = (y > KERN_RAD) ? (input[KERN_RAD - 1]) : (input[KERN_RAD]);
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
				output[i][v] = (y < static_cast<vx_uint16>(IMG_ROWS + KERN_SIZE - 1 - i)) ? (input[i][v]) : (output[i - 1][v]);
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
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE, const vx_uint16 WIN_BORD_A, const vx_uint16 WIN_BORD_B, const vx_uint16 WIN_COLS>
void SlidingWindowReplicatedX(ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][WIN_COLS], const vx_uint16 x) {
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
				window[i][j] = window[i][WIN_BORD_B-1];
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
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD, const vx_uint16 KERN_SIZE>
void SlidingWindowConstantY(const InType input[KERN_SIZE][VEC_NUM], InType output[KERN_SIZE][VEC_NUM], const vx_uint16 y) {
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
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE, const vx_uint16 WIN_BORD_A, const vx_uint16 WIN_BORD_B, const vx_uint16 WIN_COLS>
void SlidingWindowConstantX(ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][WIN_COLS], const vx_uint16 x) {
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
template <typename ScalarType, const vx_uint16 KERN_SIZE, const vx_uint16 VEC_SIZE, const vx_uint16 WIN_BORD_B, const vx_uint16 WIN_COLS>
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
template <typename ScalarType, const vx_uint16 KERN_RAD, const vx_uint16 VEC_COLS, const vx_uint16 VEC_SIZE, const vx_uint16 WIN_COLS, const vx_border_e BORDER_TYPE>
void SlidingWindowHorizontal(const ScalarType input[VEC_SIZE], ScalarType window[1][WIN_COLS], const vx_uint16 x) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 WIN_BORD_A = WIN_COLS - VEC_SIZE - KERN_RAD;
	const vx_uint16 WIN_BORD_B = WIN_COLS - VEC_SIZE;

	// Input data rows in scalar representation (after considering y border)
	ScalarType input_vector[1][VEC_SIZE];
#pragma HLS array_partition variable=input_vector complete  dim=0

	for (vx_uint16 i = 0; i < VEC_SIZE; i++) {
#pragma HLS unroll
		input_vector[0][i] = input[i];
	}

	// REPLICATED: replicates the border values when exceeding borders
	if (BORDER_TYPE == VX_BORDER_REPLICATE) {

		// Sets sliding window and replicated x borders
		SlidingWindowReplicatedX<ScalarType, 1, VEC_COLS, VEC_SIZE, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(input_vector, window, x);

	// CONSTANT: creates a constant border of zeros around the image
	} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {

		// Sets sliding window and constant x borders
		SlidingWindowConstantX<ScalarType, 1, VEC_COLS, VEC_SIZE, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(input_vector, window, x);

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
template <typename ScalarType, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD, const vx_uint16 VEC_SIZE, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SlidingWindowVertical(const ScalarType input[KERN_SIZE][VEC_SIZE], ScalarType window[KERN_SIZE][VEC_SIZE], const vx_uint16 y) {
#pragma HLS INLINE

	// Input data rows in vector representation (after considering y border)
	ScalarType buffer_vector[KERN_SIZE][VEC_SIZE];
#pragma HLS array_partition variable=buffer_vector complete dim=0

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
			if(BORDER_TYPE == VX_BORDER_REPLICATE || BORDER_TYPE == VX_BORDER_CONSTANT)
				window[i][j] = buffer_vector[i][j];
			else if(BORDER_TYPE == VX_BORDER_UNDEFINED)
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
template <typename InType, const vx_uint8 VEC_NUM, const vx_uint32 IMG_ROWS, const vx_uint16 KERN_RAD, const vx_uint16 VEC_COLS, const vx_uint16 WIN_COLS, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SlidingWindow(const InType input[KERN_SIZE][VEC_NUM], InType window[KERN_SIZE][WIN_COLS], const vx_uint16 x, const vx_uint16 y) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 WIN_BORD_A = WIN_COLS - VEC_NUM - KERN_RAD;
	const vx_uint16 WIN_BORD_B = WIN_COLS - VEC_NUM;

	// Input data rows in vector representation (after considering y border)
	InType buffer_vector[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=buffer_vector complete dim=0

	// REPLICATED: replicates the border values when exceeding borders
	if (BORDER_TYPE == VX_BORDER_REPLICATE) {

		// Replicated y borders
		SlidingWindowReplicatedY<InType, VEC_NUM, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);

		// Sets sliding window and replicated x borders
		SlidingWindowReplicatedX<InType, KERN_SIZE, VEC_COLS, VEC_NUM, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(buffer_vector, window, x);

	// CONSTANT: creates a constant border of zeros around the image
	} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {

		// Constant y borders
		SlidingWindowConstantY<InType, VEC_NUM, IMG_ROWS, KERN_RAD, KERN_SIZE>(input, buffer_vector, y);

		// Sets sliding window and constant x borders
		SlidingWindowConstantX<InType, KERN_SIZE, VEC_COLS, VEC_NUM, WIN_BORD_A, WIN_BORD_B, WIN_COLS>(buffer_vector, window, x);

	// UNCHANGED: filters exceeding the borders are invalid
	} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {

		// Sets sliding window and does not create borders
		SlidingWindowUnchanged<InType, KERN_SIZE, VEC_NUM, WIN_BORD_B, WIN_COLS>(input, window);
	}
}

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
template <typename InType>
void CompareAndSwap(InType A, InType B, InType &L, InType &H) {
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

/** @brief Computes a 1d box filter
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the box filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE>
ScalarType ComputeBox1d(ScalarType window[KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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

/** @brief Computes a 2d box filter
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the box filter
*/
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE>
OutType ComputeBox2d(InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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
template <typename InType, typename CompType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeConvolve2d(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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

/** @brief Computes a 1d dilate filter
@param ScalarType    The Scalar Data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the dilate filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE>
ScalarType ComputeDilate1d(ScalarType window[KERN_SIZE]) {
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

/** @brief Computes a 1d erode filter
@param ScalarType    The Scalar Data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the erode filter
*/
template <typename ScalarType, const vx_uint16 KERN_SIZE>
ScalarType ComputeErode1d(ScalarType window[KERN_SIZE]) {
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
template <typename ScalarType, typename KernelType, const vx_uint16 KERN_SIZE>
ScalarType ComputeGaussian1d(KernelType kernel[KERN_SIZE], ScalarType window[KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeGaussian2d(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE>
OutType ComputeMedian3x3(InType window[KERN_SIZE][KERN_SIZE]) {
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

/** @brief Computes a median filter
@details Algorithm taken from the the following book:
Knuth in The Art of Computer Programming, vol 3 (algorithm 5.2.2M)
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KERN_SIZE     The size of the kernel
@param window        The sliding window of this scalar computation
@return              The result of the median filter
*/
template <typename InType, typename OutType, const vx_uint16 KERN_SIZE>
OutType ComputeMedian(InType window[KERN_SIZE][KERN_SIZE]) {
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
#pragma HLS array_partition variable=buffer complete dim=0

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
					CompareAndSwap<InType>(buffer[row_ptr][i], buffer[row_ptr][i + d], buffer[row_ptr + 1][i], buffer[row_ptr + 1][i + d]);
			}
			row_ptr++;
		}
	}

	// Take median value
	return static_cast<OutType>(buffer[row_ptr][BUFFER_RAD]);
}

/** @brief Computes the x derivative (optimized for kernel symmetry)
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel
@param kernel        The sobel/scharr kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the x derivative computation
*/
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeX(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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
				sum = static_cast<vx_int64>(window[y][xn]) +
					static_cast<vx_int64>(window[yn][xn]) -
					static_cast<vx_int64>(window[y][x]) - 
					static_cast<vx_int64>(window[yn][x]);
			} else {
				sum = static_cast<vx_int64>(window[y][xn]) -
					static_cast<vx_int64>(window[y][x]);
			}

			// Add to result
			result += sum*mul;
		}
	}

	// Normalize
	const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
	const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

	// Return result
	return static_cast<OutType>(norm);
}

/** @brief Computes the y derivative (optimized for kernel symmetry)
@param InType        The input scalar data type
@param OutType       The output scalar data type
@param KernType      The kernel scalar data type
@param KERN_SIZE     The size of the kernel
@param kernel        The sobel/scharr kernel
@param window        The sliding window of this scalar computation
@param kernel_mult   Multiplier for normalization
@param kernel_shift  Shifter For normalization
@return              The result of the y derivative computation
*/
template <typename InType, typename OutType, typename KernType, const vx_uint16 KERN_SIZE>
OutType ComputeDerivativeY(KernType kernel[KERN_SIZE][KERN_SIZE], InType window[KERN_SIZE][KERN_SIZE], const vx_uint64 kernel_mult, const vx_uint32 kernel_shift) {
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
				sum = static_cast<vx_int64>(window[yn][x]) +
					static_cast<vx_int64>(window[yn][xn]) -
					static_cast<vx_int64>(window[y][x]) -
					static_cast<vx_int64>(window[y][xn]);
			} else {
				sum = static_cast<vx_int64>(window[yn][x]) -
					static_cast<vx_int64>(window[y][x]);
			}

			// Add to result
			result += sum*mul;
		}
	}

	// Normalize
	const vx_uint64 rounding = static_cast<vx_uint64>(1) << (kernel_shift - 1);
	const vx_uint64 norm = (result * kernel_mult + rounding) >> kernel_shift;

	// Return result
	return static_cast<OutType>(norm);
}

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
			if(kernel[y][x] == 0) {

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

template<typename InType, typename OutType, vx_uint16 KERN_SIZE>
OutType SegmentTestDetector(InType window[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	// Variables
	InType circle[16] = { 0 };
#pragma HLS array_partition variable=circle complete dim=0
	InType difference[16];
#pragma HLS array_partition variable=difference complete dim=0
	vx_uint8 direction[16];
#pragma HLS array_partition variable=direction complete dim=0

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

template<typename InType, typename OutType, vx_uint16 KERN_SIZE, InType THRESH_LOWER, InType THRESH_UPPER>
OutType Hysteresis(InType window[KERN_SIZE][KERN_SIZE]) {
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
			if ((y != KERN_RAD) && (x != KERN_RAD)) {
				InType neighbour = window[y][x];
				if ((neighbour >= THRESH_UPPER) && (weak == true)) {
					strong = true;
				}
			}
		}
	}
	OutType result = (strong == true) ? (MAX_VAL) : ((OutType)0);

	// Return result
	return result;
}

/*********************************************************************************************************************/
/* Filter Main Function */
/*********************************************************************************************************************/

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
template <typename ScalarType, typename KernelType, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE, vx_uint16 WIN_COLS, vx_uint16 VEC_NUM>
void ComputeFilterHorizontal(ScalarType window[WIN_COLS], KernelType kernel[KERN_SIZE], 
	const vx_uint64 kernel_mult, const vx_uint32 kernel_shift, ScalarType output[VEC_NUM]) {
#pragma HLS INLINE

	for (vx_uint16 vecId = 0; vecId < VEC_NUM; vecId++) {
#pragma HLS unroll

		// Window for single vector element
		ScalarType kernel_window[KERN_SIZE];
#pragma HLS array_partition variable=kernel_window complete dim=0

		// Get window for single vector element
		for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
			kernel_window[i] = window[i + vecId];
		}

		// Compute filter for single vector element
		ScalarType result = 0;
		switch (KERN_TYPE) {
		case HIFLIPVX::GAUSSIAN_FILTER:
			result = ComputeGaussian1d<ScalarType, KernelType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
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
template <typename ScalarType, typename KernelType, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE, vx_uint16 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
void ComputeFilterVertical(ScalarType window[KERN_SIZE][VEC_NUM], KernelType kernel[KERN_SIZE], 
	const vx_uint64 kernel_mult, const vx_uint32 kernel_shift, vx_image_data<ScalarType, VEC_NUM> &output, vx_uint16 x, vx_uint16 y) {
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
#pragma HLS array_partition variable=kernel_window complete dim=0

		// Get window for single vector element
		for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
			kernel_window[i] = window[i][vecId];
		}

		// Compute filter for single vector element
		ScalarType result = 0;
		switch (KERN_TYPE) {
		case HIFLIPVX::GAUSSIAN_FILTER:
			result = ComputeGaussian1d<ScalarType, KernelType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
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
template <typename InType, typename OutType, typename KernType, vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, HIFLIPVX::FilterOperation KERN_TYPE, vx_uint16 KERN_SIZE, vx_uint16 WIN_COLS, InType THRESH_LOWER, InType THRESH_UPPER, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
void ComputeFilter(InType window[KERN_SIZE][WIN_COLS], KernType kernel[KERN_SIZE][KERN_SIZE], 
	const vx_uint64 kernel_mult, const vx_uint32 kernel_shift, vx_image_data<OutType, VEC_NUM>& output, vx_uint16 x, vx_uint16 y) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
	const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
	const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;

	// Output
	vx_image_data<OutType, VEC_NUM> output_vector;

	// Compute KERN_NUM different filter with VEC_NUM elements per clock cycle
	for (vx_uint16 vecId = 0; vecId < VEC_NUM; vecId++) {
#pragma HLS unroll

		// Window for single vector element
		InType kernel_window[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_window complete dim=0

		// Get window for single vector element
		for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
			for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
				kernel_window[i][j] = window[i][j + vecId];
			}
		}

		// Check, if input or output is signed
		bool isSigned = (std::numeric_limits<InType>::is_signed) || (std::numeric_limits<OutType>::is_signed);

		// Compute filter for single vector element
		OutType result = 0;
		if (KERN_TYPE == HIFLIPVX::GAUSSIAN_FILTER) {
			result = ComputeGaussian2d<InType, OutType, KernType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
		} else if (KERN_TYPE == HIFLIPVX::DERIVATIVE_X) {
			result = ComputeDerivativeX<InType, OutType, KernType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
		} else if (KERN_TYPE == HIFLIPVX::DERIVATIVE_Y) {
			result = ComputeDerivativeY<InType, OutType, KernType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
		} else if (KERN_TYPE == HIFLIPVX::CUSTOM_CONVOLUTION) {
			if (isSigned)
				result = ComputeConvolve2d<InType, vx_int64, OutType, KernType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
			else
				result = ComputeConvolve2d<InType, vx_uint64, OutType, KernType, KERN_SIZE>(kernel, kernel_window, kernel_mult, kernel_shift);
		} else if (KERN_TYPE == HIFLIPVX::BOX_FILTER) {
			result = ComputeBox2d<InType, OutType, KERN_SIZE>(kernel_window, kernel_mult, kernel_shift);
		} else if (KERN_TYPE == HIFLIPVX::MEDIAN_FILTER) {
			if (KERN_SIZE == 3)
				result = ComputeMedian3x3<InType, OutType, KERN_SIZE>(kernel_window);
			else
				result = ComputeMedian<InType, OutType, KERN_SIZE>(kernel_window);
		} else if (KERN_TYPE == HIFLIPVX::NON_MAX_SUPPRESSION) {
			result = NonMaxSuppression<InType, OutType, KernType, KERN_SIZE>(kernel, kernel_window);
		} else if (KERN_TYPE == HIFLIPVX::SEGMENT_TEST_DETECTOR) {
			result = SegmentTestDetector<InType, OutType, KERN_SIZE>(kernel_window);
		} else if (KERN_TYPE == HIFLIPVX::HYSTERESIS) {
			result = Hysteresis<InType, OutType, KERN_SIZE, THRESH_LOWER, THRESH_UPPER>(kernel_window);
		} 
		output_vector.pixel[vecId] = result;
	}

	// Set user(SOF) & last(EOF)	
	const bool sof = ((y == KERN_RAD) && (x == OHD_COLS));
	const bool eof = ((y == (IMG_ROWS + KERN_RAD - 1)) && (x == (VEC_COLS + OHD_COLS - 1)));
	GenerateDmaSignal<OutType, VEC_NUM>(sof, eof, output_vector);

	// Write Output
	output = output_vector;
}

/** @brief  Checks the input parameters of the convolution filter
@param InType       Data type of the input image (8-bit, 16-bit, 32-bit)
@param OutType      Data type of the output image (8-bit, 16-bit, 32-bit)
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image width
@param KERN_SIZE    Kernel size (3, 5, 7, 9, 11)
*/
template<typename InType, typename OutType, vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, vx_uint16 IMG_COLS, vx_uint16 KERN_SIZE>
void CheckFilterParameters() {
#pragma HLS INLINE
	// Check function parameters/types
	const vx_uint16 src_size = sizeof(InType);
	const vx_uint16 dst_size = sizeof(OutType);
	const bool allowed_kernel_size = (KERN_SIZE == 3) || (KERN_SIZE == 5) || (KERN_SIZE == 7) || (KERN_SIZE == 9) || (KERN_SIZE == 11);
	const bool allowed_vector_size = (VEC_NUM == 1) || (VEC_NUM == 2) || (VEC_NUM == 4) || (VEC_NUM == 8);
	const bool allowed_data_type = (src_size == 1) || (src_size == 2) || (src_size == 4);
	const bool allowed_kernel_type = (src_size == 1) || (src_size == 2);
	const bool allowed_kernel_num = (KERN_NUM == 1) || (KERN_NUM == 2);
	STATIC_ASSERT(allowed_kernel_size, kernel_size_must_be_3_5_7_9_11);
	STATIC_ASSERT((IMG_COLS % VEC_NUM == 0), image_colums_are_not_multiple_of_vector_size);
	STATIC_ASSERT((src_size == dst_size), size_of_in_and_out_type_must_be_equal);
	STATIC_ASSERT(allowed_vector_size, vector_size_must_be_1_2_4_8);
	STATIC_ASSERT(allowed_data_type, data_type_must_be_8_16_32_bit);
	STATIC_ASSERT(allowed_kernel_type, kernel_data_type_must_be_8_16_bit);
	STATIC_ASSERT(allowed_kernel_num, kernel_num_can_only_be_1__or_2);
}


/*****************************************************************************************************************************************************************************************************/
/* Main Functions */
/*****************************************************************************************************************************************************************************************************/

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
template <typename ScalarType, typename KernelType, typename BufferType, vx_uint16 BUFFER_NUM, vx_uint32 VEC_PIX, vx_uint16 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE, vx_border_e BORDER_TYPE>
void ComputeFilter1dFunc(
	vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], 
	vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX], 
	KernelType kernel[KERN_SIZE], 
	const vx_uint64 kernel_mult, 
	const vx_uint32 kernel_shift) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
	const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
	const vx_uint16 WIN_COLS = 2 * KERN_RAD + VEC_NUM + (VEC_NUM - (KERN_RAD % VEC_NUM)) % VEC_NUM;
	const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;
	const vx_uint16 LINE_BUFFER_WIDTH = VEC_NUM*(KERN_SIZE - 1);

	// Linebuffer
	BufferType linebuffer[BUFFER_NUM][VEC_COLS];

	// Sliding window for complete vector
	ScalarType window_hor[1][WIN_COLS];
#pragma HLS array_partition variable=window_hor complete dim=0

	ScalarType window_ver[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=window_ver complete dim=0

	// Compute the filter (pipelined)
	vx_uint32 ptr_src = 0, ptr_dst = 0;
	for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; y++) {
		for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; x++) {
#pragma HLS PIPELINE II=1

			// Variables (Registers)
			ScalarType input_buffer[VEC_NUM];
#pragma HLS array_partition variable=input_buffer complete dim=0
			ScalarType buffer[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=buffer complete dim=0
			ScalarType internal_buffer[VEC_NUM];
#pragma HLS array_partition variable=internal_buffer complete dim=0
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
			SlidingWindowHorizontal<ScalarType, KERN_RAD, VEC_COLS, VEC_NUM, WIN_COLS, BORDER_TYPE>(input_buffer, window_hor, x);
			ComputeFilterHorizontal<ScalarType, KernelType, KERN_SIZE, KERN_TYPE, WIN_COLS, VEC_NUM>(window_hor[0], kernel, kernel_mult, kernel_shift, internal_buffer);

			// Compute linebuffer
			vx_int16 xs = x - OHD_COLS;
			if (xs >= 0) {				
				ReadFromLineBuffer<ScalarType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(internal_buffer, linebuffer, buffer, xs);
				WriteToLineBuffer<ScalarType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(buffer, linebuffer, xs);
			}

			// Compute Vertical Filter including sliding window
			SlidingWindowVertical<ScalarType, IMG_ROWS, KERN_RAD, VEC_NUM, KERN_SIZE, BORDER_TYPE>(buffer, window_ver, y);
			ComputeFilterVertical<ScalarType, KernelType, KERN_SIZE, KERN_TYPE, VEC_NUM, IMG_COLS, IMG_ROWS>(window_ver, kernel, kernel_mult, kernel_shift, output_data, x, y);

			// Write output data to global memory
			if ((y >= KERN_RAD) && (x >= OHD_COLS)) {
				output[ptr_dst] = output_data;
				ptr_dst++;
			}
		}
	}
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
@param IMG_COLS     Image width
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
template <typename InType, typename OutType, typename KernType, typename BufferType, vx_uint16 BUFFER_NUM, vx_uint32 VEC_PIX, vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, 
	HIFLIPVX::FilterOperation KERN_TYPE1, HIFLIPVX::FilterOperation KERN_TYPE2, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, vx_border_e BORDER_TYPE, InType THRESH_LOWER, InType THRESH_UPPER>
void ComputeFilter2dFunc(
	vx_image_data<InType, VEC_NUM> input[VEC_PIX], 
	vx_image_data<OutType, VEC_NUM> output1[VEC_PIX], 
	vx_image_data<OutType, VEC_NUM> output2[VEC_PIX], 
	KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE], 
	const vx_uint64 kernel_mult, 
	const vx_uint32 kernel_shift) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
	const vx_uint16 KERN_RAD = KERN_SIZE / 2;
	const vx_uint16 WIN_COLS = 2 * KERN_RAD + VEC_NUM + (VEC_NUM - (KERN_RAD % VEC_NUM)) % VEC_NUM;
	const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;
	const vx_uint16 LINE_BUFFER_WIDTH = VEC_NUM*(KERN_SIZE - 1);

	// Linebuffer
	BufferType linebuffer[BUFFER_NUM][VEC_COLS];

	// Sliding window for complete vector
	InType window[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable=window complete dim=0

	// Compute the filter (pipelined)
	vx_uint32 ptr_src = 0, ptr_dst = 0;
	for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; ++y) {
		for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II=1

			InType input_buffer[VEC_NUM];
#pragma HLS array_partition variable=input_buffer complete dim=0
			InType buffer[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=buffer complete dim=0

			// Input & Output
			vx_image_data<InType, VEC_NUM> input_data_2d;
			vx_image_data<OutType, VEC_NUM> output_data[KERN_NUM];

			// Read input data from global memory
			if ((y < IMG_ROWS) && (x < VEC_COLS)) {
				input_data_2d = input[ptr_src];
				++ptr_src;
			}
			for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS unroll
				input_buffer[v] = input_data_2d.pixel[v];
			}

			// Read data from line_buffer to buffer
			ReadFromLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(input_buffer, linebuffer, buffer, x);

			// Write data from buffer to line_buffer
			WriteToLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(buffer, linebuffer, x);

			// Move sliding window with replicated/constant border
			SlidingWindow<InType, VEC_NUM, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE>(buffer, window, x, y);

			// Compute filter
			if (KERN_NUM > 0)
				ComputeFilter<InType, OutType, KernType, VEC_NUM, KERN_NUM, KERN_TYPE1, KERN_SIZE, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS>(window, kernel[0], kernel_mult, kernel_shift, output_data[0], x, y);
			if (KERN_NUM > 1)
				ComputeFilter<InType, OutType, KernType, VEC_NUM, KERN_NUM, KERN_TYPE2, KERN_SIZE, WIN_COLS, THRESH_LOWER, THRESH_UPPER, IMG_COLS, IMG_ROWS>(window, kernel[1], kernel_mult, kernel_shift, output_data[1], x, y);

			// Write output data to global memory
			if ((y >= KERN_RAD) && (x >= OHD_COLS)) {
				if (KERN_NUM > 0)
					output1[ptr_dst] = output_data[0];
				if (KERN_NUM > 1)
					output2[ptr_dst] = output_data[1];
				++ptr_dst;
			}
		}
	}
}

/*****************************************************************************************************************************************************************************************************/
/* Main Functions (Top) */
/*****************************************************************************************************************************************************************************************************/

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
template <typename ScalarType, typename KernelType, vx_uint32 VEC_PIX, vx_uint16 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, HIFLIPVX::FilterOperation KERN_TYPE, vx_border_e BORDER_TYPE>
void ComputeFilter1d(
	vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],
	vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX],
	KernelType kernel[KERN_SIZE],
	const vx_uint64 kernel_mult, 
	const vx_uint32 kernel_shift) {
#pragma HLS INLINE

	// Check parameter input
	CheckFilterParameters<ScalarType, ScalarType, VEC_NUM, 1, IMG_COLS, KERN_SIZE>();

	// Constants to compute optimum buffer width
	const vx_uint16 DATA_WIDTH = sizeof(ScalarType) * VEC_NUM * (KERN_SIZE - 1);
	const vx_uint16 BUFFER_UINT32 = DATA_WIDTH % 4;
	const vx_uint16 BUFFER_UINT16 = DATA_WIDTH % 2;
	const vx_uint16 BUFFER_NUM = (BUFFER_UINT32 == 0) ? (DATA_WIDTH >> 2) : ((BUFFER_UINT16 == 0) ? (DATA_WIDTH >> 1) : (DATA_WIDTH));
	const vx_uint16 BUFFER_WIDTH = (BUFFER_UINT32 == 0) ? (4) : ((BUFFER_UINT16 == 0) ? (2) : (1));

	// Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
	if (BUFFER_WIDTH == 4)
		ComputeFilter1dFunc<ScalarType, KernelType, vx_uint32, BUFFER_NUM, VEC_PIX, VEC_NUM, IMG_COLS, IMG_ROWS, KERN_SIZE, KERN_TYPE, BORDER_TYPE>(input, output, kernel, kernel_mult, kernel_shift);
	else if (BUFFER_WIDTH == 2)
		ComputeFilter1dFunc<ScalarType, KernelType, vx_uint16, BUFFER_NUM, VEC_PIX, VEC_NUM, IMG_COLS, IMG_ROWS, KERN_SIZE, KERN_TYPE, BORDER_TYPE>(input, output, kernel, kernel_mult, kernel_shift);
	else if (BUFFER_WIDTH == 1)
		ComputeFilter1dFunc<ScalarType, KernelType, vx_uint8, BUFFER_NUM, VEC_PIX, VEC_NUM, IMG_COLS, IMG_ROWS, KERN_SIZE, KERN_TYPE, BORDER_TYPE>(input, output, kernel, kernel_mult, kernel_shift);
}

/** @brief  Computes multiple 2d convolution filter of kernel_type type
@param InType       Data type of the input image (8-bit, 16-bit, 32-bit)
@param OutType      Data type of the output image (8-bit, 16-bit, 32-bit)
@param KernType     Data type of the kernel (8-bit, 16-bit, 32-bit)
@param VEC_PIX      The amount of pixels diveded by the pixels computed in parallel
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param KERN_NUM     Amount of pixels computed in parallel (1, 2)
@param IMG_COLS     Image width
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
template <typename InType, typename OutType, typename KernType, vx_uint32 VEC_PIX, vx_uint16 VEC_NUM, vx_uint16 KERN_NUM, HIFLIPVX::FilterOperation KERN_TYPE1, HIFLIPVX::FilterOperation KERN_TYPE2,
	vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, vx_border_e BORDER_TYPE, InType THRESH_LOWER, InType THRESH_UPPER>
void ComputeFilter2d(
	vx_image_data<InType, VEC_NUM> input[VEC_PIX], 
	vx_image_data<OutType, VEC_NUM> output1[VEC_PIX],
	vx_image_data<OutType, VEC_NUM> output2[VEC_PIX], 
	KernType kernel[KERN_NUM][KERN_SIZE][KERN_SIZE], 
	const vx_uint64 kernel_mult, 
	const vx_uint32 kernel_shift) {
#pragma HLS INLINE

	// Check parameter input
	CheckFilterParameters<InType, OutType, VEC_NUM, KERN_NUM, IMG_COLS, KERN_SIZE>();

	// Constants to compute optimum buffer width
	const vx_uint16 DATA_WIDTH = sizeof(InType) * VEC_NUM * (KERN_SIZE - 1);
	const vx_uint16 BUFFER_UINT32 = DATA_WIDTH % 4;
	const vx_uint16 BUFFER_UINT16 = DATA_WIDTH % 2;
	const vx_uint16 BUFFER_NUM = (BUFFER_UINT32 == 0) ? (DATA_WIDTH >> 2) : ((BUFFER_UINT16 == 0) ? (DATA_WIDTH >> 1) : (DATA_WIDTH));
	const vx_uint16 BUFFER_WIDTH = (BUFFER_UINT32 == 0) ? (4) : ((BUFFER_UINT16 == 0) ? (2) : (1));

	// Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
	if (BUFFER_WIDTH == 4)
		ComputeFilter2dFunc<InType, OutType, KernType, vx_uint32, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_NUM, KERN_TYPE1, KERN_TYPE2, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output1, output2, kernel, kernel_mult, kernel_shift);
	else if (BUFFER_WIDTH == 2)
		ComputeFilter2dFunc<InType, OutType, KernType, vx_uint16, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_NUM, KERN_TYPE1, KERN_TYPE2, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output1, output2, kernel, kernel_mult, kernel_shift);
	else if (BUFFER_WIDTH == 1)
		ComputeFilter2dFunc<InType, OutType, KernType, vx_uint8, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_NUM, KERN_TYPE1, KERN_TYPE2, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output1, output2, kernel, kernel_mult, kernel_shift);
}


/*****************************************************************************************************************************************************************************************************/
/* Filter Function Definition */
/*****************************************************************************************************************************************************************************************************/

/** @brief  Computes a Box filter over a window of the input image. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void HwBox(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

	// Kernel Type
	typedef vx_uint16 KernelType;

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;
	const ScalarType THRESH_LOWER = 0;
	const ScalarType THRESH_UPPER = 0;

	if (SEPARABLE == vx_false_e) {

		// Function Input
		KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

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
		ComputeFilter2d<ScalarType, ScalarType, KernelType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::BOX_FILTER, HIFLIPVX::BOX_FILTER, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);

	} else {

		// Function Input
		KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

		// Compute kernel and normalization (Compile-Time)
		for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
			kernel_vector[i] = 1;
		}
		ComputeNormalization1d<KernelType, KERN_SIZE>(kernel_vector, kernel_mult, kernel_shift);

		// Compute Filter
		ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::BOX_FILTER, BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);
	}
}

/** @brief  Convolves the input with the client supplied convolution matrix. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwConvolve(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX], const ScalarType conv[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;
	const ScalarType THRESH_LOWER = 0;
	const ScalarType THRESH_UPPER = 0;

	// Function Input
	ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

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
	ComputeFilter2d<ScalarType, ScalarType, ScalarType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::CUSTOM_CONVOLUTION, HIFLIPVX::CUSTOM_CONVOLUTION, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);
}

/** @brief  Implements Dilation, which grows the white space in a Boolean image. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwDilate(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

	// Kernel Parameters
	typedef vx_uint16 KernelType;

	// Function Input
	KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Kernel normalization and type
	const vx_uint64 kernel_mult = 1;
	const vx_uint32 kernel_shift = 0;

	// Compute Filter
	ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::DILATE_IMAGE, BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);
}

/** @brief  Implements Erosion, which shrinks the white space in a Boolean image. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwErode(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

	// Kernel Parameters
	typedef vx_uint16 KernelType;

	// Function Input
	KernelType kernel_vector[KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Kernel normalization and type
	const vx_uint64 kernel_mult = 1;
	const vx_uint32 kernel_shift = 0;

	// Compute Filter
	ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::ERODE_IMAGE, BORDER_TYPE>(input, output, kernel_vector, kernel_mult, kernel_shift);
}

/** @brief  Computes a Gaussian filter over a window of the input image. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void HwGaussian(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

	// Kernel Type
	typedef vx_uint16 KernelType;

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;
	const ScalarType THRESH_LOWER = 0;
	const ScalarType THRESH_UPPER = 0;

	if (SEPARABLE == vx_false_e) {
		
		// Function Input
		KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0
		
		// Compute kernel and normalization (Compile-Time)
		ComputeGaussianKernels<KernelType, KERN_SIZE>(kernel_vector[0]);
		ComputeNormalization2d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);
		
		// Compute Filter
		ComputeFilter2d<ScalarType, ScalarType, KernelType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::GAUSSIAN_FILTER, HIFLIPVX::GAUSSIAN_FILTER, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);

	} else {		

		// Function Input
		KernelType kernel_vector[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

		// Compute kernel and normalization (Compile-Time)
		ComputeGaussianKernels<KernelType, KERN_SIZE>(kernel_vector);
		ComputeNormalization1d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

		// Compute Filter
		ComputeFilter1d<ScalarType, KernelType, VEC_PIX, VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, HIFLIPVX::GAUSSIAN_FILTER, BORDER_TYPE>(input, output, kernel_vector[0], kernel_mult, kernel_shift);
	}
}

/** @brief  Computes a median pixel value over a window of the input image. The output image dimensions should be the same as the dimensions of the input image.
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
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwMedian(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<ScalarType>::is_signed == false, scalar_type_must_be_unsigned);

	// Kernel Type
	typedef vx_uint16 KernelType;

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	const vx_uint64 kernel_mult = 1;
	const vx_uint32 kernel_shift = 0;
	const ScalarType THRESH_LOWER = 0;
	const ScalarType THRESH_UPPER = 0;

	// Function Input
	KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Compute Filter
	ComputeFilter2d<ScalarType, ScalarType, KernelType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::MEDIAN_FILTER, HIFLIPVX::MEDIAN_FILTER, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);
}

/** @brief  Implements the Scharr Image Filter Kernel. The output images dimensions should be the same as the dimensions of the input image.
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
template <typename InType, typename OutType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void HwScharr3x3(vx_image_data<InType, VEC_NUM> input[VEC_PIX], vx_image_data<OutType, VEC_NUM> output1[VEC_PIX], vx_image_data<OutType, VEC_NUM> output2[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<InType>::is_signed == false, derivative_input_must_be_unsigned);
	STATIC_ASSERT(numeric_limits<OutType>::is_signed == true, derivative_output_must_be_signed);

	// Kernel Type
	typedef vx_int16 KernelType;

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 2;
	const vx_uint8 KERN_SIZE = 3;
	const vx_uint64 kernel_mult = 1;
	const vx_uint32 kernel_shift = 5;
	const InType THRESH_LOWER = 0;
	const InType THRESH_UPPER = 0;

	// Function Input
	KernelType kernel_vector[2][KERN_SIZE][KERN_SIZE] = {
		{
			{ 3, 0, -3 },
			{ 10, 0, -10 },
			{ 3, 0, -3 },
		}, {
			{ 3, 10, 3 },
			{ 0, 0, 0 },
			{ -3, -10, -3 },
		} };
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Compute Filter
	ComputeFilter2d<InType, OutType, KernelType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::DERIVATIVE_X, HIFLIPVX::DERIVATIVE_Y, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output1, output2, kernel_vector, kernel_mult, kernel_shift);
}

/** @brief  Implements the Sobel Image Filter Kernel. The output images dimensions should be the same as the dimensions of the input image.
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
template <typename InType, typename OutType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwSobel(vx_image_data<InType, VEC_NUM> input[VEC_PIX], vx_image_data<OutType, VEC_NUM> output1[VEC_PIX], vx_image_data<OutType, VEC_NUM> output2[VEC_PIX]) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(numeric_limits<InType>::is_signed == false, input_must_be_unsigned);
	STATIC_ASSERT(numeric_limits<OutType>::is_signed == true, output_must_be_signed);

	// Kernel Type
	typedef vx_int16 KernelType;

	// Kernel Amount
	const vx_uint8 KERN_NUM = 2;

	// Kernel Normalization
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;
	const InType THRESH_LOWER = 0;
	const InType THRESH_UPPER = 0;

	// Function Input
	KernelType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Compute kernel and normalization (Compile-Time)
	ComputeSobelKernels<KernelType, KERN_SIZE>(kernel_vector[0], kernel_vector[1]);
	ComputeNormalization2d<KernelType, KERN_SIZE>(kernel_vector[0], kernel_mult, kernel_shift);

	// Compute Filter
	ComputeFilter2d<InType, OutType, KernelType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::DERIVATIVE_X, HIFLIPVX::DERIVATIVE_Y, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output1, output2, kernel_vector, kernel_mult, kernel_shift);
}

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
template<typename DataType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void HwNonMaxSuppression(vx_image_data<DataType, VEC_NUM> input[VEC_PIX], vx_image_data<DataType, VEC_NUM> output[VEC_PIX], const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;
	const DataType THRESH_LOWER = 0;
	const DataType THRESH_UPPER = 0;

	// Function Input
	vx_uint8 mask_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=mask_vector complete dim=0

	// Compute kernel and normalization (Compile-Time)
	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
			mask_vector[0][i][j] = mask[i][j];
		}
	}

	// Compute Filter
	ComputeFilter2d<DataType, DataType, vx_uint8, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::NON_MAX_SUPPRESSION, HIFLIPVX::NON_MAX_SUPPRESSION, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, mask_vector, kernel_mult, kernel_shift);
}

//
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void HwSegmentTestDetector(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	const vx_uint8 KERN_SIZE = 7;
	const ScalarType THRESH_LOWER = 0;
	const ScalarType THRESH_UPPER = 0;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;

	// Function Input
	ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Compute Filter
	ComputeFilter2d<ScalarType, ScalarType, ScalarType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::SEGMENT_TEST_DETECTOR, HIFLIPVX::SEGMENT_TEST_DETECTOR, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);
}

//
template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, ScalarType THRESH_LOWER, ScalarType THRESH_UPPER>
void HwHysteresis(vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX], vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Kernel Parameters
	const vx_uint8 KERN_NUM = 1;
	vx_uint64 kernel_mult = 1;
	vx_uint32 kernel_shift = 0;

	// Function Input
	ScalarType kernel_vector[KERN_NUM][KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_vector complete dim=0

	// Compute Filter
	ComputeFilter2d<ScalarType, ScalarType, ScalarType, VEC_PIX, VEC_NUM, KERN_NUM, HIFLIPVX::HYSTERESIS, HIFLIPVX::HYSTERESIS, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output, NULL, kernel_vector, kernel_mult, kernel_shift);
}

























template <typename InType, typename OutType, vx_uint32 KERN_SIZE, vx_uint32 QUANTIZATION>
OutType ComputeOrientedNonMaxSuppression(InType window1[KERN_SIZE][KERN_SIZE], InType window2[KERN_SIZE][KERN_SIZE]) {
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

template <typename InType, typename OutType, vx_uint32 VEC_NUM, HIFLIPVX::FilterOperation KERN_TYPE, vx_uint32 KERN_SIZE, vx_uint32 WIN_COLS, vx_uint16 IMG_ROWS, vx_uint32 IMG_COLS, vx_uint32 QUANTIZATION>
void ComputeFilter_2input(InType window1[KERN_SIZE][WIN_COLS], InType window2[KERN_SIZE][WIN_COLS], vx_image_data<OutType, VEC_NUM>& output, const vx_uint16 x, const vx_uint16 y) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
	const vx_uint16 KERN_RAD = KERN_SIZE / 2;
	const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;

	// Output
	vx_image_data<OutType, VEC_NUM> output_vector;

	// Compute KERN_NUM different filter with VEC_NUM elements per clock cycle
	for (vx_uint16 vecId = 0; vecId < VEC_NUM; vecId++) {
#pragma HLS unroll

		// Window for single vector element
		InType kernel_window1[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_window1 complete dim=0
		InType kernel_window2[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=kernel_window2 complete dim=0

		// Get window for single vector element
		for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
			for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
				kernel_window1[i][j] = window1[i][j + vecId];
				kernel_window2[i][j] = window2[i][j + vecId];
			}
		}

		// Compute filter for single vector element
		OutType result = 0;
		if (KERN_TYPE == HIFLIPVX::ORIENTED_NON_MAX_SUPPRESSION) {
			result = ComputeOrientedNonMaxSuppression<InType, OutType, KERN_SIZE, QUANTIZATION>(kernel_window1, kernel_window2);
		} 

		output_vector.pixel[vecId] = result;
	}

	// Set user(SOF) & last(EOF)	
	const bool sof = ((y == KERN_RAD) && (x == OHD_COLS));
	const bool eof = ((y == (IMG_ROWS + KERN_RAD - 1)) && (x == (VEC_COLS + OHD_COLS - 1)));
	GenerateDmaSignal<OutType, VEC_NUM>(sof, eof, output_vector);

	// Write output
	output = output_vector;
}

template <typename InType, typename OutType, typename BufferType, vx_uint32 BUFFER_NUM, vx_uint32 VEC_PIX, vx_uint32 VEC_NUM, HIFLIPVX::FilterOperation KERN_TYPE,
	vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, vx_border_e BORDER_TYPE1, vx_border_e BORDER_TYPE2, vx_uint32 QUANTIZATION>
void ComputeFilter2dFunc_2input(
	vx_image_data<InType, VEC_NUM> input1[VEC_PIX], 
	vx_image_data<InType, VEC_NUM> input2[VEC_PIX],
	vx_image_data<OutType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM;
	const vx_uint16 KERN_RAD = KERN_SIZE / 2;
	const vx_uint16 WIN_COLS = 2 * KERN_RAD + VEC_NUM + (VEC_NUM - (KERN_RAD % VEC_NUM)) % VEC_NUM;
	const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM - 1;

	// Linebuffer
	BufferType linebuffer1[BUFFER_NUM][VEC_COLS];
	BufferType linebuffer2[BUFFER_NUM][VEC_COLS];

	// Sliding window for complete vector
	InType window1[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable=window1 complete dim=0
	InType window2[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable=window2 complete dim=0

	// Compute the filter (pipelined)
	vx_uint32 ptr_src = 0, ptr_dst = 0;
	for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; ++y) {
		for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II=1

			InType input_buffer1[VEC_NUM];
#pragma HLS array_partition variable=input_buffer1 complete dim=0
			InType input_buffer2[VEC_NUM];
#pragma HLS array_partition variable=input_buffer2 complete dim=0
			InType buffer1[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=buffer1 complete dim=0
			InType buffer2[KERN_SIZE][VEC_NUM];
#pragma HLS array_partition variable=buffer2 complete dim=0

			// Input & Output
			vx_image_data<InType, VEC_NUM> input_data1;
			vx_image_data<InType, VEC_NUM> input_data2;
			vx_image_data<OutType, VEC_NUM> output_data;

			// Read input data from global memory
			if ((y < IMG_ROWS) && (x < VEC_COLS)) {
				input_data1 = input1[ptr_src];
				input_data2 = input2[ptr_src];
				++ptr_src;
			}
			for (vx_uint16 v = 0; v < VEC_NUM; v++) {
#pragma HLS unroll
				input_buffer1[v] = input_data1.pixel[v];
				input_buffer2[v] = input_data2.pixel[v];
			}

			// Read data from line_buffer to buffer
			ReadFromLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(input_buffer1, linebuffer1, buffer1, x);
			ReadFromLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(input_buffer2, linebuffer2, buffer2, x);

			// Write data from buffer to line_buffer
			WriteToLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(buffer1, linebuffer1, x);
			WriteToLineBuffer<InType, BufferType, BUFFER_NUM, VEC_NUM, KERN_SIZE, VEC_COLS>(buffer2, linebuffer2, x);

			// Move sliding window with replicated/constant border
			SlidingWindow<InType, VEC_NUM, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE1>(buffer1, window1, x, y);
			SlidingWindow<InType, VEC_NUM, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE, BORDER_TYPE2>(buffer2, window2, x, y);

			// Compute filter
			ComputeFilter_2input<InType, OutType, VEC_NUM, KERN_TYPE, KERN_SIZE, WIN_COLS, IMG_ROWS, IMG_COLS, QUANTIZATION>(window1, window2, output_data, x, y);

			// Write output data to global memory
			if ((y >= KERN_RAD) && (x >= OHD_COLS)) {
				output[ptr_dst] = output_data;
				++ptr_dst;
			}
		}
	}
}

template <typename InType, typename OutType, vx_uint32 VEC_PIX, vx_uint32 VEC_NUM, HIFLIPVX::FilterOperation KERN_TYPE, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint32 KERN_SIZE, vx_border_e BORDER_TYPE1, vx_border_e BORDER_TYPE2, vx_uint32 QUANTIZATION>
void ComputeFilter2d_2input(
	vx_image_data<InType, VEC_NUM> input[VEC_PIX],
	vx_image_data<InType, VEC_NUM> input_no_window[VEC_PIX],
	vx_image_data<OutType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	// Check parameter input
	CheckFilterParameters<InType, OutType, VEC_NUM, 1, IMG_COLS, KERN_SIZE>();

	// Constants to compute optimum buffer width
	const vx_uint16 DATA_WIDTH = sizeof(InType) * VEC_NUM * (KERN_SIZE - 1);
	const vx_uint16 BUFFER_UINT32 = DATA_WIDTH % 4;
	const vx_uint16 BUFFER_UINT16 = DATA_WIDTH % 2;
	const vx_uint16 BUFFER_NUM = (BUFFER_UINT32 == 0) ? (DATA_WIDTH >> 2) : ((BUFFER_UINT16 == 0) ? (DATA_WIDTH >> 1) : (DATA_WIDTH));
	const vx_uint16 BUFFER_WIDTH = (BUFFER_UINT32 == 0) ? (4) : ((BUFFER_UINT16 == 0) ? (2) : (1));

	// Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
	if (BUFFER_WIDTH == 4)
		ComputeFilter2dFunc_2input<InType, OutType, vx_uint32, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_TYPE, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE1, BORDER_TYPE2, QUANTIZATION>(input, input_no_window, output);
	if (BUFFER_WIDTH == 2)
		ComputeFilter2dFunc_2input<InType, OutType, vx_uint16, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_TYPE, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE1, BORDER_TYPE2, QUANTIZATION>(input, input_no_window, output);
	if (BUFFER_WIDTH == 1)
		ComputeFilter2dFunc_2input<InType, OutType, vx_uint8, BUFFER_NUM, VEC_PIX, VEC_NUM, KERN_TYPE, IMG_COLS, IMG_ROWS, KERN_SIZE, BORDER_TYPE1, BORDER_TYPE2, QUANTIZATION>(input, input_no_window, output);
}


template<typename ScalarType, vx_uint32 VEC_PIX, vx_uint32 VEC_NUM, vx_uint32 WIDTH, vx_uint32 HEIGHT, vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>
void HwOrientedNonMaxSuppression(
	vx_image_data<ScalarType, VEC_NUM> input[VEC_PIX],
	vx_image_data<ScalarType, VEC_NUM> input_no_window[VEC_PIX],
	vx_image_data<ScalarType, VEC_NUM> output[VEC_PIX]) {
#pragma HLS INLINE

	STATIC_ASSERT(QUANTIZATION >= 3, quantization_of_minimum_3_needed);

	ComputeFilter2d_2input<ScalarType, ScalarType, VEC_PIX, VEC_NUM, HIFLIPVX::ORIENTED_NON_MAX_SUPPRESSION, WIDTH, HEIGHT, 3, BORDER_TYPE, VX_BORDER_UNDEFINED, QUANTIZATION>(input, input_no_window, output);
}


#endif /* SRC_IMG_FILTER_CORE_H_ */
