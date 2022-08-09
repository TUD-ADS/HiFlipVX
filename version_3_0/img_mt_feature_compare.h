/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_compare.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  Functions needed for the feature compare function.
 */

#ifndef __SRC_IMG_MT_FEATURE_COMPARE_H__
#define __SRC_IMG_MT_FEATURE_COMPARE_H__

#include "img_mt_helper.h"

/**********************************************************************************************************************/
template <typename DataType, vx_uint8 VEC_NUM>
vx_bool CompareIsValidVector(DataType vec[VEC_NUM], vx_bool (*is_invalid_element)(const DataType)) {
#pragma HLS INLINE

    vx_bool valid = vx_false_e;

    for (vx_uint8 i = 0; i < VEC_NUM; i++) {
#pragma HLS unroll
        valid |= (!is_invalid_element(vec[i]));
    }

    return valid;
}

template <typename DataType, vx_uint16 PARTITION_SIZE, vx_uint8 NUMBER_OF_PREFETCH_REGISTER_ELEMENTS>
DataType CompareReadFromBuffer(vx_ra_ring_buffer<DataType, PARTITION_SIZE> &buffer,
                               vx_ra_shift_register<DataType, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS> &prefetch_register,
                               const DataType input_element, const DataType cmp_element, const vx_uint16 buf_read_index,
                               const vx_bool push_input_element, const vx_bool push_invalid_element,
                               const vx_bool push_back_to_register, const DataType invalid_element) {
#pragma HLS INLINE
    /*
     * Preload a keypoint from the buffer or the an invalid keypoint (invalid keypoint is at the end replaced by the
     * input) depending on the iteration / elements on the prefetch shift register.
     */
    DataType preloaded_element;
    if (push_invalid_element) {
        preloaded_element = invalid_element;
    } else if (push_input_element) {
        preloaded_element = input_element;
    } else if (push_back_to_register) {
        preloaded_element = cmp_element;
    } else {
        preloaded_element = buffer[buf_read_index];
    }

    // push next element to the prefetch shift register and take popped element from the shift register for comparision
    return prefetch_register.shift_from_lowest_to_highest(preloaded_element);
}

template <typename DataType, vx_uint16 PARTITION_SIZE>
vx_bool CompareWriteToBuffer(vx_ra_ring_buffer<DataType, PARTITION_SIZE> &buffer, DataType &removed_buffer_element,
                             DataType &buf_element, const vx_uint16 buf_write_index, const vx_bool is_push_partition,
                             const vx_bool invalidate_buffer_element, const vx_bool invalidate_last_cycle_input_element,
                             const vx_bool push_to_buffer, vx_bool (*is_ignore_element)(const DataType),
                             vx_bool (*is_invalid_element)(const DataType), void (*invalidate_element)(DataType &)) {
#pragma HLS INLINE

    vx_bool write_buffer_element_out = vx_false_e;

    // write keypoint removed from the prefetch cache to the buffer
    if (!is_ignore_element(buf_element)) {
        if (is_push_partition && push_to_buffer) {
            const vx_bool is_full = buffer.is_full();

            // invalidate the input keypoint if necessary
            if (invalidate_last_cycle_input_element || invalidate_buffer_element) {
                invalidate_element(buf_element);
            }
            // if this is the first iterations of the comparison cycle push the new elements to the buffer...
            removed_buffer_element = buffer.push(buf_element);

            // write popped keypoint only out if the buffer was full before pushing the new keypoint
            write_buffer_element_out = (is_full && !is_invalid_element(removed_buffer_element));
        } else {

            if (invalidate_buffer_element) {
                invalidate_element(buf_element);
            }

            buffer[buf_write_index] = buf_element;
        }
    }

    return write_buffer_element_out;
}

template <vx_uint8 VEC_NUM, vx_uint8 NUMBER_OF_PREFETCH_REGISTER_ELEMENTS, vx_uint8 INVALID_SCALE>
void CompareKeypointsPushToPrefetchRegister(
    vx_ra_shift_register<KeyPoint, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS> &prefetch_register,
    const KeyPoint input_keypoints[VEC_NUM], const vx_bool invalidate_input_keypoints[VEC_NUM],
    const KeyPoint cmp_keypoint, const vx_uint16 buf_full_sectors, const vx_uint16 cmp_iter,
    const vx_bool is_selected_partition) {
#pragma HLS INLINE

    /*
     * Update the keypoint also int the prefetch register until the buffer contains at least as much element as the
     * prefetch register can store, afterwards the updated keypoint from the buffer can be directly loaded into the
     * prefetch buffer without any delay concerns.
     */
    if (buf_full_sectors < NUMBER_OF_PREFETCH_REGISTER_ELEMENTS) {
        prefetch_register[NUMBER_OF_PREFETCH_REGISTER_ELEMENTS - buf_full_sectors - 1] = cmp_keypoint;
    }
}

template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint16 MAX_INPUTS>
void CompareReadInput(DataType input_vector[VEC_NUM_IN], vx_uint16 &ptr_input, vx_bool &is_active,
                      const vx_image_data<DataType, VEC_NUM_IN> input[(MAX_INPUTS > 0) ? (MAX_INPUTS) : (1)],
                      const vx_bool read_input, vx_bool (*is_invalid_element)(const DataType)) {
#pragma HLS INLINE
    // check if maximum number of input keypoints for this input have been reached
    is_active &= (ptr_input < MAX_INPUTS);

    // read next input and increase pointer
    if (CompareIsValidVector<DataType, VEC_NUM_IN>(input_vector, is_invalid_element) && (ptr_input < MAX_INPUTS) &&
        read_input) {
        readImageData<DataType, VEC_NUM_IN>(input_vector, input[ptr_input]);
        ++ptr_input;
    }
    // check if input vector contains at least one valid keypoint to be processed or if all keypoints are invalid
    // meaning there are no new keypoints to be read from this input
    is_active &= CompareIsValidVector<DataType, VEC_NUM_IN>(input_vector, is_invalid_element);
}

template <typename DataType, vx_uint8 VEC_NUM_IN>
vx_bool CompareUpdate(vx_bool &invalidate_buf_element, vx_bool invalidate_input_elements[VEC_NUM_IN],
                      const KeyPoint buf_element, const KeyPoint input_vector[VEC_NUM_IN],
                      vx_bool (*is_ignore_element)(const DataType),
                      vx_bool (*update_fct)(vx_bool &, vx_bool &, const DataType, const DataType)) {
#pragma HLS INLINE

    const vx_bool skip_buf_element = is_ignore_element(buf_element);
    vx_bool stop_condition = vx_false_e;
    invalidate_buf_element = vx_false_e;

    // compare all input keypoints in parallel
    for (vx_uint8 i = 0; i < VEC_NUM_IN; i++) {
#pragma HLS unroll

        const DataType input_element = input_vector[i];
        const vx_bool skip_input_element = is_ignore_element(input_element);

        // only compare if the input as well as the buffer element is valid else ignore
        if (!skip_buf_element && !skip_input_element) {

            vx_bool tmp_invalidate_buf_element;
            vx_bool tmp_invalidate_input_element;

            stop_condition |=
                update_fct(tmp_invalidate_buf_element, tmp_invalidate_input_element, buf_element, input_element);

            invalidate_buf_element |= tmp_invalidate_buf_element;
            invalidate_input_elements[i] = tmp_invalidate_input_element;

        } else {
            invalidate_input_elements[i] = vx_false_e;
        }
    }

    return stop_condition;
}

/**
 * @brief
 * @tparam VEC_NUM
 * @tparam BUFFER_SIZE
 * @tparam NUMBER_OF_PREFETCH_REGISTER_ELEMENTS
 * @tparam RADIUS_SQUARE
 * @tparam INVALID_SCALE
 * @tparam IGNORE_SCALE
 * @tparam COMPARE_PARALLELIZATION_LEVEL
 * @param buffer
 * @param prefetch_register
 * @param input_keypoints
 * @param rem_keypoint
 * @param push_iter_counter
 * @param outside_y_radius
 * @param invalidate_input_keypoints
 * @param buf_fill_level
 * @param buf_fp_write_index
 * @param buf_fp_read_index
 * @param cmp_iter
 * @param selected_first_partition
 * @return
 */
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint16 PARTITION_SIZE,
          vx_uint8 NUMBER_OF_PREFETCH_REGISTER_ELEMENTS, vx_uint8 COMPARE_PARALLELIZATION_LEVEL>
vx_bool CompareBufferHandling(
    vx_ra_ring_buffer<DataType, PARTITION_SIZE> buffer[COMPARE_PARALLELIZATION_LEVEL],
    vx_ra_shift_register<DataType, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS>
        prefetch_register[COMPARE_PARALLELIZATION_LEVEL],
    DataType input_elements[VEC_NUM_IN], vx_ping_pong<DataType, COMPARE_PARALLELIZATION_LEVEL> &buffer_elements,
    DataType &removed_element, vx_ping_pong<vx_bool, 1> &stop_signal,
    vx_ping_pong<vx_bool, VEC_NUM_IN> &invalidate_input_elements,
    vx_ping_pong<vx_bool, COMPARE_PARALLELIZATION_LEVEL> &invalidate_buffer_elements, const vx_uint16 buf_fill_level,
    const vx_uint16 buf_fp_write_index, const vx_uint16 buf_fp_read_index, const vx_uint16 cmp_iter,
    const vx_uint8 selected_first_partition, const vx_bool buf_new_sector, const vx_bool is_active,
    const vx_bool push_to_buffer, vx_bool (*update_function)(vx_bool &, vx_bool &, const DataType, const DataType),
    vx_bool (*is_ignore_element)(const DataType), vx_bool (*is_invalid_element)(const DataType),
    void (*invalidate_element)(DataType &), const DataType invalid_element) {
#pragma HLS INLINE

    // number of sectors that get a new keypoint in each new cycle, e.g. VEC_NUM = 4 and COMPARE_PARALLELIZATION_LEVEL =
    // 2, results in 2 sections reciving new keypoints in the first two iterations
    const vx_uint8 NUMBER_OF_INPUT_SECTIONS = MAX((VEC_NUM_IN / COMPARE_PARALLELIZATION_LEVEL), 1);
    const vx_uint8 NUMBER_OF_INPUTS_PER_SECTION =
        (VEC_NUM_IN > COMPARE_PARALLELIZATION_LEVEL) ? (COMPARE_PARALLELIZATION_LEVEL) : (VEC_NUM_IN);
    // determines the index of the first partition which received a new keypoint in the previous comparison cycle to
    // determine if the keypoint in the register is pushed to the buffer or just an update to the an existing one
    const vx_uint8 prev_selected_first_partition = (selected_first_partition > NUMBER_OF_INPUTS_PER_SECTION - 1)
                                                       ? (selected_first_partition - NUMBER_OF_INPUTS_PER_SECTION)
                                                       : (COMPARE_PARALLELIZATION_LEVEL - NUMBER_OF_INPUTS_PER_SECTION);
    // check if in the current iteration an input keypoint is written to the prefetch register
    const vx_bool push_input_kp_iter =
        (cmp_iter >= (NUMBER_OF_PREFETCH_REGISTER_ELEMENTS - NUMBER_OF_INPUT_SECTIONS)) &&
        (cmp_iter < NUMBER_OF_PREFETCH_REGISTER_ELEMENTS);
    // apply an offset which compensate in the case that a section is not full yet and the therefore the read for the
    // oldest written keypoints and the newest intersect
    const vx_bool read_offset =
        ((buf_fill_level == PARTITION_SIZE) && (!buf_new_sector) && (cmp_iter < NUMBER_OF_PREFETCH_REGISTER_ELEMENTS));
    // in the same scenario as for read_offset push invalid keypoints for the entries were the newest are in the
    // iteration were the oldest keypoints are pushed to prefetech buffer
    const vx_bool read_upper_section_only =
        (buf_fill_level == PARTITION_SIZE) && ((!buf_new_sector) && (cmp_iter == NUMBER_OF_PREFETCH_REGISTER_ELEMENTS));
    // push keypoint back to the register if the number of elements in the buffer is less than the prefetch register can
    // hold
    const vx_bool push_back_to_register = (buf_fill_level < NUMBER_OF_PREFETCH_REGISTER_ELEMENTS + 1);

    vx_bool write_element_out = vx_false_e;
    vx_bool tmp_stop_signal = vx_false_e;

    vx_bool invalidate_input_array[COMPARE_PARALLELIZATION_LEVEL][VEC_NUM_IN];
#pragma HLS array_partition variable = invalidate_input_array complete dim = 0

    for (vx_uint8 i = 0; i < COMPARE_PARALLELIZATION_LEVEL; i++) {
#pragma HLS unroll

        // IN BOTH CASES: if the vector size is larger than the parallelization level all partitions write a new input
        // keypoint to the buffer at the beginning of each comparision cycle determines if the current partition is a
        // partition which receives at the end of the comparision cycle the input keypoint or not
        const vx_bool is_selected_partition =
            ((i >= selected_first_partition) && (i < selected_first_partition + NUMBER_OF_INPUTS_PER_SECTION));
        // determine if the current partition contains a keypoint which shall be pushed to the buffer in the first
        // iteration(s) of the current comparision cycle or not
        const vx_bool is_push_partition =
            (VEC_NUM_IN >= COMPARE_PARALLELIZATION_LEVEL) ||
            ((i >= prev_selected_first_partition) && (i < prev_selected_first_partition + VEC_NUM_IN));
        // determine the write and write index depending on the selected partition (no check for out-of-index required,
        // because the buffer size is always a multiple of the parallelization degree)
        const vx_uint16 buf_fp_virt_read_index = buf_fp_read_index + read_offset;
        const vx_uint16 buf_read_index = (buf_fp_virt_read_index < PARTITION_SIZE) ? (buf_fp_virt_read_index) : (0);
        // push place holder to the prefetech register and skip reading from the the buffer depending on the prefetch
        // register size and the input vector size
        const vx_bool push_input_keypoint = (is_selected_partition && push_input_kp_iter);
        const vx_bool push_invalid_keypoint = ((push_input_kp_iter && (i > selected_first_partition)) ||
                                               (read_upper_section_only && i <= selected_first_partition));

        DataType buffer_write_element = buffer_elements.read_from(i);
        write_element_out |= CompareWriteToBuffer<DataType, PARTITION_SIZE>(
            buffer[i], removed_element, buffer_write_element, buf_fp_write_index, is_push_partition,
            invalidate_buffer_elements.read_from(i), invalidate_input_elements.read_from(i % VEC_NUM_IN),
            push_to_buffer, is_ignore_element, is_invalid_element, invalidate_element);
        const DataType buffer_read_element =
            CompareReadFromBuffer<DataType, PARTITION_SIZE, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS>(
                buffer[i], prefetch_register[i], input_elements[0], buffer_write_element, buf_read_index,
                push_input_keypoint, push_invalid_keypoint, push_back_to_register, invalid_element);
        buffer_elements.write_to(i, buffer_read_element);
        if (is_active) {
            vx_bool buf_elem_in;
            tmp_stop_signal |=
                CompareUpdate<DataType, VEC_NUM_IN>(buf_elem_in, invalidate_input_array[i], buffer_read_element,
                                                    input_elements, is_ignore_element, update_function);
            invalidate_buffer_elements.write_to(i, buf_elem_in);
        }
    }

    for (vx_uint8 i = 0; i < VEC_NUM_IN; i++) {
#pragma HSL unroll
        for (vx_uint8 j = 0; j < COMPARE_PARALLELIZATION_LEVEL; j++) {
#pragma HLS unroll
            invalidate_input_elements.get_write_buffer()[i] |= invalidate_input_array[j][i];
        }
    }

    stop_signal.write_to(0, tmp_stop_signal);

    return write_element_out;
}

template <typename DataType, vx_uint8 VEC_NUM_IN>
void CompareVectorEntries(DataType input_elements[VEC_NUM_IN], vx_bool invalidate_input_elements[VEC_NUM_IN],
                          vx_bool (*check_input_element)(const DataType),
                          void (*input_compare_fct)(vx_bool &, vx_bool &, const DataType, const DataType),
                          vx_bool (*is_invalid_input_element)(const DataType)) {
#pragma HLS INLINE

    /*
     * Iterations that are not meet the conditions should be optimized out. Example for VEC_NUM = 4 (all iterations in
     * brackets should be optimized out): (0) 1	 2	3 (0)(1) 2	3 (0)(1)(2) 3
     */
    for (vx_uint8 i = 0, ownId = 0; i < VEC_NUM_IN * VEC_NUM_IN; i++) {
#pragma HLS unroll

        const vx_uint8 cmpId = i % VEC_NUM_IN;

        // check each input element if its valid
        for (vx_uint8 i = 0; i < VEC_NUM_IN; i++) {
#pragma HLS unroll
            const DataType element = input_elements[i];
            // only allow to update an element if its a valid element else ignore
            const vx_bool valid_element = !is_invalid_input_element(element);
            // check the paraemeter of the input element cause an invalidation of it
            const vx_bool invalidate_element = check_input_element(element);
            // initilialize the invalidation register depending on the validation of the input
            invalidate_input_elements[i] = (invalidate_element && valid_element);
        }

        // compare only against elements with a higher id
        if (cmpId > ownId) {

            const KeyPoint own_element = input_elements[ownId];
            const KeyPoint cmp_element = input_elements[cmpId];
            // update the invalidation register only if both elements are valid inputs
            const vx_bool own_is_valid = !is_invalid_input_element(own_element);
            const vx_bool cmp_is_valid = !is_invalid_input_element(cmp_element);
            // store the result of the comparision
            vx_bool invalidate_own_element;
            vx_bool invalidate_cmp_element;
            // compare input elements agianst one another
            input_compare_fct(invalidate_own_element, invalidate_cmp_element, own_element, cmp_element);
            // update the invalidation register
            if (own_is_valid && cmp_is_valid) {
                invalidate_input_elements[ownId] |= invalidate_own_element;
                invalidate_input_elements[cmpId] |= invalidate_cmp_element;
            }
        }
        // change to next start ID if current element was compared against all other
        ownId += (cmpId == (VEC_NUM_IN - 1));
    }
}

template <typename DataType, vx_uint8 VEC_NUM_IN>
vx_bool CompareInputs(DataType first_input[VEC_NUM_IN], DataType second_input[VEC_NUM_IN],
                      DataType third_input[VEC_NUM_IN], DataType input_elements[VEC_NUM_IN],
                      vx_bool invalidate_input_elements[VEC_NUM_IN], vx_bool &read_current, vx_bool &read_previous,
                      vx_bool &read_next, const vx_bool first_input_is_active, const vx_bool second_input_is_active,
                      const vx_bool third_input_is_active, vx_bool (*read_order)(DataType *, DataType *),
                      vx_bool (*check_input_element)(const DataType),
                      void (*input_compare_fct)(vx_bool &, vx_bool &, const DataType, const DataType),
                      vx_bool (*is_invalid_input_element)(const DataType)) {
#pragma HLS INLINE

    const vx_bool is_active = first_input_is_active | second_input_is_active | third_input_is_active;

    if (is_active) {

        /*
         * Compare all keypoints vector against each other in respect to the y-coordinate.
         * All elements in a vector have the same y coordinate, even the invalid (ensured by the NMS function).
         * Only exception is in the case of non-max-suppression, than they can differ by a range of (-1, 1) compared to
         * the original y coordinate.
         */
        const vx_bool first_has_higher_order_than_second =
            first_input_is_active & (!second_input_is_active | read_order(first_input, second_input));
        const vx_bool first_has_higher_order_than_third =
            first_input_is_active & (!third_input_is_active | read_order(first_input, third_input));
        const vx_bool second_has_higher_order_than_third =
            second_input_is_active & (!third_input_is_active | read_order(second_input, third_input));

        // only one of the options can be true at the same time
        read_current = first_has_higher_order_than_second & first_has_higher_order_than_third;
        read_previous = (!first_has_higher_order_than_second) & second_has_higher_order_than_third;
        read_next =
            third_input_is_active & (!first_has_higher_order_than_second) & (!second_has_higher_order_than_third);

        // at most one of the three cases is possible
        if (read_current)
            copyVector<DataType, VEC_NUM_IN>(input_elements, first_input);
        if (read_previous)
            copyVector<DataType, VEC_NUM_IN>(input_elements, second_input);
        if (read_next)
            copyVector<DataType, VEC_NUM_IN>(input_elements, third_input);

        CompareVectorEntries<DataType, VEC_NUM_IN>(input_elements, invalidate_input_elements, check_input_element,
                                                   input_compare_fct, is_invalid_input_element);
    }

    return is_active;
}

/**
 * @brief Compares keypoints against each other in context of their response values if they are in the same radius. Only
 * the keypoint with the highest value passes all other are pruned. Also only keypoints of the same level are allowed to
 * pass but the keypoints of the lower and following level can be used to compare the keypoints of the current level.
 * @tparam VEC_NUM_IN Input vector size.
 * @tparam VEC_NUM_OUT Output vector size which COMPARE_PARALLELIZATION_LEVEL if VEC_NUM_IN is larger or equal
 * COMPARE_PARALLELIZATION_LEVEL or VEC_NUM_IN if VEC_NUM_IN is smaller than COMPARE_PARALLELIZATION_LEVEL.
 * @tparam IMG_COLS Number of pixels in x dimension depending on the current image size.
 * @tparam ORIGINAL_IMG_COLS Number of pixel in x dimension of the original image size.
 * @tparam ORIGINAL_IMG_ROWS Number of pixel in y dimension of the original image size.
 * @tparam MAX_KEYPOINTS_CURRENT Maximum number of keypoints to be read from the input of same level.
 * @tparam MAX_KEYPOINTS_PREVIOUS Maximum number of keypoints to be read from the previous level.
 * @tparam MAX_KEYPOINTS_NEXT Maximum number of keypoints to be read from the following level.
 * @tparam MAX_KEYPOINTS_OUTPUT Maximum number of keypoints to be written out. If the Maximum number of output keypoints
 * is less than the sum of all inputs, the inputs are still read until the end.
 * @tparam SCALE_WITH_FRACTION Search readius in which the keypoints are compared against each other. The the most
 * significant 16 bits are considered as integer part and the least significant 16 bits are considered as fractional
 * part.
 * @tparam LEVEL Current scale / evolution level
 * @tparam BOUNDARY Features inside the boundary are just used for comparison but are else pruned.
 * @tparam FIND_EXTREMA_KERNEL_SIZE Kernel size used in the feature extraction function (this is used to determine the
 * ring buffer size)
 * @tparam COMPARE_PARALLELIZATION_LEVEL Level of parallelization used for comparision.
 * @tparam RESTRICT_MAX_NUMBER_OF_ITERATIONS Stops writing out keypoints after a maximum number of defined iterations.
 * In the last iteration invalid keypoint is written out if the maximum number of output keypoints has not been until
 * then.
 * @param input_current Input of the same level keypoints.
 * @param input_previous Input of the previous level keypoints.
 * @param input_next Input of the following level keypoints.
 * @param output Output of all keypoints of the same level input which are outside the boundary as well are having the
 * maximum response value in the given radius.
 */
template <typename DataType, vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 MAX_FIRST_INPUT,
          vx_uint16 MAX_SECOND_INPUT, vx_uint16 MAX_THIRD_INPUT, vx_uint16 MAX_OUTPUT,
          vx_uint8 COMPARE_PARALLELIZATION_LEVEL, vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 BUFFER_SIZE>
void HwCompare(vx_image_data<DataType, VEC_NUM_IN> first_input[MAX_FIRST_INPUT],
               vx_image_data<DataType, VEC_NUM_IN>
                   second_input[(MAX_SECOND_INPUT > 0) ? (MAX_SECOND_INPUT)
                                                       : (1)], // For synthesis reasons (would work without check in
                                                               // SW). The synthesis tool does not supports array size
                                                               // of zero, even if parameter value is the null pointer.
               vx_image_data<DataType, VEC_NUM_IN> third_input[(MAX_THIRD_INPUT > 0) ? (MAX_THIRD_INPUT) : (1)],
               vx_image_data<DataType, 1> output[MAX_OUTPUT], // TODO VEC_NUM_OUT
               vx_bool (*is_invalid_element)(const DataType), vx_bool (*is_ignore_element)(const DataType),
               vx_bool (*read_order)(DataType *, DataType *), vx_bool (*check_input_vector_entry)(const DataType),
               void (*compare_input_vector_entries)(vx_bool &, vx_bool &, const DataType, const DataType),
               vx_bool (*update_function)(vx_bool &, vx_bool &, const DataType, const DataType),
               void (*invalidate_element)(DataType &), const DataType invalid_element, const DataType zero_element) {

#pragma HLS INLINE

    STATIC_ASSERT((MAX_OUTPUT <= (MAX_FIRST_INPUT + MAX_SECOND_INPUT + MAX_THIRD_INPUT)),
                  maximum_number_of_outputs_cannot_be_larger_than_the_sum_of_all_maximum_number_of_inputs);
    STATIC_ASSERT((COMPARE_PARALLELIZATION_LEVEL == 1) || (COMPARE_PARALLELIZATION_LEVEL == 2) ||
                      (COMPARE_PARALLELIZATION_LEVEL == 4) || (COMPARE_PARALLELIZATION_LEVEL == 8),
                  degree_of_parallelization_of_comparison_can_only_be_1_2_4_or_8);
    // STATIC_ASSERT for VEC_NUM_OUT

    // defines the size of the prefetch caches
    const vx_uint8 PREFETCH_REGISTER_SIZE =
        2; // TODO defines the minimum number of elements stored in the prefetch register, currently hard coded add
           // maybe later to the template parameter

    // if the input vector is larger than the (user) defined size of the prefetch register than use the input vector as
    // size to store the whole vector for the comparision of the next input
    const vx_uint8 NUMBER_OF_PREFETCH_REGISTER_ELEMENTS =
        ((VEC_NUM_IN / COMPARE_PARALLELIZATION_LEVEL) > PREFETCH_REGISTER_SIZE)
            ? (VEC_NUM_IN / COMPARE_PARALLELIZATION_LEVEL)
            : (PREFETCH_REGISTER_SIZE);
    // define the size of a partition depending on the the buffer size and the degree of parallelization
    const vx_uint32 PARTITION_SIZE = (BUFFER_SIZE / COMPARE_PARALLELIZATION_LEVEL) +
                                     (BUFFER_SIZE % COMPARE_PARALLELIZATION_LEVEL) * COMPARE_PARALLELIZATION_LEVEL;

    // define maximum number of loop iterations without emptying the buffer
    const vx_uint32 MAX_INPUTS = MAX_FIRST_INPUT + MAX_SECOND_INPUT + MAX_THIRD_INPUT;
    // determine the maximum number of iterations
    const vx_uint64 IT_FILL_REGISTER =
        PREFETCH_REGISTER_SIZE *
        (PREFETCH_REGISTER_SIZE + (COMPARE_PARALLELIZATION_LEVEL - 1) * (PREFETCH_REGISTER_SIZE + 1));
    const vx_uint64 IT_FILL_BUFFER =
        (COMPARE_PARALLELIZATION_LEVEL *
             (PARTITION_SIZE * (PARTITION_SIZE + 1) - PREFETCH_REGISTER_SIZE * (PREFETCH_REGISTER_SIZE + 1)) >>
         1) +
        PREFETCH_REGISTER_SIZE - PARTITION_SIZE;
    const vx_uint64 IT_FULL_BUFFER = ((MAX_INPUTS - BUFFER_SIZE) / COMPARE_PARALLELIZATION_LEVEL) *
                                     ((COMPARE_PARALLELIZATION_LEVEL - 1) * (PARTITION_SIZE + 1) + PARTITION_SIZE);
    const vx_uint64 MAX_ITERATIONS = 5 + IT_FILL_REGISTER + IT_FILL_BUFFER + IT_FULL_BUFFER + BUFFER_SIZE;
    const vx_uint64 RESTRICTED_ITERATION_NUM =
        (RESTRICT_MAX_NUMBER_OF_ITERATIONS == 0)
            ? (MAX_ITERATIONS)
            : MIN((RESTRICT_MAX_NUMBER_OF_ITERATIONS + MAX_INPUTS), MAX_ITERATIONS);

    // number of sectors which are written to at the beginning of each compare cycle
    const vx_uint8 ACCESSED_SECTORS =
        (VEC_NUM_IN > COMPARE_PARALLELIZATION_LEVEL) ? (VEC_NUM_IN / COMPARE_PARALLELIZATION_LEVEL) : (1);
    const vx_uint8 ACCESSED_PARTITIONS =
        (VEC_NUM_IN > COMPARE_PARALLELIZATION_LEVEL) ? (COMPARE_PARALLELIZATION_LEVEL) : (VEC_NUM_IN);

    // define buffer to store intermediate results
    vx_ra_ring_buffer<DataType, PARTITION_SIZE> buffer[COMPARE_PARALLELIZATION_LEVEL];
#pragma HLS array_partition variable = buffer complete dim = 1
    // PRAGMA_HLS_MEM_CORE(buffer->buffer, DataType, PARTITION_SIZE)
    HIFLIPVX_DATAPACK(buffer->buffer);

    // save the keypoint of the last iteration to hide the memory access latency of the buffer
    vx_ra_shift_register<DataType, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS>
        prefetch_register[COMPARE_PARALLELIZATION_LEVEL];
#pragma HLS array_partition variable = prefetch_register complete dim = 0

    // define variables used for interloop iterations

    // define if from this input shall be read in the first iteration
    vx_bool read_first_input = vx_true_e;
    vx_bool read_second_input = (MAX_SECOND_INPUT > 0);
    vx_bool read_third_input = (MAX_THIRD_INPUT > 0);

    // set active inputs
    vx_bool first_input_is_active = vx_true_e;
    vx_bool second_input_is_active = read_second_input;
    vx_bool third_input_is_active = read_third_input;

    // store input keypoint here until its written to the buffer
    DataType input_elements[VEC_NUM_IN];
#pragma HLS array_partition variable = input_elements complete dim = 0
    // buffer to store the vector from all inputs
    DataType first_input_data[VEC_NUM_IN];
#pragma HLS array_partition variable = first_input_data complete dim = 0
    DataType second_input_data[VEC_NUM_IN];
#pragma HLS array_partition variable = second_input_data complete dim = 0
    DataType third_input_data[VEC_NUM_IN];
#pragma HLS array_partition variable = third_input_data complete dim = 0

    // registers which are used to update the buffer keypoints to write them back in the following iteration
    vx_ping_pong<DataType, COMPARE_PARALLELIZATION_LEVEL> buffer_elements;
#pragma HLS array_partition variable = buffer_elements.buffer complete dim = 0
    // register to store popped keypoints from the buffer and write them out in the follwing iteration
    vx_ping_pong<DataType, 1> output_element;
    vx_ping_pong<vx_bool, 1> valid_output_element;

    valid_output_element[0][0] = vx_false_e;
    valid_output_element[1][0] = vx_false_e;

    output_element[0][0] = invalid_element;
    output_element[1][0] = invalid_element;
    vx_bool buffer_is_empty[COMPARE_PARALLELIZATION_LEVEL];
#pragma HLS array_partition variable = buffer_is_empty complete dim = 0

    /*
     * Keeps track if a input keypoint has been invalidated during a compare cycle. The invalidation (change of scale to
     * INVALID_SCALE value) at the end of each cycle when the keypoint is written on top of the prefetch register.
     * Implemented in a ping pong buffer style to remove dependency issues.
     */
    vx_ping_pong<vx_bool, VEC_NUM_IN> invalidate_input_elements;
#pragma HLS array_partition variable = invalidate_input_elements.buffer complete dim = 0
    vx_ping_pong<vx_bool, COMPARE_PARALLELIZATION_LEVEL> invalidate_buffer_elements;
#pragma HLS array_partition variable = invalidate_buffer_elements.buffer complete dim = 0

    vx_ping_pong<vx_bool, 1> stop_signal;
#pragma HLS array_partition variable = stop_signal.buffer complete dim = 0

    vx_ping_pong<vx_bool, 1> stop_buffer_access;
#pragma HLS array_partition variable = stop_buffer_access.buffer complete dim = 0

    stop_signal[0][0] = vx_false_e;
    stop_signal[1][0] = vx_false_e;

    stop_buffer_access[0][0] = vx_false_e;
    stop_buffer_access[1][0] = vx_false_e;

    // initialiaze all arrays which are depending on the input vector size
    for (vx_uint8 i = 0; i < VEC_NUM_IN; i++) {
#pragma HLS unroll
        input_elements[i] = zero_element;
        first_input_data[i] = zero_element;
        second_input_data[i] = zero_element;
        third_input_data[i] = zero_element;
        invalidate_input_elements[0][i] = vx_false_e;
        invalidate_input_elements[1][i] = vx_false_e;
        invalidate_buffer_elements[0][i] = vx_false_e;
        invalidate_buffer_elements[1][i] = vx_false_e;
    }

    // initialiaze all arrays which are depending on the parallelization size
    for (vx_uint8 i = 0; i < COMPARE_PARALLELIZATION_LEVEL; i++) {
#pragma HLS unroll
        prefetch_register[i].initialize(invalid_element);
        buffer_elements.buffer[0][i] = invalid_element;
        buffer_elements.buffer[1][i] = invalid_element;
        buffer_is_empty[i] = vx_false_e;
    }

    // index for output
    vx_uint16 ptr_dst = 0;

    // define indices for the inputs
    vx_uint16 ptr_first = 0;
    vx_uint16 ptr_second = 0;
    vx_uint16 ptr_third = 0;
    // index variables to update the start indices for read and write of each new iterations
    vx_uint16 buf_fp_start_write_index = PARTITION_SIZE - 1;
    vx_uint16 buf_fp_start_read_index = PARTITION_SIZE - NUMBER_OF_PREFETCH_REGISTER_ELEMENTS;
    vx_uint16 buf_fp_last_read_index;
    // index variables which are used during the compare iterations to update the read and write indices accordingly
    vx_uint16 buf_fp_write_index = 0;
    vx_uint16 buf_fp_read_index = 0;
    // variable
    vx_uint16 buf_post_push_full_sectors = 0;
    vx_uint16 buf_fp_write_index_update_register = 0;
    vx_uint16 cmp_iter = 0;

    // the first iterations of each cycle are reserved for pushing the input vector of the last cycle to the buffer
    vx_uint8 push_iter_counter = 0;

    /*
     * If no following keypoint was read from the input, the previous keypoint is still in the prefetch shift register
     * and not yet written to the buffer. Therefore this variable ensures if the loop switches to the empty buffer state
     * that the last read keypoint is written out (if valid) when the buffer is completely emptied.
     */
    vx_uint8 rem_reg_keypoint = VEC_NUM_IN;
    // defines to which partition the input keypoint is written to
    vx_uint8 selected_partition = COMPARE_PARALLELIZATION_LEVEL - 1;
    vx_bool is_active = vx_true_e;
    vx_bool buf_new_sector = vx_false_e;
    vx_uint8 sector_fill_level = 0;
    vx_bool stop_write_out = vx_false_e;

    // switch buffer also cyclic if emptying it
    vx_uint8 selected_partition_to_empty = 0;

    for (vx_uint64 i = 0; i < RESTRICTED_ITERATION_NUM; i++) {
#pragma HLS PIPELINE II = 1

        DataType rem_buf_element = invalid_element;
        vx_bool write_element_out = vx_false_e;
        vx_bool read_input = vx_false_e;
        vx_bool emptied_all_buffer = vx_false_e;

        // switch between register were the update is performed in this iteration and the update was performed in the
        // last iteration (and write this updated keypoint back to the buffer)
        invalidate_buffer_elements.switch_buffer();
        buffer_elements.switch_buffer();
        output_element.switch_buffer();
        valid_output_element.switch_buffer();
        stop_signal.switch_buffer();
        stop_buffer_access.switch_buffer();

        buf_fp_write_index_update_register = buf_fp_write_index;

        const vx_bool stop_cycle = stop_signal.read_from(0);

        const vx_bool stop_iter = ((i >= RESTRICT_MAX_NUMBER_OF_ITERATIONS) && (RESTRICT_MAX_NUMBER_OF_ITERATIONS > 0));

        // if cmp_iter is zero than a new comparison cycle starts
        if (cmp_iter == 0) {
            /*
             * Initialize:
             * Number of comparison iterations which depends on the number of elements written to the buffer.
             */
            // update the selected partition were the first new input is write to
            selected_partition = (selected_partition + ACCESSED_PARTITIONS) % COMPARE_PARALLELIZATION_LEVEL;
            // update first the variables used during the iterations
            buf_fp_write_index = buf_fp_start_write_index;
            buf_fp_read_index = buf_fp_start_read_index;

            // check if the current sector is full after pushing the input keypoint of the last comparison cycle to the
            // buffer
            buf_new_sector = (sector_fill_level ==
                              0); // check if the input keypoint of the last cycle is pushed to a new sector or not
            /*
             * The new keypoint is pushed to buffer in the first iteration of each compare cycle (only exception is the
             * first iteration). This done to prevent that two writes occur to the same partition in the same iteration
             * without introducing an extra iteration to write the new keypoint to the buffer.
             */
            buf_post_push_full_sectors =
                (i > 0) ? MIN((buf_post_push_full_sectors + buf_new_sector), PARTITION_SIZE) : (0);
            sector_fill_level = (sector_fill_level + 1) %
                                COMPARE_PARALLELIZATION_LEVEL; // compute the sector fill level for the next cycle
            // also take the read offset into consideration while computing the read address in case the comparison
            // cycle stops earlier
            const vx_bool read_offset = (buf_post_push_full_sectors == PARTITION_SIZE) && (!buf_new_sector);
            const vx_uint32 buf_fp_virt_start_write_index = buf_fp_start_write_index + buf_new_sector;
            const vx_uint32 buf_fp_virt_start_read_index = buf_fp_start_read_index + buf_new_sector;
            const vx_uint32 buf_fp_virt_last_read_index =
                buf_fp_virt_start_read_index + NUMBER_OF_PREFETCH_REGISTER_ELEMENTS - read_offset;
            buf_fp_start_write_index = (buf_fp_virt_start_write_index > PARTITION_SIZE - 1)
                                           ? (buf_fp_virt_start_write_index - PARTITION_SIZE)
                                           : (buf_fp_virt_start_write_index);
            buf_fp_start_read_index = (buf_post_push_full_sectors == NUMBER_OF_PREFETCH_REGISTER_ELEMENTS)
                                          ? (buf_fp_start_read_index)
                                          : ((buf_fp_virt_start_read_index > PARTITION_SIZE - 1)
                                                 ? (buf_fp_virt_start_read_index - PARTITION_SIZE)
                                                 : (buf_fp_virt_start_read_index));
            buf_fp_last_read_index = (buf_fp_virt_last_read_index > PARTITION_SIZE - 1)
                                         ? (buf_fp_virt_last_read_index - PARTITION_SIZE)
                                         : (buf_fp_virt_last_read_index);

            // determine the number of comparison iterations which depends on the number of elements in the buffer after
            // the push of the new elements
            cmp_iter =
                (stop_iter)
                    ? (0)
                    : (MAX(((MAX(buf_post_push_full_sectors, 1) - 1)), NUMBER_OF_PREFETCH_REGISTER_ELEMENTS - 1) +
                       (!buf_new_sector));
            // counter to keep track how many elements still have to be pushed to the buffer (+1 because the value is
            // updated before the actual transaction)
            push_iter_counter = ACCESSED_SECTORS + 1;
            // a new comparison cycle means to read new input keypoints
            read_input = vx_true_e;
            // switch invalidate register
            invalidate_input_elements.switch_buffer();

        } else {
            // for all following steps decrement the counter which keeps track of the number of iterations left in the
            // current comparison cycle

            const vx_bool stop_earlier = is_active && stop_cycle && (cmp_iter > NUMBER_OF_PREFETCH_REGISTER_ELEMENTS) &&

                                         (cmp_iter < (buf_post_push_full_sectors - 1));

            cmp_iter = (stop_earlier) ? (NUMBER_OF_PREFETCH_REGISTER_ELEMENTS - 1) : (cmp_iter - 1);

            /*
             * If a placeholder is pushed to the prefetch register the read index is stopped from decrementing.
             * If the a cyclic is stopped earlier change the read index also to load the keypoints for the next cycle to
             * the buffer
             */
            const vx_bool push_place_holder =
                (buf_new_sector) && ((cmp_iter >= VEC_NUM_IN) && (cmp_iter < NUMBER_OF_PREFETCH_REGISTER_ELEMENTS));
            // analogous to the push place holder for the read index but here it is related to the update buffer and the
            // write index (in short stops update of write index until all new keypoints are pushed to the buffer)
            const vx_bool push_next_kp =
                ((push_iter_counter > ACCESSED_SECTORS) && (buf_post_push_full_sectors == PARTITION_SIZE));
            // update the read index
            buf_fp_read_index =
                (stop_earlier)
                    ? (buf_fp_last_read_index)
                    : ((push_place_holder)
                           ? (buf_fp_read_index)
                           : ((buf_fp_read_index > 0)
                                  ? (buf_fp_read_index - 1)
                                  : (buf_post_push_full_sectors +
                                     (!(buf_post_push_full_sectors == PARTITION_SIZE) && (!buf_new_sector)) - 1)));
            // update the write index
            buf_fp_write_index = (buf_fp_write_index > 0) ? (buf_fp_write_index - 1) : (PARTITION_SIZE - 1);
            // decrease the counter for the push iteration until it reaches zero to indicate that all input keypoints
            // have been written to the buffer
            push_iter_counter = (push_iter_counter > 0) ? (push_iter_counter - 1) : (0);
        }

        /*
         * First part of the pipeline: read input if all comparison with the previous input are done or it is the first
         * iteration.
         */
        if (read_input) {

            // read next input
            CompareReadInput<DataType, VEC_NUM_IN, MAX_FIRST_INPUT>(first_input_data, ptr_first, first_input_is_active,
                                                                    first_input, read_first_input, is_invalid_element);
            CompareReadInput<DataType, VEC_NUM_IN, MAX_FIRST_INPUT>(second_input_data, ptr_second,
                                                                    second_input_is_active, second_input,
                                                                    read_second_input, is_invalid_element);
            CompareReadInput<DataType, VEC_NUM_IN, MAX_FIRST_INPUT>(third_input_data, ptr_third, third_input_is_active,
                                                                    third_input, read_third_input, is_invalid_element);

            // check if at least one input is still active and decide which input should be select next for comparision,
            // also initilizae invalidation register for inputs
            is_active = CompareInputs<DataType, VEC_NUM_IN>(
                first_input_data, second_input_data, third_input_data, input_elements,
                invalidate_input_elements.get_write_buffer(), read_first_input, read_second_input, read_third_input,
                first_input_is_active, second_input_is_active, third_input_is_active, read_order,
                check_input_vector_entry, compare_input_vector_entries, is_invalid_element);
        }

        /*
         * Second part of the pipeline: Consists of two parts where just one can be active at the same time: (1) fill
         * buffer and compare (2.1) empty buffer and (2.2) prefetch register
         */
        if (!stop_iter && !stop_buffer_access.read_from(0)) {

            // execute this step a last time to push the last keypoints from the prefetch register to the buffer before
            // emptying it.
            stop_buffer_access.write_to(0, (!is_active) && (push_iter_counter == 0));

            /*
             * Part (1): compare the new input with all other buffer elements.
             * If this is the first iteration of a new comparison cycle (starts with the read of a new input) write/push
             * the previous read input to the buffer. If the buffer is full, the oldest element is popped from the
             * buffer and replaced by the newly pushed element. If the popped element has a valid scale (meaning that it
             * has the largest response value in the defined radius and its from the same evolution level) than write
             * this value out in the third stage of the pipeline.
             */
            const vx_bool push_kp = ((push_iter_counter > 0) && (push_iter_counter <= ACCESSED_SECTORS));

            write_element_out =
                CompareBufferHandling<DataType, VEC_NUM_IN, PARTITION_SIZE, NUMBER_OF_PREFETCH_REGISTER_ELEMENTS,
                                      COMPARE_PARALLELIZATION_LEVEL>(
                    buffer, prefetch_register, input_elements, buffer_elements, rem_buf_element, stop_signal,
                    invalidate_input_elements, invalidate_buffer_elements, buf_post_push_full_sectors,
                    buf_fp_write_index_update_register, buf_fp_read_index, cmp_iter, selected_partition, buf_new_sector,
                    is_active, push_kp, update_function, is_ignore_element, is_invalid_element, invalidate_element,
                    invalid_element);

        } else if (buffer[selected_partition_to_empty].fill_level() > 0) {
            /*
             * Part (2.1): all inputs are read/no new input, start to empty the buffer (no comparison in with other
             * keypoints at this stage)
             */
            rem_buf_element = buffer[selected_partition_to_empty].pop_reverse();
            write_element_out = !is_invalid_element(rem_buf_element);
            stop_buffer_access.write_to(0, vx_true_e);
            selected_partition_to_empty = (selected_partition_to_empty + 1) % COMPARE_PARALLELIZATION_LEVEL;

        } else {
            emptied_all_buffer = vx_true_e;
            for (vx_uint8 j = 0; j < COMPARE_PARALLELIZATION_LEVEL; j++) {
#pragma HLS unroll
                emptied_all_buffer &= !(buffer[j].fill_level() > 0);
            }
        }

        output_element.write_to(0, rem_buf_element);
        valid_output_element.write_to(0, write_element_out);

        /*
         * Third part of pipeline: Write keypoint out and if less valid keypoints were detected than possible write an
         * invalid keypoint at the end to signal the end of the list/transaction.
         */
        if (valid_output_element.read_from(0)) {
            vx_image_data<DataType, 1> output_kp;
            output_kp.pixel[0] = output_element.read_from(0);
            GenerateDmaSignal<DataType, 1>((ptr_dst == 0), (ptr_dst == MAX_OUTPUT - 1), output_kp);
            output[ptr_dst] = output_kp;
            ++ptr_dst;

#ifndef __SYNTHESIS__
            // speed things up for software
            if (ptr_dst == MAX_OUTPUT - 1) {
                break;
            }
#endif

        } else if (((!is_active) && (emptied_all_buffer) && (!stop_write_out))) {

            vx_image_data<DataType, 1> invalid_output;
            invalid_output.pixel[0] = invalid_element;
            GenerateDmaSignal<DataType, 1>((ptr_dst == 0), vx_true_e, invalid_output);
            output[ptr_dst] = invalid_output;
            ++ptr_dst;

            stop_write_out = vx_true_e;

#ifndef __SYNTHESIS__
            // speed things up for software
            break;
#endif
        }
    }
}

/*
 * Function definitions for the HwCompareKeypoints function
 */

template <vx_uint8 INVALID_SCALE> vx_bool CompareValidKeypoint(const KeyPoint kp) {
#pragma HLS INLINE
    return (kp.scale == INVALID_SCALE);
}

vx_bool CompareIgnoreKeypoint(const KeyPoint kp) {
#pragma HLS INLINE

    KeyPointConverter kp_converter;
    kp_converter.data = INVALID_KEYPOINT_DATA;
    const KeyPoint INVALID_KEYPOINT = kp_converter.keypoint;

    return (kp.scale == INVALID_KEYPOINT.scale && kp.x == INVALID_KEYPOINT.x && kp.y == INVALID_KEYPOINT.y &&
            kp.orientation == INVALID_KEYPOINT.orientation && kp.response == INVALID_KEYPOINT.response);
}

template <vx_uint8 INVALID_SCALE> void CompareInvalidateKeypoint(KeyPoint &kp) {
#pragma HLS INLINE
    kp.scale = INVALID_SCALE;
}

template <vx_uint8 VEC_NUM_IN> vx_bool CompareKeypointsReadOrder(KeyPoint hkp[VEC_NUM_IN], KeyPoint lkp[VEC_NUM_IN]) {
#pragma HLS_INLINE
    return (hkp[0].y <= lkp[0].y);
}

template <vx_uint8 VEC_NUM_IN, vx_uint16 RADIUS_SQUARE>
vx_bool CompareKeypointsUpdate(vx_bool &invalidate_buf_kp, vx_bool &invalidate_input_kp, const KeyPoint buf_kp,
                               const KeyPoint input_kp) {
#pragma HLS INLINE

    // compute the squared distance distance for the next iteration
    const vx_int32 dist_x = (buf_kp.x - input_kp.x);
    const vx_int32 dist_y = (buf_kp.y - input_kp.y);
    const vx_uint32 dist_y_square = dist_y * dist_y;
    const vx_uint32 dist_square = dist_x * dist_x + dist_y * dist_y;
#pragma HLS RESOURCE variable = dist_y_square core = Mul

    // compare response values if buffer keypoint is in the radius of the input keypoint
    const vx_bool outside_y_direction = (dist_y_square > RADIUS_SQUARE);

    const vx_bool inside_radius = (dist_square <= RADIUS_SQUARE);

    const vx_bool input_response_is_smaller = (buf_kp.response > input_kp.response);

    // if the response value of the input keypoint is smaller than the response value of the buffer keypoint then
    // invalidate the input keypoint...
    invalidate_input_kp = (inside_radius && input_response_is_smaller);
    //... else invalidate the buffer keypoint
    invalidate_buf_kp = (inside_radius && (!input_response_is_smaller));

    return outside_y_direction;
}

template <vx_uint8 LEVEL, vx_uint16 LOWER_BOUNDARY, vx_uint16 UPPER_BOUNDARY_COLS, vx_uint16 UPPER_BOUNDARY_ROWS>
vx_bool CompareKeypointsCheckVectorEntry(const KeyPoint kp) {
#pragma HLS INLINE

    // check level / class id and boundary
    const vx_bool same_level = (LEVEL == kp.scale);
    const vx_bool in_boundary = ((kp.x > LOWER_BOUNDARY) & (kp.x < UPPER_BOUNDARY_COLS) & (kp.y > LOWER_BOUNDARY) &
                                 (kp.y < UPPER_BOUNDARY_ROWS));
    // only invalidate the keypoint if its actual a valid keypoint and one of both conditions (level, boundary) is not
    // fulfilled
    return (!(same_level && in_boundary));
}

template <vx_uint16 RADIUS>
void CompareKeypointsCompareVectorEntries(vx_bool &invalidate_first_kp, vx_bool &invalidate_second_kp,
                                          const KeyPoint first_kp, const KeyPoint second_kp) {
#pragma HLS INLINE
    // because the input vector contains only elements with the same position in respect to y and only differ in respect
    // to x compute only the distance between both keypoints in respect to x
    const vx_int16 dist_x = first_kp.x - second_kp.x;

    const vx_bool in_radius = (dist_x < RADIUS);
    const vx_bool first_kp_response_is_larger = (first_kp.response > second_kp.response);

    // invalidate the keypoints only if they are inside the radius in respect to x
    invalidate_first_kp = ((!first_kp_response_is_larger) && in_radius);
    invalidate_second_kp = (first_kp_response_is_larger && in_radius);
}

/*
 * Definition of the HwCompare function for keypoint comparison in the A-KAZE algorithm.
 */

template <vx_uint8 VEC_NUM_IN, vx_uint16 IMG_COLS, vx_uint16 ORIGINAL_IMG_COLS, vx_uint16 ORIGINAL_IMG_ROWS,
          vx_uint16 MAX_KEYPOINTS_CURRENT, vx_uint16 MAX_KEYPOINTS_PREVIOUS, vx_uint16 MAX_KEYPOINTS_NEXT,
          vx_uint16 MAX_KEYPOINTS_OUTPUT, vx_uint32 SCALE_WITH_FRACTION, vx_uint8 LEVEL, vx_uint16 BOUNDARY,
          vx_uint8 FIND_EXTREMA_KERNEL_SIZE, vx_uint8 COMPARE_PARALLELIZATION_LEVEL,
          vx_uint64 RESTRICT_MAX_NUMBER_OF_ITERATIONS, vx_uint32 MAX_BUFFER_SIZE>
void HwCompareKeypoints(
    vx_image_data<KeyPoint, VEC_NUM_IN> input_current[MAX_KEYPOINTS_CURRENT],
    vx_image_data<KeyPoint, VEC_NUM_IN>
        input_previous[(MAX_KEYPOINTS_PREVIOUS > 0)
                           ? (MAX_KEYPOINTS_PREVIOUS)
                           : (1)], // For synthesis reasons (would work without check in SW). The synthesis tool does
                                   // not supports array size of zero, even if parameter value is the null pointer.
    vx_image_data<KeyPoint, VEC_NUM_IN> input_next[(MAX_KEYPOINTS_NEXT > 0) ? (MAX_KEYPOINTS_NEXT) : (1)],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) { // TODO VEC_NUM_OUT
#pragma HLS INLINE

    STATIC_ASSERT((MAX_KEYPOINTS_OUTPUT <= MAX_KEYPOINTS_CURRENT),
                  maximum_number_of_outputs_cannot_be_larger_than_the_maximum_number_of_inputs_of_current_level);
    /*
     * Definition: In the following are elements which share the same index over all partitions are denoted as "sector".
     * E.g.: COMPARE_PARALLELIZATION_LEVEL = 4 -> 4 Partitions,	Indices 0 (partition 0), 1 (partition 1), 2 (partition
     *2) and 3 (partition 3) are sector 0 Indices 4 (partition 0), 5 (partition 1), 6 (partition 2) and 7 (partition 4)
     *are sector 1 and so on.
     */

    // Consider for the upper boundaries also that the last bits are fraction bits
    const vx_uint16 MAX_SIDE_LENGTH = MAX(ORIGINAL_IMG_COLS, ORIGINAL_IMG_ROWS);
    // hard coded number of bits available to store the x/y coordinate
    const vx_uint8 COORDINATE_BITS = 16;
    // determine the minumum number of required bits to represent the location of the image without fraction
    const vx_uint8 INTEGER_BITS = comp_log2<MAX_SIDE_LENGTH>::log2_ceil;
    // all bits that are left are used to represented the fractional part
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;
    // fill the boundary with zeros at the fractional part so that they are aligned with the coordinates used by
    // keypoint
    const vx_uint16 LOWER_BOUNDARY = BOUNDARY << FRACTION_BITS;
    const vx_uint16 UPPER_BOUNDARY_COLS = (ORIGINAL_IMG_COLS - BOUNDARY) << FRACTION_BITS;
    const vx_uint16 UPPER_BOUNDARY_ROWS = (ORIGINAL_IMG_ROWS - BOUNDARY) << FRACTION_BITS;

    // determine the number of iterations per comparison cycle is reduced by the factor of the parallelization level
    // (because COMPARE_PARALLELIZATION_LEVEL is always to the power of two floor=ceil)
    //    const vx_uint8 BIT_SHIFT_CMP_ITER = comp_log2<COMPARE_PARALLELIZATION_LEVEL>::log2_floor; // was not used
    // the radius of the find extrema kernel is required to determine the how many keypoints can be in a worst cased
    // scenario in the reach of the search radius which is used to dermine the minimum size of the ring buffer to store
    // the keypoints in this radius
    const vx_uint8 FIND_EXTREMA_KERNEL_RADIUS = FIND_EXTREMA_KERNEL_SIZE >> 1;
    const vx_uint8 NUMBER_OF_INPUTS = 1 + (MAX_KEYPOINTS_PREVIOUS > 0) + (MAX_KEYPOINTS_NEXT > 0);
    // compute the scale to define the maximum size for the buffer
    const vx_uint8 SCALE = (SCALE_WITH_FRACTION + (1 << 15)) >> 16;
    // compute the buffer size using the given information to determine the buffer size
    const vx_uint32 WORST_CASE_COLUMNS = IMG_COLS / FIND_EXTREMA_KERNEL_RADIUS / 2 + ((IMG_COLS % (2 * FIND_EXTREMA_KERNEL_RADIUS)) > 0);
    const vx_uint32 WORST_CASE_ROWS = SCALE / FIND_EXTREMA_KERNEL_RADIUS / 2 + ((SCALE % (2 * FIND_EXTREMA_KERNEL_RADIUS)) > 0);
    const vx_uint32 WORST_CASE_BUFFER_SIZE = WORST_CASE_COLUMNS * WORST_CASE_ROWS * NUMBER_OF_INPUTS;
    // if a maximum buffer size has been declared, always round it to full BRAM sizes (18k BRAM -> 16384/64=256
    // elemeents)
    const vx_uint32 MAX_BUFFER_SIZE_ROUNDED =
        (MAX_BUFFER_SIZE > 0) ? ((MAX_BUFFER_SIZE / 256) * 256 + (MAX_BUFFER_SIZE % 256 > 0) * 256) : (VX_UINT32_MAX);
    // Deterimine which of the three size (worst-case (depending on search radius), sum of all inputs or defines max
    // buffer size (if larger than 0) is minimal)
    const vx_uint32 BUFFER_SIZE_WO_PADDING = MIN((MAX_KEYPOINTS_CURRENT + MAX_KEYPOINTS_PREVIOUS + MAX_KEYPOINTS_NEXT),
                                                 (MIN(WORST_CASE_BUFFER_SIZE, MAX_BUFFER_SIZE_ROUNDED)));
    // ensure that the buffersize is always a multiple of the parallelization degree of the comparison
    const vx_uint32 BUFFER_SIZE_PAD =
        (COMPARE_PARALLELIZATION_LEVEL - (BUFFER_SIZE_WO_PADDING % COMPARE_PARALLELIZATION_LEVEL)) %
        COMPARE_PARALLELIZATION_LEVEL;
    // determine the final buffer size
    const vx_uint32 BUFFER_SIZE = BUFFER_SIZE_WO_PADDING + BUFFER_SIZE_PAD;
    // compute the radius which contains a fractional part
    const vx_uint16 RADIUS = SCALE_WITH_FRACTION >> ((sizeof(SCALE_WITH_FRACTION) << 2) - FRACTION_BITS);
    // compute the square of the radius which contains a fractional part
    const vx_uint16 RADIUS_SQUARE = (static_cast<vx_uint64>(SCALE_WITH_FRACTION) * SCALE_WITH_FRACTION) >>
                                    ((sizeof(SCALE_WITH_FRACTION) << 3) - (FRACTION_BITS << 1));
    // use level / class ID to set a keypoint in a buffer as invalid
    const vx_uint8 INVALID_SCALE = 255;

    // define invalid keypoint
    KeyPointConverter kp_converter;
    kp_converter.data = INVALID_KEYPOINT_DATA;
    const KeyPoint INVALID_KEYPOINT = kp_converter.keypoint;
    kp_converter.data = 0;
    const KeyPoint ZERO_KEYPOINT = kp_converter.keypoint;

    const vx_uint8 VEC_NUM_OUT = 1; // TODO move to template parameters later

    HwCompare<KeyPoint, VEC_NUM_IN, VEC_NUM_OUT, MAX_KEYPOINTS_CURRENT, MAX_KEYPOINTS_PREVIOUS, MAX_KEYPOINTS_NEXT,
              MAX_KEYPOINTS_OUTPUT, COMPARE_PARALLELIZATION_LEVEL, RESTRICT_MAX_NUMBER_OF_ITERATIONS, BUFFER_SIZE>(
        input_current, input_previous, input_next, output, CompareValidKeypoint<INVALID_SCALE>, CompareIgnoreKeypoint,
        CompareKeypointsReadOrder<VEC_NUM_IN>,
        CompareKeypointsCheckVectorEntry<LEVEL, LOWER_BOUNDARY, UPPER_BOUNDARY_COLS, UPPER_BOUNDARY_ROWS>,
        CompareKeypointsCompareVectorEntries<RADIUS>, CompareKeypointsUpdate<VEC_NUM_IN, RADIUS_SQUARE>,
        CompareInvalidateKeypoint<INVALID_SCALE>, INVALID_KEYPOINT, ZERO_KEYPOINT);
}

#endif /* __SRC_IMG_MT_FEATURE_COMPARE_H__ */
