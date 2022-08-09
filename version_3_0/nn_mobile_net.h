/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_neural_network_core.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*
*/

#ifndef NN_MOBILE_NET_H_
#define NN_MOBILE_NET_H_

#include "nn_base.h"

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int32 PARALLEL, vx_int32 PIXELS_A, vx_int32 PIXELS_B>
void Splitter2(
	vx_image_data<TypeData, PARALLEL> src_stream[(PIXELS_A + PIXELS_B) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_a[PIXELS_A / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_b[PIXELS_B / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src_stream
#pragma HLS data_pack variable=dst_stream_a	
#pragma HLS data_pack variable=dst_stream_b

	const vx_int32 ELEMENTS_A = (PIXELS_A / PARALLEL);
	const vx_int32 ELEMENTS_B = (PIXELS_B / PARALLEL) + ELEMENTS_A;

	for (vx_int32 i = 0; i < ELEMENTS_B; ++i) {
#pragma HLS pipeline II=1	

		vx_image_data<TypeData, PARALLEL> input = src_stream[i];
#pragma HLS array_partition variable=input complete dim=0

		if (i < ELEMENTS_A) {
			dst_stream_a[i - 0] = input;
		} else if ((i >= ELEMENTS_A) && (i < ELEMENTS_B)) {
			dst_stream_b[i - ELEMENTS_A] = input;
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int32 PARALLEL, vx_int32 PIXELS_A, vx_int32 PIXELS_B, vx_int32 PIXELS_C>
void Splitter3(
	vx_image_data<TypeData, PARALLEL> src_stream[(PIXELS_A + PIXELS_B + PIXELS_C) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_a[PIXELS_A / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_b[PIXELS_B / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_c[PIXELS_C / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src_stream
#pragma HLS data_pack variable=dst_stream_a	
#pragma HLS data_pack variable=dst_stream_b
#pragma HLS data_pack variable=dst_stream_c

	const vx_int32 ELEMENTS_A = (PIXELS_A / PARALLEL);
	const vx_int32 ELEMENTS_B = (PIXELS_B / PARALLEL) + ELEMENTS_A;
	const vx_int32 ELEMENTS_C = (PIXELS_C / PARALLEL) + ELEMENTS_B;

	for (vx_int32 i = 0; i < ELEMENTS_C; ++i) {
#pragma HLS pipeline II=1	

		vx_image_data<TypeData, PARALLEL> input = src_stream[i];
#pragma HLS array_partition variable=input complete dim=0

		if (i < ELEMENTS_A) {			
			dst_stream_a[i - 0] = input;
		} else if ((i >= ELEMENTS_A) && (i < ELEMENTS_B)) {
			dst_stream_b[i - ELEMENTS_A] = input;
		} else if ((i >= ELEMENTS_B) && (i < ELEMENTS_C)) {
			dst_stream_c[i - ELEMENTS_B] = input;
		} 
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int32 PARALLEL, vx_int32 PIXELS_A, vx_int32 PIXELS_B, vx_int32 PIXELS_C, vx_int32 PIXELS_D, vx_int32 PIXELS_E, vx_int32 PIXELS_F>
void Splitter6(
	vx_image_data<TypeData, PARALLEL> src_stream[(PIXELS_A + PIXELS_B + PIXELS_C + PIXELS_D + PIXELS_E + PIXELS_F) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_a[PIXELS_A / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_b[PIXELS_B / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_c[PIXELS_C / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_d[PIXELS_D / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_e[PIXELS_E / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst_stream_f[PIXELS_F / PARALLEL]) {
#pragma HLS INLINE 
#pragma HLS data_pack variable=src_stream
#pragma HLS data_pack variable=dst_stream_a	
#pragma HLS data_pack variable=dst_stream_b
#pragma HLS data_pack variable=dst_stream_c
#pragma HLS data_pack variable=dst_stream_d
#pragma HLS data_pack variable=dst_stream_e
#pragma HLS data_pack variable=dst_stream_f

	const vx_int32 ELEMENTS_A = (PIXELS_A / PARALLEL);
	const vx_int32 ELEMENTS_B = (PIXELS_B / PARALLEL) + ELEMENTS_A;
	const vx_int32 ELEMENTS_C = (PIXELS_C / PARALLEL) + ELEMENTS_B;
	const vx_int32 ELEMENTS_D = (PIXELS_D / PARALLEL) + ELEMENTS_C;
	const vx_int32 ELEMENTS_E = (PIXELS_E / PARALLEL) + ELEMENTS_D;
	const vx_int32 ELEMENTS_F = (PIXELS_F / PARALLEL) + ELEMENTS_E;

	for (vx_int32 i = 0; i < ELEMENTS_F; ++i) {
#pragma HLS pipeline II=1	

		vx_image_data<TypeData, PARALLEL> input = src_stream[i];
#pragma HLS array_partition variable=input complete dim=0

		if (i < ELEMENTS_A) {			
			dst_stream_a[i - 0] = input;
		} else if ((i >= ELEMENTS_A) && (i < ELEMENTS_B)) {
			dst_stream_b[i - ELEMENTS_A] = input;
		} else if ((i >= ELEMENTS_B) && (i < ELEMENTS_C)) {
			dst_stream_c[i - ELEMENTS_B] = input;
		} else if ((i >= ELEMENTS_C) && (i < ELEMENTS_D)) {
			dst_stream_d[i - ELEMENTS_C] = input;
		} else if ((i >= ELEMENTS_D) && (i < ELEMENTS_E)) {
			dst_stream_e[i - ELEMENTS_D] = input;
		} else if ((i >= ELEMENTS_E) && (i < ELEMENTS_F)) {
			dst_stream_f[i - ELEMENTS_E] = input;
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 DST_ROWS, vx_int64 DST_COLS, vx_int64 IFM, vx_int64 OFM,
	vx_int64 DW_PARALLEL, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL, vx_int64 PW_PARALLEL, vx_int64 DMA_PARALLEL, vx_int64 BATCHES,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUND_POLICY, vx_convert_policy_e OVERFLOW_POLICY>
void MobileNet1(
	vx_image_data<TypeData, DMA_PARALLEL>* coefficients,
	vx_image_data<TypeData, DMA_PARALLEL> inputs[(BATCHES * SRC_ROWS * SRC_COLS * IFM) / DMA_PARALLEL],
	vx_image_data<TypeData, PW_PARALLEL> outputs[(BATCHES * DST_ROWS * DST_COLS * OFM) / PW_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS data_pack variable=coefficients
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=outputs	

#pragma HLS DATAFLOW	

	// Constant parameters
	const vx_int64 KERNEL = 3;
	const vx_int64 BN_WEIGHTS_NUM = 2;
	const bool RELAXED_MATH = false;
	const bool BUFFER_COEFFICIENTS = false;
	const TypeData ACTIVATION_PARAM_A = 0;
	const TypeData ACTIVATION_PARAM_B = 0;
	const vx_nn_activation_function_e ACTIVATION_FUNCTION = VX_NN_ACTIVATION_RELU;

	// Coefficients
	const vx_int64 WEIGHTS_SIZE = OFM * IFM * KERNEL * KERNEL;
	const vx_int64 BIASES_SIZE = OFM;
	const vx_int64 BN_WEIGHTS_SIZE = OFM * BN_WEIGHTS_NUM;
	const vx_int64 __WEIGHTS_SIZE = ALIGN(WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __BIASES_SIZE = ALIGN(BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __BN_WEIGHTS_SIZE = ALIGN(BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 WEIGHTS_PARALLEL = OFM_PARALLEL * IFM_PARALLEL * KERNEL * KERNEL;
	const vx_int64 BIASES_PARALLEL = OFM_PARALLEL;
	const vx_int64 BN_WEIGHTS_PARALLEL = PW_PARALLEL * BN_WEIGHTS_NUM;

	// Constants
	const vx_int64 OUTPUT_PIXELS = DST_ROWS * DST_COLS;
	const vx_int64 INPUTS_ELEMENTS = SRC_ROWS * SRC_COLS * IFM;
	const vx_int64 OUTPUT_ELEMENTS = DST_ROWS * DST_COLS * OFM;
	const vx_int64 INPUTS_STREAM = BATCHES * INPUTS_ELEMENTS;
	const vx_int64 OUTPUT_STREAM = BATCHES * OUTPUT_ELEMENTS;

#ifdef __XILINX__
	// Buffers for Coefficients
	vx_image_data<TypeData, WEIGHTS_PARALLEL> weights_buffer[WEIGHTS_SIZE / WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=weights_buffer
#pragma HLS RESOURCE variable=weights_buffer core=RAM_2P_LUTRAM
	vx_image_data<TypeData, BIASES_PARALLEL> bias_buffer[BIASES_SIZE / BIASES_PARALLEL];
#pragma HLS data_pack variable=bias_buffer
#pragma HLS RESOURCE variable=bias_buffer core=RAM_2P_LUTRAM 
	vx_image_data<TypeData, BN_WEIGHTS_PARALLEL> bn_weights_buffer[BN_WEIGHTS_SIZE / BN_WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=bn_weights_buffer
#pragma HLS RESOURCE variable=bn_weights_buffer core=RAM_2P_LUTRAM

	// Coefficient streams
	vx_image_data<TypeData, DMA_PARALLEL> weights_stream[__WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=weights_stream
#pragma HLS stream variable=weights_stream depth=8
#pragma HLS RESOURCE variable=weights_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> bias_stream[__BIASES_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=bias_stream
#pragma HLS stream variable=bias_stream depth=8
#pragma HLS RESOURCE variable=bias_stream core=FIFO_LUTRAM 
	vx_image_data<TypeData, DMA_PARALLEL> bn_weights_stream[__BN_WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=bn_weights_stream
#pragma HLS stream variable=bn_weights_stream depth=8
#pragma HLS RESOURCE variable=bn_weights_stream core=FIFO_LUTRAM

	// Mobile Net Layer 1
	vx_image_data<TypeData, IFM_PARALLEL> inputs_stream[INPUTS_STREAM / IFM_PARALLEL];
#pragma HLS data_pack variable=inputs_stream
#pragma HLS stream variable=inputs_stream depth=8
#pragma HLS RESOURCE variable=inputs_stream core=FIFO_LUTRAM	
	vx_image_data<TypeData, OFM_PARALLEL> dst_conv_stream[OUTPUT_STREAM / OFM_PARALLEL];
#pragma HLS data_pack variable=dst_conv_stream
#pragma HLS stream variable=dst_conv_stream depth=8
#pragma HLS RESOURCE variable=dst_conv_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, PW_PARALLEL> internal_stream[OUTPUT_STREAM / PW_PARALLEL];
#pragma HLS data_pack variable=internal_stream
#pragma HLS stream variable=internal_stream depth=8
#pragma HLS RESOURCE variable=internal_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, PW_PARALLEL> dst_bn_stream[OUTPUT_STREAM / PW_PARALLEL];
#pragma HLS data_pack variable=dst_bn_stream
#pragma HLS stream variable=dst_bn_stream depth=8
#pragma HLS RESOURCE variable=dst_bn_stream core=FIFO_LUTRAM

#else
	// Buffers for Coefficients
	vx_image_data<TypeData, WEIGHTS_PARALLEL>* weights_buffer = new vx_image_data<TypeData, WEIGHTS_PARALLEL>[WEIGHTS_SIZE / WEIGHTS_PARALLEL];
	vx_image_data<TypeData, BIASES_PARALLEL>* bias_buffer = new vx_image_data<TypeData, BIASES_PARALLEL>[BIASES_SIZE / BIASES_PARALLEL];
	vx_image_data<TypeData, BN_WEIGHTS_PARALLEL>* bn_weights_buffer = new vx_image_data<TypeData, BN_WEIGHTS_PARALLEL>[BN_WEIGHTS_SIZE / BN_WEIGHTS_PARALLEL];

	// Coefficient streams
	vx_image_data<TypeData, DMA_PARALLEL>* weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__WEIGHTS_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* bias_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__BIASES_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* bn_weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__BN_WEIGHTS_SIZE / DMA_PARALLEL];

	// Mobile Net Layer 1
	vx_image_data<TypeData, IFM_PARALLEL>* inputs_stream = new vx_image_data<TypeData, IFM_PARALLEL>[INPUTS_STREAM / IFM_PARALLEL];
	vx_image_data<TypeData, OFM_PARALLEL>* dst_conv_stream = new vx_image_data<TypeData, OFM_PARALLEL>[OUTPUT_STREAM / OFM_PARALLEL];
	vx_image_data<TypeData, PW_PARALLEL>* internal_stream = new vx_image_data<TypeData, PW_PARALLEL>[OUTPUT_STREAM / PW_PARALLEL];
	vx_image_data<TypeData, PW_PARALLEL>* dst_bn_stream = new vx_image_data<TypeData, PW_PARALLEL>[OUTPUT_STREAM / PW_PARALLEL];
#endif

	// Read different coefficients
	Splitter3<TypeData, DMA_PARALLEL, __WEIGHTS_SIZE, __BIASES_SIZE, __BN_WEIGHTS_SIZE>(coefficients, weights_stream, bias_stream, bn_weights_stream);
	ImgVectorSizeConverter<TypeData, 1, __WEIGHTS_SIZE, WEIGHTS_SIZE, DMA_PARALLEL, WEIGHTS_PARALLEL>(weights_stream, weights_buffer);
	ImgVectorSizeConverter<TypeData, 1, __BIASES_SIZE, BIASES_SIZE, DMA_PARALLEL, BIASES_PARALLEL>(bias_stream, bias_buffer);
	ImgVectorSizeConverter<TypeData, 1, __BN_WEIGHTS_SIZE, BN_WEIGHTS_SIZE, DMA_PARALLEL, BN_WEIGHTS_PARALLEL>(bn_weights_stream, bn_weights_buffer);

	// Input Stream
	ImgVectorSizeConverter<TypeData, BATCHES, INPUTS_ELEMENTS, INPUTS_ELEMENTS, DMA_PARALLEL, IFM_PARALLEL>(inputs, inputs_stream);

	// Layer 1
	ImgConvPointWise<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, IFM, DST_ROWS, DST_COLS, OFM, KERNEL, KERNEL, BIASES_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUND_POLICY, BUFFER_COEFFICIENTS>(inputs_stream, weights_buffer, bias_buffer, dst_conv_stream);
	ImgVectorSizeConverter<TypeData, BATCHES, OUTPUT_ELEMENTS, OUTPUT_ELEMENTS, OFM_PARALLEL, PW_PARALLEL>(dst_conv_stream, internal_stream);
	ImgBatchNorm2<TypeData, BATCHES, OUTPUT_PIXELS, OFM, PW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, BN_WEIGHTS_NUM, BUFFER_COEFFICIENTS>(bn_weights_buffer, internal_stream, dst_bn_stream);
	ImgActivationLayer<TypeData, BATCHES, OUTPUT_ELEMENTS, PW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, dst_bn_stream, outputs);

#ifndef __XILINX__
	delete[] weights_buffer;
	delete[] bias_buffer;
	delete[] bn_weights_buffer;
	delete[] weights_stream;
	delete[] bias_stream;
	delete[] bn_weights_stream;
	delete[] inputs_stream;
	delete[] dst_conv_stream;
	delete[] internal_stream;
	delete[] dst_bn_stream;
#endif
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 DST_ROWS, vx_int64 DST_COLS, vx_int64 IFM, vx_int64 OFM, 
	vx_int64 DW_PARALLEL, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL, vx_int64 PW_PARALLEL, vx_int64 DMA_PARALLEL, vx_int64 BATCHES, 
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUND_POLICY, vx_convert_policy_e OVERFLOW_POLICY>
void MobileNet2(
	vx_image_data<TypeData, DMA_PARALLEL>* coefficients,
	vx_image_data<TypeData, DW_PARALLEL> inputs[(BATCHES * SRC_ROWS * SRC_COLS * IFM) / DW_PARALLEL],
	vx_image_data<TypeData, PW_PARALLEL> outputs[(BATCHES * DST_ROWS * DST_COLS * OFM) / PW_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS data_pack variable=coefficients
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=outputs	

#pragma HLS DATAFLOW

	// Constant parameters
	const vx_int64 DW_KERNEL = 3;
	const vx_int64 PW_KERNEL = 1;
	const vx_int64 BN_WEIGHTS_NUM = 2;
	const bool RELAXED_MATH = false;
	const bool BUFFER_COEFFICIENTS = false;
	const TypeData ACTIVATION_PARAM_A = 0;
	const TypeData ACTIVATION_PARAM_B = 0;
	const vx_nn_activation_function_e ACTIVATION_FUNCTION = VX_NN_ACTIVATION_RELU;

	// DW: Coefficients
	const vx_int64 DW_WEIGHTS_SIZE = IFM * DW_KERNEL * DW_KERNEL;
	const vx_int64 DW_BIASES_SIZE = IFM;
	const vx_int64 DW_BN_WEIGHTS_SIZE = IFM * BN_WEIGHTS_NUM;
	const vx_int64 __DW_WEIGHTS_SIZE = ALIGN(DW_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __DW_BIASES_SIZE = ALIGN(DW_BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __DW_BN_WEIGHTS_SIZE = ALIGN(DW_BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 DW_WEIGHTS_PARALLEL = DW_PARALLEL * DW_KERNEL * DW_KERNEL;
	const vx_int64 DW_BIASES_PARALLEL = DW_PARALLEL;
	const vx_int64 DW_BN_WEIGHTS_PARALLEL = DW_PARALLEL * BN_WEIGHTS_NUM;

	// PW: Coefficients
	const vx_int64 PW_WEIGHTS_SIZE = OFM * IFM * PW_KERNEL * PW_KERNEL;
	const vx_int64 PW_BIASES_SIZE = OFM;
	const vx_int64 PW_BN_WEIGHTS_SIZE = OFM * BN_WEIGHTS_NUM;
	const vx_int64 __PW_WEIGHTS_SIZE = ALIGN(PW_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __PW_BIASES_SIZE = ALIGN(PW_BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __PW_BN_WEIGHTS_SIZE = ALIGN(PW_BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 PW_WEIGHTS_PARALLEL = OFM_PARALLEL * IFM_PARALLEL * PW_KERNEL * PW_KERNEL;
	const vx_int64 PW_BIASES_PARALLEL = OFM_PARALLEL;
	const vx_int64 PW_BN_WEIGHTS_PARALLEL = PW_PARALLEL * BN_WEIGHTS_NUM;

	// Data Streams
	const vx_int64 OUTPUT_PIXELS = DST_ROWS * DST_COLS;
	const vx_int64 INTERNAL_ELEMENTS = OUTPUT_PIXELS * IFM;
	const vx_int64 INTERNAL_STREAM = BATCHES * INTERNAL_ELEMENTS;
	const vx_int64 OUTPUT_ELEMENTS = OUTPUT_PIXELS * OFM;
	const vx_int64 OUTPUT_STREAM = BATCHES * OUTPUT_ELEMENTS;

	// Compile-time checks
	STATIC_ASSERT(((IFM % DW_PARALLEL) == 0), MISSMATCH_IFM_DW_PARALLEL);
	STATIC_ASSERT(((IFM % IFM_PARALLEL) == 0), MISSMATCH_IFM_IFM_PARALLEL);
	STATIC_ASSERT(((OFM % OFM_PARALLEL) == 0), MISSMATCH_OFM_OFM_PARALLEL);
	STATIC_ASSERT(((OFM % PW_PARALLEL) == 0), MISSMATCH_OFM_PW_PARALLEL);

#ifdef __XILINX__
	// DW: Stream Coefficients
	vx_image_data<TypeData, DMA_PARALLEL> dw_weights_stream[__DW_WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=dw_weights_stream
#pragma HLS stream variable=dw_weights_stream depth=8
#pragma HLS RESOURCE variable=dw_weights_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> dw_biases_stream[__DW_BIASES_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=dw_biases_stream
#pragma HLS stream variable=dw_biases_stream depth=8
#pragma HLS RESOURCE variable=dw_biases_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> dw_bn_weights_stream[__DW_BN_WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=dw_bn_weights_stream
#pragma HLS stream variable=dw_bn_weights_stream depth=8
#pragma HLS RESOURCE variable=dw_bn_weights_stream core=FIFO_LUTRAM

	// DW: Buffer Coefficients
	vx_image_data<TypeData, DW_WEIGHTS_PARALLEL> dw_weights_buffer[DW_WEIGHTS_SIZE / DW_WEIGHTS_PARALLEL];
#pragma HLS RESOURCE variable=dw_weights_buffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=dw_weights_buffer
	vx_image_data<TypeData, DW_BIASES_PARALLEL> dw_biases_buffer[DW_BIASES_SIZE / DW_BIASES_PARALLEL];
#pragma HLS RESOURCE variable=dw_biases_buffer core=RAM_2P_LUTRAM 
#pragma HLS data_pack variable=dw_biases_buffer
	vx_image_data<TypeData, DW_BN_WEIGHTS_PARALLEL> dw_bn_weights_buffer[DW_BN_WEIGHTS_SIZE / DW_BN_WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=dw_bn_weights_buffer
#pragma HLS RESOURCE variable=dw_bn_weights_buffer core=RAM_2P_LUTRAM

	// PW: Stream Coefficients
	vx_image_data<TypeData, DMA_PARALLEL> pw_weights_stream[__PW_WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=pw_weights_stream
#pragma HLS stream variable=pw_weights_stream depth=8
#pragma HLS RESOURCE variable=pw_weights_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> pw_biases_stream[__PW_BIASES_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=pw_biases_stream
#pragma HLS stream variable=pw_biases_stream depth=8
#pragma HLS RESOURCE variable=pw_biases_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> pw_bn_weights_stream[__PW_BN_WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=pw_bn_weights_stream
#pragma HLS stream variable=pw_bn_weights_stream depth=8
#pragma HLS RESOURCE variable=pw_bn_weights_stream core=FIFO_LUTRAM

	// PW: Buffer Coefficients
	vx_image_data<TypeData, PW_WEIGHTS_PARALLEL> pw_weights_buffer[PW_WEIGHTS_SIZE / PW_WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=pw_weights_buffer
#pragma HLS RESOURCE variable=pw_weights_buffer core=RAM_2P_BRAM
	vx_image_data<TypeData, PW_BIASES_PARALLEL> pw_biases_buffer[PW_BIASES_SIZE / PW_BIASES_PARALLEL];
#pragma HLS data_pack variable=pw_biases_buffer
#pragma HLS RESOURCE variable=pw_biases_buffer core=RAM_2P_LUTRAM 
	vx_image_data<TypeData, PW_BN_WEIGHTS_PARALLEL> pw_bn_weights_buffer[PW_BN_WEIGHTS_SIZE / PW_BN_WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=pw_bn_weights_buffer
#pragma HLS RESOURCE variable=pw_bn_weights_buffer core=RAM_2P_LUTRAM

	// DW: Convolution / Batch Normalization / Activation
	vx_image_data<TypeData, DW_PARALLEL> dw_dst_conv_stream[INTERNAL_STREAM / DW_PARALLEL];
#pragma HLS data_pack variable=dw_dst_conv_stream
#pragma HLS stream variable=dw_dst_conv_stream depth=8
#pragma HLS RESOURCE variable=dw_dst_conv_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DW_PARALLEL> dw_dst_bn_stream[INTERNAL_STREAM / DW_PARALLEL];
#pragma HLS data_pack variable=dw_dst_bn_stream
#pragma HLS stream variable=dw_dst_bn_stream depth=8
#pragma HLS RESOURCE variable=dw_dst_bn_stream core=FIFO_LUTRAM

	// Internal stream
	vx_image_data<TypeData, DW_PARALLEL> dw_internal_stream[INTERNAL_STREAM / DW_PARALLEL];
#pragma HLS data_pack variable=dw_internal_stream
#pragma HLS stream variable=dw_internal_stream depth=8
#pragma HLS RESOURCE variable=dw_internal_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, IFM_PARALLEL> pw_internal_stream[INTERNAL_STREAM / IFM_PARALLEL];
#pragma HLS data_pack variable=pw_internal_stream
#pragma HLS stream variable=pw_internal_stream depth=8
#pragma HLS RESOURCE variable=pw_internal_stream core=FIFO_LUTRAM

	// PW: Convolution / Batch Normalization / Activation
	vx_image_data<TypeData, OFM_PARALLEL> pw_dst_conv_stream[OUTPUT_STREAM / OFM_PARALLEL];
#pragma HLS data_pack variable=pw_dst_conv_stream
#pragma HLS stream variable=pw_dst_conv_stream depth=8
#pragma HLS RESOURCE variable=pw_dst_conv_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, PW_PARALLEL> pw_src_bn_stream[OUTPUT_STREAM / PW_PARALLEL];
#pragma HLS data_pack variable=pw_src_bn_stream
#pragma HLS stream variable=pw_src_bn_stream depth=8
#pragma HLS RESOURCE variable=pw_src_bn_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, PW_PARALLEL> pw_dst_bn_stream[OUTPUT_STREAM / PW_PARALLEL];
#pragma HLS data_pack variable=pw_dst_bn_stream
#pragma HLS stream variable=pw_dst_bn_stream depth=8
#pragma HLS RESOURCE variable=pw_dst_bn_stream core=FIFO_LUTRAM

#else
	// DW: Stream Coefficients
	vx_image_data<TypeData, DMA_PARALLEL>* dw_weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__DW_WEIGHTS_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* dw_biases_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__DW_BIASES_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* dw_bn_weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__DW_BN_WEIGHTS_SIZE / DMA_PARALLEL];

	// DW: Buffer Coefficients
	vx_image_data<TypeData, DW_WEIGHTS_PARALLEL>* dw_weights_buffer = new vx_image_data<TypeData, DW_WEIGHTS_PARALLEL>[DW_WEIGHTS_SIZE / DW_WEIGHTS_PARALLEL];
	vx_image_data<TypeData, DW_BIASES_PARALLEL>* dw_biases_buffer = new vx_image_data<TypeData, DW_BIASES_PARALLEL>[DW_BIASES_SIZE / DW_BIASES_PARALLEL];
	vx_image_data<TypeData, DW_BN_WEIGHTS_PARALLEL>* dw_bn_weights_buffer = new vx_image_data<TypeData, DW_BN_WEIGHTS_PARALLEL>[DW_BN_WEIGHTS_SIZE / DW_BN_WEIGHTS_PARALLEL];

	// PW: Stream Coefficients
	vx_image_data<TypeData, DMA_PARALLEL>* pw_weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__PW_WEIGHTS_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* pw_biases_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__PW_BIASES_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* pw_bn_weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__PW_BN_WEIGHTS_SIZE / DMA_PARALLEL];

	// PW: Buffer Coefficients
	vx_image_data<TypeData, PW_WEIGHTS_PARALLEL>* pw_weights_buffer = new vx_image_data<TypeData, PW_WEIGHTS_PARALLEL>[PW_WEIGHTS_SIZE / PW_WEIGHTS_PARALLEL];
	vx_image_data<TypeData, PW_BIASES_PARALLEL>* pw_biases_buffer = new vx_image_data<TypeData, PW_BIASES_PARALLEL>[PW_BIASES_SIZE / PW_BIASES_PARALLEL];
	vx_image_data<TypeData, PW_BN_WEIGHTS_PARALLEL>* pw_bn_weights_buffer = new vx_image_data<TypeData, PW_BN_WEIGHTS_PARALLEL>[PW_BN_WEIGHTS_SIZE / PW_BN_WEIGHTS_PARALLEL];

	// DW: Convolution / Batch Normalization / Activation
	vx_image_data<TypeData, DW_PARALLEL>* dw_dst_conv_stream = new vx_image_data<TypeData, DW_PARALLEL>[INTERNAL_STREAM / DW_PARALLEL];
	vx_image_data<TypeData, DW_PARALLEL>* dw_dst_bn_stream = new vx_image_data<TypeData, DW_PARALLEL>[INTERNAL_STREAM / DW_PARALLEL];

	// Internal stream
	vx_image_data<TypeData, DW_PARALLEL>* dw_internal_stream = new vx_image_data<TypeData, DW_PARALLEL>[INTERNAL_STREAM / DW_PARALLEL];
	vx_image_data<TypeData, IFM_PARALLEL>* pw_internal_stream = new vx_image_data<TypeData, IFM_PARALLEL>[INTERNAL_STREAM / IFM_PARALLEL];

	// PW: Convolution / Batch Normalization / Activation
	vx_image_data<TypeData, OFM_PARALLEL>* pw_dst_conv_stream = new vx_image_data<TypeData, OFM_PARALLEL>[OUTPUT_STREAM / OFM_PARALLEL];
	vx_image_data<TypeData, PW_PARALLEL>* pw_src_bn_stream = new vx_image_data<TypeData, PW_PARALLEL>[OUTPUT_STREAM / PW_PARALLEL];
	vx_image_data<TypeData, PW_PARALLEL>* pw_dst_bn_stream = new vx_image_data<TypeData, PW_PARALLEL>[OUTPUT_STREAM / PW_PARALLEL];
#endif

	// Read Coefficients		
	Splitter6<TypeData, DMA_PARALLEL, __DW_WEIGHTS_SIZE, __DW_BIASES_SIZE, __DW_BN_WEIGHTS_SIZE, __PW_WEIGHTS_SIZE, __PW_BIASES_SIZE, __PW_BN_WEIGHTS_SIZE>(coefficients, dw_weights_stream, dw_biases_stream, dw_bn_weights_stream, pw_weights_stream, pw_biases_stream, pw_bn_weights_stream);
	ImgVectorSizeConverter<TypeData, 1, __DW_WEIGHTS_SIZE, DW_WEIGHTS_SIZE, DMA_PARALLEL, DW_WEIGHTS_PARALLEL>(dw_weights_stream, dw_weights_buffer);
	ImgVectorSizeConverter<TypeData, 1, __DW_BIASES_SIZE, DW_BIASES_SIZE, DMA_PARALLEL, DW_BIASES_PARALLEL>(dw_biases_stream, dw_biases_buffer);
	ImgVectorSizeConverter<TypeData, 1, __DW_BN_WEIGHTS_SIZE, DW_BN_WEIGHTS_SIZE, DMA_PARALLEL, DW_BN_WEIGHTS_PARALLEL>(dw_bn_weights_stream, dw_bn_weights_buffer);
	ImgVectorSizeConverter<TypeData, 1, __PW_WEIGHTS_SIZE, PW_WEIGHTS_SIZE, DMA_PARALLEL, PW_WEIGHTS_PARALLEL>(pw_weights_stream, pw_weights_buffer);
	ImgVectorSizeConverter<TypeData, 1, __PW_BIASES_SIZE, PW_BIASES_SIZE, DMA_PARALLEL, PW_BIASES_PARALLEL>(pw_biases_stream, pw_biases_buffer);
	ImgVectorSizeConverter<TypeData, 1, __PW_BN_WEIGHTS_SIZE, PW_BN_WEIGHTS_SIZE, DMA_PARALLEL, PW_BN_WEIGHTS_PARALLEL>(pw_bn_weights_stream, pw_bn_weights_buffer);

	// DW: Convolution | Batch Normalization | Activation
	ImgConvDepthWise<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, DW_KERNEL, DW_KERNEL, DW_BIASES_SIZE, BATCHES, DW_PARALLEL, OVERFLOW_POLICY, ROUND_POLICY, BUFFER_COEFFICIENTS>(inputs, dw_weights_buffer, dw_biases_buffer, dw_dst_conv_stream);
	ImgBatchNorm2<TypeData, BATCHES, OUTPUT_PIXELS, IFM, DW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, BN_WEIGHTS_NUM, BUFFER_COEFFICIENTS>(dw_bn_weights_buffer, dw_dst_conv_stream, dw_dst_bn_stream);
	ImgActivationLayer<TypeData, BATCHES, INTERNAL_ELEMENTS, DW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, dw_dst_bn_stream, dw_internal_stream);

	// Internal stream
	ImgVectorSizeConverter<TypeData, BATCHES, INTERNAL_ELEMENTS, INTERNAL_ELEMENTS, DW_PARALLEL, IFM_PARALLEL>(dw_internal_stream, pw_internal_stream);

	// PW: Convolution | Batch Normalization | Activation
	ImgConvPointWise<TypeData, FIXED_POINT_POSITION, DST_ROWS, DST_COLS, IFM, DST_ROWS, DST_COLS, OFM, PW_KERNEL, PW_KERNEL, PW_BIASES_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUND_POLICY, BUFFER_COEFFICIENTS>(pw_internal_stream, pw_weights_buffer, pw_biases_buffer, pw_dst_conv_stream);
	ImgVectorSizeConverter<TypeData, BATCHES, OUTPUT_ELEMENTS, OUTPUT_ELEMENTS, OFM_PARALLEL, PW_PARALLEL>(pw_dst_conv_stream, pw_src_bn_stream);
	ImgBatchNorm2<TypeData, BATCHES, OUTPUT_PIXELS, OFM, PW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, BN_WEIGHTS_NUM, BUFFER_COEFFICIENTS>(pw_bn_weights_buffer, pw_src_bn_stream, pw_dst_bn_stream);
	ImgActivationLayer<TypeData, BATCHES, OUTPUT_ELEMENTS, PW_PARALLEL, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, pw_dst_bn_stream, outputs);

#ifndef __XILINX__
	delete[] dw_weights_stream;
	delete[] dw_biases_stream;
	delete[] dw_bn_weights_stream;
	delete[] dw_weights_buffer;
	delete[] dw_biases_buffer;
	delete[] dw_bn_weights_buffer;
	delete[] pw_weights_stream;
	delete[] pw_biases_stream;
	delete[] pw_bn_weights_stream;
	delete[] pw_weights_buffer;
	delete[] pw_biases_buffer;
	delete[] pw_bn_weights_buffer;
	delete[] dw_dst_conv_stream;
	delete[] dw_dst_bn_stream;
	delete[] dw_internal_stream;
	delete[] pw_internal_stream;
	delete[] pw_dst_conv_stream;
	delete[] pw_src_bn_stream;
	delete[] pw_dst_bn_stream;
#endif
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 IFM, vx_int64 OFM, vx_int64 IFM_PARALLEL, vx_int64 DMA_PARALLEL,
	vx_int64 BATCHES, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUND_POLICY, vx_convert_policy_e OVERFLOW_POLICY>
void MobileNet3(
	vx_image_data<TypeData, DMA_PARALLEL>* coefficients,
	TypeData inputs[BATCHES * SRC_ROWS * SRC_COLS * IFM],
	vx_image_data<TypeData, DMA_PARALLEL> outputs[(BATCHES * OFM) / DMA_PARALLEL]) {
#pragma HLS INLINE
#pragma HLS data_pack variable=coefficients
#pragma HLS data_pack variable=inputs
#pragma HLS data_pack variable=outputs	

#pragma HLS DATAFLOW	

	// Constant parameters
	const vx_int64 DST_ROWS = 1;
	const vx_int64 DST_COLS = 1;
	const vx_int64 PADDING_Y = 0;
	const vx_int64 PADDING_X = 0;
	const bool BUFFER_COEFFICIENTS = false;
	const vx_nn_pooling_type_e POOLING_TYPE = VX_NN_POOLING_AVG;

	// Connstants
	const vx_int64 INTERNAL_STREAM = BATCHES * IFM;
	const vx_int64 OUTPUT_STREAM = BATCHES * OFM;
	const vx_int64 KERNEL_Y = SRC_ROWS;
	const vx_int64 KERNEL_X = SRC_COLS;
	const vx_int64 WEIGHTS_SIZE = OFM * IFM;
	const vx_int64 BIASES_SIZE = OFM;
	const vx_int64 __WEIGHTS_SIZE = ALIGN(WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __BIASES_SIZE = ALIGN(BIASES_SIZE, DMA_PARALLEL);

	// Compile-Time checks
	STATIC_ASSERT((DST_ROWS == 1) && (DST_COLS == 1), DST_RESOLUTION_MUST_BE_1_TIMES_1);
	STATIC_ASSERT((PADDING_Y == 0) && (PADDING_X == 0), PADDING_MUST_BE_ZERO);
	STATIC_ASSERT((SRC_ROWS == KERNEL_Y), POOLING_SIZE_MUST_EQUAL_IMAGE_SIZE);
	STATIC_ASSERT((SRC_COLS == KERNEL_X), POOLING_SIZE_MUST_EQUAL_IMAGE_SIZE);

#ifdef __XILINX__
	// Coefficient input streams
	vx_image_data<TypeData, DMA_PARALLEL> weights_stream[__WEIGHTS_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=weights_stream
#pragma HLS stream variable=weights_stream depth=8
#pragma HLS RESOURCE variable=weights_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, DMA_PARALLEL> biases_stream[__BIASES_SIZE / DMA_PARALLEL];
#pragma HLS data_pack variable=biases_stream
#pragma HLS stream variable=biases_stream depth=8
#pragma HLS RESOURCE variable=biases_stream core=FIFO_LUTRAM

	// Coefficient buffers
	vx_image_data<TypeData, IFM_PARALLEL> weightsbuffer[WEIGHTS_SIZE / IFM_PARALLEL];
#pragma HLS RESOURCE variable=weightsbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=weightsbuffer
	vx_image_data<TypeData, 1> biasbuffer[BIASES_SIZE / 1];
#pragma HLS RESOURCE variable=biasbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=biasbuffer

	// Data streams
	vx_image_data<TypeData, 1> pool_dst_stream[INTERNAL_STREAM / 1];
#pragma HLS data_pack variable=pool_dst_stream
#pragma HLS stream variable=pool_dst_stream depth=8
#pragma HLS RESOURCE variable=pool_dst_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, IFM_PARALLEL> internal_stream[INTERNAL_STREAM / IFM_PARALLEL];
#pragma HLS data_pack variable=internal_stream
#pragma HLS stream variable=internal_stream depth=8
#pragma HLS RESOURCE variable=internal_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, 1> fully_dst_stream[OUTPUT_STREAM / 1];
#pragma HLS data_pack variable=fully_dst_stream
#pragma HLS stream variable=fully_dst_stream depth=8
#pragma HLS RESOURCE variable=fully_dst_stream core=FIFO_LUTRAM
	vx_image_data<TypeData, 1> softmax_dst_stream[OUTPUT_STREAM / 1];
#pragma HLS data_pack variable=softmax_dst_stream
#pragma HLS stream variable=softmax_dst_stream depth=8
#pragma HLS RESOURCE variable=softmax_dst_stream core=FIFO_LUTRAM

#else
	// Coefficient input streams
	vx_image_data<TypeData, DMA_PARALLEL>* weights_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__WEIGHTS_SIZE / DMA_PARALLEL];
	vx_image_data<TypeData, DMA_PARALLEL>* biases_stream = new vx_image_data<TypeData, DMA_PARALLEL>[__BIASES_SIZE / DMA_PARALLEL];

	// Coefficient buffers
	vx_image_data<TypeData, IFM_PARALLEL>* weightsbuffer = new vx_image_data<TypeData, IFM_PARALLEL>[WEIGHTS_SIZE / IFM_PARALLEL];
	vx_image_data<TypeData, 1>* biasbuffer = new vx_image_data < TypeData, 1>[BIASES_SIZE / 1];

	// Data streams
	vx_image_data<TypeData, 1>* pool_dst_stream = new vx_image_data < TypeData, 1>[INTERNAL_STREAM / 1];
	vx_image_data<TypeData, IFM_PARALLEL>* internal_stream = new vx_image_data<TypeData, IFM_PARALLEL>[INTERNAL_STREAM / IFM_PARALLEL];
	vx_image_data<TypeData, 1>* fully_dst_stream = new vx_image_data < TypeData, 1>[OUTPUT_STREAM / 1];
	vx_image_data<TypeData, 1>* softmax_dst_stream = new vx_image_data < TypeData, 1>[OUTPUT_STREAM / 1];
#endif

	// Coefficients
	Splitter2<TypeData, DMA_PARALLEL, __WEIGHTS_SIZE, __BIASES_SIZE>(coefficients, weights_stream, biases_stream);
	ImgVectorSizeConverter<TypeData, 1, __BIASES_SIZE, BIASES_SIZE, DMA_PARALLEL, 1>(biases_stream, biasbuffer);
	ImgVectorSizeConverter<TypeData, 1, __WEIGHTS_SIZE, WEIGHTS_SIZE, DMA_PARALLEL, IFM_PARALLEL>(weights_stream, weightsbuffer);

	// Avergage Pooling
	ImgPooling<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, 1, KERNEL_Y, KERNEL_X, PADDING_Y, PADDING_X, BATCHES, POOLING_TYPE>(inputs, pool_dst_stream);

	// Fully Connected
	ImgVectorSizeConverter<TypeData, BATCHES, IFM, IFM, 1, IFM_PARALLEL>(pool_dst_stream, internal_stream);
	ImgFullyConnected<TypeData, BATCHES, IFM, OFM, IFM_PARALLEL, BIASES_SIZE, WEIGHTS_SIZE, FIXED_POINT_POSITION, ROUND_POLICY, OVERFLOW_POLICY, BUFFER_COEFFICIENTS>(internal_stream, weightsbuffer, biasbuffer, fully_dst_stream);

	// Softmax
	ImgSoftmaxLayer<TypeData, BATCHES, OFM, 1, FIXED_POINT_POSITION, ROUND_POLICY>(fully_dst_stream, softmax_dst_stream);

	// Output Stream
	ImgVectorSizeConverter<TypeData, BATCHES, OFM, OFM, 1, DMA_PARALLEL>(softmax_dst_stream, outputs);

#ifndef __XILINX__
	delete[] weights_stream;
	delete[] biases_stream;
	delete[] weightsbuffer;
	delete[] biasbuffer;
	delete[] internal_stream;
	delete[] fully_dst_stream;
	delete[] softmax_dst_stream;
#endif
}

#endif // NN_MOBILE_NET_H_
