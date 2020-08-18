
#ifndef SRC_IMG_CONVERSION_BASE_H_
#define SRC_IMG_CONVERSION_BASE_H_

#include "img_conversion_core.h"

/** @brief  Implements the Channel Combine Kernel.
* @param DstType     The datatype of the > image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
* @param CHANNEL_AMOUNT The amount of channels to combine
* @param SRC_PIXELS  The pixels of the input image
* @param DST_PIXELS  The pixels of the output image (can differ for RGB, since the 24-bit are interleaved)
* @param OUTPUT_TYPE    The color type of the > image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX)
* @param input0         The 1. plane of the input image
* @param input1         The 2. plane of the input image
* @param input2         The 3. plane of the input image (optional)
* @param input3         The 4. plane of the input image (optional)
* @param output         The output image
*/
template<typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e OUTPUT_TYPE>
void ImgChannelCombine(
	vx_image_data<vx_uint8, 1> input0[SRC_PIXELS],
	vx_image_data<vx_uint8, 1> input1[SRC_PIXELS],
	vx_image_data<vx_uint8, 1> input2[SRC_PIXELS],
	vx_image_data<vx_uint8, 1> input3[SRC_PIXELS],
	vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input0
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=input3
#pragma HLS DATA_PACK variable=output
	ChannelCombine<DstType, CHANNEL_AMOUNT, SRC_PIXELS, DST_PIXELS, OUTPUT_TYPE>(input0, input1, input2, input3, output);
}
template<typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e OUTPUT_TYPE>
void ImgChannelCombine(vx_uint8 input0[SRC_PIXELS], vx_uint8 input1[SRC_PIXELS], vx_uint8 input2[SRC_PIXELS], vx_uint8 input3[SRC_PIXELS], 
	DstType output[DST_PIXELS]) {
#pragma HLS INLINE	
	ChannelCombine<DstType, CHANNEL_AMOUNT, SRC_PIXELS, DST_PIXELS, OUTPUT_TYPE>(input0, input1, input2, input3, output);
}

/** @brief  Implements the Channel Extraction Kernel. \n
* @param SrcType   The datatype of the input image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
* @param CHANNEL_ID  The id of the channel to extract
* @param SRC_PIXELS  The pixels of the input image
* @param DST_PIXELS  The pixels of the output image (can differ for RGB, since the 24-bit are interleaved)
* @param INPUT_TYPE  The color type of the input image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX)
* @param input       The input image
* @param output      The output image
*/
template<typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE>
void ImgChannelExtract(
	vx_image_data<SrcType, 1> input[SRC_PIXELS], 
	vx_image_data<vx_uint8, 1> output[DST_PIXELS]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	ChannelExtract<SrcType, CHANNEL_ID, SRC_PIXELS, DST_PIXELS, INPUT_TYPE>(input, output);
}
template<typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE>
void ImgChannelExtract(SrcType input[SRC_PIXELS], vx_uint8 output[DST_PIXELS]) {
#pragma HLS INLINE	
	ChannelExtract<SrcType, CHANNEL_ID, SRC_PIXELS, DST_PIXELS, INPUT_TYPE>(input, output);
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
template<typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE, vx_df_image_e OUTPUT_TYPE>
void ImgConvertColor(
	vx_image_data<SrcType, 1> input[SRC_PIXELS], 
	vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	ConvertColor<SrcType, DstType, SRC_PIXELS, DST_PIXELS, INPUT_TYPE, OUTPUT_TYPE>(input, output);
}
template<typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE, vx_df_image_e OUTPUT_TYPE>
void ImgConvertColor(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE
	ConvertColor<SrcType, DstType, SRC_PIXELS, DST_PIXELS, INPUT_TYPE, OUTPUT_TYPE>(input, output);
}

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
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL, const vx_convert_policy_e CONV_POLICY, 
	const vx_uint16 SHIFT>
void ImgConvertBitDepth(
	vx_image_data<SrcType, VEC_SIZE> input[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<DstType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]){
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}
template<typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL, const vx_convert_policy_e CONV_POLICY, 
	const vx_uint16 SHIFT>
void ImgConvertBitDepth(SrcType input[IMG_PIXEL], DstType output[IMG_PIXEL]) {
#pragma HLS INLINE
	ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}

/** Converts converts between two buffers with a different vector sizes (parallelization degree). If needed: bigger image needs to be aligned.
* @param DataType     The data type of the input and output
* @param IMAGES       The amount of images
* @param SRC_PIXELS   Amount of pixels in the input image
* @param DST_PIXELS   Amount of pixels in the output image
* @param SRC_PARALLEL The vector size of input image
* @param DST_PARALLEL The vector size of output image
* @param src          input image
* @param dst          output image
*/
template<typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ImgDataWidthConverter(
	vx_image_data<DataType, SRC_PARALLEL> input[(IMAGES * SRC_PIXELS) / SRC_PARALLEL], 
	vx_image_data<DataType, DST_PARALLEL> output[(IMAGES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(input, output);
}
template<typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ImgDataWidthConverter(
	DataType input[(IMAGES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<DataType, DST_PARALLEL> output[(IMAGES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=output
	DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>((vx_image_data<DataType, SRC_PARALLEL>*)input, output);
}
template<typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ImgDataWidthConverter(
	vx_image_data<DataType, SRC_PARALLEL> input[(IMAGES * SRC_PIXELS) / SRC_PARALLEL],
	DataType output[(IMAGES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
	DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(input, (vx_image_data<DataType, DST_PARALLEL>*)output);
}

/** Copies the input image to multiple output images
* @param DataType   The data type of the input and output
* @param IMG_PIXELS The amount of images
* @param VEC_NUM    Amount of pixels in the input image
* @param input        input image
* @param out1        output image
* @param out2        output image
* @param out3        output image
* @param out4        output image
*/
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out3[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out4[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=out1
#pragma HLS DATA_PACK variable=out2
#pragma HLS DATA_PACK variable=out3
#pragma HLS DATA_PACK variable=out4
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 4>(input, out1, out2, out3, out4);
}
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	DataType input[IMG_PIXELS],
	DataType out1[IMG_PIXELS],
	DataType out2[IMG_PIXELS],
	DataType out3[IMG_PIXELS],
	DataType out4[IMG_PIXELS]) {
#pragma HLS INLINE	
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 4>(input, out1, out2, out3, out4);
}

/** Copies the input image to multiple output images
* @param DataType   The data type of the input and output
* @param IMG_PIXELS The amount of images
* @param VEC_NUM    Amount of pixels in the input image
* @param input        input image
* @param out1        output image
* @param out2        output image
* @param out3        output image
*/
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out3[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=out1
#pragma HLS DATA_PACK variable=out2
#pragma HLS DATA_PACK variable=out3
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 3>(input, out1, out2, out3, NULL);
}
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	DataType input[IMG_PIXELS / VEC_NUM],
	DataType out1[IMG_PIXELS / VEC_NUM],
	DataType out2[IMG_PIXELS / VEC_NUM],
	DataType out3[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE	
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 3>(input, out1, out2, out3, NULL);
}

/** Copies the input image to multiple output images
* @param DataType   The data type of the input and output
* @param IMG_PIXELS The amount of images
* @param VEC_NUM    Amount of pixels in the input image
* @param input        input image
* @param out1        output image
* @param out2        output image
*/
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=out1
#pragma HLS DATA_PACK variable=out2
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 2>(input, out1, out2, NULL, NULL);
}
template<typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(
	DataType input[IMG_PIXELS / VEC_NUM],
	DataType out1[IMG_PIXELS / VEC_NUM],
	DataType out2[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE	
	Multicast<DataType, IMG_PIXELS, VEC_NUM, 2>(input, out1, out2, NULL, NULL);
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
void ImgScaleImage(vx_image_data<vx_uint8, 1> input[COLS_IN * ROWS_IN], vx_image_data<vx_uint8, 1> output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	ScaleImage<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT, SCALE_TYPE>(input, output);
}
template<vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT, vx_interpolation_type_e SCALE_TYPE>
void ImgScaleImage(vx_uint8 input[COLS_IN * ROWS_IN], vx_uint8 output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE
	ScaleImage<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT, SCALE_TYPE>((vx_image_data<vx_uint8, 1>*)input, (vx_image_data<vx_uint8, 1>*)output);
}

#endif // SRC_IMG_CONVERSION_BASE_H_
