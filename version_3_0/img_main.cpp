/**
 * @file    main.cpp
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
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

#include "img_pixelop_base.h"
#include "img_filter_base.h"
#include "img_conversion_base.h"
#include "img_analysis_base.h"
#include "img_feature_base.h"
#include "img_mt_feature_base.h"
#ifndef __XILINX__
#include "img_filter_test.h"
#include "img_mt_feature_test.h"
#include "img_other_test.h"
#include "img_pixelop_test.h"
#include "img_reader.h"
#endif

/***************************************************************************************************************************************************/
/* Global System configurations */
/***************************************************************************************************************************************************/

// Image Size
#define COLS_FHD 1920
#define ROWS_FHD 1080
#define PIXELS_FHD COLS_FHD *ROWS_FHD
#define COLS_HD 1280
#define ROWS_HD 720
#define PIXELS_HD COLS_HD *ROWS_HD

// Vectorization
#define VEC_NUM 1
#define VEC_PIX VECTOR_PIXELS(ROWS_FHD, COLS_FHD, VEC_NUM)
#define STEP_PIX STEP_PIXELS(ROWS_FHD, COLS_FHD, VEC_NUM, 0)

/***************************************************************************************************************************************************/
/* Accelerated Pixelwise Functions */
/***************************************************************************************************************************************************/

// Defines
#define PIXELWISE_CONV_TYPE VX_CONVERT_POLICY_WRAP   // VX_CONVERT_POLICY_SATURATE, VX_CONVERT_POLICY_WRAP
#define PIXELWISE_ROUND_TYPE VX_ROUND_POLICY_TO_ZERO // VX_ROUND_POLICY_TO_NEAREST_EVEN, VX_ROUND_POLICY_TO_ZERO
#define MULT_SCALE static_cast<vx_uint32>(0x4000)    // 0x4000=0.25 ... 0x3333=0.2
#define ALPHA static_cast<vx_uint32>(0x3333)
#define THRESH_TYPE VX_THRESHOLD_TYPE_BINARY // VX_THRESHOLD_TYPE_BINARY, VX_THRESHOLD_TYPE_RANGE
#define THRESH_VALUE static_cast<vx_int64>(50)
#define THRESH_UPPER static_cast<vx_int64>(100)
#define THRESH_LOWER static_cast<vx_int64>(20)
#define PHASE_QUANTIZATION static_cast<vx_uint8>(3)

// Typedefs
typedef vx_uint32 pixelwise_type_1;
#if (VEC_NUM == 1 && defined(__SDSCC__))
typedef pixelwise_type_1 pixelwise_image_1;
#else
typedef vx_image_data<pixelwise_type_1, VEC_NUM> pixelwise_image_1;
#endif

// Copy a data object to another.
void HwTestCopy(pixelwise_image_1 input[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgCopy<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input, output);
}

// Performs a bitwise NOT operation on a input images.
void HwTestNot(pixelwise_image_1 input[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgNot<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input, output);
}

// Performs a bitwise AND operation between two unsigned images.
void HwTestAnd(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgAnd<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Performs a bitwise INCLUSIVE OR operation between two unsigned images.
void HwTestOr(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgOr<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Performs a bitwise EXCLUSIVE OR (XOR) operation between two unsigned images.
void HwTestXor(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgXor<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Implements a pixel-wise maximum kernel.
void HwTestMax(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgMax<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Implements a pixel-wise minimum kernel.
void HwTestMin(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgMin<pixelwise_type_1, VEC_NUM, PIXELS_FHD>(input1, input2, output);
}

// Computes the absolute difference between two images
void HwTestAbsDiff(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
                   pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgAbsDiff<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Performs addition between two images
void HwTestAdd(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgAdd<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Performs subtraction between two images
void HwTestSub(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
               pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgSubtract<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE>(input1, input2, output);
}

// Implements the Gradient Magnitude Computation Kernel
void HwTestMagnitude(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
                     pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgMagnitude<pixelwise_type_1, pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_ROUND_TYPE>(input1, input2, output);
}

// Performs element-wise multiplication between two images and a scalar value
void HwTestMultiply(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
                    pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgMultiply<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE, PIXELWISE_ROUND_TYPE, MULT_SCALE>(
        input1, input2, output);
}

// Performs a multiplication by a fixed point constant value .
void HwTestMultiplyConstant(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = output
    ImgMultiplyConstant<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PIXELWISE_CONV_TYPE, PIXELWISE_ROUND_TYPE, MULT_SCALE>(
        input1, output);
}

// Weighted average value from two input images to an output image.
void HwTestWeightedAverage(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
                           pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgWeightedAverage<pixelwise_type_1, VEC_NUM, PIXELS_FHD, ALPHA, PIXELWISE_ROUND_TYPE>(input1, input2, output);
}

// Thresholds an input image and produces an output Boolean image.
void HwTestThreshold(pixelwise_image_1 input[VEC_PIX], pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgThreshold<pixelwise_type_1, VEC_NUM, PIXELS_FHD, THRESH_TYPE, THRESH_UPPER, THRESH_LOWER>(input, output);
}

// Weighted average value from two input images to an output image.
void HwTestPhase(pixelwise_image_1 input1[VEC_PIX], pixelwise_image_1 input2[VEC_PIX],
                 pixelwise_image_1 output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgPhase<pixelwise_type_1, VEC_NUM, PIXELS_FHD, PHASE_QUANTIZATION>(input1, input2, output);
}

/*********************************************************************************************************************/
// Test the pixelwise algorithms
void TestPixelwiseFunctions() {

    // Buffers for Pixelwise Functions
    pixelwise_image_1 *inputA = CreateImage<pixelwise_image_1, PIXELS_FHD, VEC_NUM>();
    pixelwise_image_1 *inputB = CreateImage<pixelwise_image_1, PIXELS_FHD, VEC_NUM>();
    pixelwise_image_1 *outputA = CreateImage<pixelwise_image_1, PIXELS_FHD, VEC_NUM>();

    // Create random data
    for (vx_uint32 i = 0; i < PIXELS_FHD; i++) {
        vx_uint8 a = rand() % 256;
        vx_uint8 b = rand() % 256;
        ((pixelwise_type_1 *)(inputA))[i] = a;
        ((pixelwise_type_1 *)(inputB))[i] = b;
    }

    // Pixelwise operations
    HwTestAbsDiff(inputA, inputB, outputA);
    HwTestAdd(inputA, inputB, outputA);
    HwTestSub(inputA, inputB, outputA);
    HwTestAnd(inputA, inputB, outputA);
    HwTestXor(inputA, inputB, outputA);
    HwTestOr(inputA, inputB, outputA);
    HwTestNot(inputA, outputA);
    HwTestCopy(inputA, outputA);
    HwTestMagnitude(inputA, inputB, outputA);
    HwTestMax(inputA, inputB, outputA);
    HwTestMin(inputA, inputB, outputA);
    HwTestPhase(inputA, inputB, outputA);
    HwTestMultiply(inputA, inputB, outputA);
    HwTestMultiplyConstant(inputA, outputA);
    HwTestThreshold(inputA, outputA);
    HwTestWeightedAverage(inputA, inputB, outputA);

    // Free memory
    DestroyImage<pixelwise_image_1>(inputA);
    DestroyImage<pixelwise_image_1>(inputB);
    DestroyImage<pixelwise_image_1>(outputA);
}

/***************************************************************************************************************************************************/
/* Accelerated Filter functions */
/***************************************************************************************************************************************************/

// Defines
#define FILTER_SIZE static_cast<vx_int32>(3)
#define FILTER_SCALE static_cast<vx_int32>(1)
#define FILTER_STEP static_cast<vx_int32>(1)
#define FILTER_BORDER static_cast<vx_border_e>(VX_BORDER_CONSTANT)
#define SEPARABLE_FILTER static_cast<vx_bool>(vx_true_e)
#define FIXED_FILTER_SIZE static_cast<vx_int32>(3)
#define HYSTERESIS_UPPER static_cast<vx_int32>(100)
#define HYSTERESIS_LOWER static_cast<vx_int32>(20)
#define NMS_QUANTIZATION static_cast<vx_uint32>(3)
#define FED_STEP_SIZE static_cast<vx_uint32>(3000)
#define CONDUCT_PRECISION static_cast<vx_hint_e>(VX_HINT_PERFORMANCE_DEFAULT)

// Typedefs
typedef vx_uint8 filter_u_type;
typedef vx_int8 filter_s_type;
typedef vx_uint32 contrast_type;

#if (VEC_NUM == 1 && defined(__SDSCC__))
typedef filter_u_type filter_u_image;
typedef filter_s_type filter_s_image;
typedef contrast_type contrast_image;
#else
typedef vx_image_data<filter_u_type, VEC_NUM> filter_u_image;
typedef vx_image_data<filter_s_type, VEC_NUM> filter_s_image;
typedef vx_image_data<contrast_type, 1> contrast_image;
#endif

// Convolution kernel for the HwConvolve filter
const filter_u_type convolve_kernel[FIXED_FILTER_SIZE][FIXED_FILTER_SIZE] = {
    {191, 185, 157}, {237, 171, 245}, {219, 235, 173}};
// Mask for Non-Max_supression. If mask entry unequal zereo, pixel will be ignored
const filter_u_type nms_mask[FIXED_FILTER_SIZE][FIXED_FILTER_SIZE] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

// Computes a Box filter over a window of the input image.
void TestHwBox(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgBox<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER, SEPARABLE_FILTER>(input, output);
}

// Convolves the input with the client supplied convolution matrix.
void TestHwConvolve(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgConvolve<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FIXED_FILTER_SIZE, FILTER_BORDER>(input, output,
                                                                                              convolve_kernel);
}

// Implements Dilation, which grows the white space in an image.
void TestHwDilate(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgDilate<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER>(input, output);
}

// Implements Erosion, which shrinks the white space in an image.
void TestHwErode(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgErode<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER>(input, output);
}

// Computes a Gaussian filter over a window of the input image.
void TestHwGaussian(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgGaussian<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER, SEPARABLE_FILTER>(input,
                                                                                                          output);
}

// Computes a Gaussian filter over a window of the input image.
void TestHwGaussian(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX], filter_u_image output_fw[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgGaussian<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER, SEPARABLE_FILTER>(input, output,
                                                                                                          output_fw);
}

// Computes a median pixel value over a window of the input image.
void TestHwMedian(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgMedian<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER>(input, output);
}

// Implements the Scharr Image Filter Kernel.
void TestHwScharr3x3(filter_u_image input[VEC_PIX], filter_s_image output1[STEP_PIX],
                     filter_s_image output2[STEP_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output1
#pragma HLS INTERFACE axis port = output2
    ImgScharr3x3<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SCALE, FILTER_STEP, FILTER_BORDER>(
        input, output1, output2);
}

// Implements the Sobel Image Filter Kernel.
#pragma SDS data access_pattern(input : SEQUENTIAL, output1 : SEQUENTIAL, output2 : SEQUENTIAL)
void TestHwSobel(filter_u_image input[VEC_PIX], filter_s_image output1[VEC_PIX], filter_s_image output2[VEC_PIX]) {
    ImgSobel<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER>(input, output1,
                                                                                                    output2);
}

// Find local maxima in an image, or otherwise suppress pixels that are not local maxima.
void TestNonMaxSupression(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgNonMaxSuppression<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FIXED_FILTER_SIZE, FILTER_BORDER>(input, output,
                                                                                                       nms_mask);
}

// Implements the segment test detector that outputs an image of response values for corners
void TestHwSegmentTestDetector(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgSegmentTestDetector<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_BORDER>(input, output);
}

// Implements the hysteres of an image
void TestHysteresis(filter_u_image input[VEC_PIX], filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgHysteresis<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, FILTER_BORDER, HYSTERESIS_LOWER,
                  HYSTERESIS_UPPER>(input, output);
}

// Find local maxima in an image depending on the phase, or otherwise suppress pixels that are not local maxima.
void TestNonMaxSuppressionOriented(filter_u_image input1[VEC_PIX], filter_u_image input2[VEC_PIX],
                                   filter_u_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgNonMaxSuppressionOriented<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_BORDER, NMS_QUANTIZATION>(
        input1, input2, output);
}

// Computes the determinant of the Hessian matrix
void TestDeterminantOfHessian(filter_s_image input1[VEC_PIX], filter_s_image input2[VEC_PIX],
                              filter_s_image output[STEP_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgDeterminantOfHessian<filter_s_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SCALE, FILTER_STEP,
                            FILTER_BORDER>(input1, input2, output);
}
void TestDeterminantOfHessian(filter_u_image input[VEC_PIX], filter_s_image output[STEP_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgDeterminantOfHessian<filter_u_type, filter_s_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SCALE, FILTER_STEP,
                            FILTER_BORDER>(input, output);
}

// Computes a FED step for given step size
void TestFastExplicitDiffusion(filter_u_image input1[VEC_PIX],    //
                               filter_u_image input2[VEC_PIX],    //
                               filter_u_image output1[VEC_PIX],   //
                               filter_u_image output2[VEC_PIX]) { //
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output1
#pragma HLS INTERFACE axis port = output2
    ImgFastExplicitDiffusion<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FED_STEP_SIZE, FILTER_BORDER>(
        input1, input2, output1, output2);
}

// Computes a FED step for given step size
void TestConductivity(filter_u_image input1[VEC_PIX], //
                      contrast_image contrast_factor[1],
                      filter_u_image output1[VEC_PIX]) { //
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = output1
#pragma HLS INTERFACE axis port = contrast_factor
    ImgConductivity<filter_u_type, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_BORDER, CONDUCT_PRECISION>(
        input1, contrast_factor, output1);
}

/*********************************************************************************************************************/
// Test the windowed algorithms
void TestFilterFunctions() {

    // Buffers for Filter Functions
    filter_u_image *srcA = CreateImage<filter_u_image, PIXELS_FHD, VEC_NUM>();
    filter_u_image *srcB = CreateImage<filter_u_image, PIXELS_FHD, VEC_NUM>();
    filter_u_image *dstA = CreateImage<filter_u_image, PIXELS_FHD, VEC_NUM>();
    filter_u_image *dstB = CreateImage<filter_u_image, PIXELS_FHD, VEC_NUM>();
    filter_s_image *srcX = CreateImage<filter_s_image, PIXELS_FHD, VEC_NUM>();
    filter_s_image *srcY = CreateImage<filter_s_image, PIXELS_FHD, VEC_NUM>();
    filter_s_image *dstX = CreateImage<filter_s_image, PIXELS_FHD, VEC_NUM>();
    filter_s_image *dstY = CreateImage<filter_s_image, PIXELS_FHD, VEC_NUM>();
    contrast_image *contrast = CreateImage<contrast_image, 1, 1>();

    // Create random data
    for (vx_uint32 i = 0; i < PIXELS_FHD; i++) {
        ((filter_u_type *)(dstA))[i] = 0;
        ((filter_u_type *)(dstB))[i] = 0;
        vx_int16 a = rand() % 256;
        vx_int16 b = rand() % 256;
        ((filter_u_type *)(srcA))[i] = (vx_uint8)a;
        ((filter_u_type *)(srcB))[i] = (vx_uint8)b;
        ((filter_u_type *)(srcX))[i] = (vx_uint8)(a - 128);
        ((filter_u_type *)(srcY))[i] = (vx_uint8)(b - 128);
    }

    // Filter Functions
    TestHwBox(srcA, dstA);
    TestHwConvolve(srcA, dstA);
    TestHwDilate(srcA, dstA);
    TestHwErode(srcA, dstA);
    TestHwGaussian(srcA, dstA);
    TestHwGaussian(srcA, dstA, dstB);
    TestHwMedian(srcA, dstA);
    TestHwScharr3x3(srcA, dstX, dstY);
    TestHwSobel(srcA, dstX, dstY);
    TestNonMaxSupression(srcA, dstA);
    TestHwSegmentTestDetector(srcA, dstA);
    TestHysteresis(srcA, dstA);
    TestNonMaxSuppressionOriented(srcA, srcB, dstA);
    TestDeterminantOfHessian(srcX, srcY, dstX);
    TestDeterminantOfHessian(srcA, dstX);
    TestFastExplicitDiffusion(srcA, srcB, dstA, dstB);
    TestConductivity(srcA, contrast, dstA);

    // Free memory
    DestroyImage<filter_u_image>(srcA);
    DestroyImage<filter_u_image>(srcB);
    DestroyImage<filter_u_image>(dstA);
    DestroyImage<filter_u_image>(dstB);
    DestroyImage<filter_s_image>(srcX);
    DestroyImage<filter_s_image>(srcY);
    DestroyImage<filter_s_image>(dstX);
    DestroyImage<filter_s_image>(dstY);
}

/***************************************************************************************************************************************************/
/* Accelerated Conversion functions */
/***************************************************************************************************************************************************/

// Defines
//#define COMBINE_AMOUNT 3 // 2, 3, 4
#define EXTRACT_CHANNEL 0             // 0, 1, 2, 3
#define CHANNEL_TYPE VX_DF_IMAGE_RGBX // VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB, VX_DF_IMAGE_RGBX
#define BIT_DEPTH_CONVERSION VX_CONVERT_POLICY_SATURATE // VX_CONVERT_POLICY_SATURATE, VX_CONVERT_POLICY_WRAP
#define BIT_DEPTH_SHIFT 8                               //
#define SCALE_INTERPOLATION VX_INTERPOLATION_AREA       // VX_INTERPOLATION_BILINEAR, VX_INTERPOLATION_NEAREST_NEIGHBOR
#define DW_CONV_SRC_PARALLEL 3                          //
#define DW_CONV_DST_PARALLEL 2                          //
#define CHANNEL_SIZE                                                                                                   \
    (CHANNEL_TYPE == VX_DF_IMAGE_RGB) ? ((PIXELS_FHD * 3) / 4) : (PIXELS_FHD) // TODO: alignement not correct
#define SCATTER_GATHER_MODE HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC           //

// Typedefs
typedef vx_uint32 channel_type;       // vx_uint16, vx_uint32
typedef vx_int16 bit_depth_src_type;  // vx_uint8, vx_int8, vx_uint16, vx_int16, vx_uint32, vx_int32
typedef vx_uint8 bit_depth_dst_type;  // vx_uint8, vx_int8, vx_uint16, vx_int16, vx_uint32, vx_int32
typedef vx_uint8 dw_conv_type;        // all
typedef vx_uint8 multicast_type;      // all
typedef vx_uint8 scatter_gather_type; // all

#ifdef __SDSCC__
typedef channel_type channel_image;
typedef vx_uint8 conversion_u8_image;
#else
typedef vx_image_data<channel_type, 1> channel_image;
typedef vx_image_data<vx_uint8, 1> conversion_u8_image;
#endif
#if (VEC_NUM == 1 && defined(__SDSCC__))
typedef bit_depth_src_type bit_depth_src_image;
typedef bit_depth_dst_type bit_depth_dst_image;
typedef multicast_type multicast_image;
typedef scatter_gather_type scatter_gather_image;
#else
typedef vx_image_data<bit_depth_src_type, VEC_NUM> bit_depth_src_image;
typedef vx_image_data<bit_depth_dst_type, VEC_NUM> bit_depth_dst_image;
typedef vx_image_data<multicast_type, VEC_NUM> multicast_image;
typedef vx_image_data<scatter_gather_type, VEC_NUM> scatter_gather_image;
#endif
#if (DW_CONV_SRC_PARALLEL == 1 && defined(__SDSCC__))
typedef dw_conv_type dw_conv_image_src;
#else
typedef vx_image_data<dw_conv_type, DW_CONV_SRC_PARALLEL> dw_conv_image_src;
#endif
#if (DW_CONV_DST_PARALLEL == 1 && defined(__SDSCC__))
typedef dw_conv_type dw_conv_image_dst;
#else
typedef vx_image_data<dw_conv_type, DW_CONV_DST_PARALLEL> dw_conv_image_dst;
#endif

// Implements the Channel Combine Kernel.
void HwChannelCombine(conversion_u8_image input1[PIXELS_FHD], //
                      conversion_u8_image input2[PIXELS_FHD], //
                      conversion_u8_image input3[PIXELS_FHD], //
                      // conversion_u8_image input4[PIXELS_FHD], //
                      channel_image output[CHANNEL_SIZE]) { //
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = input3
#pragma HLS INTERFACE axis port = input4
#pragma HLS INTERFACE axis port = output
    ImgChannelCombine<channel_type, PIXELS_FHD, CHANNEL_SIZE, CHANNEL_TYPE>(input1, input2, input3, output);
}

//  Implements the Channel Extraction Kernel.
void HwChannelExtract(channel_image input[CHANNEL_SIZE], conversion_u8_image output[PIXELS_FHD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgChannelExtract<channel_type, EXTRACT_CHANNEL, CHANNEL_SIZE, PIXELS_FHD, CHANNEL_TYPE>(input, output);
}

// Converts the Color of an image between RGB/RGBX/Gray
void HwColourConversion(channel_image input[CHANNEL_SIZE], conversion_u8_image output[PIXELS_FHD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgConvertColor<channel_type, vx_uint8, CHANNEL_SIZE, PIXELS_FHD, CHANNEL_TYPE, VX_DF_IMAGE_U8>(input, output);
}

// Converts image bit depth.
void HwConvertBitDepth(bit_depth_src_image input[VEC_PIX], bit_depth_dst_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgConvertBitDepth<bit_depth_src_type, bit_depth_dst_type, VEC_NUM, PIXELS_FHD, BIT_DEPTH_CONVERSION,
                       BIT_DEPTH_SHIFT>(input, output);
}

// Scale an image down using bilinear or nearest neighbor interpolation
void HwScaleImage(conversion_u8_image input[PIXELS_FHD], conversion_u8_image output[PIXELS_HD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgScaleImage<COLS_FHD, ROWS_FHD, COLS_HD, ROWS_HD, SCALE_INTERPOLATION>(input, output);
}

//  Converts converts between two buffers with a different vector sizes (parallelization degree). If needed: bigger
//  image needs to be aligned.
void HwDataWidthConverter(dw_conv_image_src input[PIXELS_FHD / DW_CONV_SRC_PARALLEL],
                          dw_conv_image_dst output[PIXELS_FHD / DW_CONV_DST_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgDataWidthConverter<dw_conv_type, 1, PIXELS_FHD, PIXELS_FHD, DW_CONV_SRC_PARALLEL, DW_CONV_DST_PARALLEL>(input,
                                                                                                               output);
}

// Copies the input image to multiple output images
void HwMulticast(multicast_image input[VEC_PIX], multicast_image output1[VEC_PIX], multicast_image output2[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output1
#pragma HLS INTERFACE axis port = output2
    ImgMulticast<multicast_type, PIXELS_FHD, VEC_NUM>(input, output1, output2);
}

void HwGather(scatter_gather_image input1[VEC_PIX / 2], scatter_gather_image input2[VEC_PIX / 2],
              scatter_gather_image output[VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgGather<scatter_gather_type, PIXELS_FHD, VEC_NUM, SCATTER_GATHER_MODE>(input1, input2, output);
}

void HwScatter(scatter_gather_image input[VEC_PIX], scatter_gather_image output1[VEC_PIX / 2],
               scatter_gather_image output2[VEC_PIX / 2]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output1
#pragma HLS INTERFACE axis port = output2
    ImgScatter<scatter_gather_type, PIXELS_FHD, VEC_NUM, SCATTER_GATHER_MODE>(input, output1, output2);
}

/*********************************************************************************************************************/
// Test the conversion algorithms
void TestConversionFunctions() {

    // Allocate memory
    conversion_u8_image *src_fhd_u8_a = CreateImage<conversion_u8_image, PIXELS_FHD, 1>();
    conversion_u8_image *src_fhd_u8_b = CreateImage<conversion_u8_image, PIXELS_FHD, 1>();
    conversion_u8_image *src_fhd_u8_c = CreateImage<conversion_u8_image, PIXELS_FHD, 1>();
    conversion_u8_image *src_fhd_u8_d = CreateImage<conversion_u8_image, PIXELS_FHD, 1>();
    conversion_u8_image *dst_fhd_u8_a = CreateImage<conversion_u8_image, PIXELS_FHD, 1>();
    conversion_u8_image *dst_hd_u8_a = CreateImage<conversion_u8_image, PIXELS_HD, 1>();
    channel_image *src_channel = CreateImage<channel_image, CHANNEL_SIZE, 1>();
    channel_image *dst_channel = CreateImage<channel_image, PIXELS_FHD, 1>();
    bit_depth_src_image *src_bitdepth = CreateImage<bit_depth_src_image, PIXELS_FHD, VEC_NUM>();
    bit_depth_dst_image *dst_bitdepth = CreateImage<bit_depth_dst_image, PIXELS_FHD, VEC_NUM>();
    dw_conv_image_src *src_dw_conv = CreateImage<dw_conv_image_src, PIXELS_FHD, DW_CONV_SRC_PARALLEL>();
    dw_conv_image_dst *dst_dw_conv = CreateImage<dw_conv_image_dst, PIXELS_FHD, DW_CONV_DST_PARALLEL>();
    multicast_image *src_multicast = CreateImage<multicast_image, PIXELS_FHD, VEC_NUM>();
    multicast_image *dst1_multicast = CreateImage<multicast_image, PIXELS_FHD, VEC_NUM>();
    multicast_image *dst2_multicast = CreateImage<multicast_image, PIXELS_FHD, VEC_NUM>();
    scatter_gather_image *src1_gather = CreateImage<scatter_gather_image, PIXELS_FHD / 2, VEC_NUM>();
    scatter_gather_image *src2_gather = CreateImage<scatter_gather_image, PIXELS_FHD / 2, VEC_NUM>();
    scatter_gather_image *dst1_gather = CreateImage<scatter_gather_image, PIXELS_FHD, VEC_NUM>();
    scatter_gather_image *src1_scatter = CreateImage<scatter_gather_image, PIXELS_FHD, VEC_NUM>();
    scatter_gather_image *dst1_scatter = CreateImage<scatter_gather_image, PIXELS_FHD / 2, VEC_NUM>();
    scatter_gather_image *dst2_scatter = CreateImage<scatter_gather_image, PIXELS_FHD / 2, VEC_NUM>();

    // Create random data
    for (vx_uint32 i = 0; i < PIXELS_FHD; i++) {
        vx_uint8 a = rand() % 256;
        vx_uint8 b = rand() % 256;
        vx_uint8 c = rand() % 256;
        vx_uint8 d = rand() % 256;
        ((vx_uint8 *)(src_fhd_u8_a))[i] = a;
        ((vx_uint8 *)(src_fhd_u8_b))[i] = b;
        ((vx_uint8 *)(src_fhd_u8_c))[i] = c;
        ((vx_uint8 *)(src_fhd_u8_d))[i] = d;
        ((bit_depth_src_type *)(src_bitdepth))[i] = a;
        ((dw_conv_type *)(src_dw_conv))[i] = d;
        if (i < CHANNEL_SIZE)
            ((channel_type *)(src_channel))[i] = a;
        ((multicast_type *)(src_multicast))[i] = a;
        if (i < (PIXELS_FHD / 2)) {
            ((scatter_gather_type *)(src1_gather))[i] = a;
            ((scatter_gather_type *)(src2_gather))[i] = b;
        }
        ((scatter_gather_type *)(src1_scatter))[i] = a;
    }

    // Compute
    HwChannelCombine(src_fhd_u8_a, src_fhd_u8_b, src_fhd_u8_c, dst_channel); // src_fhd_u8_d,
    HwChannelExtract(src_channel, dst_fhd_u8_a);
    HwColourConversion(src_channel, dst_fhd_u8_a);
    HwConvertBitDepth(src_bitdepth, dst_bitdepth);
    HwDataWidthConverter(src_dw_conv, dst_dw_conv);
    HwScaleImage(src_fhd_u8_a, dst_hd_u8_a);
    HwMulticast(src_multicast, dst1_multicast, dst2_multicast);
    HwGather(src1_gather, src2_gather, dst1_gather);
    HwScatter(src1_scatter, dst1_scatter, dst2_scatter);

    // Free memory
    DestroyImage<conversion_u8_image>(src_fhd_u8_a);
    DestroyImage<conversion_u8_image>(src_fhd_u8_b);
    DestroyImage<conversion_u8_image>(src_fhd_u8_c);
    DestroyImage<conversion_u8_image>(src_fhd_u8_d);
    DestroyImage<conversion_u8_image>(dst_fhd_u8_a);
    DestroyImage<conversion_u8_image>(dst_hd_u8_a);
    DestroyImage<channel_image>(src_channel);
    DestroyImage<channel_image>(dst_channel);
    DestroyImage<bit_depth_src_image>(src_bitdepth);
    DestroyImage<bit_depth_dst_image>(dst_bitdepth);
    DestroyImage<dw_conv_image_src>(src_dw_conv);
    DestroyImage<dw_conv_image_dst>(dst_dw_conv);
    DestroyImage<multicast_image>(src_multicast);
    DestroyImage<multicast_image>(dst1_multicast);
    DestroyImage<multicast_image>(dst2_multicast);
    DestroyImage<scatter_gather_image>(src1_gather);
    DestroyImage<scatter_gather_image>(src2_gather);
    DestroyImage<scatter_gather_image>(dst1_gather);
    DestroyImage<scatter_gather_image>(src1_scatter);
    DestroyImage<scatter_gather_image>(dst1_scatter);
    DestroyImage<scatter_gather_image>(dst2_scatter);
}

/***************************************************************************************************************************************************/
/* Accelerated Analysis functions */
/***************************************************************************************************************************************************/

// Defines
#define CONTROL_FLOW_OP (vx_scalar_operation_e) VX_SCALAR_OP_GREATER
#define HIST_BINS 256
#define HIST_OFFSET 0
#define HIST_RANGE (1 << 8) // (1 << 8), (1 << 16)
#define MIN_MAX_CAPACITY 32
#define CONTRAST_PERCENTILE 45875 // 0.7
#define CONTRAST_BORDER 3

// Typedefs
typedef vx_uint8 controlflow_src_type; // all
typedef vx_uint8 controlflow_dst_type; // all
typedef vx_uint8 hist_type;            // vx_uint8, vx_uint16
typedef vx_uint8 analysis_type;        // vx_uint8, vx_uint16, vx_int8, vx_int16
typedef vx_uint16 contrast_dst_type;   // vx_uint16
#if (VEC_NUM == 1 && defined(__SDSCC__))
typedef vx_uint8 analysis_u8_image;
typedef vx_uint16 analysis_u16_image;
typedef vx_uint32 analysis_u32_image;
typedef vx_float32 analysis_f32_image;
typedef controlflow_src_type controlflow_src_image;
typedef controlflow_dst_type controlflow_dst_image;
typedef hist_type hist_image;
typedef analysis_type analysis_image;
typedef contrast_dst_type contrast_dst_image;
#else
typedef vx_image_data<vx_uint8, 1> analysis_u8_image;
typedef vx_image_data<vx_uint16, 1> analysis_u16_image;
typedef vx_image_data<vx_uint32, 1> analysis_u32_image;
typedef vx_image_data<vx_float32, 1> analysis_f32_image;
typedef vx_image_data<controlflow_src_type, 1> controlflow_src_image;
typedef vx_image_data<controlflow_dst_type, 1> controlflow_dst_image;
typedef vx_image_data<hist_type, VEC_NUM> hist_image;
typedef vx_image_data<analysis_type, 1> analysis_image;
typedef vx_image_data<contrast_dst_type, 1> contrast_dst_image;
#endif
typedef vx_image_data<vx_uint16, 2> coordinates_image;

// These features allow for conditional graph flow in OpenVX, via support for a variety of operations between two
// scalars.
void HwScalarOperation(controlflow_src_image a[1], controlflow_src_image b[1], controlflow_dst_image output[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = a
#pragma HLS INTERFACE axis port = b
#pragma HLS INTERFACE axis port = output
    ImgControlFlow<controlflow_src_type, controlflow_dst_type, CONTROL_FLOW_OP>(a, b, output);
}

// Equalizes the histogram of a grayscale image. The output image dimensions should be the same as the dimensions of the
// input image.
void HwEqualizeHist(analysis_u8_image input1[PIXELS_FHD], analysis_u8_image input2[PIXELS_FHD],
                    analysis_u32_image output[PIXELS_FHD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = output
    ImgEqualizeHist<PIXELS_FHD>(input1, input2, output);
}

// Generates a distribution from an image.
void HwHistogram(hist_image input[VEC_PIX], analysis_u32_image output[HIST_BINS]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgHistogram<hist_type, COLS_FHD, ROWS_FHD, VEC_NUM, HIST_BINS, HIST_RANGE, HIST_OFFSET>(input, output);
}

void HwContrastFactor(hist_image input[PIXELS_FHD], contrast_dst_image output[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgContrastFactor<hist_type, COLS_FHD, ROWS_FHD, VEC_NUM, CONTRAST_PERCENTILE, CONTRAST_BORDER>(input, output);
}

// Computes the integral image of the input.
void HwIntegral(analysis_u8_image input[PIXELS_FHD], analysis_u32_image output[PIXELS_FHD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgIntegral<COLS_FHD, ROWS_FHD>(input, output);
}

// Computes the mean pixel value and the standard deviation of the pixels in the input image (which has a dimension
// width and height).
void HwMeanStdDev1(analysis_image input1[PIXELS_FHD], analysis_f32_image mean[1], analysis_image input2[PIXELS_FHD],
                   analysis_f32_image stddev[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = mean
#pragma HLS INTERFACE axis port = input2
#pragma HLS INTERFACE axis port = stddev
    ImgMeanStdDev<analysis_type, COLS_FHD, ROWS_FHD>(input1, input2, mean, stddev);
}

// Computes the mean pixel value and the standard deviation of the pixels in the input image (which has a dimension
// width and height).
void HwMeanStdDev2(analysis_image input1[PIXELS_FHD], analysis_f32_image mean[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input1
#pragma HLS INTERFACE axis port = mean
    ImgMeanStdDev<analysis_type, COLS_FHD, ROWS_FHD>(input1, mean);
}

// Finds the minimum and maximum values in an image and a location for each. If the input image has several
// minimums/maximums, the kernel returns all of them.
void HwMinMaxLocation1(analysis_image input[PIXELS_FHD], analysis_image min_val[1], analysis_image max_val[1],
                       coordinates_image min_loc[MIN_MAX_CAPACITY], coordinates_image max_loc[MIN_MAX_CAPACITY],
                       analysis_u16_image min_count[1], analysis_u16_image max_count[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = min_val
#pragma HLS INTERFACE axis port = max_val
#pragma HLS INTERFACE axis port = min_loc
#pragma HLS INTERFACE axis port = max_loc
#pragma HLS INTERFACE axis port = min_count
#pragma HLS INTERFACE axis port = max_count
    ImgMinMaxLocation<analysis_type, COLS_FHD, ROWS_FHD, MIN_MAX_CAPACITY>(input, min_val, max_val, min_loc, max_loc,
                                                                           min_count, max_count);
}

// Finds the minimum and maximum values in an image and a location for each. If the input image has several
// minimums/maximums, the kernel returns all of them.
void HwMinMaxLocation2(analysis_image input[PIXELS_FHD], analysis_image min_val[1], analysis_image max_val[1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = min_val
#pragma HLS INTERFACE axis port = max_val
    ImgMinMaxLocation<analysis_type, COLS_FHD, ROWS_FHD>(input, min_val, max_val);
}

// Implements the Table Lookup Image Kernel.
void HwTableLookup(hist_image input[PIXELS_FHD], hist_image lut[HIST_BINS], hist_image output[PIXELS_FHD]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = lut
#pragma HLS INTERFACE axis port = output
    ImgTableLookup<hist_type, PIXELS_FHD, HIST_BINS, HIST_OFFSET>(input, lut, output);
}

/*********************************************************************************************************************/
// Test the conversion algorithms
void TestAnalysisFunctions() {

    // Allocate memory
    controlflow_src_image *src1_controlflow = CreateImage<controlflow_src_image, 1, 1>();
    controlflow_src_image *src2_controlflow = CreateImage<controlflow_src_image, 1, 1>();
    controlflow_dst_image *dst1_controlflow = CreateImage<controlflow_dst_image, 1, 1>();
    analysis_u8_image *src1_analysis_u8 = CreateImage<analysis_u8_image, PIXELS_FHD, 1>();
    analysis_u8_image *src2_analysis_u8 = CreateImage<analysis_u8_image, PIXELS_FHD, 1>();
    hist_image *src0_histogram = CreateImage<hist_image, PIXELS_FHD, VEC_NUM>();
    hist_image *src1_histogram = CreateImage<hist_image, PIXELS_FHD, 1>();
    hist_image *src2_histogram = CreateImage<hist_image, HIST_BINS, 1>();
    hist_image *dst1_histogram = CreateImage<hist_image, PIXELS_FHD, 1>();
    analysis_u32_image *dst1_analysis_u32 = CreateImage<analysis_u32_image, PIXELS_FHD, 1>();
    analysis_u32_image *dst2_analysis_u32 = CreateImage<analysis_u32_image, HIST_BINS, 1>();
    analysis_image *src1_analysis = CreateImage<analysis_image, PIXELS_FHD, 1>();
    analysis_image *dst1_analysis = CreateImage<analysis_image, 1, 1>();
    analysis_image *dst2_analysis = CreateImage<analysis_image, 1, 1>();
    coordinates_image *dst1_coordinates = CreateImage<coordinates_image, MIN_MAX_CAPACITY, 1>();
    coordinates_image *dst2_coordinates = CreateImage<coordinates_image, MIN_MAX_CAPACITY, 1>();
    analysis_u16_image *dst1_analysis_u16 = CreateImage<analysis_u16_image, MIN_MAX_CAPACITY, 1>();
    analysis_u16_image *dst2_analysis_u16 = CreateImage<analysis_u16_image, MIN_MAX_CAPACITY, 1>();
    analysis_f32_image *dst1_analysis_f32 = CreateImage<analysis_f32_image, 1, 1>();
    analysis_f32_image *dst2_analysis_f32 = CreateImage<analysis_f32_image, 1, 1>();
    contrast_dst_image *dst1_contrast_u16 = CreateImage<contrast_dst_image, 1, 1>();

    // Create random data
    for (vx_uint32 i = 0; i < PIXELS_FHD; i++) {
        vx_uint8 a = rand() % 256;
        vx_uint8 b = rand() % 256;
        if (i == 0) {
            ((controlflow_src_type *)(src1_controlflow))[i] = a;
            ((controlflow_src_type *)(src2_controlflow))[i] = b;
        }
        ((vx_uint8 *)(src1_analysis_u8))[i] = a;
        ((vx_uint8 *)(src2_analysis_u8))[i] = b;
        ((hist_type *)(src0_histogram))[i] = a;
        ((hist_type *)(src1_histogram))[i] = a;
        if (i < HIST_BINS) {
            ((hist_type *)(src2_histogram))[i] = b;
        }
    }

    // Compute
    HwScalarOperation(src1_controlflow, src2_controlflow, dst1_controlflow);
    HwEqualizeHist(src1_analysis_u8, src1_analysis_u8, dst1_analysis_u32);
    HwHistogram(src0_histogram, dst2_analysis_u32);
    HwContrastFactor(src0_histogram, dst1_contrast_u16);
    HwIntegral(src1_analysis_u8, dst1_analysis_u32);
    HwMeanStdDev1(src1_analysis, dst1_analysis_f32, src1_analysis, dst2_analysis_f32);
    HwMeanStdDev2(src1_analysis, dst1_analysis_f32);
    HwMinMaxLocation1(src1_analysis, dst1_analysis, dst2_analysis, dst1_coordinates, dst2_coordinates,
                      dst1_analysis_u16, dst2_analysis_u16);
    HwMinMaxLocation2(src1_analysis, dst1_analysis, dst2_analysis);
    HwTableLookup(src1_histogram, src2_histogram, dst1_histogram);

    // Free memory
    DestroyImage<controlflow_src_image>(src1_controlflow);
    DestroyImage<controlflow_src_image>(src2_controlflow);
    DestroyImage<controlflow_dst_image>(dst1_controlflow);
    DestroyImage<analysis_u8_image>(src1_analysis_u8);
    DestroyImage<analysis_u8_image>(src2_analysis_u8);
    DestroyImage<hist_image>(src1_histogram);
    DestroyImage<hist_image>(src2_histogram);
    DestroyImage<hist_image>(dst1_histogram);
    DestroyImage<analysis_u32_image>(dst1_analysis_u32);
    DestroyImage<analysis_u32_image>(dst2_analysis_u32);
    DestroyImage<analysis_image>(src1_analysis);
    DestroyImage<analysis_image>(dst1_analysis);
    DestroyImage<analysis_image>(dst2_analysis);
    DestroyImage<coordinates_image>(dst1_coordinates);
    DestroyImage<coordinates_image>(dst2_coordinates);
    DestroyImage<analysis_u16_image>(dst1_analysis_u16);
    DestroyImage<analysis_u16_image>(dst2_analysis_u16);
    DestroyImage<analysis_f32_image>(dst1_analysis_f32);
    DestroyImage<analysis_f32_image>(dst2_analysis_f32);
}

/***************************************************************************************************************************************************/
/* Accelerated Feature Detection functions */
/***************************************************************************************************************************************************/

/***************************************************************************************************************************************************/
/* Example Applications. (including loop level parallelism) */
/***************************************************************************************************************************************************/

/*********************************************************/
// Image Structure & Data Types
const vx_uint32 APP_COLS = 850;
const vx_uint32 APP_ROWS = 680;
const vx_uint32 APP_PIXELS = APP_COLS * APP_ROWS;
const vx_uint32 APP_VEC_NUM = 1;
const vx_uint32 APP_VEC_PIX = APP_PIXELS / APP_VEC_NUM;
const vx_border_e APP_BORDER_TYPE = VX_BORDER_CONSTANT;
const vx_uint32 APP_GAUSSIAN_KERNEL = 7;
typedef vx_uint8 app_u_type;
typedef vx_int8 app_s_type;

#if (APP_VEC_NUM == 1 && defined(__SDSCC__))
typedef app_u_type app_u_image;
typedef app_s_type app_s_image;
#else
typedef vx_image_data<app_u_type, APP_VEC_NUM> app_u_image;
typedef vx_image_data<app_s_type, APP_VEC_NUM> app_s_image;
#endif

/*********************************************************/
// 5x5 Median Filter
#pragma SDS data access_pattern(input : SEQUENTIAL, output : SEQUENTIAL)
void ExampleApp0(app_u_image input[APP_VEC_PIX], app_u_image output[APP_VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgMedian<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, 5, APP_BORDER_TYPE>(input, output);
}

/*********************************************************/
// 7x7 Gaussian Filter
#pragma SDS data access_pattern(input : SEQUENTIAL, output : SEQUENTIAL)
void ExampleApp1(app_u_image input[APP_VEC_PIX], app_u_image output[APP_VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
    ImgGaussian<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, APP_GAUSSIAN_KERNEL, APP_BORDER_TYPE, vx_true_e>(input,
                                                                                                              output);
}

/*********************************************************/
// Smoothed Gradient Magnitude
#pragma SDS data access_pattern(input : SEQUENTIAL, output : SEQUENTIAL)
void ExampleApp2(app_u_image input[APP_VEC_PIX], app_u_image output[APP_VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output

    // FIFOs to stream data between functions
#ifdef __XILINX__
    app_u_image ls[APP_VEC_PIX];
#pragma HLS STREAM variable = ls depth = 16
#pragma HLS data_pack variable = ls
    app_s_image lx[APP_VEC_PIX];
#pragma HLS STREAM variable = lx depth = 16
#pragma HLS data_pack variable = lx
    app_s_image ly[APP_VEC_PIX];
#pragma HLS STREAM variable = ly depth = 16
#pragma HLS data_pack variable = ly
#else
    app_u_image *ls = new app_u_image[APP_VEC_PIX];
    app_s_image *lx = new app_s_image[APP_VEC_PIX];
    app_s_image *ly = new app_s_image[APP_VEC_PIX];
#endif

    // Computation
#pragma HLS DATAFLOW
    ImgGaussian<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, APP_GAUSSIAN_KERNEL, APP_BORDER_TYPE, vx_true_e>(input,
                                                                                                              ls);
    ImgSobel<app_u_type, app_s_type, APP_VEC_NUM, APP_COLS, APP_ROWS, 3, APP_BORDER_TYPE>(ls, lx, ly);
    ImgMagnitude<app_s_type, app_u_type, APP_VEC_NUM, APP_PIXELS, VX_ROUND_POLICY_TO_ZERO>(lx, ly, output);

    // Free Memory
#ifndef __XILINX__
    delete[] ls;
    delete[] lx;
    delete[] ly;
#endif
}

/*********************************************************/
// Edge Detector
#pragma SDS data access_pattern(input : SEQUENTIAL, output : SEQUENTIAL)
void ExampleApp3(app_u_image input[APP_VEC_PIX], app_u_image output[APP_VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output

    // FIFOs to stream data between functions
#ifdef __XILINX__
    app_s_image lx[APP_VEC_PIX];
#pragma HLS STREAM variable = lx depth = 16
#pragma HLS data_pack variable = lx
    app_s_image ly[APP_VEC_PIX];
#pragma HLS STREAM variable = ly depth = 16
#pragma HLS data_pack variable = ly
    app_u_image lm[APP_VEC_PIX];
#pragma HLS STREAM variable = lm depth = 16
#pragma HLS data_pack variable = lm
#else
    //	app_u_image* ls = new app_u_image[APP_VEC_PIX];
    app_s_image *lx = new app_s_image[APP_VEC_PIX];
    app_s_image *ly = new app_s_image[APP_VEC_PIX];
    app_u_image *lm = new app_u_image[APP_VEC_PIX];
#endif

    // Computation
#pragma HLS DATAFLOW
    ImgSobel<app_u_type, app_s_type, APP_VEC_NUM, APP_COLS, APP_ROWS, 3, APP_BORDER_TYPE>(input, lx, ly);
    ImgMagnitude<app_s_type, app_u_type, APP_VEC_NUM, APP_PIXELS, VX_ROUND_POLICY_TO_ZERO>(lx, ly, lm);
    ImgHysteresis<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, 3, APP_BORDER_TYPE, 25, 35>(lm, output);

    // Free Memory
#ifndef __XILINX__
    delete[] lx;
    delete[] ly;
    delete[] lm;
#endif
}

/*********************************************************/
// Corner Detector
#pragma SDS data access_pattern(input : SEQUENTIAL, output : SEQUENTIAL)
void ExampleApp4(app_u_image input[APP_VEC_PIX], app_u_image output[APP_VEC_PIX]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port = return
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
#endif

    // FIFOs to stream data between functions
#ifdef __XILINX__
    app_u_image lf[APP_VEC_PIX];
#pragma HLS STREAM variable = lf depth = 1
#pragma HLS data_pack variable = lf
#else
    app_u_image *lf = new app_u_image[APP_VEC_PIX];
#endif

    // Computation
#pragma HLS DATAFLOW
    ImgSegmentTestDetector<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, APP_BORDER_TYPE>(input, lf);
    ImgHysteresis<app_u_type, APP_VEC_NUM, APP_COLS, APP_ROWS, 3, APP_BORDER_TYPE, 10, 40>(lf, output);

    // Free Memory
#ifndef __XILINX__
    delete[] lf;
#endif
}

/*********************************************************/
// Test all example applications with an image
void TestExampleApplications() {
#ifndef __XILINX__

    // Create data structures
    struct PgmImage *image1 = new PgmImage;
    struct PgmImage *image2 = new PgmImage;

    // Allocate
    image1->matrix = new app_u_type[APP_COLS * APP_ROWS];
    image2->matrix = new app_u_type[APP_COLS * APP_ROWS];

    // Read input (uses boat image from the oxford dataset)
    if (APP_COLS == 850 && APP_ROWS == 680)
        readPGM("../../Source/Library/hiflipvx/data/img1.pgm", image1);

    // Set image data
    CopyImageMetaData(image1, image2);

    // Test all applications and write output to file
    for (vx_uint16 i = 0; i < 5; ++i) {
        if (i == 0) {
            ExampleApp0((app_u_image *)(image1->matrix), (app_u_image *)(image2->matrix));
            if (APP_COLS == 850 && APP_ROWS == 680)
                writePGM("out1.pgm", image2);
        } else if (i == 1) {
            ExampleApp1((app_u_image *)(image1->matrix), (app_u_image *)(image2->matrix));
            if (APP_COLS == 850 && APP_ROWS == 680)
                writePGM("out2.pgm", image2);
        } else if (i == 2) {
            ExampleApp2((app_u_image *)(image1->matrix), (app_u_image *)(image2->matrix));
            if (APP_COLS == 850 && APP_ROWS == 680)
                writePGM("out3.pgm", image2);
        } else if (i == 3) {
            ExampleApp3((app_u_image *)(image1->matrix), (app_u_image *)(image2->matrix));
            if (APP_COLS == 850 && APP_ROWS == 680)
                writePGM("out4.pgm", image2);
        } else if (i == 4) {
            ExampleApp4((app_u_image *)(image1->matrix), (app_u_image *)(image2->matrix));
            if (APP_COLS == 850 && APP_ROWS == 680)
                writePGM("out5.pgm", image2);
        }
    }

    // Deallocate
    delete[] image1->matrix;
    delete[] image2->matrix;
#endif
}

/*********************************************************************************************************************/
/* Main function */
/*********************************************************************************************************************/
int main(void) {

    // Verification
#ifndef __SDSCC__
    // swTestOtherMain();
    // SwTestPixelopMain();
    // TestFilterMain();
    // TestImgMtFeature();
#endif

    // Simple testing
     TestPixelwiseFunctions();
     TestFilterFunctions();
     TestConversionFunctions();
     TestAnalysisFunctions();
     TestExampleApplications();

    printf("Finished\n");
    return 0;
}
