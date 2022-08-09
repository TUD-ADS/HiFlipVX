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

#ifndef NN_CORE2_H_
#define NN_CORE2_H_

#include "vx_helper.h"

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ConverterSrcMulitple(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 VECTOR = SRC_PIXELS / SRC_PARALLEL;
	const vx_int64 PARALLEL = MAX(SRC_PARALLEL / DST_PARALLEL, 1);

	// Input buffer
	vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
#pragma HLS array_partition variable=inputbuffer complete dim=0

	// Copy elements
	for (vx_int64 batch = 0, src_ptr = 0, dst_ptr = 0; batch < BATCHES; ++batch) {
		for (vx_int64 i = 0; i < VECTOR* PARALLEL; ++i) {
			//for (vx_int64 j = 0; j < PARALLEL; ++j) {
#pragma HLS pipeline II=1					
				vx_int64 j = i % PARALLEL;

				// Output buffer
				vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
//#pragma HLS array_partition variable=outputbuffer complete dim=0

				// Input
				if (j == 0) {
					inputbuffer = src[src_ptr];
					++src_ptr;
				}

				// Conversion
				for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
					outputbuffer.pixel[k] = inputbuffer.pixel[j * DST_PARALLEL + k];

				// Output
				GenerateDmaSignal<TypeData, DST_PARALLEL>((i == 0), (i == (VECTOR * PARALLEL) - 1), outputbuffer);
				dst[dst_ptr] = outputbuffer;
				++dst_ptr;
			//}
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ConverterDstMulitple(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 VECTOR = DST_PIXELS / DST_PARALLEL;
	const vx_int64 PARALLEL = MAX(DST_PARALLEL / SRC_PARALLEL, 1);

	// Output buffer
	vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
//#pragma HLS array_partition variable=outputbuffer complete dim=0

	// Copy elements
	for (vx_int64 batch = 0, src_ptr = 0, dst_ptr = 0; batch < BATCHES; ++batch) {
		for (vx_int64 i = 0; i < VECTOR* PARALLEL; ++i) {
			//for (vx_int64 j = 0; j < PARALLEL; ++j) {
#pragma HLS pipeline II=1	
				vx_int64 j = i % PARALLEL;

				// Input buffer
				vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
//#pragma HLS array_partition variable=inputbuffer complete dim=0

				// Input
				inputbuffer = src[src_ptr];
				++src_ptr;

				// Conversion
				for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
					outputbuffer.pixel[j * SRC_PARALLEL + k] = inputbuffer.pixel[k];

				// Output
				if (j == (PARALLEL - 1)) {
					GenerateDmaSignal<TypeData, DST_PARALLEL>((i == 0), (i == (VECTOR * PARALLEL) - 1), outputbuffer);
					dst[dst_ptr] = outputbuffer;
					++dst_ptr;
				}
			//}
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ConverterPassThrough(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 VECTOR = SRC_PIXELS / SRC_PARALLEL;
	const vx_int64 PARALLEL = MIN(DST_PARALLEL,SRC_PARALLEL);

	// Copy elements
	for (vx_int64 batch = 0, ptr = 0; batch < BATCHES; ++batch) {
		for (vx_int64 i = 0; i < VECTOR; ++i) {
#pragma HLS pipeline II=1	

			// Input buffer
			vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
//#pragma HLS array_partition variable=inputbuffer complete dim=0

			// Output buffer
			vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
//#pragma HLS array_partition variable=outputbuffer complete dim=0

			// Input
			inputbuffer = src[ptr];

			// Conversion ;)
			for (vx_int64 k = 0; k < PARALLEL; ++k)
				outputbuffer.pixel[k] = inputbuffer.pixel[k];

			// Output
			GenerateDmaSignal<TypeData, DST_PARALLEL>((i == 0), (i == VECTOR - 1), outputbuffer);
			dst[ptr] = outputbuffer;
			++ptr;
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ConvertSrcBigger(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 DST_VECTOR = DST_PIXELS / DST_PARALLEL;
	const vx_int64 SRC_VECTOR = SRC_PIXELS / SRC_PARALLEL;
	const vx_int64 BUFFER_SIZE = MIN(SRC_PARALLEL * DST_PARALLEL, MAX(DST_PIXELS, SRC_PIXELS));
	const vx_int64 VECTOR = MAX((BATCHES * DST_PIXELS) / (SRC_PARALLEL * DST_PARALLEL), (BATCHES * SRC_PIXELS) / (SRC_PARALLEL * DST_PARALLEL));

	// Long buffer to overcome the SRC/DST PARALLEL missmatch
	TypeData buffer[BUFFER_SIZE]; // TODO: Initialize
#pragma HLS array_partition variable=buffer complete dim=0

	// Iteration
	for (vx_int64 i = 0, src_ptr = 0, dst_ptr = 0; i < VECTOR; ++i) {
		for (vx_int64 j = 0; j < SRC_PARALLEL; ++j) {
#pragma HLS pipeline II=1	

			// Input buffer
			vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
//#pragma HLS array_partition variable=inputbuffer complete dim=0

			// Output buffer
			vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
//#pragma HLS array_partition variable=outputbuffer complete dim=0

			// Read data from input and write to buffer
			if ((j < DST_PARALLEL) && (src_ptr < (BATCHES * SRC_VECTOR))) {

				inputbuffer = src[src_ptr];
				++src_ptr;

				for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
					buffer[j * SRC_PARALLEL + k] = inputbuffer.pixel[k];
			}

			// Read data from buffer and write to output
			if ((j < SRC_PARALLEL) && (dst_ptr < (BATCHES * DST_VECTOR))) {

				for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
					outputbuffer.pixel[k] = buffer[j * DST_PARALLEL + k];

				GenerateDmaSignal<TypeData, DST_PARALLEL>(((dst_ptr % DST_VECTOR) == 0), ((dst_ptr % DST_VECTOR) == (DST_VECTOR - 1)), outputbuffer);
				dst[dst_ptr] = outputbuffer;
				++dst_ptr;
			}
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void ConvertDstBigger(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 GCD_PARALLEL = GCD(DST_PARALLEL, SRC_PARALLEL);
	const vx_int64 LCM_PARALLEL = LCM(DST_PARALLEL, SRC_PARALLEL);
	const vx_int64 BUFFER_SIZE = MIN(LCM_PARALLEL, MAX(DST_PIXELS, SRC_PIXELS));
	const vx_int64 DST_VECTOR = DST_PIXELS / DST_PARALLEL;
	const vx_int64 SRC_VECTOR = SRC_PIXELS / SRC_PARALLEL;
	const vx_int64 VECTOR = MAX(SRC_VECTOR, DST_VECTOR);

	// Long buffer to overcome the SRC/DST PARALLEL missmatch
	TypeData buffer[BUFFER_SIZE];
#pragma HLS array_partition variable=buffer complete dim=0

	// Itaeration
	for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
		for (vx_int64 src_ptr = 0, dst_ptr = 0; (src_ptr < VECTOR); ++src_ptr) {
#pragma HLS pipeline II=1	

			// Input buffer
			vx_image_data<TypeData, SRC_PARALLEL> inputbuffer;
//#pragma HLS array_partition variable=inputbuffer complete dim=0

			// Output buffer
			vx_image_data<TypeData, DST_PARALLEL> outputbuffer;
//#pragma HLS array_partition variable=outputbuffer complete dim=0

			// Buffer pointer
			const vx_int64 src_cnt = (src_ptr % (DST_PARALLEL / GCD_PARALLEL)) * SRC_PARALLEL;
			const vx_int64 dst_cnt = (dst_ptr % (SRC_PARALLEL / GCD_PARALLEL)) * DST_PARALLEL;

			// Read data from input and write to buffer
			if (src_ptr < SRC_VECTOR) {

				inputbuffer = src[batch * SRC_VECTOR + src_ptr];

				for (vx_int64 k = 0; k < SRC_PARALLEL; ++k)
					buffer[src_cnt + k] = inputbuffer.pixel[k];
			}

			// Read data from buffer and write to output
			if (dst_ptr < DST_VECTOR) {
				if ((src_cnt + SRC_PARALLEL) >= (dst_cnt + DST_PARALLEL)) {

					for (vx_int64 k = 0; k < DST_PARALLEL; ++k)
						outputbuffer.pixel[k] = buffer[dst_cnt + k];

					GenerateDmaSignal<TypeData, DST_PARALLEL>((dst_ptr == 0), (dst_ptr == DST_VECTOR - 1), outputbuffer);
					dst[batch * DST_VECTOR + dst_ptr] = outputbuffer;
					++dst_ptr;
				}
			}
		}
	}
}


/***************************************************************************************************************************************************/
// Convert between vector sizes of a stream
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_PIXELS, vx_int64 DST_PIXELS, vx_int64 SRC_PARALLEL, vx_int64 DST_PARALLEL>
void VectorSizeConverter(
	vx_image_data<TypeData, SRC_PARALLEL> src[(BATCHES * SRC_PIXELS) / SRC_PARALLEL],
	vx_image_data<TypeData, DST_PARALLEL> dst[(BATCHES * DST_PIXELS) / DST_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const bool MULTIPLE = (SRC_PARALLEL > DST_PARALLEL) ? ((SRC_PARALLEL % DST_PARALLEL) == 0) : ((DST_PARALLEL % SRC_PARALLEL) == 0);

	// Compile time checks
	STATIC_ASSERT((BATCHES > 0) && (SRC_PIXELS > 0) && (DST_PIXELS > 0) && (SRC_PARALLEL > 0) && (DST_PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
	STATIC_ASSERT(((SRC_PIXELS % SRC_PARALLEL) == 0) && (SRC_PIXELS >= SRC_PARALLEL), PIXELS_ARE_NOT_MULTIPLE_OF_SRC_PARALLEL);
	STATIC_ASSERT(((DST_PIXELS % DST_PARALLEL) == 0) && (DST_PIXELS >= DST_PARALLEL), PIXELS_ARE_NOT_MULTIPLE_OF_DST_PARALLEL);

	// Down Conversion
	if (SRC_PARALLEL > DST_PARALLEL) {
		if (MULTIPLE == true) {
			ConverterSrcMulitple<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
		} else {
			ConvertSrcBigger<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
		}

	// Up Conversion
	} else if (DST_PARALLEL > SRC_PARALLEL) {
		if (MULTIPLE == true) {
			ConverterDstMulitple<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
		} else {
			ConvertDstBigger<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
		}

	// Pass through
	} else if (SRC_PARALLEL == DST_PARALLEL) {
		ConverterPassThrough<TypeData, BATCHES, SRC_PIXELS, DST_PIXELS, SRC_PARALLEL, DST_PARALLEL>(src, dst);
	}
}

///***************************************************************************************************************************************************/
//// Creates an AXI4 stream including last/user signal
//template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL>
//void Axi4StreamCreator(
//	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS) / PARALLEL], 
//	vx_image_stream<TypeData, PARALLEL> dst[(BATCHES * PIXELS) / PARALLEL]) {
//#pragma HLS INLINE
//
//	// Constants
//	const vx_int64 VECTOR = PIXELS / PARALLEL;
//	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
//	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
//	const bool interger_size = ((digits == 7) || (digits == 8) || (digits == 15) || (digits == 16));
//
//	// Compile time checks
//	STATIC_ASSERT((BATCHES > 0) && (PIXELS > 0) && (PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
//	STATIC_ASSERT(((is_integer == true) && (interger_size == true)) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
//	STATIC_ASSERT((PIXELS % PARALLEL) == 0, PIXELS_NOT_MULTIPLE_OF_PARALLELIZATION);
//
//	// Iterate through all elements
//	for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
//		for (vx_int64 i = 0; i < VECTOR; ++i) {
//#pragma HLS pipeline II=1	
//
//			// Input & Output buffers
//			vx_image_data<TypeData, PARALLEL> inputbuffer;
//#pragma HLS array_partition variable=inputbuffer complete dim=0
//			vx_image_stream<TypeData, PARALLEL> outputbuffer;
//
//			// Input
//			inputbuffer = src[batch * VECTOR + i];
//
//			// Add tlast and tuser signals
//			for (vx_int64 j = 0; j < PARALLEL; ++j)
//				outputbuffer.pixel[j] = inputbuffer.pixel[j];
//			outputbuffer.user = (i == 0) ? (1) : (0);
//			outputbuffer.last = (i == (VECTOR - 1)) ? (1) : (0);
//
//			// Output
//			dst[batch * VECTOR + i] = outputbuffer;
//		}
//	}
//}

/***************************************************************************************************************************************************/
// Compute the batch normalization for 1 pixel using floating point values
template<typename T, vx_int64 WEIGHT_SIZE>
T BatchNormFloat(vx_image_data<T, WEIGHT_SIZE> weights, T input) {
#pragma HLS INLINE
	return weights.pixel[0] * ((input - weights.pixel[1]) * weights.pixel[2]) + weights.pixel[3];
}

/***************************************************************************************************************************************************/
// Compute the batch normalization for 1 pixel using fixe point values
template<typename T, vx_int64 FIXED_POINT_POSITION, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, vx_int64 WEIGHT_SIZE>
T BatchNormFixed(vx_image_data<T, WEIGHT_SIZE> weights, T input) {
#pragma HLS INLINE

	// Input + weights
	vx_int64 input_ = (vx_int64)input;
	vx_int64 filler_ = (vx_int64)weights.pixel[0];
	vx_int64 mean_ = (vx_int64)weights.pixel[1];
	vx_int64 rec_sqrt_var_ = (vx_int64)weights.pixel[2];
	vx_int64 bias_ = (vx_int64)weights.pixel[3];

	// Subtract by mean and then divide by square root of variance
	vx_int64 result = (input_ - mean_) * rec_sqrt_var_;

	// Rounding
	if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO) {
		result = result >> FIXED_POINT_POSITION;
	} else if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
		vx_int64 round = ((vx_int64)1 << (FIXED_POINT_POSITION - 1));
		result = (result + round) >> FIXED_POINT_POSITION;
	}

	// Multiply by filler
	result = filler_ * result;

	// Rounding
	if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO) {
		result = result >> FIXED_POINT_POSITION;
	} else if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
		vx_int64 round = ((vx_int64)1 << (FIXED_POINT_POSITION - 1));
		result = (result + round) >> FIXED_POINT_POSITION;
	}

	// Add bias
	result = result + bias_;

	// Check for overflow
	if (OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE) {
		vx_int64 maximum = (vx_int64)ComputeMax<T>();
		vx_int64 minimum = (vx_int64)ComputeMin<T>();
		result = MAX(result, minimum);
		result = MIN(result, maximum);
	}

	return (T)result;
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION,
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE>
void BatchNormalizationChecks() {
#pragma HLS INLINE

	// Constants
	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const bool interger_size = ((digits == 7) || (digits == 8) || (digits == 15) || (digits == 16));

	// Compile time checks
	STATIC_ASSERT((BATCHES > 0) && (PIXELS > 0) && (IFM > 0) && (PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
	STATIC_ASSERT(((is_integer == true) && (interger_size == true)) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
	STATIC_ASSERT(ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO || ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN, ROUNDING_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT(OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE || OVERFLOW_POLICY == VX_CONVERT_POLICY_WRAP, OVERFLOW_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT(FIXED_POINT_POSITION >= 0 && FIXED_POINT_POSITION <= digits, FIXED_POINT_POSITION_OUT_OF_SCOPE);
	STATIC_ASSERT(((IFM % PARALLEL) == 0) && (IFM >= PARALLEL), FEATURE_MAP_NOT_MULTIPLE_OF_PARALLELIZATION);
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE>
TypeData BatchNormCompute(TypeData data, vx_image_data<TypeData, WEIGHT_SIZE* PARALLEL> weights, vx_int64 parallel_id) {
#pragma HLS INLINE

	// Get the weights
	vx_image_data<TypeData, WEIGHT_SIZE> weight;
//#pragma HLS array_partition variable=weight complete dim=0

	// Get the weights
	for (vx_int64 k = 0; k < WEIGHT_SIZE; ++k)
		weight.pixel[k] = weights.pixel[parallel_id * WEIGHT_SIZE + k];

	// Compute batch normalization
	TypeData result = 0;
	if (std::numeric_limits<TypeData>::is_integer == true) {
		result = BatchNormFixed<TypeData, FIXED_POINT_POSITION, OVERFLOW_POLICY, ROUNDING_POLICY, WEIGHT_SIZE>(weight, data);
	}
	else {
		result = BatchNormFloat<TypeData, WEIGHT_SIZE>(weight, data);
	}

	// Output result
	return result;
}

/***************************************************************************************************************************************************/
// Batch Normalization
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, 
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void BatchNormalization(
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> batch_norm[IFM / PARALLEL],
	vx_image_data<TypeData, PARALLEL> src[BATCHES * PIXELS * (IFM / PARALLEL)],
	vx_image_data<TypeData, PARALLEL> dst[BATCHES * PIXELS * (IFM / PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int32 IFM_VECTOR = IFM / PARALLEL;

	// Compile time checks
	BatchNormalizationChecks<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE>();

	// Buffers the batch normalization weights
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> buffer[IFM_VECTOR];
#pragma HLS RESOURCE variable=buffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=buffer

	// Iterate over pixels [PIXELS] and Feature Maps [FM]
	for (vx_int64 i = 0; i < BATCHES * PIXELS; ++i) {
		for (vx_int64 fm = 0; fm < IFM_VECTOR; ++fm) {
#pragma HLS pipeline II=1	

			// To buffer the complete input/output vector
			vx_image_data<TypeData, PARALLEL> input;
//#pragma HLS array_partition variable=input complete dim=0
			vx_image_data <TypeData, PARALLEL> output;
//#pragma HLS array_partition variable=output complete dim=0

			// Read the input data vector
			input = src[i * IFM_VECTOR + fm];

			// To buffer the complete bn weights vector
			vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> weights;
//#pragma HLS array_partition variable=weights complete dim=0

			// Read the weights
			if (BUFFER_WEIGHTS == true) {
				if (i == 0) {
					weights = batch_norm[fm];
					buffer[fm] = weights;
				} else {
					weights = buffer[fm];
				}
			} else {
				weights = batch_norm[fm];
			}

			// Parallelize this computation for the PARALLEL degree
			for (vx_int64 j = 0; j < PARALLEL; ++j)
				output.pixel[j] = BatchNormCompute<TypeData, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE>(input.pixel[j], weights, j);

			// Write to output
			GenerateDmaSignal<TypeData, PARALLEL>(((fm == 0) && ((i % PIXELS) == 0)), ((fm == IFM_VECTOR - 1) && ((i % PIXELS) == (PIXELS - 1))), output);
			dst[i * IFM_VECTOR + fm] = output;
		}
	}
}

/***************************************************************************************************************************************************/
// Batch Normalization
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, 
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void BatchNormalization(
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> batch_norm[IFM / PARALLEL],
	TypeData src[BATCHES * PIXELS * (IFM / PARALLEL)],
	TypeData dst[BATCHES * PIXELS * (IFM / PARALLEL)]) {
#pragma HLS INLINE

	// Compile time checks
	BatchNormalizationChecks<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE>();

	// Buffers the batch normalization weights
	vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> buffer[(IFM / PARALLEL)];
#pragma HLS RESOURCE variable=buffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=buffer

	// Iterate over pixels [PIXELS] and Feature Maps [FM]
	for (vx_int64 i = 0; i < BATCHES * PIXELS; ++i) {
		for (vx_int64 fm = 0; fm < (IFM / PARALLEL); ++fm) {
#pragma HLS pipeline II=1	

			// To buffer the complete input/output vector
			TypeData input, output;

			// Read the input data vector
			input = src[i * (IFM / PARALLEL) + fm];

			// To buffer the complete bn weights vector
			vx_image_data<TypeData, WEIGHT_SIZE * PARALLEL> weights;
//#pragma HLS array_partition variable=weights complete dim=0

			// Read the weights
			if (BUFFER_WEIGHTS == true) {
				if (i == 0) {
					weights = batch_norm[fm];
					buffer[fm] = weights;
				} else {
					weights = buffer[fm];
				}
			} else {
				weights = batch_norm[fm];
			}

			// Parallelize this computation for the PARALLEL degree
			output = BatchNormCompute<TypeData, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, WEIGHT_SIZE>(input, weights, 0);

			// Write to output			
			dst[i * (IFM / PARALLEL) + fm] = output;
		}
	}
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int32 BATCHES, vx_int32 PIXELS, vx_int32 IFM, vx_int32 PARALLEL, vx_int32 FIXED_POINT_POSITION>
void BatchNormMean(vx_int32 i, vx_int32 ifmv, vx_int32& sum_total_int, vx_int64& sum1_total_flt, vx_int32& mean_int, vx_float32& mean_flt, vx_int32& src_ptr,
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> buffer_input[(IFM / PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const vx_float32 SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const vx_float32 IFM_INV_FLT = 1.0f / (vx_float32)IFM;
	const vx_int64 IFM_INV_INT = (1 << 16) / IFM;
	const vx_float32 MAX_INT31 = 1073741824.0f;
	const vx_float32 MIN_INT31 = -1073741824.0f;

	// Variables
	vx_int32 sum_part_int = 0, sum_tmp_int = 0;
	vx_int64 sum_part_flt = 0, sum_tmp_flt = 0;
	vx_image_data<TypeData, PARALLEL> input;

	// Read next input vector
	input = src[src_ptr];
	++src_ptr;

	// Buffer input vector
	buffer_input[ifmv] = input;

	// Vectorization
	for (vx_int32 ifmp = 0; ifmp < PARALLEL; ++ifmp) {
		TypeData data = 0;

		// Single input
		data = input.pixel[ifmp];

		// Convert from float to int32
		if (is_integer == true) {
			sum_part_int += static_cast<vx_int32>(data);
		} else {
			data = (vx_float32)data * SHIFT;
			data = MAX(MIN((vx_float32)data, MAX_INT31), MIN_INT31);
			sum_part_flt += static_cast<vx_int64>(data);
		}
	}

	// Compute Sum and Mean
	if (is_integer == true) {
		sum_tmp_int = (ifmv == 0) ? (sum_part_int) : (sum_total_int + sum_part_int);
		sum_total_int = sum_tmp_int;
		mean_int = (vx_int32)(((vx_int64)sum_tmp_int * (vx_int64)IFM_INV_INT) >> 16); // INT with FP_FRACTION
	} else {
		sum_tmp_flt = (ifmv == 0) ? (sum_part_flt) : (sum1_total_flt + sum_part_flt);
		sum1_total_flt = sum_tmp_flt;
		mean_flt = (vx_float32)sum_tmp_flt * IFM_INV_FLT; // FLT with FP_FRACTION
	}
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int32 IFM, vx_int32 PARALLEL, vx_int32 FIXED_POINT_POSITION>
void BatchNorm2Variance(vx_int32 ifmv, vx_float32 mean_flt, vx_int32 mean_int, vx_float32& variance_flt, vx_int32& variance_int, vx_int32& sum_total_int, vx_int64& sum1_total_flt,
	vx_image_data<TypeData, PARALLEL> buffer_input[(IFM / PARALLEL)]) {	
#pragma HLS INLINE

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const vx_float32 SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const vx_float32 INV_SHIFT = (1.0f / SHIFT);
	const vx_float32 IFM_INV_FLT = 1.0f / (vx_float32)IFM;
	const vx_int64 IFM_INV_INT = (1 << 16) / IFM;
	const vx_float32 MAX_INT31 = 1073741824.0f;
	const vx_float32 MIN_INT31 = -1073741824.0f;

	// Variables
	vx_int32 sum_part_int = 0, sum_tmp_int = 0;
	vx_int64 sum_part_flt = 0, sum_tmp_flt = 0;
	vx_image_data<TypeData, PARALLEL> input;

	// Get input vector from buffer
	input = buffer_input[ifmv];

	// Vectorization
	for (vx_int32 ifmp = 0; ifmp < PARALLEL; ++ifmp) {
		TypeData pixel = 0;
		vx_int32 data = 0;

		// Single input
		pixel = input.pixel[ifmp];

		// Compute Subtraction
		if (is_integer == true) {
			vx_int32 val = (vx_int32)pixel - mean_int;
			data = (val * val) >> FIXED_POINT_POSITION;
			sum_part_int += static_cast<vx_int32>(data);
		} else {
			vx_float32 val = (vx_float32)pixel * SHIFT - mean_flt;
			val = (val * val) * INV_SHIFT;
			data = (vx_int32)MAX(MIN(val, MAX_INT31), MIN_INT31);
			sum_part_flt += static_cast<vx_int64>(data);
		}
	}

	// Compute Sum and Variance
	if (is_integer == true) {
		sum_tmp_int = (ifmv == 0) ? (sum_part_int) : (sum_total_int + sum_part_int);
		sum_total_int = sum_tmp_int;
		variance_int = (vx_int32)(((vx_int64)sum_tmp_int * (vx_int64)IFM_INV_INT) >> 16); // INT with FP_FRACTION
	} else {
		sum_tmp_flt = (ifmv == 0) ? (sum_part_flt) : (sum1_total_flt + sum_part_flt);
		sum1_total_flt = sum_tmp_flt;
		variance_flt = (vx_float32)sum_tmp_flt * IFM_INV_FLT; // FLT with FP_FRACTION	
	}
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int32 BATCHES, vx_int32 PIXELS, vx_int32 IFM, vx_int32 PARALLEL,
	vx_int32 FIXED_POINT_POSITION, vx_convert_policy_e OVERFLOW_POLICY, vx_int32 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void BatchNorm2Normalize(vx_int32 i, vx_int32 ifmv, vx_float32 mean_flt, vx_float32 variance_flt, vx_int32 mean_int, vx_int32 variance_int, vx_int32& dst_ptr,
	vx_image_data<TypeData, PARALLEL* WEIGHT_SIZE> batch_norm[IFM / PARALLEL],
	vx_image_data<TypeData, PARALLEL> buffer_input[(IFM / PARALLEL)],
	vx_image_data<TypeData, PARALLEL* WEIGHT_SIZE> buffer_weights[(IFM / PARALLEL)],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const vx_float32 SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const vx_float32 INV_SHIFT = (1.0f / SHIFT);
	const vx_int32 IFM_VECTOR = IFM / PARALLEL;
	const vx_int32 PIXEL_VECTOR = PIXELS * IFM_VECTOR;

	// Variables
	vx_float32 variance = 0, norm = 0;
	vx_image_data<TypeData, WEIGHT_SIZE* PARALLEL> weights;
	vx_image_data<TypeData, PARALLEL> input;
	vx_image_data<TypeData, PARALLEL> output;

	// Get input vector from buffer
	input = buffer_input[ifmv];

	// Weight vector
	if (BUFFER_WEIGHTS == true) {
		if (i == 2) {
			weights = batch_norm[ifmv];
			buffer_weights[ifmv] = weights;
		} else {
			weights = buffer_weights[ifmv];
		}
	} else {
		weights = batch_norm[ifmv];
	}

	// Precompute part of Normalization
	if (is_integer == true) {
		variance = (vx_float32)variance_int * INV_SHIFT;
	} else {
		variance = variance_flt * INV_SHIFT;
	}
	norm = 1.0f / sqrtf(variance + 0.000001f); // FP_POSITION = 0

	// Vectorization
	for (vx_int32 ifmp = 0; ifmp < PARALLEL; ++ifmp) {
		vx_float32 mean = 0, normalize = 0;
		TypeData result = 0, gamma = 0, beta = 0, pixel = 0;

		// Signel Input/Weight
		pixel = input.pixel[ifmp];
		gamma = weights.pixel[ifmp * WEIGHT_SIZE + 0];
		beta = weights.pixel[ifmp * WEIGHT_SIZE + 1];

		// Compute Normalization
		if (is_integer == true) {
			mean = (vx_float32)((vx_int32)pixel - mean_int);
		} else {
			mean = ((vx_float32)pixel * SHIFT - mean_flt) * INV_SHIFT;
		}
		normalize = mean * norm; // FP_POSITION = 0

		// Scale and Shift
		if (is_integer == true) {

			// Multiply with gamma
			vx_int64 scale = (vx_int64)gamma * (vx_int64)normalize;

			// Add beta
			vx_int32 shift = (vx_int32)(scale >> FIXED_POINT_POSITION) + (vx_int32)beta;

			// Check for overflow
			if (OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE) {
				vx_int32 maximum = (vx_int32)ComputeMax<TypeData>();
				vx_int32 minimum = (vx_int32)ComputeMin<TypeData>();
				shift = MAX(shift, minimum);
				shift = MIN(shift, maximum);
			}
			result = (TypeData)shift;

		} else {
			vx_float32 data = (vx_float32)gamma * normalize + (vx_float32)beta;
			result = (TypeData)data;
		}		
		output.pixel[ifmp] = result;
	}

	//
	GenerateDmaSignal<TypeData, PARALLEL>(((dst_ptr % PIXEL_VECTOR) == 0), ((dst_ptr % PIXEL_VECTOR) == PIXEL_VECTOR - 1), output);
	dst[dst_ptr] = output;
	++dst_ptr;
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int32 BATCHES, vx_int32 PIXELS, vx_int32 IFM, vx_int32 PARALLEL, vx_int32 FIXED_POINT_POSITION,
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int32 WEIGHT_SIZE, bool BUFFER_WEIGHTS>
void BatchNormalization2(
	vx_image_data<TypeData, PARALLEL * WEIGHT_SIZE> batch_norm[IFM / PARALLEL],
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * PIXELS * IFM) / PARALLEL],
	vx_image_data<TypeData, PARALLEL> dst[(BATCHES * PIXELS * IFM) / PARALLEL]) {
#pragma HLS INLINE

	// Buffers Inputs & Weights
	vx_image_data<TypeData, PARALLEL> buffer_input[3][(IFM / PARALLEL)];
#pragma HLS array_partition variable=buffer_input complete dim=1
#pragma HLS RESOURCE variable=buffer_input core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=buffer_input
	vx_image_data<TypeData, PARALLEL* WEIGHT_SIZE> buffer_weights[(IFM / PARALLEL)];
#pragma HLS RESOURCE variable=buffer_weights core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=buffer_weights

	// Global Variables
	vx_float32 mean_flt[3], variance_flt[3];
	vx_int32 mean_int[3], variance_int[3];
	vx_int32 sum1_total_int = 0, sum2_total_int = 0;
	vx_int64 sum1_total_flt = 0, sum2_total_flt = 0;
	vx_int32 src_ptr = 0, dst_ptr = 0;
#pragma HLS array_partition variable=mean_flt complete dim=0
#pragma HLS array_partition variable=variance_flt complete dim=0
#pragma HLS array_partition variable=mean_int complete dim=0
#pragma HLS array_partition variable=variance_int complete dim=0	

	// Compute Batch Normalization
	for (vx_int32 i = 0; i < (BATCHES * PIXELS + 2); ++i) {
		for (vx_int32 ifmv = 0; ifmv < (IFM / PARALLEL); ++ifmv) {
#pragma HLS pipeline II=1

			//
			const vx_int32 stage0 = 2-((i+2) % 3);
			const vx_int32 stage1 = 2-((i+1) % 3);
			const vx_int32 stage2 = 2-((i+0) % 3);

			//
			if ((i >= 0) && (i < (BATCHES * PIXELS + 0)))
				BatchNormMean<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION>(i, ifmv, sum1_total_int, sum1_total_flt, mean_int[stage0], mean_flt[stage0], src_ptr, src, buffer_input[stage0]);
			if ((i >= 1) && (i < (BATCHES * PIXELS + 1)))
				BatchNorm2Variance<TypeData, IFM, PARALLEL, FIXED_POINT_POSITION>(ifmv, mean_flt[stage1], mean_int[stage1], variance_flt[stage1], variance_int[stage1], sum2_total_int, sum2_total_flt, buffer_input[stage1]);
			if ((i >= 2) && (i < (BATCHES * PIXELS + 2)))
				BatchNorm2Normalize<TypeData, BATCHES, PIXELS, IFM, PARALLEL, FIXED_POINT_POSITION, OVERFLOW_POLICY, WEIGHT_SIZE, BUFFER_WEIGHTS>(i, ifmv, mean_flt[stage2], variance_flt[stage2], mean_int[stage2], variance_int[stage2], dst_ptr, batch_norm, buffer_input[stage2], buffer_weights, dst);
		}
	}
}

/***************************************************************************************************************************************************/
// Convert from floating point to fixed incl. shifting and rounding
template<typename TypeDst, vx_round_policy_e ROUNDING_POLICY>
TypeDst RoundingFloatToFixed(float input, float shift) {
#pragma HLS INLINE

	float data = input * shift;
	if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
		data = data + 0.5f;
	return (TypeDst)data;
}

/***************************************************************************************************************************************************/
// Check fixed number for overflow
template<typename TypeDst, vx_convert_policy_e OVERFLOW_POLICY>
TypeDst OverflowFixed(vx_int64 input) {
#pragma HLS INLINE

	vx_int64 data = input;
	if (OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE)
		data = MIN(data, (vx_int64)ComputeMax<TypeDst>());
	return (TypeDst)data;
}

/***************************************************************************************************************************************************/
// Round a fixed number, which needs to be shifted
template<vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY>
vx_int64 RoundingFixed(vx_int64 input) {
#pragma HLS INLINE

	if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO) {
		return input >> FIXED_POINT_POSITION;
	} else if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
		const vx_int64 round = ((vx_int64)1 << (FIXED_POINT_POSITION - 1));
		return (input + round) >> FIXED_POINT_POSITION;
	}
}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION,
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_nn_activation_function_e ACTIVATION_FUNCTION>
void ActivationLayerCheck() {
#pragma HLS INLINE

	// Constants
	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const bool interger_size = ((digits == 7) || (digits == 8) || (digits == 15) || (digits == 16));
	const vx_int32 digits_fp_max = (ACTIVATION_FUNCTION == VX_NN_ACTIVATION_SOFTRELU) ? (digits - 1) : (digits);

	// Compile time checks
	STATIC_ASSERT((BATCHES > 0) && (PIXELS > 0) && (PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
	STATIC_ASSERT(((is_integer == true) && (interger_size == true)) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
	STATIC_ASSERT(ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO || ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN, ROUNDING_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT(OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE || OVERFLOW_POLICY == VX_CONVERT_POLICY_WRAP, OVERFLOW_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT((FIXED_POINT_POSITION >= 0) && (FIXED_POINT_POSITION <= digits_fp_max), FIXED_POINT_POSITION_OUT_OF_SCOPE);
	STATIC_ASSERT(((PIXELS % PARALLEL) == 0) && (PIXELS >= PARALLEL), PIXELS_NOT_MULTIPLE_OF_PARALLELIZATION);
	STATIC_ASSERT(ACTIVATION_FUNCTION == VX_NN_ACTIVATION_LOGISTIC || ACTIVATION_FUNCTION == VX_NN_ACTIVATION_HYPERBOLIC_TAN ||
		ACTIVATION_FUNCTION == VX_NN_ACTIVATION_RELU || ACTIVATION_FUNCTION == VX_NN_ACTIVATION_BRELU ||
		ACTIVATION_FUNCTION == VX_NN_ACTIVATION_SOFTRELU || ACTIVATION_FUNCTION == VX_NN_ACTIVATION_ABS ||
		ACTIVATION_FUNCTION == VX_NN_ACTIVATION_SQUARE || ACTIVATION_FUNCTION == VX_NN_ACTIVATION_SQRT ||
		ACTIVATION_FUNCTION == VX_NN_ACTIVATION_LINEAR, ACTIVATION_FUNCTION_NOT_SUPPORTED);

}

/***************************************************************************************************************************************************/
//
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, 
	vx_nn_activation_function_e ACTIVATION_FUNCTION, bool RELAXED_MATH>
TypeData ActivationLayerCompute(TypeData param_a, TypeData param_b, TypeData input) {
#pragma HLS INLINE

	// Constants
	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
	const bool is_signed = std::numeric_limits<TypeData>::is_signed;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const float INV_SHIFT = (1.0f / SHIFT);

	// Variables
	TypeData variable = input;
	TypeData result = 0;

	// Check th different activation functions
	switch (ACTIVATION_FUNCTION) {

	// f(x) = 1 / (1+e^(-x))
	case VX_NN_ACTIVATION_LOGISTIC:
		if (is_integer == true) {

			// Convert to floating point				
			float data = (float)variable * INV_SHIFT;

			// Calculate function
			data = 1.0f / (1.0f + expf(-data));

			// Convert back to fixed point (no overflow possible in this function)
			result = RoundingFloatToFixed<TypeData, ROUNDING_POLICY>(data, SHIFT);

		} else {
			result = (TypeData)(1.0f / (1.0f + expf(static_cast<float>(-variable))));
		}
		break;

	// f(x) = a * tanh(b * x) = a * (e^(2x) - 1) / (e^(2x) + 1)
	case VX_NN_ACTIVATION_HYPERBOLIC_TAN:
		if (is_integer == true) {
				
			// Convert input data to floating point
			const float data = (float)variable * (INV_SHIFT * 2.0f);
			const float a = (float)param_a * INV_SHIFT;
			const float b = (float)param_b * INV_SHIFT;

			// Calculate function
			const float mul = b * data;
			const float exp = expf(mul);
			const float add = exp + 1.0f;
			const float sub = exp - 1.0f;
			const float div = a * (sub / add);

			// Convert back to fixed point (no overflow possible in this function)
			result = RoundingFloatToFixed<TypeData, ROUNDING_POLICY>(div, SHIFT);
		} else {
			const float mul = 2.0f * param_b * variable;
			const float exp = expf(mul);
			const float add = exp + 1.0f;
			const float sub = exp - 1.0f;
			result = (TypeData)(param_a * (sub / add));
		}			
		break;

	// f(x) = max(0,x)
	case VX_NN_ACTIVATION_RELU:
		result = MAX(variable, static_cast<TypeData>(0));
		break;

	// f(x) = min(a, max(0,x))
	case VX_NN_ACTIVATION_BRELU:
		result = MAX(variable, static_cast<TypeData>(0));
		result = MIN(param_a, result);
		break;

	// f(x) = log(1 + e^(x))
	case VX_NN_ACTIVATION_SOFTRELU:
		if (is_integer == true) {

			// Convert to floating point
			float data0 = (float)variable * INV_SHIFT;

			// Calculate function
			data0 = logf(1.0f + expf(data0));

			// Convert back to fixed point
			vx_int64 data1 = RoundingFloatToFixed<vx_int64, ROUNDING_POLICY>(data0, SHIFT);

			// Check for overflow
			result = OverflowFixed<TypeData, OVERFLOW_POLICY>(data1);

		} else {
			result = (TypeData)(logf(1.0f + expf(variable)));
		}
		break;

	// f(x) = |x|
	case VX_NN_ACTIVATION_ABS:
		if (is_integer == true) {
			if ((OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE) && (variable == ComputeMin<TypeData>()) && (is_signed == true))
				result = ComputeMax<TypeData>();
			else
				result = ABS(variable);
		} else {
			result = ABS(variable);
		}
		break;

	// f(x) = x^(2)
	case VX_NN_ACTIVATION_SQUARE:
		if (is_integer == true) {

			// Calculate function
			vx_int64 data = (vx_int64)variable * (vx_int64)variable;

			// Rounding				
			data = RoundingFixed<FIXED_POINT_POSITION, ROUNDING_POLICY>(data);

			// Check for overflow
			result = OverflowFixed<TypeData, OVERFLOW_POLICY>(data);
		} else {
			result = variable * variable;
		}
		break;

	// f(x) = sqrt(x)
	case VX_NN_ACTIVATION_SQRT:
		if (is_integer == true) {
			const vx_int32 SHIFT = (RELAXED_MATH == true) ? (0) : (FIXED_POINT_POSITION);

			// Shift data by fixed point position to get accurate square root results
			vx_uint32 data = (vx_uint32)variable << (vx_uint32)SHIFT;

			// Calculate function(For Square Root no overflow can occur)
#if defined(__SYNTHESIS__)
			data = hls::sqrt((vx_uint32)data);
#else
			const vx_uint8 SQRT_OUTPUT_DIGITS = (vx_uint8)((SHIFT + digits) / 2);
			data = SqrtLester<vx_uint32, vx_uint32, false, ROUNDING_POLICY, SQRT_OUTPUT_DIGITS>(data);
#endif

			// Shift to correct fixed point position
			result = (TypeData)(data << ((FIXED_POINT_POSITION - SHIFT) / 2));

		} else {
			result = (TypeData)sqrtf(variable);
		}
		break;

	// f(x) = a*x + b
	case VX_NN_ACTIVATION_LINEAR:
			
		if (is_integer == true) {

			// Calculate function (multiplication)
			vx_int64 data = (vx_int64)param_a * (vx_int64)variable;

			// Rounding				
			data = RoundingFixed<FIXED_POINT_POSITION, ROUNDING_POLICY>(data);

			// Calculate function (addition)
			data = data + (vx_int64)param_b;

			// Check for overflow
			result = OverflowFixed<TypeData, OVERFLOW_POLICY>(data);
		} else {
			result = param_a * variable + param_b;
		}
		break;
	}

	// Store result in array
	return result;
}

/***************************************************************************************************************************************************/
// The activation layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, 
	vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_nn_activation_function_e ACTIVATION_FUNCTION, bool RELAXED_MATH>
void ActivationLayer(TypeData param_a, TypeData param_b,
	vx_image_data <TypeData, PARALLEL> src[(BATCHES * PIXELS) / PARALLEL],
	vx_image_data <TypeData, PARALLEL> dst[(BATCHES * PIXELS) / PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 PIXELS_VECTOR = PIXELS / PARALLEL;

	// Compile time checks
	ActivationLayerCheck<TypeData, BATCHES, PIXELS, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION>();

	// Iterate over all elements
	for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
		for (vx_int64 i = 0; i < PIXELS_VECTOR; ++i) {
#pragma HLS pipeline II=1

			// To buffer the complete input/output vector
			vx_image_data<TypeData, PARALLEL> input;
//#pragma HLS array_partition variable=input complete dim=0
			vx_image_data <TypeData, PARALLEL> output;
//#pragma HLS array_partition variable=output complete dim=0

			// Read the input data vector
			input = src[batch * PIXELS_VECTOR + i];

			// Parallelize this computation for the PARALLEL degree
			for (vx_int64 j = 0; j < PARALLEL; ++j) 				
				output.pixel[j] = ActivationLayerCompute<TypeData, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(param_a, param_b, input.pixel[j]);		

			// Write to output
			GenerateDmaSignal<TypeData, PARALLEL>((i == 0), (i == PIXELS_VECTOR - 1), output);
			dst[batch * PIXELS_VECTOR + i] = output;
		}
	}
}

/***************************************************************************************************************************************************/
// The activation layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_nn_activation_function_e ACTIVATION_FUNCTION, bool RELAXED_MATH>
void ActivationLayer(TypeData param_a, TypeData param_b,
		TypeData src[(BATCHES * PIXELS) / PARALLEL],
		TypeData dst[(BATCHES * PIXELS) / PARALLEL]) {
#pragma HLS INLINE

	// Compile time checks
	ActivationLayerCheck<TypeData, BATCHES, PIXELS, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION>();

	// Iterate over all elements
	for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
		for (vx_int64 i = 0; i < (PIXELS / PARALLEL); ++i) {
#pragma HLS pipeline II=1

			// To buffer the complete input/output vector
			TypeData input, output;

			// Read the input data vector
			input = src[batch * (PIXELS / PARALLEL) + i];

			// Parallelize this computation for the PARALLEL degree
			output = ActivationLayerCompute<TypeData, FIXED_POINT_POSITION, ROUNDING_POLICY, OVERFLOW_POLICY, ACTIVATION_FUNCTION, RELAXED_MATH>(param_a, param_b, input);

			// Write to output			
			dst[batch * (PIXELS / PARALLEL) + i] = output;		
		}
	}
}

/***************************************************************************************************************************************************/
// The Softmax Layer
template<typename TypeData, vx_int64 BATCHES, vx_int64 IFM, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY>
void SoftmaxLayer(
	vx_image_data<TypeData, PARALLEL> src[(BATCHES * IFM) / PARALLEL], 
	vx_image_data <TypeData, PARALLEL> dst[(BATCHES * IFM) / PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 FM_VECTOR = IFM / PARALLEL;
	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const bool interger_size = ((digits == 7) || (digits == 8) || (digits == 15) || (digits == 16));
	const float SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const float INV_SHIFT = (1.0f / SHIFT);
	const float MAX_INT31 = 1073741824.0f;
	const float MIN_INT31 = -1073741824.0f;	

	// Compile time checks
	STATIC_ASSERT((BATCHES > 0) && (IFM > 0) && (PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
	STATIC_ASSERT(((is_integer == true) && (interger_size == true)) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
	STATIC_ASSERT(ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO || ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN, ROUNDING_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT(FIXED_POINT_POSITION >= 0 && FIXED_POINT_POSITION <= digits, FIXED_POINT_POSITION_OUT_OF_SCOPE);
	STATIC_ASSERT((IFM % PARALLEL) == 0 && (IFM >= PARALLEL), TOTAL_ELEMENTS_NOT_MULTIPLE_OF_PARALLELIZATION);

	// Buffers the exponential of all incoming values
	vx_image_data <float, PARALLEL> buffer[FM_VECTOR];
#pragma HLS data_pack variable=buffer
#pragma HLS RESOURCE variable=buffer core=RAM_2P_LUTRAM

	// Softmax Computation
	for (vx_int64 b = 0; b < BATCHES; ++b) {
		for (vx_int64 i = 0, sum = 0; i < 2; ++i) {
			for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

				// Computes: n(i) = exp(src(i)) | N: sum of all n
				if (i == 0) {

					// To buffer the complete output vector
					vx_image_data <float, PARALLEL> output;
//#pragma HLS array_partition variable=output complete dim=0

					// To buffer the complete PARALLEL vector
					vx_image_data <TypeData, PARALLEL> input;
//#pragma HLS array_partition variable=input complete dim=0

					// Read the input data vector
					input = src[b * FM_VECTOR + fmv];

					// Needed to compute the some of all exponentials
					vx_int64 part_sum = 0;

					// Parallelize this computation for the exponential function
					for (vx_int64 fmp = 0; fmp < PARALLEL; ++fmp) {

						// Variables
						TypeData variable1 = input.pixel[fmp];

						// Convert to floating point if needed
						const float variable2 = (is_integer == true) ? ((float)variable1 * INV_SHIFT) : (variable1);

						// Compute exponential of input value
						const float exponential = expf(variable2);

						// Convert the sum to float
						float data = exponential * SHIFT;
						//float data = ldexpf(exponential, FIXED_POINT_POSITION);
						data = MAX(MIN(data, MAX_INT31), MIN_INT31);

						// Add to the sum of all exponentials
						part_sum += static_cast<vx_int32>(data);

						// Buffer exponential result
						output.pixel[fmp] = exponential;
					}

					// Add to the sum of all exponentials
					sum += part_sum;

					// Buffer exponential result
					buffer[fmv] = output;

				// Computes: m(i) = n(i) / N
				} else {

					// To buffer the complete output vector
					vx_image_data <TypeData, PARALLEL> output;
//#pragma HLS array_partition variable=output complete dim=0

					// To buffer the complete input vector
					vx_image_data <float, PARALLEL> input;
//#pragma HLS array_partition variable=input complete dim=0

					// Read the exponentials of "PARALLEL" input values
					input = buffer[fmv];

					// Parallelize this computation
					for (vx_int64 j = 0; j < PARALLEL; ++j) {
						float result = (input.pixel[j] * SHIFT) / static_cast<float>(sum);

						// Convert back to fixed if needed
						if (is_integer == true)
							output.pixel[j] = RoundingFloatToFixed<TypeData, ROUNDING_POLICY>(result, SHIFT);
						else
							output.pixel[j] = (TypeData)result;
					}

					// Write back result vector
					GenerateDmaSignal<TypeData, PARALLEL>((fmv == 0), (fmv == FM_VECTOR - 1), output);
					dst[b * FM_VECTOR + fmv] = output;
				}
			}
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 BATCHES, vx_int64 IFM_VECTOR, vx_int64 OFM, vx_int64 BIASES_SIZE, bool BUFFER_COEFFICIENTS>
void FullyConnectedAddBias(vx_int64 batch, vx_int64 row, vx_int64 col, TypeComp& result,
	vx_image_data<TypeBias, 1> biasbuffer[OFM],
	vx_image_data<TypeBias, 1> biases[OFM]) {
#pragma HLS INLINE

	// Add the bias to the ofm pixel
	if (col == (IFM_VECTOR - 1)) {

		// Buffers input vector of biases
		vx_image_data<TypeBias, 1> bias;
//#pragma HLS array_partition variable=bias complete dim=0
		for (vx_int64 i = 0; i < 1; ++i)
			bias.pixel[i] = 0;

		// Get the bias and buffer it if demanded
		if (BIASES_SIZE == OFM) {
			if (BUFFER_COEFFICIENTS == true) {
				if (batch == 0) {
					bias = biases[row];
					biasbuffer[row] = bias;
				} else {
					bias = biasbuffer[row];
				}
			} else {
				bias = biases[row];
			}
		} else if (BIASES_SIZE == (BATCHES * OFM)) {
			bias = biases[batch * OFM + row];
		}

		// Add the bias
		result += (TypeComp)bias.pixel[0];
	}
}


/***************************************************************************************************************************************************/
// Fully connected operation (Fixed Point)
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM_VECTOR, vx_int64 OFM, vx_int64 BIASES_SIZE, vx_int64 PARALLEL,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_COEFFICIENTS>
TypeData FullyConnectedFixed(vx_int64 batch, vx_int64 row, vx_int64 col, vx_int64& sum_fixed, 
	vx_image_data<TypeData, PARALLEL> input,
	vx_image_data<TypeData, PARALLEL> input_weights,
	vx_image_data<TypeBias, 1> biasbuffer[OFM],
	vx_image_data<TypeBias, 1> biases[OFM]) {
#pragma HLS INLINE

	// Variables
	vx_int64 part_sum = 0;
	vx_int64 result = 0;

	// Compute (update) the sum	
	for (vx_int64 p = 0; p < PARALLEL; ++p)
		part_sum += static_cast<vx_int64>(input_weights.pixel[p])* static_cast<vx_int64>(input.pixel[p]);
	sum_fixed += part_sum;

	// Shift the sum back to original fixed point position (incl. rounding)
	result = RoundingFixed<FIXED_POINT_POSITION, ROUNDING_POLICY>(sum_fixed);

	// Add the bias to the ofm pixel
	FullyConnectedAddBias<TypeData, TypeBias, vx_int64, BATCHES, IFM_VECTOR, OFM, BIASES_SIZE, BUFFER_COEFFICIENTS>(batch, row, col, result, biasbuffer, biases);

	// Check for overflow
	result = OverflowFixed<TypeData, OVERFLOW_POLICY>(result);

	// Convert to output data type
	return static_cast<TypeData>(result);
}

/***************************************************************************************************************************************************/
// Fully connected operation (Floating Point)
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM_VECTOR, vx_int64 OFM, vx_int64 BIASES_SIZE, vx_int64 PARALLEL, vx_int64 FIXED_POINT_POSITION, bool BUFFER_COEFFICIENTS>
TypeData FullyConnectedFloat(vx_int64 batch, vx_int64 row, vx_int64 col, vx_int64& sum_fixed,
	vx_image_data<TypeData, PARALLEL> input, 
	vx_image_data<TypeData, PARALLEL> input_weights, 
	vx_image_data<TypeBias, 1> biasbuffer[OFM],
	vx_image_data<TypeBias, 1> biases[OFM]) {
#pragma HLS INLINE

	// Constants
	const float MAX_INT31 = 1073741824.0f;
	const float MIN_INT31 = -1073741824.0f;
	const float SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const float INV_SHIFT = (1.0f / SHIFT);

	// Variables
	vx_int64 part_sum = 0;
	float result = 0;

	// Compute (update) the sum	
	for (vx_int64 p = 0; p < PARALLEL; ++p) {
		float mul = static_cast<float>(input_weights.pixel[p] * input.pixel[p]);
		float shift = mul * SHIFT;
		float saturate = MAX(MIN(shift, MAX_INT31), MIN_INT31);
		vx_int64 convert = static_cast<vx_int64>(static_cast<vx_int32>(saturate));		
		part_sum += convert;
	}
	sum_fixed += part_sum;	

	// Read and add the bias value
	result = static_cast<float>(sum_fixed) * INV_SHIFT;

	// Add the bias to the ofm pixel
	FullyConnectedAddBias<TypeData, TypeBias, float, BATCHES, IFM_VECTOR, OFM, BIASES_SIZE, BUFFER_COEFFICIENTS>(batch, row, col, result, biasbuffer, biases);

	// Convert to output data type
	return static_cast<TypeData>(result);
}

/***************************************************************************************************************************************************/
// Fully connected layer
template<typename TypeData, typename TypeBias, vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 PARALLEL, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE,
	vx_int64 FIXED_POINT_POSITION, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, bool BUFFER_COEFFICIENTS> 
void FullyConnected (
	vx_image_data<TypeData, PARALLEL> src[BATCHES * (IFM / PARALLEL)], 
	vx_image_data<TypeData, PARALLEL> weights[WEIGHTS_SIZE / PARALLEL],
	vx_image_data<TypeBias, 1> biases[BIASES_SIZE],
	vx_image_data<TypeData, 1> dst[BATCHES * OFM]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 IFM_VECTOR = IFM / PARALLEL;
	const vx_int32 digits = std::numeric_limits<TypeData>::digits;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;	
	const bool interger_size = (digits == 7) || (digits == 8) || (digits == 15) || (digits == 16);

	// Compile time checks
	STATIC_ASSERT((BATCHES > 0) && (IFM > 0) && (OFM > 0) && (PARALLEL > 0), ONE_OF_THE_TEMPLATE_PARAMETERS_IS_ZERO);
	STATIC_ASSERT(((is_integer == true) && (interger_size == true)) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
	STATIC_ASSERT(ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO || ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN, ROUNDING_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT(OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE || OVERFLOW_POLICY == VX_CONVERT_POLICY_WRAP, OVERFLOW_POLICY_IS_NOT_SUPPORTED);
	STATIC_ASSERT((FIXED_POINT_POSITION >= 0) && (FIXED_POINT_POSITION <= digits), FIXED_POINT_POSITION_NOT_VALID);
	STATIC_ASSERT((IFM % PARALLEL) == 0 && (IFM >= PARALLEL), IFM_NOT_MULTIPLE_OF_PARALLELIZATION);
	STATIC_ASSERT((BIASES_SIZE == 0) || (BIASES_SIZE == OFM) || (BIASES_SIZE == BATCHES * OFM), BIASES_SIZE_NOT_VALID);
	STATIC_ASSERT((WEIGHTS_SIZE == (OFM * IFM)) || (WEIGHTS_SIZE == (BATCHES * OFM * IFM)), WEIGHTS_SIZE_NOT_VALID);
	STATIC_ASSERT(((BUFFER_COEFFICIENTS == true) && (WEIGHTS_SIZE == (OFM * IFM))) || (BUFFER_COEFFICIENTS == false), WEIGHTS_CAN_ONLY_BE_BUFFERED_IF_WEIGHT_SIZE_IS_IFM_X_OFM);
	STATIC_ASSERT(((BUFFER_COEFFICIENTS == true) && (BIASES_SIZE == OFM)) || (BUFFER_COEFFICIENTS == false), BIASES_CAN_ONLY_BE_BUFFERED_IF_WEIGHT_SIZE_IS_OFM);

	// Bias and others need to have the same data type
	const bool is_integer_data = std::numeric_limits<TypeData>::is_integer;
	const bool is_integer_bias = std::numeric_limits<TypeBias>::is_integer;
	STATIC_ASSERT(is_integer_data == is_integer_bias, BIAS_AND_REST_MUST_BOTH_BE_INTEGER_OR_FLOAT);

	// Buffers the complete input vector
	vx_image_data<TypeData, PARALLEL> inputbuffer[IFM_VECTOR];
#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete input vector
	vx_image_data<TypeBias, 1> biasbuffer[OFM];
#pragma HLS RESOURCE variable=biasbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=biasbuffer

	// Buffers the complete input vector
	vx_image_data<TypeData, PARALLEL> weightsbuffer[(OFM * IFM) / PARALLEL];
#pragma HLS RESOURCE variable=weightsbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=weightsbuffer

	// Variable to store: Vector * (1 Columns of Matrix)
	vx_int64 sum_fixed = 0;

	// Iterate over the matrix
	for(vx_int64 batch = 0; batch < BATCHES; ++batch) {
		for (vx_int64 row = 0; row < OFM; ++row) {
			for (vx_int64 col = 0; col < IFM_VECTOR; ++col) {
#pragma HLS pipeline II=1

				// Buffers the output vector
				vx_image_data<TypeData, 1> output;
//#pragma HLS array_partition variable=output complete dim=0

				// Buffers the input vector
				vx_image_data <TypeData, PARALLEL> input;
//#pragma HLS array_partition variable=input complete dim=0

				// Buffers the input weights vector
				vx_image_data <TypeData, PARALLEL> input_weights;
//#pragma HLS array_partition variable=input_weights complete dim=0

				// Read the input data vector
				if (row == 0)
					input = src[batch * IFM_VECTOR + col];
				else
					input = inputbuffer[col];

				// Read the weights data vector (buffer them if BUFFER_COEFFICIENTS flag is set)
				if (WEIGHTS_SIZE == (OFM * IFM)) {
					const vx_int64 weights_ptr = row * IFM_VECTOR + col;
					if (BUFFER_COEFFICIENTS == true) {
						if (batch == 0) {
							input_weights = weights[weights_ptr];
							weightsbuffer[weights_ptr] = input_weights;
						} else {
							input_weights = weightsbuffer[weights_ptr];
						}
					} else {
						input_weights = weights[weights_ptr];
					}
				} else if (WEIGHTS_SIZE == (BATCHES * OFM * IFM)) {
					input_weights = weights[batch * OFM * IFM_VECTOR + row * IFM_VECTOR + col];
				}

				// Reset sums at start of new column
				if (col == 0)
					sum_fixed = 0;

				// Compute: Vector * (1 Columns of Matrix)
				if (is_integer == true) {
					output.pixel[0] = FullyConnectedFixed<TypeData, TypeBias, BATCHES, IFM_VECTOR, OFM, BIASES_SIZE, PARALLEL, FIXED_POINT_POSITION, ROUNDING_POLICY,
						OVERFLOW_POLICY, BUFFER_COEFFICIENTS>(batch, row, col, sum_fixed, input, input_weights, biasbuffer, biases);
				} else {				
					output.pixel[0] = FullyConnectedFloat<TypeData, TypeBias, BATCHES, IFM_VECTOR, OFM, BIASES_SIZE, PARALLEL, FIXED_POINT_POSITION, BUFFER_COEFFICIENTS>
						(batch, row, col, sum_fixed, input, input_weights, biasbuffer, biases);
				}

				// Buffer the input vector
				if (row == 0)
					inputbuffer[col] = input;

				// Write result to the output
				GenerateDmaSignal<TypeData, 1>((row == 0), (row == (OFM - 1)), output);
				if (col == (IFM_VECTOR - 1)) {
					dst[batch * OFM + row] = output;
				}
			}
		}
	}
}

#endif // NN_CORE2_H_
