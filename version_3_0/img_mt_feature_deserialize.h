/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_deserialize.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  Functions needed for the feature deserialize function.
 */

#ifndef __SRC_IMG_MT_DESERIALIZE_H__
#define __SRC_IMG_MT_DESERIALIZE_H__

#include "vx_helper.h"

/**********************************************************************************************************************/
/**
 * @brief Takes as an input a vector and writtes them out in a serial manner. All invalid elements are discarded in this
 * process. If the maximum number of output values is reached than is a invalid output written to signal the end of the
 * transaction. If all entries of the input vector are invalid then stops this function before the maximum number of
 * input vectors are reached.
 * @tparam DataType Data type of the input vector
 * @tparam VEC_NUM_IN Input vector size. Which is transformed to an output vector size of one.
 * @tparam NUMBER_OF_INPUT_VECTORS Maximum number of input vectors.
 * @tparam NUMBER_OF_OUTPUT_VALUES Maximum number of outputs.
 * @param input Input of vectors which shall be transformed to output vectors of size one.
 * @param output Created output vectors of size one.
 * @param invalid_element Invalid element which is used to dermine a vector entry is valid or not. Also is this value
 * used to create the last output element to signal the end of the transaction.
 * @param valid_vec_elem_fct Compare function which uses the invalid element to determine if a vector entry is valid or
 * not.
 */
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint32 NUMBER_OF_INPUT_VECTORS, vx_uint32 NUMBER_OF_OUTPUT_VALUES>
void HwDeserialization(vx_image_data<DataType, VEC_NUM_IN> input[NUMBER_OF_INPUT_VECTORS],
                       vx_image_data<DataType, 1> output[NUMBER_OF_OUTPUT_VALUES], const DataType invalid_element,
                       vx_bool (*valid_vec_elem_fct)(const DataType, const DataType)) {
#pragma HLS INLINE

    vx_bool read_next_input = vx_true_e;
    vx_bool valid_vector = vx_false_e;
    vx_bool eof = vx_false_e;
    vx_uint32 ptr_dst = 0;

    vx_image_data<DataType, VEC_NUM_IN> input_data;
//#pragma HLS array_partition variable = input_data complete dim = 0

    vx_image_data<DataType, 1> output_data;

    for (vx_uint32 i = 0; i < NUMBER_OF_INPUT_VECTORS; i++) {
        for (vx_uint8 j = 0; j < VEC_NUM_IN; j++) {
#pragma HLS PIPELINE II = 1

            if ((read_next_input) && (j == 0)) {
                input_data = input[i];
                valid_vector = vx_false_e;
                for (vx_uint8 k = 0; k < VEC_NUM_IN; k++) {
#pragma HLS unroll
                    const DataType element = input_data.pixel[k];
                    valid_vector |= valid_vec_elem_fct(element, invalid_element);
                }
                read_next_input = valid_vector;
            }

            // write valid vector out, ignore/discard invalid vector elements
            if ((!valid_vector) && (!eof)) {
                eof = vx_true_e;
                output_data.pixel[0] = invalid_element;
                // In each loop output_data is initialize with invalid keypoints.
                GenerateDmaSignal<DataType, 1>((ptr_dst == 0), vx_true_e, output_data);
                output[ptr_dst] = output_data;
                ++ptr_dst;
            } else if (!eof) {

                const DataType element = input_data.pixel[j];

                if (valid_vec_elem_fct(element, invalid_element)) {
                    eof = (ptr_dst == NUMBER_OF_OUTPUT_VALUES - 1);
                    output_data.pixel[0] = element;
                    GenerateDmaSignal<DataType, 1>((ptr_dst == 0), eof, output_data);
                    output[ptr_dst] = output_data;
                    ++ptr_dst;
                }
            }
        }
    }
}

vx_bool HwFeatureDeserializationCompareKeypoint(const KeyPoint lhs, const KeyPoint rhs) {
#pragma HLS INLINE
    return (lhs.scale != rhs.scale);
}

#endif /* __SRC_IMG_MT_DESERIALIZE_H__ */
