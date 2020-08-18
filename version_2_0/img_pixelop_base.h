/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_pixelop_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These are all pixel operation functions (Call from here)
*/

#ifndef SRC_IMG_PIXELOP_BASE_H_
#define SRC_IMG_PIXELOP_BASE_H_

#include "img_pixelop_core.h"

/***************************************************************************************************************************************************/
/* Pixelwise Function */
/***************************************************************************************************************************************************/

/** @brief Copy data from an input data object into another data object. 
@param DataType        Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgCopy(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::COPY_DATA, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input1, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgCopy(DataType input1[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::COPY_DATA, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input1, output);
}

/** @brief Performs a bitwise NOT operation on a input image. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgNot(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_NOT, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input1, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgNot(DataType input1[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_NOT, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input1, output);
}

/** @brief Performs a bitwise AND operation between two images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgAnd(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_AND, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgAnd(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_AND, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Performs a bitwise INCLUSIVE OR operation between two images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgOr(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_OR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgOr(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_OR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Performs a bitwise EXCLUSIVE OR (XOR) operation between two images. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgXor(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_XOR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgXor(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_XOR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Implements a pixel-wise maximum kernel. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMax(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAX, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMax(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAX, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}

/** @brief Implements a pixel-wise minimum kernel. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMin(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MIN, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMin(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MIN, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}

/** @brief Computes the absolute difference between two images. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAbsDiff(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAbsDiff(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Performs addition between two images. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAdd(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE],
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAdd(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Performs subtraction between two images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgSubtract(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgSubtract(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0, 
		VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>(input1, input2, output);
}

/** @brief Implements the Gradient Magnitude Computation Kernel. 
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param ROUND_POLICY    The round policy <tt>\ref vx_round_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_round_policy_e ROUND_POLICY>
void ImgMagnitude(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAGNITUDE, VX_CONVERT_POLICY_SATURATE, ROUND_POLICY, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_round_policy_e ROUND_POLICY>
void ImgMagnitude(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAGNITUDE, VX_CONVERT_POLICY_SATURATE, ROUND_POLICY, 0, VX_THRESHOLD_TYPE_BINARY, 0, 
		0, 0, 0, 1>(input1, input2, output);
}

/** @brief Performs element-wise multiplication between two images and a scalar value. 
@param DataType        Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param ROUND_POLICY    The round policy <tt>\ref vx_round_policy_e</tt>
@param SCALE           A number multiplied to each product before overflow handling.
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
void ImgMultiply(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE, VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>
		(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
void ImgMultiply(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE, VX_THRESHOLD_TYPE_BINARY, 0, 0, 0, 0, 1>
		(input1, input2, output);
}

/** @brief  Weighted average value from two input images to an output image.
@param DataType       Data type of the image pixels
@param VEC_SIZE       Amount of pixels computed in parallel
@param IMG_PIXEL      Amount of pixels in the image
@param ROUND_POLICY   The round policy <tt>\ref vx_round_policy_e</tt>
@param ALPHA          The weighted value in fixed-point between 0 and 1 using 16-bit fraction
@param input1         Input image
@param input2         Input image
@param output         Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_uint32 ALPHA, vx_round_policy_e ROUND_POLICY>
void ImgWeightedAverage(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::WEIGHTED_AVERAGE, VX_CONVERT_POLICY_WRAP, ROUND_POLICY, 0, VX_THRESHOLD_TYPE_BINARY, 
		0, 0, 0, ALPHA, 1>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_uint32 ALPHA, vx_round_policy_e ROUND_POLICY>
void ImgWeightedAverage(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::WEIGHTED_AVERAGE, VX_CONVERT_POLICY_WRAP, ROUND_POLICY, 0, VX_THRESHOLD_TYPE_BINARY, 
		0, 0, 0, ALPHA, 1>(input1, input2, output);
}

/** @brief  Thresholds an input image and produces an output Boolean image.
@param DataType       Data type of the image pixels
@param VEC_SIZE       Amount of pixels computed in parallel
@param IMG_PIXEL      Amount of pixels in the image
@param THRESH_TYPE    The type of thresholding (VX_THRESHOLD_TYPE_BINARY, VX_THRESHOLD_TYPE_RANGE)
@param THRESH_VALUE   The threshold value if threshold type is VX_THRESHOLD_TYPE_BINARY
@param THRESH_UPPER   The upper threshold value if threshold type is VX_THRESHOLD_TYPE_RANGE
@param THRESH_LOWER   The lower threshold value if threshold type is VX_THRESHOLD_TYPE_RANGE
@param input1         Input image
@param input2         Input image
@param output         Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_threshold_type_e THRESH_TYPE, DataType THRESH_VALUE, DataType THRESH_UPPER, 
	DataType THRESH_LOWER>
void ImgThreshold(
	vx_image_data<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::THRESHOLD, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, THRESH_TYPE, 
		THRESH_VALUE, THRESH_UPPER, THRESH_LOWER, 0, 1>(input1, input1, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_threshold_type_e THRESH_TYPE, DataType THRESH_VALUE, DataType THRESH_UPPER, 
	DataType THRESH_LOWER>
void ImgThreshold(DataType input1[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::THRESHOLD, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0, THRESH_TYPE, 
		THRESH_VALUE, THRESH_UPPER, THRESH_LOWER, 0, 1>(input1, input1, output);
}

/** @brief  Computes the orientation of vector(x,y). East is 0. The values increase counter clock wise depending on the quantization.
@param DataType       Data type of the image pixels
@param VEC_SIZE       Amount of pixels computed in parallel
@param IMG_PIXEL      Amount of pixels in the image
@param QUANTIZATION   The quantisazion of the orientation values. Allowed values Q = [1..8]. 2^(Q) different output values
@param input_x        Input vectors in x direction
@param input_y        Input vectors in y direction
@param output         Output orientations
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_uint8 QUANTIZATION>
void ImgPhase(
	vx_image_data<DataType, VEC_SIZE> input_x[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> input_y[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input_x
#pragma HLS DATA_PACK variable=input_y
#pragma HLS DATA_PACK variable=output
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::PHASE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 1, VX_THRESHOLD_TYPE_BINARY, 
		1, 1, 1, 1, QUANTIZATION>(input_x, input_y, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_uint8 QUANTIZATION>
void ImgPhase(DataType input_x[IMG_PIXEL / VEC_SIZE], DataType input_y[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::PHASE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 1, VX_THRESHOLD_TYPE_BINARY, 
		1, 1, 1, 1, QUANTIZATION>(input_x, input_y, output);
}

#endif /* SRC_IMG_PIXELOP_BASE_H_ */
