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

#ifndef NN_CORE1_H_
#define NN_CORE1_H_

#include "vx_helper.h"

/***************************************************************************************************************************************************/
// Read input pixel from input image
template<typename TypeData, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 BATCHES, vx_int64 IFM_PARALLEL>
void ReadInput(vx_int64 y, vx_int64 x, vx_int64 ofmv, 
	vx_image_data<TypeData, IFM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, IFM_PARALLEL>& input, vx_int64& src_ptr) {
#pragma HLS INLINE

	// Initialize the input vector before first use
	for (vx_int64 i = 0; i < IFM_PARALLEL; ++i) {
		input.pixel[i] = 0;
	}

	// Read input pixel from input image
	if ((y < INPUT_HEIGHT) && (x < INPUT_WIDTH) && (ofmv == 0)) {
		input = src[src_ptr];		
		++src_ptr;
	}
}
template<typename TypeData, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 BATCHES, vx_int64 IFM_PARALLEL>
void ReadInput(vx_int64 y, vx_int64 x, vx_int64 ofmv,
	TypeData src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, IFM_PARALLEL>& input, vx_int64& src_ptr) {
#pragma HLS INLINE

	// Initialize the input vector before first use
	for (vx_int64 i = 0; i < IFM_PARALLEL; ++i) {
		input.pixel[i] = 0;
	}

	// Read input pixel from input image
	if ((y < INPUT_HEIGHT) && (x < INPUT_WIDTH) && (ofmv == 0)) {
		TypeData data = src[src_ptr];
		input.pixel[0] = data;
		++src_ptr;
	}
}

/***************************************************************************************************************************************************/
// Read data and write it into window
template<typename TypeData, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 IFM_PARALLEL, 
	vx_int64 LINEBUFFER_Y, vx_int64 WINDOWBUFFER_X>
	void UpdateWindow(vx_int64 y, vx_int64 x, vx_int64 ofmv, vx_int64 ifmv,
		vx_image_data<TypeData, IFM_PARALLEL> input,
		vx_image_data<TypeData, IFM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * (IFM / IFM_PARALLEL)],
		vx_image_data<TypeData, IFM_PARALLEL> inputbuffer[IFM / IFM_PARALLEL],
		vx_image_data<TypeData, IFM_PARALLEL> windowbuffer[IFM / IFM_PARALLEL][WINDOWBUFFER_X * KERNEL_Y],
		vx_image_data<TypeData, IFM_PARALLEL> window[KERNEL_Y * KERNEL_X]) {
#pragma HLS INLINE

	// Buffers a linebuffer column
	vx_image_data<TypeData, IFM_PARALLEL * LINEBUFFER_Y> linebuffer_data;
	for (vx_int64 i = 0; i < (IFM_PARALLEL * LINEBUFFER_Y); ++i)
		linebuffer_data.pixel[i] = 0;

	// Read complete column from linebuffer
	if ((x < INPUT_WIDTH) && (KERNEL_Y > 0))
		linebuffer_data = linebuffer[(x * (IFM / IFM_PARALLEL)) + ifmv];

	// Read data and write it into window
	for (vx_int64 i = 0; i < KERNEL_Y; ++i) {
		for (vx_int64 j = 0; j < KERNEL_X; ++j) {

			// Used for zero padding
			vx_image_data<TypeData, IFM_PARALLEL> data;
			for (vx_int64 k = 0; k < IFM_PARALLEL; ++k) 
				data.pixel[k] = 0;	

			// Get input from (linebuffer and (inputbuffer or input))
			if (j == 0) {

				// Only read if input is valid, else output zero
				if (x < INPUT_WIDTH) {

					// Read from input into row 0 of window
					if (i == 0) {

						// Only read if input is valid, else output zero
						if (y < INPUT_HEIGHT) {

							// Read from input on 1. ofm
							if (ofmv == 0)
								data = input;

							// Otherwise read from inputbuffer (which buffers the dame value)
							else
								data = inputbuffer[ifmv];
						}

					// Read from linebuffer into rows other than row 0
					} else {

						// Only read from valid linebuffer
						if (y >= i) {
							for (vx_int64 k = 0; k < IFM_PARALLEL; ++k)
								data.pixel[k] = linebuffer_data.pixel[(i - 1) * IFM_PARALLEL + k];
						}
					}
				}

			// Get input from windowbuffer
			} else {
				if (j < (x + 1))
					data = windowbuffer[ifmv][(i * (KERNEL_X - 1)) + (j - 1)];
			}

			// Store data into window							
			window[(i * KERNEL_X) + j] = data;
		}
	}
}

/***************************************************************************************************************************************************/
// Store data from window into (windowbuffer and (linebuffer or inputbuffer)

template<typename TypeData, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL,
	vx_int64 LINEBUFFER_Y, vx_int64 WINDOWBUFFER_X>
void UpdateBuffers(vx_int64 y, vx_int64 x, vx_int64 ofmv, vx_int64 ifmv,
	vx_image_data<TypeData, IFM_PARALLEL> window[KERNEL_Y * KERNEL_X],
	vx_image_data<TypeData, IFM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, IFM_PARALLEL> inputbuffer[IFM / IFM_PARALLEL],
	vx_image_data<TypeData, IFM_PARALLEL> windowbuffer[IFM / IFM_PARALLEL][WINDOWBUFFER_X * KERNEL_Y]) {
#pragma HLS INLINE

	// Buffers a linebuffer column
	vx_image_data<TypeData, IFM_PARALLEL * LINEBUFFER_Y> linebuffer_data;
	for (vx_int64 i = 0; i < (IFM_PARALLEL * LINEBUFFER_Y); ++i)
		linebuffer_data.pixel[i] = 0;	

	// Store data from window into (windowbuffer and (linebuffer or inputbuffer)
	for (vx_int64 i = 0; i < KERNEL_Y; ++i) {
		for (vx_int64 j = 0; j < KERNEL_X; ++j) {

			// Store data into (linebuffer or inputbuffer)
			if (j == 0) {

				if (ofmv == ((OFM / OFM_PARALLEL) - 1)) {
					if ((i > 0) && (x < INPUT_WIDTH))
						for (vx_int64 k = 0; k < IFM_PARALLEL; ++k) 
							linebuffer_data.pixel[(i - 1) * IFM_PARALLEL + k] = window[(i - 1) * KERNEL_X].pixel[k];
				} else {
					if (i == 0)
						inputbuffer[ifmv] = window[i * KERNEL_X];
				}

			// Store data into (windowbuffer)
			} else {
				if (ofmv == ((OFM / OFM_PARALLEL) - 1))
					windowbuffer[ifmv][(i * (KERNEL_X - 1)) + (j - 1)] = window[(i * KERNEL_X) + (j - 1)];
			}
		}
	}

	// Write complete column to linebuffer
	if ((x < INPUT_WIDTH) && (KERNEL_Y > 0) && (ofmv == ((OFM / OFM_PARALLEL) - 1)))
		linebuffer[(x * (IFM / IFM_PARALLEL)) + ifmv] = linebuffer_data;

}

/***************************************************************************************************************************************************/
// Floating point convolution computation
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 OFM_PARALLEL>
TypeComp FloatingPoint(vx_int64 ofmp, vx_image_data<TypeBias, OFM_PARALLEL> biasbuffer, TypeComp conv3d) { // TODO: BIAS (data_type)
#pragma HLS INLINE

	TypeComp bias_var = (TypeComp)(biasbuffer.pixel[ofmp]);
	return (conv3d + bias_var);
}

/***************************************************************************************************************************************************/
// Fixed point convolution computation
template<typename TypeData, typename TypeBias, typename TypeComp, vx_round_policy_e ROUNDING_POLICY, vx_convert_policy_e OVERFLOW_POLICY, vx_int64 FRACTION, vx_int64 OFM_PARALLEL>
TypeComp FixedPoint(
	vx_int64 ofmp,
	vx_image_data<TypeBias, OFM_PARALLEL> biasbuffer, // TODO: BIAS
	TypeComp conv3d) {
#pragma HLS INLINE

	TypeComp result = 0;

	// Rounding
	if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_ZERO) {
		result = (((vx_int64)(conv3d)) >> FRACTION);
	} else if (ROUNDING_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
		TypeComp round = (1 << (FRACTION - 1));
		result = (((vx_int64)(conv3d + round)) >> FRACTION);
	}

	// Add Bias
	TypeComp bias_var = (TypeComp)(biasbuffer.pixel[ofmp]); // TODO: BIAS
	result = result + bias_var;

	// Check for overflow
	if (OVERFLOW_POLICY == VX_CONVERT_POLICY_SATURATE) {
		TypeComp maximum = (TypeComp)ComputeMax<TypeData>();
		TypeComp minimum = (TypeComp)ComputeMin<TypeData>();
		result = MAX(result, minimum);
		result = MIN(result, maximum);
	}

	return result;
}

/***************************************************************************************************************************************************/
// 3d Pointwise Convolution
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 FIXED_POINT_POSITION, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH,
	vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ConvolutionPointwise(vx_int64 ofmv, vx_int64 ifmv, vx_int64 dst_ptr, bool cond_dst, bool cond_weights, bool cond_bias,
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weightsbuffer[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
	vx_image_data<TypeBias, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL], // TODO: BIAS
	vx_image_data<TypeBias, OFM_PARALLEL> biasbuffer[OFM / OFM_PARALLEL], // TODO: BIAS
	vx_image_data<TypeData, IFM_PARALLEL> window[KERNEL_Y * KERNEL_X],
	vx_int64 conv3d[OFM_PARALLEL],
	TypeComp output[OFM_PARALLEL]) {
#pragma HLS INLINE

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const float INV_SHIFT = (1.0f / SHIFT);
	const float MAX_INT31 = 1073741824.0f;
	const float MIN_INT31 = -1073741824.0f;

	// Buffers the current 2d kernel
	vx_image_data<TypeData, OFM_PARALLEL * IFM_PARALLEL * KERNEL_X * KERNEL_Y> kernelbuffer;
//#pragma HLS array_partition variable=kernelbuffer complete dim=0

	vx_image_data<TypeBias, OFM_PARALLEL> biasvector; // TODO: BIAS

	// Init 
	for (vx_int64 ofmp = 0; ofmp < OFM_PARALLEL; ++ofmp) {
		biasvector.pixel[ofmp] = 0;
	}

	// Buffer ofm vector of bias elements in bias buffer
	if (BIAS_SIZE == OFM) {
		if (BUFFER_WEIGHTS == true) {
			if (cond_bias == true) {
				biasvector = biases[ofmv];
				biasbuffer[ofmv] = biasvector;
			} else {
				biasvector = biasbuffer[ofmv];
			}
		} else {
			biasvector = biases[ofmv];
		}

	} else if((cond_dst == true) && (BIAS_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM))) {
		biasvector = biases[dst_ptr];
	}

	// Read a 2d kernel from kernel input
	if (BUFFER_WEIGHTS == true) {
		if (cond_weights == true) {
			kernelbuffer = weights[ofmv * (IFM / IFM_PARALLEL) + ifmv];
			weightsbuffer[ofmv * (IFM / IFM_PARALLEL) + ifmv] = kernelbuffer;
		} else {
			kernelbuffer = weightsbuffer[ofmv * (IFM / IFM_PARALLEL) + ifmv];
		}
	} else {
		kernelbuffer = weights[ofmv * (IFM / IFM_PARALLEL) + ifmv];
	}

	// 3d Convolution
	for (vx_int64 ofmp = 0; ofmp < OFM_PARALLEL; ++ofmp) {

		// Stores the 2d convolution result
		TypeComp conv2d = 0;
		vx_int64 conv2d_t = 0;
		TypeComp conv3d_t = 0;

		// 2d Convolution (when ifmp > 1, then it is already 3d)
		for (vx_int64 ky = 0; ky < KERNEL_Y; ++ky) {
			for (vx_int64 kx = 0; kx < KERNEL_X; ++kx) {
				vx_int64 win_ptr = ky * KERNEL_X + kx;
				vx_int64 ker_ptr = ((KERNEL_Y - 1 - ky) * KERNEL_X) + (KERNEL_X - 1 - kx);
				for (vx_int64 ifmp = 0; ifmp < IFM_PARALLEL; ++ifmp) {
					TypeComp data1 = (TypeComp)(window[win_ptr].pixel[ifmp]);
					TypeComp data2 = (TypeComp)(kernelbuffer.pixel[(ofmp * IFM_PARALLEL * KERNEL_Y * KERNEL_X) + (ifmp * KERNEL_Y * KERNEL_X) + ker_ptr]);					
					conv2d += data1 * data2;
				}
			}
		}

		// Convert to integer for floating point summation, since it does not meet timing
		if (is_integer == false) {
			conv2d = MAX(MIN(conv2d, MAX_INT31), MIN_INT31);
			conv2d_t = static_cast<vx_int32>(conv2d * SHIFT + 0.5f);
		} else {
			conv2d_t = conv2d;
		}

		// 3d convolution
		if (ifmv == 0)
			conv3d[ofmp] = conv2d_t;
		else
			conv3d[ofmp] += conv2d_t;  // TODO: Initialize

		// Convert back
		if (is_integer == false) {
			conv3d_t = (TypeComp)(conv3d[ofmp]) * INV_SHIFT;
		} else {
			conv3d_t = conv3d[ofmp];
		}

		// Rounding + Add Bias + Overflow
		if (is_integer == false)
			output[ofmp] = FloatingPoint<TypeData, TypeBias, TypeComp, OFM_PARALLEL>(ofmp, biasvector, conv3d_t);
		else
			output[ofmp] = FixedPoint<TypeData, TypeBias, TypeComp, ROUNDING_POLICY, OVERFLOW_POLICY, FIXED_POINT_POSITION, OFM_PARALLEL>(ofmp, biasvector, conv3d_t);
	}
}

/***************************************************************************************************************************************************/
// 2d DepthWise convolution
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 FIXED_POINT_POSITION, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM,
	vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, 
	vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ConvolutionDepthWise(vx_int64 fmv, vx_int64 dst_ptr, bool cond_dst, bool cond_bias,
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weightsbuffer[FM / FM_PARALLEL],
	vx_image_data<TypeBias, FM_PARALLEL> biases[BIAS_SIZE / FM_PARALLEL],
	vx_image_data<TypeBias, FM_PARALLEL> biasbuffer[FM / FM_PARALLEL],
	vx_image_data<TypeData, FM_PARALLEL> window[KERNEL_Y * KERNEL_X],
	TypeComp result[FM_PARALLEL]) {
#pragma HLS INLINE

	// Buffers the current 2d kernel
	vx_image_data<TypeData, KERNEL_X * KERNEL_Y * FM_PARALLEL> kernelbuffer;
//#pragma HLS array_partition variable=kernelbuffer complete dim=0	
	vx_image_data<TypeBias, FM_PARALLEL> biasvector;

	// Init biasbuffer
	for (vx_int64 fmp = 0; fmp < FM_PARALLEL; ++fmp) {
		biasvector.pixel[fmp] = 0;
	}

	// Buffer ofm vector of bias elements in bias buffer
	if (BIAS_SIZE == FM) {
		if (BUFFER_WEIGHTS == true) {
			if (cond_bias == true) {
				biasvector = biases[fmv];
				biasbuffer[fmv] = biasvector;
			} else {
				biasvector = biasbuffer[fmv];
			}
		} else {
			biasvector = biases[fmv];
		}
	} else if((cond_dst == true) && (BIAS_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM))) {
		biasvector = biases[dst_ptr];
	}

	// Read multiple a 2d kernel from weights input
	//kernelbuffer = weights[fmv];
	if (BUFFER_WEIGHTS == true) {
		if (cond_bias == true) {
			kernelbuffer = weights[fmv];
			weightsbuffer[fmv] = kernelbuffer;
		} else {
			kernelbuffer = weightsbuffer[fmv];
		}
	} else {
		kernelbuffer = weights[fmv];
	}

	// Stores the 2d convolution result
	TypeComp conv2d[FM_PARALLEL] = { 0 };

	// Multiple 2d Convolution
	for (vx_int64 fmp = 0; fmp < FM_PARALLEL; ++fmp) {

		// 2d Convolution
		for (vx_int64 ky = 0; ky < KERNEL_Y; ++ky) {
			for (vx_int64 kx = 0; kx < KERNEL_X; ++kx) {
				vx_int64 win_ptr = ky * KERNEL_X + kx;
				vx_int64 ker_ptr = ((KERNEL_Y - 1 - ky) * KERNEL_X) + (KERNEL_X - 1 - kx);
				TypeComp data1 = (TypeComp)(window[win_ptr].pixel[fmp]);
				TypeComp data2 = (TypeComp)(kernelbuffer.pixel[(fmp * KERNEL_Y * KERNEL_X) + ker_ptr]);
				conv2d[fmp] += data1 * data2; // TODO: This is the critical path
			}
		}

		// Rounding + Add Bias + Overflow
		if (std::numeric_limits<TypeData>::is_integer == false)
			result[fmp] = FloatingPoint<TypeData, TypeBias, TypeComp, FM_PARALLEL>(fmp, biasvector, conv2d[fmp]);
		else
			result[fmp] = FixedPoint<TypeData, TypeBias, TypeComp, ROUNDING_POLICY, OVERFLOW_POLICY, FIXED_POINT_POSITION, FM_PARALLEL>(fmp, biasvector, conv2d[fmp]);
	}
}

/***************************************************************************************************************************************************/
// Write 3d convolution to output ofm pixel
template<typename TypeData, typename TypeComp, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 BATCHES, vx_int64 OFM_PARALLEL>
void WriteOutput(bool cond_dst, TypeComp result[OFM_PARALLEL], 
	vx_image_data<TypeData, OFM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)], vx_int64& dst_ptr) {
#pragma HLS INLINE

	// Constants
	const vx_int64 OUTPUT_SIZE = OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL);

	// Write 3d convolution to output ofm pixel
	if (cond_dst == true) {
		vx_image_data<TypeData, OFM_PARALLEL> output;		

		// Transform result to output vector		
		for (vx_int64 ofmp = 0; ofmp < OFM_PARALLEL; ++ofmp)
			output.pixel[ofmp] = (TypeData)(result[ofmp]);
		GenerateDmaSignal<TypeData, OFM_PARALLEL>(((dst_ptr % OUTPUT_SIZE) == 0), ((dst_ptr % OUTPUT_SIZE) == (OUTPUT_SIZE - 1)), output);

		// Write the resulting pixel to the output		
		dst[dst_ptr] = output;
		++dst_ptr;		
	}
}
template<typename TypeData, typename TypeComp, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 BATCHES, vx_int64 OFM_PARALLEL>
void WriteOutput(bool cond_dst, TypeComp result[OFM_PARALLEL],
	TypeData dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)], vx_int64& dst_ptr) {
#pragma HLS INLINE

	// Write 3d convolution to output ofm pixel
	if (cond_dst == true) {

		// Write the resulting pixel to the output
		dst[dst_ptr] = (TypeData)(result[0]);
		++dst_ptr;
	}
}

/***************************************************************************************************************************************************/
// Point-Wise Convolution Layer
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM,
	vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, 
	vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ConvComputePointwise(
		vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
		vx_image_data<TypeBias, OFM_PARALLEL> biases[BIAS_SIZE / OFM_PARALLEL], // TODO: BIAS
		vx_image_data<TypeData, IFM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],
		vx_image_data<TypeData, OFM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 KERNEL_RAD_Y = (KERNEL_Y / 2);
	const vx_int64 KERNEL_RAD_X = (KERNEL_X / 2);
	const vx_int64 STRIDE_Y = (INPUT_HEIGHT - 1) / (OUTPUT_HEIGHT - 1);
	const vx_int64 STRIDE_X = (INPUT_WIDTH - 1) / (OUTPUT_WIDTH - 1);
	const vx_int64 IFM_VECTOR = MAX((IFM / IFM_PARALLEL), 1);
	const vx_int64 OFM_VECTOR = MAX((OFM / OFM_PARALLEL), 1);
	const vx_int64 LINEBUFFER_Y = MAX((KERNEL_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((KERNEL_X - 1), 1);

	// Buffers for the Weights
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weightsbuffer[(OFM / OFM_PARALLEL) * IFM_VECTOR];
#pragma HLS RESOURCE variable=weightsbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=weightsbuffer

	// Buffer for Biases
	vx_image_data<TypeBias, OFM_PARALLEL> biasbuffer[OFM / OFM_PARALLEL]; // TODO: BIAS
#pragma HLS RESOURCE variable=biasbuffer core=RAM_2P_LUTRAM 
#pragma HLS data_pack variable=biasbuffer

	// Buffers the (KERNEL_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, IFM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * IFM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the IFM of the input pixel
	vx_image_data<TypeData, IFM_PARALLEL> inputbuffer[IFM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM  // TODO: THis gave problems for small IFM_VECTOR
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete IFM of the current window
	vx_image_data<TypeData, IFM_PARALLEL> windowbuffer[IFM_VECTOR][WINDOWBUFFER_X * KERNEL_Y];
//#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, IFM_PARALLEL> window[KERNEL_X * KERNEL_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Buffers the current 3d convolution, which is 1 output pixel
	vx_int64 conv3d[OFM_PARALLEL];
#pragma HLS array_partition variable=conv3d complete dim=0

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + (KERNEL_Y / 2)); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + (KERNEL_X / 2)); ++x) {
				for (vx_int64 i = 0; i < IFM_VECTOR * OFM_VECTOR; ++i) {
#pragma HLS pipeline II=1
#pragma HLS dependence variable=linebuffer inter false // TODO: This is bad style

					// VHLS has problems with loop of size 1
					const vx_int64 ofmv = i / IFM_VECTOR;
					const vx_int64 ifmv = i % IFM_VECTOR;

					// Stores 1 ofm pixel
					vx_image_data<TypeData, IFM_PARALLEL> input;
					TypeComp result[OFM_PARALLEL] = { 0 };

					// Conditions to write to output
					const bool cond_x = (((x - KERNEL_RAD_X) % STRIDE_X) == 0) && (x >= KERNEL_RAD_X);
					const bool cond_y = (((y - KERNEL_RAD_Y) % STRIDE_Y) == 0) && (y >= KERNEL_RAD_Y);
					const bool cond_dst = cond_x && cond_y && (ifmv == (IFM_VECTOR - 1));
					const bool cond_bias = (ifmv == 0) && (x == 0) && (y == 0) && (batch == 0);
					const bool cond_weights = (x == 0) && (y == 0) && (batch == 0);

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, IFM, BATCHES, IFM_PARALLEL>(y, x, ofmv, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, IFM, KERNEL_X, KERNEL_Y, IFM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, ofmv, ifmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, IFM, OFM, KERNEL_X, KERNEL_Y, IFM_PARALLEL, OFM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, ofmv, ifmv, window, linebuffer, inputbuffer, windowbuffer);

					// 3d Convolution for floating/fixed point numbers	
					ConvolutionPointwise<TypeData, TypeBias, TypeComp, FIXED_POINT_POSITION, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM,
						KERNEL_X, KERNEL_Y, BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>
						(ofmv, ifmv, dst_ptr, cond_dst, cond_weights, cond_bias, weights, weightsbuffer, biases, biasbuffer, window, conv3d, result);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeComp, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, BATCHES, OFM_PARALLEL>(cond_dst, result, dst, dst_ptr);
				}
			}
		}
	}
}

/***************************************************************************************************************************************************/
// Depth-Wise Convolution Layer
template<typename TypeData, typename TypeBias, typename TypeComp, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ConvComputeDepthWise(
		vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
		vx_image_data<TypeBias, FM_PARALLEL> biases[BIAS_SIZE / FM_PARALLEL], // TODO: BIAS
		vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
		vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 KERNEL_RAD_Y = (KERNEL_Y / 2);
	const vx_int64 KERNEL_RAD_X = (KERNEL_X / 2);
	const vx_int64 STRIDE_Y = (INPUT_HEIGHT - 1) / (OUTPUT_HEIGHT - 1);
	const vx_int64 STRIDE_X = (INPUT_WIDTH - 1) / (OUTPUT_WIDTH - 1);
	const vx_int64 FM_VECTOR = (FM / FM_PARALLEL);
	const vx_int64 LINEBUFFER_Y = MAX((KERNEL_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((KERNEL_X - 1), 1);

	// Buffers for the Weights
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weightsbuffer[FM_VECTOR];
#pragma HLS RESOURCE variable=weightsbuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=weightsbuffer

	// Buffer for Biases
	vx_image_data<TypeBias, FM_PARALLEL> biasbuffer[FM_VECTOR]; // TODO: BIAS
#pragma HLS RESOURCE variable=biasbuffer core=RAM_2P_LUTRAM 
#pragma HLS data_pack variable=biasbuffer

	// Buffers the (KERNEL_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, FM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * FM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the FM of the input pixel
	vx_image_data<TypeData, FM_PARALLEL> inputbuffer[FM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete FM of the current window
	vx_image_data<TypeData, FM_PARALLEL> windowbuffer[FM_VECTOR][WINDOWBUFFER_X * KERNEL_Y];
#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, FM_PARALLEL> window[KERNEL_X * KERNEL_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + (KERNEL_Y / 2)); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + (KERNEL_X / 2)); ++x) {

				// Iterates through the feature maps (ofm,ifm)
				for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

					// Stores 1 ofm pixel
					vx_image_data<TypeData, FM_PARALLEL> input;
					TypeComp result[FM_PARALLEL] = { 0 };

					// Conditions to write to output
					const bool cond_x = (((x - KERNEL_RAD_X) % STRIDE_X) == 0) && (x >= KERNEL_RAD_X);
					const bool cond_y = (((y - KERNEL_RAD_Y) % STRIDE_Y) == 0) && (y >= KERNEL_RAD_Y);
					const bool cond_dst = cond_x && cond_y;
					const bool cond_bias = (x == 0) && (y == 0) && (batch == 0);

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(y, x, 0, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, KERNEL_X, KERNEL_Y, FM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, 1, KERNEL_X, KERNEL_Y, FM_PARALLEL, 1, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, window, linebuffer, inputbuffer, windowbuffer);

					// 3d Convolution for floating/fixed point numbers	
					ConvolutionDepthWise<TypeData, TypeBias, TypeComp, FIXED_POINT_POSITION, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y, BIAS_SIZE, BATCHES, FM_PARALLEL,
						OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(fmv, dst_ptr, cond_dst, cond_bias, weights, weightsbuffer, biases, biasbuffer, window, result);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeComp, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(cond_dst, result, dst, dst_ptr);
				}
			}
		}
	}
}

/***************************************************************************************************************************************************/
// Compile time checks of the template parameter list
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT, 
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL>
void CompileTimeChecks() {
#pragma HLS INLINE

	const vx_int64 digits = std::numeric_limits<TypeData>::digits;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;

	// Compile time checks
	STATIC_ASSERT(((is_integer == true) && ((digits == 7) || (digits == 8) || (digits == 15) || (digits == 16))) || ((is_integer == false) && (digits == 24)), DATA_TYPE_IS_NOT_SUPPORTED);
	STATIC_ASSERT((FIXED_POINT_POSITION >= 0 && FIXED_POINT_POSITION <= digits), FIXED_POINT_POSITION_NOT_VALID);
	STATIC_ASSERT((INPUT_HEIGHT > 0) && (INPUT_WIDTH > 0) && (IFM > 0) && (OUTPUT_HEIGHT > 0) && (OUTPUT_WIDTH > 0) && (OFM > 0) &&
		(KERNEL_X > 0) && (KERNEL_Y > 0) && (BATCHES > 0) && (IFM_PARALLEL > 0) && (OFM_PARALLEL > 0), NO_ZERO_OR_NEGATIVE_DIMENSIONS_IN_INPUTS_OR_OUTPUTS);
	STATIC_ASSERT((INPUT_HEIGHT <= 2048) && (INPUT_WIDTH <= 2048) && (IFM <= 2048) && (OUTPUT_HEIGHT <= 2048) && (OUTPUT_WIDTH <= 2048) && (OFM <= 2048) &&
		(KERNEL_X <= 9) && (KERNEL_Y <= 9) && (BATCHES <= 2048) && (IFM_PARALLEL <= 128) && (OFM_PARALLEL <= 128), TEMPLATE_PARAMETER_SIZE_TO_HIGH);
	STATIC_ASSERT((BIAS_SIZE == 0) || (BIAS_SIZE == OFM) || (BIAS_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM)), BIAS_SIZE_IS_NOT_VALID);
	STATIC_ASSERT(((OFM % OFM_PARALLEL) == 0) && ((IFM % IFM_PARALLEL) == 0), FEATURE_MAP_NEEDS_TO_BE_MULTIPLE_OF_ITS_PARALLELIZATION);
	STATIC_ASSERT((OFM >= OFM_PARALLEL) && (IFM >= IFM_PARALLEL), FEATURE_MAP_NEEDS_TO_BE_BIGGER_OR_EQUAL_OF_ITS_PARALLELIZATION);		
	STATIC_ASSERT((INPUT_HEIGHT >= OUTPUT_HEIGHT) && (INPUT_WIDTH >= OUTPUT_WIDTH), INPUT_WIDTH_AND_HEIGHT_MUST_BE_BIGGER_EQUAL_OUTPUT_WIDTH_AND_HEIGHT);
}

/***************************************************************************************************************************************************/
// Top function of the point-wise convolution
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 OFM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 IFM_PARALLEL, 
	vx_int64 OFM_PARALLEL, vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
	void ConvPointWiseTop(
		vx_image_data<TypeData, IFM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (IFM / IFM_PARALLEL)],   
		vx_image_data<TypeData, KERNEL_Y * KERNEL_X * IFM_PARALLEL * OFM_PARALLEL> weights[(OFM / OFM_PARALLEL) * (IFM / IFM_PARALLEL)],
		vx_image_data<TypeBias, OFM_PARALLEL> biases[BIAS_SIZE/ OFM_PARALLEL],  // TODO: BIAS                                      
		vx_image_data<TypeData, OFM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (OFM / OFM_PARALLEL)]) {
#pragma HLS INLINE

	// Compile time checks
	STATIC_ASSERT(((INPUT_HEIGHT % OUTPUT_HEIGHT) == 0) && ((INPUT_WIDTH % OUTPUT_WIDTH) == 0), INPUT_WIDTH_AND_HEIGHT_MUST_BE_MULTIPLE_OF_OUTPUT_WIDTH_AND_HEIGHT);
	STATIC_ASSERT(((KERNEL_X % 2) == 1) && ((KERNEL_Y % 2) == 1), KERNEL_SIZE_MUST_BE_UNEVEN_NUMBER);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X, KERNEL_Y, BIAS_SIZE, 
		BATCHES, IFM_PARALLEL, OFM_PARALLEL>();

	// Bias and others need to have the same data type
	const bool is_integer_data = std::numeric_limits<TypeData>::is_integer;
	const bool is_integer_bias = std::numeric_limits<TypeBias>::is_integer;
	STATIC_ASSERT(is_integer_data == is_integer_bias, BIAS_AND_REST_MUST_BOTH_BE_INTEGER_OR_FLOAT);

	// Check the size of the Bias array and if data is fixed-point or floating-point
	if (std::numeric_limits<TypeData>::is_integer == true) {
		ConvComputePointwise<TypeData, TypeBias, vx_int64, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X,
			KERNEL_Y, BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(weights, biases, src, dst);
	} else {
		ConvComputePointwise<TypeData, TypeBias, float, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, IFM, OUTPUT_HEIGHT, OUTPUT_WIDTH, OFM, KERNEL_X,
			KERNEL_Y, BIAS_SIZE, BATCHES, IFM_PARALLEL, OFM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(weights, biases, src, dst);
	}
}

/***************************************************************************************************************************************************/
// Top function of the depth-wise convolution
template<typename TypeData, typename TypeBias, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT,
	vx_int64 OUTPUT_WIDTH, vx_int64 FM, vx_int64 KERNEL_X, vx_int64 KERNEL_Y, vx_int64 BIAS_SIZE, vx_int64 BATCHES, vx_int64 FM_PARALLEL, 
	vx_convert_policy_e OVERFLOW_POLICY, vx_round_policy_e ROUNDING_POLICY, bool BUFFER_WEIGHTS>
void ConvDepthWiseTop(
	vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, KERNEL_Y * KERNEL_X * FM_PARALLEL> weights[FM / FM_PARALLEL],
	vx_image_data<TypeBias, FM_PARALLEL> biases[BIAS_SIZE / FM_PARALLEL], // TODO: BIAS
	vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Compile time checks
	STATIC_ASSERT(((INPUT_HEIGHT % OUTPUT_HEIGHT) == 0) && ((INPUT_WIDTH % OUTPUT_WIDTH) == 0), INPUT_WIDTH_AND_HEIGHT_MUST_BE_MULTIPLE_OF_OUTPUT_WIDTH_AND_HEIGHT);
	STATIC_ASSERT(((KERNEL_X % 2) == 1) && ((KERNEL_Y % 2) == 1), KERNEL_SIZE_MUST_BE_UNEVEN_NUMBER);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, FM, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X, KERNEL_Y, BIAS_SIZE, 
		BATCHES, FM_PARALLEL, FM_PARALLEL>();

	// Bias and others need to have the same data type
	const bool is_integer_data = std::numeric_limits<TypeData>::is_integer;
	const bool is_integer_bias = std::numeric_limits<TypeBias>::is_integer;
	STATIC_ASSERT(is_integer_data == is_integer_bias, BIAS_AND_REST_MUST_BOTH_BE_INTEGER_OR_FLOAT);

		// Check the size of the Bias array and if data is fixed-point or floating-point
	if (std::numeric_limits<TypeData>::is_integer == true) {
		ConvComputeDepthWise<TypeData, TypeBias, vx_int64, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X,
			KERNEL_Y, BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(weights, biases, src, dst);
	} else {
		ConvComputeDepthWise<TypeData, TypeBias, float, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, KERNEL_X,
			KERNEL_Y, BIAS_SIZE, BATCHES, FM_PARALLEL, OVERFLOW_POLICY, ROUNDING_POLICY, BUFFER_WEIGHTS>(weights, biases, src, dst);
	}
}

/***************************************************************************************************************************************************/
// Compute the pooling function (including parallelization)
template<typename TypeData, typename TypeComp, vx_int64 FIXED_POINT_POSITION, vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_nn_pooling_type_e POOLING_TYPE>
void ComputePooling(vx_image_data<TypeData, FM_PARALLEL> window[POOLING_X * POOLING_Y], TypeData output[FM_PARALLEL]) {

	// Constants	
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = static_cast<float>(1 << FIXED_POINT_POSITION);
	const float NORM_FLT = 1.0f / static_cast<float>(POOLING_Y * POOLING_X);
	const vx_int64 NORM_INT = static_cast<vx_int64>(NORM_FLT * SHIFT + 0.5f);	

	// Do pooling on the input window for "FM_PARALLEL" feature maps
	for (vx_int64 fmp = 0; fmp < FM_PARALLEL; ++fmp) {

		// Max Pooling
		if (POOLING_TYPE == VX_NN_POOLING_MAX) {
			TypeData result = ComputeMin<TypeData>();

			// Get maximum value of the input values
			for (vx_int64 py = 0; py < POOLING_Y; ++py) {
				for (vx_int64 px = 0; px < POOLING_Y; ++px) {
					result = MAX(result, window[py * POOLING_X + px].pixel[fmp]);
				}
			}

			// Store result
			output[fmp] = (TypeData)result;

		// Average Pooling
		} else if (POOLING_TYPE == VX_NN_POOLING_AVG) {
			TypeComp result = 0;

			// Sum up the values of the input window
			for (vx_int64 py = 0; py < POOLING_Y; ++py) {
				for (vx_int64 px = 0; px < POOLING_X; ++px) {
					result += static_cast<TypeComp>(window[py * POOLING_X + px].pixel[fmp]);
				}
			}

			// Compute the average of the sum
			if (is_integer == true)
				result = static_cast<vx_int64>(result * NORM_INT) >> FIXED_POINT_POSITION;
			else
				result = result * NORM_FLT;

			// Store result
			output[fmp] = (TypeData)result;
		}
	}
}

/***************************************************************************************************************************************************/
// Pooling Layer
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH,
	vx_int64 FM, vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void Pooling(
	vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 STRIDE_Y_A = OUTPUT_HEIGHT - 1;
	const vx_int64 STRIDE_Y_B = INPUT_HEIGHT + 2 * PADDING_Y - POOLING_Y;
	const vx_int64 STRIDE_X_A = OUTPUT_WIDTH - 1;
	const vx_int64 STRIDE_X_B = INPUT_WIDTH + 2 * PADDING_X - POOLING_X;
	const vx_int64 STRIDE_Y = ((STRIDE_Y_A <= 0) || (STRIDE_Y_B <= 0)) ? (1) : (STRIDE_Y_B / STRIDE_Y_A);
	const vx_int64 STRIDE_X = ((STRIDE_X_A <= 0) || (STRIDE_X_B <= 0)) ? (1) : (STRIDE_X_B / STRIDE_X_A);
	const vx_int64 FM_VECTOR = (FM / FM_PARALLEL);
	const vx_int64 LINEBUFFER_Y = MAX((POOLING_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((POOLING_X - 1), 1);

	// Compile time checks
	STATIC_ASSERT((1 <= POOLING_Y) && (POOLING_Y <= INPUT_WIDTH), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_Y) && (PADDING_Y <= (POOLING_Y / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_Y) && (STRIDE_Y <= POOLING_Y), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= POOLING_X) && (POOLING_X <= INPUT_HEIGHT), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_X) && (PADDING_X <= (POOLING_X / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_X) && (STRIDE_X <= POOLING_X), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, FM, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, POOLING_X, POOLING_Y, 
		0, BATCHES, FM_PARALLEL, FM_PARALLEL>();

	// Buffers the (POOLING_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, FM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * FM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the FM of the input pixel
	vx_image_data<TypeData, FM_PARALLEL> inputbuffer[FM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete FM of the current window
	vx_image_data<TypeData, FM_PARALLEL> windowbuffer[FM_VECTOR][WINDOWBUFFER_X * POOLING_Y];
#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, FM_PARALLEL> window[POOLING_X * POOLING_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + PADDING_Y); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + PADDING_X); ++x) {

				// Iterates through the feature maps (ofm,ifm)
				for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

					// Stores 1 ofm pixel
					vx_image_data<TypeData, FM_PARALLEL> input;
					TypeData output[FM_PARALLEL] = { 0 };

					// Conditions to write to output
					const vx_int64 DST_START_X = (POOLING_X - PADDING_X - 1);
					const vx_int64 DST_START_Y = (POOLING_Y - PADDING_Y - 1);
					const vx_int64 DST_END_X = DST_START_X + OUTPUT_HEIGHT * STRIDE_Y;
					const vx_int64 DST_END_Y = DST_START_Y + OUTPUT_WIDTH * STRIDE_Y;
					const bool cond_x = (((x - DST_START_X) % STRIDE_X) == 0) && (x >= DST_START_X) && (x < DST_END_X);
					const bool cond_y = (((y - DST_START_Y) % STRIDE_Y) == 0) && (y >= DST_START_Y) && (y < DST_END_Y);
					const bool cond_dst = cond_x && cond_y;

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(y, x, 0, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, POOLING_X, POOLING_Y, FM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, 1, POOLING_X, POOLING_Y, FM_PARALLEL, 1, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, window, linebuffer, inputbuffer, windowbuffer);

					// Compute Pooling
					if (std::numeric_limits<TypeData>::is_integer == true)
						ComputePooling<TypeData, vx_int64, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);
					else
						ComputePooling<TypeData, float, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeData, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(cond_dst, output, dst, dst_ptr);			
				}				
			}
		}
	}
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH,
	vx_int64 FM, vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void Pooling(
	TypeData src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	vx_image_data<TypeData, FM_PARALLEL> dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 STRIDE_Y_A = OUTPUT_HEIGHT - 1;
	const vx_int64 STRIDE_Y_B = INPUT_HEIGHT + 2 * PADDING_Y - POOLING_Y;
	const vx_int64 STRIDE_X_A = OUTPUT_WIDTH - 1;
	const vx_int64 STRIDE_X_B = INPUT_WIDTH + 2 * PADDING_X - POOLING_X;
	const vx_int64 STRIDE_Y = ((STRIDE_Y_A <= 0) || (STRIDE_Y_B <= 0)) ? (1) : (STRIDE_Y_B / STRIDE_Y_A);
	const vx_int64 STRIDE_X = ((STRIDE_X_A <= 0) || (STRIDE_X_B <= 0)) ? (1) : (STRIDE_X_B / STRIDE_X_A);
	const vx_int64 FM_VECTOR = (FM / FM_PARALLEL);
	const vx_int64 LINEBUFFER_Y = MAX((POOLING_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((POOLING_X - 1), 1);

	// Compile time checks
	STATIC_ASSERT((1 <= POOLING_Y) && (POOLING_Y <= INPUT_WIDTH), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_Y) && (PADDING_Y <= (POOLING_Y / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_Y) && (STRIDE_Y <= POOLING_Y), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= POOLING_X) && (POOLING_X <= INPUT_HEIGHT), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_X) && (PADDING_X <= (POOLING_X / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_X) && (STRIDE_X <= POOLING_X), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, FM, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, POOLING_X, POOLING_Y, 
		0, BATCHES, FM_PARALLEL, FM_PARALLEL>();

	// Buffers the (POOLING_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, FM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * FM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the FM of the input pixel
	vx_image_data<TypeData, FM_PARALLEL> inputbuffer[FM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete FM of the current window
	vx_image_data<TypeData, FM_PARALLEL> windowbuffer[FM_VECTOR][WINDOWBUFFER_X * POOLING_Y];
#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, FM_PARALLEL> window[POOLING_X * POOLING_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + PADDING_Y); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + PADDING_X); ++x) {

				// Iterates through the feature maps (ofm,ifm)
				for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

					// Stores 1 ofm pixel
					vx_image_data<TypeData, FM_PARALLEL> input;
					TypeData output[FM_PARALLEL] = { 0 };

					// Conditions to write to output
					const vx_int64 DST_START_X = (POOLING_X - PADDING_X - 1);
					const vx_int64 DST_START_Y = (POOLING_Y - PADDING_Y - 1);
					const vx_int64 DST_END_X = DST_START_X + OUTPUT_HEIGHT * STRIDE_Y;
					const vx_int64 DST_END_Y = DST_START_Y + OUTPUT_WIDTH * STRIDE_Y;
					const bool cond_x = (((x - DST_START_X) % STRIDE_X) == 0) && (x >= DST_START_X) && (x < DST_END_X);
					const bool cond_y = (((y - DST_START_Y) % STRIDE_Y) == 0) && (y >= DST_START_Y) && (y < DST_END_Y);
					const bool cond_dst = cond_x && cond_y;

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(y, x, 0, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, POOLING_X, POOLING_Y, FM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, 1, POOLING_X, POOLING_Y, FM_PARALLEL, 1, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, window, linebuffer, inputbuffer, windowbuffer);

					// Compute Pooling
					if (std::numeric_limits<TypeData>::is_integer == true)
						ComputePooling<TypeData, vx_int64, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);
					else
						ComputePooling<TypeData, float, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeData, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(cond_dst, output, dst, dst_ptr);			
				}				
			}
		}
	}
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH,
	vx_int64 FM, vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void Pooling(
	vx_image_data<TypeData, FM_PARALLEL> src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	TypeData dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 STRIDE_Y_A = OUTPUT_HEIGHT - 1;
	const vx_int64 STRIDE_Y_B = INPUT_HEIGHT + 2 * PADDING_Y - POOLING_Y;
	const vx_int64 STRIDE_X_A = OUTPUT_WIDTH - 1;
	const vx_int64 STRIDE_X_B = INPUT_WIDTH + 2 * PADDING_X - POOLING_X;
	const vx_int64 STRIDE_Y = ((STRIDE_Y_A <= 0) || (STRIDE_Y_B <= 0)) ? (1) : (STRIDE_Y_B / STRIDE_Y_A);
	const vx_int64 STRIDE_X = ((STRIDE_X_A <= 0) || (STRIDE_X_B <= 0)) ? (1) : (STRIDE_X_B / STRIDE_X_A);
	const vx_int64 FM_VECTOR = (FM / FM_PARALLEL);
	const vx_int64 LINEBUFFER_Y = MAX((POOLING_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((POOLING_X - 1), 1);

	// Compile time checks
	STATIC_ASSERT((1 <= POOLING_Y) && (POOLING_Y <= INPUT_WIDTH), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_Y) && (PADDING_Y <= (POOLING_Y / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_Y) && (STRIDE_Y <= POOLING_Y), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= POOLING_X) && (POOLING_X <= INPUT_HEIGHT), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_X) && (PADDING_X <= (POOLING_X / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_X) && (STRIDE_X <= POOLING_X), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, FM, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, POOLING_X, POOLING_Y, 
		0, BATCHES, FM_PARALLEL, FM_PARALLEL>();

	// Buffers the (POOLING_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, FM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * FM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the FM of the input pixel
	vx_image_data<TypeData, FM_PARALLEL> inputbuffer[FM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete FM of the current window
	vx_image_data<TypeData, FM_PARALLEL> windowbuffer[FM_VECTOR][WINDOWBUFFER_X * POOLING_Y];
#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, FM_PARALLEL> window[POOLING_X * POOLING_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + PADDING_Y); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + PADDING_X); ++x) {

				// Iterates through the feature maps (ofm,ifm)
				for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

					// Stores 1 ofm pixel
					vx_image_data<TypeData, FM_PARALLEL> input;
					TypeData output[FM_PARALLEL] = { 0 };

					// Conditions to write to output
					const vx_int64 DST_START_X = (POOLING_X - PADDING_X - 1);
					const vx_int64 DST_START_Y = (POOLING_Y - PADDING_Y - 1);
					const vx_int64 DST_END_X = DST_START_X + OUTPUT_HEIGHT * STRIDE_Y;
					const vx_int64 DST_END_Y = DST_START_Y + OUTPUT_WIDTH * STRIDE_Y;
					const bool cond_x = (((x - DST_START_X) % STRIDE_X) == 0) && (x >= DST_START_X) && (x < DST_END_X);
					const bool cond_y = (((y - DST_START_Y) % STRIDE_Y) == 0) && (y >= DST_START_Y) && (y < DST_END_Y);
					const bool cond_dst = cond_x && cond_y;

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(y, x, 0, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, POOLING_X, POOLING_Y, FM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, 1, POOLING_X, POOLING_Y, FM_PARALLEL, 1, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, window, linebuffer, inputbuffer, windowbuffer);

					// Compute Pooling
					if (std::numeric_limits<TypeData>::is_integer == true)
						ComputePooling<TypeData, vx_int64, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);
					else
						ComputePooling<TypeData, float, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeData, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(cond_dst, output, dst, dst_ptr);			
				}				
			}
		}
	}
}
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH,
	vx_int64 FM, vx_int64 FM_PARALLEL, vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void Pooling(
	TypeData src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * (FM / FM_PARALLEL)],
	TypeData dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * (FM / FM_PARALLEL)]) {
#pragma HLS INLINE

	// Constants
	const vx_int64 STRIDE_Y_A = OUTPUT_HEIGHT - 1;
	const vx_int64 STRIDE_Y_B = INPUT_HEIGHT + 2 * PADDING_Y - POOLING_Y;
	const vx_int64 STRIDE_X_A = OUTPUT_WIDTH - 1;
	const vx_int64 STRIDE_X_B = INPUT_WIDTH + 2 * PADDING_X - POOLING_X;
	const vx_int64 STRIDE_Y = ((STRIDE_Y_A <= 0) || (STRIDE_Y_B <= 0)) ? (1) : (STRIDE_Y_B / STRIDE_Y_A);
	const vx_int64 STRIDE_X = ((STRIDE_X_A <= 0) || (STRIDE_X_B <= 0)) ? (1) : (STRIDE_X_B / STRIDE_X_A);
	const vx_int64 FM_VECTOR = (FM / FM_PARALLEL);
	const vx_int64 LINEBUFFER_Y = MAX((POOLING_Y - 1), 1);
	const vx_int64 WINDOWBUFFER_X = MAX((POOLING_X - 1), 1);

	// Compile time checks
	STATIC_ASSERT((1 <= POOLING_Y) && (POOLING_Y <= INPUT_WIDTH), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_Y) && (PADDING_Y <= (POOLING_Y / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_Y) && (STRIDE_Y <= POOLING_Y), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= POOLING_X) && (POOLING_X <= INPUT_HEIGHT), POOLING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((0 <= PADDING_X) && (PADDING_X <= (POOLING_X / 2)), PADDING_IN_Y_DIRECTION_NOT_VALID);
	STATIC_ASSERT((1 <= STRIDE_X) && (STRIDE_X <= POOLING_X), STRIDE_IN_Y_DIRECTION_NOT_VALID);
	CompileTimeChecks<TypeData, FIXED_POINT_POSITION, INPUT_HEIGHT, INPUT_WIDTH, FM, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, POOLING_X, POOLING_Y, 
		0, BATCHES, FM_PARALLEL, FM_PARALLEL>();

	// Buffers the (POOLING_Y - 1) input rows for the sliding window
	vx_image_data<TypeData, FM_PARALLEL * LINEBUFFER_Y> linebuffer[INPUT_WIDTH * FM_VECTOR];
#pragma HLS RESOURCE variable=linebuffer core=RAM_2P_BRAM
#pragma HLS data_pack variable=linebuffer

	// Buffers the FM of the input pixel
	vx_image_data<TypeData, FM_PARALLEL> inputbuffer[FM_VECTOR];
//#pragma HLS RESOURCE variable=inputbuffer core=RAM_2P_LUTRAM
#pragma HLS data_pack variable=inputbuffer

	// Buffers the complete FM of the current window
	vx_image_data<TypeData, FM_PARALLEL> windowbuffer[FM_VECTOR][WINDOWBUFFER_X * POOLING_Y];
#pragma HLS RESOURCE variable=windowbuffer core=RAM_2P_LUTRAM
#pragma HLS array_partition variable=windowbuffer complete dim=2
#pragma HLS data_pack variable=windowbuffer

	// The window for the current 2d convolution
	vx_image_data<TypeData, FM_PARALLEL> window[POOLING_X * POOLING_Y];
#pragma HLS array_partition variable=window complete dim=0	

	// Batches are the amount of images
	for (vx_int64 batch = 0, dst_ptr = 0, src_ptr = 0; batch < BATCHES; ++batch) {

		// Iterates through the image pixels (y,x)
		for (vx_int64 y = 0; y < (INPUT_HEIGHT + PADDING_Y); ++y) {
			for (vx_int64 x = 0; x < (INPUT_WIDTH + PADDING_X); ++x) {

				// Iterates through the feature maps (ofm,ifm)
				for (vx_int64 fmv = 0; fmv < FM_VECTOR; ++fmv) {
#pragma HLS pipeline II=1

					// Stores 1 ofm pixel
					vx_image_data<TypeData, FM_PARALLEL> input;
					TypeData output[FM_PARALLEL] = { 0 };

					// Conditions to write to output
					const vx_int64 DST_START_X = (POOLING_X - PADDING_X - 1);
					const vx_int64 DST_START_Y = (POOLING_Y - PADDING_Y - 1);
					const vx_int64 DST_END_X = DST_START_X + OUTPUT_HEIGHT * STRIDE_Y;
					const vx_int64 DST_END_Y = DST_START_Y + OUTPUT_WIDTH * STRIDE_Y;
					const bool cond_x = (((x - DST_START_X) % STRIDE_X) == 0) && (x >= DST_START_X) && (x < DST_END_X);
					const bool cond_y = (((y - DST_START_Y) % STRIDE_Y) == 0) && (y >= DST_START_Y) && (y < DST_END_Y);
					const bool cond_dst = cond_x && cond_y;

					// Read input pixel from input image						
					ReadInput<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(y, x, 0, src, input, src_ptr);

					// Read data and write it into window						
					UpdateWindow<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, POOLING_X, POOLING_Y, FM_PARALLEL, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, input, linebuffer, inputbuffer, windowbuffer, window);

					// Store data from window into (windowbuffer and (linebuffer or inputbuffer)						
					UpdateBuffers<TypeData, INPUT_HEIGHT, INPUT_WIDTH, FM, 1, POOLING_X, POOLING_Y, FM_PARALLEL, 1, LINEBUFFER_Y, WINDOWBUFFER_X>
						(y, x, 0, fmv, window, linebuffer, inputbuffer, windowbuffer);

					// Compute Pooling
					if (std::numeric_limits<TypeData>::is_integer == true)
						ComputePooling<TypeData, vx_int64, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);
					else
						ComputePooling<TypeData, float, FIXED_POINT_POSITION, FM_PARALLEL, POOLING_Y, POOLING_X, POOLING_TYPE>(window, output);

					// Write 3d convolution to output ofm pixel						
					WriteOutput<TypeData, TypeData, OUTPUT_HEIGHT, OUTPUT_WIDTH, FM, BATCHES, FM_PARALLEL>(cond_dst, output, dst, dst_ptr);			
				}				
			}
		}
	}
}

#endif // NN_CORE1_H_
