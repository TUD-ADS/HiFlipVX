/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_filter_base.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are all accelerated image filter functions (Call from here)
 */

#ifndef SRC_IMG_FILTER_BASE_H_
#define SRC_IMG_FILTER_BASE_H_

#include "img_filter_core.h"

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>             //
void ImgBox(                                                                          //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwBox<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM,         //
          WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>(input, output); //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>         //
void ImgBox(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                //
            DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {             //
#pragma HLS INLINE
    HwBox<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
          WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE>(        //
        (vx_image_data<DataType, VEC_NUM> *)input,                  //
        (vx_image_data<DataType, VEC_NUM> *)output);                //
}

/**********************************************************************************************************************/
/** @brief  Convolves the input with the client supplied convolution matrix.
@param DataType    Data type of the input/output image (unsigned, signed)(8-, 16- or 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   Kernel size (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param conv        The custom convolution kernel
*/
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,   //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                              //
void ImgConvolve(                                                                   //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],  //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)], //
    const DataType conv[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwConvolve<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM,    //
               WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output, conv); //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                            //
void ImgConvolve(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],           //
                 DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],          //
                 const DataType conv[KERN_SIZE][KERN_SIZE]) {                     //
#pragma HLS INLINE
    HwConvolve<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
               WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                   //
        (vx_image_data<DataType, VEC_NUM> *)input,                       //
        (vx_image_data<DataType, VEC_NUM> *)output, conv);               //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                                //
void ImgDilate(                                                                       //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwDilate<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
             WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);    //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                            //
void ImgDilate(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],             //
               DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {          //
#pragma HLS INLINE
    HwDilate<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
             WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                   //
        (vx_image_data<DataType, VEC_NUM> *)input,                     //
        (vx_image_data<DataType, VEC_NUM> *)output);                   //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                                //
void ImgErode(                                                                        //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwErode<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
            WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);    //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                            //
void ImgErode(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],              //
              DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {           //
#pragma HLS INLINE
    HwErode<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
            WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                   //
        (vx_image_data<DataType, VEC_NUM> *)input,                    //
        (vx_image_data<DataType, VEC_NUM> *)output);                  //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>             //
void ImgGaussian(                                                                     //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH,             //
               HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE, vx_false_e>(input, output, NULL); //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>         //
void ImgGaussian(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],           //
                 DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {        //
#pragma HLS INLINE
    HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, //
               HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE, vx_false_e>(          //
        (vx_image_data<DataType, VEC_NUM> *)input,                              //
        (vx_image_data<DataType, VEC_NUM> *)output,                             //
        NULL);                                                                  //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,        //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>                //
void ImgGaussian(                                                                        //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],       //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],      //
    vx_image_data<DataType, VEC_NUM> output_fw[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output, output_fw);
    HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH,                 //
               HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE, vx_true_e>(input, output, output_fw); //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE, vx_bool SEPARABLE>         //
void ImgGaussian(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],           //
                 DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],          //
                 DataType output_fw[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {     //
#pragma HLS INLINE
    HwGaussian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, WIDTH, //
               HEIGHT, KERN_SIZE, BORDER_TYPE, SEPARABLE, vx_true_e>(           //
        (vx_image_data<DataType, VEC_NUM> *)input,                              //
        (vx_image_data<DataType, VEC_NUM> *)output,                             //
        (vx_image_data<DataType, VEC_NUM> *)output_fw);                         //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                                //
void ImgMedian(                                                                       //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwMedian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
             WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output);    //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                            //
void ImgMedian(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],             //
               DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {          //
#pragma HLS INLINE
    HwMedian<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
             WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                   //
        (vx_image_data<DataType, VEC_NUM> *)input,                     //
        (vx_image_data<DataType, VEC_NUM> *)output);                   //
}

/**********************************************************************************************************************/
/**
 * @brief Computes the first order derivatives in respect to x and y using 3x3 Scharr filter.
 * @tparam InType       Data type of the input image(unsigned)(8 - bit, 16 - bit)
 * @tparam OutType      Data type of the output image(signed)(8 - bit, 16 - bit)
 * @tparam VEC_NUM      Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam WIDTH        Image width
 * @tparam HEIGHT       Image height
 * @tparam SCALE        Kernel scale factor (new kernel size )
 * @tparam STEP         Number of pixels to be skipped
 * @tparam BORDER_TYPE  Type of border(Constant, Replicated or Undefined)
 * @param input         Input image
 * @param output_lx     First order derivatives in respect to x
 * @param output_ly     First order derivatives in respect to y
 */
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH,                                     //
          vx_uint16 HEIGHT, vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                                 //
void ImgScharr3x3(                                                                                                  //
    vx_image_data<InType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                                    //
    vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> output_lx[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)],   //
    vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> output_ly[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output_lx, output_ly);
    HwScalableScharr3x3<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), //
                        STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP), VEC_NUM,     //
                        STEP_VEC_NUM_OUT(VEC_NUM, STEP), WIDTH, HEIGHT,         //
                        SCALE, STEP, BORDER_TYPE>(input, output_lx, output_ly); //
}
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH,     //
          vx_uint16 HEIGHT, vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE> //
void ImgScharr3x3(InType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],              //
                  OutType output_lx[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)],     //
                  OutType output_ly[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) {   //
#pragma HLS INLINE
    HwScalableScharr3x3<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), //
                        STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP), VEC_NUM,     //
                        STEP_VEC_NUM_OUT(VEC_NUM, STEP), WIDTH, HEIGHT,         //
                        SCALE, STEP, BORDER_TYPE>(                              //
        (vx_image_data<InType, VEC_NUM> *)input,                                //
        (vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> *)output_lx,   //
        (vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> *)output_ly);  //
}

/**********************************************************************************************************************/
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
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM,                           //
          vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE> //
void ImgSobel(                                                                            //
    vx_image_data<SrcType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],         //
    vx_image_data<DstType, VEC_NUM> output1[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],       //
    vx_image_data<DstType, VEC_NUM> output2[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {     //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output1, output2);
    HwSobel<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
            WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output1, output2);  //
}
template <typename SrcType, typename DstType, vx_uint8 VEC_NUM,                           //
          vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE> //
void ImgSobel(SrcType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                       //
              DstType output1[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                     //
              DstType output2[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {                   //
#pragma HLS INLINE
    HwSobel<SrcType, DstType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
            WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                           //
        (vx_image_data<SrcType, VEC_NUM> *)input,                             //
        (vx_image_data<DstType, VEC_NUM> *)output1,                           //
        (vx_image_data<DstType, VEC_NUM> *)output2);                          //
}

/**********************************************************************************************************************/
/** @brief  Find local maxima in an image, or otherwise suppress pixels that are not local maxima.
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param KERN_SIZE   The size of window over which to perform the localized NMS (3, 5, 7, 9, 11)
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
@param mask        Constrict suppression to a ROI.
*/
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,   //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                              //
void ImgNonMaxSuppression(                                                          //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],  //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)], //
    const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {                                    //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM,    //
                        WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(input, output, mask); //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE>                            //
void ImgNonMaxSuppression(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],  //
                          DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)], //
                          const vx_uint8 mask[KERN_SIZE][KERN_SIZE]) {            //
#pragma HLS INLINE
    HwNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                        WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE>(                   //
        (vx_image_data<DataType, VEC_NUM> *)input,                                //
        (vx_image_data<DataType, VEC_NUM> *)output,                               //
        mask);                                                                    //
}

/**********************************************************************************************************************/
/** @brief  Computes an image of response values according to the FAST9 corner detector
@param DataType    Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM     Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH       Image width
@param HEIGHT      Image height
@param BORDER_TYPE Type of border (Constant, Replicated or Undefined)
@param input       Input image
@param output      Output image
*/
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_border_e BORDER_TYPE>                                                    //
void ImgSegmentTestDetector(                                                          //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwSegmentTestDetector<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                          WIDTH, HEIGHT, BORDER_TYPE>(input, output);               //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_border_e BORDER_TYPE>                                                    //
void ImgSegmentTestDetector(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
                            DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HwSegmentTestDetector<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                          WIDTH, HEIGHT, BORDER_TYPE>(                              //
        (vx_image_data<DataType, VEC_NUM> *)input,                                  //
        (vx_image_data<DataType, VEC_NUM> *)output);                                //
}

/**********************************************************************************************************************/
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
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE,                                //
          DataType THRESH_LOWER, DataType THRESH_UPPER>                               //
void ImgHysteresis(                                                                   //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwHysteresis<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                 WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE,                    //
                 THRESH_LOWER, THRESH_UPPER>(input, output);               //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 KERN_SIZE, vx_border_e BORDER_TYPE,                            //
          DataType THRESH_LOWER, DataType THRESH_UPPER>                           //
void ImgHysteresis(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],         //
                   DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {      //
#pragma HLS INLINE
    HwHysteresis<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM,           //
                 WIDTH, HEIGHT, KERN_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>( //
        (vx_image_data<DataType, VEC_NUM> *)input,                                   //
        (vx_image_data<DataType, VEC_NUM> *)output);                                 //
}

/**********************************************************************************************************************/
/** @brief  Find local maxima in an image depending on the phase, or otherwise suppress pixels that are not local
maxima.
@param DataType     Data type of the input/output image (unsigned, signed)(8-bit, 16-bit, 32-bit)
@param VEC_NUM      Amount of pixels computed in parallel (1, 2, 4, 8)
@param WIDTH        Image WIDTH
@param HEIGHT       Image HEIGHT
@param BORDER_TYPE  Type of border (Constant, Replicated or Undefined)
@param QUANTIZATION The quantisazion of the orientation values. Allowed values Q = [1..8]. 2^(Q) different output values
@param input1       Input image (Gradient Magnitude)
@param input2       Input image (Orientation)
@param output       Output image
*/
template <typename DataType, vx_uint32 VEC_NUM, vx_uint32 WIDTH, vx_uint32 HEIGHT,    //
          vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>                            //
void ImgNonMaxSuppressionOriented(                                                    //
    vx_image_data<DataType, VEC_NUM> input1[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],   //
    vx_image_data<DataType, VEC_NUM> input2[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],   //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, output);
    HwOrientedNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                                WIDTH, HEIGHT, BORDER_TYPE,                               //
                                QUANTIZATION>(input1, input2, output);                    //
}
template <typename DataType, vx_uint32 VEC_NUM, vx_uint32 WIDTH, vx_uint32 HEIGHT,          //
          vx_border_e BORDER_TYPE, vx_uint32 QUANTIZATION>                                  //
void ImgNonMaxSuppressionOriented(DataType input1[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],   //
                                  DataType input2[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],   //
                                  DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HwOrientedNonMaxSuppression<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                                WIDTH, HEIGHT, BORDER_TYPE, QUANTIZATION>(                //
        (vx_image_data<DataType, VEC_NUM> *)input1,                                       //
        (vx_image_data<DataType, VEC_NUM> *)input2,                                       //
        (vx_image_data<DataType, VEC_NUM> *)output);                                      //
}

/**********************************************************************************************************************/
/**
 * @brief Computes the determinant of the Hessian matrix
 * @tparam InType       Data type of the input image(signed)(8 - bit, 16 - bit)
 * @tparam OutType      Data type of the output image(signed)(8 - bit, 16 - bit)
 * @tparam VEC_NUM      Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam WIDTH        Image width
 * @tparam HEIGHT       Image height
 * @tparam SCALE        Kernel scale factor (new kernel size )
 * @tparam STEP         Number of pixels to be skipped
 * @tparam BORDER_TYPE  Type of border(Constant, Replicated or Undefined)
 * @param input_lx      First order derivatives in respect to x
 * @param input_ly      First order derivatives in respect to y
 * @param output        Determinant of the Hessian response values
 */
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,                //
          vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                                                //
void ImgDeterminantOfHessian(                                                                                    //
    vx_image_data<InType, VEC_NUM> input_lx[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                              //
    vx_image_data<InType, VEC_NUM> input_ly[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                              //
    vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> output[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_lx, input_ly, output);
    HwDetHessian<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), //
                 STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP),              //
                 VEC_NUM, STEP_VEC_NUM_OUT(VEC_NUM, STEP),               //
                 WIDTH, HEIGHT, SCALE, STEP, BORDER_TYPE, vx_false_e>    //
        (input_lx, input_ly, output);                                    //
}
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                                 //
void ImgDeterminantOfHessian(InType input_lx[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],              //
                             InType input_ly[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],              //
                             OutType output[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) {         //
#pragma HLS INLINE
    HwDetHessian<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM),    //
                 STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP),                 //
                 VEC_NUM, STEP_VEC_NUM_OUT(VEC_NUM, STEP),                  //
                 WIDTH, HEIGHT, SCALE, STEP, BORDER_TYPE, vx_false_e>(      //
        (vx_image_data<InType, VEC_NUM> *)input_lx,                         //
        (vx_image_data<InType, VEC_NUM> *)input_ly,                         //
        (vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> *)output); //
}

/**********************************************************************************************************************/
/**
 * @brief Computes the determinant of the Hessian matrix
 * @tparam InType Data type of the input image(signed)(8 - bit, 16 - bit)
 * @tparam OutType Data type of the output image(signed)(8 - bit, 16 - bit)
 * @tparam VEC_NUM Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam IMG_COLS Image width
 * @tparam IMG_ROWS Image height
 * @tparam SCALE Kernel scale factor (new kernel size )
 * @tparam STEP Number of pixels to be skipped
 * @tparam BORDER_TYPE Type of border(Constant, Replicated or Undefined)
 * @param input_lx First order derivatives in respect to x
 * @param input_ly First order derivatives in respect to y
 * @param output Determinant of the Hessian response values
 */
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,                //
          vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                                                //
void ImgDeterminantOfHessian(                                                                                    //
    vx_image_data<InType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                                 //
    vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> output[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwDetHessian<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), //
                 STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP),              //
                 VEC_NUM, STEP_VEC_NUM_OUT(VEC_NUM, STEP),               //
                 WIDTH, HEIGHT, SCALE, STEP, BORDER_TYPE, vx_true_e>     //
        (input, NULL, output);                                           //
}
template <typename InType, typename OutType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_uint8 SCALE, vx_uint8 STEP, vx_border_e BORDER_TYPE>                                 //
void ImgDeterminantOfHessian(InType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],                 //
                             OutType output[STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP)]) {         //
#pragma HLS INLINE
    HwDetHessian<InType, OutType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM),    //
                 STEP_PIXELS(HEIGHT, WIDTH, VEC_NUM, STEP),                 //
                 VEC_NUM, STEP_VEC_NUM_OUT(VEC_NUM, STEP),                  //
                 WIDTH, HEIGHT, SCALE, STEP, BORDER_TYPE, vx_true_e>(       //
        (vx_image_data<InType, VEC_NUM> *)input,                            //
        NULL,                                                               //
        (vx_image_data<OutType, STEP_VEC_NUM_OUT(VEC_NUM, STEP)> *)output); //
}

/**********************************************************************************************************************/
/**
 * @brief Computes a FED step for given step size. (+ forwarding of conduction coefficient matrix)
 * @tparam DataType         Data type of the input / output image(unsigned)(8-, 16-bit)
 * @tparam VEC_NUM          Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam WIDTH            Image width
 * @tparam HEIGHT           Image height
 * @tparam STEP_SIZE        Step size of the diffusion step
 * @tparam BORDER_TYPE      Type of border(Constant, Replicated or Undefined)
 * @param input_coeff       Matrix of conduction coefficents
 * @param input_lt          Input image
 * @param output_lt         Diffused output image
 * @param output_fw_coeff   forwarded Matrix of conduction coefficents
 */
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,              //
          vx_uint32 STEP_SIZE, vx_border_e BORDER_TYPE>                                        //
void ImgFastExplicitDiffusion(                                                                 //
    vx_image_data<DataType, VEC_NUM> input_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],          //
    vx_image_data<DataType, VEC_NUM> input_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],       //
    vx_image_data<DataType, VEC_NUM> output_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],         //
    vx_image_data<DataType, VEC_NUM> output_fw_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_lt, input_coeff, output_lt, output_fw_coeff);
    HwFED<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
          WIDTH, HEIGHT, STEP_SIZE, BORDER_TYPE, vx_true_e>         //
        (input_lt, input_coeff, output_lt, output_fw_coeff);        //
}

template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,                //
          vx_uint32 STEP_SIZE, vx_border_e BORDER_TYPE>                                          //
void ImgFastExplicitDiffusion(DataType input_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],          //
                              DataType input_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],       //
                              DataType output_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],         //
                              DataType output_fw_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HwFED<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
          WIDTH, HEIGHT, STEP_SIZE, BORDER_TYPE, vx_true_e>(        //
        (vx_image_data<DataType, VEC_NUM> *)input_lt,               //
        (vx_image_data<DataType, VEC_NUM> *)input_coeff,            //
        (vx_image_data<DataType, VEC_NUM> *)output_lt,              //
        (vx_image_data<DataType, VEC_NUM> *)output_fw_coeff);       //
}

/**********************************************************************************************************************/
/**
 * @brief Computes a FED step for given step size.
 * @tparam DataType         Data type of the input / output image(unsigned)(8-, 16-bit)
 * @tparam VEC_NUM          Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam WIDTH            Image width
 * @tparam HEIGHT           Image height
 * @tparam STEP_SIZE        Step size of the diffusion step
 * @tparam BORDER_TYPE      Type of border(Constant, Replicated or Undefined)
 * @param input_coeff       Matrix of conduction coefficents
 * @param input_lt          Input image
 * @param output_lt         Diffused output image
 */
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,        //
          vx_uint32 STEP_SIZE, vx_border_e BORDER_TYPE>                                  //
void ImgFastExplicitDiffusion(                                                           //
    vx_image_data<DataType, VEC_NUM> input_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<DataType, VEC_NUM> input_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)], //
    vx_image_data<DataType, VEC_NUM> output_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_lt, input_coeff, output_lt);
    HwFED<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
          WIDTH, HEIGHT, STEP_SIZE, BORDER_TYPE, vx_false_e>        //
        (input_lt, input_coeff, output_lt, NULL);                   //
}

template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,          //
          vx_uint32 STEP_SIZE, vx_border_e BORDER_TYPE>                                    //
void ImgFastExplicitDiffusion(DataType input_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
                              DataType input_coeff[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)], //
                              DataType output_lt[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HwFED<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
          WIDTH, HEIGHT, STEP_SIZE, BORDER_TYPE, vx_false_e>(       //
        (vx_image_data<DataType, VEC_NUM> *)input_lt,               //
        (vx_image_data<DataType, VEC_NUM> *)input_coeff,            //
        (vx_image_data<DataType, VEC_NUM> *)output_lt,              //
        NULL);                                                      //
}

/**********************************************************************************************************************/
/**
 * @brief Computes conduction coefficient matrix using 3x3 Scharr filter to compute the first order derivates.
 * @tparam DataType     Data type of the input / output image(unsigned)(8 - bit, 16 - bit)
 * @tparam VEC_NUM      Amount of pixels computed in parallel(1, 2, 4, 8)
 * @tparam WIDTH        Image width
 * @tparam HEIGHT       Image height
 * @tparam BORDER_TYPE  Type of border(Constant, Replicated or Undefined)
 * @tparam PRECISION    Level of precision to compute the conduction coefficents
 * @param input         Input image
 * @param output        Output image
 * @param csquare       Square of the contrast factor
 */
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT,     //
          vx_border_e BORDER_TYPE, vx_hint_e PRECISION>                               //
void ImgConductivity(                                                                 //
    vx_image_data<DataType, VEC_NUM> input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],    //
    vx_image_data<contrastSquareT, 1> csquare[1],                                     //
    vx_image_data<DataType, VEC_NUM> output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, csquare, output);
    HwConductivity<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                   WIDTH, HEIGHT, BORDER_TYPE,                               //
                   HIFLIPVX::PERONA_MALIK_WIDE_REGION, PRECISION>            //
        (input, output, csquare);                                            //
}
template <typename DataType, vx_uint8 VEC_NUM, vx_uint16 WIDTH, vx_uint16 HEIGHT, //
          vx_border_e BORDER_TYPE, vx_hint_e PRECISION>                           //
void ImgConductivity(DataType input[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)],       //
                     contrastSquareT csquare[1],                                  //
                     DataType output[VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)]) {    //
#pragma HLS INLINE
    HwConductivity<DataType, VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM), VEC_NUM, //
                   WIDTH, HEIGHT, BORDER_TYPE,                               //
                   HIFLIPVX::PERONA_MALIK_WIDE_REGION, PRECISION>(           //
        (vx_image_data<DataType, VEC_NUM> *)(input),                         //
        (vx_image_data<DataType, VEC_NUM> *)(output),                        //
        (vx_image_data<contrastSquareT, 1> *)csquare);                       //
}

#endif /* SRC_IMG_FILTER_BASE_H_ */
