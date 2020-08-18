/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_other_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These are all remaining image processing functions functions (Call from here)
*/

#ifndef SRC_IMG_ANALYSIS_BASE_H_
#define SRC_IMG_ANALYSIS_BASE_H_

#include "img_analysis_core.h"


/** These features allow for conditional graph flow in OpenVX, via support for a variety of operations between two scalars.
* @param SrcType           The data type of the input scalars
* @param DstType           The data type of the output scalar
* @param scalar_operation  The scalar operation enum
* @param a                 First scalar operand
* @param b                 Second scalar operand
* @param output            Result of the scalar operation
*/
template<typename SrcType, typename DstType, vx_scalar_operation_e scalar_operation>
void ImgControlFlow(vx_image_data<SrcType, 1> a[1], vx_image_data<SrcType, 1> b[1], vx_image_data<DstType, 1> output[1]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=a
#pragma HLS DATA_PACK variable=b
#pragma HLS DATA_PACK variable=output
	ControlFlow<SrcType, DstType, scalar_operation>(a, b, output);
}
template<typename SrcType, typename DstType, vx_scalar_operation_e scalar_operation>
void ImgControlFlow(SrcType a[1], SrcType b[1], DstType output[1]) {
#pragma HLS INLINE	
	ControlFlow<SrcType, DstType, scalar_operation>((vx_image_data<SrcType, 1>*)a, (vx_image_data<SrcType, 1>*)b, (vx_image_data<DstType, 1>*)output);
}


/** @brief   Equalizes the histogram of a grayscale image. The output image dimensions should be the same as the dimensions of the input image.
*   @details This kernel uses Histogram Equalization to modify the values of a grayscale image so that it will automatically have a standardized 
             brightness and contrast.
* @param IMG_PIXELS The amount of pixels for input and output image
* @param input1     The grayscale input image in vx_uint8 (1. read)
* @param input2     The grayscale input image in vx_uint8 (2. read)
* @param output     The grayscale output image of type vx_uint8 with equalized brightness and contrast and same size as the input image.
*/
template<vx_uint32 IMG_PIXELS>
void ImgEqualizeHist(
	vx_image_data<vx_uint8, 1> input1[IMG_PIXELS],
	vx_image_data<vx_uint8, 1> input2[IMG_PIXELS],
	vx_image_data<vx_uint32, 1> output[IMG_PIXELS]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=output
	EqualizeHist<IMG_PIXELS>(input1, input2, output);
}
template<vx_uint32 IMG_PIXELS>
void ImgEqualizeHist(vx_uint8 input1[IMG_PIXELS], vx_uint8 input2[IMG_PIXELS], vx_uint32 output[IMG_PIXELS]) {
#pragma HLS INLINE	
	EqualizeHist<IMG_PIXELS>(input1, input2, output);
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
template<typename SrcType, vx_uint32 IMG_PIXELS, vx_uint32 DISTRIBUTION_BINS, vx_uint32 DISTRIBUTION_RANG, SrcType DISTRIBUTION_OFFSET>
void ImgHistogram(
	vx_image_data<SrcType, 1> input[IMG_PIXELS],
	vx_image_data<vx_uint32, 1> output[IMG_PIXELS]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	Histogram<SrcType, IMG_PIXELS, DISTRIBUTION_BINS, DISTRIBUTION_RANG, DISTRIBUTION_OFFSET>(input, output);
}
template<typename SrcType, vx_uint32 IMG_PIXELS, vx_uint32 DISTRIBUTION_BINS, vx_uint32 DISTRIBUTION_RANG, SrcType DISTRIBUTION_OFFSET>
void ImgHistogram(SrcType input[IMG_PIXELS], vx_uint32 output[IMG_PIXELS]) {
#pragma HLS INLINE
	Histogram<SrcType, IMG_PIXELS, DISTRIBUTION_BINS, DISTRIBUTION_RANG, DISTRIBUTION_OFFSET>(input, output);
}

/** @brief  Computes the integral image of the input. The output image dimensions should be the same as the dimensions of the input image.
*   @details Each output pixel is the sum of the corresponding input pixel and all other pixels above and to the left of it. 
             The overflow policy used is VX_CONVERT_POLICY_WRAP.
* @param IMG_COLS    The columns of the image
* @param IMG_ROWS    The rows of the image
* @param input       The input image
* @param output      The output image
*/
template<vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
void ImgIntegral(
	vx_image_data<vx_uint8, 1> input[IMG_COLS * IMG_ROWS],
	vx_image_data<vx_uint32, 1> output[IMG_COLS * IMG_ROWS]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output
	Integral<IMG_COLS, IMG_ROWS>(input, output);
}
template<vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
void ImgIntegral(vx_uint8 input[IMG_ROWS * IMG_COLS], vx_uint32 output[IMG_ROWS * IMG_COLS]) {
#pragma HLS INLINE
	Integral<IMG_COLS, IMG_ROWS>(input, output);
}


/** @brief Computes the mean pixel value and the standard deviation of the pixels in the input image (which has a dimension width and height). 
           Use with or without all parameters marked as optional.
* @param DataType   The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS   The amount of columns in the image
* @param IMG_ROWS   The amount of rows in the image
* @param input1     The input image (read 1. time)
* @param mean       The VX_TYPE_FLOAT32 average pixel value.
* @param input2     [optional] The input image (read 2. time)
* @param stddev     [optional] The VX_TYPE_FLOAT32 standard deviation of the pixel values. (a small aproximation made here)
*/
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMeanStdDev(
	vx_image_data<DataType, 1> input1[IMG_ROWS * IMG_COLS],
	vx_image_data<vx_float32, 1> mean[1],
	vx_image_data<DataType, 1> input2[IMG_ROWS * IMG_COLS],
	vx_image_data<vx_float32, 1> stddev[1]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=mean
#pragma HLS DATA_PACK variable=input2
#pragma HLS DATA_PACK variable=stddev
	MeanStdDev<DataType, IMG_COLS, IMG_ROWS, vx_true_e>(input1, mean, input2, stddev);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMeanStdDev(DataType input1[IMG_ROWS * IMG_COLS], vx_float32 mean[1], DataType input2[IMG_ROWS * IMG_COLS], vx_float32 stddev[1]) {
#pragma HLS INLINE	
	MeanStdDev<DataType, IMG_COLS, IMG_ROWS, vx_true_e>(input1, mean, input2, stddev);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMeanStdDev(
	vx_image_data<DataType, 1> input1[IMG_ROWS * IMG_COLS],
	vx_image_data<vx_float32, 1> mean[1]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input1
#pragma HLS DATA_PACK variable=mean
	MeanStdDev<DataType, IMG_COLS, IMG_ROWS, vx_false_e>(input1, mean, NULL, NULL);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMeanStdDev(DataType input1[IMG_ROWS * IMG_COLS], vx_float32 mean[1]) {
#pragma HLS INLINE	
	MeanStdDev<DataType, IMG_COLS, IMG_ROWS, vx_false_e>(input1, mean, NULL, NULL);
}

/** @brief Finds the minimum and maximum values in an image and a location for each. If the input image has several minimums/maximums, 
           the kernel returns all of them. Use with or without all parameters marked as optional.
* @param DataType The data type of the input, which can be 8-bit/16-bit signed/unsigned
* @param IMG_COLS   The amount of columns in the image
* @param IMG_ROWS   The amount of rows in the image
* @param CAPACITY   [optional] The capacity of the arrays that store the locations (minLoc , max_loc)
* @param input      The input image
* @param min_val    The minimum value in the image, which corresponds to the type of the input.
* @param max_val    The maximum value in the image, which corresponds to the type of the input.
* @param min_loc    [optional] The minimum VX_TYPE_COORDINATES2D locations. If the input image has several minimums, 
                               the kernel will return up to the capacity of the array.
* @param max_loc    [optional] The maximum VX_TYPE_COORDINATES2D locations. If the input image has several maximums, 
                               the kernel will return up to the capacity of the array.
* @param min_count  [optional] The total number of detected minimums in image.
* @param max_count  [optional] The total number of detected maximums in image.
*/
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint16 CAPACITY>
void ImgMinMaxLocation(
	vx_image_data<DataType, 1> input[IMG_ROWS * IMG_COLS],
	vx_image_data<DataType, 1> min_val[1],
	vx_image_data<DataType, 1> max_val[1],
	vx_image_data<vx_uint16, 2> min_loc[CAPACITY],
	vx_image_data<vx_uint16, 2> max_loc[CAPACITY],
	vx_image_data<vx_uint16, 1> min_count[1],
	vx_image_data<vx_uint16, 1> max_count[1]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=min_val
#pragma HLS DATA_PACK variable=max_val
#pragma HLS DATA_PACK variable=min_loc
#pragma HLS DATA_PACK variable=max_loc
#pragma HLS DATA_PACK variable=min_count
#pragma HLS DATA_PACK variable=max_count
	MinMaxLocation<DataType, IMG_COLS, IMG_ROWS, CAPACITY, vx_true_e>(input, min_val, max_val, min_loc, max_loc, min_count, max_count);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint16 CAPACITY>
void ImgMinMaxLocation(
	DataType input[IMG_ROWS * IMG_COLS], 
	DataType min_val[1], 
	DataType max_val[1], 
	vx_image_data<vx_uint16, 2> min_loc[CAPACITY], 
	vx_image_data<vx_uint16, 2> max_loc[CAPACITY], 
	vx_uint16 min_count[1], 
	vx_uint16 max_count[1]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=min_loc
#pragma HLS DATA_PACK variable=max_loc
	MinMaxLocation<DataType, IMG_COLS, IMG_ROWS, CAPACITY, vx_true_e>(input, min_val, max_val, min_loc, max_loc, min_count, max_count);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMinMaxLocation(
	vx_image_data<DataType, 1> input[IMG_ROWS * IMG_COLS],
	vx_image_data<DataType, 1> min_val[1],
	vx_image_data<DataType, 1> max_val[1]) {
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=min_val
#pragma HLS DATA_PACK variable=max_val
#pragma HLS INLINE	
	MinMaxLocation<DataType, IMG_COLS, IMG_ROWS, (vx_uint16)1, vx_false_e>(input, min_val, max_val, NULL, NULL, NULL, NULL);
}
template<typename DataType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS>
void ImgMinMaxLocation(DataType input[IMG_ROWS * IMG_COLS], DataType min_val[1], DataType max_val[1]) {
#pragma HLS INLINE	
	MinMaxLocation<DataType, IMG_COLS, IMG_ROWS, (vx_uint16)1, vx_false_e>(input, min_val, max_val, NULL, NULL, NULL, NULL);
}

/** @brief  Implements the Table Lookup Image Kernel. The output image dimensions should be the same as the dimensions of the input image.
*   @details This kernel uses each pixel in an image to index into a LUT and put the indexed LUT value into the output image. 
             The formats supported are vx_uint8 and vx_int16.
* @param DataType   The data type can be vx_uint8 and vx_int16
* @param IMG_PIXELS  The amount of pixels for input and output image
* @param LUT_COUNT   Indicates the number of elements in the LUT.
* @param LUT_OFFSET  Indicates the index of the input value = 0.
* @param input       The input image (vx_uint8, vx_int16)
* @param output      The output image (vx_uint8, vx_int16)
*/
template<typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
void ImgTableLookup(
	vx_image_data<DataType, 1> input[IMG_PIXELS],
	vx_image_data<DataType, 1> lut[LUT_COUNT],
	vx_image_data<DataType, 1> output[IMG_PIXELS]) {
#pragma HLS INLINE	
#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=lut
#pragma HLS DATA_PACK variable=output
	TableLookup<DataType, IMG_PIXELS, LUT_COUNT, LUT_OFFSET>(input, lut, output);
}
template<typename DataType, const vx_uint32 IMG_PIXELS, const vx_uint32 LUT_COUNT, const vx_uint32 LUT_OFFSET>
void ImgTableLookup(DataType input[IMG_PIXELS], DataType lut[LUT_COUNT], DataType output[IMG_PIXELS]) {
#pragma HLS INLINE	
	TableLookup<DataType, IMG_PIXELS, LUT_COUNT, LUT_OFFSET>(input, lut, output);
}

#endif /* SRC_IMG_ANALYSIS_BASE_H_ */
