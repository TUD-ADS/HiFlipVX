/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_feature_base.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are accelerated feature (multicat, retain best, FAST, Canny, ORB) functions (Call from here)
 */

#ifndef SRC_IMG_FEATURES_BASE_H_
#define SRC_IMG_FEATURES_BASE_H_

#include "img_feature_core.h"

/***************************************************************************************************************************************************/
template <typename ResponseType, typename OrientationType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD, vx_uint8 SCALE>
void ImgFeatureExtraction(vx_image_data<ResponseType, 1> responses[IMG_COLS * IMG_ROWS],       //
                          vx_image_data<OrientationType, 1> orientations[IMG_COLS * IMG_ROWS], //
                          vx_image_data<KeyPoint, 1> output[MAX_FEATURES]) {                   //
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = responses
#pragma HLS DATA_PACK variable = orientations
#pragma HLS DATA_PACK variable = output
    FeatureExtraction<ResponseType, OrientationType, IMG_COLS, IMG_ROWS, MAX_FEATURES, BORDER_WIDTH, THRESHOLD, SCALE>(
        responses, orientations, output);
}
template <typename ResponseType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH,
          ResponseType THRESHOLD, vx_uint8 SCALE>
void ImgFeatureExtraction(vx_image_data<ResponseType, 1> responses[IMG_COLS * IMG_ROWS], //
                          vx_image_data<KeyPoint, 1> output[MAX_FEATURES]) {             //
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = responses
#pragma HLS DATA_PACK variable = output
    FeatureExtraction<ResponseType, IMG_COLS, IMG_ROWS, MAX_FEATURES, BORDER_WIDTH, THRESHOLD, SCALE>(responses,
                                                                                                      output);
}
template <typename ResponseType, typename OrientationType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD, vx_uint8 SCALE>
void ImgFeatureExtraction(ResponseType responses[IMG_COLS * IMG_ROWS],                                        //
                          OrientationType orientations[IMG_COLS * IMG_ROWS], KeyPoint output[MAX_FEATURES]) { //
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = output
    FeatureExtraction<ResponseType, OrientationType, IMG_COLS, IMG_ROWS, MAX_FEATURES, BORDER_WIDTH, THRESHOLD, SCALE>(
        responses, orientations, output);
}
template <typename ResponseType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH,
          ResponseType THRESHOLD, vx_uint8 SCALE>
void ImgFeatureExtraction(ResponseType responses[IMG_COLS * IMG_ROWS], //
                          KeyPoint output[MAX_FEATURES]) {             //
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = output
    FeatureExtraction<ResponseType, IMG_COLS, IMG_ROWS, MAX_FEATURES, BORDER_WIDTH, THRESHOLD, SCALE>(responses,
                                                                                                      output);
}

/*********************************************************************************************************************/
/** Copies the input feature vector to multiple output images
 * @param IMG_PIXELS Total amount of features
 * @param VEC_NUM    Vector size
 * @param input      input feature vector
 * @param out0       output feature vector
 * @param out1       output feature vector
 * @param out2       output feature vector
 * @param out3       output feature vector
 */
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(vx_image_data<KeyPoint, VEC_NUM> input[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out0[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out1[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out2[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out3[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out0, out1, out2, out3);
    FeatureMulticast<ELEMENTS, VEC_NUM, 4>(input, out0, out1, out2, out3);
}
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(KeyPoint input[ELEMENTS / VEC_NUM], KeyPoint out0[ELEMENTS / VEC_NUM],
                         KeyPoint out1[ELEMENTS / VEC_NUM], KeyPoint out2[ELEMENTS / VEC_NUM],
                         KeyPoint out3[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    FeatureMulticast<ELEMENTS, VEC_NUM, 4>(input, out0, out1, out2, out3);
}

/*********************************************************************************************************************/
/** Copies the input feature vector to multiple output images
 * @param IMG_PIXELS Total amount of features
 * @param VEC_NUM    Vector size
 * @param input      input feature vector
 * @param out0       output feature vector
 * @param out1       output feature vector
 * @param out2       output feature vector
 */
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(vx_image_data<KeyPoint, VEC_NUM> input[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out0[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out1[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out2[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out0, out1, out2);
    FeatureMulticast<ELEMENTS, VEC_NUM, 3>(input, out0, out1, out2, NULL);
}
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(KeyPoint input[ELEMENTS / VEC_NUM], KeyPoint out0[ELEMENTS / VEC_NUM],
                         KeyPoint out1[ELEMENTS / VEC_NUM], KeyPoint out2[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    FeatureMulticast<ELEMENTS, VEC_NUM, 3>(input, out0, out1, out2, NULL);
}

/*********************************************************************************************************************/
/** Copies the input feature vector to multiple output images
 * @param IMG_PIXELS Total amount of features
 * @param VEC_NUM    Vector size
 * @param input      input feature vector
 * @param out0       output feature vector
 * @param out1       output feature vector
 */
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(vx_image_data<KeyPoint, VEC_NUM> input[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out0[ELEMENTS / VEC_NUM],
                         vx_image_data<KeyPoint, VEC_NUM> out1[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, out0, out1);
    FeatureMulticast<ELEMENTS, VEC_NUM, 2>(input, out0, out1, NULL, NULL);
}
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM>
void ImgFeatureMulticast(KeyPoint input[ELEMENTS / VEC_NUM], KeyPoint out0[ELEMENTS / VEC_NUM],
                         KeyPoint out1[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE
    FeatureMulticast<ELEMENTS, VEC_NUM, 2>(input, out0, out1, NULL, NULL);
}

/***************************************************************************************************************************************************/
template <typename SrcType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint32 MAX_FEATURES, SrcType THRESHOLD,
          vx_uint8 SCALE>
void ImgFastCorners(vx_image_data<SrcType, 1> input[IMG_COLS * IMG_ROWS],
                    vx_image_data<KeyPoint, 1> output[MAX_FEATURES]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = input
#pragma HLS DATA_PACK variable = output
    FastCorners<SrcType, vx_image_data<SrcType, 1>, vx_image_data<KeyPoint, 1>, IMG_COLS, IMG_ROWS, MAX_FEATURES,
                THRESHOLD, SCALE>(input, output);
}
template <typename SrcType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, vx_uint32 MAX_FEATURES, SrcType THRESHOLD,
          vx_uint8 SCALE>
void ImgFastCorners(SrcType input[IMG_COLS * IMG_ROWS], KeyPoint output[MAX_FEATURES]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = output
    FastCorners<SrcType, SrcType, KeyPoint, IMG_COLS, IMG_ROWS, MAX_FEATURES, THRESHOLD, SCALE>(input, output);
}

/***************************************************************************************************************************************************/
template <typename UintType, typename IntType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS,       //
          vx_uint8 VEC_NUM, UintType THRESH_LOWER, UintType THRESH_UPPER,                    //
          vx_uint8 SOBEL_KERN_SIZE, vx_uint8 HYST_KERN_SIZE, vx_round_policy_e ROUND_POLICY> //
void ImgCannyEdgeDetector(vx_image_data<UintType, VEC_NUM> src[IMG_COLS * IMG_ROWS],         //
                          vx_image_data<UintType, VEC_NUM> dst[IMG_COLS * IMG_ROWS]) {       //
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = src
#pragma HLS DATA_PACK variable = dst
    CannyEdgeDetector<UintType, IntType, vx_image_data<UintType, VEC_NUM>, vx_image_data<IntType, VEC_NUM>, //
                      IMG_COLS, IMG_ROWS, VEC_NUM, THRESH_LOWER, THRESH_UPPER,                              //
                      SOBEL_KERN_SIZE, HYST_KERN_SIZE, ROUND_POLICY>(src, dst);                             //
}
template <typename UintType, typename IntType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS,       //
          vx_uint8 VEC_NUM, UintType THRESH_LOWER, UintType THRESH_UPPER,                    //
          vx_uint8 SOBEL_KERN_SIZE, vx_uint8 HYST_KERN_SIZE, vx_round_policy_e ROUND_POLICY> //
void ImgCannyEdgeDetector(UintType src[IMG_COLS * IMG_ROWS],                                 //
                          UintType dst[IMG_COLS * IMG_ROWS]) {                               //
#pragma HLS INLINE
    CannyEdgeDetector<UintType, IntType, UintType, IntType, IMG_COLS, IMG_ROWS, VEC_NUM, THRESH_LOWER, //
                      THRESH_UPPER, SOBEL_KERN_SIZE, HYST_KERN_SIZE, ROUND_POLICY>(src, dst);          //
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL,
          vx_uint16 MAX_RESPONSE_VAL>
void ImgFeatureRetainBest(vx_image_data<KeyPoint, 1> input[MAX_INPUT_FEATURES],
                          vx_image_data<KeyPoint, 1> output[MAX_OUTPUT_FEATURES]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = input
#pragma HLS DATA_PACK variable = output
    RetainBestFixedRange<MAX_INPUT_FEATURES, MAX_OUTPUT_FEATURES, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(input, output);
}
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL,
          vx_uint16 MAX_RESPONSE_VAL>
void ImgFeatureRetainBest(KeyPoint input[MAX_INPUT_FEATURES], KeyPoint output[MAX_OUTPUT_FEATURES]) {
#pragma HLS INLINE
#pragma HLS DATA_PACK variable = input
#pragma HLS DATA_PACK variable = output
    RetainBestFixedRange<MAX_INPUT_FEATURES, MAX_OUTPUT_FEATURES, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(input, output);
}

/***************************************************************************************************************************************************/

template <typename SrcType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, SrcType THRESHOLD>
void ImgOrbFeatures(vx_image_data<SrcType, 1> input[IMG_COLS * IMG_ROWS], vx_image_data<KeyPoint, 1> output[2048]) {
#pragma HLS INLINE
    HIFLIPVX_DATAPACK(input, output);
    OrbDetection<SrcType, IMG_COLS, IMG_ROWS, THRESHOLD>(input, output);
}

#endif //
