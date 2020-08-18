/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_helper.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
* @brief Description:\n
*  These are helper functions needed by different functions (e.g. static assertion).
* All own Enumerations, Typedefs, Macros and Namespaces are contained in this file.
*/

#ifndef SRC_IMG_HELPER_H_
#define SRC_IMG_HELPER_H_

/*! \brief set this to SDSoC, when using SDSoC */
//#define SDSOC

/*********************************************************************************************************************/
/* All includes libraries are here */
/*********************************************************************************************************************/

#define _USE_MATH_DEFINES

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <limits>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "vx_types.h"
#include <vector>
#ifdef __SDSCC__
#include "sds_lib.h"
#endif

/*********************************************************************************************************************/
/* Global function */
/*********************************************************************************************************************/

/*! \brief Needed as image type for vectorization
@param TYPE The data type for the pixels
@param SIZE The vectorization degree
*/
template<class TYPE, const size_t SIZE>
struct vx_image {
	TYPE pixel[SIZE];
};

/*! \brief Creates an image object
@param  ImageType The output image type (use "struct vx_image<...>" for vectorization of 2, 4 and 8)
@param  PixelType The data type of the image pixels
@param  COLS      The image columns
@param  ROWS      The image rows
@return           Pointer to the allocated memory
*/
template<typename ImageType, typename PixelType, vx_uint32 COLS, vx_uint32 ROWS>
__inline ImageType *CreateImage() {
#ifdef __SDSCC__
	return (ImageType *)sds_alloc(sizeof(PixelType)*COLS*ROWS);
#else
	return (ImageType *)(new PixelType[COLS*ROWS]);
#endif
}

/*! \brief Destroys an image object
@param  ImageType The image type
@param  image     The image
*/
template<typename ImageType>
__inline void DestroyImage(ImageType *image) {
#ifdef __SDSCC__
	return sds_free(image);
#else
	return delete[] image;
#endif
}

/*********************************************************************************************************************/
/* Own Macros */
/*********************************************************************************************************************/

// Maximum and Minumum values of datatypes
#define VX_INT8_MIN     static_cast<int8_t>(0-128)         /*!< \brief Minimum of signed 8 bit type */
#define VX_INT16_MIN    static_cast<int16_t>(0-32768)      /*!< \brief Minimum of signed 16 bit type */
#define VX_INT32_MIN    static_cast<int32_t>(0-2147483648) /*!< \brief Minimum of signed 32 bit type */
#define VX_INT8_MAX     static_cast<int8_t>(127)           /*!< \brief Maximum of signed 8 bit type */
#define VX_INT16_MAX    static_cast<int16_t>(32767)        /*!< \brief Maximum of signed 16 bit type */
#define VX_INT32_MAX    static_cast<int32_t>(2147483647)   /*!< \brief Maximum of signed 32 bit type */
#define VX_UINT8_MAX    static_cast<uint8_t>(0xff)         /*!< \brief Maximum of unsigned 8 bit type */
#define VX_UINT16_MAX   static_cast<uint16_t>(0xffff)      /*!< \brief Maximum of unsigned 16 bit type */
#define VX_UINT32_MAX   static_cast<uint32_t>(0xffffffff)  /*!< \brief Maximum of unsigned 32 bit type */

#ifndef M_PI
#define M_PI 3.14159265358979323846 /*!< \brief PI if not defined by cmath */
#endif

/*! \brief Gets the datatype of an integer:
* \details The output is of type <tt>\ref vx_type_e</tt> and is 8, 16, 32, 64 bit signed or unsigned
*/
#define GET_TYPE(TYPE) (const vx_type_e)( \
	(std::numeric_limits<TYPE>::is_integer == true) ? ( \
		(std::numeric_limits<TYPE>::is_signed == false) ? ( \
			(sizeof(TYPE) == 1) ? (VX_TYPE_UINT8) : ( \
				(sizeof(TYPE) == 2) ? (VX_TYPE_UINT16) : ( \
					(sizeof(TYPE) == 4) ? (VX_TYPE_UINT32) : ( \
						(sizeof(TYPE) == 8) ? (VX_TYPE_UINT64) : ( \
							VX_TYPE_INVALID))))) :  ( \
			(sizeof(TYPE) == 1) ? (VX_TYPE_INT8) : ( \
				(sizeof(TYPE) == 2) ? (VX_TYPE_INT16) : ( \
					(sizeof(TYPE) == 4) ? (VX_TYPE_INT32) : ( \
						(sizeof(TYPE) == 8) ? (VX_TYPE_INT64) : ( \
							VX_TYPE_INVALID)))))) : ( \
	VX_TYPE_INVALID) \
)

#define VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM) (const vx_uint32) \
	((static_cast<vx_uint32>(HEIGHT)*static_cast<vx_uint32>(WIDTH)) / static_cast<vx_uint32>(VEC_NUM))

/*********************************************************************************************************************/
/* Own enums  */
/*********************************************************************************************************************/

/*! \brief Contains the different pixelwise and filter operation types
*/
namespace HIFLIPVX {

	/*! \brief Perfroms a windowed filter function on an image
	*/
	enum FilterOperation {
		GAUSSIAN_FILTER,     /*!< \brief Gaussian filter (optimized for kernel structure) */
		DERIVATIVE_X,        /*!< \brief Scharr/Sobel derivative filter (optimized for kernel structure) */
		DERIVATIVE_Y,        /*!< \brief Scharr/Sobel derivative filter (optimized for kernel structure) */
		CUSTOM_CONVOLUTION,  /*!< \brief Costum convolution filter */
		BOX_FILTER,          /*!< \brief Box filter (optimized for kernel structure) */
		MEDIAN_FILTER,       /*!< \brief Median filter (optimized for kernel structure) */
		ERODE_IMAGE,         /*!< \brief Erode image filter (sperable kernel only) */
		DILATE_IMAGE,        /*!< \brief Dilate image filter (sperable kernel only) */
		NON_MAX_SUPPRESSION  /*!< \brief Do Non-Maxima Suppression */
	};

	/*! \brief Performs an pixelwise operation on an image (same data type for input/output)
	*/
	enum PixelwiseOperationA {
		COPY_DATA,    /*!< \brief Copy a data object to another. */
		BITWISE_NOT,  /*!< \brief Performs a bitwise NOT operation on a input images. */
		BITWISE_AND,  /*!< \brief Performs a bitwise AND operation between two unsigned images. */
		BITWISE_OR,   /*!< \brief Performs a bitwise INCLUSIVE OR operation between two unsigned images. */
		BITWISE_XOR,  /*!< \brief Performs a bitwise EXCLUSIVE OR (XOR) operation between two unsigned images. */
		MAX,          /*!< \brief Implements a pixel-wise maximum kernel. */
		MIN,          /*!< \brief Implements a pixel-wise minimum kernel. */
		ABSOLUTE_DIFFERENCE,   /*!< \brief Computes the absolute difference between two images */
		ARITHMETIC_ADDITION,   /*!< \brief Performs addition between two images */
		ARITHMETIC_SUBTRACTION, /*!< \brief Performs subtraction between two images */
		MAGNITUDE,               /*!< \brief Implements the Gradient Magnitude Computation Kernel */
		MULTIPLY                 /*!< \brief Performs element-wise multiplication between two images and a scalar value. */
	};

	/*! \brief Performs an pixelwise operation on an image (vary data type for input/output)
	*/
	enum PixelwiseOperationB {
		AC_IMAGE,    /*!< \brief  Accumulates an input image into output image. */
		AC_SQUARED,  /*!< \brief  Accumulates a squared value from an input image to an output image. */
		AC_WEIGHTED, /*!< \brief  Accumulates a weighted value from an input image to an output image.  */
		THRESHOLD    /*!< \brief  Thresholds an input image and produces an output Boolean image. */
	};
}

/*********************************************************************************************************************/
/* Static Assertion (COMPILE TIME) */
/*********************************************************************************************************************/

// note that we wrap the non existing type inside a struct to avoid warning
// messages about unused variables when static assertions are used at function
// scope
// the use of sizeof makes sure the assertion error is not ignored by SFINAE
template <bool>
struct StaticAssertion;
template <>
struct StaticAssertion<true>
{
}; // StaticAssertion<true>
template<int i>
struct StaticAssertionTest
{
}; // StaticAssertionTest<int>

#define CONCATENATE(arg1, arg2)   CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2)  CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2)  arg1##arg2

/*! \brief Pre-C++11 Static Assertions Without Boost
* \details: from  http://stackoverflow.com/questions/1980012/boost-static-assert-without-boost/1980156
*
* <code>STATIC_ASSERT(expression, message)</code>
*
* When the static assertion test fails, a compiler error message that somehow
* contains the "STATIC_ASSERTION_FAILED_AT_LINE_xxx_message" is generated.
*
* /!\ message has to be a valid C++ identifier, that is to say it must not
* contain space characters, cannot start with a digit, etc.
*
* STATIC_ASSERT(true, this_message_will_never_be_displayed);
*/
#define STATIC_ASSERT(expression, message)\
struct CONCATENATE(__static_assertion_at_line_, __LINE__)\
{\
	StaticAssertion<static_cast<bool>((expression))> CONCATENATE(CONCATENATE(CONCATENATE(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__), _), message);\
};\
typedef StaticAssertionTest<sizeof(CONCATENATE(__static_assertion_at_line_, __LINE__))> CONCATENATE(__static_assertion_test_at_line_, __LINE__)

/*********************************************************************************************************************/
/* HW: Helper Functions for arithmetic computation  */
/*********************************************************************************************************************/

/*! \brief Gets the maximum value of a certain data type
@param Type  The data type that should be evaluated
@return      The maximum value for "Type"
*/
template<typename Type>
Type ComputeMax() {
#pragma HLS INLINE
	const vx_type_e MAX_TYPE = GET_TYPE(Type);
	return
		(MAX_TYPE == VX_TYPE_UINT8) ? (VX_UINT8_MAX) : (
		(MAX_TYPE == VX_TYPE_UINT16) ? (VX_UINT16_MAX) : (
			(MAX_TYPE == VX_TYPE_UINT32) ? (VX_UINT32_MAX) : (
			(MAX_TYPE == VX_TYPE_INT8) ? (VX_INT8_MAX) : (
				(MAX_TYPE == VX_TYPE_INT16) ? (VX_INT16_MAX) : (
				(MAX_TYPE == VX_TYPE_INT32) ? (VX_INT32_MAX) : (
					VX_TYPE_INVALID))))));
}

/*! \brief Gets the minimum value of a certain data type
@param Type  The data type that should be evaluated
@return      The minimum value for "Type"
*/
template<typename Type>
Type ComputeMin() {
#pragma HLS INLINE
	const vx_type_e MIN_TYPE = GET_TYPE(Type);
	return
		(MIN_TYPE == VX_TYPE_UINT8) ? (0) : (
		(MIN_TYPE == VX_TYPE_UINT16) ? (0) : (
			(MIN_TYPE == VX_TYPE_UINT32) ? (0) : (
			(MIN_TYPE == VX_TYPE_INT8) ? (VX_INT8_MIN) : (
				(MIN_TYPE == VX_TYPE_INT16) ? (VX_INT16_MIN) : (
				(MIN_TYPE == VX_TYPE_INT32) ? (VX_INT32_MIN) : (
					VX_TYPE_INVALID))))));
}

/*! \brief Saturates a function to a maximum and minimum value
@param Type    The data type of the value
@param data    The input value
@param max_val The maximum value
@param min_val The minimum value
@return        The saturated value
*/
template<typename Type>
Type SaturateMaxMin(Type data, Type max_val, Type min_val) {
#pragma HLS INLINE
	if (data > max_val)
		return max_val;
	else if (data < min_val)
		return min_val;
	else
		return data;
}

/*! \brief An arithmetic right shift that simulates the behavior of division. For signed values there is a different behavior between shift and division when rounding.
@param Type             The data type of the value
@param SIGNED_DATA_TYPE If value is signed
@param data             The input value
@param shift            The shift value
@return                 The result
*/
template<typename Type, bool SIGNED_DATA_TYPE>
Type ShiftArithmeticRight(Type data, vx_uint16 shift) {
#pragma HLS INLINE
	Type a = data >> shift;
	if ((SIGNED_DATA_TYPE == true) && (a < 0)) {
		a += 1;
	}
	return a;
}

/** @brief Computes square root: Rounding to floor or to nearest integer
@param OutType		The data type of the output
@param InType		The data type of the input
@param ROUND_POLICY Rounding policy (to zero & nearest even)
@param CHECK_MAX	Is true, if the maximum value should be checked (needed for rounding to nearest even)
@param value		The input value
@return				The square root of the input value
*/
template<typename OutType, typename InType, bool CHECK_MAX, vx_round_policy_e ROUND_POLICY>
OutType SqrtLester(InType value) {
#pragma HLS INLINE

	const OutType MAX_VAL = ComputeMax<OutType>();

	// Number of stages (latency)
	const vx_uint8 N = sizeof(OutType) * 8;

	// Variables
	OutType   A1 = 0;  // A^1 Intermediate result
	InType A2 = 0;  // A^2 Square of the intermediate result

	// Each stage computes 1 bit of the resulting vector
	for (vx_uint8 n = N - 1; n < N; n--) {
#pragma HLS unroll

		// Add new bit of position n and compute (A1 + B1)^2
		OutType B1 = static_cast<OutType>( 1) << (n);
		InType  B2 = static_cast<InType >(B1) << (n);
		InType  AB = static_cast<InType >(A1) << (n);
		InType  A2_next = A2 + B2 + (AB << 1); // A*A + B*B + 2*A*B

		// Store if tmp does not exceed value
		if (A2_next <= value) {
			A1 |= B1;
			A2 = A2_next;
		}
	}

	// Round to the nearest integer and check for overflow
	if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
		if (CHECK_MAX) {
			if (((value - A2) > static_cast<InType>(A1)) && (A1 != MAX_VAL))
				A1++;
		} else {
			if ((value - A2) > static_cast<InType>(A1))
				A1++;
		}
	}

	// Return result
	return A1;
}

/*********************************************************************************************************************/
/* HW: Helper Functions to Create Kernels (COMPILE TIME) */
/*********************************************************************************************************************/

/** @brief Computes the Gaussian Kernel using the Sigma values (not compile time in all SDx versions)
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The computed gaussian kernel
@param sigma      The input sigma value
@return           The fraction size of the fixed-point result
*/
template <typename ScalarType, const vx_uint8 KERN_SIZE>
const vx_uint16 ComputeGaussianKernelSigma(ScalarType kernel[KERN_SIZE][KERN_SIZE], const double sigma) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 K_RAD = (vx_uint16)KERN_SIZE / (vx_uint16)2;
	const double s = 2.0 * sigma * sigma;

	// Variables
	double sum = 0.0;
	double doubleKernel[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable=doubleKernel complete dim=0

	// Creates the gaussian kernel
	for (vx_uint8 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
		for (vx_uint8 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
			const vx_uint32 a = static_cast<vx_uint32>((vx_uint16)x - K_RAD);
			const vx_uint32 b = static_cast<vx_uint32>((vx_uint16)y - K_RAD);
			const double c = static_cast<double>(a*a + b*b);
			const double r = sqrt(c);
			doubleKernel[x][y] = (exp(-(r*r) / s)) / (M_PI * s);
			sum += doubleKernel[x][y];
		}
	}

	// Normalizes the gaussian kernel
	for (vx_uint8 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
		for (vx_uint8 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
			doubleKernel[x][y] /= sum;
		}
	}

	// Computes the fraction for the fixed point representation
	vx_int32 e_max = VX_INT32_MIN;
	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
			int e = 0;
			frexp(doubleKernel[i][j], &e);
			e_max = std::max(e_max, (vx_int32)e);
		}
	}
	const vx_int32 fraction = 8 * sizeof(ScalarType) - e_max;
	const vx_uint16 kernFraction = static_cast<vx_uint16>(std::min(std::max(fraction, (vx_int32)0), (vx_int32)31));

	// Computes and stores the fixed point kernel
	vx_uint32 shift = 1;
	for (vx_uint16 i = 0; i < kernFraction; i++) {
#pragma HLS unroll
		shift *= 2;
	}
	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
			kernel[i][j] = static_cast<ScalarType>(doubleKernel[i][j] * static_cast<double>(shift));
		}
	}

	// Returns the fraction
	return kernFraction;
}

/** @brief Convolute an array with the convolution array [1 2 1]
@param ScalarType      Data type of the kernel
@param KERN_SIZE       The size of the final kernel
@param BUFFER_SIZE     The amount of kernels that can be stored in "kernel_buffer"
@param kernel_buffer   Buffers input and output kernel
@param cur_kernel_ptr  Pointer where the input kernel is stored
@param cur_kernel_size The size of the input kernel
*/
template<typename ScalarType, vx_uint8 KERN_SIZE, vx_int16 BUFFER_SIZE>
void ComputeConvolution(ScalarType kernel_buffer[BUFFER_SIZE][KERN_SIZE], const vx_int16 cur_kernel_ptr, const vx_int16 cur_kernel_size) {
#pragma HLS INLINE

	// Constants
	const vx_int16 kernel_size = cur_kernel_size + 2;
	const vx_int16 kernel_rad = kernel_size >> 1;

	// Standard convolution kernel
	ScalarType convoltion[3] = { 1, 2, 1 };

	// convolute kernel
	for (vx_int32 j = 0; j < kernel_size; j++) {
#pragma HLS unroll
		const vx_int32 range = std::min((j < kernel_rad) ? (j + 1) : (kernel_size - j), (vx_int32)3);
		const vx_int32 start_0 = std::max(2 - j, (vx_int32)0);
		const vx_int32 start_1 = std::max(j - 2, (vx_int32)0);
		ScalarType sum = 0;
		for (vx_int32 k = 0; k < range; k++) {
#pragma HLS unroll
			sum += (convoltion[start_0 + k] * kernel_buffer[cur_kernel_ptr][start_1 + k]);
		}
		kernel_buffer[cur_kernel_ptr + 1][j] = sum;
	}
}

/** @brief Computes 2d kernel from two 1d kernels
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param src1       The 1. 1d kernel
@param src2       The 2. 1d kernel
@param dst        The resulting 2d kernel
*/
template<typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeKernel(ScalarType src1[KERN_SIZE], ScalarType src2[KERN_SIZE], ScalarType dst[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
			dst[i][j] = src1[i] * src2[j];
		}
	}
}

/** @brief Computes the 1d kernels for the sobel filter
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param sobel_x    Stores the sobel kernel in x-direction
@param sobel_y    Stores the sobel kernel in y-direction
*/
template<typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeSobelKernels(ScalarType sobel_x[KERN_SIZE][KERN_SIZE], ScalarType sobel_y[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	// Constants and variables
	const vx_int16 ONE = static_cast<vx_int16>(1);
	const vx_int16 THREE = static_cast<vx_int16>(3);
	const vx_int16 BUFFER_SIZE = (((vx_int16)KERN_SIZE - THREE) >> ONE) + ONE;
	vx_int16 buffer_ptr = 0;

	// Sobel 1d kernel
	ScalarType buffer_0[BUFFER_SIZE][KERN_SIZE];
	buffer_0[0][0] = 1;
	buffer_0[0][1] = 2;
	buffer_0[0][2] = 1;
	ScalarType buffer_1[BUFFER_SIZE][KERN_SIZE];
	buffer_1[0][0] = -1;
	buffer_1[0][1] = 0;
	buffer_1[0][2] = 1;

	// vx_uint8 kernel for higher kernel size
	for (vx_uint8 i = 3; i < KERN_SIZE; i += 2) {
#pragma HLS unroll
		ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_0, buffer_ptr, (vx_int16)i);
		ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_1, buffer_ptr, (vx_int16)i);
		buffer_ptr++;
	}

	// Compute the 2d kernel
	ComputeKernel<ScalarType, KERN_SIZE>(buffer_0[buffer_ptr], buffer_1[buffer_ptr], sobel_x);
	ComputeKernel<ScalarType, KERN_SIZE>(buffer_1[buffer_ptr], buffer_0[buffer_ptr], sobel_y);
}

/** @brief Computes the Gaussian kernel
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param gaussian   Stores the gaussian kernel
*/
template<typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeGaussianKernels(ScalarType gaussian[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

	const vx_int16 ONE = static_cast<vx_int16>(1);
	const vx_int16 THREE = static_cast<vx_int16>(3);

	// Constants and variables
	const vx_int16 BUFFER_SIZE = (((vx_int16)KERN_SIZE - THREE) >> ONE) + ONE;
	vx_int16 buffer_ptr = 0;

	// Gaussian 1d kernel
	ScalarType buffer_0[BUFFER_SIZE][KERN_SIZE];
	buffer_0[0][0] = 1;
	buffer_0[0][1] = 2;
	buffer_0[0][2] = 1;

	// Computer Gaussian kernel with bigger kernel sizes
	for (vx_uint8 i = 3; i < KERN_SIZE; i += 2) {
#pragma HLS unroll
		ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_0, buffer_ptr, (vx_int16)i);
		buffer_ptr++;
	}

	// Compute the 2d kernel from the 1d kernel
	ComputeKernel<ScalarType, KERN_SIZE>(buffer_0[buffer_ptr], buffer_0[buffer_ptr], gaussian);
}

/** @brief Computes the normalization with given sum of kernel coefficients
@param kernel_sum Sum of the kernel coefficients
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
void ComputeNormalization(const vx_uint64 kernel_sum, vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

	// Constants
	const vx_uint32 MAX_SHIFT = static_cast<vx_uint32>(48);
	const vx_uint32 MIN_SHIFT = static_cast<vx_uint32>(15);
	const vx_uint64 ONE = static_cast<vx_uint64>(1);
	const vx_uint64 ZERO = static_cast<vx_uint64>(0);

	// Variables
	vx_uint16 new_shift = 0;
	vx_uint16 new_mult = 1;
	vx_uint32 old_shift = 0;
	vx_uint64 old_mult = 1;
	vx_uint64 norm_mult_a = 1;
	vx_uint32 norm_shift_a = 0;
	vx_uint64 norm_mult_b = 1;
	vx_uint32 norm_shift_b = 0;
	vx_uint64 temp = 0;
	bool is_power_two = false;

	// Compute Mult and Shift (Not Power of two)
	old_mult = temp = (ONE << MAX_SHIFT) / kernel_sum;
	for (vx_uint32 i = 0; i < MAX_SHIFT; i++) {
#pragma HLS unroll
		if (temp > ZERO) {
			temp = temp >> static_cast<vx_uint32>(1);
			old_shift = i;
		}
	}
	if (old_shift > 16) {
		new_shift = static_cast<vx_uint16>(MAX_SHIFT - (old_shift - MIN_SHIFT));
		new_mult = static_cast<vx_uint16>(old_mult >> (old_shift - MIN_SHIFT));
	} else {
		new_shift = static_cast<vx_uint16>(MAX_SHIFT);
		new_mult = static_cast<vx_uint16>(old_mult);
	}
	norm_mult_a = static_cast<vx_uint64>(new_mult);
	norm_shift_a = static_cast<vx_uint32>(new_shift);

	// Compute Mult and Shift (Power of two)
	for (vx_uint32 i = 0; i < sizeof(vx_uint32) * 8; i++) {
#pragma HLS unroll
		if ((kernel_sum & (ONE << i)) != ZERO)
			norm_shift_b = i;
	}

	// Check if scaling is by a power of two
	is_power_two = (kernel_sum & (kernel_sum - ONE)) == ZERO;

	// Compute Normalization and Multiplication
	norm_mult = (is_power_two) ? (norm_mult_b) : (norm_mult_a);
	norm_shift = (is_power_two) ? (norm_shift_b) : (norm_shift_a);
}

/** @brief Computes the normalization of a 2d kernel ((a * norm_mult) >> norm_shift)
@param KernType   Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The kernel itself
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
template <typename KernType, vx_uint8 KERN_SIZE>
void ComputeNormalization2d(KernType kernel[KERN_SIZE][KERN_SIZE], vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

	// Variables
	vx_uint64 kernel_sum = 0;

	// Sum the kernel values
	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
			kernel_sum += static_cast<vx_uint64>(abs(static_cast<vx_int64>(kernel[i][j])));
		}
	}

	// Compute Normalization
	ComputeNormalization(kernel_sum, norm_mult, norm_shift);
}

/** @brief Computes the normalization of a 1d kernel ((a * norm_mult) >> norm_shift)
@param KernType   Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The kernel itself
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
template <typename KernType, vx_uint8 KERN_SIZE>
void ComputeNormalization1d(KernType kernel[KERN_SIZE], vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

	// Variables
	vx_uint64 kernel_sum = 0;

	// Sum the kernel values for normalization (pre-compile)
	for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
		kernel_sum += static_cast<vx_uint64>(abs(static_cast<vx_int64>(kernel[i])));
	}

	// Compute Normalization
	ComputeNormalization(kernel_sum, norm_mult, norm_shift);
}

#endif /* SRC_IMG_HELPER_H_ */
