/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_test_help.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These functions test the mt feature functions
 */

#ifndef __SRC_IMG_MT_FEATURE_TEST_H__
#define __SRC_IMG_MT_FEATURE_TEST_H__

#include "img_mt_feature_base.h"

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS_INPUT>
vx_uint16 createTestKeypoints(vx_image_data<KeyPoint, 1> keypoints[MAX_KEYPOINTS_INPUT], const vx_uint64 start_id) {

    KeyPointConverter conv;
    conv.data = INVALID_KEYPOINT_DATA;
    const KeyPoint INVALID_KEYPOINT = conv.keypoint;

    const vx_uint16 number_of_keypoints = 1 + (rand() % (MAX_KEYPOINTS_INPUT));

    for (vx_uint16 i = 0; i < number_of_keypoints; i++) {
        conv.data = start_id + i;
        keypoints[i].pixel[0] = conv.keypoint;
    }

    if (number_of_keypoints < MAX_KEYPOINTS_INPUT) {
        keypoints[number_of_keypoints].pixel[0] = INVALID_KEYPOINT;
    }

    return number_of_keypoints;
}

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS_INPUT>
vx_uint16 createTestKeypoints(KeyPoint keypoints[MAX_KEYPOINTS_INPUT], const vx_uint64 start_id) {
    return createTestKeypoints<MAX_KEYPOINTS_INPUT>((vx_image_data<KeyPoint, 1> *)keypoints, start_id);
}

/**********************************************************************************************************************/
template <vx_uint16 NUMBER_OF_INPUTS, vx_uint32 MAX_KEYPOINTS_OUTPUT>
vx_bool testGatherResultsCyclic(const KeyPoint keypoints_output[MAX_KEYPOINTS_OUTPUT],
                                const vx_uint16 number_of_keypoints_per_input[NUMBER_OF_INPUTS],
                                const vx_uint64 number_of_keypoints[NUMBER_OF_INPUTS]) {

    KeyPointConverter conv;

    vx_bool success = vx_true_e;

    vx_uint32 dst = 0;
    vx_uint16 src = 0;

    while (dst < MIN(number_of_keypoints[NUMBER_OF_INPUTS - 1], MAX_KEYPOINTS_OUTPUT) && success) {

        for (vx_uint8 j = 0;
             j < NUMBER_OF_INPUTS && dst < MIN(number_of_keypoints[NUMBER_OF_INPUTS - 1], MAX_KEYPOINTS_OUTPUT); j++) {
            if (src < number_of_keypoints_per_input[j]) {
                const vx_uint64 start_id = (j == 0) ? (0) : (number_of_keypoints[j - 1]);
                const vx_uint64 expected = start_id + src;
                conv.keypoint = keypoints_output[dst];
                if (conv.data != expected) {
                    std::cout << "Error in results for " << NUMBER_OF_INPUTS << " inputs at index " << dst
                              << ". Expected " << expected << ", received " << conv.data << "\n";
                    success = vx_false_e;
                    break;
                }
                ++dst;
            }
        }

        ++src;
    }

    return success;
}

/**********************************************************************************************************************/
template <vx_uint16 NUMBER_OF_INPUTS, vx_uint32 MAX_KEYPOINTS_OUTPUT>
vx_bool testGatherResultsBlock(const KeyPoint keypoints[MAX_KEYPOINTS_OUTPUT], const vx_uint32 number_of_keypoints) {
    KeyPointConverter conv;

    vx_bool success = vx_true_e;

    for (vx_uint64 i = 0; i < MIN(number_of_keypoints, MAX_KEYPOINTS_OUTPUT) && success; i++) {
        conv.keypoint = keypoints[i];
        if (i != conv.data) {
            std::cout << "Error in results for " << NUMBER_OF_INPUTS << " inputs at index " << i << ". Expected " << i
                      << ", received " << conv.data << "\n";
            success = vx_false_e;
        }
    }

    return success;
}

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2, vx_uint16 MAX_KEYPOINTS_INPUT_3,
          vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5, vx_uint16 MAX_KEYPOINTS_INPUT_6,
          vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8, vx_uint16 MAX_KEYPOINTS_INPUT_9,
          vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11, vx_uint16 MAX_KEYPOINTS_INPUT_12,
          vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14, vx_uint16 MAX_KEYPOINTS_INPUT_15,
          vx_uint16 MAX_KEYPOINTS_INPUT_16, vx_uint32 MAX_KEYPOINTS_OUTPUT_2, vx_uint32 MAX_KEYPOINTS_OUTPUT_3,
          vx_uint32 MAX_KEYPOINTS_OUTPUT_4, vx_uint32 MAX_KEYPOINTS_OUTPUT_5, vx_uint32 MAX_KEYPOINTS_OUTPUT_6,
          vx_uint32 MAX_KEYPOINTS_OUTPUT_7, vx_uint32 MAX_KEYPOINTS_OUTPUT_8, vx_uint32 MAX_KEYPOINTS_OUTPUT_9,
          vx_uint32 MAX_KEYPOINTS_OUTPUT_10, vx_uint32 MAX_KEYPOINTS_OUTPUT_11, vx_uint32 MAX_KEYPOINTS_OUTPUT_12,
          vx_uint32 MAX_KEYPOINTS_OUTPUT_13, vx_uint32 MAX_KEYPOINTS_OUTPUT_14, vx_uint32 MAX_KEYPOINTS_OUTPUT_15,
          vx_uint32 MAX_KEYPOINTS_OUTPUT_16, HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
vx_bool TestGatherKeypoints() {

    KeyPoint *keypoints_input_1 = new KeyPoint[MAX_KEYPOINTS_INPUT_1];
    KeyPoint *keypoints_input_2 = new KeyPoint[MAX_KEYPOINTS_INPUT_2];
    KeyPoint *keypoints_input_3 = new KeyPoint[MAX_KEYPOINTS_INPUT_3];
    KeyPoint *keypoints_input_4 = new KeyPoint[MAX_KEYPOINTS_INPUT_4];
    KeyPoint *keypoints_input_5 = new KeyPoint[MAX_KEYPOINTS_INPUT_5];
    KeyPoint *keypoints_input_6 = new KeyPoint[MAX_KEYPOINTS_INPUT_6];
    KeyPoint *keypoints_input_7 = new KeyPoint[MAX_KEYPOINTS_INPUT_7];
    KeyPoint *keypoints_input_8 = new KeyPoint[MAX_KEYPOINTS_INPUT_8];
    KeyPoint *keypoints_input_9 = new KeyPoint[MAX_KEYPOINTS_INPUT_9];
    KeyPoint *keypoints_input_10 = new KeyPoint[MAX_KEYPOINTS_INPUT_10];
    KeyPoint *keypoints_input_11 = new KeyPoint[MAX_KEYPOINTS_INPUT_11];
    KeyPoint *keypoints_input_12 = new KeyPoint[MAX_KEYPOINTS_INPUT_12];
    KeyPoint *keypoints_input_13 = new KeyPoint[MAX_KEYPOINTS_INPUT_13];
    KeyPoint *keypoints_input_14 = new KeyPoint[MAX_KEYPOINTS_INPUT_14];
    KeyPoint *keypoints_input_15 = new KeyPoint[MAX_KEYPOINTS_INPUT_15];
    KeyPoint *keypoints_input_16 = new KeyPoint[MAX_KEYPOINTS_INPUT_16];

    KeyPoint *keypoints_output_2 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_2];
    KeyPoint *keypoints_output_3 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_3];
    KeyPoint *keypoints_output_4 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_4];
    KeyPoint *keypoints_output_5 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_5];
    KeyPoint *keypoints_output_6 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_6];
    KeyPoint *keypoints_output_7 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_7];
    KeyPoint *keypoints_output_8 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_8];
    KeyPoint *keypoints_output_9 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_9];
    KeyPoint *keypoints_output_10 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_10];
    KeyPoint *keypoints_output_11 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_11];
    KeyPoint *keypoints_output_12 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_12];
    KeyPoint *keypoints_output_13 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_13];
    KeyPoint *keypoints_output_14 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_14];
    KeyPoint *keypoints_output_15 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_15];
    KeyPoint *keypoints_output_16 = new KeyPoint[MAX_KEYPOINTS_OUTPUT_16];

    vx_uint64 number_of_keypoints[16];
    vx_uint16 number_of_keypoints_per_input[16];

    std::cout << "Run test for gather function in ";
    if (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK) {
        std::cout << "block ";
    } else {
        std::cout << "cyclic ";
    }
    std::cout << "mode for up to 16 inputs.\n";

    number_of_keypoints_per_input[0] = createTestKeypoints<MAX_KEYPOINTS_INPUT_1>(keypoints_input_1, 0);
    number_of_keypoints[0] = number_of_keypoints_per_input[0];
    number_of_keypoints_per_input[1] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_2>(keypoints_input_2, number_of_keypoints[0]);
    number_of_keypoints[1] = number_of_keypoints[0] + number_of_keypoints_per_input[1];
    number_of_keypoints_per_input[2] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_3>(keypoints_input_3, number_of_keypoints[1]);
    number_of_keypoints[2] = number_of_keypoints[1] + number_of_keypoints_per_input[2];
    number_of_keypoints_per_input[3] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_4>(keypoints_input_4, number_of_keypoints[2]);
    number_of_keypoints[3] = number_of_keypoints[2] + number_of_keypoints_per_input[3];
    number_of_keypoints_per_input[4] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_5>(keypoints_input_5, number_of_keypoints[3]);
    number_of_keypoints[4] = number_of_keypoints[3] + number_of_keypoints_per_input[4];
    number_of_keypoints_per_input[5] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_6>(keypoints_input_6, number_of_keypoints[4]);
    number_of_keypoints[5] = number_of_keypoints[4] + number_of_keypoints_per_input[5];
    number_of_keypoints_per_input[6] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_7>(keypoints_input_7, number_of_keypoints[5]);
    number_of_keypoints[6] = number_of_keypoints[5] + number_of_keypoints_per_input[6];
    number_of_keypoints_per_input[7] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_8>(keypoints_input_8, number_of_keypoints[6]);
    number_of_keypoints[7] = number_of_keypoints[6] + number_of_keypoints_per_input[7];
    number_of_keypoints_per_input[8] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_9>(keypoints_input_9, number_of_keypoints[7]);
    number_of_keypoints[8] = number_of_keypoints[7] + number_of_keypoints_per_input[8];
    number_of_keypoints_per_input[9] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_10>(keypoints_input_10, number_of_keypoints[8]);
    number_of_keypoints[9] = number_of_keypoints[8] + number_of_keypoints_per_input[9];
    number_of_keypoints_per_input[10] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_11>(keypoints_input_11, number_of_keypoints[9]);
    number_of_keypoints[10] = number_of_keypoints[9] + number_of_keypoints_per_input[10];
    number_of_keypoints_per_input[11] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_12>(keypoints_input_12, number_of_keypoints[10]);
    number_of_keypoints[11] = number_of_keypoints[10] + number_of_keypoints_per_input[11];
    number_of_keypoints_per_input[12] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_13>(keypoints_input_13, number_of_keypoints[11]);
    number_of_keypoints[12] = number_of_keypoints[11] + number_of_keypoints_per_input[12];
    number_of_keypoints_per_input[13] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_14>(keypoints_input_14, number_of_keypoints[12]);
    number_of_keypoints[13] = number_of_keypoints[12] + number_of_keypoints_per_input[13];
    number_of_keypoints_per_input[14] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_15>(keypoints_input_15, number_of_keypoints[13]);
    number_of_keypoints[14] = number_of_keypoints[13] + number_of_keypoints_per_input[14];
    number_of_keypoints_per_input[15] =
        createTestKeypoints<MAX_KEYPOINTS_INPUT_16>(keypoints_input_16, number_of_keypoints[14]);
    number_of_keypoints[15] = number_of_keypoints[14] + number_of_keypoints_per_input[15];

    // call functions
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_OUTPUT_2, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_output_2);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_OUTPUT_3,
                     SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_output_3);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_OUTPUT_4, SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3,
                                                           keypoints_input_4, keypoints_output_4);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_OUTPUT_5, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_output_5);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_OUTPUT_6, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_output_6);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_OUTPUT_7,
                     SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4,
                                   keypoints_input_5, keypoints_input_6, keypoints_input_7, keypoints_output_7);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_OUTPUT_8, SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3,
                                                           keypoints_input_4, keypoints_input_5, keypoints_input_6,
                                                           keypoints_input_7, keypoints_input_8, keypoints_output_8);

    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_OUTPUT_9, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_output_9);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_OUTPUT_10, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_input_10,
        keypoints_output_10);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_OUTPUT_11,
                     SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4,
                                   keypoints_input_5, keypoints_input_6, keypoints_input_7, keypoints_input_8,
                                   keypoints_input_9, keypoints_input_10, keypoints_input_11, keypoints_output_11);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                     MAX_KEYPOINTS_OUTPUT_12, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_input_10,
        keypoints_input_11, keypoints_input_12, keypoints_output_12);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                     MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_OUTPUT_13, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_input_10,
        keypoints_input_11, keypoints_input_12, keypoints_input_13, keypoints_output_13);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                     MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_OUTPUT_14, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_input_10,
        keypoints_input_11, keypoints_input_12, keypoints_input_13, keypoints_input_14, keypoints_output_14);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                     MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_OUTPUT_5,
                     SCATTER_MODE>(keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4,
                                   keypoints_input_5, keypoints_input_6, keypoints_input_7, keypoints_input_8,
                                   keypoints_input_9, keypoints_input_10, keypoints_input_11, keypoints_input_12,
                                   keypoints_input_13, keypoints_input_14, keypoints_input_15, keypoints_output_15);
    ImgFeatureGather<MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                     MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                     MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                     MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_INPUT_16,
                     MAX_KEYPOINTS_OUTPUT_16, SCATTER_MODE>(
        keypoints_input_1, keypoints_input_2, keypoints_input_3, keypoints_input_4, keypoints_input_5,
        keypoints_input_6, keypoints_input_7, keypoints_input_8, keypoints_input_9, keypoints_input_10,
        keypoints_input_11, keypoints_input_12, keypoints_input_13, keypoints_input_14, keypoints_input_15,
        keypoints_input_16, keypoints_output_16);

    // test results
    vx_bool success =
        (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
            ? (testGatherResultsBlock<2, MAX_KEYPOINTS_OUTPUT_2>(keypoints_output_2, number_of_keypoints[1]))
            : (testGatherResultsCyclic<2, MAX_KEYPOINTS_OUTPUT_2>(keypoints_output_2, number_of_keypoints_per_input,
                                                                  number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<3, MAX_KEYPOINTS_OUTPUT_3>(keypoints_output_3, number_of_keypoints[2]))
                   : (testGatherResultsCyclic<3, MAX_KEYPOINTS_OUTPUT_3>(
                         keypoints_output_3, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<4, MAX_KEYPOINTS_OUTPUT_4>(keypoints_output_4, number_of_keypoints[3]))
                   : (testGatherResultsCyclic<4, MAX_KEYPOINTS_OUTPUT_4>(
                         keypoints_output_4, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<5, MAX_KEYPOINTS_OUTPUT_5>(keypoints_output_5, number_of_keypoints[4]))
                   : (testGatherResultsCyclic<5, MAX_KEYPOINTS_OUTPUT_5>(
                         keypoints_output_5, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<6, MAX_KEYPOINTS_OUTPUT_6>(keypoints_output_6, number_of_keypoints[5]))
                   : (testGatherResultsCyclic<6, MAX_KEYPOINTS_OUTPUT_6>(
                         keypoints_output_6, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<7, MAX_KEYPOINTS_OUTPUT_7>(keypoints_output_7, number_of_keypoints[6]))
                   : (testGatherResultsCyclic<7, MAX_KEYPOINTS_OUTPUT_7>(
                         keypoints_output_7, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<8, MAX_KEYPOINTS_OUTPUT_8>(keypoints_output_8, number_of_keypoints[7]))
                   : (testGatherResultsCyclic<8, MAX_KEYPOINTS_OUTPUT_8>(
                         keypoints_output_8, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<9, MAX_KEYPOINTS_OUTPUT_9>(keypoints_output_9, number_of_keypoints[8]))
                   : (testGatherResultsCyclic<9, MAX_KEYPOINTS_OUTPUT_9>(
                         keypoints_output_9, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<10, MAX_KEYPOINTS_OUTPUT_10>(keypoints_output_10, number_of_keypoints[9]))
                   : (testGatherResultsCyclic<10, MAX_KEYPOINTS_OUTPUT_10>(
                         keypoints_output_10, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<11, MAX_KEYPOINTS_OUTPUT_11>(keypoints_output_11, number_of_keypoints[10]))
                   : (testGatherResultsCyclic<11, MAX_KEYPOINTS_OUTPUT_11>(
                         keypoints_output_11, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<12, MAX_KEYPOINTS_OUTPUT_12>(keypoints_output_12, number_of_keypoints[11]))
                   : (testGatherResultsCyclic<12, MAX_KEYPOINTS_OUTPUT_12>(
                         keypoints_output_12, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<13, MAX_KEYPOINTS_OUTPUT_13>(keypoints_output_13, number_of_keypoints[12]))
                   : (testGatherResultsCyclic<13, MAX_KEYPOINTS_OUTPUT_13>(
                         keypoints_output_13, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<14, MAX_KEYPOINTS_OUTPUT_14>(keypoints_output_14, number_of_keypoints[13]))
                   : (testGatherResultsCyclic<14, MAX_KEYPOINTS_OUTPUT_14>(
                         keypoints_output_14, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<15, MAX_KEYPOINTS_OUTPUT_15>(keypoints_output_15, number_of_keypoints[14]))
                   : (testGatherResultsCyclic<15, MAX_KEYPOINTS_OUTPUT_15>(
                         keypoints_output_15, number_of_keypoints_per_input, number_of_keypoints));
    success &= (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK)
                   ? (testGatherResultsBlock<16, MAX_KEYPOINTS_OUTPUT_16>(keypoints_output_16, number_of_keypoints[15]))
                   : (testGatherResultsCyclic<16, MAX_KEYPOINTS_OUTPUT_16>(
                         keypoints_output_16, number_of_keypoints_per_input, number_of_keypoints));

    // free memory
    delete[] keypoints_input_1;
    delete[] keypoints_input_2;
    delete[] keypoints_input_3;
    delete[] keypoints_input_4;
    delete[] keypoints_input_5;
    delete[] keypoints_input_6;
    delete[] keypoints_input_7;
    delete[] keypoints_input_8;
    delete[] keypoints_input_9;
    delete[] keypoints_input_10;
    delete[] keypoints_input_11;
    delete[] keypoints_input_12;
    delete[] keypoints_input_13;
    delete[] keypoints_input_14;
    delete[] keypoints_input_15;
    delete[] keypoints_input_16;

    delete[] keypoints_output_2;
    delete[] keypoints_output_3;
    delete[] keypoints_output_4;
    delete[] keypoints_output_5;
    delete[] keypoints_output_6;
    delete[] keypoints_output_7;
    delete[] keypoints_output_8;
    delete[] keypoints_output_9;
    delete[] keypoints_output_10;
    delete[] keypoints_output_11;
    delete[] keypoints_output_12;
    delete[] keypoints_output_13;
    delete[] keypoints_output_14;
    delete[] keypoints_output_15;
    delete[] keypoints_output_16;

    return success;
}

/**********************************************************************************************************************/
void sw_test_gather() {

    // test configurations
    const vx_uint16 MAX_KEYPOINTS_INPUT_1 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_2 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_3 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_4 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_5 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_6 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_7 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_8 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_9 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_10 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_11 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_12 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_13 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_14 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_15 = 16384;
    const vx_uint16 MAX_KEYPOINTS_INPUT_16 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_2 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_3 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_4 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_5 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_6 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_7 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_8 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_9 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_10 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_11 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_12 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_13 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_14 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_15 = 16384;
    const vx_uint16 MAX_KEYPOINTS_OUTPUT_16 = 16384;

    checkSuccess(TestGatherKeypoints<
                 MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                 MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                 MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                 MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_INPUT_16,
                 MAX_KEYPOINTS_OUTPUT_2, MAX_KEYPOINTS_OUTPUT_3, MAX_KEYPOINTS_OUTPUT_4, MAX_KEYPOINTS_OUTPUT_5,
                 MAX_KEYPOINTS_OUTPUT_6, MAX_KEYPOINTS_OUTPUT_7, MAX_KEYPOINTS_OUTPUT_8, MAX_KEYPOINTS_OUTPUT_9,
                 MAX_KEYPOINTS_OUTPUT_10, MAX_KEYPOINTS_OUTPUT_11, MAX_KEYPOINTS_OUTPUT_12, MAX_KEYPOINTS_OUTPUT_13,
                 MAX_KEYPOINTS_OUTPUT_14, MAX_KEYPOINTS_OUTPUT_15, MAX_KEYPOINTS_OUTPUT_16,
                 HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK>());

    checkSuccess(TestGatherKeypoints<
                 MAX_KEYPOINTS_INPUT_1, MAX_KEYPOINTS_INPUT_2, MAX_KEYPOINTS_INPUT_3, MAX_KEYPOINTS_INPUT_4,
                 MAX_KEYPOINTS_INPUT_5, MAX_KEYPOINTS_INPUT_6, MAX_KEYPOINTS_INPUT_7, MAX_KEYPOINTS_INPUT_8,
                 MAX_KEYPOINTS_INPUT_9, MAX_KEYPOINTS_INPUT_10, MAX_KEYPOINTS_INPUT_11, MAX_KEYPOINTS_INPUT_12,
                 MAX_KEYPOINTS_INPUT_13, MAX_KEYPOINTS_INPUT_14, MAX_KEYPOINTS_INPUT_15, MAX_KEYPOINTS_INPUT_16,
                 MAX_KEYPOINTS_OUTPUT_2, MAX_KEYPOINTS_OUTPUT_3, MAX_KEYPOINTS_OUTPUT_4, MAX_KEYPOINTS_OUTPUT_5,
                 MAX_KEYPOINTS_OUTPUT_6, MAX_KEYPOINTS_OUTPUT_7, MAX_KEYPOINTS_OUTPUT_8, MAX_KEYPOINTS_OUTPUT_9,
                 MAX_KEYPOINTS_OUTPUT_10, MAX_KEYPOINTS_OUTPUT_11, MAX_KEYPOINTS_OUTPUT_12, MAX_KEYPOINTS_OUTPUT_13,
                 MAX_KEYPOINTS_OUTPUT_14, MAX_KEYPOINTS_OUTPUT_15, MAX_KEYPOINTS_OUTPUT_16,
                 HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC>());
}

/**********************************************************************************************************************/
template <vx_uint8 VEC_NUM_NMS, vx_uint8 CMP_VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE,
          vx_uint8 LEVEL, vx_uint16 NSUBLEVELS, vx_uint16 BORDER_WIDTH, vx_uint32 THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_bool WITH_SUBPIXEL_REFINEMENT>
void test_compare_keypoints_create_keypoints(vx_image_data<KeyPoint, CMP_VEC_NUM> kps_hw[MAX_KEYPOINTS],
                                             vx_image_data<KeyPoint, 1> kps_sw[MAX_KEYPOINTS], vx_uint16 &nr_kps) {

    const vx_uint8 INVALID_SCALE = 255;

    const vx_uint16 MAX_SIDE_LENGTH = MAX(IMG_COLS, IMG_ROWS);
    ;

    const vx_uint32 NUMBER_OF_PIXELS = IMG_COLS * IMG_ROWS;
    const vx_uint32 VEC_PIXELS = NUMBER_OF_PIXELS / VEC_NUM_NMS;
    vx_image_data<vx_int32, VEC_NUM_NMS> *input_nms = new vx_image_data<vx_int32, VEC_NUM_NMS>[VEC_PIXELS];

    // first create a random picture
    for (vx_uint32 i = 0; i < VEC_PIXELS; i++) {
        for (vx_uint8 j = 0; j < VEC_NUM_NMS; j++) {
            // chances of sign shall be 25%
            const vx_int32 sign = ((rand() % 100) < 25) ? (-1) : (1);
            // chances for division is 50%
            const vx_int32 do_div = ((rand() % 100) < 50);
            const vx_uint16 mod_1 = rand() + 1; // bias of one to avoid modulo of 0
            const vx_uint16 mod_2 = rand() + 1; // bias of one to avoid modulo of 0
            const vx_int32 div = rand();
            // because rand returns only up to 16 bit values (0x7fff = 32767, minimum defined by the standard) create
            // two random values which are multiplied together
            const vx_int32 rand_1 = rand() % mod_1;
            const vx_int32 rand_2 = rand() % mod_2;
            const vx_int32 pixel = (sign * rand_1 * rand_2) / (do_div * div + 1); // bias of one to avoid division by 0
            input_nms[i].pixel[j] = pixel;
        }
    }

    const vx_uint32 SAMPLING_FACTOR = (1 << (LEVEL / NSUBLEVELS)) << 16; //((LEVEL / NSUBLEVELS) + 1) << 16;
    ImgFeatureExtraction<vx_int32, vx_uint8, VEC_NUM_NMS, IMG_COLS, IMG_ROWS, KERN_SIZE, 0, THRESHOLD, MAX_KEYPOINTS,
                         SAMPLING_FACTOR, BORDER_WIDTH, LEVEL, HIFLIPVX::SQUARE, WITH_SUBPIXEL_REFINEMENT>(input_nms,
                                                                                                           kps_hw);

    nr_kps = 0;

    vx_bool stop = vx_false_e;

    // count number of detected keypoints and removes invalid keypoints from the input for the SW implementation
    for (vx_uint32 i = 0; i < MAX_KEYPOINTS && (!stop); i++) {

        // stays only true if all entries of the vector are invalid keypoints signaling the end of the keypoint list
        stop = vx_true_e;

        for (vx_uint8 j = 0; j < CMP_VEC_NUM; j++) {

            const KeyPoint kp_hw = kps_hw[i].pixel[j];

            if (kp_hw.scale != INVALID_SCALE) {
                if (kps_sw != NULL) {
                    kps_sw[nr_kps].pixel[0] = kp_hw;
                }
                ++nr_kps;
                stop = vx_false_e;
            }
        }
    }

    delete[] input_nms;
}

/**********************************************************************************************************************/
template <vx_uint8 VEC_NUM_NMS, vx_uint8 CMP_VEC_NUM, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE,
          vx_uint8 LEVEL, vx_uint16 NSUBLEVELS, vx_uint16 BORDER_WIDTH, vx_uint32 THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_bool WITH_SUBPIXEL_REFINEMENT>
void test_compare_keypoints_create_keypoints(KeyPoint kps_hw[CMP_VEC_NUM * MAX_KEYPOINTS],
                                             KeyPoint kps_sw[MAX_KEYPOINTS], vx_uint16 &nr_kps) {
    test_compare_keypoints_create_keypoints<VEC_NUM_NMS, CMP_VEC_NUM, IMG_COLS, IMG_ROWS, KERN_SIZE, LEVEL, NSUBLEVELS,
                                            BORDER_WIDTH, THRESHOLD, MAX_KEYPOINTS, WITH_SUBPIXEL_REFINEMENT>(
        (vx_image_data<KeyPoint, CMP_VEC_NUM> *)kps_hw, (vx_image_data<KeyPoint, 1> *)kps_sw, nr_kps);
}

/**********************************************************************************************************************/
vx_bool compare_deserilization_keypoint(const KeyPoint lhs, const KeyPoint rhs) { return (lhs.scale != rhs.scale); }

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS> vx_uint16 count_keypoints(vx_image_data<KeyPoint, 1> kps[MAX_KEYPOINTS]) {
    vx_uint16 counter = 0;
    for (vx_uint16 i = 0; i < MAX_KEYPOINTS; i++) {

        KeyPointConverter kpc;
        const KeyPoint kp = kps[i].pixel[0];
        kpc.keypoint = kp;

        if (kpc.data == INVALID_KEYPOINT_DATA) {
            break;
        } else {
            ++counter;
        }
    }
    return counter;
}

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS> vx_uint16 count_keypoints(KeyPoint kps[MAX_KEYPOINTS]) {
    return count_keypoints<MAX_KEYPOINTS>((vx_image_data<KeyPoint, 1> *)kps);
}

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS_SW, vx_uint16 MAX_KEYPOINTS_HW>
vx_bool test_compare_keypoints_verify_result(const vx_image_data<KeyPoint, 1> output_sw[MAX_KEYPOINTS_SW],
                                             const vx_image_data<KeyPoint, 1> output_hw[MAX_KEYPOINTS_HW],
                                             const vx_uint16 number_of_keypoints) {

    // If the number of keypoints is zero then not even invalid keypoint was written to the list which cannot happen if
    // the algorithm works as intended.
    vx_bool success = (number_of_keypoints > 0);

    for (vx_uint16 i = 0; i < number_of_keypoints; i++) {
        const KeyPoint kp_sw = output_sw[i].pixel[0];
        const KeyPoint kp_hw = output_hw[i].pixel[0];

        if (memcmp(&kp_sw, &kp_hw, sizeof(KeyPoint)) != 0) {
            std::cout << "Error the keypoint with index " << i << " differs!\n";
            std::cout << "SW (x, y, response, scale, orientation): " << kp_sw.x << ", " << kp_sw.y << ", "
                      << kp_sw.response << ", " << static_cast<vx_uint16>(kp_sw.scale) << ", "
                      << static_cast<vx_uint16>(kp_sw.orientation) << "\n";
            std::cout << "HW (x, y, response, scale, orientation): " << kp_hw.x << ", " << kp_hw.y << ", "
                      << kp_hw.response << ", " << static_cast<vx_uint16>(kp_hw.scale) << ", "
                      << static_cast<vx_uint16>(kp_hw.orientation) << "\n";
            success = vx_false_e;
            break;
        }
    }
    if (success) {
        std::cout << "All keypoints found in HW match with the keypoints found in SW and are also in the same order.\n";
    }
    return success;
}

/**********************************************************************************************************************/
template <vx_uint16 MAX_KEYPOINTS_SW, vx_uint16 MAX_KEYPOINTS_HW>
vx_bool test_compare_keypoints_verify_result(const KeyPoint output_sw[MAX_KEYPOINTS_SW],
                                             const KeyPoint output_hw[MAX_KEYPOINTS_HW],
                                             const vx_uint16 number_of_keypoints) {
    return test_compare_keypoints_verify_result<MAX_KEYPOINTS_SW, MAX_KEYPOINTS_HW>(
        (vx_image_data<KeyPoint, 1> *)output_sw, (vx_image_data<KeyPoint, 1> *)output_hw, number_of_keypoints);
}

/**********************************************************************************************************************/
template <vx_uint16 TEST_CYCLES, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 VEC_NUM_NMS, vx_uint16 MAX_KEYPOINTS>
vx_bool TestDeserialization() {

    vx_bool success = vx_true_e;

    const vx_uint8 KERN_SIZE = 3;

    const vx_uint32 THRESHOLD = static_cast<vx_uint32>(0.02 * 65536 * 65536);
    const vx_uint16 LEVEL = 1;
    const vx_uint16 NSUBLEVELS = 4;
    const vx_uint16 BORDER_WIDTH = 1;
    const vx_uint16 MAX_SIDE_LENGTH = MAX(IMG_COLS, IMG_ROWS);
    const vx_bool WITH_SUBPIXEL_REFINEMENT = vx_false_e;

    const vx_uint32 NUMBER_OF_PIXELS = IMG_COLS * IMG_ROWS;
    const vx_uint32 VEC_PIXELS = NUMBER_OF_PIXELS / VEC_NUM_NMS;

    const vx_uint8 VEC_NUM_DES_IN = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_NMS, KERN_SIZE);
    vx_image_data<vx_int32, VEC_NUM_NMS> *input_nms = new vx_image_data<vx_int32, VEC_NUM_NMS>[VEC_PIXELS];
    vx_image_data<vx_int32, 1> *input_nms_vec_one = new vx_image_data<vx_int32, 1>[NUMBER_OF_PIXELS];
    vx_image_data<KeyPoint, VEC_NUM_DES_IN> *output_nms = new vx_image_data<KeyPoint, VEC_NUM_DES_IN>[MAX_KEYPOINTS];
    vx_image_data<KeyPoint, 1> *output_expected = new vx_image_data<KeyPoint, 1>[MAX_KEYPOINTS];
    vx_image_data<KeyPoint, 1> *output_des = new vx_image_data<KeyPoint, 1>[MAX_KEYPOINTS];

    KeyPointConverter kv;
    kv.data = INVALID_KEYPOINT_DATA;

    for (vx_uint16 i = 0; i < TEST_CYCLES && success; i++) {

        std::cout << "Test of Deserialization function\n";
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "Resolution: " << IMG_COLS << "x" << IMG_ROWS << "\n";
        std::cout << "Maximum number of keypoints: " << MAX_KEYPOINTS << "\n";
        std::cout << "NMS input vector size: " << static_cast<vx_uint16>(VEC_NUM_NMS) << "\n";
        std::cout << "Deserialization input vector size: " << static_cast<vx_uint16>(VEC_NUM_DES_IN) << "\n";
        std::cout << "Test cycle " << i + 1 << " of " << TEST_CYCLES << ".\n";

        // first create a random picture
        for (vx_uint32 i = 0; i < NUMBER_OF_PIXELS / VEC_NUM_NMS; i++) {
            for (vx_uint8 j = 0; j < VEC_NUM_NMS; j++) {
                // changes of sign shall be 25%
                const vx_int32 sign = (rand() % 100 < 25) ? (-1) : (1);
                // divider used to reduce the changes of values over the threshold
                const vx_int32 div = rand() % 1024 + 1;
                // because rand returns only up to 16 bit values (0x7fff = 32767, minimum defined by the standard)
                // create two random values which are multiplied together
                const vx_int32 rand_1 = rand() % 65536;
                const vx_int32 rand_2 = rand() % 65536;
                const vx_int32 pixel = sign * (rand_1 * rand_2) / div;
                input_nms[i].pixel[j] = pixel;
                input_nms_vec_one[i * VEC_NUM_NMS + j].pixel[0] = pixel;
            }
        }
        const vx_uint32 SAMPLING_FACTOR = (1 << (LEVEL / NSUBLEVELS)) << 16; //((LEVEL / NSUBLEVELS) + 1) << 16;

        // do feature extraction with vector size one
        ImgFeatureExtraction<vx_int32, vx_uint8, 1, IMG_COLS, IMG_ROWS, KERN_SIZE, 0, THRESHOLD, MAX_KEYPOINTS,
                             SAMPLING_FACTOR, BORDER_WIDTH, LEVEL, HIFLIPVX::SQUARE, WITH_SUBPIXEL_REFINEMENT>(
            input_nms_vec_one, output_expected);
        // do feature extraction of the same image with the input vector size
        ImgFeatureExtraction<vx_int32, vx_uint8, VEC_NUM_NMS, IMG_COLS, IMG_ROWS, KERN_SIZE, 0, THRESHOLD,
                             MAX_KEYPOINTS, SAMPLING_FACTOR, BORDER_WIDTH, LEVEL, HIFLIPVX::SQUARE,
                             WITH_SUBPIXEL_REFINEMENT>(input_nms, output_nms);
        // do the deserialization
        HwDeserialization<KeyPoint, VEC_NUM_DES_IN, MAX_KEYPOINTS, MAX_KEYPOINTS>(output_nms, output_des, kv.keypoint,
                                                                                  compare_deserilization_keypoint);

        // count the number of keypoints for both the gold standard and the deserialized output
        const vx_uint16 nr_vec_one_kp = count_keypoints<MAX_KEYPOINTS>(output_expected);
        const vx_uint16 nr_des_kp = count_keypoints<MAX_KEYPOINTS>(output_des);

        if (nr_vec_one_kp != nr_des_kp) {
            printf("Number of keypoints after deserialization is %i but expected was %i!\n", nr_des_kp, nr_vec_one_kp);
        } else {
            printf("%i keypoints detected.\n", nr_vec_one_kp);
        }

        // compare results
        success &= test_compare_keypoints_verify_result<MAX_KEYPOINTS, MAX_KEYPOINTS>(output_expected, output_des,
                                                                                      nr_vec_one_kp);
    }
    delete[] input_nms;
    delete[] input_nms_vec_one;
    delete[] output_nms;
    delete[] output_expected;
    delete[] output_des;

    return success;
}

/**********************************************************************************************************************/
void sw_test_deserialization() {

    // test configurations
    const vx_uint16 TEST_WIDTH = 1920;
    const vx_uint16 TEST_HEIGHT = 1080;
    const vx_uint16 TEST_CYCLES = 10;
    const vx_uint16 TEST_MAX_KEYPOINTS = 16384;

    checkSuccess(TestDeserialization<TEST_CYCLES, TEST_WIDTH, TEST_HEIGHT, 4, TEST_MAX_KEYPOINTS>());
    checkSuccess(TestDeserialization<TEST_CYCLES, TEST_WIDTH, TEST_HEIGHT, 8, TEST_MAX_KEYPOINTS>());
}

/**********************************************************************************************************************/
template <typename InType, vx_uint16 WIDTH, vx_uint16 HEIGHT, InType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint16 LEVEL, vx_uint16 NSUBLEVEL, vx_uint16 BORDER_WIDTH, vx_uint16 MAX_SIDE_LENGTH, vx_bool WITH_SR,
          vx_bool WITH_NMS>
void SwFeatureExtraction(const float *input, KeyPoint *outKpoint, vx_uint16 &kpointSize) {

    kpointSize = 0;

    const vx_uint16 OCTAVE = LEVEL / NSUBLEVEL;
    const vx_uint16 SCALE = 1 << OCTAVE;

    const vx_uint16 COORDINATE_BITS = 16;
    const vx_uint16 RESPONSE_VALUE_BITS = 16;
    const vx_uint16 BIT_SHIFT_RESPONSE_VALUE = (sizeof(InType) << 3) - RESPONSE_VALUE_BITS;
    const vx_uint8 INTEGER_BITS = int_ceil_log2(MAX_SIDE_LENGTH);
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;
    const vx_uint16 FRAC_MUL = static_cast<vx_uint16>(1) << FRACTION_BITS;
    const vx_uint16 FRAC_MASK = FRAC_MUL - 1;

    const float fthreshold = fractionToFloat(THRESHOLD);

    const vx_uint8 ORIENTATION = 0;

    for (vx_uint32 ix = BORDER_WIDTH; ix < HEIGHT - BORDER_WIDTH; ix++) {

        // Y pointer
        const float *in_l = &input[(ix - 1) * WIDTH];
        const float *in_m = &input[(ix)*WIDTH];
        const float *in_h = &input[(ix + 1) * WIDTH];

        for (vx_uint32 jx = BORDER_WIDTH; jx < WIDTH - BORDER_WIDTH; jx++) {

            // Check if maximum is reached
            if (kpointSize == MAX_KEYPOINTS)
                break;

            // Filter the points with the detector threshold
            float value = in_m[jx];

            if ((value > fthreshold) &&
                ((!WITH_NMS) ||
                 (value >= in_m[jx - 1] && value > in_m[jx + 1] && value >= in_l[jx - 1] && value >= in_l[jx] &&
                  value >= in_l[jx + 1] && value > in_h[jx - 1] && value > in_h[jx] && value > in_h[jx + 1]))) {

                bool isKpoint = vx_true_e;
                float res0 = 0.0f;
                float res1 = 0.0f;

                if (WITH_SR) {
                    // Compute the gradient and the Hessian
                    float Dx = 0.5f * (in_m[jx + 1] - in_m[jx - 1]);
                    float Dy = 0.5f * (in_h[jx] - in_l[jx]);
                    float Dxx = (in_m[jx + 1] + in_m[jx - 1] - 2.0f * value);
                    float Dyy = (in_h[jx] + in_l[jx] - 2.0f * value);
                    float Dxy = 0.25f * ((in_h[jx + 1] + in_l[jx - 1]) - (in_l[jx + 1] + in_h[jx - 1]));

                    // Build and solve the linear system with gaussian elimination
                    const float res1_num = (Dx * Dxy - Dy * Dxx);
                    const float res1_den = (Dyy * Dxx - Dxy * Dxy);
                    res1 = res1_num / res1_den;
                    const float res0_num = -(Dx * res1_den + Dxy * res1_num);
                    const float res0_den = Dxx * res1_den;
                    res0 = res0_num / res0_den;

                    // Create KP information
                    isKpoint = fabs(res0) <= 1.0 && fabs(res1) <= 1.0;
                }

                if (isKpoint) {

                    const float x = ((float)jx + res0) * SCALE;
                    const float y = ((float)ix + res1) * SCALE;

                    const vx_uint16 res_x = (static_cast<vx_uint16>(x) << FRACTION_BITS) |
                                            (static_cast<vx_uint16>(x * FRAC_MUL) & FRAC_MASK);
                    const vx_uint16 res_y = (static_cast<vx_uint16>(y) << FRACTION_BITS) |
                                            (static_cast<vx_uint16>(y * FRAC_MUL) & FRAC_MASK);

                    // Store KP Information, + 0.5f rounding
                    outKpoint[kpointSize++] =
                        KeyPoint{res_x, res_y,
                                 static_cast<vx_uint16>(floatToFraction<InType>(value) >> BIT_SHIFT_RESPONSE_VALUE),
                                 LEVEL, ORIENTATION};
                }
            }
        }
    }
}

/**********************************************************************************************************************/
template <vx_uint8 VEC_NUM_OUT, vx_uint16 MAX_KEYPOINTS>
vx_uint16 CheckKeypointError(KeyPoint *keypoint_sw, KeyPoint *keypoint_hw, vx_uint16 &number_of_keypoints_hw,
                             const vx_uint16 number_of_keypoints_sw, const vx_uint16 max_deviation) {

    std::cout << "Check keypoints (x, y, response, scale, orientation).\n";

    KeyPoint *tmp_keypoint_hw = new KeyPoint[MAX_KEYPOINTS];

    KeyPointConverter conv;
    conv.data = INVALID_KEYPOINT_DATA;

    vx_image_data<KeyPoint, 1> inv;
    inv.pixel[0] = conv.keypoint;

    KeyPoint invalid_keypoint = inv.pixel[0];

    number_of_keypoints_hw = 0;

    vx_bool stop = vx_false_e;

    // count number of detected keypoints
    for (vx_uint32 i = 0; i < MAX_KEYPOINTS && (!stop); i++) {

        // stays only true if all entries of the vector are invalid keypoints signaling the end of the keypoint list
        stop = vx_true_e;

        for (vx_uint8 j = 0; j < VEC_NUM_OUT; j++) {

            const KeyPoint khw = keypoint_hw[i * VEC_NUM_OUT + j];

            if (khw.scale != invalid_keypoint.scale) {
                tmp_keypoint_hw[number_of_keypoints_hw] = khw;
                ++number_of_keypoints_hw;
                stop = vx_false_e;
            }
        }
    }

    vx_uint16 left_overs_sw_id = 0;
    vx_uint16 left_overs_hw_id = 0;

    std::cout << "Keypoints not in HW detected or have higher deviation than allowed: \n";

    for (vx_uint16 i = 0; i < number_of_keypoints_sw; i++) {

        const KeyPoint ksw = keypoint_sw[i];

        vx_bool detected = vx_false_e;

        for (vx_uint16 j = 0; j < number_of_keypoints_hw && !detected; j++) {

            const KeyPoint khw = tmp_keypoint_hw[j];

            const vx_uint16 x = ABS(ksw.x - khw.x);
            const vx_uint16 y = ABS(ksw.y - khw.y);

            const vx_uint16 r = ABS(ksw.response - khw.response);

            detected = x <= max_deviation && y <= max_deviation && r < 2 && ksw.scale == khw.scale &&
                       ksw.orientation == khw.orientation;
        }

        if (!detected) {
            std::cout << i << ": {" << ksw.x << ", " << ksw.y << ", " << ksw.response << ", "
                      << static_cast<vx_uint16>(ksw.scale) << ", " << static_cast<vx_uint16>(ksw.orientation) << "}\n";
            ++left_overs_sw_id;
        }
    }

    std::cout << "Keypoints not in SW detected or have higher deviation than allowed: \n";

    for (vx_uint16 i = 0; i < number_of_keypoints_hw; i++) {

        const KeyPoint khw = tmp_keypoint_hw[i];

        vx_bool detected = vx_false_e;

        for (vx_uint16 j = 0; j < number_of_keypoints_sw && !detected; j++) {

            const KeyPoint ksw = keypoint_sw[j];

            const vx_uint16 x = ABS(ksw.x - khw.x);
            const vx_uint16 y = ABS(ksw.y - khw.y);

            const vx_uint16 r = ABS(ksw.response - khw.response);

            detected = x < max_deviation && y < max_deviation && r < 2 && ksw.scale == khw.scale &&
                       ksw.orientation == khw.orientation;
        }

        if (!detected) {
            std::cout << i << ": {" << khw.x << ", " << khw.y << ", " << khw.response << ", "
                      << static_cast<vx_uint16>(khw.scale) << ", " << static_cast<vx_uint16>(khw.orientation) << "}\n";
            ++left_overs_hw_id;
        }
    }

    delete[] tmp_keypoint_hw;

    return MAX(left_overs_sw_id, left_overs_hw_id);
}

/**********************************************************************************************************************/
template <vx_uint16 WIDTH, vx_uint16 HEIGHT, vx_uint8 VEC_NUM, vx_uint16 THRESHOLD16, vx_uint32 THRESHOLD32,
          vx_uint16 MAX_KEYPOINTS, vx_uint16 LEVEL, vx_uint16 NSUBLEVELS, vx_uint16 BORDER_WIDTH, vx_bool WITH_SR,
          vx_uint16 TEST_CYCLES>
vx_bool TestFeatureExtraction(const vx_uint16 diff_limit_16, const vx_uint16 diff_limit_32,
                              const vx_uint16 diff_keypoints) {

    // constants
    const vx_uint8 OCTAVE = LEVEL / NSUBLEVELS;
    const vx_uint16 SCALE_HEIGHT = HEIGHT >> OCTAVE;
    const vx_uint16 SCALE_WIDTH = WIDTH >> OCTAVE;
    const vx_uint32 NUMBER_OF_PIXELS = SCALE_HEIGHT * SCALE_WIDTH;
    const vx_uint8 KERN_SIZE = 3;
    const vx_uint16 MAX_SIDE_LENGTH = MAX(WIDTH, HEIGHT);

    const vx_uint8 VEC_NUM_OUT = FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM, KERN_SIZE);

    vx_uint16 absDifMax_16 = 0;
    vx_uint16 absDifMax_32 = 0;

    vx_int16 *input16_hw;
    vx_int32 *input32_hw;

    float *input16_sw;
    float *input32_sw;

    KeyPoint *output16_hw;
    KeyPoint *output16_sw;
    KeyPoint *output32_hw;
    KeyPoint *output32_sw;

    /******** Allocate memory ******************************************************************/

    input16_hw = new vx_int16[NUMBER_OF_PIXELS];
    input16_sw = new float[NUMBER_OF_PIXELS];
    input32_hw = new vx_int32[NUMBER_OF_PIXELS];
    input32_sw = new float[NUMBER_OF_PIXELS];

    output16_hw = new KeyPoint[MAX_KEYPOINTS * VEC_NUM_OUT];
    output16_sw = new KeyPoint[MAX_KEYPOINTS];
    output32_hw = new KeyPoint[MAX_KEYPOINTS * VEC_NUM_OUT];
    output32_sw = new KeyPoint[MAX_KEYPOINTS];

    /******** Create image ******************************************************************/

    std::cout << "Test of find extrema with and without subpixel refinement function\n";
    std::cout << "------------------------------------------------------------------\n";
    std::cout << "Resolution: " << WIDTH << "x" << HEIGHT << "\n";
    std::cout << "Maximum number of keypoints: " << MAX_KEYPOINTS << "\n";
    std::cout << "Input vector size: " << static_cast<vx_uint16>(VEC_NUM) << "\n";
    std::cout << "Output vector size: " << static_cast<vx_uint16>(VEC_NUM_OUT) << "\n";
    std::cout << "Threshold 16 Bit: " << THRESHOLD16 << "\n";
    std::cout << "Threshold 32 Bit: " << THRESHOLD32 << "\n";
    std::cout << "Evolution level: " << LEVEL << "\n";
    std::cout << "With subpixel refinement: ";
    if (WITH_SR)
        std::cout << "Yes\n";
    else
        std::cout << "No\n";
    std::cout << "Octave: " << LEVEL / NSUBLEVELS << "\n";
    std::cout << "Sublevel: " << LEVEL % NSUBLEVELS << "\n";

    for (vx_uint16 i = 0; i < TEST_CYCLES; i++) {

        std::cout << "Cycle " << i + 1 << " of " << TEST_CYCLES << "\n";

        vx_uint16 absDif = 0;
        vx_uint16 number_of_keypoints16_sw = 0;
        vx_uint16 number_of_keypoints32_sw = 0;
        vx_uint16 number_of_keypoints_hw = 0;

        for (vx_uint32 i = 0; i < NUMBER_OF_PIXELS; i++) {
            const vx_int16 pixel = rand() % 65536;
            input16_hw[i] = pixel;
            input16_sw[i] = fractionToFloat<vx_int16>(pixel);
        }

        for (vx_uint32 i = 0; i < NUMBER_OF_PIXELS; i++) {
            const vx_int32 rand_1 = rand() % 65536;
            const vx_int32 rand_2 = rand() % 65536;
            const vx_int32 pixel = rand_1 * rand_2;
            input32_hw[i] = pixel;
            input32_sw[i] = fractionToFloat<vx_int32>(pixel);
        }

        const vx_uint32 SAMPLING_FACTOR = (1 << OCTAVE) << 16; //(OCTAVE + 1) << 16;

        std::cout << "16 bit: ";
        SwFeatureExtraction<vx_int16, SCALE_WIDTH, SCALE_HEIGHT, THRESHOLD16, MAX_KEYPOINTS, LEVEL, NSUBLEVELS,
                            BORDER_WIDTH, MAX_SIDE_LENGTH, WITH_SR, vx_true_e>(input16_sw, output16_sw,
                                                                               number_of_keypoints16_sw);
        ImgFeatureExtraction<vx_int16, vx_uint8, VEC_NUM, SCALE_WIDTH, SCALE_HEIGHT, KERN_SIZE, 0, THRESHOLD16,
                             MAX_KEYPOINTS, SAMPLING_FACTOR, BORDER_WIDTH, LEVEL, HIFLIPVX::SQUARE, WITH_SR>(
            input16_hw, output16_hw);
        absDif = CheckKeypointError<VEC_NUM_OUT, MAX_KEYPOINTS>(output16_sw, output16_hw, number_of_keypoints_hw,
                                                                number_of_keypoints16_sw, diff_limit_16);
        absDifMax_16 = MAX(absDifMax_16, absDif);
        std::cout << number_of_keypoints16_sw << " keypoints detected in SW and " << number_of_keypoints_hw
                  << " keypoints detected in HW.\n";

        std::cout << "32 bit: ";
        SwFeatureExtraction<vx_int32, SCALE_WIDTH, SCALE_HEIGHT, THRESHOLD32, MAX_KEYPOINTS, LEVEL, NSUBLEVELS,
                            BORDER_WIDTH, MAX_SIDE_LENGTH, WITH_SR, vx_true_e>(input32_sw, output32_sw,
                                                                               number_of_keypoints32_sw);
        ImgFeatureExtraction<vx_int32, vx_uint8, VEC_NUM, SCALE_WIDTH, SCALE_HEIGHT, KERN_SIZE, 0, THRESHOLD32,
                             MAX_KEYPOINTS, SAMPLING_FACTOR, BORDER_WIDTH, LEVEL, HIFLIPVX::SQUARE, WITH_SR>(
            input32_hw, output32_hw);
        absDif = CheckKeypointError<VEC_NUM_OUT, MAX_KEYPOINTS>(output32_sw, output32_hw, number_of_keypoints_hw,
                                                                number_of_keypoints32_sw, diff_limit_32);
        absDifMax_32 = MAX(absDifMax_32, absDif);
        std::cout << number_of_keypoints32_sw << " keypoints detected in SW and " << number_of_keypoints_hw
                  << " keypoints detected in HW.\n";
    }

    std::cout << "------------------------------------\n"
              << "Absolute difference over all cycles.\n"
              << "------------------------------------\n";
    std::cout << "16 Bit: " << std::fixed << std::setprecision(2) << absDifMax_16 << "\n";
    std::cout << "32 Bit: " << absDifMax_32 << "\n\n";

    const vx_bool success = (absDifMax_16 <= diff_limit_16) && (absDifMax_32 <= diff_limit_32);

    delete[] input16_hw;
    delete[] input16_sw;
    delete[] input32_hw;
    delete[] input32_sw;

    delete[] output16_hw;
    delete[] output16_sw;
    delete[] output32_hw;
    delete[] output32_sw;

    return success;
}

/**********************************************************************************************************************/
template <vx_bool WITH_SR> void sw_test_fe_sr() {

    // test configurations
    const vx_uint16 TEST_WIDTH = 1920;
    const vx_uint16 TEST_HEIGHT = 1080;
    const vx_uint16 TEST_THRESHOLD16 = static_cast<vx_uint16>(0.0005 * 65536);
    const vx_uint32 TEST_THRESHOLD32 = static_cast<vx_uint32>(0.0005 * 65536 * 65536);
    const vx_uint16 TEST_MAX_KEYPOINTS = 16384;
    const vx_uint16 TEST_NSUBLEVELS = 4;
    const vx_uint16 TEST_BORDER_WIDTH = 1;
    const vx_uint16 TEST_CYCLES = 1;

    // maximum threshold for deviation if this threshold is reached than does the test function return false and stops
    // the whole testing process
    const vx_uint16 max_deviation_coordinate_octave_0_16 = 3;
    const vx_uint16 max_deviation_coordinate_octave_1_16 = 3;
    const vx_uint16 max_deviation_coordinate_octave_2_16 = 3;
    const vx_uint16 max_deviation_coordinate_octave_3_16 = 3;
    const vx_uint16 max_deviation_coordinate_octave_4_16 = 3;

    const vx_uint16 max_deviation_coordinate_octave_0_32 = 3;
    const vx_uint16 max_deviation_coordinate_octave_1_32 = 3;
    const vx_uint16 max_deviation_coordinate_octave_2_32 = 3;
    const vx_uint16 max_deviation_coordinate_octave_3_32 = 3;
    const vx_uint16 max_deviation_coordinate_octave_4_32 = 3;

    const vx_uint16 max_deviation_keypoint_octave_0 = 2;
    const vx_uint16 max_deviation_keypoint_octave_1 = 2;
    const vx_uint16 max_deviation_keypoint_octave_2 = 2;
    const vx_uint16 max_deviation_keypoint_octave_3 = 2;
    const vx_uint16 max_deviation_keypoint_octave_4 = 2;

    // test for different number of octaves
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 1, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              0 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_0_16, max_deviation_coordinate_octave_0_32,
            max_deviation_keypoint_octave_0));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 1, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              1 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_1_16, max_deviation_coordinate_octave_1_32,
            max_deviation_keypoint_octave_1));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 1, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              2 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_2_16, max_deviation_coordinate_octave_2_32,
            max_deviation_keypoint_octave_2));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 1, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              3 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_3_16, max_deviation_coordinate_octave_3_32,
            max_deviation_keypoint_octave_3));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 1, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              4 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_4_16, max_deviation_coordinate_octave_4_32,
            max_deviation_keypoint_octave_4));

    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 2, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              0 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_0_16, max_deviation_coordinate_octave_0_32,
            max_deviation_keypoint_octave_0));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 2, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              1 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_1_16, max_deviation_coordinate_octave_1_32,
            max_deviation_keypoint_octave_1));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 2, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              2 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_2_16, max_deviation_coordinate_octave_2_32,
            max_deviation_keypoint_octave_2));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 2, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              3 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_3_16, max_deviation_coordinate_octave_3_32,
            max_deviation_keypoint_octave_3));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 2, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              4 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_4_16, max_deviation_coordinate_octave_4_32,
            max_deviation_keypoint_octave_4));

    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 4, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              0 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_0_16, max_deviation_coordinate_octave_0_32,
            max_deviation_keypoint_octave_0));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 4, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              1 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_1_16, max_deviation_coordinate_octave_1_32,
            max_deviation_keypoint_octave_1));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 4, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              2 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_2_16, max_deviation_coordinate_octave_2_32,
            max_deviation_keypoint_octave_2));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 4, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              3 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_3_16, max_deviation_coordinate_octave_3_32,
            max_deviation_keypoint_octave_3));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 4, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              4 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_4_16, max_deviation_coordinate_octave_4_32,
            max_deviation_keypoint_octave_4));

    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 8, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              0 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_0_16, max_deviation_coordinate_octave_0_32,
            max_deviation_keypoint_octave_0));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 8, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              1 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_1_16, max_deviation_coordinate_octave_1_32,
            max_deviation_keypoint_octave_1));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 8, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              2 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_2_16, max_deviation_coordinate_octave_2_32,
            max_deviation_keypoint_octave_2));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 8, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              3 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_3_16, max_deviation_coordinate_octave_3_32,
            max_deviation_keypoint_octave_3));
    checkSuccess(
        TestFeatureExtraction<TEST_WIDTH, TEST_HEIGHT, 8, TEST_THRESHOLD16, TEST_THRESHOLD32, TEST_MAX_KEYPOINTS,
                              4 * TEST_NSUBLEVELS, TEST_NSUBLEVELS, TEST_BORDER_WIDTH, WITH_SR, TEST_CYCLES>(
            max_deviation_coordinate_octave_4_16, max_deviation_coordinate_octave_4_32,
            max_deviation_keypoint_octave_4));
}

/**********************************************************************************************************************/
template <vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS,
          vx_uint16 MAX_KEYPOINTS_NEXT, vx_uint32 SCALE_WITH_FRACTION, vx_uint16 LEVEL, vx_uint16 BOUNDARY>
void SwCompareKeypoints(const KeyPoint *input_current, const KeyPoint *input_previous, const KeyPoint *input_next,
                        const vx_uint16 number_of_keypoints_current, const vx_uint16 number_of_keypoints_previous,
                        const vx_uint16 number_of_keypoints_next, vx_uint16 &number_of_keypoints, KeyPoint *output) {

    // Consider for the upper boundaries also that the last bits are fraction bits
    const vx_uint16 MAX_SIDE_LENGTH = MAX(IMG_COLS, IMG_ROWS);
    const vx_uint8 COORDINATE_BITS = sizeof(KeyPoint::x) << 3;
    const vx_uint8 INTEGER_BITS = comp_log2<MAX_SIDE_LENGTH>::log2_ceil;
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;
    const vx_uint16 LOWER_BOUNDARY = BOUNDARY << FRACTION_BITS;
    const vx_uint16 UPPER_BOUNDARY_COLS = (IMG_COLS - BOUNDARY) << FRACTION_BITS;
    const vx_uint16 UPPER_BOUNDARY_ROWS = (IMG_ROWS - BOUNDARY) << FRACTION_BITS;

    // compute the radius which contains a fractional part
    const vx_uint16 RADIUS_SQUARE = (static_cast<vx_uint64>(SCALE_WITH_FRACTION) * SCALE_WITH_FRACTION) >>
                                    ((sizeof(SCALE_WITH_FRACTION) << 3) - (FRACTION_BITS << 1));

    // if keypoint is invalid set scale to maximum possible value
    const vx_uint8 INVALID_SCALE = 255;

    const vx_uint32 TOTAL_NUMBER_OF_KEYPOINTS =
        number_of_keypoints_current + number_of_keypoints_previous + number_of_keypoints_next;

    KeyPoint *buffer = new KeyPoint[TOTAL_NUMBER_OF_KEYPOINTS];

    // Get Keypoints
    vx_uint32 ik = 0;
    for (vx_uint16 kp_prev = 0, kp_curr = 0; ik < TOTAL_NUMBER_OF_KEYPOINTS; ik++) {

        // Sort the lower with the upper level in y direction
        if (kp_curr < number_of_keypoints_current) {
            if (kp_prev < number_of_keypoints_previous) {
                const vx_uint16 yl = input_previous[kp_prev].y;
                const vx_uint16 ym = input_current[kp_curr].y;
                if (yl < ym) {
                    memcpy(&buffer[ik], &(input_previous[kp_prev]), sizeof(KeyPoint));
                    kp_prev++;
                } else {
                    memcpy(&buffer[ik], &(input_current[kp_curr]), sizeof(KeyPoint));
                    kp_curr++;
                }
            } else {
                memcpy(&buffer[ik], &(input_current[kp_curr]), sizeof(KeyPoint));
                kp_curr++;
            }
        } else {
            memcpy(&buffer[ik], &(input_previous[kp_prev]), sizeof(KeyPoint));
            kp_prev++;
        }

        // Get Keypoint
        KeyPoint &last_kp = buffer[ik];

        // If lower keypoint
        if (LEVEL != last_kp.scale)
            last_kp.scale = INVALID_SCALE;

        // Check for boundary
        if ((last_kp.x <= LOWER_BOUNDARY) || (last_kp.x >= UPPER_BOUNDARY_COLS) || (last_kp.y <= LOWER_BOUNDARY) ||
            (last_kp.y >= UPPER_BOUNDARY_ROWS))
            last_kp.scale = INVALID_SCALE;

        // Check if keypoint is extremum
        for (vx_int32 jk = ik - 1; jk >= 0; jk--) {

            // Get Keypoint to compare
            KeyPoint &buf_kp = buffer[jk];

            // Get distance
            vx_int32 distX = last_kp.x - buf_kp.x;
            vx_int32 distY = last_kp.y - buf_kp.y;
            vx_uint32 dist = distX * distX + distY * distY;

            // Check if KP is in distance
            if (dist <= RADIUS_SQUARE) {
                if (buf_kp.response > last_kp.response)
                    last_kp.scale = INVALID_SCALE;
                else
                    buf_kp.scale = INVALID_SCALE;
            }
        }
    }

    vx_uint16 ptr_dst = 0;
    for (vx_uint32 i = 0; i < TOTAL_NUMBER_OF_KEYPOINTS; i++) {

        KeyPoint &kp = buffer[i];

        if (kp.scale < INVALID_SCALE) {
            output[ptr_dst] = kp;
            ptr_dst++;
        }
    }

    if (ptr_dst < MAX_KEYPOINTS_CURRENT - 1) {
        KeyPointConverter kpc;
        kpc.data = INVALID_KEYPOINT_DATA;
        output[ptr_dst] = kpc.keypoint;
    }

    number_of_keypoints = ptr_dst;

    delete[] buffer;
}

/**********************************************************************************************************************/
template <vx_uint8 FE_VEC_NUM, vx_uint8 CMP_PARALLIZATION, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint16 TEST_CYCLES>
vx_bool TestCompareKeypoints() {

    const vx_uint8 FE_KERN_SIZE = 3;
    const vx_bool FE_WITH_SR = vx_false_e;
    const vx_uint16 FE_NSUBLEVELS = 4;
    const vx_uint16 FE_BORDER_WIDTH = 1;
    const vx_uint32 FE_THRESHOLD = static_cast<vx_uint32>(0.0005 * 65536 * 65536);

    const vx_uint8 CMP_VEC_NUM = FIND_EXTREMA_VEC_NUM_OUT(FE_VEC_NUM, FE_KERN_SIZE);
    const vx_uint64 CMP_MAX_ITERATIONS = 0;
    const vx_uint32 CMP_MAX_BUFFER_SIZE = 0;

    KeyPoint *keypoints_current_hw = new KeyPoint[CMP_VEC_NUM * MAX_KEYPOINTS_CURRENT];
    KeyPoint *keypoints_previous_hw =
        (MAX_KEYPOINTS_PREVIOUS > 0) ? (new KeyPoint[CMP_VEC_NUM * MAX_KEYPOINTS_PREVIOUS]) : (NULL);
    KeyPoint *keypoints_next_hw = (MAX_KEYPOINTS_NEXT > 0) ? (new KeyPoint[CMP_VEC_NUM * MAX_KEYPOINTS_NEXT]) : (NULL);
    KeyPoint *keypoints_current_sw = new KeyPoint[MAX_KEYPOINTS_CURRENT];
    KeyPoint *keypoints_previous_sw = (MAX_KEYPOINTS_PREVIOUS > 0) ? (new KeyPoint[MAX_KEYPOINTS_PREVIOUS]) : (NULL);
    KeyPoint *keypoints_next_sw = (MAX_KEYPOINTS_NEXT > 0) ? (new KeyPoint[MAX_KEYPOINTS_NEXT]) : (NULL);
    KeyPoint *output_sw = new KeyPoint[MAX_KEYPOINTS_CURRENT];
    KeyPoint *output_hw = new KeyPoint[MAX_KEYPOINTS_OUTPUT];

    const vx_uint16 COORDINATE_BITS = 16;
    const vx_uint16 MAX_SIDE_LENGTH = MAX(IMG_COLS, IMG_ROWS);
    const vx_uint8 INTEGER_BITS = comp_log2<MAX_SIDE_LENGTH>::log2_ceil;
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;
    const vx_uint32 SCALE_WITH_FRACTION = 175355;
    const vx_uint16 BOUNDARY = 57;
    const vx_uint8 LEVEL = 1;

    vx_bool success = vx_true_e;

    for (vx_uint16 i = 0; i < TEST_CYCLES && success; i++) {

        vx_uint16 number_of_keypoints_sw = 0;
        vx_uint16 number_of_keypoints_hw = 0;
        vx_uint16 kp_current = 0;
        vx_uint16 kp_previous = 0;
        vx_uint16 kp_next = 0;

        std::cout << "Test of compare keypoints function\n";
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "Resolution: " << IMG_COLS << "x" << IMG_ROWS << "\n";
        std::cout << "Maximum number of keypoints lower level: " << MAX_KEYPOINTS_PREVIOUS << "\n";
        std::cout << "Maximum number of keypoints current level: " << MAX_KEYPOINTS_CURRENT << "\n";
        std::cout << "Maximum number of keypoints following level: " << MAX_KEYPOINTS_NEXT << "\n";
        std::cout << "Maximum number of output keypoints: " << MAX_KEYPOINTS_OUTPUT << "\n";
        std::cout << "FE input vector size: " << static_cast<vx_uint16>(FE_VEC_NUM) << "\n";
        std::cout << "Compare input vector size: " << static_cast<vx_uint16>(CMP_VEC_NUM) << "\n";
        std::cout << "Compare parallelization degree: " << static_cast<vx_uint16>(CMP_PARALLIZATION) << "\n";
        std::cout << "Compare number of maximum iterations: ";
        (CMP_MAX_ITERATIONS > 0) ? std::cout << CMP_MAX_ITERATIONS : std::cout << "None";
        std::cout << "\n";
        std::cout << "FE Kernel size: " << static_cast<vx_uint16>(FE_KERN_SIZE) << "\n";
        std::cout << "FE threshold: " << FE_THRESHOLD << " = "
                  << static_cast<float>(FE_THRESHOLD) /
                         (static_cast<vx_uint64>(std::numeric_limits<vx_uint32>::max()) + 1)
                  << "\n";
        std::cout << "Evolution level: " << static_cast<vx_uint16>(LEVEL) << "\n";
        std::cout << "Scale: " << static_cast<float>(SCALE_WITH_FRACTION) / std::numeric_limits<vx_uint16>::max()
                  << "\n";
        std::cout << "Boundary: " << static_cast<float>(BOUNDARY) / FRACTION_BITS << "\n";
        std::cout << "Test cycle " << i + 1 << " of " << TEST_CYCLES << ".\n";

        test_compare_keypoints_create_keypoints<FE_VEC_NUM, CMP_VEC_NUM, IMG_COLS, IMG_ROWS, FE_KERN_SIZE, LEVEL,
                                                FE_NSUBLEVELS, FE_BORDER_WIDTH, FE_THRESHOLD, MAX_KEYPOINTS_CURRENT,
                                                FE_WITH_SR>(keypoints_current_hw, keypoints_current_sw, kp_current);
        test_compare_keypoints_create_keypoints<FE_VEC_NUM, CMP_VEC_NUM, IMG_COLS, IMG_ROWS, FE_KERN_SIZE, LEVEL - 1,
                                                FE_NSUBLEVELS, FE_BORDER_WIDTH, FE_THRESHOLD, MAX_KEYPOINTS_PREVIOUS,
                                                FE_WITH_SR>(keypoints_previous_hw, keypoints_previous_sw, kp_previous);
        // TODO test 3 input later: test_compare_keypoints_create_keypoints<VEC_NUM_NMS, CMP_VEC_NUM, IMG_COLS,
        // IMG_ROWS, KERN_SIZE, LEVEL + 1, MAX_KEYPOINTS_CURRENT, WITH_SUBPIXEL_REFINEMENT>(keypoints_next_hw,
        // keypoints_next_sw, kp_next);

        std::cout << "For the input of the previous level " << kp_previous << " keypoints were detected.\n";
        std::cout << "For the input of the current level " << kp_current << " keypoints were detected.\n";
        std::cout << "For the input of the next level " << kp_next << " keypoints were detected.\n";

        SwCompareKeypoints<IMG_COLS, IMG_ROWS, MAX_KEYPOINTS_CURRENT, MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT,
                           SCALE_WITH_FRACTION, LEVEL, BOUNDARY>(keypoints_current_sw, keypoints_previous_sw,
                                                                 keypoints_next_sw, kp_current, kp_previous, kp_next,
                                                                 number_of_keypoints_sw, output_sw);
        ImgFeatureComparison<CMP_VEC_NUM, MAX_SIDE_LENGTH, IMG_COLS, IMG_ROWS, MAX_KEYPOINTS_CURRENT,
                             MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT, MAX_KEYPOINTS_OUTPUT, SCALE_WITH_FRACTION,
                             LEVEL, BOUNDARY, FE_KERN_SIZE, CMP_PARALLIZATION, CMP_MAX_ITERATIONS, CMP_MAX_BUFFER_SIZE>(
            keypoints_current_hw, keypoints_previous_hw, NULL, output_hw);
        number_of_keypoints_hw = count_keypoints<MAX_KEYPOINTS_OUTPUT>(output_hw);

        if (number_of_keypoints_sw == number_of_keypoints_hw) {
            std::cout << "SW and HW have the same number of keypoints: " << number_of_keypoints_hw << "\n";
            success = vx_true_e;
        } else if (MAX_KEYPOINTS_OUTPUT == number_of_keypoints_hw) {
            std::cout << "HW reached maximum number of outputs. SW detected " << number_of_keypoints_sw
                      << " keypoints.\n";
            success = vx_true_e;
        } else {
            std::cout << "Number of keypoints for SW and HW do not match and maximum number of output keypoint in HW "
                         "was also not detected. SW has "
                      << number_of_keypoints_sw << " and HW " << number_of_keypoints_hw << ".\n";
            success = vx_false_e;
        }

        success &= test_compare_keypoints_verify_result<MAX_KEYPOINTS_CURRENT, MAX_KEYPOINTS_OUTPUT>(
            output_sw, output_hw, number_of_keypoints_hw);
    }

    delete[] keypoints_current_hw;
    delete[] keypoints_previous_hw;
    delete[] keypoints_next_hw;
    delete[] keypoints_current_sw;
    delete[] keypoints_previous_sw;
    delete[] keypoints_next_sw;
    delete[] output_sw;
    delete[] output_hw;

    return success;
}

/**********************************************************************************************************************/
void sw_test_compare_keypoints() {

    // test configurations
    const vx_uint16 TEST_WIDTH = 1920;
    const vx_uint16 TEST_HEIGHT = 1080;
    const vx_uint16 TEST_CYCLES = 10;
    const vx_uint16 TEST_MAX_KEYPOINTS_CURRENT = 16384;
    const vx_uint16 TEST_MAX_KEYPOINTS_PREVIOUS = 16384;
    const vx_uint16 TEST_MAX_KEYPOINTS_NEXT = 0;
    const vx_uint16 TEST_MAX_KEYPOINTS_OUTPUT = 16384;

    // Feature extraction vector size 1 & 2 -> Compare 1 and 4 -> 2 and 8 -> 3 for parallelization factor of 1
    checkSuccess(
        TestCompareKeypoints<1, 1, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT, TEST_MAX_KEYPOINTS_PREVIOUS,
                             TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    // checkSuccess(TestCompareKeypoints<4, 1, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    // checkSuccess(TestCompareKeypoints<8, 1, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());

    // Feature extraction vector size 1 & 2 -> Compare 1 and 4 -> 2 and 8 -> 3 for parallelization factor of 2
    checkSuccess(
        TestCompareKeypoints<1, 2, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT, TEST_MAX_KEYPOINTS_PREVIOUS,
                             TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<4, 2, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<8, 2, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());

    // Feature extraction vector size 1 & 2 -> Compare 1 and 4 -> 2 and 8 -> 3 for parallelization factor of 4
    checkSuccess(
        TestCompareKeypoints<1, 4, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT, TEST_MAX_KEYPOINTS_PREVIOUS,
                             TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<4, 4, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<8, 4, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());

    // Feature extraction vector size 1 & 2 -> Compare 1 and 4 -> 2 and 8 -> 3 for parallelization factor of 8
    checkSuccess(
        TestCompareKeypoints<1, 8, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT, TEST_MAX_KEYPOINTS_PREVIOUS,
                             TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<4, 8, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
    //	checkSuccess(TestCompareKeypoints<8, 8, TEST_WIDTH, TEST_HEIGHT, TEST_MAX_KEYPOINTS_CURRENT,
    // TEST_MAX_KEYPOINTS_PREVIOUS, TEST_MAX_KEYPOINTS_NEXT, TEST_MAX_KEYPOINTS_OUTPUT, TEST_CYCLES>());
}

/**********************************************************************************************************************/
//
#define IMG_COLS_ 1920
#define IMG_ROWS_ 1080
#define MAX_SIDE_LENGTH_ 1920
#define NR_PIXELS_ (IMG_COLS_ * IMG_ROWS_)
#define VEC_NUM_ 1
#define VEC_PIXELS_ (NR_PIXELS_ / VEC_NUM_)

//
#define NSUBLEVELS_ 4

// values for level 0-3 are the same for 4-7 and 8-11
#define FHD_RADIUS_LEVEL_0_ 147456
#define FHD_BOUNDARY_LEVEL_0_ 47
#define FHD_RADIUS_LEVEL_1_ 175355
#define FHD_BOUNDARY_LEVEL_1_ 57
#define FHD_RADIUS_LEVEL_2_ 208534
#define FHD_BOUNDARY_LEVEL_2_ 67
#define FHD_RADIUS_LEVEL_3_ 247990
#define FHD_BOUNDARY_LEVEL_3_ 78
#define FHD_RADIUS_LEVEL_8_ FHD_RADIUS_LEVEL_0_
#define FHD_BOUNDARY_LEVEL_8_ FHD_BOUNDARY_LEVEL_0_
#define FHD_RADIUS_LEVEL_9_ FHD_RADIUS_LEVEL_1_
#define FHD_BOUNDARY_LEVEL_9_ FHD_BOUNDARY_LEVEL_1_
#define FHD_RADIUS_LEVEL_10_ FHD_RADIUS_LEVEL_2_
#define FHD_BOUNDARY_LEVEL_10_ FHD_BOUNDARY_LEVEL_2_
#define FHD_RADIUS_LEVEL_11_ FHD_RADIUS_LEVEL_3_
#define FHD_BOUNDARY_LEVEL_11_ FHD_BOUNDARY_LEVEL_3_

//
#define GATHER_SCATTER_MODE_ HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC
#define GATHER_MAX_KEYPOINTS_INPUT_1_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_2_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_3_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_4_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_5_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_6_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_7_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_8_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_9_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_10_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_11_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_12_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_13_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_14_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_15_ 16384
#define GATHER_MAX_KEYPOINTS_INPUT_16_ 16384
#define GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_ 16384
#define GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_ 16384
#define GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_ 16384
#define GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_ 16384

//
#define FE_THRESHOLD_ 32
#define FE_LEVEL_ 1
#define FE_BORDER_WIDTH_ 1
#define FE_KERN_SIZE_ 3
#define FE_VEC_NUM_OUT_ (FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_, FE_KERN_SIZE_))
#define FE_MAX_KEYPOINTS_OUTPUT_ 16384
#define FE_WITH_SR_ vx_false_e

//
#define DES_VEC_NUM_IN_ FE_VEC_NUM_OUT_
#define DES_MAX_KEYPOINTS_INPUT_ 16384
#define DES_MAX_KEYPOINTS_OUTPUT_ 16384

//
#define CMP_VEC_NUM_IN_ FE_VEC_NUM_OUT_
#define CMP_MAX_KEYPOINTS_CURRENT_ 16384
#define CMP_MAX_KEYPOINTS_PREVIOUS_ 16384
#define CMP_MAX_KEYPOINTS_OUTPUT_ 16384
#define CMP_KERN_SIZE_ FE_KERN_SIZE_
#define CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_ 0
#define CMP_MAX_BUFFER_SIZE_ 0
#define CMP_PARALLELIZATION_ 1

//
typedef vx_image_data<KeyPoint, DES_VEC_NUM_IN_> des_inT;
typedef vx_image_data<KeyPoint, 1> des_outT;
typedef vx_int32 feT;
typedef vx_image_data<feT, VEC_NUM_> fe_imageT;
typedef vx_image_data<KeyPoint, FE_VEC_NUM_OUT_> keypoint_imageT;
typedef vx_image_data<KeyPoint, CMP_VEC_NUM_IN_> cmp_kp_inT;
typedef vx_image_data<KeyPoint, 1> cmp_kp_outT;

/**********************************************************************************************************************/
void TestHwGatherFeatures2(vx_image_data<KeyPoint, 1> input1[GATHER_MAX_KEYPOINTS_INPUT_1_],
                           vx_image_data<KeyPoint, 1> input2[GATHER_MAX_KEYPOINTS_INPUT_2_],
                           vx_image_data<KeyPoint, 1> output[GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_]) {
#pragma HLS INTERFACE ap_fifo port = input1
#pragma HLS INTERFACE ap_fifo port = input2
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_, GATHER_SCATTER_MODE_>(input1, input2, output);
}

/**********************************************************************************************************************/
void TestGatherKeypointsInput8(vx_image_data<KeyPoint, 1> input1[GATHER_MAX_KEYPOINTS_INPUT_1_],
                               vx_image_data<KeyPoint, 1> input2[GATHER_MAX_KEYPOINTS_INPUT_2_],
                               vx_image_data<KeyPoint, 1> input3[GATHER_MAX_KEYPOINTS_INPUT_3_],
                               vx_image_data<KeyPoint, 1> input4[GATHER_MAX_KEYPOINTS_INPUT_4_],
                               vx_image_data<KeyPoint, 1> input5[GATHER_MAX_KEYPOINTS_INPUT_5_],
                               vx_image_data<KeyPoint, 1> input6[GATHER_MAX_KEYPOINTS_INPUT_6_],
                               vx_image_data<KeyPoint, 1> input7[GATHER_MAX_KEYPOINTS_INPUT_7_],
                               vx_image_data<KeyPoint, 1> input8[GATHER_MAX_KEYPOINTS_INPUT_8_],
                               vx_image_data<KeyPoint, 1> output[GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_]) {
#pragma HLS INTERFACE ap_fifo port = input1
#pragma HLS INTERFACE ap_fifo port = input2
#pragma HLS INTERFACE ap_fifo port = input3
#pragma HLS INTERFACE ap_fifo port = input4
#pragma HLS INTERFACE ap_fifo port = input5
#pragma HLS INTERFACE ap_fifo port = input6
#pragma HLS INTERFACE ap_fifo port = input7
#pragma HLS INTERFACE ap_fifo port = input8
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, output);
}

void TestGatherKeypointsInput12(vx_image_data<KeyPoint, 1> input1[GATHER_MAX_KEYPOINTS_INPUT_1_],
                                vx_image_data<KeyPoint, 1> input2[GATHER_MAX_KEYPOINTS_INPUT_2_],
                                vx_image_data<KeyPoint, 1> input3[GATHER_MAX_KEYPOINTS_INPUT_3_],
                                vx_image_data<KeyPoint, 1> input4[GATHER_MAX_KEYPOINTS_INPUT_4_],
                                vx_image_data<KeyPoint, 1> input5[GATHER_MAX_KEYPOINTS_INPUT_5_],
                                vx_image_data<KeyPoint, 1> input6[GATHER_MAX_KEYPOINTS_INPUT_6_],
                                vx_image_data<KeyPoint, 1> input7[GATHER_MAX_KEYPOINTS_INPUT_7_],
                                vx_image_data<KeyPoint, 1> input8[GATHER_MAX_KEYPOINTS_INPUT_8_],
                                vx_image_data<KeyPoint, 1> input9[GATHER_MAX_KEYPOINTS_INPUT_9_],
                                vx_image_data<KeyPoint, 1> input10[GATHER_MAX_KEYPOINTS_INPUT_10_],
                                vx_image_data<KeyPoint, 1> input11[GATHER_MAX_KEYPOINTS_INPUT_11_],
                                vx_image_data<KeyPoint, 1> input12[GATHER_MAX_KEYPOINTS_INPUT_12_],
                                vx_image_data<KeyPoint, 1> output[GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_]) {
#pragma HLS INTERFACE ap_fifo port = input1
#pragma HLS INTERFACE ap_fifo port = input2
#pragma HLS INTERFACE ap_fifo port = input3
#pragma HLS INTERFACE ap_fifo port = input4
#pragma HLS INTERFACE ap_fifo port = input5
#pragma HLS INTERFACE ap_fifo port = input6
#pragma HLS INTERFACE ap_fifo port = input7
#pragma HLS INTERFACE ap_fifo port = input8
#pragma HLS INTERFACE ap_fifo port = input9
#pragma HLS INTERFACE ap_fifo port = input10
#pragma HLS INTERFACE ap_fifo port = input11
#pragma HLS INTERFACE ap_fifo port = input12
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_, GATHER_MAX_KEYPOINTS_INPUT_9_,
                     GATHER_MAX_KEYPOINTS_INPUT_10_, GATHER_MAX_KEYPOINTS_INPUT_11_, GATHER_MAX_KEYPOINTS_INPUT_12_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, output);
}

void TestGatherKeypointsInput16(vx_image_data<KeyPoint, 1> input1[GATHER_MAX_KEYPOINTS_INPUT_1_],
                                vx_image_data<KeyPoint, 1> input2[GATHER_MAX_KEYPOINTS_INPUT_2_],
                                vx_image_data<KeyPoint, 1> input3[GATHER_MAX_KEYPOINTS_INPUT_3_],
                                vx_image_data<KeyPoint, 1> input4[GATHER_MAX_KEYPOINTS_INPUT_4_],
                                vx_image_data<KeyPoint, 1> input5[GATHER_MAX_KEYPOINTS_INPUT_5_],
                                vx_image_data<KeyPoint, 1> input6[GATHER_MAX_KEYPOINTS_INPUT_6_],
                                vx_image_data<KeyPoint, 1> input7[GATHER_MAX_KEYPOINTS_INPUT_7_],
                                vx_image_data<KeyPoint, 1> input8[GATHER_MAX_KEYPOINTS_INPUT_8_],
                                vx_image_data<KeyPoint, 1> input9[GATHER_MAX_KEYPOINTS_INPUT_9_],
                                vx_image_data<KeyPoint, 1> input10[GATHER_MAX_KEYPOINTS_INPUT_10_],
                                vx_image_data<KeyPoint, 1> input11[GATHER_MAX_KEYPOINTS_INPUT_11_],
                                vx_image_data<KeyPoint, 1> input12[GATHER_MAX_KEYPOINTS_INPUT_12_],
                                vx_image_data<KeyPoint, 1> input13[GATHER_MAX_KEYPOINTS_INPUT_13_],
                                vx_image_data<KeyPoint, 1> input14[GATHER_MAX_KEYPOINTS_INPUT_14_],
                                vx_image_data<KeyPoint, 1> input15[GATHER_MAX_KEYPOINTS_INPUT_15_],
                                vx_image_data<KeyPoint, 1> input16[GATHER_MAX_KEYPOINTS_INPUT_16_],
                                vx_image_data<KeyPoint, 1> output[GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_]) {
#pragma HLS INTERFACE ap_fifo port = input1
#pragma HLS INTERFACE ap_fifo port = input2
#pragma HLS INTERFACE ap_fifo port = input3
#pragma HLS INTERFACE ap_fifo port = input4
#pragma HLS INTERFACE ap_fifo port = input5
#pragma HLS INTERFACE ap_fifo port = input6
#pragma HLS INTERFACE ap_fifo port = input7
#pragma HLS INTERFACE ap_fifo port = input8
#pragma HLS INTERFACE ap_fifo port = input9
#pragma HLS INTERFACE ap_fifo port = input10
#pragma HLS INTERFACE ap_fifo port = input11
#pragma HLS INTERFACE ap_fifo port = input12
#pragma HLS INTERFACE ap_fifo port = input13
#pragma HLS INTERFACE ap_fifo port = input14
#pragma HLS INTERFACE ap_fifo port = input15
#pragma HLS INTERFACE ap_fifo port = input16
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_, GATHER_MAX_KEYPOINTS_INPUT_9_,
                     GATHER_MAX_KEYPOINTS_INPUT_10_, GATHER_MAX_KEYPOINTS_INPUT_11_, GATHER_MAX_KEYPOINTS_INPUT_12_,
                     GATHER_MAX_KEYPOINTS_INPUT_13_, GATHER_MAX_KEYPOINTS_INPUT_14_, GATHER_MAX_KEYPOINTS_INPUT_15_,
                     GATHER_MAX_KEYPOINTS_INPUT_16_, GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, input13,
        input14, input15, input16, output);
}

/**********************************************************************************************************************/
vx_bool ImgTestDeserializationCompareDeserilizationKeypoint(const KeyPoint lhs, const KeyPoint rhs) {
#pragma HLS INLINE
    return (lhs.scale != rhs.scale);
}

void TestHwFeatureDeserialization(des_inT input[DES_MAX_KEYPOINTS_INPUT_], des_outT output[DES_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
#pragma HLS interface ap_ctrl_none port = return

    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

    ImgFeatureDeserialize<KeyPoint, FE_VEC_NUM_OUT_, DES_MAX_KEYPOINTS_INPUT_, DES_MAX_KEYPOINTS_OUTPUT_>(
        input, output, kp_converter_invalid_kp.keypoint, ImgTestDeserializationCompareDeserilizationKeypoint);
}

/**********************************************************************************************************************/
void TestImgFeatureExtraction(fe_imageT input[VEC_PIXELS_], keypoint_imageT output[FE_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE axis port = input
#pragma HLS INTERFACE axis port = output
#pragma HLS INTERFACE ap_ctrl_none port = return
    ImgFeatureExtraction<feT, vx_uint8, VEC_NUM_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_, 0, FE_THRESHOLD_,
                         FE_MAX_KEYPOINTS_OUTPUT_, (1 << 16), FE_BORDER_WIDTH_, 0, HIFLIPVX::SQUARE, FE_WITH_SR_>(
        input, output);
}

/**********************************************************************************************************************/
void TestHwFeatureComparisonEvolZero(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                     cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_, 0, 0,
                         CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_0_, 0, FHD_BOUNDARY_LEVEL_0_, CMP_KERN_SIZE_,
                         CMP_PARALLELIZATION_, CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(
        current, NULL, NULL, output);
}

/**********************************************************************************************************************/
void TestHwFeatureComparisonEvolOne(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                    cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                    cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_1_, 1,
                         FHD_BOUNDARY_LEVEL_1_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolTwo(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                 cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                 cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_2_, 2,
                         FHD_BOUNDARY_LEVEL_2_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolThree(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                   cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                   cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_3_, 3,
                         FHD_BOUNDARY_LEVEL_3_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolEight(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                   cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                   cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_ / 4, IMG_ROWS_ / 4, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_8_, 8,
                         FHD_BOUNDARY_LEVEL_8_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolNine(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                  cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                  cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_ / 4, IMG_ROWS_ / 4, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_9_, 9,
                         FHD_BOUNDARY_LEVEL_9_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolTen(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                 cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                 cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_ / 4, IMG_ROWS_ / 4, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_10_, 10,
                         FHD_BOUNDARY_LEVEL_10_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void TestCompareKeypointsEvolEleven(cmp_kp_inT current[CMP_MAX_KEYPOINTS_CURRENT_],
                                    cmp_kp_inT lower[CMP_MAX_KEYPOINTS_PREVIOUS_],
                                    cmp_kp_outT output[CMP_MAX_KEYPOINTS_OUTPUT_]) {
#pragma HLS INTERFACE ap_fifo port = current
#pragma HLS INTERFACE ap_fifo port = lower
#pragma HLS INTERFACE ap_fifo port = output
#pragma HLS interface ap_ctrl_hs port = return

    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_ / 4, IMG_ROWS_ / 4, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_11_, 11,
                         FHD_BOUNDARY_LEVEL_11_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output);
}

/**********************************************************************************************************************/
void cosim_gather_kp_16() {
    vx_image_data<KeyPoint, 1> *input1 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_1_];
    vx_image_data<KeyPoint, 1> *input2 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_2_];
    vx_image_data<KeyPoint, 1> *input3 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_3_];
    vx_image_data<KeyPoint, 1> *input4 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_4_];
    vx_image_data<KeyPoint, 1> *input5 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_5_];
    vx_image_data<KeyPoint, 1> *input6 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_6_];
    vx_image_data<KeyPoint, 1> *input7 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_7_];
    vx_image_data<KeyPoint, 1> *input8 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_8_];
    vx_image_data<KeyPoint, 1> *input9 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_9_];
    vx_image_data<KeyPoint, 1> *input10 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_10_];
    vx_image_data<KeyPoint, 1> *input11 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_11_];
    vx_image_data<KeyPoint, 1> *input12 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_12_];
    vx_image_data<KeyPoint, 1> *input13 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_13_];
    vx_image_data<KeyPoint, 1> *input14 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_14_];
    vx_image_data<KeyPoint, 1> *input15 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_15_];
    vx_image_data<KeyPoint, 1> *input16 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_16_];
    vx_image_data<KeyPoint, 1> *output_sw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_];
    vx_image_data<KeyPoint, 1> *output_hw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_];

#ifndef __SYNTHESIS__
    vx_uint64 keypoint_sum = createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_1_>(input1, 0);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_2_>(input2, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_3_>(input3, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_4_>(input4, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_5_>(input5, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_6_>(input6, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_7_>(input7, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_8_>(input8, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_9_>(input9, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_10_>(input10, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_11_>(input11, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_12_>(input12, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_13_>(input13, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_14_>(input14, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_15_>(input15, keypoint_sum);
    createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_16_>(input16, keypoint_sum);
#endif

    // test hardware against software
    TestGatherKeypointsInput16(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11,
                               input12, input13, input14, input15, input16, output_hw);
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_, GATHER_MAX_KEYPOINTS_INPUT_9_,
                     GATHER_MAX_KEYPOINTS_INPUT_10_, GATHER_MAX_KEYPOINTS_INPUT_11_, GATHER_MAX_KEYPOINTS_INPUT_12_,
                     GATHER_MAX_KEYPOINTS_INPUT_13_, GATHER_MAX_KEYPOINTS_INPUT_14_, GATHER_MAX_KEYPOINTS_INPUT_15_,
                     GATHER_MAX_KEYPOINTS_INPUT_16_, GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, input13,
        input14, input15, input16, output_sw);
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_);
    printf("HwGatherFeatures16: %d\n", res);

    //
    delete[] input1;
    delete[] input2;
    delete[] input3;
    delete[] input4;
    delete[] input5;
    delete[] input6;
    delete[] input7;
    delete[] input8;
    delete[] input9;
    delete[] input10;
    delete[] input11;
    delete[] input12;
    delete[] input13;
    delete[] input14;
    delete[] input15;
    delete[] input16;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void cosim_gather_kp_12() {
    vx_image_data<KeyPoint, 1> *input1 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_1_];
    vx_image_data<KeyPoint, 1> *input2 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_2_];
    vx_image_data<KeyPoint, 1> *input3 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_3_];
    vx_image_data<KeyPoint, 1> *input4 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_4_];
    vx_image_data<KeyPoint, 1> *input5 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_5_];
    vx_image_data<KeyPoint, 1> *input6 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_6_];
    vx_image_data<KeyPoint, 1> *input7 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_7_];
    vx_image_data<KeyPoint, 1> *input8 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_8_];
    vx_image_data<KeyPoint, 1> *input9 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_9_];
    vx_image_data<KeyPoint, 1> *input10 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_10_];
    vx_image_data<KeyPoint, 1> *input11 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_11_];
    vx_image_data<KeyPoint, 1> *input12 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_12_];
    vx_image_data<KeyPoint, 1> *output_sw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_];
    vx_image_data<KeyPoint, 1> *output_hw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_];

#ifndef __SYNTHESIS__
    vx_uint64 keypoint_sum = createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_1_>(input1, 0);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_2_>(input2, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_3_>(input3, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_4_>(input4, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_5_>(input5, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_6_>(input6, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_7_>(input7, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_8_>(input8, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_9_>(input9, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_10_>(input10, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_11_>(input11, keypoint_sum);
    createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_12_>(input12, keypoint_sum);
#endif

    // test hardware against software
    TestGatherKeypointsInput12(input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11,
                               input12, output_hw);
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_, GATHER_MAX_KEYPOINTS_INPUT_9_,
                     GATHER_MAX_KEYPOINTS_INPUT_10_, GATHER_MAX_KEYPOINTS_INPUT_11_, GATHER_MAX_KEYPOINTS_INPUT_12_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, input9, input10, input11, input12, output_sw);
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_);
    printf("HwGatherFeatures12: %d\n", res);

    //
    delete[] input1;
    delete[] input2;
    delete[] input3;
    delete[] input4;
    delete[] input5;
    delete[] input6;
    delete[] input7;
    delete[] input8;
    delete[] input9;
    delete[] input10;
    delete[] input11;
    delete[] input12;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void cosim_gather_kp_8() {
    vx_image_data<KeyPoint, 1> *input1 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_1_];
    vx_image_data<KeyPoint, 1> *input2 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_2_];
    vx_image_data<KeyPoint, 1> *input3 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_3_];
    vx_image_data<KeyPoint, 1> *input4 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_4_];
    vx_image_data<KeyPoint, 1> *input5 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_5_];
    vx_image_data<KeyPoint, 1> *input6 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_6_];
    vx_image_data<KeyPoint, 1> *input7 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_7_];
    vx_image_data<KeyPoint, 1> *input8 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_8_];
    vx_image_data<KeyPoint, 1> *output_sw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_];
    vx_image_data<KeyPoint, 1> *output_hw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_];

#ifndef __SYNTHESIS__
    vx_uint64 keypoint_sum = createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_1_>(input1, 0);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_2_>(input2, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_3_>(input3, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_4_>(input4, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_5_>(input5, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_6_>(input6, keypoint_sum);
    keypoint_sum += createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_7_>(input7, keypoint_sum);
    createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_8_>(input8, keypoint_sum);
#endif

    // test hardware against software
    TestGatherKeypointsInput8(input1, input2, input3, input4, input5, input6, input7, input8, output_hw);
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_, GATHER_MAX_KEYPOINTS_INPUT_3_,
                     GATHER_MAX_KEYPOINTS_INPUT_4_, GATHER_MAX_KEYPOINTS_INPUT_5_, GATHER_MAX_KEYPOINTS_INPUT_6_,
                     GATHER_MAX_KEYPOINTS_INPUT_7_, GATHER_MAX_KEYPOINTS_INPUT_8_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_, GATHER_SCATTER_MODE_>(
        input1, input2, input3, input4, input5, input6, input7, input8, output_sw);
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * GATHER_MAX_KEYPOINTS_INPUT_8_);
    printf("HwGatherFeatures8: %d\n", res);

    //
    delete[] input1;
    delete[] input2;
    delete[] input3;
    delete[] input4;
    delete[] input5;
    delete[] input6;
    delete[] input7;
    delete[] input8;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void cosim_gather_kp_2() {
    vx_image_data<KeyPoint, 1> *input1 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_1_];
    vx_image_data<KeyPoint, 1> *input2 = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_INPUT_2_];
    vx_image_data<KeyPoint, 1> *output_sw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_];
    vx_image_data<KeyPoint, 1> *output_hw = new vx_image_data<KeyPoint, 1>[GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_];

#ifndef __SYNTHESIS__
    vx_uint64 keypoint_sum = createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_1_>(input1, 0);
    createTestKeypoints<GATHER_MAX_KEYPOINTS_INPUT_2_>(input2, keypoint_sum);
#endif

    // test hardware against software
    TestHwGatherFeatures2(input1, input2, output_hw);
    ImgFeatureGather<GATHER_MAX_KEYPOINTS_INPUT_1_, GATHER_MAX_KEYPOINTS_INPUT_2_,
                     GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_, GATHER_SCATTER_MODE_>(input1, input2, output_sw);
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_);
    printf("HwGatherFeatures2: %d\n", res);

    //
    delete[] input1;
    delete[] input2;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
int cosim_fe_with_sr() {
    int res = 0;
    typedef typename next_larger_unsigned<feT>::DataType modT;
    const modT mod = static_cast<to_unsigned<feT>::DataType>(-1) + static_cast<modT>(1);
    feT *input = new feT[NR_PIXELS_];
    KeyPoint *output_sw = new KeyPoint[FE_VEC_NUM_OUT_ * FE_MAX_KEYPOINTS_OUTPUT_];
    KeyPoint *output_hw = new KeyPoint[FE_VEC_NUM_OUT_ * FE_MAX_KEYPOINTS_OUTPUT_];

    // Create random data
    for (vx_uint32 i = 0; i < NR_PIXELS_; i++) {
        const feT p = rand() % mod;
        input[i] = p;
    }

    // test function in HW
    TestImgFeatureExtraction((fe_imageT *)input, (keypoint_imageT *)output_hw);
    // test function in SW
    ImgFeatureExtraction<feT, vx_uint8, VEC_NUM_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_, 0, FE_THRESHOLD_,
                         FE_MAX_KEYPOINTS_OUTPUT_, (1 << 16), FE_BORDER_WIDTH_, 0, HIFLIPVX::SQUARE, FE_WITH_SR_>(
        input, output_sw);

    // compare results
    res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * FE_MAX_KEYPOINTS_OUTPUT_ * FE_VEC_NUM_OUT_);
    printf("ImgFeatureExtraction: %d\n", res);

    delete[] input;
    delete[] output_hw;
    delete[] output_sw;

    return res;
}

/**********************************************************************************************************************/
void cosim_deserialization() {
    des_inT *input = new des_inT[DES_MAX_KEYPOINTS_INPUT_];
    des_outT *output_sw = new des_outT[DES_MAX_KEYPOINTS_OUTPUT_];
    des_outT *output_hw = new des_outT[DES_MAX_KEYPOINTS_OUTPUT_];
    vx_uint16 nr_kps = 0;
    const vx_uint8 EVOL_LEVEL = 1;
    KeyPointConverter kp_converter_invalid_kp;
    kp_converter_invalid_kp.data = INVALID_KEYPOINT_DATA;

#ifndef __SYNTHESIS__
    test_compare_keypoints_create_keypoints<VEC_NUM_, DES_VEC_NUM_IN_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_, EVOL_LEVEL,
                                            NSUBLEVELS_, FE_BORDER_WIDTH_, FE_THRESHOLD_, DES_MAX_KEYPOINTS_INPUT_,
                                            FE_WITH_SR_>(input, NULL, nr_kps);
#endif

    // test hardware against software
    ImgFeatureDeserialize<KeyPoint, DES_VEC_NUM_IN_, DES_MAX_KEYPOINTS_INPUT_, DES_MAX_KEYPOINTS_OUTPUT_>(
        input, output_sw, kp_converter_invalid_kp.keypoint, ImgTestDeserializationCompareDeserilizationKeypoint);
    TestHwFeatureDeserialization(input, output_hw);
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * nr_kps);
    printf("ImgFeatureDeserialize: %d\n", res);

    //
    delete[] input;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void cosim_comp_kp_evol_one() {
    const vx_uint8 EVOL_LEVEL = 1;

    cmp_kp_inT *current = new vx_image_data<KeyPoint, CMP_VEC_NUM_IN_>[CMP_MAX_KEYPOINTS_CURRENT_];
    cmp_kp_inT *lower = new vx_image_data<KeyPoint, CMP_VEC_NUM_IN_>[CMP_MAX_KEYPOINTS_PREVIOUS_];
    cmp_kp_outT *output_sw = new vx_image_data<KeyPoint, 1>[CMP_MAX_KEYPOINTS_OUTPUT_];
    cmp_kp_outT *output_hw = new vx_image_data<KeyPoint, 1>[CMP_MAX_KEYPOINTS_OUTPUT_];
    vx_uint16 nr_kps = 0;

#ifndef __SYNTHESIS__
    test_compare_keypoints_create_keypoints<VEC_NUM_, CMP_VEC_NUM_IN_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_, EVOL_LEVEL,
                                            NSUBLEVELS_, FE_BORDER_WIDTH_, FE_THRESHOLD_, CMP_MAX_KEYPOINTS_CURRENT_,
                                            FE_WITH_SR_>(current, NULL, nr_kps);
    test_compare_keypoints_create_keypoints<VEC_NUM_, CMP_VEC_NUM_IN_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_,
                                            EVOL_LEVEL - 1, NSUBLEVELS_, FE_BORDER_WIDTH_, FE_THRESHOLD_,
                                            CMP_MAX_KEYPOINTS_PREVIOUS_, FE_WITH_SR_>(lower, NULL, nr_kps);
#endif

    // test hardware against software
    TestHwFeatureComparisonEvolOne(current, lower, output_hw);
    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_,
                         CMP_MAX_KEYPOINTS_PREVIOUS_, 0, CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_1_, 1,
                         FHD_BOUNDARY_LEVEL_1_, CMP_KERN_SIZE_, CMP_PARALLELIZATION_,
                         CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(current, lower, NULL, output_sw);
#ifndef __SYNTHESIS__
    nr_kps = count_keypoints<CMP_MAX_KEYPOINTS_OUTPUT_>(output_hw);
#endif
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * nr_kps);
    printf("ImgFeatureComparison: %d\n", res);

    //
    delete[] current;
    delete[] lower;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void cosim_comp_kp_evol_zero() {
    const vx_uint8 EVOL_LEVEL = 0;
    cmp_kp_inT *current = new vx_image_data<KeyPoint, CMP_VEC_NUM_IN_>[CMP_MAX_KEYPOINTS_CURRENT_];
    cmp_kp_outT *output_sw = new vx_image_data<KeyPoint, 1>[CMP_MAX_KEYPOINTS_OUTPUT_];
    cmp_kp_outT *output_hw = new vx_image_data<KeyPoint, 1>[CMP_MAX_KEYPOINTS_OUTPUT_];
    vx_uint16 nr_kps = 0;

#ifndef __SYNTHESIS__
    test_compare_keypoints_create_keypoints<VEC_NUM_, CMP_VEC_NUM_IN_, IMG_COLS_, IMG_ROWS_, FE_KERN_SIZE_, EVOL_LEVEL,
                                            NSUBLEVELS_, FE_BORDER_WIDTH_, FE_THRESHOLD_, CMP_MAX_KEYPOINTS_CURRENT_,
                                            FE_WITH_SR_>(current, NULL, nr_kps);
#endif

    // test hardware against software
    TestHwFeatureComparisonEvolZero(current, output_hw);
    ImgFeatureComparison<CMP_VEC_NUM_IN_, IMG_COLS_, IMG_COLS_, IMG_ROWS_, CMP_MAX_KEYPOINTS_CURRENT_, 0, 0,
                         CMP_MAX_KEYPOINTS_OUTPUT_, FHD_RADIUS_LEVEL_0_, 0, FHD_BOUNDARY_LEVEL_0_, CMP_KERN_SIZE_,
                         CMP_PARALLELIZATION_, CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_, CMP_MAX_BUFFER_SIZE_>(
        current, NULL, NULL, output_sw);
#ifndef __SYNTHESIS__
    nr_kps = count_keypoints<CMP_MAX_KEYPOINTS_OUTPUT_>(output_hw);
#endif
    vx_int32 res = memcmp(output_hw, output_sw, sizeof(KeyPoint) * nr_kps);
    printf("ImgFeatureComparison: %d\n", res);

    //
    delete[] current;
    delete[] output_sw;
    delete[] output_hw;
}

/**********************************************************************************************************************/
void TestImgMtFeature() {

    //
    const bool verify_compare = true;
    const bool verify_deserialize = true;
    const bool verify_extract = true;
    const bool verify_gather = true;

    // FEATURE COMPARE (TODO: COMPILE ERROR)
    if (verify_compare) {
        cosim_comp_kp_evol_zero();
        cosim_comp_kp_evol_one();
        sw_test_compare_keypoints();
    }

    // FEATURE DESERIALIZE
    if (verify_deserialize) {
        cosim_deserialization();
        sw_test_deserialization();
    }

    // FEATURE EXTRACT (TODO: SUBPIXELREFINEMENT)
    if (verify_extract) {
        cosim_fe_with_sr();
        // sw_test_fe_sr<vx_true_e>();
        sw_test_fe_sr<vx_false_e>();
    }

    // FEATURE GATHER
    if (verify_gather) {
        cosim_gather_kp_2();
        cosim_gather_kp_8();
        cosim_gather_kp_12();
        cosim_gather_kp_16();
        sw_test_gather();
    }
}

/**********************************************************************************************************************/
//
#undef IMG_COLS_ 1920
#undef IMG_ROWS_ 1080
#undef MAX_SIDE_LENGTH_ 1920
#undef NR_PIXELS_(IMG_COLS_ *IMG_ROWS_)
#undef VEC_NUM_ 1
#undef VEC_PIXELS_(NR_PIXELS_ / VEC_NUM_)

//
#undef NSUBLEVELS_ 4

// values for level 0-3 are the same for 4-7 and 8-11
#undef FHD_RADIUS_LEVEL_0_ 147456
#undef FHD_BOUNDARY_LEVEL_0_ 47
#undef FHD_RADIUS_LEVEL_1_ 175355
#undef FHD_BOUNDARY_LEVEL_1_ 57
#undef FHD_RADIUS_LEVEL_2_ 208534
#undef FHD_BOUNDARY_LEVEL_2_ 67
#undef FHD_RADIUS_LEVEL_3_ 247990
#undef FHD_BOUNDARY_LEVEL_3_ 78
#undef FHD_RADIUS_LEVEL_8_ FHD_RADIUS_LEVEL_0_
#undef FHD_BOUNDARY_LEVEL_8_ FHD_BOUNDARY_LEVEL_0_
#undef FHD_RADIUS_LEVEL_9_ FHD_RADIUS_LEVEL_1_
#undef FHD_BOUNDARY_LEVEL_9_ FHD_BOUNDARY_LEVEL_1_
#undef FHD_RADIUS_LEVEL_10_ FHD_RADIUS_LEVEL_2_
#undef FHD_BOUNDARY_LEVEL_10_ FHD_BOUNDARY_LEVEL_2_
#undef FHD_RADIUS_LEVEL_11_ FHD_RADIUS_LEVEL_3_
#undef FHD_BOUNDARY_LEVEL_11_ FHD_BOUNDARY_LEVEL_3_

//
#undef GATHER_SCATTER_MODE_ HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC
#undef GATHER_MAX_KEYPOINTS_INPUT_1_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_2_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_3_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_4_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_5_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_6_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_7_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_8_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_9_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_10_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_11_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_12_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_13_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_14_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_15_ 16384
#undef GATHER_MAX_KEYPOINTS_INPUT_16_ 16384
#undef GATHER_MAX_KEYPOINTS_OUTPUT_TWO_INPUTS_ 16384
#undef GATHER_MAX_KEYPOINTS_OUTPUT_EIGHT_INPUTS_ 16384
#undef GATHER_MAX_KEYPOINTS_OUTPUT_TWELVE_INPUTS_ 16384
#undef GATHER_MAX_KEYPOINTS_OUTPUT_SIXTEEN_INPUTS_ 16384

//
#undef FE_THRESHOLD_ 32
#undef FE_LEVEL_ 1
#undef FE_BORDER_WIDTH_ 1
#undef FE_KERN_SIZE_ 3
#undef FE_VEC_NUM_OUT_(FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_, FE_KERN_SIZE_))
#undef FE_MAX_KEYPOINTS_OUTPUT_ 16384
#undef FE_WITH_SR_ vx_false_e

//
#undef DES_VEC_NUM_IN_ FE_VEC_NUM_OUT_
#undef DES_MAX_KEYPOINTS_INPUT_ 16384
#undef DES_MAX_KEYPOINTS_OUTPUT_ 16384

//
#undef CMP_VEC_NUM_IN_ FE_VEC_NUM_OUT_
#undef CMP_MAX_KEYPOINTS_CURRENT_ 16384
#undef CMP_MAX_KEYPOINTS_PREVIOUS_ 16384
#undef CMP_MAX_KEYPOINTS_OUTPUT_ 16384
#undef CMP_KERN_SIZE_ FE_KERN_SIZE_
#undef CMP_RESTRICT_MAX_NUMBER_OF_ITERATIONS_ 0
#undef CMP_PARALLELIZATION_ 1
#undef CMP_MAX_BUFFER_SIZE_

#endif /* __SRC_IMG_MT_FEATURE_TEST_H__ */
