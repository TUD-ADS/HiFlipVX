/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_feature_extract.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  Functions needed for the feature extract function.
 */

#ifndef __SRC_IMG_MT_FEATURE_EXTRACT_H__
#define __SRC_IMG_MT_FEATURE_EXTRACT_H__

#include "img_mt_helper.h"

// TODO for testing
#define USE_FLOAT_FOR_SR

/**********************************************************************************************************************/
// feature extraction function
template <typename ResponseType, typename OrientationType, vx_uint32 DOWNSAMPLE_FACTOR, vx_uint32 CLASS_ID,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
KeyPoint CreateFeature2(const vx_uint16 x, const vx_uint16 y, const ResponseType response,
                        const OrientationType orientation) {
#pragma HLS INLINE

    const vx_uint8 BITS_INPUT = BYTES_IN_BITS(sizeof(ResponseType));

    // number of bits available to represent x or y coordinate (including fraction)
    const vx_uint8 COORDINATE_BITS = 16;

    // number of bits available to represent the response value
    const vx_uint8 RESPONSE_VALUE_BITS = 16;

    // number of fraction bits for sampling
    const vx_uint32 SAMPLE_FRACTION_BITS = 16;
    const vx_uint32 FRACTION_MASK = (1 << SAMPLE_FRACTION_BITS) - 1;

    // original rows and columns
    const vx_uint32 SAMPLE_ROWS = (static_cast<vx_uint32>(IMG_ROWS) * DOWNSAMPLE_FACTOR);
    const vx_uint32 SAMPLE_COLS = (static_cast<vx_uint32>(IMG_COLS) * DOWNSAMPLE_FACTOR);

    // compute number of integer bits at compile time
    const vx_uint8 INTEGER_BITS = comp_log2<(MAX(SAMPLE_ROWS, SAMPLE_COLS) >> SAMPLE_FRACTION_BITS)>::log2_ceil;

    // because each coordinate gets 16 bits, the number of fraction bits
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;

    // check if original image rows/cols are with fraction (should not be)
    const vx_bool NO_ROW_FRACTION = (static_cast<vx_uint32>(SAMPLE_ROWS) & FRACTION_MASK) == 0;
    const vx_bool NO_COL_FRACTION = (static_cast<vx_uint32>(SAMPLE_COLS) & FRACTION_MASK) == 0;

    // check if sampling factor: is multiple of two | has fraction (for optimized computation)
    const vx_bool NO_FRACTION = (DOWNSAMPLE_FACTOR & FRACTION_MASK) == 0;
    const vx_bool IS_POWER_TWO = (DOWNSAMPLE_FACTOR & (DOWNSAMPLE_FACTOR - 1)) == 0;

    // optimized version without fraction and a power of 2
    if (NO_FRACTION && IS_POWER_TWO) {
        const vx_uint8 OCTAVE = comp_log2<(DOWNSAMPLE_FACTOR >> SAMPLE_FRACTION_BITS)>::log2_ceil;
        const vx_uint16 res_x = (x << (FRACTION_BITS + OCTAVE));
        const vx_uint16 res_y = (y << (FRACTION_BITS + OCTAVE));
        
        return createKeypoint(res_x, res_y, static_cast<vx_uint16>(response >> (BITS_INPUT - RESPONSE_VALUE_BITS)),
                              CLASS_ID, orientation);
    } else {        
        const vx_uint16 res_x = ((x << FRACTION_BITS) * DOWNSAMPLE_FACTOR) >> SAMPLE_FRACTION_BITS;
        const vx_uint16 res_y = ((y << FRACTION_BITS) * DOWNSAMPLE_FACTOR) >> SAMPLE_FRACTION_BITS;
        return createKeypoint(res_x, res_y, static_cast<vx_uint16>(response >> (BITS_INPUT - RESPONSE_VALUE_BITS)),
                              CLASS_ID, orientation);
    }
}

/**********************************************************************************************************************/
// feature extraction function
template <typename ResponseType, typename OrientationType, vx_uint16 DOWNSAMPLE_FACTOR, vx_uint32 CLASS_ID,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
KeyPoint CreateFeature(const vx_uint16 x, const vx_uint16 y, const ResponseType response,
                       const OrientationType orientation) {
#pragma HLS INLINE

    const vx_uint8 BITS_INPUT = BYTES_IN_BITS(sizeof(ResponseType));

    // number of bits available to represent x or y coordinate (including fraction)
    const vx_uint8 COORDINATE_BITS = 16;

    // number of bits available to represent the response value
    const vx_uint8 RESPONSE_VALUE_BITS = 16;

    // compute number of integer bits at compile time
    const vx_uint8 INTEGER_BITS = comp_log2<(MAX(IMG_COLS, IMG_ROWS) * DOWNSAMPLE_FACTOR)>::log2_ceil;

    const vx_uint8 OCTAVE = comp_log2<DOWNSAMPLE_FACTOR>::log2_ceil;

    // because each coordinate gets 16 bits, the number of fraction bits
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;

    const vx_uint16 res_x = (x << (FRACTION_BITS + OCTAVE));
    const vx_uint16 res_y = (y << (FRACTION_BITS + OCTAVE));

    return createKeypoint(res_x, res_y, static_cast<vx_uint16>(response >> (BITS_INPUT - RESPONSE_VALUE_BITS)),
                          CLASS_ID, orientation);
}

/**********************************************************************************************************************/
template <typename InType, vx_uint16 KERN_SIZE, vx_uint32 DOWNSAMPLE_FACTOR, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
vx_bool SubpixelRefinement2(InType input[KERN_SIZE][KERN_SIZE], KeyPoint &keypoint) {
#pragma HLS INLINE

    vx_uint16 INT_SUBPIXEL_MUL = 1;

    // number of bits available to represent x or y coordinate (including fraction)
    const vx_uint8 COORDINATE_BITS = 16;

    // number of fraction bits for sampling
    const vx_uint32 SAMPLE_FRACTION_BITS = 16;
    const vx_uint32 FRACTION_MASK = (1 << SAMPLE_FRACTION_BITS) - 1;

    // original rows and columns
    const vx_uint32 SAMPLE_ROWS = (static_cast<vx_uint32>(IMG_ROWS) * DOWNSAMPLE_FACTOR);
    const vx_uint32 SAMPLE_COLS = (static_cast<vx_uint32>(IMG_COLS) * DOWNSAMPLE_FACTOR);

    // compute number of integer bits at compile time
    const vx_uint8 INTEGER_BITS = comp_log2<(MAX(SAMPLE_ROWS, SAMPLE_COLS) >> SAMPLE_FRACTION_BITS)>::log2_ceil;

    // because each coordinate gets 16 bits, the number of fraction bits
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;

    // check if original image rows/cols are with fraction (should not be)
    const vx_bool NO_ROW_FRACTION = (static_cast<vx_uint32>(SAMPLE_ROWS) & FRACTION_MASK) == 0;
    const vx_bool NO_COL_FRACTION = (static_cast<vx_uint32>(SAMPLE_COLS) & FRACTION_MASK) == 0;

    // update subpixel by fraction and sampling factor
    const vx_uint16 SUBPIXEL_MUL = ((1 << FRACTION_BITS) * DOWNSAMPLE_FACTOR) >> SAMPLE_FRACTION_BITS;

    // compute the hessian using the directly surrounding 8 pixels independend of window size
    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint8 LOW = KERN_RAD - 1;
    const vx_uint8 HIGH = KERN_RAD + 1;

    const vx_uint8 BITS_INPUT = BYTES_IN_BITS(sizeof(InType));

    // shift back to 16 bit for subpixel refinement
    const vx_uint8 SR_BITS = 16;
    const vx_uint8 SHIFT_TO_SR_BITS = BITS_INPUT - SR_BITS;

    const vx_uint8 HESSIAN = BITS_INPUT + 1;
    const vx_uint8 HESSIAN_SR_BITS = SR_BITS + 1;
    const vx_uint8 RES_MUL = 2 * SR_BITS + 1;

    const vx_uint8 RES0_NUM = RES_MUL + 3 + 4;
    const vx_uint8 RES0_DEN = RES_MUL + 4 + 4;
    const vx_uint8 RES1_NUM = RES_MUL + 2;
    const vx_uint8 RES1_DEN = RES_MUL + 2;

    const InType center = input[KERN_RAD][KERN_RAD];

    /*
     * Because the threshold is a positive value center also can be just positive.
     * Let B denote the number of input bits (8 or 16). All surrounding values are at most (close to) the value of
     * center. ps_Dx and ps_Dy: 0|B - -0|B = 1|B or -0|B + -0|B = -1|B Dxx and Dyy: 0|B + 0|B - 2 * 0|B = 1|B - 1|B =
     * 0|B or -0|B + -0|B - 2 * 0|B = -1|B -1|B =-2|B ps_Dxy: 0|B + 0|B - -0|B - -0|B = 4 * 0|B = 2|B or -0|B + -0|B -
     * 0|B - 0|B = 4 * -0|B = -2|B +1 bit because of sign
     */
    const vx_ap_int<HESSIAN + 1> ps_Dx =
        static_cast<vx_int64>(input[KERN_RAD][HIGH]) - static_cast<vx_int64>(input[KERN_RAD][LOW]);
    const vx_ap_int<HESSIAN + 1> ps_Dy =
        static_cast<vx_int64>(input[HIGH][KERN_RAD]) - static_cast<vx_int64>(input[LOW][KERN_RAD]);
    const vx_ap_int<HESSIAN> ps_Dxx = static_cast<vx_int64>(input[KERN_RAD][LOW]) +
                                      static_cast<vx_int64>(input[KERN_RAD][HIGH]) -
                                      (static_cast<vx_uint64>(center) << 1);
    const vx_ap_int<HESSIAN> ps_Dyy = static_cast<vx_int64>(input[LOW][KERN_RAD]) +
                                      static_cast<vx_int64>(input[HIGH][KERN_RAD]) -
                                      (static_cast<vx_uint64>(center) << 1);
    const vx_ap_int<HESSIAN + 2> ps_Dxy =
        static_cast<vx_int64>(input[HIGH][HIGH]) + static_cast<vx_int64>(input[LOW][LOW]) -
        static_cast<vx_int64>(input[LOW][HIGH]) - static_cast<vx_int64>(input[HIGH][LOW]);

    /*
     * Dx and Dy: 1|B >> 1 = 0|B or -1|B >> 1 = -0|B
     * Dxy: 3|B >> 2 = 0|B or -3|B >> 2 = -0|B
     * +1 bit because of sign
     */
    const vx_ap_int<HESSIAN_SR_BITS> Dx = ps_Dx >> (1 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dy = ps_Dy >> (1 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dxy = ps_Dxy >> (2 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dxx = ps_Dxx >> SHIFT_TO_SR_BITS;
    const vx_ap_int<HESSIAN_SR_BITS> Dyy = ps_Dyy >> SHIFT_TO_SR_BITS;

    /*
     * (Dx * Dxy - Dy * Dxx) / (Dyy * Dxx - Dxy * Dxy)
     *
     * res1_num and res1_den: 8bit * 8bit = 16 bit or 16bit * 16bit = 32bit
     * Shift back by size of input data type.
     *
     * res1_num and res1_den 0|B * (+/-2|B) - 0|B * (+/-2|B) = 2|2*B or -2|2*B
     * +1 bit because of sign
     */

    const float fDx = static_cast<float>(Dx.value);
    const float fDy = static_cast<float>(Dy.value);
    const float fDxy = static_cast<float>(Dxy.value);
    const float fDxx = static_cast<float>(Dxx.value);
    const float fDyy = static_cast<float>(Dyy.value);

    const float res1 = (fDx * fDxy - fDy * fDxx) / (fDyy * fDxx - fDxy * fDxy);
    const float res0 = (-fDx - fDxy * res1) / fDxx;

    const float res1_num_ = fDx * fDxy - fDy * fDxx;
    const float res1_den_ = fDyy * fDxx - fDxy * fDxy;

    const float res0_num_ = -(fDx * res1_den_ + fDxy * res1_num_);
    const float res0_den_ = fDxx * res1_den_;

    const vx_bool keypoint_detected = (res1 <= 1.0 && res1 >= -1.0) && (res0 <= 1.0 && res0 >= -1.0);

    const vx_int16 subpixel_x = static_cast<vx_uint16>(res0 * SUBPIXEL_MUL);
    const vx_int16 subpixel_y = static_cast<vx_uint16>(res1 * SUBPIXEL_MUL);

    /*
     * Because the findExtrema function shifts the integer part to the highest bits and
     * add or subtract subpixel fraction (considering also the UP_SCALE_FACTOR).
     * E.g.
     * 	4K image (12 integer and 4 fraction bits), UP_SCALE_FACTOR = 4 and x = 1017 (11 1111 1001)
     * 	subpixel s has 4 (fraction bits) + 2 bits (UP_SCALE_FACTOR/2) and has the value 0.671875 (43 or 10 1011)
     *
     *	(x + s) * 4 = 1017.671875 = 4070.6875 (1111 1110 0110.1011)
     *	is equivalent to x << (4 + 2) + s = 1017 << (4 + 2) + 43
     *	11 1111 1001 << (4 + 2) + 10 1011 = 1111 1110 0100.0000 + 10 1011 = 1111 1110 0110.1011
     *
     */
    // can overwrite keypoint because it is discarded anyway if keypoint_detected is false
    keypoint =
        createKeypoint(static_cast<vx_uint16>(keypoint.x + subpixel_x), static_cast<vx_uint16>(keypoint.y + subpixel_y),
                       keypoint.response, keypoint.scale, keypoint.orientation);

    return keypoint_detected;
}
/**********************************************************************************************************************/
/**
 * @brief Applies subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam KERN_SIZE Size of NMS window
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @param input Response values
 * @param keypoint (Detected) Keypoint
 * @return True if keypoint was detected else false
 */
template <typename InType, vx_uint16 KERN_SIZE, vx_uint8 DOWNSAMPLE_FACTOR, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS>
vx_bool SubpixelRefinement(InType input[KERN_SIZE][KERN_SIZE], KeyPoint &keypoint) {
#pragma HLS INLINE

// TODO for testing
#define USE_FLOAT_FOR_SR

    const vx_uint16 OCTAVE = comp_log2<DOWNSAMPLE_FACTOR>::log2_ceil;

    // number of bits available to represent x or y coordinate (including fraction)
    const vx_uint8 COORDINATE_BITS = 16;

    // compute number of integer bits at compile time
    const vx_uint8 INTEGER_BITS = comp_log2<(MAX(IMG_COLS, IMG_ROWS) * DOWNSAMPLE_FACTOR)>::log2_ceil;

    // because each coordinate gets 16 bits, the number of fraction bits
    const vx_uint8 FRACTION_BITS = COORDINATE_BITS - INTEGER_BITS;

    /*
     * Because we multiply with the SCALE (1,2,4,8)ue which equals a number of left bit shift by SCALE/2.
     * To keep the subpixel precision the number of comparisons increases linear by SCALE/2.
     * + 1 to compute the case the denominator equals the numerator.
     */
    const vx_uint16 PRECISION = FRACTION_BITS + OCTAVE;

    // compute the hessian using the directly surrounding 8 pixels independend of window size
    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint8 LOW = KERN_RAD - 1;
    const vx_uint8 HIGH = KERN_RAD + 1;

    const vx_uint8 BITS_INPUT = BYTES_IN_BITS(sizeof(InType));

    // shift back to 16 bit for subpixel refinement
    const vx_uint8 SR_BITS = 16;
    const vx_uint8 SHIFT_TO_SR_BITS = BITS_INPUT - SR_BITS;

    const vx_uint8 HESSIAN = BITS_INPUT + 1;
    const vx_uint8 HESSIAN_SR_BITS = SR_BITS + 1;
    const vx_uint8 RES_MUL = 2 * SR_BITS + 1;

    const vx_uint8 RES0_NUM = RES_MUL + 3 + 4;
    const vx_uint8 RES0_DEN = RES_MUL + 4 + 4;
    const vx_uint8 RES1_NUM = RES_MUL + 2;
    const vx_uint8 RES1_DEN = RES_MUL + 2;

    /*
     * Tests showed that using the normalized version requires to have at least four additional bits to keep the
     * precision during the calculation of res0. Also increase the number of bits per octave because with increasing
     * octave a higher precision is required.
     */
    const vx_uint8 RES0_NUM_NORM = SR_BITS + 3 + 4 + OCTAVE;
    const vx_uint8 RES0_DEN_NORM = SR_BITS + 4 + 4 + OCTAVE;
    const vx_uint8 RES1_NUM_NORM = SR_BITS + 2 + 4 + OCTAVE;
    const vx_uint8 RES1_DEN_NORM = SR_BITS + 2 + 4 + OCTAVE;

    const vx_uint8 ABS_RES0_NUM = RES0_NUM_NORM + FRACTION_BITS + 1;
    const vx_uint8 ABS_RES0_DEN = RES0_DEN_NORM + FRACTION_BITS + 1;
    const vx_uint8 ABS_RES1_NUM = RES1_NUM_NORM + FRACTION_BITS + 1;
    const vx_uint8 ABS_RES1_DEN = RES1_DEN_NORM + FRACTION_BITS + 1;

    const InType center = input[KERN_RAD][KERN_RAD];

    /*
     * Because the threshold is a positive value center also can be just positive.
     * Let B denote the number of input bits (8 or 16). All surrounding values are at most (close to) the value of
     * center. ps_Dx and ps_Dy: 0|B - -0|B = 1|B or -0|B + -0|B = -1|B Dxx and Dyy: 0|B + 0|B - 2 * 0|B = 1|B - 1|B =
     * 0|B or -0|B + -0|B - 2 * 0|B = -1|B -1|B =-2|B ps_Dxy: 0|B + 0|B - -0|B - -0|B = 4 * 0|B = 2|B or -0|B + -0|B -
     * 0|B - 0|B = 4 * -0|B = -2|B +1 bit because of sign
     */
    const vx_ap_int<HESSIAN + 1> ps_Dx =
        static_cast<vx_int64>(input[KERN_RAD][HIGH]) - static_cast<vx_int64>(input[KERN_RAD][LOW]);
    const vx_ap_int<HESSIAN + 1> ps_Dy =
        static_cast<vx_int64>(input[HIGH][KERN_RAD]) - static_cast<vx_int64>(input[LOW][KERN_RAD]);
    const vx_ap_int<HESSIAN> ps_Dxx = static_cast<vx_int64>(input[KERN_RAD][LOW]) +
                                      static_cast<vx_int64>(input[KERN_RAD][HIGH]) -
                                      (static_cast<vx_uint64>(center) << 1);
    const vx_ap_int<HESSIAN> ps_Dyy = static_cast<vx_int64>(input[LOW][KERN_RAD]) +
                                      static_cast<vx_int64>(input[HIGH][KERN_RAD]) -
                                      (static_cast<vx_uint64>(center) << 1);
    const vx_ap_int<HESSIAN + 2> ps_Dxy =
        static_cast<vx_int64>(input[HIGH][HIGH]) + static_cast<vx_int64>(input[LOW][LOW]) -
        static_cast<vx_int64>(input[LOW][HIGH]) - static_cast<vx_int64>(input[HIGH][LOW]);

    /*
     * Dx and Dy: 1|B >> 1 = 0|B or -1|B >> 1 = -0|B
     * Dxy: 3|B >> 2 = 0|B or -3|B >> 2 = -0|B
     * +1 bit because of sign
     */
    const vx_ap_int<HESSIAN_SR_BITS> Dx = ps_Dx >> (1 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dy = ps_Dy >> (1 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dxy = ps_Dxy >> (2 + SHIFT_TO_SR_BITS);
    const vx_ap_int<HESSIAN_SR_BITS> Dxx = ps_Dxx >> SHIFT_TO_SR_BITS;
    const vx_ap_int<HESSIAN_SR_BITS> Dyy = ps_Dyy >> SHIFT_TO_SR_BITS;

    /*
     * (Dx * Dxy - Dy * Dxx) / (Dyy * Dxx - Dxy * Dxy)
     *
     * res1_num and res1_den: 8bit * 8bit = 16 bit or 16bit * 16bit = 32bit
     * Shift back by size of input data type.
     *
     * res1_num and res1_den 0|B * (+/-2|B) - 0|B * (+/-2|B) = 2|2*B or -2|2*B
     * +1 bit because of sign
     */
#ifdef USE_FLOAT_FOR_SR

    const float fDx = static_cast<float>(Dx.value);
    const float fDy = static_cast<float>(Dy.value);
    const float fDxy = static_cast<float>(Dxy.value);
    const float fDxx = static_cast<float>(Dxx.value);
    const float fDyy = static_cast<float>(Dyy.value);

    const float res1 = (fDx * fDxy - fDy * fDxx) / (fDyy * fDxx - fDxy * fDxy);
    const float res0 = (-fDx - fDxy * res1) / fDxx;

    const float res1_num_ = fDx * fDxy - fDy * fDxx;
    const float res1_den_ = fDyy * fDxx - fDxy * fDxy;

    const float res0_num_ = -(fDx * res1_den_ + fDxy * res1_num_);
    const float res0_den_ = fDxx * res1_den_;

    const vx_bool keypoint_detected = (res1 <= 1.0 && res1 >= -1.0) && (res0 <= 1.0 && res0 >= -1.0);

    const vx_uint16 int_subpixel_mul = static_cast<vx_uint16>(1) << (FRACTION_BITS + OCTAVE);

    const vx_int16 subpixel_x = static_cast<vx_uint16>(res0 * int_subpixel_mul);
    const vx_int16 subpixel_y = static_cast<vx_uint16>(res1 * int_subpixel_mul);

#else

    const vx_ap_int<RES1_NUM> res1_num = Dx * Dxy - Dy * Dxx;
    const vx_ap_int<RES1_DEN> res1_den = Dyy * Dxx - Dxy * Dxy;

    /*
     * shift back to the level of the input data type 8 Bit or 16 bit
     * res1_num_norm & res1_num_norm: 2|B or -2|B
     * +1 bit because of sign
     */
    const vx_ap_int<RES1_NUM_NORM> res1_num_norm = res1_num.template shift_to<RES1_NUM_NORM>();
    const vx_ap_int<RES1_DEN_NORM> res1_den_norm = res1_den.template shift_to<RES1_DEN_NORM>();

    /*
     * -Dx - Dxy * res1 = -Dx - Dxy * res1_num  = -Dx * res1_den - Dxy * res1_num  = -(Dx * res1_den + Dxy * res1_num)
     * ----------------   ---   --------------	 --------------	  --------------   --------------------------------
     * 		 Dxx		  Dxx   Dxx * res1_den	 Dxx * res1_den   Dxx * res1_den			Dxx *
     * res1_den
     *
     * 	Because res1_num and res1_den are 16 or 32 bit use shifted back version.
     * 	Shift also back to the level of the input data type.
     *
     * 	res0_num: 0|B * (+/-2|B) + 0|B * (+/-2|B) = (+/-2|2*B) + (+/-2|2*B) = 3|2*B or -3|2*B
     * 	res0_den: (+/-2|B) * (+/-2|B) = 4|2*B or -4|2*B
     * 	+1 bit because of sign
     */
    const vx_ap_int<RES0_NUM> res0_num = -(Dx * res1_den_norm + Dxy * res1_num_norm);
    const vx_ap_int<RES0_DEN> res0_den = Dxx * res1_den_norm;

    const vx_ap_uint<ABS_RES0_NUM> abs_res0_num =
        vx_ap_uint<ABS_RES0_NUM>(abs_int(res0_num.template shift_to<ABS_RES0_NUM + 1>().value));
    vx_ap_uint<ABS_RES0_DEN> abs_res0_den =
        vx_ap_uint<ABS_RES0_DEN>(abs_int(res0_den.template shift_to<ABS_RES0_DEN + 1>().value));

    const vx_ap_uint<ABS_RES1_NUM> abs_res1_num =
        vx_ap_uint<ABS_RES1_NUM>(abs_int(res1_num.template shift_to<ABS_RES1_NUM + 1>().value));
    vx_ap_uint<ABS_RES1_DEN> abs_res1_den =
        vx_ap_uint<ABS_RES1_DEN>(abs_int(res1_den.template shift_to<ABS_RES1_DEN + 1>().value));

    // save sign of division
    const vx_bool res0_sign = (res0_num < 0) ^ (res0_den < 0);
    const vx_bool res1_sign = (res1_num < 0) ^ (res1_den < 0);

    const vx_bool keypoint_detected = (abs_res1_num <= abs_res1_den) && (abs_res0_num <= abs_res0_den);

    const vx_uint16 abs_subpixel_x =
        FastDivision<vx_ap_uint<ABS_RES0_NUM>, vx_ap_uint<ABS_RES0_DEN>, vx_uint16, PRECISION>(abs_res0_num,
                                                                                               abs_res0_den);
    const vx_uint16 abs_subpixel_y =
        FastDivision<vx_ap_uint<ABS_RES1_NUM>, vx_ap_uint<ABS_RES1_DEN>, vx_uint16, PRECISION>(abs_res1_num,
                                                                                               abs_res1_den);

    const vx_int16 subpixel_x = (res0_sign) ? (-abs_subpixel_x) : (abs_subpixel_x);
    const vx_int16 subpixel_y = (res1_sign) ? (-abs_subpixel_y) : (abs_subpixel_y);

#endif // USE_FLOAT

    /*
     * Because the findExtrema function shifts the integer part to the highest bits and
     * add or subtract subpixel fraction (considering also the UP_SCALE_FACTOR).
     * E.g.
     * 	4K image (12 integer and 4 fraction bits), UP_SCALE_FACTOR = 4 and x = 1017 (11 1111 1001)
     * 	subpixel s has 4 (fraction bits) + 2 bits (UP_SCALE_FACTOR/2) and has the value 0.671875 (43 or 10 1011)
     *
     *	(x + s) * 4 = 1017.671875 = 4070.6875 (1111 1110 0110.1011)
     *	is equivalent to x << (4 + 2) + s = 1017 << (4 + 2) + 43
     *	11 1111 1001 << (4 + 2) + 10 1011 = 1111 1110 0100.0000 + 10 1011 = 1111 1110 0110.1011
     *
     */
    // can overwrite keypoint because it is discarded anyway if keypoint_detected is false
    keypoint =
        createKeypoint(static_cast<vx_uint16>(keypoint.x + subpixel_x), static_cast<vx_uint16>(keypoint.y + subpixel_y),
                       keypoint.response, keypoint.scale, keypoint.orientation);

    return keypoint_detected;
}

/**********************************************************************************************************************/
/**
 * @brief Approximates division by using bitshifting, subtraction and comparision
 * @tparam InTypeNum Data type of the numerator
 * @tparam InTypeDen Data type of the denominator
 * @tparam PRECISION Number of bits for the division result
 * @tparam OutType Data type of the result
 * @param num Numerator
 * @param den Denominator
 * @return Result of division
 */
template <typename InTypeNum, typename InTypeDen, typename OutType, vx_uint8 PRECISION>
OutType FastDivision(const InTypeNum num, const InTypeDen den) {
#pragma HLS INLINE

    OutType res = 0;

    InTypeNum temp_num = num;

    for (vx_uint16 i = 0; i < PRECISION + 1; i++) {
#pragma HLS unroll

        const vx_uint16 bit_shift = PRECISION - i;

        const InTypeDen temp_den = den >> i;

        const vx_bool comp_res = temp_num >= temp_den && temp_den > 0;

        temp_num -= (comp_res) ? (temp_den) : (0);
        res |= (comp_res) ? (1 << bit_shift) : (0);
    }

    return res;
}

/**********************************************************************************************************************/
/**
 * @brief Finds extrema with optional subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam KernType Data type of the kernel
 * @tparam KERN_SIZE Size of NMS window
 * @tparam THRESHOLD Threshold for NMS
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam BORDER_WIDTH Number of border pixels
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @param input Response values
 * @param kernel kernel window for NMS
 * @param keypoint (Detected) Keypoint
 * @param x Source x coordinate
 * @param y Source y coordinate
 * @return True if keypoint was detected else false
 */
template <typename FeatureType, typename ResponseType, typename OrientationType, typename MaskType, vx_uint16 KERN_SIZE,
          ResponseType THRESHOLD, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 BORDER_WIDTH,
          vx_bool WITH_ORIENTATION, vx_bool WITH_NON_MAX_SUPPRESSION>
vx_bool FindExtrema(ResponseType input_response[KERN_SIZE][KERN_SIZE],
                    OrientationType input_orientation[KERN_SIZE][KERN_SIZE], FeatureType &feature,
                    MaskType mask[KERN_SIZE][KERN_SIZE], const vx_uint16 x, const vx_uint16 y,
                    FeatureType (*create_feature)(vx_uint16, vx_uint16, ResponseType, OrientationType)) {
#pragma HLS INLINE

    const vx_uint8 KERN_RAD = KERN_SIZE >> 1;

    const OrientationType NO_ORIENTATION = 0;

    const ResponseType center_response = input_response[KERN_RAD][KERN_RAD];
    const OrientationType center_orientation =
        (WITH_ORIENTATION) ? (input_orientation[KERN_RAD][KERN_RAD]) : (NO_ORIENTATION);

    const vx_bool feature_detected =
        (x >= BORDER_WIDTH) && (y >= BORDER_WIDTH) && (x < IMG_COLS - BORDER_WIDTH) && (y < IMG_ROWS - BORDER_WIDTH) &&
        (center_response > THRESHOLD) &&
        ((!WITH_NON_MAX_SUPPRESSION) ||
         (center_response == NonMaxSuppression<ResponseType, ResponseType, MaskType, KERN_SIZE>(mask, input_response)));

    // bit shift the response value if of type vx_int32 to a 16 bit value
    feature = create_feature(x, y, center_response, center_orientation);

    return feature_detected;
}

/**********************************************************************************************************************/
/**
 * @brief Finds extrema with optional subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam KernType Data type of the kernel
 * @tparam KERN_SIZE Size of NMS window
 * @tparam THRESHOLD Threshold for NMS
 * @tparam MAX_KEYPOINTS Maximum number of keypoints to retrieve
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam BORDER_WIDTH Number of border pixels
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @tparam WITH_SUBPIXEL_REFINEMENT True if subpixel refinement shall be applied else false
 * @param input Response values
 * @param kernel kernel window for NMS
 * @param keypoint (Detected) Keypoint
 * @param x Source x coordinate
 * @param y Source y coordinate
 * @return True if keypoint was detected else false
 */
template <typename FeatureType, typename ResponseType, typename OrientationType, typename MaskType, vx_uint16 KERN_SIZE,
          ResponseType THRESHOLD, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 BORDER_WIDTH,
          vx_bool WITH_SUBPIXEL_REFINEMENT, vx_bool WITH_ORIENTATION, vx_bool WITH_NON_MAX_SUPPRESSION>
vx_bool FeatureExtraction(ResponseType input_response[KERN_SIZE][KERN_SIZE],
                          OrientationType input_orientation[KERN_SIZE][KERN_SIZE], FeatureType &feature,
                          MaskType mask[KERN_SIZE][KERN_SIZE], const vx_uint16 x, const vx_uint16 y,
                          FeatureType (*create_feature)(vx_uint16, vx_uint16, ResponseType, OrientationType),
                          vx_bool (*subpixel_refinement)(ResponseType[KERN_SIZE][KERN_SIZE], FeatureType &)) {
#pragma HLS INLINE

    vx_bool feature_detected =
        FindExtrema<FeatureType, ResponseType, OrientationType, MaskType, KERN_SIZE, THRESHOLD, IMG_COLS, IMG_ROWS,
                    BORDER_WIDTH, WITH_ORIENTATION, WITH_NON_MAX_SUPPRESSION>(input_response, input_orientation,
                                                                              feature, mask, x, y, create_feature);

    if (WITH_SUBPIXEL_REFINEMENT) {
        feature_detected &= subpixel_refinement(input_response, feature);
    }

    return feature_detected;
}

/**********************************************************************************************************************/
/**
 * @brief Searches for extreme values in a window and applies a optional subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam KernType Data type of the kernel
 * @tparam BufferType Data type Data type of the linebuffer
 * @tparam BUFFER_NUM Number of buffers
 * @tparam VEC_PIX The amount of pixels diveded by the pixels computed in parallel
 * @tparam VEC_NUM Amount of pixels computed in parallel (1, 2, 4, 8) //currently only one supported
 * @tparam IMG_COLS Image width
 * @tparam IMG_ROWS Image height
 * @tparam KERN_SIZE Size of NMS window
 * @tparam THRESHOLD Threshold for NMS
 * @tparam MAX_KEYPOINTS Maximum number of keypoints to retrieve
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam BORDER_WIDTH Number of border pixels
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @tparam WITH_SUBPIXEL_REFINEMENT True if subpixel refinement shall be applied else false
 * @param input Matrix response values
 * @param kernel kernel window for NMS
 * @param output Keypoints
 */
template <typename FeatureType, typename ResponseType, typename OrientationType, typename MaskType,
          typename ResponseBufferType, typename OrientationBufferType, vx_uint16 RESPONSE_BUFFER_NUM,
          vx_uint16 ORIENTATION_BUFFER_NUM, vx_uint32 VEC_PIX, vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT,
          vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS,
          vx_uint16 BORDER_WIDTH, vx_bool WITH_SUBPIXEL_REFINEMENT, vx_bool WITH_ORIENTATION,
          vx_bool WITH_NON_MAX_SUPPRESSION>
void ComputeFeatureFunc(vx_image_data<ResponseType, VEC_NUM_IN> input_response[VEC_PIX],
                        vx_image_data<OrientationType, VEC_NUM_IN> input_orientation[VEC_PIX],
                        vx_image_data<FeatureType, VEC_NUM_OUT> output[MAX_KEYPOINTS],
                        MaskType mask[KERN_SIZE][KERN_SIZE],
                        FeatureType (*create_feature)(vx_uint16, vx_uint16, ResponseType, OrientationType),
                        vx_bool (*subpixel_refinement)(ResponseType[KERN_SIZE][KERN_SIZE], FeatureType &),
                        const FeatureType invalid_feature) {
#pragma HLS INLINE

    // Use only unsigned data types for buffers, because linebuffer produces errors with signed data types.
    typedef typename to_unsigned<ResponseType>::DataType responseBufT;
    typedef OrientationType orientationBufT;

    // compute the number of bits shifts to be performed to correct the input coordinates of x because of vectorization
    const vx_uint8 BIT_SHIFT_COORDINATE = comp_log2<VEC_NUM_IN>::log2_ceil;

    // Constants
    const vx_uint16 VEC_COLS = IMG_COLS / VEC_NUM_IN;
    const vx_uint16 KERN_RAD = KERN_SIZE >> 1;
    const vx_uint16 WIN_COLS = (KERN_RAD << 1) + VEC_NUM_IN + (VEC_NUM_IN - (KERN_RAD % VEC_NUM_IN)) % VEC_NUM_IN;
    const vx_uint16 OHD_COLS = (WIN_COLS - KERN_RAD) / VEC_NUM_IN - 1;
    const vx_uint16 LINE_BUFFER_IMG_COLS = VEC_NUM_IN * (KERN_SIZE - 1);

    // define how many input vector entries are considered for one output vector entry (at most one of the entries is
    // written to the output)
    const vx_uint16 GROUP_SIZE = (VEC_NUM_IN / VEC_NUM_OUT) + ((VEC_NUM_IN % VEC_NUM_OUT) > 0);

    // Linebuffer
    ResponseBufferType linebuffer_response[RESPONSE_BUFFER_NUM][VEC_COLS];
    OrientationBufferType linebuffer_orientation[ORIENTATION_BUFFER_NUM][VEC_COLS];

    // Sliding window for complete vector
    responseBufT vec_window_response[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable = vec_window_response complete dim = 0
    orientationBufT vec_window_orientation[KERN_SIZE][WIN_COLS];
#pragma HLS array_partition variable = vec_window_orientation complete dim = 0

    vx_uint32 ptr_src = 0;
    vx_uint32 ptr_dst = 0;
    vx_uint32 nr_kp = 0;

    // Compute the filter (pipelined)
    for (vx_uint16 y = 0; y < IMG_ROWS + KERN_RAD; ++y) {

        for (vx_uint16 x = 0; x < VEC_COLS + OHD_COLS; ++x) {
#pragma HLS PIPELINE II = 1

            // compute the original image coordinate in respect to y for the center element of the NMS window
            const vx_uint16 org_y = y - KERN_RAD;

            responseBufT input_buffer_response[VEC_NUM_IN];
#pragma HLS array_partition variable = input_buffer_response complete dim = 0
            orientationBufT input_buffer_orientation[VEC_NUM_IN];
#pragma HLS array_partition variable = input_buffer_orientation complete dim = 0

            responseBufT buffer_response[KERN_SIZE][VEC_NUM_IN];
#pragma HLS array_partition variable = buffer_response complete dim = 0
            orientationBufT buffer_orientation[KERN_SIZE][VEC_NUM_IN];
#pragma HLS array_partition variable = buffer_orientation complete dim = 0

            // Input & Output
            vx_image_data<ResponseType, VEC_NUM_IN> input_data_response;
            vx_image_data<OrientationType, VEC_NUM_IN> input_data_orientation;
            vx_image_data<KeyPoint, VEC_NUM_OUT> output_data;

            vx_bool write_out = vx_false_e;

            // Read input data from global memory
            if ((y < IMG_ROWS) && (x < VEC_COLS)) {
                input_data_response = input_response[ptr_src];
                if (WITH_ORIENTATION) {
                    input_data_orientation = input_orientation[ptr_src];
                }
                ++ptr_src;
            }

            for (vx_uint16 v = 0; v < VEC_NUM_IN; v++) {
#pragma HLS unroll
                input_buffer_response[v] = static_cast<responseBufT>(input_data_response.pixel[v]);
                if (WITH_ORIENTATION) {
                    input_buffer_orientation[v] = static_cast<orientationBufT>(input_data_orientation.pixel[v]);
                }
            }

            // Read data from line_buffer to buffer
            ReadFromLineBuffer<responseBufT, ResponseBufferType, RESPONSE_BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(
                input_buffer_response, linebuffer_response, buffer_response, x);

            // Write data from buffer to line_buffer
            WriteToLineBuffer<responseBufT, ResponseBufferType, RESPONSE_BUFFER_NUM, VEC_NUM_IN, KERN_SIZE, VEC_COLS>(
                buffer_response, linebuffer_response, x);

            // Move sliding window with replicated/constant border
            SlidingWindow<responseBufT, VEC_NUM_IN, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE,
                          VX_BORDER_UNDEFINED>(
                buffer_response, vec_window_response, x,
                y); // currently just supports no border handling by default but may extended if required

            if (WITH_ORIENTATION) {
                ReadFromLineBuffer<orientationBufT, OrientationBufferType, ORIENTATION_BUFFER_NUM, VEC_NUM_IN,
                                   KERN_SIZE, VEC_COLS>(input_buffer_orientation, linebuffer_orientation,
                                                        buffer_orientation, x);
                WriteToLineBuffer<orientationBufT, OrientationBufferType, ORIENTATION_BUFFER_NUM, VEC_NUM_IN, KERN_SIZE,
                                  VEC_COLS>(buffer_orientation, linebuffer_orientation, x);
                SlidingWindow<orientationBufT, VEC_NUM_IN, IMG_ROWS, KERN_RAD, VEC_COLS, WIN_COLS, KERN_SIZE,
                              VX_BORDER_UNDEFINED>(
                    buffer_orientation, vec_window_orientation, x,
                    y); // currently just supports no border handling by default but may extended if required
            }

            // Interpret the data using the original input data type.
            ResponseType(&signed_vec_window_response)[KERN_SIZE][WIN_COLS] =
                *reinterpret_cast<ResponseType(*)[KERN_SIZE][WIN_COLS]>(&vec_window_response);

            FeatureType features[VEC_NUM_IN];
            FeatureType output_features[VEC_NUM_OUT];
            vx_bool features_detected[VEC_NUM_OUT];

            // initialize the output keypoint with invalid data
            for (vx_uint16 vecId = 0; vecId < VEC_NUM_OUT; vecId++) {
#pragma HLS unroll
                features_detected[vecId] = vx_false_e;
                output_features[vecId] = invalid_feature;
            }

            // compare VEC_NUM_IN kernel windows in parallel
            for (vx_uint16 vecId = 0; vecId < VEC_NUM_IN; vecId++) {
#pragma HLS unroll

                // Window for single vector element
                ResponseType window_response[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = window_response complete dim = 0
                OrientationType window_orientation[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = window_orientation complete dim = 0

                // Get window for single vector element
                for (vx_uint16 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
                    for (vx_uint16 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
                        window_response[i][j] = signed_vec_window_response[i][j + vecId];
                        window_orientation[i][j] = vec_window_orientation[i][j + vecId];
                    }
                }

                // compute the original image coordinate in respect to x for the center element of the NMS window
                const vx_uint16 org_x = ((x - KERN_RAD) << BIT_SHIFT_COORDINATE) + vecId;

                const vx_bool valid_keypoint =
                    (nr_kp < MAX_KEYPOINTS) &
                    FeatureExtraction<FeatureType, ResponseType, OrientationType, MaskType, KERN_SIZE, THRESHOLD,
                                      IMG_COLS, IMG_ROWS, BORDER_WIDTH, WITH_SUBPIXEL_REFINEMENT, WITH_ORIENTATION,
                                      WITH_NON_MAX_SUPPRESSION>(window_response, window_orientation, features[vecId],
                                                                mask, org_x, org_y, create_feature,
                                                                subpixel_refinement);

                const vx_uint16 vecOutId = vecId / GROUP_SIZE;
                // at most one keypoint is written to each output vector entry
                if (valid_keypoint && (nr_kp < MAX_KEYPOINTS)) {
                    features_detected[vecOutId] = vx_true_e;
                    output_features[vecOutId] = features[vecId];
                    // if at least one keypoint was detected write the whole vector out, else the vector contains no
                    // keypoint and is discarded
                    write_out = vx_true_e;
                }
            }

            for (vx_uint16 vecOutId = 0; vecOutId < VEC_NUM_OUT; vecOutId++) {
#pragma HLS unroll
                output_data.pixel[vecOutId] = output_features[vecOutId];
                nr_kp += features_detected[vecOutId];
            }

            // Write keypoint vector out if the vector contains at least one keypoint, also write until the maximum
            // number of keypoints has been reached
            if ((y >= KERN_RAD) && (x >= OHD_COLS) && write_out) {
                GenerateDmaSignal<KeyPoint, VEC_NUM_OUT>((ptr_dst == 0), (ptr_dst == MAX_KEYPOINTS - 1), output_data);
                output[ptr_dst] = output_data;
                ++ptr_dst;

            } else if ((((y == (IMG_ROWS + KERN_RAD - 1)) && (x == (VEC_COLS + OHD_COLS - 1))) ||
                        (nr_kp == MAX_KEYPOINTS)) &&
                       (ptr_dst < MAX_KEYPOINTS)) {

                for (vx_uint16 vecId = 0; vecId < VEC_NUM_OUT; vecId++) {
#pragma HLS unroll
                    output_data.pixel[vecId] = invalid_feature;
                }

                // In each loop output_data is initialize with invalid keypoints.
                GenerateDmaSignal<KeyPoint, VEC_NUM_OUT>((ptr_dst == 0), vx_true_e, output_data);
                output[ptr_dst] = output_data;
                ++ptr_dst;
            }
        }
    }
}

/**********************************************************************************************************************/
/**
 * @brief Searches for extreme values in a window and applies a optional subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam KernType Data type of the kernel
 * @tparam VEC_PIX The amount of pixels diveded by the pixels computed in parallel
 * @tparam VEC_NUM Amount of pixels computed in parallel (1, 2, 4, 8) //currently only one supported
 * @tparam IMG_COLS Image width
 * @tparam IMG_ROWS Image height
 * @tparam KERN_SIZE Size of NMS window
 * @tparam THRESHOLD Threshold for NMS
 * @tparam MAX_KEYPOINTS Maximum number of keypoints to retrieve
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam BORDER_WIDTH Number of border pixels
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @tparam WITH_SUBPIXEL_REFINEMENT True if subpixel refinement shall be applied else false
 * @param input Matrix response values
 * @param kernel kernel window for NMS
 * @param output Keypoints
 */
template <typename FeatureType, typename ResponseType, typename OrientationType, typename MaskType, vx_uint32 VEC_PIX,
          vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint16 KERN_SIZE,
          ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS, vx_uint16 BORDER_WIDTH, vx_bool WITH_SUBPIXEL_REFINEMENT,
          vx_bool WITH_ORIENTATION, vx_bool WITH_NON_MAX_SUPPRESSION>
void ComputeFeature(vx_image_data<ResponseType, VEC_NUM_IN> input_response[VEC_PIX],
                    vx_image_data<OrientationType, VEC_NUM_IN> input_orientation[VEC_PIX],
                    vx_image_data<FeatureType, VEC_NUM_OUT> output[MAX_KEYPOINTS], MaskType mask[KERN_SIZE][KERN_SIZE],
                    FeatureType (*create_feature)(vx_uint16, vx_uint16, ResponseType, OrientationType),
                    vx_bool (*subpixel_refinement)(ResponseType[KERN_SIZE][KERN_SIZE], FeatureType &),
                    const FeatureType invalid_feature) {
#pragma HLS INLINE

    // Constants to compute optimum buffer IMG_COLS
    const vx_uint16 RESPONSE_DATA_IMG_COLS = sizeof(ResponseType) * VEC_NUM_IN * (KERN_SIZE - 1);
    const vx_uint16 RESPONSE_BUFFER_UINT32 = RESPONSE_DATA_IMG_COLS % 4;
    const vx_uint16 RESPONSE_BUFFER_UINT16 = RESPONSE_DATA_IMG_COLS % 2;
    const vx_uint16 RESPONSE_BUFFER_NUM =
        (RESPONSE_BUFFER_UINT32 == 0)
            ? (RESPONSE_DATA_IMG_COLS >> 2)
            : ((RESPONSE_BUFFER_UINT16 == 0) ? (RESPONSE_DATA_IMG_COLS >> 1) : (RESPONSE_DATA_IMG_COLS));
    const vx_uint16 RESPONSE_BUFFER_IMG_COLS =
        (RESPONSE_BUFFER_UINT32 == 0) ? (4) : ((RESPONSE_BUFFER_UINT16 == 0) ? (2) : (1));

    const vx_uint16 ORIENTATION_DATA_IMG_COLS = sizeof(OrientationType) * VEC_NUM_IN * (KERN_SIZE - 1);
    const vx_uint16 ORIENTATION_BUFFER_UINT32 = ORIENTATION_DATA_IMG_COLS % 4;
    const vx_uint16 ORIENTATION_BUFFER_UINT16 = ORIENTATION_DATA_IMG_COLS % 2;
    const vx_uint16 ORIENTATION_BUFFER_NUM =
        (ORIENTATION_BUFFER_UINT32 == 0)
            ? (ORIENTATION_DATA_IMG_COLS >> 2)
            : ((ORIENTATION_BUFFER_UINT16 == 0) ? (ORIENTATION_DATA_IMG_COLS >> 1) : (ORIENTATION_DATA_IMG_COLS));
    const vx_uint16 ORIENTATION_BUFFER_IMG_COLS =
        (RESPONSE_BUFFER_UINT32 == 0) ? (4) : ((RESPONSE_BUFFER_UINT16 == 0) ? (2) : (1));

    typedef typename select_buffer_data_type<RESPONSE_BUFFER_IMG_COLS>::DataType response_buffer_data_type;
    typedef typename select_buffer_data_type<ORIENTATION_BUFFER_IMG_COLS>::DataType orientation_buffer_data_type;

    // Sets the Buffer Data type to the maximum possible, to reduce the amount of BRAM used
    ComputeFeatureFunc<FeatureType, ResponseType, OrientationType, MaskType, response_buffer_data_type,
                       orientation_buffer_data_type, RESPONSE_BUFFER_NUM, ORIENTATION_BUFFER_NUM, VEC_PIX, VEC_NUM_IN,
                       VEC_NUM_OUT, IMG_COLS, IMG_ROWS, KERN_SIZE, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH,
                       WITH_SUBPIXEL_REFINEMENT, WITH_ORIENTATION, WITH_NON_MAX_SUPPRESSION>(
        input_response, input_orientation, output, mask, create_feature, subpixel_refinement, invalid_feature);
}

/**********************************************************************************************************************/
/**
 * @brief Creates a cyclic mask using 1 for ignore value and 0 to consider value of the window were the mask is applied
 * on.
 * @tparam MaskType Data type of the mask
 * @tparam MASK_SIZE Number of elements in one dimension of the quadric mask, resulting in MASK_SIZE * MASK_SIZE
 * elements in total.
 * @tparam RADIUS_WITH_FRACTION Radius which is used to determine if an element is to be considered inside of the circle
 * or outside (if the distance exceeds at least 0.5 of a pixel this pixel is also considered to be part of the circle).
 * The 16 most significant bits are used as integer part and the 16 least significant bits are used to represent the
 * fractional part.
 * @param mask Created cyclic mask for given radius.
 */
template <typename MaskType, vx_uint8 MASK_SIZE, vx_uint32 RADIUS_WITH_FRACTION>
void CreateCircleMask(MaskType mask[MASK_SIZE][MASK_SIZE]) {
#pragma HLS INLINE

    const vx_uint8 MASK_RAD = (MASK_SIZE >> 1);

    const vx_uint8 MASK_IGNORE = 1;
    const vx_uint8 MASK_CONSIDER = 0;

    const vx_uint8 FRACTION_BITS = 16;

    const vx_uint64 RADIUS_SQUARE_WITH_FRACTION = static_cast<vx_uint64>(RADIUS_WITH_FRACTION) * RADIUS_WITH_FRACTION;
    const vx_uint32 ROUNDING = (0x1 << (2 * FRACTION_BITS - 1));
    const vx_uint32 RADIUS_SQUARE = (RADIUS_SQUARE_WITH_FRACTION + ROUNDING) >> (2 * FRACTION_BITS);

    mask[MASK_RAD][MASK_RAD] = MASK_CONSIDER;

    for (vx_uint8 i = 1; i <= MASK_RAD; i++) {
#pragma HLS UNROLL

        const vx_uint32 si = i * i;

        const MaskType cross_value = (si <= RADIUS_SQUARE) ? (MASK_CONSIDER) : (MASK_IGNORE);

        // the cross pattern (vertical and horizontal)

        // left
        mask[MASK_RAD][MASK_RAD - i] = cross_value;
        // right
        mask[MASK_RAD][MASK_RAD + i] = cross_value;
        // top
        mask[MASK_RAD - i][MASK_RAD] = cross_value;
        // bottom
        mask[MASK_RAD + i][MASK_RAD] = cross_value;

        // the diagonal pattern
        for (vx_uint8 j = 1; j <= MASK_RAD; j++) {
#pragma HLS UNROLL
            const vx_uint16 sd = si + j * j;

            const MaskType diag_value = (sd <= RADIUS_SQUARE) ? (MASK_CONSIDER) : (MASK_IGNORE);

            // top left
            mask[MASK_RAD - j][MASK_RAD - i] = diag_value;
            // top right
            mask[MASK_RAD - j][MASK_RAD + i] = diag_value;
            // bottom left
            mask[MASK_RAD + j][MASK_RAD - i] = diag_value;
            // bottom right
            mask[MASK_RAD + j][MASK_RAD + i] = diag_value;
        }
    }
}

/**********************************************************************************************************************/
/**
 * @brief Searches for extreme values in a window and applies a optional subpixel refinement
 * @tparam InType Data type of the input image (16-bit, 32-bit)
 * @tparam VEC_PIX The amount of pixels diveded by the pixels computed in parallel
 * @tparam VEC_NUM Amount of pixels computed in parallel (1, 2, 4, 8) //currently only one supported
 * @tparam IMG_COLS Image width
 * @tparam IMG_ROWS Image height
 * @tparam KERN_SIZE Size of NMS window
 * @tparam THRESHOLD Threshold for NMS
 * @tparam MAX_KEYPOINTS Maximum number of keypoints to retrieve
 * @tparam LEVEL Evolution level
 * @tparam NUMBER_OF_SUBLEVELS Number of sublevels per octave
 * @tparam BORDER_WIDTH Number of border pixels
 * @tparam MAX_SIDE_LENGTH Maxiumum side length of the input image
 * @tparam WITH_SUBPIXEL_REFINEMENT True if subpixel refinement shall be applied else false
 * @param input Matrix response values
 * @param output Keypoints
 */
template <typename FeatureType, typename ResponseType, typename OrientationType, typename MaskType, vx_uint32 VEC_PIX,
          vx_uint8 VEC_NUM_IN, vx_uint8 VEC_NUM_OUT, vx_uint16 IMG_COLS, vx_uint16 IMG_ROWS, vx_uint8 KERN_SIZE,
          vx_uint32 RADIUS_WITH_FRACTION, ResponseType THRESHOLD, vx_uint16 MAX_KEYPOINTS, vx_uint16 BORDER_WIDTH,
          HIFLIPVX::NonMaxSuppressionMask MASK_SHAPE, vx_bool WITH_SUBPIXEL_REFINEMENT, vx_bool WITH_ORIENTATION>
void HwFeatureExtraction(vx_image_data<ResponseType, VEC_NUM_IN> input_response[VEC_PIX],
                         vx_image_data<OrientationType, VEC_NUM_IN> input_orientation[VEC_PIX],
                         vx_image_data<FeatureType, VEC_NUM_OUT> output[MAX_KEYPOINTS],
                         const MaskType user_defined_mask[KERN_SIZE][KERN_SIZE],
                         FeatureType (*create_feature)(vx_uint16, vx_uint16, ResponseType, OrientationType),
                         vx_bool (*subpixel_refinement)(ResponseType[KERN_SIZE][KERN_SIZE], FeatureType &),
                         const FeatureType invalid_feature) {
#pragma HLS INLINE

    // zero means to consider an entry in the mask
    const vx_uint8 MASK_CONSIDER_ENTRY = 0;

    const vx_uint8 MINIMUM_KERNEL_SIZE = MINIMUM_KERNEL_SIZE_FOR_RADIUS(RADIUS_WITH_FRACTION, 16, vx_false_e);

    // Initialize kernel with 0 for NMS
    const vx_bool allowed_kernel_size =
        (KERN_SIZE == 3) || (KERN_SIZE == 5) || (KERN_SIZE == 7) || (KERN_SIZE == 9) || (KERN_SIZE == 11);

    // check if mask is valid
    const vx_bool valid_circle_mask = ((MASK_SHAPE == HIFLIPVX::CIRCLE) && (MINIMUM_KERNEL_SIZE <= KERN_SIZE));
    const vx_bool no_nms = (MASK_SHAPE == HIFLIPVX::NO_NMS);
    const vx_bool valid_mask = ((MASK_SHAPE == HIFLIPVX::NO_NMS) || (MASK_SHAPE == HIFLIPVX::USER_DEFINED) ||
                                (MASK_SHAPE == HIFLIPVX::SQUARE) || (valid_circle_mask));

    STATIC_ASSERT(BORDER_WIDTH > 0, border_width_has_to_be_greater_than_zero);
    STATIC_ASSERT(BORDER_WIDTH >= (KERN_SIZE >> 1), border_width_has_to_be_greater_or_equal_the_kernel_radius);
    STATIC_ASSERT(IMG_COLS / 2 > BORDER_WIDTH, border_width_has_to_be_smaller_than_the_image_width);
    STATIC_ASSERT(IMG_ROWS / 2 > BORDER_WIDTH, border_width_has_to_be_smaller_than_the_image_height);
    STATIC_ASSERT(allowed_kernel_size, kernel_size_must_be_3_5_7_9_11);
    STATIC_ASSERT(valid_mask, mask_is_not_valid);

    MaskType nms_mask[KERN_SIZE][KERN_SIZE];

    if (MASK_SHAPE == HIFLIPVX::CIRCLE) {
        CreateCircleMask<MaskType, KERN_SIZE, RADIUS_WITH_FRACTION>(nms_mask);
    } else {
        for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS UNROLL
            for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS UNROLL
                // if the mask is not user defined use a square mask as default case
                nms_mask[i][j] =
                    (MASK_SHAPE == HIFLIPVX::USER_DEFINED) ? (user_defined_mask[i][j]) : (MASK_CONSIDER_ENTRY);
            }
        }
    }

    ComputeFeature<FeatureType, ResponseType, OrientationType, MaskType, VEC_PIX, VEC_NUM_IN, VEC_NUM_OUT, IMG_COLS,
                   IMG_ROWS, KERN_SIZE, THRESHOLD, MAX_KEYPOINTS, BORDER_WIDTH, WITH_SUBPIXEL_REFINEMENT,
                   WITH_ORIENTATION, (!no_nms)>(input_response, input_orientation, output, nms_mask, create_feature,
                                                subpixel_refinement, invalid_feature);
}

#endif /*__SRC_IMG_MT_FEATURE_EXTRACT_H__*/
