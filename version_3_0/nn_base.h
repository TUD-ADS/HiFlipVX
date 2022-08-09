/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_neural_network_base.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*
*/

#ifndef NN_BASE_H_
#define NN_BASE_H_

#include "nn_core1.h"
#include "nn_core2.h"

/***************************************************************************************************************************************************/
/** @brief Do Point-Wise Image Convolution
@param TypeData             : uint8, int8, uin16, int16, float. Be carefull with uin32, int32
@param FIXED_POINT_POSITION : Positive number. It is maximum the number of digits of TypeData. Ignorred for float.
@param INPUT_HEIGHT         : (i > 0) || (i < 2048) || ((i % OUTPUT_HEIGHT) == 0) || (i >= OUTPUT_HEIGHT)
@param INPUT_WIDTH          : (i > 0) || (i < 2048) || ((i % OUTPUT_WIDTH) == 0) || (i >= OUTPUT_WIDTH)
@param IFM                  : (i > 0) || (i < 2048) || ((i % IFM_PARALLEL) == 0) || (i > IFM_PARALLEL)
@param OUTPUT_HEIGHT        : (i > 0) || (i < 2048)
@param OUTPUT_WIDTH         : (i > 0) || (i < 2048)
@param OFM                  : (i > 0) || (i < 2048) || ((i % IFM_PARALLEL) == 0) || (i > OFM_PARALLEL)
@param KERNEL_X             : (i > 0) || (i < 9) || ((i % 2) == 1)
@param KERNEL_Y             : (i > 0) || (i < 9) || ((i % 2) == 1)
@param BIAS_SIZE            : [0], [OFM] or [BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM]
@param BATCHES              : (i > 0) || (i < 2048)
@param IFM_PARALLEL         : (i > 0) || (i < 128)
@param OFM_PARALLEL         : (i > 0) || (i < 128)
@param OVERFLOW_POLICY      : SATURATE or WRAP. Ignored for float.
@param ROUNDING_POLICY      : TO_NEAREST_EVEN or TO_ZERO. Ignored for float.
@param inputs               : The input images. [IFM, INPUT_WIDTH, INPUT_HEIGHT, BATCHES]
@param weights              : The weight matrix. [KERNEL_X, KERNEL_Y, IFM, OFM]
@param biases               : The biases. null [0], shared [OFM] or unshared [OFM, OUTPUT_WIDTH, OUTPUT_HEIGHT, BATCHES]
@param outputs              : The output images. [OFM, OUTPUT_WIDTH, OUTPUT_HEIGHT, BATCHES]
*/
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, 
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	vx_image_data<TypeData, IFM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL],
	vx_image_data<TypeData, OFM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(inputs, weights, biases, outputs);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, 
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeData biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, weights, (vx_image_data<TypeData, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL,
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	TypeData weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeData biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, (vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL>*)weights, 
		(vx_image_data<TypeData, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, 
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL],
	vx_image_data<TypeData, OFM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, weights, biases, outputs);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, 
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	vx_image_data<TypeData, IFM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeData biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		inputs, weights, (vx_image_data<TypeData, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}

/***************************************************************************************************************************************************/
// Variable bias
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	vx_image_data<TypeData, IFM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeBias, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL],
	vx_image_data<TypeData, OFM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(inputs, weights, biases, outputs);
}
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeBias biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, weights, (vx_image_data<TypeBias, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	TypeData weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeBias biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, (vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> *)weights, 
		(vx_image_data<TypeBias, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeBias, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL],
	vx_image_data<TypeData, OFM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, IFM_PARALLEL>*)inputs, weights, biases, outputs);
}
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvPointWise(
	vx_image_data<TypeData, IFM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	TypeBias biases[BIAS_SIZE / OFM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvPointWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
		inputs, weights, (vx_image_data<TypeBias, OFM_PARALLEL>*)biases, (vx_image_data<TypeData, OFM_PARALLEL>*)outputs);
}

/*************************************************************************************************************************************************************/
// Do Depth-Wise Image Convolution
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL,
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvDepthWise(
	vx_image_data<TypeData, FM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	vx_image_data<TypeData, FM_PARALLEL> biases[BIAS_SIZE / FM_PARALLEL],
	vx_image_data<TypeData, FM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvDepthWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(inputs, weights, biases, outputs);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL,
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvDepthWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	TypeData biases[BIAS_SIZE / FM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvDepthWiseTop<TypeData, TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y, BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
			(vx_image_data<TypeData, FM_PARALLEL> *)inputs, weights, (vx_image_data<TypeData, FM_PARALLEL>*)biases, (vx_image_data<TypeData, FM_PARALLEL>*)outputs);
}

/*************************************************************************************************************************************************************/
// Variable bias
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL,
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvDepthWise(
	vx_image_data<TypeData, FM_PARALLEL> inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	vx_image_data<TypeBias, FM_PARALLEL> biases[BIAS_SIZE / FM_PARALLEL],
	vx_image_data<TypeData, FM_PARALLEL> outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvDepthWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y,
		BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(inputs, weights, biases, outputs);
}
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL,
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ImgConvDepthWise(
	TypeData inputs[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	TypeBias biases[BIAS_SIZE / FM_PARALLEL],
	TypeData outputs[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=weights
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=outputs
	ConvDepthWiseTop<TypeData, TypeBias, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y, BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(
			(vx_image_data<TypeData, FM_PARALLEL> *)inputs, weights, (vx_image_data<TypeBias, FM_PARALLEL>*)biases, (vx_image_data<TypeData, FM_PARALLEL>*)outputs);
}

/*************************************************************************************************************************************************************/
// Pooling Layer
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM, 
	vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void ImgPooling(
	vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	Pooling<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, FM_PARALLEL, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X, 
		BATCHES, POOLING_TYPE>(src, dst);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM, 
	vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void ImgPooling(
	TypeData src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	TypeData dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	STATIC_ASSERT(FM_PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	Pooling<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, FM_PARALLEL, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X,
		BATCHES, POOLING_TYPE>(src, dst);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM, 
	vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void ImgPooling(
	TypeData src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	STATIC_ASSERT(FM_PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	Pooling<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, FM_PARALLEL, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X,
		BATCHES, POOLING_TYPE>(src, dst);
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM, 
	vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void ImgPooling(
	vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	TypeData dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	STATIC_ASSERT(FM_PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	Pooling<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, FM_PARALLEL, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X,
		BATCHES, POOLING_TYPE>(src, dst);
}

/*************************************************************************************************************************************************************/
// Batch Normalization Layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, 
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void ImgBatchNorm1(
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> batch_norm[IFM / PARALLEL],
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=batch_norm
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	BatchNormalization<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE, BUFFER_WEIGHTS>(
		batch_norm, src, dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, 
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void ImgBatchNorm1(
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> batch_norm[IFM / PARALLEL],
	TypeData src[(BATCHES * PIXELS * IFM) / PARALLEL],
	TypeData dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=batch_norm
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	STATIC_ASSERT(PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	BatchNormalization<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE, BUFFER_WEIGHTS>(
		batch_norm, src, dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION,
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void ImgBatchNorm2(
	vx_image_data<TypeData, PARALLEL * WEIGHT_SIZE> batch_norm[IFM / PARALLEL],
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=batch_norm
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	BatchNormalization2<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE, BUFFER_WEIGHTS>(
		batch_norm, src, dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION,
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
	void ImgBatchNorm2(
		vx_image_data<TypeData, PARALLEL* WEIGHT_SIZE> batch_norm[IFM / PARALLEL],
		TypeData src[(BATCHES * PIXELS * IFM) / PARALLEL],
		TypeData dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=batch_norm
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	BatchNormalization2<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE, BUFFER_WEIGHTS>(
		batch_norm, (vx_image_data<TypeData, PARALLEL>*)src, (vx_image_data<TypeData, PARALLEL>*)dst);
}


/*************************************************************************************************************************************************************/
// Activation Layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_nn_activation_function_e ACTIVATION_FUNCTION, bool RELAXED_MATH>
void ImgActivationLayer(
	TypeData param_a, 
	TypeData param_b,
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * PIXELS) / PARALLEL]) {
#pragma HLS INLINE
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst	
	ActivationLayer<TypeData, BATCHES, PIXELS, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(
		param_a, param_b, src,dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_nn_activation_function_e ACTIVATION_FUNCTION, bool RELAXED_MATH>
void ImgActivationLayer(
	TypeData param_a, 
	TypeData param_b,
	TypeData src[(BATCHES * PIXELS) / PARALLEL],
	TypeData dst[(BATCHES * PIXELS) / PARALLEL]) {
#pragma HLS INLINE
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst	
	STATIC_ASSERT(PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	ActivationLayer<TypeData, BATCHES, PIXELS, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(
		param_a, param_b, src, dst);
}

/*************************************************************************************************************************************************************/
// Softmay Layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY>
void ImgSoftmaxLayer(
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst		
	SoftmaxLayer<TypeData, BATCHES, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY>(src, dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY>
void ImgSoftmaxLayer(
	TypeData src[(BATCHES * IFM) / PARALLEL],
	TypeData dst[(BATCHES * IFM) / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst	
	STATIC_ASSERT(PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	SoftmaxLayer<TypeData, BATCHES, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY>(
		(vx_image_data<TypeData, PARALLEL> *)src, (vx_image_data<TypeData, PARALLEL>*)dst);
}

/*************************************************************************************************************************************************************/
// Fully connected layer (The weights matrix is read row by row)
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE, 
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL], 
	vx_image_data<TypeData, PARALLEL> weights[(BATCHES * IFM * OFM) / PARALLEL],
	vx_image_data<TypeData, 1> biases[OFM], 
	vx_image_data<TypeData, 1> dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	FullyConnected<TypeData, TypeData, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		src, weights, biases, dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
		TypeData src[(BATCHES * IFM) / PARALLEL],
		TypeData weights[(BATCHES * IFM * OFM) / PARALLEL],
		TypeData biases[OFM],
		TypeData dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	STATIC_ASSERT(PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	FullyConnected<TypeData, TypeData, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, PARALLEL> *)src, (vx_image_data<TypeData, PARALLEL>*)weights, 
		(vx_image_data<TypeData, 1> *)biases, (vx_image_data<TypeData, 1>*)dst);
}
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
		vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL],
		vx_image_data<TypeData, PARALLEL> weights[(BATCHES * IFM * OFM) / PARALLEL],
		TypeData biases[OFM],
		TypeData dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	FullyConnected<TypeData, TypeData, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		src, weights, (vx_image_data<TypeData, 1>*)biases, (vx_image_data<TypeData, 1>*)dst);
}

/*************************************************************************************************************************************************************/
// Fully connected layer (variable bias)
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL], 
	vx_image_data<TypeData, PARALLEL> weights[(BATCHES * IFM * OFM) / PARALLEL],
	vx_image_data<TypeBias, 1> biases[OFM],
	vx_image_data<TypeData, 1> dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	FullyConnected<TypeData, TypeBias, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		src, weights, biases, dst);
}
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
	TypeData src[(BATCHES * IFM) / PARALLEL],
	TypeData weights[(BATCHES * IFM * OFM) / PARALLEL],
	TypeBias biases[OFM],
	TypeData dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	STATIC_ASSERT(PARALLEL == 1, use__vx_image_data__type_if_parallelization_bigger_1);
	FullyConnected<TypeData, TypeBias, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		(vx_image_data<TypeData, PARALLEL> *)src, (vx_image_data<TypeData, PARALLEL>*)weights, 
		(vx_image_data<TypeBias, 1> *)biases, (vx_image_data<TypeData, 1>*)dst);
}
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_WEIGHTS>
void ImgFullyConnected(
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> weights[(BATCHES * IFM * OFM) / PARALLEL],
	TypeBias biases[OFM],
	TypeData dst[BATCHES * OFM]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=weights	
#pragma HLS data_pack variable=biases
#pragma HLS data_pack variable=dst	
	FullyConnected<TypeData, TypeBias, BATCHES, IFM, OFM, PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, BUFFER_WEIGHTS>(
		src, weights, (vx_image_data<TypeBias, 1>*)biases, (vx_image_data<TypeData, 1>*)dst);
}

/*************************************************************************************************************************************************************/
// Convert between vector sizes of a stream
template<typename TypeSrc, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ImgVectorSizeConverter(
	vx_image_data<TypeSrc, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeSrc, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src
#pragma HLS data_pack variable=dst
	VectorSizeConverter<TypeSrc, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
}

///***************************************************************************************************************************************************/
//// Creates an AXI4 stream including last/user signal
//template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL>
//void ImgAxi4StreamCreator(
//	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS) / PARALLEL],
//	vx_image_stream<TypeData, PARALLEL> dst[(BATCHES * PIXELS) / PARALLEL]) {
//#pragma HLS INLINE 
//#pragma HLS data_pack variable=src
//#pragma HLS data_pack variable=dst	
//	Axi4StreamCreator<TypeData, BATCHES, PIXELS, PARALLEL>(src, dst);
//}

#endif // NN_BASE_H_
