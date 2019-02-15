/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_other_core.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
* @brief Description:\n
*  These are all core functions needed for the remaining image processing functions (Do not call functions from here)
*/

#ifndef SRC_IMG_OTHER_CORE_H_
#define SRC_IMG_OTHER_CORE_H_

#include "img_helper.h"

/*********************************************************************************************************************/
/* Bit Depth Conversion Operations */
/*********************************************************************************************************************/

/** @brief Converts image bit depth.
* @param SrcType      The datatype of the input image (8, 16, 32 bit unsigned/signed)
* @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
* @param CONV_POLICY  The conversion policy for overflow (VX_CONVERT_POLICY_WRAP or VX_CONVERT_POLICY_SATURATE)
* @param SHIFT        Bits are shifted by this amount (e.g. up for u8->s16, down for s16->u8),  maximum shift is abs(inBitDepth - outBitDepth)
* @param src          The input pixel
* @param dst          The output pixel
*/
template<typename SrcType, typename DstType, const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertDepthSingle(SrcType src, DstType &dst) {
#pragma HLS INLINE

	// Result
	DstType result = 0;

	// Constants
	const bool IN_IS_SIGNED = numeric_limits<SrcType>::is_signed;
	const bool OUT_IS_SIGNED = numeric_limits<DstType>::is_signed;
	const vx_int16 IN_SIZE = sizeof(SrcType);
	const vx_int16 OUT_SIZE = sizeof(DstType);

	// Check for max shift value
	const vx_uint16 MAX_SHIFT = (IN_SIZE > OUT_SIZE) ? static_cast<vx_uint16>(IN_SIZE - OUT_SIZE) : static_cast<vx_uint16>(OUT_SIZE - IN_SIZE);
	STATIC_ASSERT(MAX_SHIFT*8 >= SHIFT, shift_size_out_of_range);

	// Check if down or up conversion
	bool DOWN_CONVERSION = false;
	bool UP_CONVERSION = false;

	// Border values for VX_CONVERT_POLICY_SATURATE
	vx_int64 MAX_VAL = 0;
	vx_int64 MIN_VAL = 0;

	// Set if down or up conversion
	if ((IN_SIZE == 1 && OUT_SIZE == 2) || 
		(IN_SIZE == 1 && OUT_SIZE == 4) || 
		(IN_SIZE == 2 && OUT_SIZE == 4)) {
		UP_CONVERSION = true;
	} else if (
		(IN_SIZE == 2 && OUT_SIZE == 1) || 
		(IN_SIZE == 4 && OUT_SIZE == 1) || 
		(IN_SIZE == 4 && OUT_SIZE == 2)) {
		DOWN_CONVERSION = true;
	} 

	// Set border values for VX_CONVERT_POLICY_SATURATE
	if (OUT_IS_SIGNED == true) {
		if (OUT_SIZE == 1) {
			MAX_VAL = VX_INT8_MAX;
			MIN_VAL = VX_INT8_MIN;
		} else if (OUT_SIZE == 2) {
			MAX_VAL = VX_INT16_MAX;
			MIN_VAL = VX_INT16_MIN;
		} else if (OUT_SIZE == 4) {
			MAX_VAL = VX_INT32_MAX;
			MIN_VAL = VX_INT32_MIN;
		}
	} else if (OUT_IS_SIGNED == false) {
		if (OUT_SIZE == 1)
			MAX_VAL = VX_UINT8_MAX;
		else if (OUT_SIZE == 2)
			MAX_VAL = VX_UINT16_MAX;
		else if (OUT_SIZE == 4)
			MAX_VAL = VX_UINT32_MAX;
	}

	// Do up or down conversion
	if (DOWN_CONVERSION == true) {
		if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
			result = (static_cast<DstType>(src >> SHIFT));
		} else if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
			vx_int64 value = static_cast<vx_int64>(src >> SHIFT);
			value = max(min(value, MAX_VAL), MIN_VAL);
			result = static_cast<DstType>(value);
		}
	}
	if (UP_CONVERSION == true) {
		if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
			result = (static_cast<DstType>(src)) << SHIFT;
		} else if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
			if (OUT_IS_SIGNED == true) {
				vx_int64 value = (static_cast<vx_int64>(src)) << SHIFT;
				value = static_cast<DstType>(max(min(value, MAX_VAL), MIN_VAL));
				result = static_cast<DstType>(value);
			} else {
				result = (static_cast<DstType>(src)) << SHIFT;
			}			
		}
	}

	// Convert from signed to unsigned or v.v.
	if ((IN_SIZE == OUT_SIZE) && (IN_IS_SIGNED != OUT_IS_SIGNED)) {
		if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
			result = static_cast<DstType>(src);
		} else if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
			vx_int64 value = static_cast<vx_int64>(src);
			value = max(min(value, MAX_VAL), MIN_VAL);
			result = static_cast<DstType>(value);
		}
	}	

	// Return result
	dst = result;
}

/** @brief Checks the data type to converts image bit depth.
* @param SrcType       The datatype of the input image (8, 16, 32 bit unsigned/signed)
* @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
* @param VEC_SIZE     The vector size
* @param IMG_PIXEL    The amount of image pixels
*/
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL>
vx_uint32 ConvertBitDepthCheck() {

	// Constants
	const vx_type_e IN_TYPE = GET_TYPE(SrcType);
	const vx_type_e OUT_TYPE = GET_TYPE(DstType);
	const bool INPUT_TYPE_CORRECT =
		(IN_TYPE == VX_TYPE_UINT8) || (IN_TYPE == VX_TYPE_UINT16) || (IN_TYPE == VX_TYPE_UINT32) ||
		(IN_TYPE == VX_TYPE_INT8) || (IN_TYPE == VX_TYPE_INT16) || (IN_TYPE == VX_TYPE_INT32);
	const bool OUTPUT_TYPE_CORRECT =
		(OUT_TYPE == VX_TYPE_UINT8) || (OUT_TYPE == VX_TYPE_UINT16) || (OUT_TYPE == VX_TYPE_UINT32) ||
		(OUT_TYPE == VX_TYPE_INT8) || (OUT_TYPE == VX_TYPE_INT16) || (OUT_TYPE == VX_TYPE_INT32);

	// Check function parameters/types
	STATIC_ASSERT(INPUT_TYPE_CORRECT, input_type_error);
	STATIC_ASSERT(OUTPUT_TYPE_CORRECT, output_type_or_output_type_error);
	STATIC_ASSERT((IMG_PIXEL % VEC_SIZE) == 0, image_pixels_are_not_multiple_of_vector_size);
	STATIC_ASSERT((VEC_SIZE == 1) || (VEC_SIZE == 2) || (VEC_SIZE == 4) || (VEC_SIZE == 8), _vec_size_musst_be_multiple_of_two_and_max_8);

	return IMG_PIXEL / static_cast<vx_uint32>(VEC_SIZE);;
}

/** @brief Converts image bit depth. 
* @param SrcType       The datatype of the input image (8, 16, 32 bit unsigned/signed)
* @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
* @param VEC_SIZE     The vector size
* @param IMG_PIXEL    The amount of image pixels
* @param CONV_POLICY  The conversion policy for overflow (VX_CONVERT_POLICY_WRAP or VX_CONVERT_POLICY_SATURATE)
* @param SHIFT        Bits are shifted by this amount (e.g. up for u8->s16, down for s16->u8),  maximum shift is abs(inBitDepth - outBitDepth)
* @param input        The input image
* @param output       The output image
*/
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL, const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertBitDepth(vx_image<SrcType, VEC_SIZE> input[IMG_PIXEL], vx_image<DstType, VEC_SIZE> output[IMG_PIXEL]) {
#pragma HLS INLINE

	// Constants
	const vx_uint32 VEC_PIXEL = ConvertBitDepthCheck<SrcType, DstType, VEC_SIZE, IMG_PIXEL>();

	// Vector variables
	vx_image<SrcType, VEC_SIZE> src;
	vx_image<DstType, VEC_SIZE> dst = { 0 };

	// Computes operations (pipelined)
	for (vx_uint32 i = 0; i < VEC_PIXEL; i++) {
#pragma HLS PIPELINE II=1

		// Read Input
		src = input[i];

		// Compute vector of bit depth operation
		for (vx_uint16 j = 0; j < VEC_SIZE; j++) {
#pragma HLS unroll
			ConvertDepthSingle<SrcType, DstType, CONV_POLICY, SHIFT>(src.pixel[j], dst.pixel[j]);
		}

		// Write result
		output[i] = dst;
	}
}
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL, const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertBitDepth(SrcType input[IMG_PIXEL], DstType output[IMG_PIXEL]) {
#pragma HLS INLINE

	// Constants
	const vx_uint32 VEC_PIXEL = ConvertBitDepthCheck<SrcType, DstType, VEC_SIZE, IMG_PIXEL>();

	// Vector variables
	SrcType src = 0;
	DstType dst = 0;

	// Computes operations (pipelined)
	for (vx_uint32 i = 0; i < VEC_PIXEL; i++) {
#pragma HLS PIPELINE II=1

		// Read Input
		src = input[i];

		// Compute bit depth operation
		ConvertDepthSingle<SrcType, DstType, CONV_POLICY, SHIFT>(src, dst);

		// Write result
		output[i] = dst;
	}
}

/*********************************************************************************************************************/
/* Color Conversion Operations */
/*********************************************************************************************************************/

/** @brief Read from Gray Input
* @param SrcType      The datatype of the input image
* @param IMG_PIXELS  The pixel amount of the image
* @param ptr         The pointer to the image 
* @param input       The input image
* @param input_data  The input pixel
* @param output_data The output pixels
*/
template<typename SrcType, vx_uint32 IMG_PIXELS>
void InputGray(vx_uint32 ptr, SrcType input[IMG_PIXELS], vx_uint8 &input_data, vx_uint8 output_data[3]) {

	input_data = static_cast<vx_uint8>(input[ptr]);
	output_data[0] = input_data; // R
	output_data[1] = input_data; // G
	output_data[2] = input_data; // B
}

/** @brief Read from RGB Input
* @param SrcType      The datatype of the input image
* @param IMG_PIXELS  The pixel amount of the image
* @param cases       Different cases how the 3 rgb pixels are distributed among the 4 input pixels
* @param ptr         The pointer to the image pixel
* @param input       The input image
* @param input_data  The input pixel
* @param output_data The output pixels
*/
template<typename SrcType, vx_uint32 IMG_PIXELS>
void InputRgb(vx_uint8 cases, vx_uint32 &ptr, SrcType input[IMG_PIXELS], vx_uint32 &input_data, vx_uint8 output_data[4]) {
#pragma HLS INLINE	

	if (cases < 3) {
		input_data = static_cast<vx_uint32>(input[ptr]);
		ptr++;
	}
	output_data[0] = static_cast<vx_uint8>((input_data >> 0) & 0xFF);
	output_data[1] = static_cast<vx_uint8>((input_data >> 8) & 0xFF);
	output_data[2] = static_cast<vx_uint8>((input_data >> 16) & 0xFF);
	output_data[3] = static_cast<vx_uint8>((input_data >> 24) & 0xFF);
}

/** @brief Read from RGBX Input
* @param SrcType      The datatype of the input image
* @param IMG_PIXELS  The pixel amount of the image
* @param ptr         The pointer to the image pixel
* @param input       The input image
* @param input_data  The input pixel
* @param output_data The output pixels
*/
template<typename SrcType, vx_uint32 IMG_PIXELS>
void InputRgbx(vx_uint32 ptr, SrcType input[IMG_PIXELS], vx_uint32 &input_data, vx_uint8 output_data[3]) {
#pragma HLS INLINE	

	input_data = static_cast<vx_uint32>(input[ptr]);
	output_data[0] = static_cast<vx_uint8>((input_data >> 0) & 0xFF);
	output_data[1] = static_cast<vx_uint8>((input_data >> 8) & 0xFF);
	output_data[2] = static_cast<vx_uint8>((input_data >> 16) & 0xFF);
}

/** @brief Write to Gray output (BT.601)
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param ptr         The pointer to the image pixel
* @param output_data The input pixels (after conversion)
* @param output      The output image
*/
template<typename DstType, vx_uint32 IMG_PIXELS>
void OutputGray(vx_uint32 &ptr, vx_uint8 output_data[3], DstType output[IMG_PIXELS]) {

	vx_uint32 gray =
		306 * static_cast<vx_uint32>(output_data[0]) + // (int)((0.299 * 1024) + 0.5)
		601 * static_cast<vx_uint32>(output_data[1]) + // (int)((0.587 * 1024) + 0.5)
		117 * static_cast<vx_uint32>(output_data[2]);  // (int)((0.114 * 1024) + 0.5)
	output[ptr] = static_cast<DstType>((gray + 512) >> 10); // round to nearest
}

/** @brief Write to RGB output
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param cases       Different cases how the 3 rgb pixels are distributed among the 4 input pixels
* @param ptr         The pointer to the image pixel
* @param output_data The input pixels (after conversion)
* @param output      The output image
*/
template<typename DstType, vx_uint32 IMG_PIXELS>
void OutputRgb(vx_uint8 cases, vx_uint32 &ptr, vx_uint8 output_data[4], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE		

	if (cases != 1) {
		output[ptr] =
			static_cast<DstType>(output_data[0] << 0) |
			static_cast<DstType>(output_data[1] << 8) |
			static_cast<DstType>(output_data[2] << 16) |
			static_cast<DstType>(output_data[3] << 24);
		ptr++;
	}
}

/** @brief Write to RGBX output
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param ptr         The pointer to the image pixel
* @param output_data The input pixels (after conversion)
* @param output      The output image
*/
template<typename DstType, vx_uint32 IMG_PIXELS>
void OutputRgbx(vx_uint32 &ptr, vx_uint8 output_data[3], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE	

	output[ptr] =
		(static_cast<DstType>(output_data[0]) << 0) |
		(static_cast<DstType>(output_data[1]) << 8) |
		(static_cast<DstType>(output_data[2]) << 16);
}

/** @brief Converts to RGB. 
* @param cases         Different cases how the 3 rgb pixels are distributed among the 4 input pixels
* @param internal_data The input pixels
* @param buffer_data   Internal buffers
* @param output_data   The output pixels
*/
void ConvertToRgb(vx_uint8 &cases, vx_uint8 internal_data[3], vx_uint8 buffer_data[2], vx_uint8 output_data[4]) {
#pragma HLS INLINE

	switch (cases) {
	case 0:
		output_data[0] = internal_data[0];
		output_data[1] = internal_data[1];
		output_data[2] = internal_data[2];
		cases = 1;
		break;
	case 1:
		output_data[3] = internal_data[0];
		buffer_data[0] = internal_data[1];
		buffer_data[1] = internal_data[2];
		cases = 2;
		break;
	case 2:
		output_data[0] = buffer_data[0];
		output_data[1] = buffer_data[1];
		output_data[2] = internal_data[0];
		output_data[3] = internal_data[1];
		buffer_data[0] = internal_data[2];
		cases = 3;
		break;
	default:
		output_data[0] = buffer_data[0];
		output_data[1] = internal_data[0];
		output_data[2] = internal_data[1];
		output_data[3] = internal_data[2];
		cases = 0;
		break;
	}
}

/** @brief Converts from RGB. 
* @param cases         Different cases how the 3 rgb pixels are distributed among the 4 input pixels
* @param internal_data The input pixels
* @param buffer_data   Internal buffers
* @param output_data   The output pixels
*/
void ConvertFromRgb(vx_uint8 &cases, vx_uint8 internal_data[4], vx_uint8 buffer_data[3], vx_uint8 output_data[3]) {
#pragma HLS INLINE

	switch (cases) {
	case 0:
		output_data[0] = internal_data[0];
		output_data[1] = internal_data[1];
		output_data[2] = internal_data[2];
		buffer_data[0] = internal_data[3];
		cases = 1;
		break;
	case 1:
		output_data[0] = buffer_data[0];
		output_data[1] = internal_data[0];
		output_data[2] = internal_data[1];
		buffer_data[0] = internal_data[2];
		buffer_data[1] = internal_data[3];
		cases = 2;
		break;
	case 2:
		output_data[0] = buffer_data[0];
		output_data[1] = buffer_data[1];
		output_data[2] = internal_data[0];
		buffer_data[0] = internal_data[1];
		buffer_data[1] = internal_data[2];
		buffer_data[2] = internal_data[3];
		cases = 3;
		break;
	default:
		output_data[0] = buffer_data[0];
		output_data[1] = buffer_data[1];
		output_data[2] = buffer_data[2];
		cases = 0;
		break;
	}
}

/** @brief Convert from Grayscale to RGB 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertGrayToRgb(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint8 cases = 0;
	vx_uint8 input_data;
	vx_uint8 internal_data[3];
	vx_uint8 buffer_data[2];
	vx_uint8 output_data[4];

	// Convert from Grayscale to RGB 
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input
		InputGray<SrcType, IMG_PIXELS>(i, input, input_data, internal_data);

		// Conversion
		ConvertToRgb(cases, internal_data, buffer_data, output_data);

		// Write to output
		OutputRgb<DstType, IMG_PIXELS>(cases, j, output_data, output);
	}
}

/** @brief Convert from Grayscale to RGBX 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertGrayToRgbx(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint8 input_data;
	vx_uint8 output_data[3];

	// Convert grayscale to RGBX
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input
		InputGray<SrcType, IMG_PIXELS>(i, input, input_data, output_data);

		// Write to output
		OutputRgbx<DstType, IMG_PIXELS>(i, output_data, output);
	}
}

/** @brief Convert from RGB to Grayscale 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertRgbToGray(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint8 cases = 0;
	vx_uint32 input_data;
	vx_uint8 internal_data[4];
	vx_uint8 buffer_data[3];
	vx_uint8 output_data[3];

	// Compute gray-scale on image
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1		

		// Read from input and store it into internal data
		InputRgb<SrcType, IMG_PIXELS>(cases, j, input, input_data, internal_data);

		// Conversion
		ConvertFromRgb(cases, internal_data, buffer_data, output_data);

		// Write to output
		OutputGray<DstType, IMG_PIXELS>(i, output_data, output);
	}
}

/** @brief Convert from RGB to RGBX 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertRgbToRgbx(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint8 cases = 0;
	vx_uint32 input_data;
	vx_uint8 internal_data[4];
	vx_uint8 buffer_data[3];
	vx_uint8 output_data[3];

	// Compute gray-scale on image
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input and store it into internal data
		InputRgb<SrcType, IMG_PIXELS>(cases, j, input, input_data, internal_data);

		// Conversion
		ConvertFromRgb(cases, internal_data, buffer_data, output_data);

		// Write to output
		OutputRgbx<DstType, IMG_PIXELS>(i, output_data, output);
	}
}

/** @brief Convert from RGBX to Grayscale 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertRgbxToGray(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint32 input_data;
	vx_uint8 output_data[3];

	// Compute gray-scale on image
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input
		InputRgbx<SrcType, IMG_PIXELS>(i, input, input_data, output_data);

		// Write to output
		OutputGray<DstType, IMG_PIXELS>(i, output_data, output);
	}
}

/** @brief Convert from RGBX to RGB 
* @param SrcType      The datatype of the input imag
* @param DstType     The datatype of the output image
* @param IMG_PIXELS  The pixel amount of the image
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, vx_uint32 IMG_PIXELS>
void ConvertRgbxToRgb(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Global variables
	vx_uint8 cases = 0;
	vx_uint32 input_data;
	vx_uint8 internal_data[3];
	vx_uint8 buffer_data[2];
	vx_uint8 output_data[4];

	// Compute gray-scale on image
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input
		InputRgbx<SrcType, IMG_PIXELS>(i, input, input_data, internal_data);

		// Conversion
		ConvertToRgb(cases, internal_data, buffer_data, output_data);

		// Write to output
		OutputRgb<DstType, IMG_PIXELS>(cases, j, output_data, output);
	}
}

/** @brief Converts the Color of an image between VX_DF_IMAGE_RGB / VX_DF_IMAGE_RGBX / VX_DF_IMAGE_U8 (grayscale) 
* @param SrcType      The datatype of the input image  (uint8 (Gray), uint32 (RGB, RGBX))
* @param DstType     The datatype of the output image (uint8 (Gray), uint32 (RGB, RGBX))
* @param IMG_PIXELS  The pixel amount of the image
* @param INPUT_TYPE  The color type of the input image (RGB, RGBX, Gray)
* @param OUTPUT_TYPE The color type of the output image (RGB, RGBX, Gray)
* @param input       The input image
* @param output      The output image
*/
template <typename SrcType, typename DstType, const vx_uint32 IMG_PIXELS, vx_df_image_e INPUT_TYPE, vx_df_image_e OUTPUT_TYPE>
void ConvertColor(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Constants
	const vx_uint32 INPUT_SIZE = sizeof(SrcType);
	const vx_uint32 OUTPUT_SIZE = sizeof(DstType);
	const bool IN_IS_SIGNED = numeric_limits<SrcType>::is_signed;
	const bool OUT_IS_SIGNED = numeric_limits<DstType>::is_signed;

	// Allowed color conversions
	const bool GRAY_to_RGB  = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);
	const bool GRAY_to_RGBX = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool RGB_to_GRAY  = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
	const bool RGB_to_RGBX  = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool RGBX_to_GRAY = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
	const bool RGBX_to_RGB  = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);

	// Check types (static assertions)
	const bool allowed_types1 =
		(IN_IS_SIGNED == false) || (OUT_IS_SIGNED == false);
	const bool allowed_types2 =
		(INPUT_SIZE == 1 && INPUT_TYPE == VX_DF_IMAGE_U8) ||
		(INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool allowed_types3 =
		(OUTPUT_SIZE == 1 && OUTPUT_TYPE == VX_DF_IMAGE_U8) ||
		(OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool allowed_types4 =
		GRAY_to_RGB || GRAY_to_RGBX || RGB_to_GRAY || RGB_to_RGBX || RGBX_to_GRAY || RGBX_to_RGB;
	STATIC_ASSERT(allowed_types1, datatype_of_input_and_output_must_be_unsigned);
	STATIC_ASSERT(allowed_types2, missmatch_input_datatype_size_and_input_type);
	STATIC_ASSERT(allowed_types3, missmatch_output_datatype_size_and_output_type);
	STATIC_ASSERT(allowed_types4, color_conversion_type_not_supported);

	// Color conversions
	if (GRAY_to_RGB)
		ConvertGrayToRgb<SrcType, DstType, IMG_PIXELS>(input, output);
	else if (GRAY_to_RGBX)
		ConvertGrayToRgbx<SrcType, DstType, IMG_PIXELS>(input, output);
	else if (RGB_to_GRAY)
		ConvertRgbToGray<SrcType, DstType, IMG_PIXELS>(input, output);
	else if (RGB_to_RGBX)
		ConvertRgbToRgbx<SrcType, DstType, IMG_PIXELS>(input, output);
	else if (RGBX_to_GRAY)
		ConvertRgbxToGray<SrcType, DstType, IMG_PIXELS>(input, output);
	else if (RGBX_to_RGB)
		ConvertRgbxToRgb<SrcType, DstType, IMG_PIXELS>(input, output);
}


/*********************************************************************************************************************/
/* Channel Extraction/Combine */
/*********************************************************************************************************************/

/** @brief Implements the Channel Extraction Kernel. 
* @param SrcType   The datatype of the input image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
* @param CHANNEL_ID  The id of the channel to extract
* @param IMG_PIXELS  The pixels of the image
* @param INPUT_TYPE  The color type of the input image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX)
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, const vx_uint16 CHANNEL_ID, const vx_uint32 IMG_PIXELS, vx_df_image_e INPUT_TYPE>
void ChannelExtract(SrcType input[IMG_PIXELS], vx_uint8 output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Constants
	const vx_uint32 INPUT_SIZE = sizeof(SrcType);

	// Variables
	vx_uint8 cases = 0;
	SrcType input_data = 0;
	vx_uint8 internal_data[INPUT_SIZE];
	vx_uint8 buffer_data[3];	
	vx_uint8 output_data[INPUT_SIZE];

	// Check Types (Static Assertions)
	const bool allowed_types1 =
		(CHANNEL_ID <= 2 && INPUT_TYPE == VX_DF_IMAGE_U16) ||
		(CHANNEL_ID <= 4 && INPUT_TYPE == VX_DF_IMAGE_U32) ||
		(CHANNEL_ID <= 3 && INPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(CHANNEL_ID <= 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool allowed_types2 =
		(INPUT_SIZE == 2 && INPUT_TYPE == VX_DF_IMAGE_U16) ||
		(INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_U32) ||
		(INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
	STATIC_ASSERT(allowed_types1, missmatch_channel_id_does_not_fit);
	STATIC_ASSERT(allowed_types2, missmatch_bit_width_of_output_type);

	// Computes bitwise operations (pipelined)
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input channel
		if (cases < 3) {
			input_data = input[j];
			j++;
		}

		// Unpack data
		if (INPUT_SIZE > 0)
			internal_data[0] = static_cast<vx_uint8>((input_data >> 0) & 0xFF);
		if (INPUT_SIZE > 1)
			internal_data[1] = static_cast<vx_uint8>((input_data >> 8) & 0xFF);
		if (INPUT_SIZE > 2)
			internal_data[2] = static_cast<vx_uint8>((input_data >> 16) & 0xFF);
		if (INPUT_SIZE > 3)
			internal_data[3] = static_cast<vx_uint8>((input_data >> 24) & 0xFF);

		// Convert input to output channel
		if (INPUT_TYPE == VX_DF_IMAGE_U16) {
			output_data[0] = internal_data[0];
			output_data[1] = internal_data[1];
		} else if (INPUT_TYPE == VX_DF_IMAGE_U32) {
			output_data[0] = internal_data[0];
			output_data[1] = internal_data[1];
			output_data[2] = internal_data[2];
			output_data[3] = internal_data[3];
		} else if (INPUT_TYPE == VX_DF_IMAGE_RGB) {
			ConvertFromRgb(cases, internal_data, buffer_data, output_data);
		} else if (INPUT_TYPE == VX_DF_IMAGE_RGBX) {
			output_data[0] = internal_data[0];
			output_data[1] = internal_data[1];
			output_data[2] = internal_data[2];
			output_data[3] = internal_data[3];
		}

		// Write to output channel
		output[i] = output_data[CHANNEL_ID];
	}
}

/** @brief Implements the Channel Combine Kernel. 
* @param DstType     The datatype of the > image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
* @param CHANNEL_AMOUNT The amount of channels to combine
* @param IMG_PIXELS     The pixels of the image
* @param OUTPUT_TYPE    The color type of the > image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX)
* @param input0         The 1. plane of the input image
* @param input1         The 2. plane of the input image
* @param input2         The 3. plane of the input image (optional)
* @param input3         The 4. plane of the input image (optional)
* @param output         The output image
*/
template<typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_uint32 IMG_PIXELS, vx_df_image_e OUTPUT_TYPE>
void ChannelCombine(vx_uint8 input0[IMG_PIXELS], vx_uint8 input1[IMG_PIXELS], vx_uint8 input2[IMG_PIXELS], vx_uint8 input3[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Constants
	const vx_uint16 OUTPUT_SIZE = sizeof(DstType);

	// Variables
	vx_uint8 cases = 0; 
	vx_uint8 input_data[CHANNEL_AMOUNT];
	vx_uint8 internal_data[CHANNEL_AMOUNT];
	vx_uint8 buffer_data[2];	
	DstType output_data;

	// Check Types (Static Assertions)
	const bool allowed_types1 =
		(CHANNEL_AMOUNT == 2 && OUTPUT_TYPE == VX_DF_IMAGE_U16) ||
		(CHANNEL_AMOUNT == 4 && OUTPUT_TYPE == VX_DF_IMAGE_U32) ||
		(CHANNEL_AMOUNT == 3 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(CHANNEL_AMOUNT == 3 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
	const bool allowed_types2 =
		(OUTPUT_SIZE == 2 && OUTPUT_TYPE == VX_DF_IMAGE_U16) ||
		(OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_U32) ||
		(OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) ||
		(OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
	STATIC_ASSERT(allowed_types1, missmatch_channel_amount_does_not_fit);
	STATIC_ASSERT(allowed_types2, missmatch_bit_width_of_output_type);

	// Computes the channel conversion
	for (vx_uint32 i = 0, j = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1

		// Read from input channel
		if (CHANNEL_AMOUNT > 0)
			input_data[0] = input0[i];
		if (CHANNEL_AMOUNT > 1)
			input_data[1] = input1[i];
		if (CHANNEL_AMOUNT > 2)
			input_data[2] = input2[i];
		if (CHANNEL_AMOUNT > 3)
			input_data[3] = input3[i];

		// Convert input to output channel			
		if (OUTPUT_TYPE == VX_DF_IMAGE_U16) {
			internal_data[0] = input_data[0];
			internal_data[1] = input_data[1];
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_U32) {
			internal_data[0] = input_data[0];
			internal_data[1] = input_data[1];
			internal_data[2] = input_data[2];
			internal_data[3] = input_data[3];
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_RGB) {
			ConvertToRgb(cases, input_data, buffer_data, internal_data);
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_RGBX) {
			internal_data[0] = input_data[0];
			internal_data[1] = input_data[1];
			internal_data[2] = input_data[2];
		}
		 
		// Pack data
		if (OUTPUT_TYPE == VX_DF_IMAGE_U16) {
			output_data =
				static_cast<DstType>(internal_data[0]) |
				static_cast<DstType>(internal_data[1]);
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_U32) {
			output_data =
				static_cast<DstType>(internal_data[0]) |
				static_cast<DstType>(internal_data[1]) |
				static_cast<DstType>(internal_data[2]) |
				static_cast<DstType>(internal_data[3]);
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_RGB) {
			output_data =
				static_cast<DstType>(internal_data[0]) |
				static_cast<DstType>(internal_data[1]) |
				static_cast<DstType>(internal_data[2]) |
				static_cast<DstType>(internal_data[3]);
		} else if (OUTPUT_TYPE == VX_DF_IMAGE_RGBX) {
			output_data =
				static_cast<DstType>(internal_data[0]) |
				static_cast<DstType>(internal_data[1]) |
				static_cast<DstType>(internal_data[2]);
		}

		// Write to output channel
		if (cases != 1) {
			output[j] = output_data;
			j++;
		}		
	}
}


/*********************************************************************************************************************/
/* Scale Functions */
/*********************************************************************************************************************/

/** @brief Resize the image down to nearest neighbor 
* @param COLS_IN     The columns of the input image
* @param ROWS_IN     The rows of the input image
* @param COLS_OUT    The columns of the output image
* @param ROWS_OUT    The rows of the output image
* @param input       The input image
* @param output      The output image
*/
template<vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT>
void ScaleDownNearest(vx_uint8 input[COLS_IN*ROWS_IN], vx_uint8 output[COLS_OUT*ROWS_OUT]) {
#pragma HLS INLINE

	// Compute the scale factor of the resized image in x and y direction
	const vx_uint32 ACCURACY = 14;
	const vx_uint32 COLS_SCALE = (vx_uint32)(((vx_float64)COLS_IN / (vx_float64)COLS_OUT) * pow(2, ACCURACY) + 0.5);
	const vx_uint32 ROWS_SCALE = (vx_uint32)(((vx_float64)ROWS_IN / (vx_float64)ROWS_OUT) * pow(2, ACCURACY) + 0.5);

	// Compute the resized image
	for (vx_uint32 y = 0, y_dst = 0, y_dst_next = 0; y < ROWS_IN; y++) {
		for (vx_uint32 x = 0, x_dst = 0; x < COLS_IN; x++) {
#pragma HLS PIPELINE II=1

			// Get next input pixel in a stream
			vx_uint8 data = input[y*COLS_IN + x];

			// Compute the coordinates of the next needed input pixel
			vx_uint32 x_src = (((((x_dst << 1) + 1) * COLS_SCALE) >> ACCURACY) - 1) >> 1; // (vx_int32)floor((x_dst + 0.5f)*(COLS_IN/COLS_OUT) - 0.5f) using floating point computation
			vx_uint32 y_src = (((((y_dst << 1) + 1) * ROWS_SCALE) >> ACCURACY) - 1) >> 1; // (vx_int32)floor((y_dst + 0.5f)*(ROWS_IN/ROWS_OUT) - 0.5f) using floating point computation

			// Check if current input pixel is the needed one
			if ((y_src == y) && (x_src == x)) {

				// Write the ouput pixel to stream
				output[y_dst*COLS_OUT + x_dst] = data;

				// Update pointer
				x_dst++;
				y_dst_next = 1;
			}

			// Update pointer
			if ((y_dst_next == 1) && (x == COLS_IN - 1)) {
				y_dst++;
				y_dst_next = 0;
			}
		}
	}
}

/** @brief Resize the image down using bilinear interpolation 
* @param COLS_IN     The columns of the input image
* @param ROWS_IN     The rows of the input image
* @param COLS_OUT    The columns of the output image
* @param ROWS_OUT    The rows of the output image
* @param input       The input image
* @param output      The output image
*/
template<vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT>
void ScaleDownBilinear(vx_uint8 input[COLS_IN*ROWS_IN], vx_uint8 output[COLS_OUT*ROWS_OUT]) {
#pragma HLS INLINE

	// Compute constants
	const vx_uint32 ACCURACY = 12; // Minimum 12, Maximum 15
	const vx_uint32 ONE = (1 << ACCURACY);
	const vx_uint32 SHIFT = 2*ACCURACY - 10-8;
	const vx_uint32 MASK_FRAKTION = (1 << (ACCURACY + 2)) - 1;	
	const vx_uint32 COLS_SCALE = (vx_uint32)(((vx_float64)COLS_IN / (vx_float64)COLS_OUT) * pow(2, ACCURACY) + 0.5);
	const vx_uint32 ROWS_SCALE = (vx_uint32)(((vx_float64)ROWS_IN / (vx_float64)ROWS_OUT) * pow(2, ACCURACY) + 0.5);

	// Strt and step size of src ptr with "ACCURACY" fraction size 
	// x_src = (x + 0.5) * scale_cols - 0.5 | fraction = 1+1+ACCURACY | not negative: SCALE > 1.0f
	const vx_uint32 X_SRC_STRT = (COLS_SCALE << 1) - 1;
	const vx_uint32 Y_SRC_STRT = (ROWS_SCALE << 1) - 1;
	const vx_uint32 X_SRC_STEP = COLS_SCALE * 4;
	const vx_uint32 Y_SRC_STEP = ROWS_SCALE * 4;

	// Buffer the input pixel
	vx_uint8 data_new = 0;
	vx_uint8 data_left = 0, data_right = 0;
	vx_uint8 buffer_one[COLS_IN];
	vx_uint8 buffer_two[COLS_IN];	

	vx_uint32 in_cnt = 0, out_cnt = 0;

	// Compute the resized image
	for (vx_uint32 y = 0, y_dst = 0, y_src = Y_SRC_STRT, y_dst_next = 0; y <= ROWS_IN; y++) {
		for (vx_uint32 x = 0, x_dst = 0, x_src = X_SRC_STRT; x <= COLS_IN; x++) {
#pragma HLS PIPELINE II=1

			vx_uint8 tl, tr, bl, br;

			// Buffer the previous input pixel [x-1]
			vx_uint8 data_old = data_new;

			// Read Input Pixel from Stream
			if (x < COLS_IN && y < ROWS_IN) {
				in_cnt++;
				data_new = input[y*COLS_IN + x];
			}

			// Compute the coordinates of all needed input pixels			
			vx_uint32 x_fract = (x_src & MASK_FRAKTION) >> 2;
			vx_uint32 y_fract = (y_src & MASK_FRAKTION) >> 2;			
			vx_uint16 x_l = (vx_uint16)(x_src >> (ACCURACY + 2));
			vx_uint16 y_t = (vx_uint16)(y_src >> (ACCURACY + 2));
			vx_uint16 x_r = x_l + 1;
			vx_uint16 y_b = y_t + 1;

	

			// Write/Read data to/from buffer
			if ((y % 2) == 0) {
				if (x_l < COLS_IN)
					data_left = buffer_two[x_l];
				if (x_r < COLS_IN)
					data_right = buffer_two[x_r];
				if (x > 0)
					buffer_one[x - 1] = data_old;
			} else {
				if (x_l < COLS_IN)
					data_left = buffer_one[x_l];
				if (x_r < COLS_IN)
					data_right = buffer_one[x_r];
				if (x > 0)
					buffer_two[x - 1] = data_old;
			}

			// Get the 4 needed input pixel (using replicated borders)
			if (y_t == COLS_IN - 1) {
				if (x_l == ROWS_IN - 1) {
					tl = tr = bl = br = data_left;
				} else {
					tr = br = data_right;
					tl = bl = data_left;
				}
			} else {
				if (x_l == ROWS_IN - 1) {
					bl = br = data_old;
					tl = tr = data_left;
				} else {
					tl = data_right;
					tr = data_left;
					bl = data_old;
					br = data_new;
				}
			}

			// Compute the Bilinear Interpolation (Saturated)
			vx_uint16 tl_part = (vx_uint16)(((((ONE - x_fract) * (ONE - y_fract)) >> 10) * (vx_uint32)tl) >> 8);
			vx_uint16 tr_part = (vx_uint16)(((((      x_fract) * (ONE - y_fract)) >> 10) * (vx_uint32)tr) >> 8);
			vx_uint16 bl_part = (vx_uint16)(((((ONE - x_fract) * (      y_fract)) >> 10) * (vx_uint32)bl) >> 8);
			vx_uint16 br_part = (vx_uint16)(((((      x_fract) * (      y_fract)) >> 10) * (vx_uint32)br) >> 8);
			vx_uint16 sum = (tl_part + tr_part + bl_part + br_part) >> (2 * ACCURACY - 10 - 8);
			vx_uint8 result = (vx_uint8)(min(sum, (vx_uint16)(255)));

			// Check if the input pixel are the needed ones
			if ((x_r == x) && (y_b == y)) {
				out_cnt++;

				// Write ouput pixel to stream
				output[y_dst*COLS_OUT + x_dst] = result;

				// Update pointer
				x_dst++;
				x_src += X_SRC_STEP;
				y_dst_next = 1;
			}

			// Update pointer
			if ((y_dst_next == 1) && (x_dst == COLS_OUT)) {
				y_dst++;
				y_src += Y_SRC_STEP;
				y_dst_next = 0;				
			}
		}		
	}
}

/** @brief Scale an image down using bilinear or nearest neighbor interpolation 
* @param COLS_IN     The columns of the input image
* @param ROWS_IN     The rows of the input image
* @param COLS_OUT    The columns of the output image
* @param ROWS_OUT    The rows of the output image
* @param SCALE_TYPE  The type of interpolation (VX_INTERPOLATION_BILINEAR or VX_INTERPOLATION_AREA)
* @param input       The input image
* @param output      The output image
*/
template<vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT, vx_interpolation_type_e SCALE_TYPE>
void ScaleImage(vx_uint8 input[COLS_IN*ROWS_IN], vx_uint8 output[COLS_OUT*ROWS_OUT]) {
#pragma HLS INLINE

	// Check if output image resolution is smaller than input image resolution
	STATIC_ASSERT(COLS_IN >= COLS_OUT, only_down_scale_of_image_supported);
	STATIC_ASSERT(ROWS_IN >= ROWS_OUT, only_down_scale_of_image_supported);
	STATIC_ASSERT(SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR || SCALE_TYPE == VX_INTERPOLATION_BILINEAR, scale_type_not_supported);

	// Nearest neighbor interpolation
	if (SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR) {
		ScaleDownNearest<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT>(input, output);

	// Bilinear Interpolation
	} else if (SCALE_TYPE == VX_INTERPOLATION_BILINEAR) {
		ScaleDownBilinear<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT>(input, output);

	// Interpolation not supported
	} else if (SCALE_TYPE == VX_INTERPOLATION_AREA) {

	}
}

/*********************************************************************************************************************/
/* Integral Functions */
/*********************************************************************************************************************/

/** @brief Computes the integral image of the input. 
* @param IMG_COLS    The columns of the image
* @param IMG_ROWS    The rows of the image
* @param input       The input image
* @param output      The output image
*/
template<const vx_uint16 IMG_COLS, const vx_uint16 IMG_ROWS>
void Integral(vx_uint8 input[IMG_COLS*IMG_ROWS], vx_uint32 output[IMG_COLS*IMG_ROWS]) {
#pragma HLS INLINE

	// Buffers 1 row of the integral image
	vx_uint32 buffer[IMG_COLS];

	// Compute integral on image
	for (vx_uint32 y = 0; y < IMG_ROWS; y++) {
		for (vx_uint32 x = 0, sum_row = 0; x < IMG_COLS; x++) {
#pragma HLS PIPELINE II=1

			// Read input
			vx_uint8 pixel = input[y*IMG_COLS + x];

			// Compute the sum of the current row
			sum_row += pixel;

			// Compute the integral pixel
			vx_uint32 sum_area = sum_row;
			if (y > 0)
				sum_area += buffer[x];

			// Buffer the integral pixels of row for next row
			buffer[x] = sum_area;

			// Write output
			output[y*IMG_COLS + x] = sum_area;
		}
	}
}

/*********************************************************************************************************************/
/* Historgam and LUT Operations */
/*********************************************************************************************************************/

/** @brief Generates a distribution from an image.  
* @param SrcType              The Input Type can be vx_uint8 and vx_uint16
* @param IMG_PIXELS           The amount of pixels for input and output image
* @param DISTRIBUTION_BINS    Indicates the number of bins.
* @param DISTRIBUTION_RANG    Indicates the total number of the consecutive values of the distribution interval.
* @param DISTRIBUTION_OFFSET  Indicates the start of the values to use (inclusive).
* @param input                The input image (vx_uint8, vx_uint16)
* @param output               The output image (vx_uint32)
*/
template<typename SrcType, const vx_uint32 IMG_PIXELS, const vx_uint32 DISTRIBUTION_BINS, const vx_uint32 DISTRIBUTION_RANGE, const SrcType DISTRIBUTION_OFFSET>
void Histogram(SrcType input[IMG_PIXELS], vx_uint32 output[IMG_PIXELS]) {
#pragma HLS INLINE

	// Check Datatype, MaximumValue, and that BIN is smaller RANGE
	const vx_type_e TYPE = GET_TYPE(SrcType);
	const bool allowed_type = (TYPE == VX_TYPE_UINT8) || (TYPE == VX_TYPE_UINT16);
	const vx_uint32 MAX_VAL = (TYPE == VX_TYPE_UINT8) ? (static_cast<vx_uint32>(0x100)) : (static_cast<vx_uint32>(0x10000));
	const bool allowed_value1 = ((DISTRIBUTION_BINS <= MAX_VAL) && (DISTRIBUTION_RANGE <= MAX_VAL));
	const bool allowed_value2 = (static_cast<vx_uint32>(DISTRIBUTION_OFFSET) <= (MAX_VAL - static_cast<vx_uint32>(1)));
	const bool allowed_range = DISTRIBUTION_BINS <= DISTRIBUTION_RANGE;
	STATIC_ASSERT(allowed_type, data_type_is_not_allowed_for_histogram);
	STATIC_ASSERT(allowed_value1, BINS_RANGE_value_out_of_range_max_0x100_for_uint8_and_0x10000_for_uint16);
	STATIC_ASSERT(allowed_value2, OFFSET_value_out_of_range_max_0xff_for_uint8_and_0xffff_for_uint16);
	STATIC_ASSERT(allowed_range, RANGE_should_be_smaller_or_equal_to_BINS);

	// Compute Scale (BINS / RANGE) "approximation"
	const vx_uint16 ACCURACY = 15;
	const vx_uint16 SCALE = static_cast<vx_uint16>((DISTRIBUTION_BINS << ACCURACY) / DISTRIBUTION_RANGE);

	// Global variables
	vx_uint16 pre_bin = 0;
	vx_uint32 pre_val = 0;

	// Buffer Histogram in BRAM
	vx_uint32 LUTA[DISTRIBUTION_BINS];
	vx_uint32 LUTB[DISTRIBUTION_BINS];

	// Reset histogram
	for (vx_uint32 i = 0; i < DISTRIBUTION_BINS; i++) {
#pragma HLS PIPELINE II=1
		LUTA[i] = 0;
		LUTB[i] = 0;
	}

	// Compute Histogram
	for (vx_uint32 i = 0; i < IMG_PIXELS + 1; i++) {
#pragma HLS PIPELINE II=1

		// Variables
		vx_uint16 cur_bin = 0;
		vx_uint32 cur_val = 0;
		SrcType pixel = 0;
		vx_bool range = vx_false_e;

		// Read input data
		if (i < IMG_PIXELS)
			pixel = input[i];

		// Check if bin will be out of range
		if ((DISTRIBUTION_OFFSET <= pixel) && (pixel <= (DISTRIBUTION_OFFSET + DISTRIBUTION_RANGE)) && (i < IMG_PIXELS))
			range = vx_true_e;
		
		// Subtract offset and compute current bin bin
		if (range) {			
			const SrcType index = pixel - DISTRIBUTION_OFFSET;

			if (DISTRIBUTION_BINS != DISTRIBUTION_RANGE)
				cur_bin = static_cast<vx_uint16>((static_cast<vx_uint32>(index) * static_cast<vx_uint32>(SCALE)) >> ACCURACY);
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
		if (range)
			cur_val++;

		// Update to store in next iteration
		pre_bin = cur_bin;
		pre_val = cur_val;
	}

	// Write output data
	for (vx_uint32 i = 0; i < DISTRIBUTION_BINS; i++) {
#pragma HLS PIPELINE II = 1
		output[i] = LUTA[i] + LUTB[i];
	}
}

/** @brief Implements the Table Lookup Image Kernel. 
* @param DataType   The data type can be vx_uint8 and vx_int16
* @param IMG_PIXELS  The amount of pixels for input and output image
* @param LUT_COUNT   Indicates the number of elements in the LUT.
* @param LUT_OFFSET  Indicates the index of the input value = 0.
* @param input       The input image (vx_uint8, vx_int16)
* @param output      The output image (vx_uint8, vx_int16)
*/
template<typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
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
#pragma HLS PIPELINE II=1
		table[i] = lut[i];
	}

	// Perform table lookup
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II=1	

		// Read from Input
		DataType src = input[i];

		// Add Offset
		vx_int32 index = (vx_int32)src + (vx_int32)LUT_OFFSET;

		// Perform Table Lookup and write to output
		if ((index >= 0) && (index < (vx_int32)LUT_COUNT))
			output[i] = table[index];
		else
			output[i] = 0;
	}
}

#endif /* SRC_IMG_OTHER_CORE_H_ */
