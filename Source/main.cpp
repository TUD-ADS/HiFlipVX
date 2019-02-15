/**
* @file    main.cpp
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
*
* @section LICENSE
* GNU GPLv3:
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @section DESCRIPTION
* This is the main function for testing
*/

#include "img_filter_base.h"
#include "img_filter_test.h"
#include "img_other_base.h"
#include "img_other_test.h"
#include "img_pixelop_base.h"
#include "img_pixelop_test.h"

/*********************************************************************************************************************/
/* System configurations */
/*********************************************************************************************************************/

// Image Size
#define COLS_FHD 1920
#define ROWS_FHD 1080
#define PIXELS_FHD COLS_FHD*ROWS_FHD
#define COLS_HD 1280
#define ROWS_HD 720
#define PIXELS_HD COLS_HD*ROWS_HD

// Vectorization
#define VEC_NUM 1
#define VEC_PIX (PIXELS_FHD / VEC_NUM)

/*********************************************************************************************************************/
/* Pixelwise Operations (Same Data Type) */
/*********************************************************************************************************************/

// Defines
#define PIXELWISE_CONV_TYPE  VX_CONVERT_POLICY_WRAP          // VX_CONVERT_POLICY_SATURATE, VX_CONVERT_POLICY_WRAP
#define PIXELWISE_ROUND_TYPE VX_ROUND_POLICY_TO_NEAREST_EVEN // VX_ROUND_POLICY_TO_NEAREST_EVEN, VX_ROUND_POLICY_TO_ZERO
#define MULT_SCALE            static_cast<vx_uint32>(0x4000) // 0x4000=0.25 ... 0x3333=0.2

// Typedefs
typedef vx_uint8 pixelwise_type_1;
#if (VEC_NUM == 1)
typedef pixelwise_type_1 pixelwise_image_1;
#else
typedef vx_image<pixelwise_type_1, VEC_NUM> pixelwise_image_1;
#endif

// Copy a data object to another.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwTestCopy(pixelwise_image_1 input[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgCopy<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input, output);
}

// Performs a bitwise NOT operation on a input images.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwTestNot(pixelwise_image_1 input[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgNot<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input, output);
}

// Performs a bitwise AND operation between two unsigned images.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestAnd(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgAnd<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Performs a bitwise INCLUSIVE OR operation between two unsigned images.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestOr(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgOr<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Performs a bitwise EXCLUSIVE OR (XOR) operation between two unsigned images.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestXor(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgXor<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Implements a pixel-wise maximum kernel.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestMax(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgMax<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Implements a pixel-wise minimum kernel.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestMin(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgMin<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Computes the absolute difference between two images
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestAbsDiff(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgAbsDiff<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Performs addition between two images
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestAdd(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgAdd<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Performs subtraction between two images
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestSub(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgSubtract<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Implements the Gradient Magnitude Computation Kernel
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestMagnitude(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgMagnitude<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_ROUND_TYPE>(input1, input2, output);
}

// Performs element-wise multiplication between two images and a scalar value
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, output:SEQUENTIAL)
void HwTestMultiply(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
	ImgMultiply<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE, PIXELWISE_ROUND_TYPE, MULT_SCALE>(input1, input2, output);
}

/*********************************************************************************************************************/
/* Accelerated Filter functions (Wrapper needed, since templates cant be accelerated) */
/*********************************************************************************************************************/

// Defines
#define FILTER_SIZE 3
#define SEPARABLE_FILTER   (vx_bool)vx_true_e
#define CONVOLVE_FILTER_SIZE 3
#define BOX_BORDER      VX_BORDER_CONSTANT
#define CONVOLVE_BORDER VX_BORDER_CONSTANT
#define DILATE_BORDER   VX_BORDER_CONSTANT
#define ERODE_BORDER    VX_BORDER_CONSTANT
#define GAUSSIAN_BORDER VX_BORDER_CONSTANT
#define MEDIAN_BORDER   VX_BORDER_REPLICATE
#define SCHARR_BORDER   VX_BORDER_CONSTANT
#define SOBEL_BORDER    VX_BORDER_CONSTANT

// Typedefs
typedef vx_uint8 filter_u_type;
typedef vx_int8 filter_s_type;
#if (VEC_NUM == 1)
typedef filter_u_type filter_u_image;
typedef filter_s_type filter_s_image;
#else
typedef vx_image<filter_u_type, VEC_NUM> filter_u_image;
typedef vx_image<filter_s_type, VEC_NUM> filter_s_image;
#endif

// Convolution kernel for the HwConvolve filter 
const filter_u_type convolve_kernel[CONVOLVE_FILTER_SIZE][CONVOLVE_FILTER_SIZE] = {
	{ 191, 185, 157 },
	{ 237, 171, 245 },
	{ 219, 235, 173 }
};

// Computes a Box filter over a window of the input image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwBox(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgBox<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BOX_BORDER, SEPARABLE_FILTER>(input, output);
}

// Convolves the input with the client supplied convolution matrix.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwConvolve(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgConvolve<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, CONVOLVE_FILTER_SIZE, CONVOLVE_BORDER>(input, output, convolve_kernel);
}

// Implements Dilation, which grows the white space in an image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwDilate(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgDilate<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, DILATE_BORDER>(input, output);
}

// Implements Erosion, which shrinks the white space in an image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwErode(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgErode<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, ERODE_BORDER>(input, output);
}

// Computes a Gaussian filter over a window of the input image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwGaussian(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgGaussian<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, GAUSSIAN_BORDER, SEPARABLE_FILTER>(input, output);
}

// Computes a median pixel value over a window of the input image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void TestHwMedian(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
	ImgMedian<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, MEDIAN_BORDER>(input, output);
}

// Implements the Scharr Image Filter Kernel.
#pragma SDS data access_pattern(input:SEQUENTIAL, output1:SEQUENTIAL, output2:SEQUENTIAL)
void TestHwScharr3x3(filter_u_image input[VEC_PIX], filter_s_image output1[VEC_PIX], filter_s_image output2[VEC_PIX]) {
	ImgScharr3x3<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, SCHARR_BORDER>(input, output1, output2);
}

// Implements the Sobel Image Filter Kernel.
#pragma SDS data access_pattern(input:SEQUENTIAL, output1:SEQUENTIAL, output2:SEQUENTIAL)
void TestHwSobel(filter_u_image input[VEC_PIX], filter_s_image output1[VEC_PIX], filter_s_image output2[VEC_PIX]) {
	ImgSobel<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, SOBEL_BORDER>(input, output1, output2);
}

/*********************************************************************************************************************/
/* Accelerated Other functions (Wrapper needed, since templates cant be accelerated */
/*********************************************************************************************************************/

// Defines
#define BIT_DEPTH_CONVERSION VX_CONVERT_POLICY_SATURATE // VX_CONVERT_POLICY_SATURATE, VX_CONVERT_POLICY_WRAP
#define BIT_DEPTH_SHIFT 8 
#define COLOUR_CONV_SRC VX_DF_IMAGE_RGB                 // VX_DF_IMAGE_U8, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX
#define COLOUR_CONV_DST VX_DF_IMAGE_U8                  // VX_DF_IMAGE_U8, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX
#define COMBINE_AMOUNT  3                              // 2, 3, 4
#define COMBINE_TYPE    VX_DF_IMAGE_RGBX               // VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX
#define EXTRACT_CHANNEL 0                               // 0, 1, 2, 3
#define EXTRACT_TYPE    VX_DF_IMAGE_RGBX                // VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX
#define SCALE_INTERPOLATION VX_INTERPOLATION_BILINEAR   // VX_INTERPOLATION_BILINEAR, VX_INTERPOLATION_NEAREST_NEIGHBOR
#define HIST_BINS   256
#define HIST_OFFSET 0
#define HIST_RANGE  (1 << 8)            // (1 << 8), (1 << 16)
#define LUT_BINS   256
#define LUT_OFFSET 0

// Typedefs
typedef vx_int16 bit_depth_src_type; // vx_uint8, vx_int8, vx_uint16, vx_int16, vx_uint32, vx_int32
typedef vx_uint8 bit_depth_dst_type; // vx_uint8, vx_int8, vx_uint16, vx_int16, vx_uint32, vx_int32
typedef vx_uint32 combine_type; // vx_uint16, vx_uint32
typedef vx_uint32 extract_type; // vx_uint16, vx_uint32
typedef vx_uint8 lut_type;      // vx_uint8, vx_int16
typedef vx_uint8 hist_type;     // vx_uint8, vx_uint16
#if (VEC_NUM == 1)
typedef bit_depth_src_type bit_depth_src_image;
typedef bit_depth_dst_type bit_depth_dst_image;
#else
typedef vx_image<bit_depth_src_type, VEC_NUM> bit_depth_src_image;
typedef vx_image<bit_depth_dst_type, VEC_NUM> bit_depth_dst_image;
#endif

// Converts image bit depth.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwConvertBitDepth(bit_depth_src_image input[VEC_PIX], bit_depth_dst_image output[VEC_PIX]) {
	ImgConvertBitDepth<bit_depth_src_type, bit_depth_dst_type, VEC_NUM, PIXELS_FHD, BIT_DEPTH_CONVERSION, BIT_DEPTH_SHIFT>(input, output);
}

// Converts the Color of an image between RGB/RGBX/Gray
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwColourConversion(vx_uint32 input[PIXELS_FHD], vx_uint8 output[PIXELS_FHD]) {
	ImgConvertColor<vx_uint32, vx_uint8, PIXELS_FHD, COLOUR_CONV_SRC, COLOUR_CONV_DST>(input, output);
}

// Implements the Channel Combine Kernel.
#pragma SDS data access_pattern(input1:SEQUENTIAL, input2:SEQUENTIAL, input3:SEQUENTIAL, input4:SEQUENTIAL, output:SEQUENTIAL)
void HwChannelCombine(vx_uint8 input1[PIXELS_FHD], vx_uint8 input2[PIXELS_FHD], vx_uint8 input3[PIXELS_FHD], vx_uint8 input4[PIXELS_FHD], combine_type output[PIXELS_FHD]) {
	ImgChannelCombine<combine_type, COMBINE_AMOUNT, PIXELS_FHD, COMBINE_TYPE>(input1, input2, input3, input4, output);
}

//  Implements the Channel Extraction Kernel.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwChannelExtract(extract_type input[PIXELS_FHD], vx_uint8 output[PIXELS_FHD]) {
	ImgChannelExtract<extract_type, EXTRACT_CHANNEL, PIXELS_FHD, EXTRACT_TYPE>(input, output);
}

// Scale an image down using bilinear or nearest neighbor interpolation
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwScaleImage(vx_uint8 input[PIXELS_FHD], vx_uint8 output[PIXELS_HD]) {
#pragma HLS inline
	ImgScaleImage<COLS_FHD, ROWS_FHD, COLS_HD, ROWS_HD, SCALE_INTERPOLATION>(input, output);
}
								 
// Computes the integral image of the input. 
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwIntegral(vx_uint8 input[PIXELS_FHD], vx_uint32 output[PIXELS_FHD]) {
	ImgIntegral<COLS_FHD, ROWS_FHD>(input, output);
}

// Generates a distribution from an image.
#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void HwHistogram(hist_type input[PIXELS_FHD], vx_uint32 output[HIST_BINS]) {
	ImgHistogram<hist_type, PIXELS_FHD, HIST_BINS, HIST_RANGE, HIST_OFFSET>(input, output);
}

// Implements the Table Lookup Image Kernel.
#pragma SDS data access_pattern(input:SEQUENTIAL, lut:SEQUENTIAL, output:SEQUENTIAL)
void HwTableLookup(lut_type input[PIXELS_FHD], lut_type lut[LUT_BINS], lut_type output[PIXELS_FHD]) {
	ImgTableLookup<lut_type, PIXELS_FHD, LUT_BINS, LUT_OFFSET>(input, lut, output);
}

/*********************************************************************************************************************/
/* Example Application. Gradient Magnitude (including loop level parallelism) */
/*********************************************************************************************************************/

#pragma SDS data access_pattern(input:SEQUENTIAL, output:SEQUENTIAL)
void ExampleApplication (filter_u_image input[VEC_PIX], filter_s_image output[VEC_PIX]) {

	// FIFOs to stream data between functions
	static filter_s_image lx[PIXELS_FHD];
#pragma HLS STREAM variable = lx depth = 512
	static filter_s_image ly[PIXELS_FHD];
#pragma HLS STREAM variable = ly depth = 512

	// Pragma to stream between loops/functions
#pragma HLS DATAFLOW

	// Compute the derivatives in X and Y direction
	ImgScharr3x3<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, SCHARR_BORDER>(input, lx, ly);

	// Compute the Gradient Magnitude
	ImgMagnitude<filter_s_type, VEC_NUM, PIXELS_FHD, PIXELWISE_ROUND_TYPE>(lx, ly, output);
}


/*********************************************************************************************************************/
/* Main function */
/*********************************************************************************************************************/
int main(void) {

	/******** Call Test Functions ********************************************************************/
	//swTestOtherMain();
	//SwTestPixelopMain();
	//TestFilterMain();

	/******** Allocate memory ********************************************************************/
	// Buffers for Pixelwise Functions
	pixelwise_image_1 *inputA = CreateImage<pixelwise_image_1, pixelwise_type_1, COLS_FHD, ROWS_FHD>();
	pixelwise_image_1 *inputB = CreateImage<pixelwise_image_1, pixelwise_type_1, COLS_FHD, ROWS_FHD>();	
	pixelwise_image_1 *outputA = CreateImage<pixelwise_image_1, pixelwise_type_1, COLS_FHD, ROWS_FHD>();

	// Buffers for Filter Functions
	filter_u_image *srcA = CreateImage<filter_u_image, filter_u_type, COLS_FHD, ROWS_FHD>();
	filter_u_image *dstA = CreateImage<filter_u_image, filter_u_type, COLS_FHD, ROWS_FHD>();
	filter_s_image *dstX = CreateImage<filter_s_image, filter_s_type, COLS_FHD, ROWS_FHD>();
	filter_s_image *dstY = CreateImage<filter_s_image, filter_s_type, COLS_FHD, ROWS_FHD>();

	// Buffers for Analysis and Conversion Functions
	vx_uint8 *src_fhd_u8_a = CreateImage<vx_uint8, vx_uint8, COLS_FHD, ROWS_FHD>();
	vx_uint8 *src_fhd_u8_b = CreateImage<vx_uint8, vx_uint8, COLS_FHD, ROWS_FHD>();
	vx_uint8 *src_fhd_u8_c = CreateImage<vx_uint8, vx_uint8, COLS_FHD, ROWS_FHD>();
	vx_uint8 *src_fhd_u8_d = CreateImage<vx_uint8, vx_uint8, COLS_FHD, ROWS_FHD>();
	vx_uint32 *src_fhd_u32 = CreateImage<vx_uint32, vx_uint32, COLS_FHD, ROWS_FHD>();
	bit_depth_src_image *src_bitdepth = CreateImage<bit_depth_src_image, bit_depth_src_type, COLS_FHD, ROWS_FHD>();
	bit_depth_dst_image *dst_bitdepth = CreateImage<bit_depth_dst_image, bit_depth_dst_type, COLS_FHD, ROWS_FHD>();
	vx_uint8 *dst_fhd_u8_a = CreateImage<vx_uint8, vx_uint8, COLS_FHD, ROWS_FHD>();
	vx_uint8 *dst_hd_u8_a = CreateImage<vx_uint8, vx_uint8, COLS_HD, ROWS_HD>();
	vx_uint32 *dst_fhd_u32_a = CreateImage<vx_uint32, vx_uint32, COLS_FHD, ROWS_FHD>();
	combine_type *dst_combine = CreateImage<combine_type, combine_type, COLS_FHD, ROWS_FHD>();
	extract_type *src_extract = CreateImage<extract_type, extract_type, COLS_FHD, ROWS_FHD>();
	hist_type *src_hist = CreateImage<hist_type, hist_type, COLS_FHD, ROWS_FHD>();
	lut_type *src_lut1 = CreateImage<lut_type, lut_type, COLS_FHD, ROWS_FHD>();
	lut_type *src_lut2 = CreateImage<lut_type, lut_type, LUT_BINS, 1>();
	lut_type *dst_lut = CreateImage<lut_type, lut_type, COLS_FHD, ROWS_FHD>();

	// Create random data
	for (vx_uint32 i = 0; i < PIXELS_FHD; i++) {
		vx_uint8 a = rand() % 256;
		vx_uint8 b = rand() % 256;
		((filter_u_type *)(srcA))[i] = a;
		((pixelwise_type_1 *)(inputA))[i] = a;
		((pixelwise_type_1 *)(inputB))[i] = b;
		src_fhd_u8_a[i] = a;
		src_fhd_u8_b[i] = b;
	}

	/******** Call HW Functions ********************************************************************/
	// Pixelwise operations (Same data type)
	HwTestCopy(inputA, outputA);
	HwTestNot(inputA, outputA);
	HwTestAnd(inputA, inputB, outputA);
	HwTestOr(inputA, inputB, outputA);
	HwTestXor(inputA, inputB, outputA);
	HwTestMax(inputA, inputB, outputA);
	HwTestMin(inputA, inputB, outputA);
	HwTestAbsDiff(inputA, inputB, outputA);
	HwTestAdd(inputA, inputB, outputA);
	HwTestAdd(inputA, inputB, outputA);
	HwTestSub(inputA, inputB, outputA);
	HwTestMagnitude(inputA, inputB, outputA);
	HwTestMultiply(inputA, inputB, outputA);

	// Filter Functions
	TestHwBox(srcA, dstA);
	TestHwConvolve(srcA, dstA);
	TestHwDilate(srcA, dstA);
	TestHwErode(srcA, dstA);
	TestHwGaussian(srcA, dstA);
	TestHwMedian(srcA, dstA);
	TestHwScharr3x3(inputA, dstX, dstY);
	TestHwSobel(srcA, dstX, dstY);

	// Other functions
	HwConvertBitDepth(src_bitdepth, dst_bitdepth);
	HwColourConversion(src_fhd_u32, dst_fhd_u8_a);
	HwChannelCombine(src_fhd_u8_a, src_fhd_u8_b, src_fhd_u8_c, src_fhd_u8_d, dst_combine);
	HwChannelExtract(src_extract, dst_fhd_u8_a);
	HwScaleImage(src_fhd_u8_a, dst_hd_u8_a);
	HwIntegral(src_fhd_u8_a, dst_fhd_u32_a);
	HwHistogram(src_hist, dst_fhd_u32_a);
	HwTableLookup(src_lut1, src_lut2, dst_lut);	

	/******** Call Example Applications ********************************************************************/
	ExampleApplication(srcA, dstX);

	//******** Free memory ************************************************************************/
	DestroyImage<pixelwise_image_1>(inputA);
	DestroyImage<pixelwise_image_1>(inputB);
	DestroyImage<pixelwise_image_1>(outputA);
	DestroyImage<filter_u_image>(srcA);
	DestroyImage<filter_u_image>(dstA);
	DestroyImage<filter_s_image>(dstX);
	DestroyImage<filter_s_image>(dstY);
	DestroyImage<vx_uint8>(src_fhd_u8_a);
	DestroyImage<vx_uint8>(src_fhd_u8_b);
	DestroyImage<vx_uint8>(src_fhd_u8_c);
	DestroyImage<vx_uint8>(src_fhd_u8_d);
	DestroyImage<vx_uint32>(src_fhd_u32);
	DestroyImage<bit_depth_src_image>(src_bitdepth);
	DestroyImage<bit_depth_dst_image>(dst_bitdepth);
	DestroyImage<vx_uint8>(dst_fhd_u8_a);
	DestroyImage<vx_uint8>(dst_hd_u8_a);
	DestroyImage<vx_uint32>(dst_fhd_u32_a);
	DestroyImage<combine_type>(dst_combine);
	DestroyImage<extract_type>(src_extract);
	DestroyImage<hist_type>(src_hist);
	DestroyImage<lut_type>(src_lut1);
	DestroyImage<lut_type>(src_lut2);
	DestroyImage<lut_type>(dst_lut);

	std::cout << "Finished" << std::endl;
	getchar();
	return 0;
}

