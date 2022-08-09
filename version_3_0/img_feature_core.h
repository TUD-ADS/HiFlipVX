/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_feature_core.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are core functions needed for the feature functions (Do not call functions from here)
 */

#ifndef SRC_IMG_FEATURES_CORE_H_
#define SRC_IMG_FEATURES_CORE_H_

#include "img_pixelop_base.h"
#include "img_filter_base.h"
#include "img_conversion_base.h"
#include "img_analysis_base.h"
#include "img_mt_feature_base.h"

/***************************************************************************************************************************************************/
/***************************************************************************************************************************************************/
template <typename ResponseType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD,
          vx_uint8 SCALE>
void FeatureExtractionWithoutOrientation(vx_uint32 y, vx_uint32 x, ResponseType response, KeyPoint &feature,
                                         bool &feature_detected) {
#pragma HLS INLINE

    // Constants
    const ResponseType SRC1_MAX_VAL = std::numeric_limits<ResponseType>::max();
    const ResponseType SRC1_MIN_VAL = std::numeric_limits<ResponseType>::min();

    // Detect feature if it meets threshold and border reqirements
    if (BORDER_WIDTH != 0) {
        if ((y >= BORDER_WIDTH) && (x >= BORDER_WIDTH) && (y < (IMG_ROWS - BORDER_WIDTH)) &&
            (x < (IMG_COLS - BORDER_WIDTH))) {
            if (response >= THRESHOLD)
                feature_detected = true;
        }
    } else {
        if (response >= THRESHOLD)
            feature_detected = true;
    }

    // Create the feature
    feature.x = x;
    feature.y = y;
    feature.scale = SCALE;
    if (SRC1_MAX_VAL > VX_INT16_MAX) {
        if (SRC1_MIN_VAL < VX_INT16_MIN) {
            feature.response =
                (response > VX_INT16_MAX) ? (VX_INT16_MAX) : ((response < VX_INT16_MIN) ? (VX_INT16_MIN) : (response));
        } else {
            feature.response = (response > VX_INT16_MAX) ? (VX_INT16_MAX) : (response);
        }
    } else {
        feature.response = response;
    }
    feature.orientation = 0;
}

/***************************************************************************************************************************************************/
template <typename ResponseType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH,
          ResponseType THRESHOLD, vx_uint8 SCALE>
void FeatureExtraction(vx_image_data<ResponseType, 1> responses[IMG_COLS * IMG_ROWS],
                       vx_image_data<KeyPoint, 1> output[MAX_FEATURES]) {
#pragma HLS INLINE

    // Constants
    const ResponseType SRC1_MAX_VAL = std::numeric_limits<ResponseType>::max();
    const ResponseType SRC1_MIN_VAL = std::numeric_limits<ResponseType>::min();

    // Extracts and creates features
    for (vx_uint32 y = 0, dst_ptr = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // Local variables
            vx_image_data<ResponseType, 1> src1;
            vx_image_data<KeyPoint, 1> dst;
            ResponseType response;
            KeyPoint feature;
            KeyPointConverter kp_converter;
            bool feature_detected = false;

            // Read the input values
            src1 = responses[y * IMG_COLS + x];
            response = src1.pixel[0];

            // Extracts a single feature
            FeatureExtractionWithoutOrientation<ResponseType, IMG_COLS, IMG_ROWS, BORDER_WIDTH, THRESHOLD, SCALE>(
                y, x, response, feature, feature_detected);

            // Writes output feature
            if ((feature_detected == true) && (dst_ptr < MAX_FEATURES)) {
                GenerateDmaSignal<KeyPoint, 1>((dst_ptr == 0), (dst_ptr == MAX_FEATURES - 1), dst);
                dst.pixel[0] = feature;
                output[dst_ptr] = dst;
                ++dst_ptr;
            } else if (x == (IMG_COLS - 1) && y == (IMG_ROWS - 1)) {
                if (dst_ptr < MAX_FEATURES) {
                    kp_converter.data = INVALID_KEYPOINT_DATA;
                    GenerateDmaSignal<KeyPoint, 1>((dst_ptr == 0), true, dst);
                    dst.pixel[0] = kp_converter.keypoint;
                    output[dst_ptr] = dst;
                }
            }
        }
    }
}

/***************************************************************************************************************************************************/
template <typename ResponseType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH,
          ResponseType THRESHOLD, vx_uint8 SCALE>
void FeatureExtraction(ResponseType responses[IMG_COLS * IMG_ROWS], KeyPoint output[MAX_FEATURES]) {
#pragma HLS INLINE

    // Constants
    const ResponseType SRC1_MAX_VAL = std::numeric_limits<ResponseType>::max();
    const ResponseType SRC1_MIN_VAL = std::numeric_limits<ResponseType>::min();

    // Extracts and creates features
    for (vx_uint32 y = 0, dst_ptr = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // Local variables
            ResponseType response;
            KeyPoint feature;
            KeyPointConverter kp_converter;
            bool feature_detected = false;

            // Read the input values
            response = responses[y * IMG_COLS + x];

            // Extracts a single feature
            FeatureExtractionWithoutOrientation<ResponseType, IMG_COLS, IMG_ROWS, BORDER_WIDTH, THRESHOLD, SCALE>(
                y, x, response, feature, feature_detected);

            // Writes output feature
            if ((feature_detected == true) && (dst_ptr < MAX_FEATURES)) {
                output[dst_ptr] = feature;
                ++dst_ptr;
            } else if (x == (IMG_COLS - 1) && y == (IMG_ROWS - 1)) {
                if (dst_ptr < MAX_FEATURES) {
                    kp_converter.data = INVALID_KEYPOINT_DATA;
                    output[dst_ptr] = kp_converter.keypoint;
                }
            }
        }
    }
}

/***************************************************************************************************************************************************/
template <typename ResponseType, typename OrientationType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD, vx_uint8 SCALE>
void FeatureExtractionWithOrientation(vx_uint32 y, vx_uint32 x, ResponseType response, OrientationType orientation,
                                      KeyPoint &feature, bool &feature_detected) {
#pragma HLS INLINE

    // Constants
    const ResponseType SRC1_MAX_VAL = std::numeric_limits<ResponseType>::max();
    const ResponseType SRC1_MIN_VAL = std::numeric_limits<ResponseType>::min();
    const OrientationType SRC2_MAX_VAL = std::numeric_limits<OrientationType>::max();
    const OrientationType SRC2_MIN_VAL = std::numeric_limits<OrientationType>::min();

    // Detect feature if it meets threshold and border reqirements
    if (BORDER_WIDTH != 0) {
        if ((y >= BORDER_WIDTH) && (x >= BORDER_WIDTH) && (y < (IMG_ROWS - BORDER_WIDTH)) &&
            (x < (IMG_COLS - BORDER_WIDTH))) {
            if (response >= THRESHOLD)
                feature_detected = true;
        }
    } else {
        if (response >= THRESHOLD)
            feature_detected = true;
    }

    // Create the feature
    feature.x = x;
    feature.y = y;
    feature.scale = SCALE;
    if (SRC1_MAX_VAL > VX_INT16_MAX) {
        if (SRC1_MIN_VAL < VX_INT16_MIN) {
            feature.response =
                (response > VX_INT16_MAX) ? (VX_INT16_MAX) : ((response < VX_INT16_MIN) ? (VX_INT16_MIN) : (response));
        } else {
            feature.response = (response > VX_INT16_MAX) ? (VX_INT16_MAX) : (response);
        }
    } else {
        feature.response = response;
    }
    if (SRC2_MAX_VAL > VX_UINT8_MAX) {
        if (SRC2_MIN_VAL < 0) {
            feature.orientation =
                (orientation > VX_UINT8_MAX) ? (VX_UINT8_MAX) : ((orientation < 0) ? (0) : (orientation));
        } else {
            feature.orientation = (orientation > VX_UINT8_MAX) ? (VX_UINT8_MAX) : (orientation);
        }
    } else {
        feature.orientation = orientation;
    }
}

/***************************************************************************************************************************************************/
template <typename ResponseType, typename OrientationType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD, vx_uint8 SCALE>
void FeatureExtraction(vx_image_data<ResponseType, 1> responses[IMG_COLS * IMG_ROWS],
                       vx_image_data<OrientationType, 1> orientations[IMG_COLS * IMG_ROWS],
                       vx_image_data<KeyPoint, 1> output[MAX_FEATURES]) {
#pragma HLS INLINE

    // Extracts and creates features
    for (vx_uint32 y = 0, dst_ptr = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // Local variables
            vx_image_data<ResponseType, 1> src1;
            vx_image_data<OrientationType, 1> src2;
            vx_image_data<KeyPoint, 1> dst;
            ResponseType response;
            OrientationType orientation;
            KeyPoint feature;
            KeyPointConverter kp_converter;
            bool feature_detected = false;

            // Read the input values
            src1 = responses[y * IMG_COLS + x];
            src2 = orientations[y * IMG_COLS + x];
            response = src1.pixel[0];
            orientation = src2.pixel[0];

            // Extracts a single feature
            FeatureExtractionWithOrientation<ResponseType, OrientationType, IMG_COLS, IMG_ROWS, BORDER_WIDTH, THRESHOLD,
                                             SCALE>(y, x, response, orientation, feature, feature_detected);

            // Writes output feature
            if ((feature_detected == true) && (dst_ptr < MAX_FEATURES)) {
                GenerateDmaSignal<KeyPoint, 1>((dst_ptr == 0), (dst_ptr == MAX_FEATURES - 1), dst);
                dst.pixel[0] = feature;
                output[dst_ptr] = dst;
                ++dst_ptr;
            } else if (x == (IMG_COLS - 1) && y == (IMG_ROWS - 1)) {
                if (dst_ptr < MAX_FEATURES) {
                    kp_converter.data = INVALID_KEYPOINT_DATA;
                    GenerateDmaSignal<KeyPoint, 1>((dst_ptr == 0), true, dst);
                    dst.pixel[0] = kp_converter.keypoint;
                    output[dst_ptr] = dst;
                }
            }
        }
    }
}

/***************************************************************************************************************************************************/
template <typename ResponseType, typename OrientationType, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint32 MAX_FEATURES, vx_uint32 BORDER_WIDTH, ResponseType THRESHOLD, vx_uint8 SCALE>
void FeatureExtraction(ResponseType responses[IMG_COLS * IMG_ROWS], OrientationType orientations[IMG_COLS * IMG_ROWS],
                       KeyPoint output[MAX_FEATURES]) {
#pragma HLS INLINE

    // Extracts and creates features
    for (vx_uint32 y = 0, dst_ptr = 0; y < IMG_ROWS; ++y) {
        for (vx_uint32 x = 0; x < IMG_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // Local variables
            ResponseType response;
            OrientationType orientation;
            KeyPoint feature;
            KeyPointConverter kp_converter;
            bool feature_detected = false;

            // Read the input values
            response = responses[y * IMG_COLS + x];
            orientation = orientations[y * IMG_COLS + x];

            // Extracts a single feature
            FeatureExtractionWithOrientation<ResponseType, OrientationType, IMG_COLS, IMG_ROWS, BORDER_WIDTH, THRESHOLD,
                                             SCALE>(y, x, response, orientation, feature, feature_detected);

            // Writes output feature
            if ((feature_detected == true) && (dst_ptr < MAX_FEATURES)) {
                output[dst_ptr] = feature;
                ++dst_ptr;
            } else if (x == (IMG_COLS - 1) && y == (IMG_ROWS - 1)) {
                if (dst_ptr < MAX_FEATURES) {
                    kp_converter.data = INVALID_KEYPOINT_DATA;
                    output[dst_ptr] = kp_converter.keypoint;
                }
            }
        }
    }
}

/***************************************************************************************************************************************************/
template <typename SrcType, typename SrcImage, typename DstFeatures, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS,
          vx_uint32 MAX_FEATURES, SrcType THRESHOLD, vx_uint8 SCALE>
void FastCorners(SrcImage input[IMG_COLS * IMG_ROWS], DstFeatures output[MAX_FEATURES]) {
#pragma HLS INLINE

    // FIFOs to stream data between functions
#ifdef __XILINX__
    SrcImage img1[IMG_COLS * IMG_ROWS];
#pragma HLS STREAM variable = img1 depth = 16
#pragma HLS data_pack variable = img1
#pragma HLS RESOURCE variable = img1 core = FIFO_LUTRAM
    SrcImage img2[IMG_COLS * IMG_ROWS];
#pragma HLS STREAM variable = img2 depth = 16
#pragma HLS data_pack variable = img2
#pragma HLS RESOURCE variable = img2 core = FIFO_LUTRAM
#else
    SrcImage *img1 = new SrcImage[IMG_COLS * IMG_ROWS];
    SrcImage *img2 = new SrcImage[IMG_COLS * IMG_ROWS];
#endif

    // Mask for Non-Max_supression.
    const vx_uint8 mask[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
#pragma HLS array_partition variable = mask complete dim = 0

    // Compute the Fast Corners
#pragma HLS dataflow
    ImgSegmentTestDetector<SrcType, 1, IMG_COLS, IMG_ROWS, VX_BORDER_UNDEFINED>(input, img1);
    ImgNonMaxSuppression<SrcType, 1, IMG_COLS, IMG_ROWS, 3, VX_BORDER_UNDEFINED>(img1, img2, mask);
    FeatureExtraction<SrcType, IMG_COLS, IMG_ROWS, MAX_FEATURES, 4, THRESHOLD, SCALE>(img2, output);

    // Free Memory
#ifndef __XILINX__
    delete[] img1;
    delete[] img2;
#endif
}

/***************************************************************************************************************************************************/
template <typename UintType, typename IntType, typename UintImage, typename IntImage, vx_uint32 IMG_COLS,
          vx_uint32 IMG_ROWS, vx_uint32 VEC_NUM, UintType THRESH_LOWER, UintType THRESH_UPPER,
          vx_uint32 SOBEL_KERN_SIZE, vx_uint32 HYSTERESIS_KERN_SIZE, vx_round_policy_e ROUND_POLICY>
void CannyEdgeDetector(UintImage src[IMG_COLS * IMG_ROWS], UintImage dst[IMG_COLS * IMG_ROWS]) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 digits1 = std::numeric_limits<UintType>::digits;
    const vx_uint32 digits2 = std::numeric_limits<IntType>::digits;
    STATIC_ASSERT(digits1 == 8 || digits1 == 16, ONLY_UNSIGNED_8_OR_16_BIT_ALLOWED_FOR_CANNY_EDGE);
    STATIC_ASSERT(digits2 == 7 || digits2 == 15, ONLY_UNSIGNED_8_OR_16_BIT_ALLOWED_FOR_CANNY_EDGE);
    const vx_uint32 IMG_PIXELS = IMG_ROWS * IMG_COLS;
    const vx_uint32 QUANTIZATION = 3;
    const vx_uint32 VEC_PIXELS = IMG_PIXELS / VEC_NUM;

    // FIFOs to stream data between functions
#ifdef __XILINX__
    IntImage img00[VEC_PIXELS];
#pragma HLS STREAM variable = img00 depth = 8
#pragma HLS data_pack variable = img00
#pragma HLS RESOURCE variable = img00 core = FIFO_LUTRAM
    IntImage img00a[VEC_PIXELS];
#pragma HLS STREAM variable = img00a depth = 8
#pragma HLS data_pack variable = img00a
#pragma HLS RESOURCE variable = img00a core = FIFO_LUTRAM
    IntImage img00b[VEC_PIXELS];
#pragma HLS STREAM variable = img00b depth = 8
#pragma HLS data_pack variable = img00b
#pragma HLS RESOURCE variable = img00b core = FIFO_LUTRAM
    IntImage img01[VEC_PIXELS];
#pragma HLS STREAM variable = img01 depth = 8
#pragma HLS data_pack variable = img01
#pragma HLS RESOURCE variable = img01 core = FIFO_LUTRAM
    IntImage img01a[VEC_PIXELS];
#pragma HLS STREAM variable = img01a depth = 8
#pragma HLS data_pack variable = img01a
#pragma HLS RESOURCE variable = img01a core = FIFO_LUTRAM
    IntImage img01b[VEC_PIXELS];
#pragma HLS STREAM variable = img01b depth = 8
#pragma HLS data_pack variable = img01b
#pragma HLS RESOURCE variable = img01b core = FIFO_LUTRAM
    IntImage img10[VEC_PIXELS];
#pragma HLS STREAM variable = img10 depth = 8
#pragma HLS data_pack variable = img10
#pragma HLS RESOURCE variable = img10 core = FIFO_LUTRAM
    IntImage img11[VEC_PIXELS];
#pragma HLS STREAM variable = img11 depth = 8
#pragma HLS data_pack variable = img11
#pragma HLS RESOURCE variable = img11 core = FIFO_LUTRAM
    IntImage img20[VEC_PIXELS];
#pragma HLS STREAM variable = img20 depth = 8
#pragma HLS data_pack variable = img20
#pragma HLS RESOURCE variable = img20 core = FIFO_LUTRAM
    UintImage img30[VEC_PIXELS];
#pragma HLS STREAM variable = img30 depth = 8
#pragma HLS data_pack variable = img30
#pragma HLS RESOURCE variable = img30 core = FIFO_LUTRAM
#else
    IntImage *img00 = new IntImage[VEC_PIXELS];
    IntImage *img00a = new IntImage[VEC_PIXELS];
    IntImage *img00b = new IntImage[VEC_PIXELS];
    IntImage *img01 = new IntImage[VEC_PIXELS];
    IntImage *img01a = new IntImage[VEC_PIXELS];
    IntImage *img01b = new IntImage[VEC_PIXELS];
    IntImage *img10 = new IntImage[VEC_PIXELS];
    IntImage *img11 = new IntImage[VEC_PIXELS];
    IntImage *img20 = new IntImage[VEC_PIXELS];
    UintImage *img30 = new UintImage[VEC_PIXELS];
#endif

    // Compute the Canny Edge detector
#pragma HLS dataflow
    ImgSobel<UintType, IntType, VEC_NUM, IMG_COLS, IMG_ROWS, SOBEL_KERN_SIZE, VX_BORDER_REPLICATE>(src, img00,
                                                                                                   img01); // REPLICATED
    ImgMulticast<IntType, IMG_PIXELS, VEC_NUM>(img00, img00a, img00b);
    ImgMulticast<IntType, IMG_PIXELS, VEC_NUM>(img01, img01a, img01b);
    ImgMagnitude<IntType, IntType, VEC_NUM, IMG_PIXELS, ROUND_POLICY>(img00a, img01a, img10);
    ImgPhase<IntType, VEC_NUM, IMG_PIXELS, QUANTIZATION>(img00b, img01b, img11);
    ImgNonMaxSuppressionOriented<IntType, VEC_NUM, IMG_COLS, IMG_ROWS, VX_BORDER_REPLICATE, QUANTIZATION>(
        img10, img11, img20); // REPLICATED
    ImgConvertBitDepth<IntType, UintType, VEC_NUM, IMG_PIXELS, VX_CONVERT_POLICY_SATURATE, 0>(img20, img30);
    ImgHysteresis<UintType, VEC_NUM, IMG_COLS, IMG_ROWS, HYSTERESIS_KERN_SIZE, VX_BORDER_CONSTANT, THRESH_LOWER,
                  THRESH_UPPER>(img30, dst); // CONSTANT

    // Free Memory
#ifndef __XILINX__
    delete[] img00;
    delete[] img00a;
    delete[] img00b;
    delete[] img01;
    delete[] img01a;
    delete[] img01b;
    delete[] img10;
    delete[] img11;
    delete[] img20;
    delete[] img30;
#endif
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL, vx_uint16 SHIFT, vx_uint16 BINS>
void RetainBestHistogram(vx_uint32 i, KeyPoint feature, vx_uint16 pre_bin, vx_uint16 pre_val, vx_uint16 &cur_bin,
                         vx_uint16 &cur_val, vx_uint32 &feature_num, bool &read_done, vx_uint16 LUTA[BINS],
                         vx_uint16 LUTB[BINS], KeyPoint feature_buffer[MAX_INPUT_FEATURES]) {
#pragma HLS INLINE

    // Variables
    KeyPointConverter kp_converter;
    vx_uint16 response = 0;

    // Convert to feature
    kp_converter.keypoint = feature;
    response = feature.response;

    // Get histogram entry
    cur_bin = static_cast<vx_uint16>(response - MIN_RESPONSE_VAL) >> SHIFT;
    cur_bin = MIN(MAX(cur_bin, 0), BINS - 1);

    // Check if for end of feature vector list
    if (kp_converter.data == INVALID_KEYPOINT_DATA || i == (MAX_INPUT_FEATURES - 1)) {
        feature_num = i + 1;
        read_done = true;
    }

    // Store feature
    feature_buffer[i] = feature;

    // Read current histogram entry & write previous histogram entry
    if (i % 2 == 0) {
        cur_val = LUTA[cur_bin];
        LUTB[pre_bin] = pre_val;
    } else {
        cur_val = LUTB[cur_bin];
        LUTA[pre_bin] = pre_val;
    }
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 BINS>
void RetainBestPrefixSum(vx_uint16 LUTA[BINS], vx_uint16 LUTB[BINS], vx_uint16 &prefix_sum, vx_uint16 &retain_val,
                         vx_uint16 &retain_bin) {
#pragma HLS INLINE

    // Iterate though histogram
    for (vx_int32 i = BINS - 1; i >= 0; --i) {
#pragma HLS PIPELINE II = 1

        // Compute the prefix sum
        vx_uint16 sum = LUTA[i] + LUTB[i];
        prefix_sum += sum;

        // Store bin entry as long es prefix some is below or equal to the MAX_OUTPUT_FEATURES
        if (prefix_sum <= MAX_OUTPUT_FEATURES) {
            retain_val = prefix_sum;
            retain_bin = i;
        }
    }
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL, vx_uint16 SHIFT,
          vx_uint16 BINS>
void RetainBestFeature(vx_uint32 i, vx_uint32 feature_num, vx_uint16 retain_val, vx_uint16 retain_bin,
                       KeyPoint feature_buffer[MAX_INPUT_FEATURES], KeyPoint &keypoint, bool &write_feature,
                       vx_uint16 &border_feature_cnt) {
#pragma HLS INLINE

    KeyPointConverter kp_converter;
    kp_converter.data = INVALID_KEYPOINT_DATA;
    vx_uint16 response = 0;
    vx_uint16 cur_bin = 0;

    // Feature num could be below MAX_INPUT_FEATURES
    if (i < feature_num) {

        // Get feature and its response value
        kp_converter.keypoint = feature_buffer[i];
        response = kp_converter.keypoint.response;

        // Get histogram entry
        cur_bin = static_cast<vx_uint16>(response - MIN_RESPONSE_VAL) >> SHIFT;
        cur_bin = MIN(MAX(cur_bin, 0), BINS - 1);

        // Add feature to list that should be retained
        if (cur_bin >= retain_bin) {
            write_feature = true;

            // Only take as much features of this bin entry to be below MAX_OUTPUT_FEATURES
        } else if ((cur_bin == (retain_bin - 1)) && (border_feature_cnt < (MAX_OUTPUT_FEATURES - retain_val))) {
            write_feature = true;
            ++border_feature_cnt;
        }
    }

    // Send end of feature list if needed
    if ((write_feature == false) && (i == (MAX_INPUT_FEATURES - 1))) {
        kp_converter.data = INVALID_KEYPOINT_DATA;
        write_feature = true;
    }

    keypoint = kp_converter.keypoint;
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL,
          vx_uint16 MAX_RESPONSE_VAL>
void RetainBestFixedRange(vx_image_data<KeyPoint, 1> input[MAX_INPUT_FEATURES],
                          vx_image_data<KeyPoint, 1> output[MAX_OUTPUT_FEATURES]) {
#pragma HLS INLINE

    STATIC_ASSERT(MAX_RESPONSE_VAL > MIN_RESPONSE_VAL, max_response_value_should_be_bigger_than_min_response_value);
    STATIC_ASSERT(MAX_INPUT_FEATURES > MAX_OUTPUT_FEATURES,
                  max_input_features_should_be_bigger_than_max_output_features);

    // Constants
    const vx_uint32 RANGE = (vx_uint32)(MAX_RESPONSE_VAL - MIN_RESPONSE_VAL + 1);
    const vx_uint32 DIVIDER = ((RANGE % 1024) == 0) ? (RANGE / 1024) : (RANGE / 1024 + 1);
    const vx_uint32 SHIFT =
        ((1 << 0) >= DIVIDER)
            ? (0)
            : (((1 << 1) >= DIVIDER)
                   ? (1)
                   : (((1 << 2) >= DIVIDER)
                          ? (2)
                          : (((1 << 3) >= DIVIDER)
                                 ? (3)
                                 : (((1 << 4) >= DIVIDER)
                                        ? (4)
                                        : (((1 << 5) >= DIVIDER)
                                               ? (5)
                                               : (((1 << 6) >= DIVIDER)
                                                      ? (6)
                                                      : (((1 << 7) >= DIVIDER)
                                                             ? (7)
                                                             : (((1 << 8) >= DIVIDER) ? (8) : (9)))))))));
    const vx_uint16 BINS = (RANGE <= 1024) ? (RANGE) : (RANGE >> SHIFT);

    // Buffer Histogram and Features in BRAM
    vx_uint16 LUTA[BINS], LUTB[BINS];

    KeyPoint feature_buffer[MAX_INPUT_FEATURES];
    PRAGMA_HLS_MEM_CORE(feature_buffer, KeyPoint, MAX_INPUT_FEATURES)

    // Global variables
    vx_uint16 pre_bin = 0, pre_val = 0, retain_val = 0, retain_bin = 0, prefix_sum = 0, dst_ptr = 0,
              border_feature_cnt = 0;
    vx_uint32 feature_num = 0;
    bool read_done = false;

    // Reset histogram
    ResetHistogram<vx_uint16, BINS>(LUTA, LUTB);

    // Compute Histogram
    for (vx_uint32 i = 0; i < MAX_INPUT_FEATURES + 1; i++) {
#pragma HLS PIPELINE II = 1
        vx_image_data<KeyPoint, 1> src;
        vx_uint16 cur_bin = 0, cur_val = 0;

        // Read input and update histogram
        if (read_done == false) {
            src = input[i];
            RetainBestHistogram<MAX_INPUT_FEATURES, MIN_RESPONSE_VAL, SHIFT, BINS>(
                i, src.pixel[0], pre_bin, pre_val, cur_bin, cur_val, feature_num, read_done, LUTA, LUTB,
                feature_buffer);
        }

        // Update to store in next iteration
        ++cur_val;
        pre_bin = cur_bin;
        pre_val = cur_val;
    }

    // Computes the prefix sum, pointer to the histogram entry to retain best
    RetainBestPrefixSum<MAX_OUTPUT_FEATURES, BINS>(LUTA, LUTB, prefix_sum, retain_val, retain_bin);

    // Go through feature list, 1 extra cycle to send end of list message
    for (vx_uint32 i = 0; i < MAX_INPUT_FEATURES; ++i) {
#pragma HLS PIPELINE II = 1
        vx_image_data<KeyPoint, 1> dst;
        KeyPoint feature;
        bool write_feature = false;

        // Gets feature below below/equal histogram entry. partially writes features 1 hist entry above to fill
        RetainBestFeature<MAX_INPUT_FEATURES, MAX_OUTPUT_FEATURES, MIN_RESPONSE_VAL, SHIFT, BINS>(
            i, feature_num, retain_val, retain_bin, feature_buffer, feature, write_feature, border_feature_cnt);

        // Write feature to output
        if ((write_feature == true) && (dst_ptr < MAX_OUTPUT_FEATURES)) {
            GenerateDmaSignal<KeyPoint, 1>(
                (dst_ptr == 0), ((i == (MAX_INPUT_FEATURES - 1)) || (dst_ptr == (MAX_OUTPUT_FEATURES - 1))), dst);
            dst.pixel[0] = feature;
            output[dst_ptr] = dst;
            ++dst_ptr;
        }
    }
}

/***************************************************************************************************************************************************/
template <vx_uint16 MAX_INPUT_FEATURES, vx_uint16 MAX_OUTPUT_FEATURES, vx_uint16 MIN_RESPONSE_VAL,
          vx_uint16 MAX_RESPONSE_VAL>
void RetainBestFixedRange(KeyPoint input[MAX_INPUT_FEATURES], KeyPoint output[MAX_OUTPUT_FEATURES]) {
#pragma HLS INLINE

    STATIC_ASSERT(MAX_RESPONSE_VAL > MIN_RESPONSE_VAL, max_response_value_should_be_bigger_than_min_response_value);
    STATIC_ASSERT(MAX_INPUT_FEATURES > MAX_OUTPUT_FEATURES,
                  max_input_features_should_be_bigger_than_max_output_features);

    // Constants
    const vx_uint32 RANGE = (vx_uint32)(MAX_RESPONSE_VAL - MIN_RESPONSE_VAL + 1);
    const vx_uint32 DIVIDER = ((RANGE % 1024) == 0) ? (RANGE / 1024) : (RANGE / 1024 + 1);
    const vx_uint32 SHIFT =
        ((1 << 0) >= DIVIDER)
            ? (0)
            : (((1 << 1) >= DIVIDER)
                   ? (1)
                   : (((1 << 2) >= DIVIDER)
                          ? (2)
                          : (((1 << 3) >= DIVIDER)
                                 ? (3)
                                 : (((1 << 4) >= DIVIDER)
                                        ? (4)
                                        : (((1 << 5) >= DIVIDER)
                                               ? (5)
                                               : (((1 << 6) >= DIVIDER)
                                                      ? (6)
                                                      : (((1 << 7) >= DIVIDER)
                                                             ? (7)
                                                             : (((1 << 8) >= DIVIDER) ? (8) : (9)))))))));
    const vx_uint16 BINS = (RANGE <= 1024) ? (RANGE) : (RANGE >> SHIFT);

    // Buffer Histogram and Features in BRAM
    vx_uint16 LUTA[BINS], LUTB[BINS];

    KeyPoint feature_buffer[MAX_INPUT_FEATURES];
    PRAGMA_HLS_MEM_CORE(feature_buffer, KeyPoint, MAX_INPUT_FEATURES)

    // Global variables
    vx_uint16 pre_bin = 0, pre_val = 0, retain_val = 0, retain_bin = 0, prefix_sum = 0, dst_ptr = 0,
              border_feature_cnt = 0;
    vx_uint32 feature_num = 0;
    bool read_done = false;

    // Reset histogram
    ResetHistogram<vx_uint16, BINS>(LUTA, LUTB);

    // Compute Histogram
    for (vx_uint32 i = 0; i < MAX_INPUT_FEATURES + 1; i++) {
#pragma HLS PIPELINE II = 1
        KeyPoint src;
        vx_uint16 cur_bin = 0, cur_val = 0;

        // Read input and update histogram
        if (read_done == false) {
            src = input[i];
            RetainBestHistogram<MAX_INPUT_FEATURES, MIN_RESPONSE_VAL, SHIFT, BINS>(
                i, src, pre_bin, pre_val, cur_bin, cur_val, feature_num, read_done, LUTA, LUTB, feature_buffer);
        }

        // Update to store in next iteration
        ++cur_val;
        pre_bin = cur_bin;
        pre_val = cur_val;
    }

    // Computes the prefix sum, pointer to the histogram entry to retain best
    RetainBestPrefixSum<MAX_OUTPUT_FEATURES, BINS>(LUTA, LUTB, prefix_sum, retain_val, retain_bin);

    // Go through feature list, 1 extra cycle to send end of list message
    for (vx_uint32 i = 0; i < MAX_INPUT_FEATURES; ++i) {
#pragma HLS PIPELINE II = 1
        vx_image_data<KeyPoint, 1> dst;
        KeyPoint feature;
        bool write_feature = false;

        // Gets feature below below/equal histogram entry. partially writes features 1 hist entry above to fill
        RetainBestFeature<MAX_INPUT_FEATURES, MAX_OUTPUT_FEATURES, MIN_RESPONSE_VAL, SHIFT, BINS>(
            i, feature_num, retain_val, retain_bin, feature_buffer, feature, write_feature, border_feature_cnt);

        // Write feature to output
        if ((write_feature == true) && (dst_ptr < MAX_OUTPUT_FEATURES)) {
            output[dst_ptr] = feature;
            ++dst_ptr;
        }
    }
}

/***************************************************************************************************************************************************/
template <typename SrcType, vx_uint32 IMG_COLS, vx_uint32 IMG_ROWS, SrcType THRESHOLD>
void OrbDetection(vx_image_data<SrcType, 1> input[IMG_COLS * IMG_ROWS], vx_image_data<KeyPoint, 1> output[2048]) {
#pragma HLS INLINE

    //
    const vx_uint16 MAX_RESPONSE_VAL = 255;
    const vx_uint16 MIN_RESPONSE_VAL = 0;
    const vx_uint32 IMG1_COLS = (IMG_COLS / 2);
    const vx_uint32 IMG1_ROWS = (IMG_ROWS / 2);
    const vx_uint32 IMG2_COLS = (IMG_COLS / 4);
    const vx_uint32 IMG2_ROWS = (IMG_ROWS / 4);
    const vx_uint32 IMG3_COLS = (IMG_COLS / 8);
    const vx_uint32 IMG3_ROWS = (IMG_ROWS / 8);
    const vx_uint32 FEATURES0 = 659;
    const vx_uint32 FEATURES1 = 549;
    const vx_uint32 FEATURES2 = 458;
    const vx_uint32 FEATURES3 = 382;

    // FIFOs to stream data between functions
    vx_image_data<SrcType, 1> img00a[IMG_COLS * IMG_ROWS];
#pragma HLS STREAM variable = img00a depth = 16
#pragma HLS data_pack variable = img00a
#pragma HLS RESOURCE variable = img00a core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img00b[IMG_COLS * IMG_ROWS];
#pragma HLS STREAM variable = img00b depth = 16
#pragma HLS data_pack variable = img00b
#pragma HLS RESOURCE variable = img00b core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img01[IMG_COLS * IMG_ROWS];
#pragma HLS STREAM variable = img01 depth = 16
#pragma HLS data_pack variable = img01
#pragma HLS RESOURCE variable = img01 core = FIFO_LUTRAM

    vx_image_data<SrcType, 1> img10[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img10 depth = 16
#pragma HLS data_pack variable = img10
#pragma HLS RESOURCE variable = img10 core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img10a[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img10a depth = 16
#pragma HLS data_pack variable = img10a
#pragma HLS RESOURCE variable = img10a core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img10b[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img10b depth = 16
#pragma HLS data_pack variable = img10b
#pragma HLS RESOURCE variable = img10b core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img11[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img11 depth = 16
#pragma HLS data_pack variable = img11
#pragma HLS RESOURCE variable = img11 core = FIFO_LUTRAM

    vx_image_data<SrcType, 1> img20[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img20 depth = 16
#pragma HLS data_pack variable = img20
#pragma HLS RESOURCE variable = img20 core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img20a[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img20a depth = 16
#pragma HLS data_pack variable = img20a
#pragma HLS RESOURCE variable = img20a core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img20b[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img20b depth = 16
#pragma HLS data_pack variable = img20b
#pragma HLS RESOURCE variable = img20b core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img21[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img21 depth = 16
#pragma HLS data_pack variable = img21
#pragma HLS RESOURCE variable = img21 core = FIFO_LUTRAM

    vx_image_data<SrcType, 1> img30[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img30 depth = 16
#pragma HLS data_pack variable = img30
#pragma HLS RESOURCE variable = img30 core = FIFO_LUTRAM
    vx_image_data<SrcType, 1> img31[IMG1_COLS * IMG1_ROWS];
#pragma HLS STREAM variable = img31 depth = 16
#pragma HLS data_pack variable = img31
#pragma HLS RESOURCE variable = img31 core = FIFO_LUTRAM

    vx_image_data<KeyPoint, 1> features0[4096];
#pragma HLS STREAM variable = features0 depth = 16
#pragma HLS data_pack variable = features0
#pragma HLS RESOURCE variable = features0 core = FIFO_LUTRAM
    vx_image_data<KeyPoint, 1> features1[4096];
#pragma HLS STREAM variable = features1 depth = 16
#pragma HLS data_pack variable = features1
#pragma HLS RESOURCE variable = features1 core = FIFO_LUTRAM
    vx_image_data<KeyPoint, 1> features2[4096];
#pragma HLS STREAM variable = features2 depth = 16
#pragma HLS data_pack variable = features2
#pragma HLS RESOURCE variable = features2 core = FIFO_LUTRAM
    vx_image_data<KeyPoint, 1> features3[4096];
#pragma HLS STREAM variable = features3 depth = 16
#pragma HLS data_pack variable = features3
#pragma HLS RESOURCE variable = features3 core = FIFO_LUTRAM

    vx_image_data<KeyPoint, 1> output0[FEATURES0];
#pragma HLS STREAM variable = output0 depth = 512
#pragma HLS data_pack variable = output0
#pragma HLS RESOURCE variable = output0 core = FIFO_BRAM
    vx_image_data<KeyPoint, 1> output1[FEATURES1];
#pragma HLS STREAM variable = output1 depth = 512
#pragma HLS data_pack variable = output1
#pragma HLS RESOURCE variable = output1 core = FIFO_BRAM
    vx_image_data<KeyPoint, 1> output2[FEATURES2];
#pragma HLS STREAM variable = output2 depth = 512
#pragma HLS data_pack variable = output2
#pragma HLS RESOURCE variable = output2 core = FIFO_BRAM
    vx_image_data<KeyPoint, 1> output3[FEATURES3];
#pragma HLS STREAM variable = output3 depth = 512
#pragma HLS data_pack variable = output3
#pragma HLS RESOURCE variable = output3 core = FIFO_BRAM

    // 4-level ORB Feature detection
#pragma HLS dataflow
    ImgMulticast<SrcType, (IMG_COLS * IMG_ROWS), 1>(input, img00a, img00b);
    ImgSegmentTestDetector<SrcType, 1, IMG_COLS, IMG_ROWS, VX_BORDER_UNDEFINED>(img00a, img01);
    ImgFeatureExtraction<SrcType, SrcType, 1, IMG_COLS, IMG_ROWS, 3, 0, THRESHOLD, 4096, 65536, 1, 0, HIFLIPVX::SQUARE,
                         vx_false_e>(img01, features0);
    RetainBestFixedRange<4096, FEATURES0, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(features0, output0);

    ImgScaleImage<SrcType, 1, IMG_COLS, IMG_ROWS, 2>(img00b, img10);
    ImgMulticast<SrcType, (IMG1_COLS * IMG1_ROWS), 1>(img10, img10a, img10b);
    ImgSegmentTestDetector<SrcType, 1, IMG1_COLS, IMG1_ROWS, VX_BORDER_UNDEFINED>(img10a, img11);
    ImgFeatureExtraction<SrcType, SrcType, 1, IMG1_COLS, IMG1_ROWS, 3, 0, THRESHOLD, 4096, 131072, 1, 1,
                         HIFLIPVX::SQUARE, vx_false_e>(img11, features1);
    RetainBestFixedRange<4096, FEATURES1, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(features1, output1);

    ImgScaleImage<SrcType, 1, IMG1_COLS, IMG1_ROWS, 2>(img10b, img20);
    ImgMulticast<SrcType, (IMG2_COLS * IMG2_ROWS), 1>(img20, img20a, img20b);
    ImgSegmentTestDetector<SrcType, 1, IMG2_COLS, IMG2_ROWS, VX_BORDER_UNDEFINED>(img20a, img21);
    ImgFeatureExtraction<SrcType, SrcType, 1, IMG2_COLS, IMG2_ROWS, 3, 0, THRESHOLD, 4096, 262144, 1, 2,
                         HIFLIPVX::SQUARE, vx_false_e>(img21, features2);
    RetainBestFixedRange<4096, FEATURES2, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(features2, output2);

    ImgScaleImage<SrcType, 1, IMG2_COLS, IMG2_ROWS, 2>(img20b, img30);
    ImgSegmentTestDetector<SrcType, 1, IMG3_COLS, IMG3_ROWS, VX_BORDER_UNDEFINED>(img30, img31);
    ImgFeatureExtraction<SrcType, SrcType, 1, IMG3_COLS, IMG3_ROWS, 3, 0, THRESHOLD, 4096, 524288, 1, 3,
                         HIFLIPVX::SQUARE, vx_false_e>(img31, features3);
    RetainBestFixedRange<4096, FEATURES3, MIN_RESPONSE_VAL, MAX_RESPONSE_VAL>(features3, output3);

    ImgFeatureGather<FEATURES0, FEATURES1, FEATURES2, FEATURES3, 2048, HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC>(
        output0, output1, output2, output3, output);
}

/***************************************************************************************************************************************************/
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM, vx_uint8 OUT_NUM>
void FeatureMulticast(vx_image_data<KeyPoint, VEC_NUM> input[ELEMENTS / VEC_NUM],
                      vx_image_data<KeyPoint, VEC_NUM> out0[ELEMENTS / VEC_NUM],
                      vx_image_data<KeyPoint, VEC_NUM> out1[ELEMENTS / VEC_NUM],
                      vx_image_data<KeyPoint, VEC_NUM> out2[ELEMENTS / VEC_NUM],
                      vx_image_data<KeyPoint, VEC_NUM> out3[ELEMENTS / VEC_NUM]) {
#pragma HLS INLINE

    STATIC_ASSERT((ELEMENTS % VEC_NUM) == 0, FEATURE_NUM_NOT_MULTIPLE_OF_VECTOR_SIZE);
    STATIC_ASSERT((OUT_NUM > 1) && (OUT_NUM < 5), OUTPUT_IMAGE_NUM_OUT_OF_RANGE);
    STATIC_ASSERT((VEC_NUM > 0) && (VEC_NUM < 9), VECTOR_SIZE_OUT_OF_RANGE);

    vx_bool eof = vx_false_e;
    for (vx_uint32 i = 0; i < (ELEMENTS / VEC_NUM); i++) {
#pragma HLS PIPELINE II = 1

        // dont read new elements after last signal has arrived
        if (eof == vx_false_e) {

            // read new element
            vx_image_data<KeyPoint, VEC_NUM> src = input[i];

            // get last element in vector to check if it is end of file
            KeyPointConverter kp_end;
            kp_end.keypoint = src.pixel[VEC_NUM - 1];
            if (kp_end.data == INVALID_KEYPOINT_DATA)
                eof = vx_true_e;

            // Set user(SOF) & last(EOF) if needed
            vx_bool last = (kp_end.data == INVALID_KEYPOINT_DATA) || (i == (ELEMENTS / VEC_NUM - 1));
            GenerateDmaSignal<KeyPoint, VEC_NUM>((i == 0), last, src);

            // send input to all valid outputs
            if (OUT_NUM >= 1)
                out0[i] = src;
            if (OUT_NUM >= 2)
                out1[i] = src;
            if (OUT_NUM >= 3)
                out2[i] = src;
            if (OUT_NUM >= 4)
                out3[i] = src;
        }
    }
}

/***************************************************************************************************************************************************/
template <vx_uint32 ELEMENTS, vx_uint8 VEC_NUM, vx_uint8 OUT_NUM>
void FeatureMulticast(KeyPoint input[ELEMENTS], KeyPoint out0[ELEMENTS], KeyPoint out1[ELEMENTS],
                      KeyPoint out2[ELEMENTS], KeyPoint out3[ELEMENTS]) {
#pragma HLS INLINE

    STATIC_ASSERT((ELEMENTS % VEC_NUM) == 0, FEATURE_NUM_NOT_MULTIPLE_OF_VECTOR_SIZE);
    STATIC_ASSERT((OUT_NUM > 1) && (OUT_NUM < 5) == 0, OUTPUT_IMAGE_NUM_OUT_OF_RANGE);
    STATIC_ASSERT((VEC_NUM > 0) && (VEC_NUM < 2) == 0, VECTOR_SIZE_OUT_OF_RANGE);

    vx_bool eof = vx_false_e;
    for (vx_uint32 i = 0; i < ELEMENTS; i++) {
#pragma HLS PIPELINE II = 1

        // dont read new elements after last signal has arrived
        if (eof == vx_false_e) {

            // read new element
            KeyPoint src = input[i];

            // get last element in vector to check if it is end of file
            KeyPointConverter kp_end;
            kp_end.keypoint = src;
            if (kp_end.data == INVALID_KEYPOINT_DATA)
                eof = vx_true_e;

            // send input to all valid outputs
            if (OUT_NUM >= 1)
                out0[i] = src;
            if (OUT_NUM >= 2)
                out1[i] = src;
            if (OUT_NUM >= 3)
                out2[i] = src;
            if (OUT_NUM >= 4)
                out3[i] = src;
        }
    }
}

#endif // SRC_IMG_FEATURES_CORE_H_
