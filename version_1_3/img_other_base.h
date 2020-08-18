/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_other_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
* @brief Description:\n
*  These are all remaining image processing functions functions (Call from here)
*/

#ifndef SRC_IMG_OTHER_BASE_H_
#define SRC_IMG_OTHER_BASE_H_

#include "img_other_core.h"

/*********************************************************************************************************************/
/* Other Operations */
/*********************************************************************************************************************/

/** @brief Converts image bit depth. The output image dimensions should be the same as the dimensions of the input image. \n
*   @details (d = down | u = up | s = sign) \n
* type|u8 |s8 |u16|s16|u32|s32
* ----|---|---|---|---|---|---
* u8	| -	| s	| u	| u	| u	| u
* s8	| s	| -	| u	| u	| u	| u
* u16	| d	| d	| -	| s	| u	| u
* s16	| d	| d	| s	| -	| u	| u
* u32	| d	| d	| d	| d	| -	| s
* s32	| d	| d	| d	| d	| s	| -
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
void ImgConvertBitDepth(vx_image<SrcType, VEC_SIZE> input[IMG_PIXEL], vx_image<DstType, VEC_SIZE> output[IMG_PIXEL]) {
#pragma HLS INLINE
	ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL, const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ImgConvertBitDepth(SrcType input[IMG_PIXEL], DstType output[IMG_PIXEL]) {
#pragma HLS INLINE
	ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}

/** @brief  Converts the Color of an image between RGB/RGBX/Gray \n
*   @details Possible Conversions are \n
* VX_DF_IMAGE_U8   -> VX_DF_IMAGE_RGB \n
* VX_DF_IMAGE_U8   -> VX_DF_IMAGE_RGBX \n
* VX_DF_IMAGE_RGB  -> VX_DF_IMAGE_U8 \n
* VX_DF_IMAGE_RGB  -> VX_DF_IMAGE_RGBX \n
* VX_DF_IMAGE_RGBX -> VX_DF_IMAGE_U8 \n
* VX_DF_IMAGE_RGBX -> VX_DF_IMAGE_RGB \n
* @param SrcType      The datatype of the input image  (uint8 (Gray), uint32 (RGB, RGBX))
* @param DstType     The datatype of the output image (uint8 (Gray), uint32 (RGB, RGBX))
* @param IMG_PIXELS  The pixels of the image
* @param INPUT_TYPE  The color type of the input image (RGB, RGBX, Gray)
* @param OUTPUT_TYPE The color type of the output image (RGB, RGBX, Gray)
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, typename DstType, const vx_uint32 IMG_PIXELS, vx_df_image_e INPUT_TYPE, vx_df_image_e OUTPUT_TYPE>
void ImgConvertColor(SrcType input[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE
	ConvertColor<SrcType, DstType, IMG_PIXELS, INPUT_TYPE, OUTPUT_TYPE>(input, output);
}

/** @brief  Implements the Channel Extraction Kernel. \n
* @param SrcType   The datatype of the input image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
* @param CHANNEL_ID  The id of the channel to extract
* @param IMG_PIXELS  The pixels of the image
* @param INPUT_TYPE  The color type of the input image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX)
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, const vx_uint16 CHANNEL_ID, const vx_uint32 IMG_PIXELS, vx_df_image_e INPUT_TYPE>
void ImgChannelExtract(SrcType input[IMG_PIXELS], vx_uint8 output[IMG_PIXELS]) {
#pragma HLS INLINE	
	ChannelExtract<SrcType, CHANNEL_ID, IMG_PIXELS, INPUT_TYPE>(input, output);
}

/** @brief  Implements the Channel Combine Kernel.
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
void ImgChannelCombine(vx_uint8 input0[IMG_PIXELS], vx_uint8 input1[IMG_PIXELS], vx_uint8 input2[IMG_PIXELS], vx_uint8 input3[IMG_PIXELS], DstType output[IMG_PIXELS]) {
#pragma HLS INLINE	
	ChannelCombine<DstType, CHANNEL_AMOUNT, IMG_PIXELS, OUTPUT_TYPE>(input0, input1, input2, input3, output);
}

/** @brief  Scale an image down using bilinear or nearest neighbor interpolation
* @param COLS_IN     The columns of the input image
* @param ROWS_IN     The rows of the input image
* @param COLS_OUT    The columns of the output image
* @param ROWS_OUT    The rows of the output image
* @param SCALE_TYPE  The type of interpolation (VX_INTERPOLATION_BILINEAR or VX_INTERPOLATION_AREA)
* @param input       The input image
* @param output      The output image
*/
template<vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT, vx_interpolation_type_e SCALE_TYPE>
void ImgScaleImage(vx_uint8 input[COLS_IN*ROWS_IN], vx_uint8 output[COLS_OUT*ROWS_OUT]) {
#pragma HLS INLINE
	ScaleImage<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT, SCALE_TYPE>(input, output);
}

/** @brief  Computes the integral image of the input. The output image dimensions should be the same as the dimensions of the input image.
*   @details Each output pixel is the sum of the corresponding input pixel and all other pixels above and to the left of it. The overflow policy used is VX_CONVERT_POLICY_WRAP.
* @param IMG_COLS    The columns of the image
* @param IMG_ROWS    The rows of the image
* @param input       The input image
* @param output      The output image
*/
template<const vx_uint16 IMG_COLS, const vx_uint16 IMG_ROWS>
void ImgIntegral(vx_uint8 input[IMG_ROWS*IMG_COLS], vx_uint32 output[IMG_ROWS*IMG_COLS]) {
#pragma HLS INLINE
	Integral<IMG_COLS, IMG_ROWS>(input, output);
}

/** @brief  Generates a distribution from an image.
*   @details This kernel counts the number of occurrences of each pixel value within the window size of a pre-calculated number of bins. 
* @param SrcType              The Input Type can be vx_uint8 and vx_uint16
* @param IMG_PIXELS           The amount of pixels for input and output image
* @param DISTRIBUTION_BINS    Indicates the number of bins.
* @param DISTRIBUTION_RANG    Indicates the total number of the consecutive values of the distribution interval.
* @param DISTRIBUTION_OFFSET  Indicates the start of the values to use (inclusive).
* @param input                The input image (vx_uint8, vx_uint16)
* @param output               The output image (vx_uint32)
*/
template<typename SrcType, const vx_uint32 IMG_PIXELS, const vx_uint32 DISTRIBUTION_BINS, const vx_uint32 DISTRIBUTION_RANG, const SrcType DISTRIBUTION_OFFSET>
void ImgHistogram(SrcType input[IMG_PIXELS], vx_uint32 output[IMG_PIXELS]) {
#pragma HLS INLINE
	Histogram<SrcType, IMG_PIXELS, DISTRIBUTION_BINS, DISTRIBUTION_RANG, DISTRIBUTION_OFFSET>(input, output);
}

/** @brief  Implements the Table Lookup Image Kernel. The output image dimensions should be the same as the dimensions of the input image.
*   @details This kernel uses each pixel in an image to index into a LUT and put the indexed LUT value into the output image. The formats supported are vx_uint8 and vx_int16.
* @param DataType   The data type can be vx_uint8 and vx_int16
* @param IMG_PIXELS  The amount of pixels for input and output image
* @param LUT_COUNT   Indicates the number of elements in the LUT.
* @param LUT_OFFSET  Indicates the index of the input value = 0.
* @param input       The input image (vx_uint8, vx_int16)
* @param output      The output image (vx_uint8, vx_int16)
*/
template<typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
void ImgTableLookup(DataType input[IMG_PIXELS], DataType lut[LUT_COUNT], DataType output[IMG_PIXELS]) {
#pragma HLS INLINE	
	TableLookup<DataType, IMG_PIXELS, LUT_COUNT, LUT_OFFSET>(input, lut, output);
}

#endif /* SRC_IMG_OTHER_BASE_H_ */
