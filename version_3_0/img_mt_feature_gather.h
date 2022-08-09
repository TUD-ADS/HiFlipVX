/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_gather.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  Functions needed for the feature gather function.
 */

#ifndef __SRC_IMG_MT_FEATURE_GATHER_H__
#define __SRC_IMG_MT_FEATURE_GATHER_H__

#include "vx_helper.h"

/**********************************************************************************************************************/
template <vx_uint8 NUMBER_OF_INPUTS, vx_uint16 MAX_KEYPOINTS_INPUT_1, vx_uint16 MAX_KEYPOINTS_INPUT_2,
          vx_uint16 MAX_KEYPOINTS_INPUT_3, vx_uint16 MAX_KEYPOINTS_INPUT_4, vx_uint16 MAX_KEYPOINTS_INPUT_5,
          vx_uint16 MAX_KEYPOINTS_INPUT_6, vx_uint16 MAX_KEYPOINTS_INPUT_7, vx_uint16 MAX_KEYPOINTS_INPUT_8,
          vx_uint16 MAX_KEYPOINTS_INPUT_9, vx_uint16 MAX_KEYPOINTS_INPUT_10, vx_uint16 MAX_KEYPOINTS_INPUT_11,
          vx_uint16 MAX_KEYPOINTS_INPUT_12, vx_uint16 MAX_KEYPOINTS_INPUT_13, vx_uint16 MAX_KEYPOINTS_INPUT_14,
          vx_uint16 MAX_KEYPOINTS_INPUT_15, vx_uint16 MAX_KEYPOINTS_INPUT_16, vx_uint32 MAX_KEYPOINTS_OUTPUT,
          HIFLIPVX::vx_scatter_mode_e SCATTER_MODE>
void HwGatherKeypoints(
    vx_image_data<KeyPoint, 1> input1[MAX_KEYPOINTS_INPUT_1], vx_image_data<KeyPoint, 1> input2[MAX_KEYPOINTS_INPUT_2],
    vx_image_data<KeyPoint, 1>
        input3[(MAX_KEYPOINTS_INPUT_3 > 0)
                   ? (MAX_KEYPOINTS_INPUT_3)
                   : (1)], // For synthesis reasons (would work without check in SW). The synthesis tool does not
                           // supports array size of zero, even if parameter value is the null pointer.
    vx_image_data<KeyPoint, 1> input4[(MAX_KEYPOINTS_INPUT_4 > 0) ? (MAX_KEYPOINTS_INPUT_4) : (1)],
    vx_image_data<KeyPoint, 1> input5[(MAX_KEYPOINTS_INPUT_5 > 0) ? (MAX_KEYPOINTS_INPUT_5) : (1)],
    vx_image_data<KeyPoint, 1> input6[(MAX_KEYPOINTS_INPUT_6 > 0) ? (MAX_KEYPOINTS_INPUT_6) : (1)],
    vx_image_data<KeyPoint, 1> input7[(MAX_KEYPOINTS_INPUT_7 > 0) ? (MAX_KEYPOINTS_INPUT_7) : (1)],
    vx_image_data<KeyPoint, 1> input8[(MAX_KEYPOINTS_INPUT_8 > 0) ? (MAX_KEYPOINTS_INPUT_8) : (1)],
    vx_image_data<KeyPoint, 1> input9[(MAX_KEYPOINTS_INPUT_9 > 0) ? (MAX_KEYPOINTS_INPUT_9) : (1)],
    vx_image_data<KeyPoint, 1> input10[(MAX_KEYPOINTS_INPUT_10 > 0) ? (MAX_KEYPOINTS_INPUT_10) : (1)],
    vx_image_data<KeyPoint, 1> input11[(MAX_KEYPOINTS_INPUT_11 > 0) ? (MAX_KEYPOINTS_INPUT_11) : (1)],
    vx_image_data<KeyPoint, 1> input12[(MAX_KEYPOINTS_INPUT_12 > 0) ? (MAX_KEYPOINTS_INPUT_12) : (1)],
    vx_image_data<KeyPoint, 1> input13[(MAX_KEYPOINTS_INPUT_13 > 0) ? (MAX_KEYPOINTS_INPUT_13) : (1)],
    vx_image_data<KeyPoint, 1> input14[(MAX_KEYPOINTS_INPUT_14 > 0) ? (MAX_KEYPOINTS_INPUT_14) : (1)],
    vx_image_data<KeyPoint, 1> input15[(MAX_KEYPOINTS_INPUT_15 > 0) ? (MAX_KEYPOINTS_INPUT_15) : (1)],
    vx_image_data<KeyPoint, 1> input16[(MAX_KEYPOINTS_INPUT_16 > 0) ? (MAX_KEYPOINTS_INPUT_16) : (1)],
    vx_image_data<KeyPoint, 1> output[MAX_KEYPOINTS_OUTPUT]) {

#pragma HLS INLINE
    // required to empty the inputs even if the maximum number of outputs has been reachd
    const vx_uint32 MAX_ITERATIONS =
        MAX_KEYPOINTS_INPUT_1 + MAX_KEYPOINTS_INPUT_2 + MAX_KEYPOINTS_INPUT_3 + MAX_KEYPOINTS_INPUT_4 +
        MAX_KEYPOINTS_INPUT_5 + MAX_KEYPOINTS_INPUT_6 + MAX_KEYPOINTS_INPUT_7 + MAX_KEYPOINTS_INPUT_8 +
        MAX_KEYPOINTS_INPUT_9 + MAX_KEYPOINTS_INPUT_10 + MAX_KEYPOINTS_INPUT_11 + MAX_KEYPOINTS_INPUT_12 +
        MAX_KEYPOINTS_INPUT_13 + MAX_KEYPOINTS_INPUT_14 + MAX_KEYPOINTS_INPUT_15 + MAX_KEYPOINTS_INPUT_16;

    // static assert checks
    const vx_bool check_max_keypoint_sizes = (MAX_KEYPOINTS_INPUT_1 > 0) && (MAX_KEYPOINTS_INPUT_2 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 2) || MAX_KEYPOINTS_INPUT_3 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 3) || MAX_KEYPOINTS_INPUT_4 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 4) || MAX_KEYPOINTS_INPUT_5 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 5) || MAX_KEYPOINTS_INPUT_6 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 6) || MAX_KEYPOINTS_INPUT_7 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 7) || MAX_KEYPOINTS_INPUT_8 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 8) || MAX_KEYPOINTS_INPUT_9 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 9) || MAX_KEYPOINTS_INPUT_10 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 10) || MAX_KEYPOINTS_INPUT_11 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 11) || MAX_KEYPOINTS_INPUT_12 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 12) || MAX_KEYPOINTS_INPUT_13 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 13) || MAX_KEYPOINTS_INPUT_14 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 14) || MAX_KEYPOINTS_INPUT_15 > 0) &&
                                             (!(NUMBER_OF_INPUTS > 15) || MAX_KEYPOINTS_INPUT_16 > 0);

    const vx_uint64 check_sum_keypoints =
        MAX_KEYPOINTS_INPUT_1 + MAX_KEYPOINTS_INPUT_2 + MAX_KEYPOINTS_INPUT_3 + MAX_KEYPOINTS_INPUT_4 +
        MAX_KEYPOINTS_INPUT_5 + MAX_KEYPOINTS_INPUT_6 + MAX_KEYPOINTS_INPUT_7 + MAX_KEYPOINTS_INPUT_8 +
        MAX_KEYPOINTS_INPUT_9 + MAX_KEYPOINTS_INPUT_10 + MAX_KEYPOINTS_INPUT_11 + MAX_KEYPOINTS_INPUT_12 +
        MAX_KEYPOINTS_INPUT_13 + MAX_KEYPOINTS_INPUT_14 + MAX_KEYPOINTS_INPUT_15 + MAX_KEYPOINTS_INPUT_16;

    STATIC_ASSERT(check_max_keypoint_sizes, maximum_number_of_keypoints_for_each_inputput_has_to_be_larger_than_zero);
    STATIC_ASSERT((MAX_KEYPOINTS_OUTPUT > 0), maximum_number_of_outputs_has_to_be_larger_than_zero);
    STATIC_ASSERT((MAX_KEYPOINTS_OUTPUT <= check_sum_keypoints),
                  maximum_number_of_outputs_has_to_be_smaller_or_equal_than_the_sum_of_all_maximum_inputs);
    STATIC_ASSERT((SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK) ||
                      (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_CYCLIC),
                  given_scatter_mode_is_not_supported_by_this_function);

    vx_uint16 MAX_KEYPOINTS_LUT[NUMBER_OF_INPUTS];
#pragma HLS array_partition variable = MAX_KEYPOINTS_LUT complete dim = 0

    vx_ap_uint<NUMBER_OF_INPUTS> active_ports = vx_ap_uint<NUMBER_OF_INPUTS>::max();
    vx_ap_uint<NUMBER_OF_INPUTS> input_ids = 1;

    vx_uint8 mask_shift = 0;
    vx_uint8 number_of_active_inputs = NUMBER_OF_INPUTS;
    vx_uint8 number_of_deactivated_ports = 0;
    vx_uint8 cycle_counter = 0;
    vx_uint16 ptr_src = 0;
    vx_uint32 ptr_dst = 0;

    KeyPointConverter kp_converter;
    kp_converter.data = INVALID_KEYPOINT_DATA;
    const KeyPoint INVALID_KEYPOINT = kp_converter.keypoint;

    // fills only to maximum number of inputs (not beautiful but it gets the thing done)
    for (vx_uint8 i = 0; i < NUMBER_OF_INPUTS; i++) {
#pragma HLS unroll
        switch (i) {
        case 0:
            MAX_KEYPOINTS_LUT[0] = MAX_KEYPOINTS_INPUT_1;
            break;
        case 1:
            MAX_KEYPOINTS_LUT[1] = MAX_KEYPOINTS_INPUT_2;
            break;
        case 2:
            MAX_KEYPOINTS_LUT[2] = MAX_KEYPOINTS_INPUT_3;
            break;
        case 3:
            MAX_KEYPOINTS_LUT[3] = MAX_KEYPOINTS_INPUT_4;
            break;
        case 4:
            MAX_KEYPOINTS_LUT[4] = MAX_KEYPOINTS_INPUT_5;
            break;
        case 5:
            MAX_KEYPOINTS_LUT[5] = MAX_KEYPOINTS_INPUT_6;
            break;
        case 6:
            MAX_KEYPOINTS_LUT[6] = MAX_KEYPOINTS_INPUT_7;
            break;
        case 7:
            MAX_KEYPOINTS_LUT[7] = MAX_KEYPOINTS_INPUT_8;
            break;
        case 8:
            MAX_KEYPOINTS_LUT[8] = MAX_KEYPOINTS_INPUT_9;
            break;
        case 9:
            MAX_KEYPOINTS_LUT[9] = MAX_KEYPOINTS_INPUT_10;
            break;
        case 10:
            MAX_KEYPOINTS_LUT[10] = MAX_KEYPOINTS_INPUT_11;
            break;
        case 11:
            MAX_KEYPOINTS_LUT[11] = MAX_KEYPOINTS_INPUT_12;
            break;
        case 12:
            MAX_KEYPOINTS_LUT[12] = MAX_KEYPOINTS_INPUT_13;
            break;
        case 13:
            MAX_KEYPOINTS_LUT[13] = MAX_KEYPOINTS_INPUT_14;
            break;
        case 14:
            MAX_KEYPOINTS_LUT[14] = MAX_KEYPOINTS_INPUT_15;
            break;
        case 15:
            MAX_KEYPOINTS_LUT[15] = MAX_KEYPOINTS_INPUT_16;
            break;
        }
    }

    for (vx_uint32 i = 0; i < MAX_ITERATIONS; i++) {
#pragma HLS PIPELINE II = 1

        vx_image_data<KeyPoint, 1> input_keypoint;
        vx_image_data<KeyPoint, 1> output_keypoint;

        const vx_bool active = (active_ports > 0);
        vx_uint8 port_index = 0;

        if (active) {
            if ((input_ids & 1) > 0) {
                input_keypoint = input1[ptr_src];
                mask_shift = 1;
                port_index = 0;
            } else if ((input_ids & (1 << 1)) > 0) {
                input_keypoint = input2[ptr_src];
                mask_shift = 2;
                port_index = 1;
            } else if ((input_ids & (1 << 2)) > 0) {
                input_keypoint = input3[ptr_src];
                mask_shift = 3;
                port_index = 2;
            } else if ((input_ids & (1 << 3)) > 0) {
                input_keypoint = input4[ptr_src];
                mask_shift = 4;
                port_index = 3;
            } else if ((input_ids & (1 << 4)) > 0) {
                input_keypoint = input5[ptr_src];
                mask_shift = 5;
                port_index = 4;
            } else if ((input_ids & (1 << 5)) > 0) {
                input_keypoint = input6[ptr_src];
                mask_shift = 6;
                port_index = 5;
            } else if ((input_ids & (1 << 6)) > 0) {
                input_keypoint = input7[ptr_src];
                mask_shift = 7;
                port_index = 6;
            } else if ((input_ids & (1 << 7)) > 0) {
                input_keypoint = input8[ptr_src];
                mask_shift = 8;
                port_index = 7;
            } else if ((input_ids & (1 << 8)) > 0) {
                input_keypoint = input9[ptr_src];
                mask_shift = 9;
                port_index = 8;
            } else if ((input_ids & (1 << 9)) > 0) {
                input_keypoint = input10[ptr_src];
                mask_shift = 10;
                port_index = 9;
            } else if ((input_ids & (1 << 10)) > 0) {
                input_keypoint = input11[ptr_src];
                mask_shift = 11;
                port_index = 10;
            } else if ((input_ids & (1 << 11)) > 0) {
                input_keypoint = input12[ptr_src];
                mask_shift = 12;
                port_index = 11;
            } else if ((input_ids & (1 << 12)) > 0) {
                input_keypoint = input13[ptr_src];
                mask_shift = 13;
                port_index = 12;
            } else if ((input_ids & (1 << 13)) > 0) {
                input_keypoint = input14[ptr_src];
                mask_shift = 14;
                port_index = 13;
            } else if ((input_ids & (1 << 14)) > 0) {
                input_keypoint = input15[ptr_src];
                mask_shift = 15;
                port_index = 14;
            } else if ((input_ids & (1 << 15)) > 0) {
                input_keypoint = input16[ptr_src];
                mask_shift = 0;
                port_index = 15;
            }
        }

        const KeyPoint keypoint = input_keypoint.pixel[0];
        kp_converter.keypoint = keypoint;

        const vx_bool write_keypoint =
            (kp_converter.data != INVALID_KEYPOINT_DATA && ptr_src < MAX_KEYPOINTS_LUT[port_index]);
        active_ports.set_bit(port_index, write_keypoint);
        // only used by VX_SCATTER_MODE_CYCLIC mode
        const vx_bool new_cycle = (cycle_counter == number_of_active_inputs - 1); // check if all still active ports
        number_of_deactivated_ports += (!write_keypoint);
        cycle_counter = (new_cycle) ? (0) : (cycle_counter + 1);
        number_of_active_inputs -= (new_cycle) ? (number_of_deactivated_ports)
                                               : (0); // update number or active ports for the following new cycle
        number_of_deactivated_ports =
            (new_cycle) ? (0) : (number_of_deactivated_ports); // reset number of deactivated ports for the next cycle

        // change port depending on mode
        if (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK) {
            input_ids <<= (active & (!write_keypoint));
        } else { // VX_SCATTER_MODE_CYCLIC
            // check if the next iteration is a new cycle and update the mask accordenly
            const vx_ap_uint<NUMBER_OF_INPUTS> mask =
                (vx_ap_uint<NUMBER_OF_INPUTS>::max() << ((new_cycle) ? (0) : (mask_shift)));
            // update input port
            input_ids = mask & active_ports;
        }

        // get index of next input depending on the the mode
        if (SCATTER_MODE == HIFLIPVX::VX_SCATTER_GATHER_MODE_BLOCK) {
            // reset to zero if the input is invalid or all inputs have beem read
            ptr_src = (active && write_keypoint) ? (ptr_src + 1) : (0);
        } else { // VX_SCATTER_MODE_CYCLIC
            // increase the src pointer if all active ports has been read once this cycle
            ptr_src += new_cycle;
        }

        if (active && (ptr_dst < MAX_KEYPOINTS_OUTPUT)) {
            // case: there are less detected keypoints than the maximum defined number -> write invalid keypoint with
            // end of transaction at the end of the list
            if (active_ports == 0) {
                // if there is no next input write invalid keypoint to indicate the end of the transaction
                GenerateDmaSignal<KeyPoint, 1>((ptr_dst == 0), vx_true_e, output_keypoint);
                output_keypoint.pixel[0] = INVALID_KEYPOINT;
                output[ptr_dst] = output_keypoint;
            }
            // case; valid keypoint was read -> keypoint is routed to the output
            else if (write_keypoint) {
                // write next keypoint out
                GenerateDmaSignal<KeyPoint, 1>((ptr_dst == 0), (ptr_dst == MAX_KEYPOINTS_OUTPUT - 1), output_keypoint);
                output_keypoint.pixel[0] = keypoint;
                output[ptr_dst] = output_keypoint;
                ++ptr_dst;
            }
        }
    }
}

#endif /* __SRC_IMG_MT_FEATURE_GATHER_H__ */
