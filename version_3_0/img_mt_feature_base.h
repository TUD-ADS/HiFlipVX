/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_base.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are accelerated feature (extract, deserialize, compare, gather) functions (Call from here)
 */

#ifndef __SRC_IMG_MT_FEATURE_BASE_H__
#define __SRC_IMG_MT_FEATURE_BASE_H__

#include "img_mt_feature_compare.h"
#include "img_mt_feature_deserialize.h"
#include "img_mt_feature_extract.h"
#include "img_mt_feature_gather.h"

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param MAX_KP_SRC12  Total amount of keypoints of the 12. array
 * @param MAX_KP_SRC13  Total amount of keypoints of the 13. array
 * @param MAX_KP_SRC14  Total amount of keypoints of the 14. array
 * @param MAX_KP_SRC15  Total amount of keypoints of the 15. array
 * @param MAX_KP_SRC16  Total amount of keypoints of the 16. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param input12       Input array of keypoints
 * @param input13       Input array of keypoints
 * @param input14       Input array of keypoints
 * @param input15       Input array of keypoints
 * @param input16       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint16 MAX_KEYPOINTS_INPUT_15,
          vx_uint16 MAX_KEYPOINTS_INPUT_16, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint input12[MAX_KEYPOINTS_INPUT_12],
                      KeyPoint input13[MAX_KEYPOINTS_INPUT_13], KeyPoint input14[MAX_KEYPOINTS_INPUT_14],
                      KeyPoint input15[MAX_KEYPOINTS_INPUT_15], KeyPoint input16[MAX_KEYPOINTS_INPUT_16],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<16, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_INPUT_16,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
        (vx_image_data<KeyPoint, 1> *)input11, (vx_image_data<KeyPoint, 1> *)input12,
        (vx_image_data<KeyPoint, 1> *)input13, (vx_image_data<KeyPoint, 1> *)input14,
        (vx_image_data<KeyPoint, 1> *)input15, (vx_image_data<KeyPoint, 1> *)input16,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint16 MAX_KEYPOINTS_INPUT_15,
          vx_uint16 MAX_KEYPOINTS_INPUT_16, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> input12[MAX_KEYPOINTS_INPUT_12],
    vx_image_data<KeyPoint, 1> input13[MAX_KEYPOINTS_INPUT_13],
    vx_image_data<KeyPoint, 1> input14[MAX_KEYPOINTS_INPUT_14],
    vx_image_data<KeyPoint, 1> input15[MAX_KEYPOINTS_INPUT_15],
    vx_image_data<KeyPoint, 1> input16[MAX_KEYPOINTS_INPUT_16],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12,
                      input13, input14, input15, input16, output);

    HwGatherKeypoints<16, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_INPUT_16,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(input1, input2, input3, input4, input5, input6, input7,
                                                          input8, input9, input10, input11, input12, input13, input14,
                                                          input15, input16, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param MAX_KP_SRC12  Total amount of keypoints of the 12. array
 * @param MAX_KP_SRC13  Total amount of keypoints of the 13. array
 * @param MAX_KP_SRC14  Total amount of keypoints of the 14. array
 * @param MAX_KP_SRC15  Total amount of keypoints of the 15. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param input12       Input array of keypoints
 * @param input13       Input array of keypoints
 * @param input14       Input array of keypoints
 * @param input15       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint16 MAX_KEYPOINTS_INPUT_15,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint input12[MAX_KEYPOINTS_INPUT_12],
                      KeyPoint input13[MAX_KEYPOINTS_INPUT_13], KeyPoint input14[MAX_KEYPOINTS_INPUT_14],
                      KeyPoint input15[MAX_KEYPOINTS_INPUT_15], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<15, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>((vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
                                    (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
                                    (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
                                    (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
                                    (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
                                    (vx_image_data<KeyPoint, 1> *)input11, (vx_image_data<KeyPoint, 1> *)input12,
                                    (vx_image_data<KeyPoint, 1> *)input13, (vx_image_data<KeyPoint, 1> *)input14,
                                    (vx_image_data<KeyPoint, 1> *)input15, NULL, (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint16 MAX_KEYPOINTS_INPUT_15,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> input12[MAX_KEYPOINTS_INPUT_12],
    vx_image_data<KeyPoint, 1> input13[MAX_KEYPOINTS_INPUT_13],
    vx_image_data<KeyPoint, 1> input14[MAX_KEYPOINTS_INPUT_14],
    vx_image_data<KeyPoint, 1> input15[MAX_KEYPOINTS_INPUT_15],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12,
                      input13, input14, input15, output);

    HwGatherKeypoints<15, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10,
                                    input11, input12, input13, input14, input15, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param MAX_KP_SRC12  Total amount of keypoints of the 12. array
 * @param MAX_KP_SRC13  Total amount of keypoints of the 13. array
 * @param MAX_KP_SRC14  Total amount of keypoints of the 14. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param input12       Input array of keypoints
 * @param input13       Input array of keypoints
 * @param input14       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint input12[MAX_KEYPOINTS_INPUT_12],
                      KeyPoint input13[MAX_KEYPOINTS_INPUT_13], KeyPoint input14[MAX_KEYPOINTS_INPUT_14],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<14, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
        (vx_image_data<KeyPoint, 1> *)input11, (vx_image_data<KeyPoint, 1> *)input12,
        (vx_image_data<KeyPoint, 1> *)input13, (vx_image_data<KeyPoint, 1> *)input14, NULL, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> input12[MAX_KEYPOINTS_INPUT_12],
    vx_image_data<KeyPoint, 1> input13[MAX_KEYPOINTS_INPUT_13],
    vx_image_data<KeyPoint, 1> input14[MAX_KEYPOINTS_INPUT_14],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12,
                      input13, input14, output);

    HwGatherKeypoints<14, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, input13,
        input14, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param MAX_KP_SRC12  Total amount of keypoints of the 12. array
 * @param MAX_KP_SRC13  Total amount of keypoints of the 13. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param input12       Input array of keypoints
 * @param input13       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint input12[MAX_KEYPOINTS_INPUT_12],
                      KeyPoint input13[MAX_KEYPOINTS_INPUT_13], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<13, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
        (vx_image_data<KeyPoint, 1> *)input11, (vx_image_data<KeyPoint, 1> *)input12,
        (vx_image_data<KeyPoint, 1> *)input13, NULL, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> input12[MAX_KEYPOINTS_INPUT_12],
    vx_image_data<KeyPoint, 1> input13[MAX_KEYPOINTS_INPUT_13],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12,
                      input13, output);

    HwGatherKeypoints<13, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                      MAX_KEYPOINTS_INPUT_13, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, input13,
        NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param MAX_KP_SRC12  Total amount of keypoints of the 12. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param input12       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint input12[MAX_KEYPOINTS_INPUT_12],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<12, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12, 0,
                      0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
        (vx_image_data<KeyPoint, 1> *)input11, (vx_image_data<KeyPoint, 1> *)input12, NULL, NULL, NULL, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> input12[MAX_KEYPOINTS_INPUT_12],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12,
                      output);

    HwGatherKeypoints<12, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12, 0,
                      0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(input1, input2, input3, input4, input5, input6,
                                                                   input7, input8, input9, input10, input11, input12,
                                                                   NULL, NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param MAX_KP_SRC11  Total amount of keypoints of the 11. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param input11       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint input11[MAX_KEYPOINTS_INPUT_11], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<11, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, 0, 0, 0, 0, 0,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10,
        (vx_image_data<KeyPoint, 1> *)input11, NULL, NULL, NULL, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> input11[MAX_KEYPOINTS_INPUT_11],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, output);

    HwGatherKeypoints<11, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, 0, 0, 0, 0, 0,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(input1, input2, input3, input4, input5, input6, input7,
                                                          input8, input9, input10, input11, NULL, NULL, NULL, NULL,
                                                          NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param MAX_KP_SRC10  Total amount of keypoints of the 10. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param input10       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint input10[MAX_KEYPOINTS_INPUT_10],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<10, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>((vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
                                    (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
                                    (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
                                    (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
                                    (vx_image_data<KeyPoint, 1> *)input9, (vx_image_data<KeyPoint, 1> *)input10, NULL,
                                    NULL, NULL, NULL, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9],
    vx_image_data<KeyPoint, 1> input10[MAX_KEYPOINTS_INPUT_10],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, output);

    HwGatherKeypoints<10, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10,
                                    NULL, NULL, NULL, NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param MAX_KP_SRC9  Total amount of keypoints of the 9. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param input9       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint input9[MAX_KEYPOINTS_INPUT_9], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<9, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8,
        (vx_image_data<KeyPoint, 1> *)input9, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> input9[MAX_KEYPOINTS_INPUT_9], vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, input9, output);

    HwGatherKeypoints<9, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                      MAX_KEYPOINTS_INPUT_9, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param MAX_KP_SRC8  Total amount of keypoints of the 8. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param input8       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint input8[MAX_KEYPOINTS_INPUT_8],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<8, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8, 0, 0,
                      0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, (vx_image_data<KeyPoint, 1> *)input8, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> input8[MAX_KEYPOINTS_INPUT_8],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, input8, output);

    HwGatherKeypoints<8, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8, 0, 0,
                      0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(input1, input2, input3, input4, input5,
                                                                            input6, input7, input8, NULL, NULL, NULL,
                                                                            NULL, NULL, NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param MAX_KP_SRC7  Total amount of keypoints of the 7. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param input7       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint input7[MAX_KEYPOINTS_INPUT_7], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<7, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6,
        (vx_image_data<KeyPoint, 1> *)input7, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3], vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
    vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5], vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
    vx_image_data<KeyPoint, 1> input7[MAX_KEYPOINTS_INPUT_7], vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, input7, output);

    HwGatherKeypoints<7, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(input1, input2, input3, input4, input5, input6, input7, NULL,
                                                          NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param MAX_KP_SRC6  Total amount of keypoints of the 6. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param input6       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint input6[MAX_KEYPOINTS_INPUT_6],
                      KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<6, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>((vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
                                    (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
                                    (vx_image_data<KeyPoint, 1> *)input5, (vx_image_data<KeyPoint, 1> *)input6, NULL,
                                    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                    (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint32 MAX_KEYPOINTS_OUTPUT, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1],
                      vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
                      vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3],
                      vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
                      vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5],
                      vx_image_data<KeyPoint, 1> input6[MAX_KEYPOINTS_INPUT_6],
                      vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, input6, output);

    HwGatherKeypoints<6, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT,
                      SCATTER_MODE>(input1, input2, input3, input4, input5, input6, NULL, NULL, NULL, NULL, NULL, NULL,
                                    NULL, NULL, NULL, NULL, output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param MAX_KP_SRC5  Total amount of keypoints of the 5. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param input5       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(KeyPoint input1[MAX_KEYPOINTS_INPUT_1], KeyPoint input2[MAX_KEYPOINTS_INPUT_2],
                      KeyPoint input3[MAX_KEYPOINTS_INPUT_3], KeyPoint input4[MAX_KEYPOINTS_INPUT_4],
                      KeyPoint input5[MAX_KEYPOINTS_INPUT_5], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwGatherKeypoints<5, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4,
        (vx_image_data<KeyPoint, 1> *)input5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void ImgFeatureGather(vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1],
                      vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
                      vx_image_data<KeyPoint, 1> input3[MAX_KEYPOINTS_INPUT_3],
                      vx_image_data<KeyPoint, 1> input4[MAX_KEYPOINTS_INPUT_4],
                      vx_image_data<KeyPoint, 1> input5[MAX_KEYPOINTS_INPUT_5],
                      vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, input5, output);

    HwGatherKeypoints<5, MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                      MAX_KEYPOINTS_INPUT_5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, MAX_KEYPOINTS_OUTPUT, SCATTER_MODE>(
        input1, input2, input3, input4, input5, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param MAX_KP_SRC4  Total amount of keypoints of the 4. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param input4       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint16 MAX_FEATURES_SRC3,
          vx_uint16 MAX_FEATURES_SRC4, vx_uint32 MAX_FEATURES_DST, HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(vx_image_data<KeyPoint, 1> input1[MAX_FEATURES_SRC1],
                      vx_image_data<KeyPoint, 1> input2[MAX_FEATURES_SRC2],
                      vx_image_data<KeyPoint, 1> input3[MAX_FEATURES_SRC3],
                      vx_image_data<KeyPoint, 1> input4[MAX_FEATURES_SRC4],
                      vx_image_data<KeyPoint, 1> output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, input4, output);

    HwGatherKeypoints<4, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, MAX_FEATURES_SRC3, MAX_FEATURES_SRC4, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, MAX_FEATURES_DST, MODE>(input1, input2, input3, input4, NULL, NULL, NULL, NULL,
                                                                NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, output);
}
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint16 MAX_FEATURES_SRC3,
          vx_uint16 MAX_FEATURES_SRC4, vx_uint32 MAX_FEATURES_DST, HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(KeyPoint input1[MAX_FEATURES_SRC1], KeyPoint input2[MAX_FEATURES_SRC2],
                      KeyPoint input3[MAX_FEATURES_SRC3], KeyPoint input4[MAX_FEATURES_SRC4],
                      KeyPoint output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HwGatherKeypoints<4, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, MAX_FEATURES_SRC3, MAX_FEATURES_SRC4, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, MAX_FEATURES_DST, MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, (vx_image_data<KeyPoint, 1> *)input4, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param MAX_KP_SRC3  Total amount of keypoints of the 3. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 * @param output       Output array of keypoints
 */
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint16 MAX_FEATURES_SRC3,
          vx_uint32 MAX_FEATURES_DST, HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(vx_image_data<KeyPoint, 1> input1[MAX_FEATURES_SRC1],
                      vx_image_data<KeyPoint, 1> input2[MAX_FEATURES_SRC2],
                      vx_image_data<KeyPoint, 1> input3[MAX_FEATURES_SRC3],
                      vx_image_data<KeyPoint, 1> output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, input3, output);

    HwGatherKeypoints<3, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, MAX_FEATURES_SRC3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_FEATURES_DST, MODE>(input1, input2, input3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                              NULL, NULL, NULL, NULL, NULL, output);
}
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint16 MAX_FEATURES_SRC3,
          vx_uint32 MAX_FEATURES_DST, HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(KeyPoint input1[MAX_FEATURES_SRC1], KeyPoint input2[MAX_FEATURES_SRC2],
                      KeyPoint input3[MAX_FEATURES_SRC3], KeyPoint output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HwGatherKeypoints<3, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, MAX_FEATURES_SRC3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_FEATURES_DST, MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2,
        (vx_image_data<KeyPoint, 1> *)input3, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, (vx_image_data<KeyPoint, 1> *)output);
}

/*********************************************************************************************************************/
/** Collects multiple input arrays of keypoints into one arrays
 * @param MAX_KP_SRC1  Total amount of keypoints of the 1. array
 * @param MAX_KP_SRC2  Total amount of keypoints of the 2. array
 * @param input1       Input array of keypoints
 * @param input2       Input array of keypoints
 * @param input3       Input array of keypoints
 */
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint32 MAX_FEATURES_DST,
          HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(vx_image_data<KeyPoint, 1> input1[MAX_FEATURES_SRC1],
                      vx_image_data<KeyPoint, 1> input2[MAX_FEATURES_SRC2],
                      vx_image_data<KeyPoint, 1> output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input1, input2, output);

    HwGatherKeypoints<2, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_FEATURES_DST, MODE>(input1, input2, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                              NULL, NULL, NULL, NULL, NULL, output);
}
template <vx_uint16 MAX_FEATURES_SRC1, vx_uint16 MAX_FEATURES_SRC2, vx_uint32 MAX_FEATURES_DST,
          HIFLIPVX::vx_scatter_mode_e MODE>
void ImgFeatureGather(KeyPoint input1[MAX_FEATURES_SRC1], KeyPoint input2[MAX_FEATURES_SRC2],
                      KeyPoint output[MAX_FEATURES_DST]) {
#pragma HLS INLINE
    HwGatherKeypoints<2, MAX_FEATURES_SRC1, MAX_FEATURES_SRC2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      MAX_FEATURES_DST, MODE>(
        (vx_image_data<KeyPoint, 1> *)input1, (vx_image_data<KeyPoint, 1> *)input2, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <vx_uint8 VEC_NUM_IN, vx_uint32 NUMBER_OF_INPUT_VECTORS, vx_uint32 NUMBER_OF_OUTPUT_VALUES>
void ImgFeatureDeserialize(vx_image_data<KeyPoint, VEC_NUM_IN> input[NUMBER_OF_INPUT_VECTORS],
                           vx_image_data<KeyPoint, 1> output[NUMBER_OF_OUTPUT_VALUES]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, input, output);

    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwDeserialization<KeyPoint, VEC_NUM_IN, NUMBER_OF_INPUT_VECTORS, NUMBER_OF_OUTPUT_VALUES>(
        input, output, kp_converter_invalid_kp.keypoint, HwFeatureDeserializationCompareKeypoint);
}
template <vx_uint8 VEC_NUM_IN, vx_uint32 NUMBER_OF_INPUT_VECTORS, vx_uint32 NUMBER_OF_OUTPUT_VALUES>
void ImgFeatureDeserialize(KeyPoint input[NUMBER_OF_INPUT_VECTORS], KeyPoint output[NUMBER_OF_OUTPUT_VALUES]) {
#pragma HLS INLINE

    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwDeserialization<KeyPoint, VEC_NUM_IN, NUMBER_OF_INPUT_VECTORS, NUMBER_OF_OUTPUT_VALUES>(
        (vx_image_data<KeyPoint, VEC_NUM_IN> *)input, (vx_image_data<KeyPoint, 1> *)output,
        kp_converter_invalid_kp.keypoint, HwFeatureDeserializationCompareKeypoint);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint32 NUMBER_OF_INPUT_VECTORS, vx_uint32 NUMBER_OF_OUTPUT_VALUES>
void ImgFeatureDeserialize(vx_image_data<DataType, VEC_NUM_IN> input[NUMBER_OF_INPUT_VECTORS],
                           vx_image_data<DataType, 1> output[NUMBER_OF_OUTPUT_VALUES], const DataType invalid_element,
                           vx_bool (*valid_vec_elem_fct)(const DataType, const DataType)) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, input, output);

    HwDeserialization<DataType, VEC_NUM_IN, NUMBER_OF_INPUT_VECTORS, NUMBER_OF_OUTPUT_VALUES>(
        input, output, invalid_element, valid_vec_elem_fct);
}
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint32 NUMBER_OF_INPUT_VECTORS, vx_uint32 NUMBER_OF_OUTPUT_VALUES>
void ImgFeatureDeserialize(DataType input[NUMBER_OF_INPUT_VECTORS], DataType output[NUMBER_OF_OUTPUT_VALUES],
                           const DataType invalid_element,
                           vx_bool (*valid_vec_elem_fct)(const DataType, const DataType)) {
#pragma HLS INLINE

    HwDeserialization<DataType, VEC_NUM_IN, NUMBER_OF_INPUT_VECTORS, NUMBER_OF_OUTPUT_VALUES>(
        (vx_image_data<DataType, VEC_NUM_IN> *)input, (vx_image_data<DataType, 1> *)output, invalid_element,
        valid_vec_elem_fct);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <typename ResponseType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint32 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(
    vx_image_data<ResponseType, VEC_NUM_IN> input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<KeyPoint, FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE)> output[MAX_KEYPOINTS]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_response, output);

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    typedef vx_uint8 OrientationType;
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_false_e>(
        input_response, NULL, output, NULL,
        CreateFeature2<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement2<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}
template <typename ResponseType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint32 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(ResponseType input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          KeyPoint output[MAX_KEYPOINTS]) {
#pragma HLS INLINE

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    typedef vx_uint8 OrientationType;
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_false_e>(
        (vx_image_data<ResponseType, VEC_NUM_IN> *)input_response, NULL, (vx_image_data<KeyPoint, VEC_NUM_OUT> *)output,
        NULL, CreateFeature2<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement2<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <typename ResponseType, typename OrientationType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS,
          vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD,
          vx_uint16 MAX_KEYPOINTS, vx_uint32 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(
    vx_image_data<ResponseType, VEC_NUM_IN> input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<OrientationType, VEC_NUM_IN> input_orientation[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<KeyPoint, FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE)> output[MAX_KEYPOINTS]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_response, input_orientation, output);

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_true_e>(
        input_response, input_orientation, output, NULL,
        CreateFeature2<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement2<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}
template <typename ResponseType, typename OrientationType, vx_uint8 MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS,
          vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD,
          vx_uint16 MAX_KEYPOINTS, vx_uint32 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(ResponseType input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          OrientationType input_orientation[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          KeyPoint output[MAX_KEYPOINTS]) {
#pragma HLS INLINE

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_true_e>(
        (vx_image_data<ResponseType, VEC_NUM_IN> *)input_response,
        (vx_image_data<OrientationType, VEC_NUM_IN> *)input_orientation, (vx_image_data<KeyPoint, VEC_NUM_OUT> *)output,
        NULL, CreateFeature2<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement2<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <typename ResponseType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint8 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(
    vx_image_data<ResponseType, VEC_NUM_IN> input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<KeyPoint, FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE)> output[MAX_KEYPOINTS],
    const MaskType user_defined_mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_response, output);

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    typedef vx_uint8 OrientationType;
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_false_e>(
        input_response, NULL, output, user_defined_mask,
        CreateFeature<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}
template <typename ResponseType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint8 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(ResponseType input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          KeyPoint output[MAX_KEYPOINTS], MaskType user_defined_mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    typedef vx_uint8 OrientationType;
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_false_e>(
        (vx_image_data<ResponseType, VEC_NUM_IN> *)input_response, NULL, (vx_image_data<KeyPoint, VEC_NUM_OUT> *)output,
        user_defined_mask,
        CreateFeature<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <typename ResponseType, typename OrientationType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS,
          vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD,
          vx_uint16 MAX_KEYPOINTS, vx_uint8 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(
    vx_image_data<ResponseType, VEC_NUM_IN> input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<OrientationType, VEC_NUM_IN> input_orientation[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
    vx_image_data<KeyPoint, FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE)> output[MAX_KEYPOINTS],
    const MaskType user_defined_mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_response, input_orientation, output);

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_true_e>(
        input_response, input_orientation, output, user_defined_mask,
        CreateFeature<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        SubpixelRefinement<ResponseType, KERN_SIZE, DOWNSAMPLE_FACTOR, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}
template <typename ResponseType, typename OrientationType, typename MaskType, vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS,
          vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE, vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD,
          vx_uint16 MAX_KEYPOINTS, vx_uint8 DOWNSAMPLE_FACTOR, vx_uint16 BORDER_WIDTH, vx_uint8 CLASS_ID,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT>
void ImgFeatureExtraction(ResponseType input_response[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          OrientationType input_orientation[VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN)],
                          KeyPoint output[MAX_KEYPOINTS], MaskType user_defined_mask[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERN_SIZE);
    const vx_uint32 VEC_PIX = VECTOR_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN);
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    HwFeatureExtraction<KeyPoint, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                        IMG_ROWS, KERN_SIZE, RADIUS_WITH_FRACTION, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, MASK_SHAPE,
                        WITH_SUBPIXEL_REFINEMENT, vx_true_e>(
        (vx_image_data<ResponseType, VEC_NUM_IN> *)input_response,
        (vx_image_data<OrientationType, VEC_NUM_IN> *)input_orientation, (vx_image_data<KeyPoint, VEC_NUM_OUT> *)output,
        user_defined_mask,
        CreateFeature<ResponseType, OrientationType, DOWNSAMPLE_FACTOR, CLASS_ID, IMG_COLS, IMG_ROWS>,
        kp_converter_invalid_kp.keypoint);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(
    vx_image_data<KeyPoint, VEC_NUM> input_current[MAX_KEYPOINTS_CURRENT],
    vx_image_data<KeyPoint, VEC_NUM> input_previous[(MAX_KEYPOINTS_PREVIOUS > 0) ? (MAX_KEYPOINTS_PREVIOUS) : (1)],
    vx_image_data<KeyPoint, VEC_NUM> input_next[(MAX_KEYPOINTS_NEXT > 0) ? (MAX_KEYPOINTS_NEXT) : (1)],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_current, input_previous, input_next, output);

    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(input_current, input_previous, input_next,
                                                                           output);
}
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(KeyPoint input_current[MAX_KEYPOINTS_CURRENT],
                          KeyPoint input_previous[(MAX_KEYPOINTS_PREVIOUS > 0) ? (MAX_KEYPOINTS_PREVIOUS) : (1)],
                          KeyPoint input_next[(MAX_KEYPOINTS_NEXT > 0) ? (MAX_KEYPOINTS_NEXT) : (1)],
                          KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(
        (vx_image_data<KeyPoint, VEC_NUM> *)input_current, (vx_image_data<KeyPoint, VEC_NUM> *)input_previous,
        (vx_image_data<KeyPoint, VEC_NUM> *)input_next, (vx_image_data<KeyPoint, 1> *)output);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(
    vx_image_data<KeyPoint, VEC_NUM> input_current[MAX_KEYPOINTS_CURRENT],
    vx_image_data<KeyPoint, VEC_NUM> input_previous[(MAX_KEYPOINTS_PREVIOUS > 0) ? (MAX_KEYPOINTS_PREVIOUS) : (1)],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_current, input_previous, output);

    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(input_current, input_previous, NULL, output);
}
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(KeyPoint input_current[MAX_KEYPOINTS_CURRENT],
                          KeyPoint input_previous[(MAX_KEYPOINTS_PREVIOUS > 0) ? (MAX_KEYPOINTS_PREVIOUS) : (1)],
                          KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_current, input_previous, output);

    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(
        (vx_image_data<KeyPoint, VEC_NUM> *)input_current, (vx_image_data<KeyPoint, VEC_NUM> *)input_previous, NULL,
        (vx_image_data<KeyPoint, 1> *)output);
}

/*********************************************************************************************************************/
/**
 * @brief
 * @tparam
 * @param
 */
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(vx_image_data<KeyPoint, VEC_NUM> input_current[MAX_KEYPOINTS_CURRENT],
                          vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_current, output);

    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(input_current, NULL, NULL, output);
}
template <vx_uint8 VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void ImgFeatureComparison(KeyPoint input_current[MAX_KEYPOINTS_CURRENT], KeyPoint output[MAX_KEYPOINTS_OUTPUT]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input_current, output);

    HwCompareKeypoints<VEC_NUM, IMG_COLS, ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                       MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION, LEVEL,
                       BOUNDARY, FIND_EXTREMA_KERNEL_SIZE, COMPARE_PARALLELIZATION_LEVEL,
                       RESTRICT_MAX_NUMBER_OF_ITERATIONS, MAX_BUFFER_SIZE>(
        (vx_image_data<KeyPoint, VEC_NUM> *)input_current, NULL, NULL, (vx_image_data<KeyPoint, 1> *)output);
}

#endif /* __SRC_IMG_MT_FEATURE_BASE_H__ */
