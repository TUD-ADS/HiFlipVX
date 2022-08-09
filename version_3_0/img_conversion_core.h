/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_conversion_core.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are all core functions needed for the image conversion functions (Do not call functions from here)
 */

#ifndef SRC_IMG_CONVERSION_CORE_H_
#define SRC_IMG_CONVERSION_CORE_H_

#include "vx_helper.h"

/*********************************************************************************************************************/
/** @brief Read from Gray Input
 * @param SrcType      The datatype of the input image
 * @param input       The input image
 * @param input_data  The input pixel
 * @param output_data The output pixels
 */
template <typename SrcType> void InputGray(SrcType input, vx_uint8 &input_data, vx_uint8 output_data[3]) {
    input_data = static_cast<vx_uint8>(input);
    output_data[0] = input_data; // R
    output_data[1] = input_data; // G
    output_data[2] = input_data; // B
}

/*********************************************************************************************************************/
/** @brief Read from RGB Input
 * @param SrcType      The datatype of the input image
 * @param input       The input image
 * @param input_data  The input pixel
 * @param output_data The output pixels
 */
template <typename SrcType> void InputRgb(SrcType input, vx_uint32 &input_data, vx_uint8 output_data[4]) {
#pragma HLS INLINE
    input_data = static_cast<vx_uint32>(input);
    output_data[0] = static_cast<vx_uint8>((input_data >> 0) & 0xFF);
    output_data[1] = static_cast<vx_uint8>((input_data >> 8) & 0xFF);
    output_data[2] = static_cast<vx_uint8>((input_data >> 16) & 0xFF);
    output_data[3] = static_cast<vx_uint8>((input_data >> 24) & 0xFF);
}

/*********************************************************************************************************************/
/** @brief Read from RGBX Input
 * @param SrcType      The datatype of the input image
 * @param input       The input image
 * @param input_data  The input pixel
 * @param output_data The output pixels
 */
template <typename SrcType> void InputRgbx(SrcType input, vx_uint32 &input_data, vx_uint8 output_data[3]) {
#pragma HLS INLINE
    input_data = static_cast<vx_uint32>(input);
    output_data[0] = static_cast<vx_uint8>((input_data >> 0) & 0xFF);
    output_data[1] = static_cast<vx_uint8>((input_data >> 8) & 0xFF);
    output_data[2] = static_cast<vx_uint8>((input_data >> 16) & 0xFF);
}

/*********************************************************************************************************************/
/** @brief Write to Gray output (BT.601)
 * @param DstType     The datatype of the output image
 * @param output_data The input pixels (after conversion)
 * @return            The output image
 */
template <typename DstType> DstType OutputGray(vx_uint8 output_data[3]) {
#pragma HLS INLINE
    vx_uint32 gray = 306 * static_cast<vx_uint32>(output_data[0]) + // (int)((0.299 * 1024) + 0.5)
                     601 * static_cast<vx_uint32>(output_data[1]) + // (int)((0.587 * 1024) + 0.5)
                     117 * static_cast<vx_uint32>(output_data[2]);  // (int)((0.114 * 1024) + 0.5)
    return static_cast<DstType>((gray + 512) >> 10);                // round to nearest
}

/*********************************************************************************************************************/
/** @brief Write to RGB output
 * @param DstType     The datatype of the output image
 * @param output_data The input pixels (after conversion)
 * @return            The output image
 */
template <typename DstType> DstType OutputRgb(vx_uint8 output_data[4]) {
#pragma HLS INLINE
    DstType result = static_cast<DstType>(output_data[0] << 0) | static_cast<DstType>(output_data[1] << 8) |
                     static_cast<DstType>(output_data[2] << 16) | static_cast<DstType>(output_data[3] << 24);
    return result;
}

/*********************************************************************************************************************/
/** @brief Write to RGBX output
 * @param DstType     The datatype of the output image
 * @param output_data The input pixels (after conversion)
 * @return            The output image
 */
template <typename DstType> DstType OutputRgbx(vx_uint8 output_data[3]) {
#pragma HLS INLINE
    DstType result = (static_cast<DstType>(output_data[0]) << 0) | (static_cast<DstType>(output_data[1]) << 8) |
                     (static_cast<DstType>(output_data[2]) << 16);
    return result;
}

/*********************************************************************************************************************/
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

/*********************************************************************************************************************/
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

/*********************************************************************************************************************/
// Converts the input channels to output channels
template <typename SrcType, vx_df_image_e INPUT_TYPE, vx_uint32 INPUT_SIZE>
void ChannelExtractConversion(vx_uint8 &cases, SrcType input_data, vx_uint8 internal_data[INPUT_SIZE],
                              vx_uint8 buffer_data[3], vx_uint8 output_data[INPUT_SIZE]) {
#pragma HLS INLINE

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
}

/*********************************************************************************************************************/
// Checkesif the data types of the input are allowed
template <typename SrcType, const vx_uint16 CHANNEL_ID, vx_df_image_e INPUT_TYPE> void ChannelExtractDataTypes() {
#pragma HLS INLINE

    // Constants
    const vx_uint32 INPUT_SIZE = sizeof(SrcType);

    // Check Types (Static Assertions)
    const bool allowed_types1 =
        (CHANNEL_ID <= 2 && INPUT_TYPE == VX_DF_IMAGE_U16) || (CHANNEL_ID <= 4 && INPUT_TYPE == VX_DF_IMAGE_U32) ||
        (CHANNEL_ID <= 3 && INPUT_TYPE == VX_DF_IMAGE_RGB) || (CHANNEL_ID <= 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool allowed_types2 =
        (INPUT_SIZE == 2 && INPUT_TYPE == VX_DF_IMAGE_U16) || (INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_U32) ||
        (INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGB) || (INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
    STATIC_ASSERT(allowed_types1, missmatch_channel_id_does_not_fit);
    STATIC_ASSERT(allowed_types2, missmatch_bit_width_of_output_type);
}

/*********************************************************************************************************************/
/** @brief Implements the Channel Extraction Kernel.
 * @param SrcType   The datatype of the input image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
 * @param CHANNEL_ID  The id of the channel to extract
 * @param IMG_PIXELS  The pixels of the image
 * @param INPUT_TYPE  The color type of the input image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB,
 * VX_DF_IMAGE_RGBX)
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS,
          vx_df_image_e INPUT_TYPE>
void ChannelExtract(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<vx_uint8, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 INPUT_SIZE = sizeof(SrcType);

    // Variables
    vx_uint8 cases = 0;
    SrcType input_data = 0;
    vx_uint8 internal_data[INPUT_SIZE];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[INPUT_SIZE];
#pragma HLS array_partition variable = internal_data complete dim = 0
#pragma HLS array_partition variable = buffer_data complete dim = 0
#pragma HLS array_partition variable = output_data complete dim = 0

    // Check Types (Static Assertions)
    ChannelExtractDataTypes<SrcType, CHANNEL_ID, INPUT_TYPE>();
    ;

    // Computes bitwise operations (pipelined)
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<vx_uint8, 1> dst;

        // Read from input channel
        if (cases < 3 && j < SRC_PIXELS) {
            src = input[j];
            input_data = src.pixel[0];
            j++;
        }

        // Converts the input channels to output channels
        ChannelExtractConversion<SrcType, INPUT_TYPE, INPUT_SIZE>(cases, input_data, internal_data, buffer_data,
                                                                  output_data);

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<vx_uint8, 1>((i == 0), (i == DST_PIXELS - 1), dst);

        // Write to output channel
        dst.pixel[0] = output_data[CHANNEL_ID];
        output[i] = dst;
    }
}

/*********************************************************************************************************************/
/** @brief Implements the Channel Extraction Kernel.
 * @param SrcType   The datatype of the input image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
 * @param CHANNEL_ID  The id of the channel to extract
 * @param IMG_PIXELS  The pixels of the image
 * @param INPUT_TYPE  The color type of the input image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB,
 * VX_DF_IMAGE_RGBX)
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, const vx_uint16 CHANNEL_ID, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS,
          vx_df_image_e INPUT_TYPE>
void ChannelExtract(SrcType input[SRC_PIXELS], vx_uint8 output[DST_PIXELS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 INPUT_SIZE = sizeof(SrcType);

    // Variables
    vx_uint8 cases = 0;
    SrcType input_data = 0;
    vx_uint8 internal_data[INPUT_SIZE];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[INPUT_SIZE];
#pragma HLS array_partition variable = internal_data complete dim = 0
#pragma HLS array_partition variable = buffer_data complete dim = 0
#pragma HLS array_partition variable = output_data complete dim = 0

    // Check Types (Static Assertions)
    ChannelExtractDataTypes<SrcType, CHANNEL_ID, INPUT_TYPE>();

    // Computes bitwise operations (pipelined)
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // Read from input channel
        if (cases < 3 && j < SRC_PIXELS) {
            input_data = input[j];
            j++;
        }

        // Converts the input channels to output channels
        ChannelExtractConversion<SrcType, INPUT_TYPE, INPUT_SIZE>(cases, input_data, internal_data, buffer_data,
                                                                  output_data);

        // Write to output channel
        output[i] = output_data[CHANNEL_ID];
    }
}

/*********************************************************************************************************************/
//
template <typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_df_image_e OUTPUT_TYPE> void ChannelCombineDataTypes() {

    // Constants
    const vx_uint16 OUTPUT_SIZE = sizeof(DstType);

    // Check Types (Static Assertions)
    const bool allowed_types1 = (CHANNEL_AMOUNT == 2 && OUTPUT_TYPE == VX_DF_IMAGE_U16) ||
                                (CHANNEL_AMOUNT == 4 && OUTPUT_TYPE == VX_DF_IMAGE_U32) ||
                                (CHANNEL_AMOUNT == 3 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) ||
                                (CHANNEL_AMOUNT == 3 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool allowed_types2 =
        (OUTPUT_SIZE == 2 && OUTPUT_TYPE == VX_DF_IMAGE_U16) || (OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_U32) ||
        (OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) || (OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    STATIC_ASSERT(allowed_types1, missmatch_channel_amount_does_not_fit);
    STATIC_ASSERT(allowed_types2, missmatch_bit_width_of_output_type);
}

/*********************************************************************************************************************/
/** @brief Implements the Channel Combine Kernel.
 * @param DstType     The datatype of the > image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
 * @param CHANNEL_AMOUNT The amount of channels to combine
 * @param IMG_PIXELS     The pixels of the image
 * @param OUTPUT_TYPE    The color type of the > image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB,
 * VX_DF_IMAGE_RGBX)
 * @param input0         The 1. plane of the input image
 * @param input1         The 2. plane of the input image
 * @param input2         The 3. plane of the input image (optional)
 * @param input3         The 4. plane of the input image (optional)
 * @param output         The output image
 */
template <typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS,
          vx_df_image_e OUTPUT_TYPE>
void ChannelCombine(vx_image_data<vx_uint8, 1> input0[SRC_PIXELS], vx_image_data<vx_uint8, 1> input1[SRC_PIXELS],
                    vx_image_data<vx_uint8, 1> input2[SRC_PIXELS], vx_image_data<vx_uint8, 1> input3[SRC_PIXELS],
                    vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 OUTPUT_SIZE = sizeof(DstType);

    // Variables
    vx_uint8 cases = 0;
    vx_image_data<vx_uint8, 1> input_data[4];
    vx_uint8 buffer[4];
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[2];
#pragma HLS array_partition variable = input_data complete dim = 0
#pragma HLS array_partition variable = buffer complete dim = 0
#pragma HLS array_partition variable = internal_data complete dim = 0
#pragma HLS array_partition variable = buffer_data complete dim = 0

    // Check Types (Static Assertions)
    ChannelCombineDataTypes<DstType, CHANNEL_AMOUNT, OUTPUT_TYPE>();

    // Computes the channel conversion
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<DstType, 1> output_data;

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
        if (OUTPUT_TYPE == VX_DF_IMAGE_RGB) {
            for (vx_uint32 k = 0; k < CHANNEL_AMOUNT; ++k)
                buffer[k] = input_data[k].pixel[0];
            ConvertToRgb(cases, buffer, buffer_data, internal_data);
        } else {
            for (vx_uint32 k = 0; k < CHANNEL_AMOUNT; ++k)
                internal_data[k] = input_data[k].pixel[0];
        }

        // Pack data
        output_data.pixel[0] = static_cast<DstType>(internal_data[0]);
        output_data.pixel[0] |= static_cast<DstType>(internal_data[1]) << 8;
        if (OUTPUT_TYPE == VX_DF_IMAGE_U32 || OUTPUT_TYPE == VX_DF_IMAGE_RGB || OUTPUT_TYPE == VX_DF_IMAGE_RGBX)
            output_data.pixel[0] |= static_cast<DstType>(internal_data[2]) << 16;
        if (OUTPUT_TYPE == VX_DF_IMAGE_U32 || OUTPUT_TYPE == VX_DF_IMAGE_RGB)
            output_data.pixel[0] |= static_cast<DstType>(internal_data[3]) << 24;

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<DstType, 1>((j == 0), (j == DST_PIXELS - 1), output_data);

        // Write to output channel
        if (cases != 1 && j < DST_PIXELS) {
            output[j] = output_data;
            j++;
        }
    }
}

/*********************************************************************************************************************/
/** @brief Implements the Channel Combine Kernel.
 * @param DstType     The datatype of the > image  (vx_uint16 (U16), vx_uint32 (U32, RGB, RGBX))
 * @param CHANNEL_AMOUNT The amount of channels to combine
 * @param IMG_PIXELS     The pixels of the image
 * @param OUTPUT_TYPE    The color type of the > image (VX_DF_IMAGE_U16, VX_DF_IMAGE_U32, VX_DF_IMAGE_RGB,
 * VX_DF_IMAGE_RGBX)
 * @param input0         The 1. plane of the input image
 * @param input1         The 2. plane of the input image
 * @param input2         The 3. plane of the input image (optional)
 * @param input3         The 4. plane of the input image (optional)
 * @param output         The output image
 */
template <typename DstType, vx_uint16 CHANNEL_AMOUNT, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS,
          vx_df_image_e OUTPUT_TYPE>
void ChannelCombine(vx_uint8 input0[SRC_PIXELS], vx_uint8 input1[SRC_PIXELS], vx_uint8 input2[SRC_PIXELS],
                    vx_uint8 input3[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 OUTPUT_SIZE = sizeof(DstType);

    // Variables
    vx_uint8 cases = 0;
    vx_uint8 input_data[4];
    vx_uint8 buffer[4];
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[2];
#pragma HLS array_partition variable = input_data complete dim = 0
#pragma HLS array_partition variable = buffer complete dim = 0
#pragma HLS array_partition variable = internal_data complete dim = 0
#pragma HLS array_partition variable = buffer_data complete dim = 0

    // Check Types (Static Assertions)
    ChannelCombineDataTypes<DstType, CHANNEL_AMOUNT, OUTPUT_TYPE>();

    // Computes the channel conversion
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        DstType output_data;

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
        if (OUTPUT_TYPE == VX_DF_IMAGE_RGB) {
            for (vx_uint32 k = 0; k < CHANNEL_AMOUNT; ++k)
                buffer[k] = input_data[k];
            ConvertToRgb(cases, buffer, buffer_data, internal_data);
        } else {
            for (vx_uint32 k = 0; k < CHANNEL_AMOUNT; ++k)
                internal_data[k] = input_data[k];
        }

        // Pack data
        output_data = static_cast<DstType>(internal_data[0]);
        output_data |= static_cast<DstType>(internal_data[1]) << 8;
        if (OUTPUT_TYPE == VX_DF_IMAGE_U32 || OUTPUT_TYPE == VX_DF_IMAGE_RGB || OUTPUT_TYPE == VX_DF_IMAGE_RGBX)
            output_data |= static_cast<DstType>(internal_data[2]) << 16;
        if (OUTPUT_TYPE == VX_DF_IMAGE_U32 || OUTPUT_TYPE == VX_DF_IMAGE_RGB)
            output_data |= static_cast<DstType>(internal_data[3]) << 24;

        // Write to output channel
        if (cases != 1 && j < DST_PIXELS) {
            output[j] = output_data;
            j++;
        }
    }
}

/*********************************************************************************************************************/
/** @brief Convert from Grayscale to RGB
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertGrayToRgb(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint8 input_data;
    vx_uint8 internal_data[3];
    vx_uint8 buffer_data[2];
    vx_uint8 output_data[4];

    // Convert from Grayscale to RGB
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input
        src = input[i];
        InputGray<SrcType>(src.pixel[0], input_data, internal_data);

        // Conversion
        ConvertToRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        if (cases != 1) {
            GenerateDmaSignal<DstType, 1>((j == 0), (j == DST_PIXELS - 1), dst);
            dst.pixel[0] = OutputRgb<DstType>(output_data);
            output[j] = dst;
            j++;
        }
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertGrayToRgb(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint8 input_data;
    vx_uint8 internal_data[3];
    vx_uint8 buffer_data[2];
    vx_uint8 output_data[4];

    // Convert from Grayscale to RGB
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // Read from input
        InputGray<SrcType>(input[i], input_data, internal_data);

        // Conversion
        ConvertToRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        if (cases != 1) {
            output[j] = OutputRgb<DstType>(output_data);
            j++;
        }
    }
}

/*********************************************************************************************************************/
/** @brief Convert from Grayscale to RGBX
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertGrayToRgbx(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 input_data;
    vx_uint8 output_data[3];

    // Convert grayscale to RGBX
    for (vx_uint32 i = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input
        src = input[i];
        InputGray<SrcType>(src.pixel[0], input_data, output_data);

        // Write to output
        GenerateDmaSignal<DstType, 1>((i == 0), (i == DST_PIXELS - 1), dst);
        dst.pixel[0] = OutputRgbx<DstType>(output_data);
        output[i] = dst;
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertGrayToRgbx(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 input_data;
    vx_uint8 output_data[3];

    // Convert grayscale to RGBX
    for (vx_uint32 i = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // Read from input
        InputGray<SrcType>(input[i], input_data, output_data);

        // Write to output
        output[i] = OutputRgbx<DstType>(output_data);
    }
}

/*********************************************************************************************************************/
/** @brief Convert from RGB to Grayscale
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbToGray(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input and store it into internal data
        if (cases < 3) {
            src = input[j];
            ++j;
        }
        InputRgb<SrcType>(src.pixel[0], input_data, internal_data);

        // Conversion
        ConvertFromRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        GenerateDmaSignal<DstType, 1>((i == 0), (i == DST_PIXELS - 1), dst);
        dst.pixel[0] = OutputGray<DstType>(output_data);
        output[i] = dst;
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbToGray(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        SrcType src;

        // Read from input and store it into internal data
        if (cases < 3) {
            src = input[j];
            ++j;
        }
        InputRgb<SrcType>(src, input_data, internal_data);

        // Conversion
        ConvertFromRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        output[i] = OutputGray<DstType>(output_data);
    }
}

/*********************************************************************************************************************/
/** @brief Convert from RGB to RGBX
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbToRgbx(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input and store it into internal data
        if (cases < 3) {
            src = input[j];
            ++j;
        }
        InputRgb<SrcType>(src.pixel[0], input_data, internal_data);

        // Conversion
        ConvertFromRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        GenerateDmaSignal<DstType, 1>((i == 0), (i == DST_PIXELS - 1), dst);
        dst.pixel[0] = OutputRgbx<DstType>(output_data);
        output[i] = dst;
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbToRgbx(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[4];
    vx_uint8 buffer_data[3];
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < DST_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        SrcType src;

        // Read from input and store it into internal data
        if (cases < 3) {
            src = input[j];
            ++j;
        }
        InputRgb<SrcType>(src, input_data, internal_data);

        // Conversion
        ConvertFromRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        output[i] = OutputRgbx<DstType>(output_data);
    }
}

/*********************************************************************************************************************/
/** @brief Convert from RGBX to Grayscale
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbxToGray(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint32 input_data;
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input
        src = input[i];
        InputRgbx<SrcType>(src.pixel[0], input_data, output_data);

        // Write to output
        GenerateDmaSignal<DstType, 1>((i == 0), (i == DST_PIXELS - 1), dst);
        dst.pixel[0] = OutputGray<DstType>(output_data);
        output[i] = dst;
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbxToGray(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint32 input_data;
    vx_uint8 output_data[3];

    // Compute gray-scale on image
    for (vx_uint32 i = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // Read from input
        InputRgbx<SrcType>(input[i], input_data, output_data);

        // Write to output
        output[i] = OutputGray<DstType>(output_data);
    }
}

/*********************************************************************************************************************/
/** @brief Convert from RGBX to RGB
 * @param SrcType      The datatype of the input imag
 * @param DstType     The datatype of the output image
 * @param IMG_PIXELS  The pixel amount of the image
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbxToRgb(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[3];
    vx_uint8 buffer_data[2];
    vx_uint8 output_data[4];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<SrcType, 1> src;
        vx_image_data<DstType, 1> dst;

        // Read from input
        src = input[i];
        InputRgbx<SrcType>(src.pixel[0], input_data, internal_data);

        // Conversion
        ConvertToRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        if (cases != 1) {
            GenerateDmaSignal<DstType, 1>((j == 0), (j == DST_PIXELS - 1), dst);
            dst.pixel[0] = OutputRgb<DstType>(output_data);
            output[j] = dst;
            j++;
        }
    }
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS>
void ConvertRgbxToRgb(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Global variables
    vx_uint8 cases = 0;
    vx_uint32 input_data;
    vx_uint8 internal_data[3];
    vx_uint8 buffer_data[2];
    vx_uint8 output_data[4];

    // Compute gray-scale on image
    for (vx_uint32 i = 0, j = 0; i < SRC_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // Read from input
        InputRgbx<SrcType>(input[i], input_data, internal_data);

        // Conversion
        ConvertToRgb(cases, internal_data, buffer_data, output_data);

        // Write to output
        if (cases != 1) {
            output[j] = OutputRgb<DstType>(output_data);
            j++;
        }
    }
}

/*********************************************************************************************************************/
// Check data types of color conversion
template <typename SrcType, typename DstType, vx_df_image_e INPUT_TYPE, vx_df_image_e OUTPUT_TYPE>
void ConvertColorDataTypes() {

    // Constants
    const vx_uint32 INPUT_SIZE = sizeof(SrcType);
    const vx_uint32 OUTPUT_SIZE = sizeof(DstType);
    const bool IN_IS_SIGNED = std::numeric_limits<SrcType>::is_signed;
    const bool OUT_IS_SIGNED = std::numeric_limits<DstType>::is_signed;

    // Check types (static assertions)
    const bool allowed_types1 = (IN_IS_SIGNED == false) || (OUT_IS_SIGNED == false);
    const bool allowed_types2 = (INPUT_SIZE == 1 && INPUT_TYPE == VX_DF_IMAGE_U8) ||
                                (INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGB) ||
                                (INPUT_SIZE == 4 && INPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool allowed_types3 = (OUTPUT_SIZE == 1 && OUTPUT_TYPE == VX_DF_IMAGE_U8) ||
                                (OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGB) ||
                                (OUTPUT_SIZE == 4 && OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    STATIC_ASSERT(allowed_types1, datatype_of_input_and_output_must_be_unsigned);
    STATIC_ASSERT(allowed_types2, missmatch_input_datatype_size_and_input_type);
    STATIC_ASSERT(allowed_types3, missmatch_output_datatype_size_and_output_type);
}

/*********************************************************************************************************************/
/** @brief Converts the Color of an image between VX_DF_IMAGE_RGB / VX_DF_IMAGE_RGBX / VX_DF_IMAGE_U8 (grayscale)
 * @param SrcType      The datatype of the input image  (uint8 (Gray), uint32 (RGB, RGBX))
 * @param DstType     The datatype of the output image (uint8 (Gray), uint32 (RGB, RGBX))
 * @param IMG_PIXELS  The pixel amount of the image
 * @param INPUT_TYPE  The color type of the input image (RGB, RGBX, Gray)
 * @param OUTPUT_TYPE The color type of the output image (RGB, RGBX, Gray)
 * @param input       The input image
 * @param output      The output image
 */
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE,
          vx_df_image_e OUTPUT_TYPE>
void ConvertColor(vx_image_data<SrcType, 1> input[SRC_PIXELS], vx_image_data<DstType, 1> output[DST_PIXELS]) {
#pragma HLS INLINE

    // Allowed color conversions
    const bool GRAY_to_RGB = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);
    const bool GRAY_to_RGBX = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool RGB_to_GRAY = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
    const bool RGB_to_RGBX = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool RGBX_to_GRAY = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
    const bool RGBX_to_RGB = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);

    // Check data types of color conversion
    ConvertColorDataTypes<SrcType, DstType, INPUT_TYPE, OUTPUT_TYPE>();
    const bool allowed_types4 =
        GRAY_to_RGB || GRAY_to_RGBX || RGB_to_GRAY || RGB_to_RGBX || RGBX_to_GRAY || RGBX_to_RGB;
    STATIC_ASSERT(allowed_types4, color_conversion_type_not_supported);

    // Color conversions
    if (GRAY_to_RGB)
        ConvertGrayToRgb<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (GRAY_to_RGBX)
        ConvertGrayToRgbx<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGB_to_GRAY)
        ConvertRgbToGray<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGB_to_RGBX)
        ConvertRgbToRgbx<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGBX_to_GRAY)
        ConvertRgbxToGray<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGBX_to_RGB)
        ConvertRgbxToRgb<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
}
template <typename SrcType, typename DstType, vx_uint32 SRC_PIXELS, vx_uint32 DST_PIXELS, vx_df_image_e INPUT_TYPE,
          vx_df_image_e OUTPUT_TYPE>
void ConvertColor(SrcType input[SRC_PIXELS], DstType output[DST_PIXELS]) {
#pragma HLS INLINE

    // Allowed color conversions
    const bool GRAY_to_RGB = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);
    const bool GRAY_to_RGBX = (INPUT_TYPE == VX_DF_IMAGE_U8) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool RGB_to_GRAY = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
    const bool RGB_to_RGBX = (INPUT_TYPE == VX_DF_IMAGE_RGB) && (OUTPUT_TYPE == VX_DF_IMAGE_RGBX);
    const bool RGBX_to_GRAY = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_U8);
    const bool RGBX_to_RGB = (INPUT_TYPE == VX_DF_IMAGE_RGBX) && (OUTPUT_TYPE == VX_DF_IMAGE_RGB);

    // Check data types of color conversion
    ConvertColorDataTypes<SrcType, DstType, INPUT_TYPE, OUTPUT_TYPE>();
    const bool allowed_types4 =
        GRAY_to_RGB || GRAY_to_RGBX || RGB_to_GRAY || RGB_to_RGBX || RGBX_to_GRAY || RGBX_to_RGB;
    STATIC_ASSERT(allowed_types4, color_conversion_type_not_supported);

    // Color conversions
    if (GRAY_to_RGB)
        ConvertGrayToRgb<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (GRAY_to_RGBX)
        ConvertGrayToRgbx<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGB_to_GRAY)
        ConvertRgbToGray<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGB_to_RGBX)
        ConvertRgbToRgbx<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGBX_to_GRAY)
        ConvertRgbxToGray<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
    else if (RGBX_to_RGB)
        ConvertRgbxToRgb<SrcType, DstType, SRC_PIXELS, DST_PIXELS>(input, output);
}

/*********************************************************************************************************************/
/** @brief Converts image bit depth.
 * @param SrcType      The datatype of the input image (8, 16, 32 bit unsigned/signed)
 * @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
 * @param CONV_POLICY  The conversion policy for overflow (VX_CONVERT_POLICY_WRAP or VX_CONVERT_POLICY_SATURATE)
 * @param SHIFT        Bits are shifted by this amount (e.g. up for u8->s16, down for s16->u8),  maximum shift is
 * abs(inBitDepth - outBitDepth)
 * @param src          The input pixel
 * @param dst          The output pixel
 */
template <typename SrcType, typename DstType, const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertDepthSingle(SrcType src, DstType &dst) {
#pragma HLS INLINE

    // Result
    DstType result = 0;

    // Constants
    const bool IN_IS_SIGNED = std::numeric_limits<SrcType>::is_signed;
    const bool OUT_IS_SIGNED = std::numeric_limits<DstType>::is_signed;
    const vx_int16 IN_SIZE = sizeof(SrcType);
    const vx_int16 OUT_SIZE = sizeof(DstType);

    // Check for max shift value
    const vx_uint16 MAX_SHIFT =
        (IN_SIZE > OUT_SIZE) ? static_cast<vx_uint16>(IN_SIZE - OUT_SIZE) : static_cast<vx_uint16>(OUT_SIZE - IN_SIZE);
    STATIC_ASSERT(MAX_SHIFT * 8 >= SHIFT, shift_size_out_of_range);

    // Check if down or up conversion
    bool DOWN_CONVERSION = false;
    bool UP_CONVERSION = false;

    // Border values for VX_CONVERT_POLICY_SATURATE
    vx_int64 MAX_VAL = 0;
    vx_int64 MIN_VAL = 0;

    // Set if down or up conversion
    if ((IN_SIZE == 1 && OUT_SIZE == 2) || (IN_SIZE == 1 && OUT_SIZE == 4) || (IN_SIZE == 2 && OUT_SIZE == 4)) {
        UP_CONVERSION = true;
    } else if ((IN_SIZE == 2 && OUT_SIZE == 1) || (IN_SIZE == 4 && OUT_SIZE == 1) || (IN_SIZE == 4 && OUT_SIZE == 2)) {
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
            value = MAX(MIN(value, MAX_VAL), MIN_VAL);
            result = static_cast<DstType>(value);
        }
    }
    if (UP_CONVERSION == true) {
        if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
            result = (static_cast<DstType>(src)) << SHIFT;
        } else if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
            if (OUT_IS_SIGNED == true) {
                vx_int64 value = (static_cast<vx_int64>(src)) << SHIFT;
                value = static_cast<DstType>(MAX(MIN(value, MAX_VAL), MIN_VAL));
                result = static_cast<DstType>(value);
            } else {
                result = (static_cast<DstType>(src)) << SHIFT;
            }
        }
    }

    // Convert from signed to unsigned or v.v.
    if (IN_SIZE == OUT_SIZE) {
        if (IN_IS_SIGNED != OUT_IS_SIGNED) {
            if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
                result = static_cast<DstType>(src);
            } else if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
                vx_int64 value = static_cast<vx_int64>(src);
                value = MAX(MIN(value, MAX_VAL), MIN_VAL);
                result = static_cast<DstType>(value);
            }
        } else {
            result = static_cast<DstType>(src);
        }
    }

    // Return result
    dst = result;
}

/*********************************************************************************************************************/
/** @brief Checks the data type to converts image bit depth.
 * @param SrcType       The datatype of the input image (8, 16, 32 bit unsigned/signed)
 * @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
 * @param VEC_SIZE     The vector size
 * @param IMG_PIXEL    The amount of image pixels
 */
template <typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL>
vx_uint32 ConvertBitDepthCheck() {

    // Constants
    const vx_type_e IN_TYPE = GET_TYPE(SrcType);
    const vx_type_e OUT_TYPE = GET_TYPE(DstType);
    const bool INPUT_TYPE_CORRECT = (IN_TYPE == VX_TYPE_UINT8) || (IN_TYPE == VX_TYPE_UINT16) ||
                                    (IN_TYPE == VX_TYPE_UINT32) || (IN_TYPE == VX_TYPE_INT8) ||
                                    (IN_TYPE == VX_TYPE_INT16) || (IN_TYPE == VX_TYPE_INT32);
    const bool OUTPUT_TYPE_CORRECT = (OUT_TYPE == VX_TYPE_UINT8) || (OUT_TYPE == VX_TYPE_UINT16) ||
                                     (OUT_TYPE == VX_TYPE_UINT32) || (OUT_TYPE == VX_TYPE_INT8) ||
                                     (OUT_TYPE == VX_TYPE_INT16) || (OUT_TYPE == VX_TYPE_INT32);

    // Check function parameters/types
    STATIC_ASSERT(INPUT_TYPE_CORRECT, input_type_error);
    STATIC_ASSERT(OUTPUT_TYPE_CORRECT, output_type_or_output_type_error);
    STATIC_ASSERT((IMG_PIXEL % VEC_SIZE) == 0, image_pixels_are_not_multiple_of_vector_size);
    STATIC_ASSERT((VEC_SIZE == 1) || (VEC_SIZE == 2) || (VEC_SIZE == 4) || (VEC_SIZE == 8),
                  _vec_size_musst_be_multiple_of_two_and_max_8);

    return IMG_PIXEL / static_cast<vx_uint32>(VEC_SIZE);
    ;
}

/*********************************************************************************************************************/
/** @brief Converts image bit depth.
 * @param SrcType       The datatype of the input image (8, 16, 32 bit unsigned/signed)
 * @param DstType      The datatype of the output image (8, 16, 32 bit unsigned/signed)
 * @param VEC_SIZE     The vector size
 * @param IMG_PIXEL    The amount of image pixels
 * @param CONV_POLICY  The conversion policy for overflow (VX_CONVERT_POLICY_WRAP or VX_CONVERT_POLICY_SATURATE)
 * @param SHIFT        Bits are shifted by this amount (e.g. up for u8->s16, down for s16->u8),  maximum shift is
 * abs(inBitDepth - outBitDepth)
 * @param input        The input image
 * @param output       The output image
 */
template <typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL,
          const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertBitDepth(vx_image_data<SrcType, VEC_SIZE> input[IMG_PIXEL / VEC_SIZE],
                     vx_image_data<DstType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 VEC_PIXEL = ConvertBitDepthCheck<SrcType, DstType, VEC_SIZE, IMG_PIXEL>();

    // Vector variables
    vx_image_data<SrcType, VEC_SIZE> src;
    vx_image_data<DstType, VEC_SIZE> dst;

    // Computes operations (pipelined)
    for (vx_uint32 i = 0; i < VEC_PIXEL; i++) {
#pragma HLS PIPELINE II = 1

        // Read Input
        src = input[i];

        // Compute vector of bit depth operation
        for (vx_uint16 j = 0; j < VEC_SIZE; j++) {
#pragma HLS unroll
            ConvertDepthSingle<SrcType, DstType, CONV_POLICY, SHIFT>(src.pixel[j], dst.pixel[j]);
        }

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<DstType, VEC_SIZE>((i == 0), (i == VEC_PIXEL - 1), dst);

        // Write result
        output[i] = dst;
    }
}
template <typename SrcType, typename DstType, const vx_uint8 VEC_SIZE, const vx_uint32 IMG_PIXEL,
          const vx_convert_policy_e CONV_POLICY, const vx_uint16 SHIFT>
void ConvertBitDepth(SrcType input[IMG_PIXEL], DstType output[IMG_PIXEL]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 VEC_PIXEL = ConvertBitDepthCheck<SrcType, DstType, VEC_SIZE, IMG_PIXEL>();

    // Vector variables
    SrcType src = 0;
    DstType dst = 0;

    // Computes operations (pipelined)
    for (vx_uint32 i = 0; i < VEC_PIXEL; i++) {
#pragma HLS PIPELINE II = 1

        // Read Input
        src = input[i];

        // Compute bit depth operation
        ConvertDepthSingle<SrcType, DstType, CONV_POLICY, SHIFT>(src, dst);

        // Write result
        output[i] = dst;
    }
}

/*********************************************************************************************************************/
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void ConverterSrcMulitple(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                          vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const vx_int64 VECTOR = SRC_PIXELS / SRC_PARALLEL;
    const vx_int64 PARALLEL = MAX(SRC_PARALLEL / DST_PARALLEL, 1);

    // Input buffer
    vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
    //#pragma HLS array_partition variable = inputbuffer complete dim = 0

    // Copy elements
    for (vx_int64 batch = 0, src_ptr = 0, dst_ptr = 0; batch < BATCHES; ++batch) {
        for (vx_int64 i = 0; i < VECTOR * PARALLEL; ++i) {
            // for (vx_int64 j = 0; j < PARALLEL; ++j) {
#pragma HLS pipeline II = 1
            vx_int64 j = i % PARALLEL;

            // Output buffer
            vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
            //#pragma HLS array_partition variable = outputbuffer complete dim = 0

            // Input
            if (j == 0) {
                inputbuffer = src[src_ptr];
                ++src_ptr;
            }

            // Conversion
            for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
                outputbuffer.pixel[k] = inputbuffer.pixel[j * DST_PARALLEL + k];

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<TypeData, DST_PARALLEL>((dst_ptr == 0), (dst_ptr == VECTOR - 1), outputbuffer);

            // Output
            dst[dst_ptr] = outputbuffer;
            ++dst_ptr;
            //}
        }
    }
}

/*********************************************************************************************************************/
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void ConverterDstMulitple(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                          vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const vx_int64 VECTOR = DST_PIXELS / DST_PARALLEL;
    const vx_int64 PARALLEL = MAX(DST_PARALLEL / SRC_PARALLEL, 1);

    // Output buffer
    vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
    //#pragma HLS array_partition variable = outputbuffer complete dim = 0

    // Copy elements
    for (vx_int64 batch = 0, src_ptr = 0, dst_ptr = 0; batch < BATCHES; ++batch) {
        for (vx_int64 i = 0; i < VECTOR * PARALLEL; ++i) {
            // for (vx_int64 j = 0; j < PARALLEL; ++j) {
#pragma HLS pipeline II = 1
            vx_int64 j = i % PARALLEL;

            // Input buffer
            vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
            //#pragma HLS array_partition variable = inputbuffer complete dim = 0

            // Input
            inputbuffer = src[src_ptr];
            ++src_ptr;

            // Conversion
            for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
                outputbuffer.pixel[j * SRC_PARALLEL + k] = inputbuffer.pixel[k];

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<TypeData, DST_PARALLEL>((dst_ptr == 0), (dst_ptr == VECTOR - 1), outputbuffer);

            // Output
            if (j == (PARALLEL - 1)) {
                dst[dst_ptr] = outputbuffer;
                ++dst_ptr;
            }
            //}
        }
    }
}

/*********************************************************************************************************************/
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void ConverterPassThrough(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                          vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const vx_int64 VECTOR = SRC_PIXELS / SRC_PARALLEL;
    const vx_int64 PARALLEL = MIN(DST_PARALLEL, SRC_PARALLEL);

    // Copy elements
    for (vx_int64 batch = 0, ptr = 0; batch < BATCHES; ++batch) {
        for (vx_int64 i = 0; i < VECTOR; ++i) {
#pragma HLS pipeline II = 1

            // Input buffer
            vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
            //#pragma HLS array_partition variable = inputbuffer complete dim = 0

            // Output buffer
            vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
            //#pragma HLS array_partition variable = outputbuffer complete dim = 0

            // Input
            inputbuffer = src[ptr];

            // Conversion ;)
            for (vx_int64 k = 0; k < PARALLEL; ++k)
                outputbuffer.pixel[k] = inputbuffer.pixel[k];

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<TypeData, DST_PARALLEL>((ptr == 0), (ptr == VECTOR - 1), outputbuffer);

            // Output
            dst[ptr] = outputbuffer;
            ++ptr;
        }
    }
}

/*********************************************************************************************************************/
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void ConvertSrcBigger(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                      vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const vx_int64 DST_VECTOR = DST_PIXELS / DST_PARALLEL;
    const vx_int64 SRC_VECTOR = SRC_PIXELS / SRC_PARALLEL;
    const vx_int64 BUFFER_SIZE = MIN(SRC_PARALLEL * DST_PARALLEL, MAX(DST_PIXELS, SRC_PIXELS));
    const vx_int64 VECTOR = MAX((BATCHES * DST_PIXELS) / (SRC_PARALLEL * DST_PARALLEL),
                                (BATCHES * SRC_PIXELS) / (SRC_PARALLEL * DST_PARALLEL));

    // Long buffer to overcome the SRC/DST PARALLEL missmatch
    TypeData buffer[BUFFER_SIZE]; // TODO: Initialize
#pragma HLS array_partition variable = buffer complete dim = 0

    // Iteration
    for (vx_int64 i = 0, src_ptr = 0, dst_ptr = 0; i < VECTOR; ++i) {
        for (vx_int64 j = 0; j < SRC_PARALLEL; ++j) {
#pragma HLS pipeline II = 1

            // Input buffer
            vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
            //#pragma HLS array_partition variable = inputbuffer complete dim = 0

            // Output buffer
            vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
            //#pragma HLS array_partition variable = outputbuffer complete dim = 0

            // Read data from input and write to buffer
            if ((j < DST_PARALLEL) && (src_ptr < (BATCHES * SRC_VECTOR))) {

                inputbuffer = src[src_ptr];
                ++src_ptr;

                for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
                    buffer[j * SRC_PARALLEL + k] = inputbuffer.pixel[k];
            }

            // Read data from buffer and write to output
            if ((j < SRC_PARALLEL) && (dst_ptr < (BATCHES * DST_VECTOR))) {

                for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
                    outputbuffer.pixel[k] = buffer[j * DST_PARALLEL + k];

                // Set user(SOF) & last(EOF)
                GenerateDmaSignal<TypeData, DST_PARALLEL>((dst_ptr == 0), (dst_ptr == DST_VECTOR - 1), outputbuffer);

                dst[dst_ptr] = outputbuffer;
                ++dst_ptr;
            }
        }
    }
}

/*********************************************************************************************************************/
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void ConvertDstBigger(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                      vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const vx_int64 GCD_PARALLEL = GCD(DST_PARALLEL, SRC_PARALLEL);
    const vx_int64 LCM_PARALLEL = LCM(DST_PARALLEL, SRC_PARALLEL);
    const vx_int64 BUFFER_SIZE = MIN(LCM_PARALLEL, MAX(DST_PIXELS, SRC_PIXELS));
    const vx_int64 DST_VECTOR = DST_PIXELS / DST_PARALLEL;
    const vx_int64 SRC_VECTOR = SRC_PIXELS / SRC_PARALLEL;
    const vx_int64 VECTOR = MAX(SRC_VECTOR, DST_VECTOR);

    // Long buffer to overcome the SRC/DST PARALLEL missmatch
    TypeData buffer[BUFFER_SIZE];
#pragma HLS array_partition variable = buffer complete dim = 0

    // Itaeration
    for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
        for (vx_int64 src_ptr = 0, dst_ptr = 0; (src_ptr < VECTOR); ++src_ptr) {
#pragma HLS pipeline II = 1

            // Input buffer
            vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
            //#pragma HLS array_partition variable = inputbuffer complete dim = 0

            // Output buffer
            vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
            //#pragma HLS array_partition variable = outputbuffer complete dim = 0

            // Buffer pointer
            const vx_int64 src_cnt = (src_ptr % (DST_PARALLEL / GCD_PARALLEL)) * SRC_PARALLEL;
            const vx_int64 dst_cnt = (dst_ptr % (SRC_PARALLEL / GCD_PARALLEL)) * DST_PARALLEL;

            // Read data from input and write to buffer
            if (src_ptr < SRC_VECTOR) {

                inputbuffer = src[batch * SRC_VECTOR + src_ptr];

                for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
                    buffer[src_cnt + k] = inputbuffer.pixel[k];
            }

            // Read data from buffer and write to output
            if (dst_ptr < DST_VECTOR) {
                if ((src_cnt + SRC_PARALLEL) >= (dst_cnt + DST_PARALLEL)) {

                    for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
                        outputbuffer.pixel[k] = buffer[dst_cnt + k];

                    // Set user(SOF) & last(EOF)
                    GenerateDmaSignal<TypeData, DST_PARALLEL>((dst_ptr == 0), (dst_ptr == DST_VECTOR - 1),
                                                              outputbuffer);

                    dst[batch * DST_VECTOR + dst_ptr] = outputbuffer;
                    ++dst_ptr;
                }
            }
        }
    }
}

/*********************************************************************************************************************/
// Convert between vector sizes of a stream
template <typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL,
          vx_int64 DST_PARALLEL>
void DataWidthConverter(vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
                        vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

    // Constants
    const bool MULTIPLE =
        (SRC_PARALLEL > DST_PARALLEL) ? ((SRC_PARALLEL % DST_PARALLEL) == 0) : ((DST_PARALLEL % SRC_PARALLEL) == 0);

    // Compile time checks
    STATIC_ASSERT((BATCHES > 0) && (SRC_PIXELS > 0) && (DST_PIXELS > 0) && (SRC_PARALLEL > 0) && (DST_PARALLEL > 0),
                  ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
    STATIC_ASSERT(((SRC_PIXELS % SRC_PARALLEL) == 0) && (SRC_PIXELS >= SRC_PARALLEL),
                  PIXELS_ARE_NOT_MULTIPLE_OF_SRC_PARALLEL);
    STATIC_ASSERT(((DST_PIXELS % DST_PARALLEL) == 0) && (DST_PIXELS >= DST_PARALLEL),
                  PIXELS_ARE_NOT_MULTIPLE_OF_DST_PARALLEL);

    // Down Conversion
    if (SRC_PARALLEL > DST_PARALLEL) {
        if (MULTIPLE == true) {
            ConverterSrcMulitple<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
        } else {
            ConvertSrcBigger<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
        }

        // Up Conversion
    } else if (DST_PARALLEL > SRC_PARALLEL) {
        if (MULTIPLE == true) {
            ConverterDstMulitple<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
        } else {
            ConvertDstBigger<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
        }

        // Pass through
    } else if (SRC_PARALLEL == DST_PARALLEL) {
        ConverterPassThrough<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
    }
}

/*********************************************************************************************************************/
// Takes 1 image and sends it to multiple ports
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 OUT_NUM>
void Multicast(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],
               vx_image_data<DataType, VEC_NUM> out1[IMG_PIXELS / VEC_NUM],
               vx_image_data<DataType, VEC_NUM> out2[IMG_PIXELS / VEC_NUM],
               vx_image_data<DataType, VEC_NUM> out3[IMG_PIXELS / VEC_NUM],
               vx_image_data<DataType, VEC_NUM> out4[IMG_PIXELS / VEC_NUM]) {
#pragma HLS INLINE

    for (vx_uint32 i = 0; i < (IMG_PIXELS / VEC_NUM); i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<DataType, VEC_NUM> src = input[i];

        // Set user(SOF) & last(EOF)
        GenerateDmaSignal<DataType, VEC_NUM>((i == 0), (i == (IMG_PIXELS / VEC_NUM - 1)), src);

        if (OUT_NUM >= 1)
            out1[i] = src;
        if (OUT_NUM >= 2)
            out2[i] = src;
        if (OUT_NUM >= 3)
            out3[i] = src;
        if (OUT_NUM >= 4)
            out4[i] = src;
    }
}

/*********************************************************************************************************************/
// Takes 1 image and sends it to multiple ports
template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 OUT_NUM>
void Multicast(DataType input[IMG_PIXELS], DataType out1[IMG_PIXELS], DataType out2[IMG_PIXELS],
               DataType out3[IMG_PIXELS], DataType out4[IMG_PIXELS]) {
#pragma HLS INLINE

    STATIC_ASSERT(VEC_NUM == 1, USE_THE_VX_IMAGE_DATA_TYPE_FOR_VEC_NUM_BIGGER_1);

    for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II = 1
        DataType src = input[i];

        if (OUT_NUM >= 1)
            out1[i] = src;
        if (OUT_NUM >= 2)
            out2[i] = src;
        if (OUT_NUM >= 3)
            out3[i] = src;
        if (OUT_NUM >= 4)
            out4[i] = src;
    }
}

/*********************************************************************************************************************/
/** @brief Resize the image down to nearest neighbor
 * @param COLS_IN     The columns of the input image
 * @param ROWS_IN     The rows of the input image
 * @param COLS_OUT    The columns of the output image
 * @param ROWS_OUT    The rows of the output image
 * @param input       The input image
 * @param output      The output image
 */
template <vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT>
void ScaleDownNearest(vx_image_data<vx_uint8, 1> input[COLS_IN * ROWS_IN],
                      vx_image_data<vx_uint8, 1> output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE

    // Compute the scale factor of the resized image in x and y direction
    const vx_uint32 ACCURACY = 14;
    const vx_uint32 COLS_SCALE =
        (vx_uint32)(((vx_float64)COLS_IN / (vx_float64)COLS_OUT) * std::pow(2, ACCURACY) + 0.5);
    const vx_uint32 ROWS_SCALE =
        (vx_uint32)(((vx_float64)ROWS_IN / (vx_float64)ROWS_OUT) * std::pow(2, ACCURACY) + 0.5);

    // Compute the resized image
    for (vx_uint32 y = 0, y_dst = 0, y_dst_next = 0; y < ROWS_IN; y++) {
        for (vx_uint32 x = 0, x_dst = 0; x < COLS_IN; x++) {
#pragma HLS PIPELINE II = 1

            vx_image_data<vx_uint8, 1> src;
            vx_image_data<vx_uint8, 1> dst;

            // Get next input pixel in a stream
            src = input[y * COLS_IN + x];
            vx_uint8 data = src.pixel[0];

            // Compute the coordinates of the next needed input pixel
            // (vx_int32)floor(_dst + 0.5f)*(COLS_IN/COLS_OUT) - 0.5f) using floating point computation
            vx_uint32 x_src = (((((x_dst << 1) + 1) * COLS_SCALE) >> ACCURACY) - 1) >> 1;
            // (vx_int32)floor((y_dst + 0.5f)*(ROWS_IN/ROWS_OUT) - 0.5f) using floating point computation
            vx_uint32 y_src = (((((y_dst << 1) + 1) * ROWS_SCALE) >> ACCURACY) - 1) >> 1;

            // Check if current input pixel is the needed one
            if ((y_src == y) && (x_src == x)) {

                // Set user(SOF) & last(EOF)
                GenerateDmaSignal<vx_uint8, 1>((x_dst == 0 && y_dst == 0),
                                               (x_dst == (COLS_OUT - 1) && y_dst == (ROWS_OUT - 1)), dst);

                // Write the ouput pixel to stream
                dst.pixel[0] = data;
                output[y_dst * COLS_OUT + x_dst] = dst;

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

/*********************************************************************************************************************/
/** @brief Resize the image down using bilinear interpolation
 * @param COLS_IN     The columns of the input image
 * @param ROWS_IN     The rows of the input image
 * @param COLS_OUT    The columns of the output image
 * @param ROWS_OUT    The rows of the output image
 * @param input       The input image
 * @param output      The output image
 */
template <vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT>
void ScaleDownBilinear(vx_image_data<vx_uint8, 1> input[COLS_IN * ROWS_IN],
                       vx_image_data<vx_uint8, 1> output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE

    // Compute constants
    const vx_uint32 ACCURACY = 12; // Minimum 12, Maximum 15
    const vx_uint32 ONE = (1 << ACCURACY);
    const vx_uint32 MASK_FRAKTION = (1 << (ACCURACY + 2)) - 1;
    const vx_uint32 COLS_SCALE =
        (vx_uint32)(((vx_float64)COLS_IN / (vx_float64)COLS_OUT) * std::pow(2, ACCURACY) + 0.5);
    const vx_uint32 ROWS_SCALE =
        (vx_uint32)(((vx_float64)ROWS_IN / (vx_float64)ROWS_OUT) * std::pow(2, ACCURACY) + 0.5);

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
#pragma HLS PIPELINE II = 1

            vx_image_data<vx_uint8, 1> src;
            vx_image_data<vx_uint8, 1> dst;

            vx_uint8 tl, tr, bl, br;

            // Buffer the previous input pixel [x-1]
            vx_uint8 data_old = data_new;

            // Read Input Pixel from Stream
            if (x < COLS_IN && y < ROWS_IN) {
                in_cnt++;
                src = input[y * COLS_IN + x];
                data_new = src.pixel[0];
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
            vx_uint16 tr_part = (vx_uint16)(((((x_fract) * (ONE - y_fract)) >> 10) * (vx_uint32)tr) >> 8);
            vx_uint16 bl_part = (vx_uint16)(((((ONE - x_fract) * (y_fract)) >> 10) * (vx_uint32)bl) >> 8);
            vx_uint16 br_part = (vx_uint16)(((((x_fract) * (y_fract)) >> 10) * (vx_uint32)br) >> 8);
            vx_uint16 sum = (tl_part + tr_part + bl_part + br_part) >> (2 * ACCURACY - 10 - 8);
            vx_uint8 result = (vx_uint8)(MIN(sum, (vx_uint16)(255)));

            // Check if the input pixel are the needed ones
            if ((x_r == x) && (y_b == y)) {
                out_cnt++;

                // Set user(SOF) & last(EOF)
                GenerateDmaSignal<vx_uint8, 1>((x_dst == 0 && y_dst == 0),
                                               (x_dst == (COLS_OUT - 1) && y_dst == (ROWS_OUT - 1)), dst);

                // Write ouput pixel to stream
                dst.pixel[0] = result;
                output[y_dst * COLS_OUT + x_dst] = dst;

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

/*********************************************************************************************************************/
/** @brief Resize the image down using bilinear interpolation
 * @param COLS_IN     The columns of the input image
 * @param ROWS_IN     The rows of the input image
 * @param COLS_OUT    The columns of the output image
 * @param ROWS_OUT    The rows of the output image
 * @param input       The input image
 * @param output      The output image
 */
template <vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT>
void ScaleDownArea(vx_image_data<vx_uint8, 1> input[COLS_IN * ROWS_IN],
                   vx_image_data<vx_uint8, 1> output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE
    // Using all pixels partly covered by bounded area weighted evenly

    // Compute constants
    const vx_uint32 ACCURACY = 14;
    const vx_uint32 ERROR_MARGIN = 2;
    const vx_uint32 ONE = (1 << ACCURACY);
    const vx_uint32 MASK_FRAKTION = ONE - 1;
    const vx_uint32 COLS_SCALE =
        (vx_uint32)(((vx_float64)COLS_IN / (vx_float64)COLS_OUT) * (vx_float64)(1 << ACCURACY) + 0.5);
    const vx_uint32 ROWS_SCALE =
        (vx_uint32)(((vx_float64)ROWS_IN / (vx_float64)ROWS_OUT) * (vx_float64)(1 << ACCURACY) + 0.5);

    const vx_uint32 max_window_x =
        (vx_uint32)(((((COLS_SCALE & MASK_FRAKTION) == 0) ? COLS_SCALE : COLS_SCALE + ONE) >> ACCURACY) + 1);
    const vx_uint32 max_window_y =
        (vx_uint32)(((((ROWS_SCALE & MASK_FRAKTION) == 0) ? ROWS_SCALE : ROWS_SCALE + ONE) >> ACCURACY) + 1);

    // STATIC_ASSERT(max_window_y > 0, window_y_not_greater_than_zero);
    // STATIC_ASSERT(max_window_x > 0, window_x_not_greater_than_zero);

    // input buffer
    vx_uint8 linebuffer[max_window_y - 1][COLS_IN];
#pragma HLS array_partition variable = linebuffer complete dim = 1
    vx_uint8 right_side[max_window_y];
#pragma HLS array_partition variable = right_side complete dim = 0
    vx_uint8 window[max_window_y][max_window_x];
#pragma HLS array_partition variable = window complete dim = 0

    // precompute normalization parameters
    vx_uint64 kernel_mult_array[4];
    vx_uint32 kernel_shift_array[4];
    vx_uint64 kernel_mult = 1;
    vx_uint32 kernel_shift = 0;

    const vx_uint32 w1 = max_window_y * max_window_x;
    const vx_uint32 w2 = (max_window_y - 1) * max_window_x;
    const vx_uint32 w3 = max_window_y * (max_window_x - 1);
    const vx_uint32 w4 = (max_window_y - 1) * (max_window_x - 1);

    ComputeNormalization(w1, kernel_mult, kernel_shift);
    kernel_mult_array[0] = kernel_mult;
    kernel_shift_array[0] = kernel_shift;
    ComputeNormalization(w2, kernel_mult, kernel_shift);
    kernel_mult_array[1] = kernel_mult;
    kernel_shift_array[1] = kernel_shift;
    ComputeNormalization(w3, kernel_mult, kernel_shift);
    kernel_mult_array[2] = kernel_mult;
    kernel_shift_array[2] = kernel_shift;
    ComputeNormalization(w4, kernel_mult, kernel_shift);
    kernel_mult_array[3] = kernel_mult;
    kernel_shift_array[3] = kernel_shift;

    for (vx_uint32 y = 0, y_dst = 0; y < ROWS_IN; ++y) {
        for (vx_uint32 x = 0, x_dst = 0; x < COLS_IN; ++x) {
#pragma HLS PIPELINE II = 1
            // do we have enough pixels in y direction to calculate the next
            // output pixel
            vx_uint32 y_dst_plus = y_dst + 1;
            vx_uint32 y_thresh = (vx_uint32)(((((vx_uint64)y_dst_plus) << ACCURACY) * ROWS_SCALE) >> ACCURACY);
            bool y_next = ((((y + 1) << ACCURACY) >> ERROR_MARGIN) >= (y_thresh >> ERROR_MARGIN));

            vx_image_data<vx_uint8, 1> src;
            vx_image_data<vx_uint8, 1> dst;

            src = input[y * COLS_IN + x];
            vx_uint8 data = src.pixel[0];

            // shift buffer
            right_side[0] = linebuffer[0][x];
            for (vx_uint16 y = 1; y < max_window_y - 1; ++y) {
                right_side[y] = linebuffer[y - 1][x] = linebuffer[y][x];
            }
            right_side[max_window_y - 1] = data;
            linebuffer[max_window_y - 2][x] = data;
            // shift window
            for (vx_uint16 win_y = 0; win_y < max_window_y; ++win_y) {
                for (vx_uint16 win_x = 0; win_x < max_window_x - 1; ++win_x) {
                    window[win_y][win_x] = window[win_y][win_x + 1];
                }
            }
            // update right side
            for (vx_uint16 win_y = 0; win_y < max_window_y; ++win_y) {
                window[win_y][max_window_x - 1] = right_side[win_y];
            }

            // do we have enough pixels in x direction to calculate the next
            // output pixel
            vx_uint32 x_dst_plus = x_dst + 1;
            vx_uint32 x_thresh = (vx_uint32)(((((vx_uint64)x_dst_plus) << ACCURACY) * COLS_SCALE) >> ACCURACY);
            bool x_next = ((((x + 1) << ACCURACY) >> ERROR_MARGIN) >= (x_thresh >> ERROR_MARGIN));

            if (x_next && y_next) { // calculate next output pixel
                // find window borders in x direction
                vx_uint32 x_floor =
                    (vx_uint32)(((((vx_uint64)x_dst << ACCURACY) * COLS_SCALE) >> ACCURACY) >> ACCURACY);
                vx_uint32 x_tmp = (vx_uint32)((((vx_uint64)x_dst_plus << ACCURACY) * COLS_SCALE) >> ACCURACY);
                vx_uint32 x_ceil =
                    (x_tmp >> ACCURACY) + ((((x_tmp >> ERROR_MARGIN) & (MASK_FRAKTION >> ERROR_MARGIN)) == 0) ? 0 : 1);
                vx_uint32 x_offset = max_window_x - (x_ceil - x_floor);

                // find window borders in y direction
                vx_uint32 y_floor =
                    (vx_uint32)(((((vx_uint64)y_dst << ACCURACY) * ROWS_SCALE) >> ACCURACY) >> ACCURACY);
                vx_uint32 y_tmp = (vx_uint32)((((vx_uint64)y_dst_plus << ACCURACY) * ROWS_SCALE) >> ACCURACY);
                vx_uint32 y_ceil =
                    (y_tmp >> ACCURACY) + ((((y_tmp >> ERROR_MARGIN) & (MASK_FRAKTION >> ERROR_MARGIN)) == 0) ? 0 : 1);
                vx_uint32 y_offset = max_window_y - (y_ceil - y_floor);

                // calculate output
                vx_uint32 tmp = 0;
                vx_uint32 norm = (y_ceil - y_floor) * (x_ceil - x_floor);
                for (vx_uint32 y_win = 0; y_win < max_window_y; ++y_win) {
#pragma HLS UNROLL
                    for (vx_uint32 x_win = 0; x_win < max_window_x; ++x_win) {
#pragma HLS UNROLL
                        if ((y_win + y_offset < max_window_y) && (x_win + x_offset < max_window_x))
                            tmp += window[y_win + y_offset][x_win + x_offset];
                    }
                }
                if (norm == w1) {
                    kernel_mult = kernel_mult_array[0];
                    kernel_shift = kernel_shift_array[0];
                } else if (norm == w2) {
                    kernel_mult = kernel_mult_array[1];
                    kernel_shift = kernel_shift_array[1];
                } else if (norm == w3) {
                    kernel_mult = kernel_mult_array[2];
                    kernel_shift = kernel_shift_array[2];
                } else if (norm == w4) {
                    kernel_mult = kernel_mult_array[3];
                    kernel_shift = kernel_shift_array[3];
                }
                vx_uint32 result = (tmp * kernel_mult) >> kernel_shift;

                // Set user(SOF) & last(EOF)
                GenerateDmaSignal<vx_uint8, 1>((x_dst == 0 && y_dst == 0),
                                               (x_dst == (COLS_OUT - 1) && y_dst == (ROWS_OUT - 1)), dst);

                // Write ouput pixel to stream
                dst.pixel[0] = result;
                output[y_dst * COLS_OUT + x_dst] = dst;

                x_dst += 1;
            }
            if (y_next && x == (COLS_IN - 1)) {
                y_dst += 1;
            }
        }
    }
}

/*********************************************************************************************************************/
/** @brief Scale an image down using area, bilinear or nearest neighbor
 * interpolation
 * @param COLS_IN     The columns of the input image
 * @param ROWS_IN     The rows of the input image
 * @param COLS_OUT    The columns of the output image
 * @param ROWS_OUT    The rows of the output image
 * @param SCALE_TYPE  The type of interpolation
 * (VX_INTERPOLATION_NEAREST_NEIGHBOR or VX_INTERPOLATION_BILINEAR or
 * VX_INTERPOLATION_AREA)
 * @param input       The input image
 * @param output      The output image
 */
template <vx_uint16 COLS_IN, vx_uint16 ROWS_IN, vx_uint16 COLS_OUT, vx_uint16 ROWS_OUT,
          vx_interpolation_type_e SCALE_TYPE>
void ScaleImage(vx_image_data<vx_uint8, 1> input[COLS_IN * ROWS_IN],
                vx_image_data<vx_uint8, 1> output[COLS_OUT * ROWS_OUT]) {
#pragma HLS INLINE

    // Check if output image resolution is smaller than input image resolution
    STATIC_ASSERT(COLS_IN >= COLS_OUT, only_down_scale_of_image_supported);
    STATIC_ASSERT(ROWS_IN >= ROWS_OUT, only_down_scale_of_image_supported);
    STATIC_ASSERT(SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR || SCALE_TYPE == VX_INTERPOLATION_BILINEAR ||
                      SCALE_TYPE == VX_INTERPOLATION_AREA,
                  scale_type_not_supported);

    // Nearest neighbor interpolation
    if (SCALE_TYPE == VX_INTERPOLATION_NEAREST_NEIGHBOR) {
        ScaleDownNearest<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT>(input, output);

        // Bilinear Interpolation
    } else if (SCALE_TYPE == VX_INTERPOLATION_BILINEAR) {
        ScaleDownBilinear<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT>(input, output);

        // Area Interpolation
    } else if (SCALE_TYPE == VX_INTERPOLATION_AREA) {
        ScaleDownArea<COLS_IN, ROWS_IN, COLS_OUT, ROWS_OUT>(input, output);
    }
}

/*********************************************************************************************************************/
/* Takes one image and scatters it to multiple outputs */

template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 FACTOR,
          HIFLIPVX::vx_scatter_mode_e MODE>        //
void Scatter(DataType input[IMG_PIXELS],           //
             DataType out1[IMG_PIXELS / FACTOR],   //
             DataType out2[IMG_PIXELS / FACTOR],   //
             DataType out3[IMG_PIXELS / FACTOR],   //
             DataType out4[IMG_PIXELS / FACTOR],   //
             DataType out5[IMG_PIXELS / FACTOR],   //
             DataType out6[IMG_PIXELS / FACTOR],   //
             DataType out7[IMG_PIXELS / FACTOR],   //
             DataType out8[IMG_PIXELS / FACTOR]) { //
#pragma HLS INLINE

    // check parameters
    STATIC_ASSERT(VEC_NUM == 1, USE_THE_VX_IMAGE_DATA_TYPE_FOR_VEC_NUM_BIGGER_1);
    STATIC_ASSERT(FACTOR > 1, CAN_NOT_SCATTER_TO_ONLY_ONE_OUTPUT);
    STATIC_ASSERT(FACTOR < 9, CAN_NOT_SCATTER_TO_MORE_THAN_EIGHT_OUTPUTS);
    STATIC_ASSERT(IMG_PIXELS % FACTOR == 0, CHOOSEN_FACTOR_DIVIDES_NUMBER_OF_PIXELS_WITH_REST);
    STATIC_ASSERT(MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK || MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC,
                  INVALID_SCATTER_MODE);

    for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // read input
        DataType src = input[i];

        // cyclic mode
        if (HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC == MODE) {

            // write/select output
            if (FACTOR >= 1 && i % FACTOR == 0)
                out1[i / FACTOR] = src;
            if (FACTOR >= 2 && i % FACTOR == 1)
                out2[i / FACTOR] = src;
            if (FACTOR >= 3 && i % FACTOR == 2)
                out3[i / FACTOR] = src;
            if (FACTOR >= 4 && i % FACTOR == 3)
                out4[i / FACTOR] = src;
            if (FACTOR >= 5 && i % FACTOR == 4)
                out5[i / FACTOR] = src;
            if (FACTOR >= 6 && i % FACTOR == 5)
                out6[i / FACTOR] = src;
            if (FACTOR >= 7 && i % FACTOR == 6)
                out7[i / FACTOR] = src;
            if (FACTOR >= 8 && i % FACTOR == 7)
                out8[i / FACTOR] = src;
        }

        // block mode
        else if (HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK == MODE) {

            // write/select output
            if (FACTOR >= 1 && (i / (IMG_PIXELS / FACTOR)) == 0)
                out1[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 2 && (i / (IMG_PIXELS / FACTOR)) == 1)
                out2[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 3 && (i / (IMG_PIXELS / FACTOR)) == 2)
                out3[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 4 && (i / (IMG_PIXELS / FACTOR)) == 3)
                out4[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 5 && (i / (IMG_PIXELS / FACTOR)) == 4)
                out5[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 6 && (i / (IMG_PIXELS / FACTOR)) == 5)
                out6[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 7 && (i / (IMG_PIXELS / FACTOR)) == 6)
                out7[i % (IMG_PIXELS / FACTOR)] = src;
            if (FACTOR >= 8 && (i / (IMG_PIXELS / FACTOR)) == 7)
                out8[i % (IMG_PIXELS / FACTOR)] = src;
        }
    }
}

template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 FACTOR,
          HIFLIPVX::vx_scatter_mode_e MODE>                                            //
void Scatter(vx_image_data<DataType, VEC_NUM> input[IMG_PIXELS / VEC_NUM],             //
             vx_image_data<DataType, VEC_NUM> out1[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out2[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out3[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out4[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out5[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out6[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out7[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
             vx_image_data<DataType, VEC_NUM> out8[(IMG_PIXELS / VEC_NUM) / FACTOR]) { //
#pragma HLS INLINE

    // check parameters
    STATIC_ASSERT(FACTOR > 1, CAN_NOT_SCATTER_TO_ONLY_ONE_OUTPUT);
    STATIC_ASSERT(FACTOR < 9, CAN_NOT_SCATTER_TO_MORE_THAN_EIGHT_OUTPUTS);
    STATIC_ASSERT((IMG_PIXELS / VEC_NUM) % FACTOR == 0, CHOOSEN_FACTOR_DIVIDES_NUMBER_OF_PIXELS_WITH_REST);
    STATIC_ASSERT(MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK || MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC,
                  INVALID_SCATTER_MODE);

    for (vx_uint32 i = 0; i < (IMG_PIXELS / VEC_NUM); i++) {
#pragma HLS PIPELINE II = 1

        // read input
        vx_image_data<DataType, VEC_NUM> src = input[i];

        // cyclic mode
        if (HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC == MODE) {

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<DataType, VEC_NUM>((i < FACTOR), (i >= ((IMG_PIXELS / VEC_NUM) - FACTOR)), src);

            // write/select output
            if (FACTOR >= 1 && i % FACTOR == 0)
                out1[i / FACTOR] = src;
            if (FACTOR >= 2 && i % FACTOR == 1)
                out2[i / FACTOR] = src;
            if (FACTOR >= 3 && i % FACTOR == 2)
                out3[i / FACTOR] = src;
            if (FACTOR >= 4 && i % FACTOR == 3)
                out4[i / FACTOR] = src;
            if (FACTOR >= 5 && i % FACTOR == 4)
                out5[i / FACTOR] = src;
            if (FACTOR >= 6 && i % FACTOR == 5)
                out6[i / FACTOR] = src;
            if (FACTOR >= 7 && i % FACTOR == 6)
                out7[i / FACTOR] = src;
            if (FACTOR >= 8 && i % FACTOR == 7)
                out8[i / FACTOR] = src;
        }

        // block mode
        else if (HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK == MODE) {
            const vx_uint32 DST_PIXELS = (IMG_PIXELS / VEC_NUM) / FACTOR;

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<DataType, VEC_NUM>((0 == (i % DST_PIXELS)), ((DST_PIXELS - 1) == (i % DST_PIXELS)), src);

            // write/select output
            if (FACTOR >= 1 && (i / DST_PIXELS) == 0)
                out1[i % DST_PIXELS] = src;
            if (FACTOR >= 2 && (i / DST_PIXELS) == 1)
                out2[i % DST_PIXELS] = src;
            if (FACTOR >= 3 && (i / DST_PIXELS) == 2)
                out3[i % DST_PIXELS] = src;
            if (FACTOR >= 4 && (i / DST_PIXELS) == 3)
                out4[i % DST_PIXELS] = src;
            if (FACTOR >= 5 && (i / DST_PIXELS) == 4)
                out5[i % DST_PIXELS] = src;
            if (FACTOR >= 6 && (i / DST_PIXELS) == 5)
                out6[i % DST_PIXELS] = src;
            if (FACTOR >= 7 && (i / DST_PIXELS) == 6)
                out7[i % DST_PIXELS] = src;
            if (FACTOR >= 8 && (i / DST_PIXELS) == 7)
                out8[i % DST_PIXELS] = src;
        }
    }
}

/*********************************************************************************************************************/
/* Takes multiple inputs and gathers them into one image */

template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 FACTOR, HIFLIPVX::vx_scatter_mode_e MODE>
void Gather(DataType output[IMG_PIXELS],         //
            DataType in1[IMG_PIXELS / FACTOR],   //
            DataType in2[IMG_PIXELS / FACTOR],   //
            DataType in3[IMG_PIXELS / FACTOR],   //
            DataType in4[IMG_PIXELS / FACTOR],   //
            DataType in5[IMG_PIXELS / FACTOR],   //
            DataType in6[IMG_PIXELS / FACTOR],   //
            DataType in7[IMG_PIXELS / FACTOR],   //
            DataType in8[IMG_PIXELS / FACTOR]) { //
#pragma HLS INLINE

    // check parameters
    STATIC_ASSERT(VEC_NUM == 1, USE_THE_VX_IMAGE_DATA_TYPE_FOR_VEC_NUM_BIGGER_1);
    STATIC_ASSERT(FACTOR > 1, CAN_NOT_SCATTER_TO_ONLY_ONE_OUTPUT);
    STATIC_ASSERT(FACTOR < 9, CAN_NOT_SCATTER_TO_MORE_THAN_EIGHT_OUTPUTS);
    STATIC_ASSERT(IMG_PIXELS % FACTOR == 0, CHOOSEN_FACTOR_DIVIDES_NUMBER_OF_PIXELS_WITH_REST);
    STATIC_ASSERT(MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK || MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC,
                  INVALID_SCATTER_MODE);

    for (vx_uint32 i = 0; i < IMG_PIXELS; i++) {
#pragma HLS PIPELINE II = 1

        // input temp
        DataType src = 0;

        // cyclic mode
        if (HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC == MODE) {

            // select input
            if (FACTOR >= 1 && i % FACTOR == 0)
                src = in1[i / FACTOR];
            if (FACTOR >= 2 && i % FACTOR == 1)
                src = in2[i / FACTOR];
            if (FACTOR >= 3 && i % FACTOR == 2)
                src = in3[i / FACTOR];
            if (FACTOR >= 4 && i % FACTOR == 3)
                src = in4[i / FACTOR];
            if (FACTOR >= 5 && i % FACTOR == 4)
                src = in5[i / FACTOR];
            if (FACTOR >= 6 && i % FACTOR == 5)
                src = in6[i / FACTOR];
            if (FACTOR >= 7 && i % FACTOR == 6)
                src = in7[i / FACTOR];
            if (FACTOR >= 8 && i % FACTOR == 7)
                src = in8[i / FACTOR];
        }

        // block mode
        else if (HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK == MODE) {

            // select input
            if (FACTOR >= 1 && (i / (IMG_PIXELS / FACTOR)) == 0)
                src = in1[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 2 && (i / (IMG_PIXELS / FACTOR)) == 1)
                src = in2[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 3 && (i / (IMG_PIXELS / FACTOR)) == 2)
                src = in3[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 4 && (i / (IMG_PIXELS / FACTOR)) == 3)
                src = in4[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 5 && (i / (IMG_PIXELS / FACTOR)) == 4)
                src = in5[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 6 && (i / (IMG_PIXELS / FACTOR)) == 5)
                src = in6[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 7 && (i / (IMG_PIXELS / FACTOR)) == 6)
                src = in7[i % (IMG_PIXELS / FACTOR)];
            if (FACTOR >= 8 && (i / (IMG_PIXELS / FACTOR)) == 7)
                src = in8[i % (IMG_PIXELS / FACTOR)];
        }

        // write output
        output[i] = src;
    }
}

template <typename DataType, vx_uint32 IMG_PIXELS, vx_uint8 VEC_NUM, vx_uint8 FACTOR, HIFLIPVX::vx_scatter_mode_e MODE>
void Gather(vx_image_data<DataType, VEC_NUM> output[IMG_PIXELS / VEC_NUM],           //
            vx_image_data<DataType, VEC_NUM> in1[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in2[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in3[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in4[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in5[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in6[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in7[(IMG_PIXELS / VEC_NUM) / FACTOR],   //
            vx_image_data<DataType, VEC_NUM> in8[(IMG_PIXELS / VEC_NUM) / FACTOR]) { //
#pragma HLS INLINE

    // check parameters
    STATIC_ASSERT(FACTOR > 1, CAN_NOT_SCATTER_TO_ONLY_ONE_OUTPUT);
    STATIC_ASSERT(FACTOR < 9, CAN_NOT_SCATTER_TO_MORE_THAN_EIGHT_OUTPUTS);
    STATIC_ASSERT((IMG_PIXELS / VEC_NUM) % FACTOR == 0, CHOOSEN_FACTOR_DIVIDES_NUMBER_OF_PIXELS_WITH_REST);
    STATIC_ASSERT(MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK || MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC,
                  INVALID_SCATTER_MODE);

    for (vx_uint32 i = 0; i < (IMG_PIXELS / VEC_NUM); i++) {
#pragma HLS PIPELINE II = 1

        // input temp
        vx_image_data<DataType, VEC_NUM> src;

        // cyclic mode
        if (HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC == MODE) {

            // select input
            if (FACTOR >= 1 && i % FACTOR == 0)
                src = in1[i / FACTOR];
            if (FACTOR >= 2 && i % FACTOR == 1)
                src = in2[i / FACTOR];
            if (FACTOR >= 3 && i % FACTOR == 2)
                src = in3[i / FACTOR];
            if (FACTOR >= 4 && i % FACTOR == 3)
                src = in4[i / FACTOR];
            if (FACTOR >= 5 && i % FACTOR == 4)
                src = in5[i / FACTOR];
            if (FACTOR >= 6 && i % FACTOR == 5)
                src = in6[i / FACTOR];
            if (FACTOR >= 7 && i % FACTOR == 6)
                src = in7[i / FACTOR];
            if (FACTOR >= 8 && i % FACTOR == 7)
                src = in8[i / FACTOR];

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<DataType, VEC_NUM>((i == 0), (i == (IMG_PIXELS / VEC_NUM) - 1), src);
        }

        // block mode
        else if (HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK == MODE) {
            const vx_uint32 DST_PIXELS = (IMG_PIXELS / VEC_NUM) / FACTOR;

            // select input
            if (FACTOR >= 1 && (i / DST_PIXELS) == 0)
                src = in1[i % DST_PIXELS];
            if (FACTOR >= 2 && (i / DST_PIXELS) == 1)
                src = in2[i % DST_PIXELS];
            if (FACTOR >= 3 && (i / DST_PIXELS) == 2)
                src = in3[i % DST_PIXELS];
            if (FACTOR >= 4 && (i / DST_PIXELS) == 3)
                src = in4[i % DST_PIXELS];
            if (FACTOR >= 5 && (i / DST_PIXELS) == 4)
                src = in5[i % DST_PIXELS];
            if (FACTOR >= 6 && (i / DST_PIXELS) == 5)
                src = in6[i % DST_PIXELS];
            if (FACTOR >= 7 && (i / DST_PIXELS) == 6)
                src = in7[i % DST_PIXELS];
            if (FACTOR >= 8 && (i / DST_PIXELS) == 7)
                src = in8[i % DST_PIXELS];

            // Set user(SOF) & last(EOF)
            GenerateDmaSignal<DataType, VEC_NUM>((i == 0), (i == (IMG_PIXELS / VEC_NUM) - 1), src);
        }

        // write output
        output[i] = src;
    }
}

#endif // SRC_IMG_CONVERSION_CORE_H_
