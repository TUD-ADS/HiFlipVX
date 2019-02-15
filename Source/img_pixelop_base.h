/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_pixelop_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
* @brief Description:\n
*  These are all pixel operation functions (Call from here)
*/

#ifndef SRC_IMG_PIXELOP_BASE_H_
#define SRC_IMG_PIXELOP_BASE_H_

#include "img_pixelop_core.h"

/*********************************************************************************************************************/
/* Pixelwise Operations (Same Data Type) */
/*********************************************************************************************************************/

/** @brief Copy data from an input data object into another data object. The input and output object must have the same object type and meta data.
@param DataType        Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgCopy(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::COPY_DATA, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input1, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgCopy(DataType input1[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::COPY_DATA, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input1, output);
}

/** @brief Performs a bitwise NOT operation on a input image. The output image dimensions should be the same as the dimensions of the input image.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgNot(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_NOT, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input1, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgNot(DataType input1[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_NOT, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input1, output);
}

/** @brief Performs a bitwise AND operation between two images.The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgAnd(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_AND, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgAnd(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_AND, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Performs a bitwise INCLUSIVE OR operation between two images. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgOr(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_OR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgOr(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_OR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Performs a bitwise EXCLUSIVE OR (XOR) operation between two images. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgXor(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_XOR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgXor(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::BITWISE_XOR, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Implements a pixel-wise maximum kernel. The output image dimensions should be the same as the dimensions of the input image.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMax(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAX, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMax(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAX, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Implements a pixel-wise minimum kernel. The output image dimensions should be the same as the dimensions of the input image.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMin(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MIN, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
void ImgMin(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MIN, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Computes the absolute difference between two images. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAbsDiff(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAbsDiff(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Performs addition between two images. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAdd(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgAdd(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}


/** @brief Performs subtraction between two images.The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param CONV_POLICY     The round conversion <tt>\ref vx_convert_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgSubtract(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY>
void ImgSubtract(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, VX_ROUND_POLICY_TO_ZERO, 0>(input1, input2, output);
}

/** @brief Implements the Gradient Magnitude Computation Kernel. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@param ROUND_POLICY    The round policy <tt>\ref vx_round_policy_e</tt>
@param input1          Input image
@param input2          Input image
@param output          Output image
*/
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_round_policy_e ROUND_POLICY>
void ImgMagnitude(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAGNITUDE, VX_CONVERT_POLICY_SATURATE, ROUND_POLICY, 0>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_round_policy_e ROUND_POLICY>
void ImgMagnitude(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MAGNITUDE, VX_CONVERT_POLICY_SATURATE, ROUND_POLICY, 0>(input1, input2, output);
}

/** @brief Performs element-wise multiplication between two images and a scalar value. The output image dimensions should be the same as the dimensions of the input images.
@param DataType      Data type of the image pixels
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
void ImgMultiply(vx_image<DataType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], vx_image<DataType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE>(input1, input2, output);
}
template<typename DataType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
void ImgMultiply(DataType input1[IMG_PIXEL / VEC_SIZE], DataType input2[IMG_PIXEL / VEC_SIZE], DataType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE
	PixelwiseSameType<DataType, VEC_SIZE, IMG_PIXEL, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE>(input1, input2, output);
}

#endif /* SRC_IMG_PIXELOP_BASE_H_ */
