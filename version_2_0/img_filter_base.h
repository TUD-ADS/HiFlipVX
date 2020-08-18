/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_filter_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These are all accelerated image filter functions (Call from here)
*/

#ifndef SRC_IMG_FILTER_BASE_H_
#define SRC_IMG_FILTER_BASE_H_

#include "img_filter_core.h"

/***************************************************************************************************************************************************/
/* Filter Function Declaration */
/***************************************************************************************************************************************************/

/** @brief  Computes a Box filter over a window of the input image. 
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param SEPARABLE   Is true, if separable filter should be used
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void ImgBox(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwBox<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void ImgBox(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE	
	HwBox<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>((vx_image_data<DataType, 
		VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output);
}

/** @brief  Convolves the input with the client supplied convolution matrix. 
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param conv        The custom convolution kernel
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgConvolve(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM], 
	const DataType conv[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwConvolve<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output, conv);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgConvolve(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM], 
	const DataType conv[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE		
	HwConvolve<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output, conv);
}

/** @brief  Implements Dilation, which grows the white space in a Boolean image. 
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgDilate(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwDilate<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgDilate(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwDilate<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output);
}

/** @brief  Implements Erosion, which shrinks the white space in a Boolean image. 
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgErode(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwErode<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgErode(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwErode<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output);
}

/** @brief  Computes a Gaussian filter over a window of the input image. 
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param SEPARABLE   Is true, if separable filter should be used
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void ImgGaussian(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>
void ImgGaussian(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output);
}

/** @brief  Computes a median pixel value over a window of the input image. 
@param DataType    Data type of the input/output image (unsigned)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgMedian(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwMedian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgMedian(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM],
	DataType output[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwMedian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output);
}

/** @brief  Implements the Scharr Image Filter Kernel. 
@param SrcType     Data type of the input image (unsigned)(8-bit, 16-bit, 32-bit)
@param DstType     Data type of the output image (signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       The input image
@param output1     The output image (x derivative)
@param output2     The output image (y derivative)
*/
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void ImgScharr3x3(
	vx_image_data<SrcType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DstType, VEC_NUM> output1[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DstType, VEC_NUM> output2[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output1
#pragma HLS DATA_PACK variable=output2
	HwScharr3x3<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE>(input, output1, output2);
}
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void ImgScharr3x3(
	SrcType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DstType output1[(HEIGHT*WIDTH) / VEC_NUM], 
	DstType output2[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwScharr3x3<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE>(
		(vx_image_data<SrcType, VEC_NUM> *)input, (vx_image_data<DstType, VEC_NUM> *)output1, (vx_image_data<DstType, VEC_NUM> *)output2);
}

/** @brief  Implements the Sobel Image Filter Kernel. 
@param SrcType     Data type of the input image (unsigned)(8-bit, 16-bit, 32-bit)
@param DstType     Data type of the output image (signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       The input image
@param output1     The output image (x derivative)
@param output2     The output image (y derivative)
*/
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgSobel(
	vx_image_data<SrcType, VEC_NUM> input[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DstType, VEC_NUM> output1[(HEIGHT*WIDTH) / VEC_NUM], 
	vx_image_data<DstType, VEC_NUM> output2[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output1
#pragma HLS DATA_PACK variable=output2
	HwSobel<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output1, output2);
}
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgSobel(
	SrcType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DstType output1[(HEIGHT*WIDTH) / VEC_NUM], 
	DstType output2[(HEIGHT*WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwSobel<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<SrcType, VEC_NUM> *)input, (vx_image_data<DstType, VEC_NUM> *)output1, (vx_image_data<DstType, VEC_NUM> *)output2);
}

/** @brief  Find local maxima in an image, or otherwise suppress pixels that are not local maxima.
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   The size of window over which to perform the localized non-maxima suppression (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param mask        Constrict suppression to a ROI.
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgNonMaxSuppression(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT * WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT * WIDTH) / VEC_NUM], 
	const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output, mask);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>
void ImgNonMaxSuppression(
	DataType input[(HEIGHT*WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT*WIDTH) / VEC_NUM], 
	const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE		
	HwNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM> *)input, (vx_image_data<DataType, VEC_NUM> *)output, mask);
}


/** @brief  Computes an image of response values according to the FAST9 corner detector
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void ImgSegmentTestDetector(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT * WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwSegmentTestDetector<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE>(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_border_e BORDER_TYPE>
void ImgSegmentTestDetector(
	DataType input[(HEIGHT * WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwSegmentTestDetector<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE>(
		(vx_image_data<DataType, VEC_NUM>*)input, (vx_image_data<DataType, VEC_NUM>*)output);
}

/** @brief  Observed pixel (n) is true if it is bigger than THRESH_UPPER or if it is bigger THRESH_LOWER and 
            the other pixel in window is bigger than THRESH_UPPER
@param DataType     Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH        Image width
@param HEIGHT       Image height
@param KERN_SIZE    The size of window over which to perform the localized non-maxima suppression (3, 5, 7, 9, 11)
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param THRESH_LOWER Upper threshold
@param THRESH_UPPER Lower threshold
@param input        Input image
@param output       Output image
*/
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, DataType THRESH_LOWER, 
	DataType THRESH_UPPER>
void ImgHysteresis(
	vx_image_data<DataType, VEC_NUM> input[(HEIGHT * WIDTH) / VEC_NUM], 
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	HwHysteresis<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>
		(input, output);
}
template<typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, DataType THRESH_LOWER, 
	DataType THRESH_UPPER>
void ImgHysteresis(
	DataType input[(HEIGHT * WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwHysteresis<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(
		(vx_image_data<DataType, VEC_NUM>*)input, (vx_image_data<DataType, VEC_NUM>*)output);
}

/** @brief  Find local maxima in an image depending on the phase, or otherwise suppress pixels that are not local maxima.
@param DataType     Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH        Image width
@param HEIGHT       Image height
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param QUANTIZATION The quantisazion of the orientation values. Allowed values Q = [1..8]. 2^(Q) different output values
@param input1       Input image (Gradient Magnitude)
@param input2       Input image (Orientation)
@param output       Output image
*/
template<typename DataType, vx_uint32 VEC_NUM, vx_uint32 WIDTH, vx_uint32 HEIGHT, vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>
void ImgOrientedNonMaxSuppression(
	vx_image_data<DataType, VEC_NUM> input1[(HEIGHT * WIDTH) / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> input2[(HEIGHT * WIDTH) / VEC_NUM],
	vx_image_data<DataType, VEC_NUM> output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	HwOrientedNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE, QUANTIZATION>(
		input1, input2, output);
}
template<typename DataType, vx_uint32 VEC_NUM, vx_uint32 WIDTH, vx_uint32 HEIGHT, vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>
void ImgOrientedNonMaxSuppression(
	DataType input1[(HEIGHT * WIDTH) / VEC_NUM], 
	DataType input2[(HEIGHT * WIDTH) / VEC_NUM], 
	DataType output[(HEIGHT * WIDTH) / VEC_NUM]) {
#pragma HLS INLINE		
	HwOrientedNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, HEIGHT, BORDER_TYPE, QUANTIZATION>(
		(vx_image_data<DataType, VEC_NUM>*)input1, (vx_image_data<DataType, VEC_NUM>*)input2, (vx_image_data<DataType, VEC_NUM>*)output);
}

#endif /* SRC_IMG_FILTER_BASE_H_ */
