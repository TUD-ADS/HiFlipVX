/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_conversion_base.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are all accelerated image conversion functions (Call from here)
 */

#ifndef SRC_IMG_CONVERSION_BASE_H_
#define SRC_IMG_CONVERSION_BASE_H_

#include "img_conversion_core.h"
#include "img_mt_conversion_scale.h"

/*********************************************************************************************************************/
/** @brief  Implements the Channel Combine Kernel. \n
 *  @details each "image type" has a "data type" \n
 *                      - VX_DF_IMAGE_U16 (vx_uint16) \n
 *                      - VX_DF_IMAGE_U32 (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGB (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGBX (vx_uint32) \n
 * @param DstType       The "data type" of the output image
 * @param SRC_PIXELS    The pixels of the input image
 * @param DST_PIXELS    The pixels of the output image.
 *                      can differ for RGB, since the 24-bit are interleaved.
 * @param DST_IMAGE      The "image type" of the output image
 * @param input0        The 1. plane of the input image
 * @param input1        The 2. plane of the input image
 * @param input2        The 3. plane of the input image
 * @param input3        The 4. plane of the input image
 * @param output        The output image
 */
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_image_data<vx_uint8, 1> input0[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input1[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input2[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input3[SRC_PIXELS],                            //
                       vx_image_data<DstType, 1> output[DST_PIXELS]) {                           //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input0, input1, input2, input3, output);
    ChannelCombine<DstType, 4, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, input2, input3, output);                 //
}
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_uint8 input0[SRC_PIXELS],                                              //
                       vx_uint8 input1[SRC_PIXELS],                                              //
                       vx_uint8 input2[SRC_PIXELS],                                              //
                       vx_uint8 input3[SRC_PIXELS],                                              //
                       DstType output[DST_PIXELS]) {                                             //
#pragma HLS INLINE
    ChannelCombine<DstType, 4, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, input2, input3, output);                 //
}

/*********************************************************************************************************************/
/** @brief  Implements the Channel Combine Kernel. \n
 *  @details each "image type" has a "data type" \n
 *                      - VX_DF_IMAGE_U16 (vx_uint16) \n
 *                      - VX_DF_IMAGE_U32 (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGB (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGBX (vx_uint32) \n
 * @param DstType       The "data type" of the output image
 * @param SRC_PIXELS    The pixels of the input image
 * @param DST_PIXELS    The pixels of the output image.
 *                      Can differ for RGB, since the 24-bit are interleaved.
 * @param DST_IMAGE     The "image type" of the output image
 * @param input0        The 1. plane of the input image
 * @param input1        The 2. plane of the input image
 * @param input2        The 3. plane of the input image
 * @param output        The output image
 */
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_image_data<vx_uint8, 1> input0[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input1[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input2[SRC_PIXELS],                            //
                       vx_image_data<DstType, 1> output[DST_PIXELS]) {                           //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input0, input1, input2, output);
    ChannelCombine<DstType, 3, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, input2, NULL, output);                   //
}
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_uint8 input0[SRC_PIXELS],                                              //
                       vx_uint8 input1[SRC_PIXELS],                                              //
                       vx_uint8 input2[SRC_PIXELS],                                              //
                       DstType output[DST_PIXELS]) {                                             //
#pragma HLS INLINE
    ChannelCombine<DstType, 3, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, input2, NULL, output);                   //
}

/*********************************************************************************************************************/
/** @brief  Implements the Channel Combine Kernel. \n
 *  @details each "image type" has a "data type" \n
 *                      - VX_DF_IMAGE_U16 (vx_uint16) \n
 *                      - VX_DF_IMAGE_U32 (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGB (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGBX (vx_uint32) \n
 * @param DstType       The "data type" of the output image
 * @param SRC_PIXELS    The pixels of the input image
 * @param DST_PIXELS    The pixels of the output image.
 *                      Can differ for RGB, since the 24-bit are interleaved.
 * @param DST_IMAGE     The "image type" of the output image (vx_df_image_e)
 * @param input0        The 1. plane of the input image
 * @param input1        The 2. plane of the input image
 * @param output        The output image
 */
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_image_data<vx_uint8, 1> input0[SRC_PIXELS],                            //
                       vx_image_data<vx_uint8, 1> input1[SRC_PIXELS],                            //
                       vx_image_data<DstType, 1> output[DST_PIXELS]) {                           //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input0, input1, output);
    ChannelCombine<DstType, 2, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, NULL, NULL, output);                     //
}
template <typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e DST_IMAGE> //
void ImgChannelCombine(vx_uint8 input0[SRC_PIXELS],                                              //
                       vx_uint8 input1[SRC_PIXELS],                                              //
                       DstType output[DST_PIXELS]) {                                             //
#pragma HLS INLINE
    ChannelCombine<DstType, 2, SRC_PIXELS, DST_PIXELS, DST_IMAGE> //
        (input0, input1, NULL, NULL, output);                     //
}

/*********************************************************************************************************************/
/** @brief  Implements the Channel Extraction Kernel. \n
 *  @details each "image type" has a "data type" \n
 *                      - VX_DF_IMAGE_U16 (vx_uint16) \n
 *                      - VX_DF_IMAGE_U32 (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGB (vx_uint32) \n
 *                      - VX_DF_IMAGE_RGBX (vx_uint32) \n
 * @param SrcType       The data type of the input image
 * @param CHANNEL_ID    The id of the channel to extract
 * @param SRC_PIXELS    The pixels of the input imagec
 *                      Can differ for RGB, since the 24-bit are interleaved.
 * @param DST_PIXELS    The pixels of the output image
 * @param SRC_IMAGE     The image type of the input image (vx_df_image_e)
 * @param input         The input image
 * @param output        The output image
 */
template <typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, //
          vx_uint32 DST_PIXELS, vx_df_image_e SRC_IMAGE>                      //
void ImgChannelExtract(vx_image_data<SrcType, 1> input[SRC_PIXELS],           //
                       vx_image_data<vx_uint8, 1> output[DST_PIXELS]) {       //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    ChannelExtract<SrcType, CHANNEL_ID, SRC_PIXELS, DST_PIXELS, SRC_IMAGE>(input, output);
}
template <typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, //
          vx_uint32 DST_PIXELS, vx_df_image_e SRC_IMAGE>                      //
void ImgChannelExtract(SrcType input[SRC_PIXELS],                             //
                       vx_uint8 output[DST_PIXELS]) {                         //
#pragma HLS INLINE
    ChannelExtract<SrcType, CHANNEL_ID, SRC_PIXELS, DST_PIXELS, SRC_IMAGE>(input, output);
}

/*********************************************************************************************************************/
/** @brief Converts image bit depth. The output image dimensions
 *         should be the same as the dimensions of the input image. \n
 *   @details (d = down | u = up | s = sign) \n
 * type|u8 |s8 |u16|s16|u32|s32
 * ----|---|---|---|---|---|---
 * u8	| -	| s	| u	| u	| u	| u
 * s8	| s	| -	| u	| u	| u	| u
 * u16	| d	| d	| -	| s	| u	| u
 * s16	| d	| d	| s	| -	| u	| u
 * u32	| d	| d	| d	| d	| -	| s
 * s32	| d	| d	| d	| d	| s	| -
 * @param SrcType      The data type of the input image (u8, u16, u32, s8, s16, s32 bit)
 * @param DstType      The data type of the output image (u8, u16, u32, s8, s16, s32 bit)
 * @param VEC_SIZE     The vector size
 * @param IMG_PIXEL    The amount of image pixels
 * @param CONV_POLICY  The conversion policy for overflow (vx_convert_policy_e)
 * @param SHIFT        Bits are shifted by this amount (e.g. up for u8->s16, down for s16->u8),
 *                     maximum shift is abs(inBitDepth - outBitDepth)
 * @param input        The input image
 * @param output       The output image
 */
template <typename SrcType, typename DstType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL,    //
          vx_convert_policy_e CONV_POLICY, vx_uint16 SHIFT>                              //
void ImgConvertBitDepth(vx_image_data<SrcType, VEC_SIZE> input[IMG_PIXEL / VEC_SIZE],    //
                        vx_image_data<DstType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}
template <typename SrcType, typename DstType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, //
          vx_convert_policy_e CONV_POLICY, vx_uint16 SHIFT>                           //
void ImgConvertBitDepth(SrcType input[IMG_PIXEL],                                     //
                        DstType output[IMG_PIXEL]) {                                  //
#pragma HLS INLINE
    ConvertBitDepth<SrcType, DstType, VEC_SIZE, IMG_PIXEL, CONV_POLICY, SHIFT>(input, output);
}

/*********************************************************************************************************************/
/** @brief  Converts the Color of an image between RGB/RGBX/Gray \n
 *  @details Possible Conversions are \n
 *      VX_DF_IMAGE_U8   -> VX_DF_IMAGE_RGB \n
 *      VX_DF_IMAGE_U8   -> VX_DF_IMAGE_RGBX \n
 *      VX_DF_IMAGE_RGB  -> VX_DF_IMAGE_U8 \n
 *      VX_DF_IMAGE_RGB  -> VX_DF_IMAGE_RGBX \n
 *      VX_DF_IMAGE_RGBX -> VX_DF_IMAGE_U8 \n
 *      VX_DF_IMAGE_RGBX -> VX_DF_IMAGE_RGB \n
 * @param SrcType       The datatype of the input image  (uint8 (Gray), uint32 (RGB, RGBX))
 * @param DstType       The datatype of the output image (uint8 (Gray), uint32 (RGB, RGBX))
 * @param IMG_PIXELS    The pixels of the image
 * @param SRC_IMAGE     The color type of the input image (RGB, RGBX, U8)
 * @param DST_IAMGE     The color type of the output image (RGB, RGBX, U8)
 * @param input         The input image
 * @param output        The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, //
          vx_df_image_e SRC_IMAGE, vx_df_image_e DST_IAMGE>                               //
void ImgConvertColor(vx_image_data<SrcType, 1> input[SRC_PIXELS],                         //
                     vx_image_data<DstType, 1> output[DST_PIXELS]) {                      //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    ConvertColor<SrcType, DstType, SRC_PIXELS, DST_PIXELS, SRC_IMAGE, DST_IAMGE>(input, output);
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, //
          vx_df_image_e SRC_IMAGE, vx_df_image_e DST_IAMGE>                               //
void ImgConvertColor(SrcType input[SRC_PIXELS],                                           //
                     DstType output[DST_PIXELS]) {                                        //
#pragma HLS INLINE
    ConvertColor<SrcType, DstType, SRC_PIXELS, DST_PIXELS, SRC_IMAGE, DST_IAMGE>(input, output);
}

/*********************************************************************************************************************/
/** @brief  Converts between two buffers with a different vector sizes (parallelization degree). \n
 * @details Input/output pixels can differ slightly if vector sizes are not a multiple of each other. \n
 *          In that case the bigger image needs to be aligned and filled with zeroes. \n
 * @param DataType      The data type of the input and output
 * @param IMAGES        The amount of images (batches)
 * @param SRC_PIXELS    Amount of pixels in the input image
 * @param DST_PIXELS    Amount of pixels in the output image
 * @param SRC_VECTOR    The vector size of input image
 * @param DST_VECTOR    The vector size of output image
 * @param src           input image
 * @param dst           output image
 */
template <typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, //
          vx_int64 SRC_VECTOR, vx_int64 DST_VECTOR>                                     //
void ImgDataWidthConverter(                                                             //
    vx_image_data<DataType, SRC_VECTOR> input[(IMAGES * SRC_PIXELS) / SRC_VECTOR],      //
    vx_image_data<DataType, DST_VECTOR> output[(IMAGES * DST_PIXELS) / DST_VECTOR]) {   //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_VECTOR, DST_VECTOR> //
        (input, output);                                                                 //
}
template <typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, //
          vx_int64 SRC_VECTOR, vx_int64 DST_VECTOR>                                     //
void ImgDataWidthConverter(                                                             //
    DataType input[(IMAGES * SRC_PIXELS) / SRC_VECTOR],                                 //
    vx_image_data<DataType, DST_VECTOR> output[(IMAGES * DST_PIXELS) / DST_VECTOR]) {   //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(output);
    DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_VECTOR, DST_VECTOR> //
        ((vx_image_data<DataType, SRC_VECTOR> *)input, output);                          //
}
template <typename DataType, vx_int64 IMAGES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, //
          vx_int64 SRC_VECTOR, vx_int64 DST_VECTOR>                                     //
void ImgDataWidthConverter(                                                             //
    vx_image_data<DataType, SRC_VECTOR> input[(IMAGES * SRC_PIXELS) / SRC_VECTOR],      //
    DataType output[(IMAGES * DST_PIXELS) / DST_VECTOR]) {                              //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input);
    DataWidthConverter<DataType, IMAGES, SRC_PIXELS, DST_PIXELS, SRC_VECTOR, DST_VECTOR> //
        (input, (vx_image_data<DataType, DST_VECTOR> *)output);                          //
}

/*********************************************************************************************************************/
/** Copies the input image to multiple output images
 * @param DataType   The data type of the input and output
 * @param IMG_PIXELS The amount of images
 * @param VEC_NUM    Amount of pixels in the input image
 * @param input      input image
 * @param out1       output image
 * @param out2       output image
 * @param out3       output image
 * @param out4       output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>             //
void ImgMulticast(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],  //
                  vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out3[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out4[IMG_PIXELS / VEC_NUM]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2, out3, out4);
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 4>(input, out1, out2, out3, out4);
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(DataType input[IMG_PIXELS],  //
                  DataType out1[IMG_PIXELS],   //
                  DataType out2[IMG_PIXELS],   //
                  DataType out3[IMG_PIXELS],   //
                  DataType out4[IMG_PIXELS]) { //
#pragma HLS INLINE
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 4>(input, out1, out2, out3, out4);
}

/** Copies the input image to multiple output images
 * @param DataType   The data type of the input and output
 * @param IMG_PIXELS The amount of images
 * @param VEC_NUM    Amount of pixels in the input image
 * @param input      input image
 * @param out1       output image
 * @param out2       output image
 * @param out3       output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>             //
void ImgMulticast(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],  //
                  vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out3[IMG_PIXELS / VEC_NUM]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2, out3);
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 3>(input, out1, out2, out3, NULL);
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(DataType input[IMG_PIXELS / VEC_NUM],  //
                  DataType out1[IMG_PIXELS / VEC_NUM],   //
                  DataType out2[IMG_PIXELS / VEC_NUM],   //
                  DataType out3[IMG_PIXELS / VEC_NUM]) { //
#pragma HLS INLINE
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 3>(input, out1, out2, out3, NULL);
}

/** Copies the input image to multiple output images
 * @param DataType   The data type of the input and output
 * @param IMG_PIXELS The amount of images
 * @param VEC_NUM    Amount of pixels in the input image
 * @param input      input image
 * @param out1       output image
 * @param out2       output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>             //
void ImgMulticast(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],  //
                  vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],   //
                  vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2);
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 2>(input, out1, out2, NULL, NULL);
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM>
void ImgMulticast(DataType input[IMG_PIXELS / VEC_NUM],  //
                  DataType out1[IMG_PIXELS / VEC_NUM],   //
                  DataType out2[IMG_PIXELS / VEC_NUM]) { //
#pragma HLS INLINE
    Multicast<DataType, IMG_PIXELS, VEC_NUM, 2>(input, out1, out2, NULL, NULL);
}

/*********************************************************************************************************************/

/** Splits the input image into multiple output blocks
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param FACTOR        The factor of scattering (vx_scatter_mode_e)
 * @param MODE          The scatter mode
 * @param input         input image
 * @param out1          output block
 * @param out2          output block
 * @param out3          output block
 * @param out4          output block
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],                          //
                vx_image_data<DataType, VEC_NUM> out1[(IMG_PIXELS / VEC_NUM) / 4],                     //
                vx_image_data<DataType, VEC_NUM> out2[(IMG_PIXELS / VEC_NUM) / 4],                     //
                vx_image_data<DataType, VEC_NUM> out3[(IMG_PIXELS / VEC_NUM) / 4],                     //
                vx_image_data<DataType, VEC_NUM> out4[(IMG_PIXELS / VEC_NUM) / 4]) {                   //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2, out3, out4);
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 4, MODE>              //
        (input, out1, out2, out3, out4, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(DataType input[IMG_PIXELS],                                                            //
                DataType out1[IMG_PIXELS / 4],                                                         //
                DataType out2[IMG_PIXELS / 4],                                                         //
                DataType out3[IMG_PIXELS / 4],                                                         //
                DataType out4[IMG_PIXELS / 4]) {                                                       //
#pragma HLS INLINE
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 4, MODE>              //
        (input, out1, out2, out3, out4, NULL, NULL, NULL, NULL); //
}

/** Splits the input image into multiple output blocks
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param FACTOR        The factor of scattering (vx_scatter_mode_e)
 * @param MODE          The scatter mode
 * @param input         input image
 * @param out1          output block
 * @param out2          output block
 * @param out3          output block
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],                          //
                vx_image_data<DataType, VEC_NUM> out1[(IMG_PIXELS / VEC_NUM) / 3],                     //
                vx_image_data<DataType, VEC_NUM> out2[(IMG_PIXELS / VEC_NUM) / 3],                     //
                vx_image_data<DataType, VEC_NUM> out3[(IMG_PIXELS / VEC_NUM) / 3]) {                   //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2, out3);
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 3, MODE>              //
        (input, out1, out2, out3, NULL, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(DataType input[IMG_PIXELS],                                                            //
                DataType out1[IMG_PIXELS / 3],                                                         //
                DataType out2[IMG_PIXELS / 3],                                                         //
                DataType out3[IMG_PIXELS / 3]) {                                                       //
#pragma HLS INLINE
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 3, MODE>              //
        (input, out1, out2, out3, NULL, NULL, NULL, NULL, NULL); //
}

/** Splits the input image into multiple output blocks
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param FACTOR        The factor of scattering (vx_scatter_mode_e)
 * @param MODE          The scatter mode
 * @param input         input image
 * @param out1          output block
 * @param out2          output block
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],                          //
                vx_image_data<DataType, VEC_NUM> out1[(IMG_PIXELS / VEC_NUM) / 2],                     //
                vx_image_data<DataType, VEC_NUM> out2[(IMG_PIXELS / VEC_NUM) / 2]) {                   //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out1, out2);
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 2, MODE>              //
        (input, out1, out2, NULL, NULL, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgScatter(DataType input[IMG_PIXELS],                                                            //
                DataType out1[IMG_PIXELS / 2],                                                         //
                DataType out2[IMG_PIXELS / 2]) {                                                       //
#pragma HLS INLINE
    Scatter<DataType, IMG_PIXELS, VEC_NUM, 2, MODE>              //
        (input, out1, out2, NULL, NULL, NULL, NULL, NULL, NULL); //
}

/*********************************************************************************************************************/

/** Collects multiple input blocks into one output image
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param MODE          The gather mode (vx_scatter_mode_e)
 * @param in1           input block
 * @param in2           input block
 * @param in3           input block
 * @param in4           input block
 * @param output        output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(vx_image_data<DataType, VEC_NUM> in1[(IMG_PIXELS / VEC_NUM) / 4],                       //
               vx_image_data<DataType, VEC_NUM> in2[(IMG_PIXELS / VEC_NUM) / 4],                       //
               vx_image_data<DataType, VEC_NUM> in3[(IMG_PIXELS / VEC_NUM) / 4],                       //
               vx_image_data<DataType, VEC_NUM> in4[(IMG_PIXELS / VEC_NUM) / 4],                       //
               vx_image_data<DataType, VEC_NUM> output[IMG_PIXELS / VEC_NUM]) {                        //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(in1, in2, in3, in4, output);
    Gather<DataType, IMG_PIXELS, VEC_NUM, 4, MODE>            //
        (output, in1, in2, in3, in4, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(DataType output[IMG_PIXELS],                                                            //
               DataType in1[IMG_PIXELS / 4],                                                           //
               DataType in2[IMG_PIXELS / 4],                                                           //
               DataType in3[IMG_PIXELS / 4],                                                           //
               DataType in4[IMG_PIXELS / 4]) {                                                         //
#pragma HLS INLINE
    Gather<DataType, IMG_PIXELS, VEC_NUM, 4, MODE>            //
        (output, in1, in2, in3, in4, NULL, NULL, NULL, NULL); //
}

/** Collects multiple input blocks into one output image
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param MODE          The gather mode (vx_scatter_mode_e)
 * @param in1           input block
 * @param in2           input block
 * @param in3           input block
 * @param output        output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(vx_image_data<DataType, VEC_NUM> in1[(IMG_PIXELS / VEC_NUM) / 3],                       //
               vx_image_data<DataType, VEC_NUM> in2[(IMG_PIXELS / VEC_NUM) / 3],                       //
               vx_image_data<DataType, VEC_NUM> in3[(IMG_PIXELS / VEC_NUM) / 3],                       //
               vx_image_data<DataType, VEC_NUM> output[IMG_PIXELS / VEC_NUM]) {                        //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(in1, in2, in3, output);
    Gather<DataType, IMG_PIXELS, VEC_NUM, 3, MODE>             //
        (output, in1, in2, in3, NULL, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(DataType output[IMG_PIXELS],                                                            //
               DataType in1[IMG_PIXELS / 3],                                                           //
               DataType in2[IMG_PIXELS / 3],                                                           //
               DataType in3[IMG_PIXELS / 3]) {                                                         //
#pragma HLS INLINE
    Gather<DataType, IMG_PIXELS, VEC_NUM, 3, MODE>             //
        (output, in1, in2, in3, NULL, NULL, NULL, NULL, NULL); //
}

/** Collects multiple input blocks into one output image
 * @param DataType      The data type of the input and output
 * @param IMG_PIXELS    The total amount of image pixels
 * @param VEC_NUM       The size of the vectorized data
 * @param MODE          The gather mode (vx_scatter_mode_e)
 * @param in1           input block
 * @param in2           input block
 * @param output        output image
 */
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(vx_image_data<DataType, VEC_NUM> in1[(IMG_PIXELS / VEC_NUM) / 2],                       //
               vx_image_data<DataType, VEC_NUM> in2[(IMG_PIXELS / VEC_NUM) / 2],                       //
               vx_image_data<DataType, VEC_NUM> output[IMG_PIXELS / VEC_NUM]) {                        //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(in1, in2, output);
    Gather<DataType, IMG_PIXELS, VEC_NUM, 2, MODE>              //
        (output, in1, in2, NULL, NULL, NULL, NULL, NULL, NULL); //
}
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, HIFLIPVX::vx_scatter_mode_e MODE> //
void ImgGather(DataType output[IMG_PIXELS],                                                            //
               DataType in1[IMG_PIXELS / 2],                                                           //
               DataType in2[IMG_PIXELS / 2]) {                                                         //
#pragma HLS INLINE
    Gather<DataType, IMG_PIXELS, VEC_NUM, 2, MODE>              //
        (output, in1, in2, NULL, NULL, NULL, NULL, NULL, NULL); //
}

/*********************************************************************************************************************/
/** @brief  Scale an image down using bilinear or nearest neighbor interpolation
 * @param SRC_COLS      The columns of the input image
 * @param SRC_ROWS      The rows of the input image
 * @param DST_COLS      The columns of the output image
 * @param DST_ROWS      The rows of the output image
 * @param SCALE_TYPE    The type of interpolation (vx_interpolation_type_e)
 * @param input         The input image
 * @param output        The output image
 */
template <vx_uint16 SRC_COLS, vx_uint16 SRC_ROWS, vx_uint16 DST_COLS,        //
          vx_uint16 DST_ROWS, vx_interpolation_type_e SCALE_TYPE>            //
void ImgScaleImage(vx_image_data<vx_uint8, 1> input[SRC_COLS * SRC_ROWS],    //
                   vx_image_data<vx_uint8, 1> output[DST_COLS * DST_ROWS]) { //
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    ScaleImage<SRC_COLS, SRC_ROWS, DST_COLS, DST_ROWS, SCALE_TYPE>(input, output);
}
template <vx_uint16 SRC_COLS, vx_uint16 SRC_ROWS, vx_uint16 DST_COLS, //
          vx_uint16 DST_ROWS, vx_interpolation_type_e SCALE_TYPE>     //
void ImgScaleImage(vx_uint8 input[SRC_COLS * SRC_ROWS],               //
                   vx_uint8 output[DST_COLS * DST_ROWS]) {            //
#pragma HLS INLINE
    ScaleImage<SRC_COLS, SRC_ROWS, DST_COLS, DST_ROWS, SCALE_TYPE>( //
        (vx_image_data<vx_uint8, 1> *)input,                        //
        (vx_image_data<vx_uint8, 1> *)output);                      //
}

/**
 * @brief Downscales an image by computing the mean value of FACTOR * FACTOR pixels.
 * @tparam DataType Data type of the input / output image(unsigned)(8 - bit, 16 - bit)
 * @tparam VEC_NUM_IN Amount of input pixels computed in parallel(1, 2, 4, 8)
 * @tparam IMG_COLS Image width
 * @tparam IMG_ROWS Image height
 * @tparam FACTOR Downscale factor (2, 4, 8)
 * @param input Input image
 * @param output Downscaled output image
 */
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 SCALE_FACTOR>
void ImgScaleImage(vx_image_data<DataType, VEC_NUM_IN> input[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                      vx_image_data<DataType, SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, SCALE_FACTOR)>
                          output[SCALE_DOWN_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, SCALE_FACTOR)]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    HwScaleDownMean<DataType, VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN),
                    SCALE_DOWN_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, SCALE_FACTOR), VEC_NUM_IN,
                    SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, SCALE_FACTOR), IMG_COLS, IMG_ROWS, SCALE_FACTOR>(input, output);
}
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 SCALE_FACTOR>
void ImgScaleImage(DataType input[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                   DataType output[SCALE_DOWN_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, SCALE_FACTOR)]) {
#pragma HLS INLINE

    HwScaleDownMean<DataType, VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN),
                    SCALE_DOWN_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, SCALE_FACTOR), VEC_NUM_IN,
                    SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, SCALE_FACTOR), IMG_COLS, IMG_ROWS, SCALE_FACTOR>(
        (vx_image_data<DataType, VEC_NUM_IN> *)input,
        (vx_image_data<DataType, SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, SCALE_FACTOR)> *)output);
}

#endif // SRC_IMG_CONVERSION_BASE_H_
