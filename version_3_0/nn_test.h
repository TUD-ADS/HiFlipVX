/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_neural_network_test.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*
*/

#ifndef NN_TEST_H_
#define NN_TEST_H_

#include "nn_base.h"

/*********************************************************************************************************************/
// 3d convolution using floating point numbers
template<vx_int64 BATCHES, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, 
	vx_int64 OFM, vx_int64 IFM, vx_int64 KERNEL_Y, vx_int64 KERNEL_X, vx_int64 BIASES_SIZE>
void SwConvPointWise(
	float weights[OFM * IFM * KERNEL_Y * KERNEL_X], 
	float biases[BIASES_SIZE],
	float src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * IFM], 
	float dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM]) {

	// Compile time checks
	STATIC_ASSERT((INPUT_HEIGHT > 0) && (INPUT_WIDTH > 0) && (IFM > 0) && (OUTPUT_HEIGHT > 0) && (OUTPUT_WIDTH > 0) && (OFM > 0) &&
		(KERNEL_X > 0) && (KERNEL_Y > 0) && (BATCHES > 0), NO_ZERO_OR_NEGATIVE_DIMENSIONS_IN_INPUTS_OR_OUTPUTS);
	STATIC_ASSERT((INPUT_HEIGHT <= 2048) && (INPUT_WIDTH <= 2048) && (IFM <= 2048) && (OUTPUT_HEIGHT <= 2048) && (OUTPUT_WIDTH <= 2048) && (OFM <= 2048) &&
		(KERNEL_X <= 9) && (KERNEL_Y <= 9) && (BATCHES <= 2048), TEMPLATE_PARAMETER_SIZE_TO_HIGH);
	STATIC_ASSERT((BIASES_SIZE == 0) || (BIASES_SIZE == OFM) || (BIASES_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM)), BIASES_SIZE_IS_NOT_VALID);
	STATIC_ASSERT(((KERNEL_X % 2) == 1) && ((KERNEL_Y % 2) == 1), KERNEL_SIZE_MUST_BE_UNEVEN_NUMBER);
	STATIC_ASSERT(((INPUT_HEIGHT % OUTPUT_HEIGHT) == 0) && ((INPUT_WIDTH % OUTPUT_WIDTH) == 0), INPUT_WIDTH_AND_HEIGHT_MUST_BE_MULTIPLE_OF_OUTPUT_WIDTH_AND_HEIGHT);
	STATIC_ASSERT((INPUT_HEIGHT >= OUTPUT_HEIGHT) && (INPUT_WIDTH >= OUTPUT_WIDTH), INPUT_WIDTH_AND_HEIGHT_MUST_BE_BIGGER_EQUAL_OUTPUT_WIDTH_AND_HEIGHT);

	// Constants
	const vx_int64 SKIP_Y = INPUT_HEIGHT / OUTPUT_HEIGHT;
	const vx_int64 SKIP_X = INPUT_WIDTH / OUTPUT_WIDTH;

	// Batches
	for (vx_int64 b = 0; b < BATCHES; ++b) {

		// 2d Image Pixels
		for (vx_int64 y = 0; y < INPUT_HEIGHT; y += SKIP_Y) {
			for (vx_int64 x = 0; x < INPUT_WIDTH; x += SKIP_X) {

				// Input/Output Feature Maps
				for (vx_int64 ofm = 0; ofm < OFM; ++ofm) {
					float conv3d = 0.0f;
					for (vx_int64 ifm = 0; ifm < IFM; ++ifm) {
						float conv2d = 0.0f;

						// 2d Kernel Pixels
						for (vx_int64 y_kern = 0; y_kern < KERNEL_Y; ++y_kern) {
							for (vx_int64 x_kern = 0; x_kern < KERNEL_X; ++x_kern) {

								// Pointer
								vx_int64 y_src = y + y_kern - (KERNEL_Y / 2);
								vx_int64 x_src = x + x_kern - (KERNEL_X / 2);
								vx_int64 src_ptr = (b * INPUT_HEIGHT * INPUT_WIDTH * IFM) + (y_src * INPUT_WIDTH * IFM) + (x_src * IFM) + ifm;
								vx_int64 kern_ptr = (ofm * IFM * KERNEL_Y * KERNEL_X) + (ifm * KERNEL_Y * KERNEL_X) + (y_kern * KERNEL_X) + x_kern;

								// 2d Convolution
								if (((y_src >= 0) && (y_src < INPUT_HEIGHT)) && ((x_src >= 0) && (x_src < INPUT_WIDTH))) {
									conv2d += src[src_ptr] * weights[kern_ptr];
								}
							}
						}

						// 3d Convolution
						conv3d += conv2d;
					}

					// Pointer
					vx_int64 y_dst = y / SKIP_Y;
					vx_int64 x_dst = x / SKIP_X;
					vx_int64 dst_ptr = (b * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM) + (y_dst * OUTPUT_WIDTH * OFM) + (x_dst * OFM) + ofm;

					// Get bias
					float bias = 0;
					if (BIASES_SIZE == OFM)
						bias = biases[ofm];
					else if (BIASES_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * OFM))
						bias = biases[dst_ptr];

					// Store output
					dst[dst_ptr] = conv3d + bias;
				}
			}
		}
	}
}


/*********************************************************************************************************************/
// 3d convolution using floating point numbers
template<vx_int64 BATCHES, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, 
	vx_int64 FM, vx_int64 KERNEL_Y, vx_int64 KERNEL_X, vx_int64 BIASES_SIZE>
	void SwConvDepthWise(
		float weights[FM * KERNEL_Y * KERNEL_X],
		float biases[BIASES_SIZE],
		float src[BATCHES * INPUT_HEIGHT * INPUT_WIDTH * FM],
		float dst[BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM]) {

	// Compile time checks
	STATIC_ASSERT((INPUT_HEIGHT > 0) && (INPUT_WIDTH > 0) && (FM > 0) && (OUTPUT_HEIGHT > 0) && (OUTPUT_WIDTH > 0) && (FM > 0) &&
		(KERNEL_X > 0) && (KERNEL_Y > 0) && (BATCHES > 0), NO_ZERO_OR_NEGATIVE_DIMENSIONS_IN_INPUTS_OR_OUTPUTS);
	STATIC_ASSERT((INPUT_HEIGHT <= 2048) && (INPUT_WIDTH <= 2048) && (FM <= 2048) && (OUTPUT_HEIGHT <= 2048) && (OUTPUT_WIDTH <= 2048) && (FM <= 2048) &&
		(KERNEL_X <= 9) && (KERNEL_Y <= 9) && (BATCHES <= 2048), TEMPLATE_PARAMETER_SIZE_TO_HIGH);
	STATIC_ASSERT((BIASES_SIZE == 0) || (BIASES_SIZE == FM) || (BIASES_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM)), BIASES_SIZE_IS_NOT_VALID);
	STATIC_ASSERT(((KERNEL_X % 2) == 1) && ((KERNEL_Y % 2) == 1), KERNEL_SIZE_MUST_BE_UNEVEN_NUMBER);
	STATIC_ASSERT(((INPUT_HEIGHT % OUTPUT_HEIGHT) == 0) && ((INPUT_WIDTH % OUTPUT_WIDTH) == 0), INPUT_WIDTH_AND_HEIGHT_MUST_BE_MULTIPLE_OF_OUTPUT_WIDTH_AND_HEIGHT);
	STATIC_ASSERT((INPUT_HEIGHT >= OUTPUT_HEIGHT) && (INPUT_WIDTH >= OUTPUT_WIDTH), INPUT_WIDTH_AND_HEIGHT_MUST_BE_BIGGER_EQUAL_OUTPUT_WIDTH_AND_HEIGHT);

	// Constants
	const vx_int64 SKIP_Y = INPUT_HEIGHT / OUTPUT_HEIGHT;
	const vx_int64 SKIP_X = INPUT_WIDTH / OUTPUT_WIDTH;

	// Batches
	for (vx_int64 b = 0; b < BATCHES; ++b) {

		// 2d Image Pixels
		for (vx_int64 y = 0; y < INPUT_HEIGHT; y += SKIP_Y) {
			for (vx_int64 x = 0; x < INPUT_WIDTH; x += SKIP_X) {

				// Input/Output Feature Maps
				for (vx_int64 fm = 0; fm < FM; ++fm) {
					float conv2d = 0.0f;

					// 2d Kernel Pixels
					for (vx_int64 y_kern = 0; y_kern < KERNEL_Y; ++y_kern) {
						for (vx_int64 x_kern = 0; x_kern < KERNEL_X; ++x_kern) {

							// Pointer
							vx_int64 y_src = y + y_kern - (KERNEL_Y / 2);
							vx_int64 x_src = x + x_kern - (KERNEL_X / 2);
							vx_int64 src_ptr = (b * INPUT_HEIGHT * INPUT_WIDTH * FM) + (y_src * INPUT_WIDTH * FM) + (x_src * FM) + fm;
							vx_int64 kern_ptr = (fm * KERNEL_Y * KERNEL_X) + (y_kern * KERNEL_X) + x_kern;

							// 2d Convolution
							if (((y_src >= 0) && (y_src < INPUT_HEIGHT)) && ((x_src >= 0) && (x_src < INPUT_WIDTH))) {
								conv2d += src[src_ptr] * weights[kern_ptr];
							}
						}
					}

					// Pointer
					vx_int64 y_dst = y / SKIP_Y;
					vx_int64 x_dst = x / SKIP_X;
					vx_int64 dst_ptr = (b * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM) + (y_dst * OUTPUT_WIDTH * FM) + (x_dst * FM) + fm;

					// Get bias
					float bias = 0;
					if (BIASES_SIZE == FM)
						bias = biases[fm];
					else if (BIASES_SIZE == (BATCHES * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM))
						bias = biases[dst_ptr];

					// Store output
					dst[dst_ptr] = conv2d + bias;
				}
			}
		}
	}
}

/***************************************************************************************************************************************************/
// Pooling Layer
template<typename TypeData, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 OUTPUT_HEIGHT, vx_int64 OUTPUT_WIDTH, vx_int64 FM, 
	vx_int64 POOLING_Y, vx_int64 POOLING_X, vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_int64 BATCHES, vx_nn_pooling_type_e POOLING_TYPE>
void SwPooling(float* src, float* dst) {

	// Constants
	const vx_int64 STRIDE_Y_A = OUTPUT_HEIGHT - 1;
	const vx_int64 STRIDE_Y_B = INPUT_HEIGHT + 2 * PADDING_Y - POOLING_Y;
	const vx_int64 STRIDE_X_A = OUTPUT_WIDTH - 1;
	const vx_int64 STRIDE_X_B = INPUT_WIDTH + 2 * PADDING_X - POOLING_X;
	const vx_int64 STRIDE_Y = ((STRIDE_Y_A <= 0) || (STRIDE_Y_B <= 0)) ? (1) : (STRIDE_Y_B / STRIDE_Y_A);
	const vx_int64 STRIDE_X = ((STRIDE_X_A <= 0) || (STRIDE_X_B <= 0)) ? (1) : (STRIDE_X_B / STRIDE_X_A);

	// Iterate over the input images
	for (vx_int64 batch = 0; batch < BATCHES; ++batch) {
		for (vx_int64 y = -PADDING_Y, dst_y = 0; y < INPUT_HEIGHT + PADDING_Y; y += STRIDE_Y) {
			for (vx_int64 x = -PADDING_X, dst_x = 0; x < INPUT_WIDTH + PADDING_X; x += STRIDE_X) {
				for (vx_int64 fm = 0; fm < FM; ++fm) {

					// Initialize pooling
					float result = (POOLING_TYPE == VX_NN_POOLING_MAX) ? (std::numeric_limits<float>::min()) : (0.0f);

					// Compute pooling
					for (vx_int64 py = 0; py < POOLING_Y; ++py) {
						for (vx_int64 px = 0; px < POOLING_X; ++px) {
							vx_int64 src_y = y + py;
							vx_int64 src_x = x + px;
							float data = 0;

							// Get input pixel
							if ((src_y >= 0) && (src_y < INPUT_HEIGHT) && (src_x >= 0) && (src_x < INPUT_WIDTH)) 
								data = src[(batch * INPUT_HEIGHT * INPUT_WIDTH * FM) + (src_y * INPUT_WIDTH * FM) + (src_x * FM) + fm];								

							// Max or Average pooling
							if (POOLING_TYPE == VX_NN_POOLING_MAX) {
								result = MAX(result, data);
							} else {
								result += data;
							}
						}
					}

					// Get average
					if (POOLING_TYPE == VX_NN_POOLING_AVG)
						result = result / (float)(POOLING_Y * POOLING_X);

					// Store result
					if ((dst_y < OUTPUT_HEIGHT) && (dst_x < OUTPUT_WIDTH)) {
						dst[(batch * OUTPUT_HEIGHT * OUTPUT_WIDTH * FM) + (dst_y * OUTPUT_WIDTH * FM) + (dst_x * FM) + fm] = result;
					}
				}
				++dst_x;
				
			}
			++dst_y;
		}
	}	
}

/*********************************************************************************************************************/
// Batch Activation Layer
template<vx_int64 TOTAL_ELEMENTS, vx_nn_activation_function_e ACTIVATION_FUNCTION>
void SwActivation(float param_a, float param_b, float src[TOTAL_ELEMENTS], float dst[TOTAL_ELEMENTS]) {

	for (vx_int64 i = 0; i < TOTAL_ELEMENTS; ++i) {
		float input = src[i];
		float result = 0;

		switch (ACTIVATION_FUNCTION) {

			// f(x) = 1 / (1+e^(-x))
			case VX_NN_ACTIVATION_LOGISTIC:
				result = 1.0f / (1.0f + expf(-input));
				break;

			// f(x) = a * tanh(b * x) 
			case VX_NN_ACTIVATION_HYPERBOLIC_TAN:
				result = param_a * tanh(param_b * input);
				break;

			// f(x) = max(0,x)
			case VX_NN_ACTIVATION_RELU:
				result = MAX(input, 0.0f);
				break;

			// f(x) = min(a, max(0,x))
			case VX_NN_ACTIVATION_BRELU:
				result = MIN(param_a, MAX(input, 0.0f));
				break;

			// f(x) = log(1 + e^(x))
			case VX_NN_ACTIVATION_SOFTRELU:
				result = logf(1.0f + expf(input));
				break;

			// f(x) = |x|
			case VX_NN_ACTIVATION_ABS:
				result = ABS(input);
				break;

			// f(x) = x^(2)
			case VX_NN_ACTIVATION_SQUARE:
				result = input * input;
				break;

			// f(x) = sqrt(x)
			case VX_NN_ACTIVATION_SQRT:
				result = sqrtf(input);
				break;

			// f(x) = a*x + b
			case VX_NN_ACTIVATION_LINEAR:
				result = param_a * input + param_b;
				break;
		}
		dst[i] = result;
	}
}

/*********************************************************************************************************************/
// Batch Normalization
template<vx_int64 BATCHES, vx_int64 PIXELS, vx_int64 FM, vx_int64 WEIGHT_SIZE>
void SwBatchNorm1(float batch_norm[WEIGHT_SIZE * FM], float src[BATCHES * PIXELS * FM], float dst[BATCHES * PIXELS * FM]) {

	// Iterate over pixels [PIXELS] and Feature Maps [FM]
	for (vx_int64 i = 0; i < BATCHES * PIXELS; ++i) {
		for (vx_int64 fm = 0; fm < FM; ++fm) {

			// Read inputs and weights
			float input = src[i * FM + fm];
			float weights[WEIGHT_SIZE] = { 0 };
			for (vx_int64 j = 0; j < WEIGHT_SIZE; ++j) {
				weights[j] = batch_norm[fm * WEIGHT_SIZE + j];
			}
			

			// Compute batch normalization
			float output = weights[0] * ((input - weights[1]) * weights[2]) + weights[3];

			// Write results to output
			dst[i * FM + fm] = output;
		}
	}
}

/*********************************************************************************************************************/
//
template<vx_int32 BATCHES, vx_int64 PIXELS, vx_int64 IFM, vx_int64 WEIGHT_SIZE>
void SwBatchNorm2(
	float weights[WEIGHT_SIZE * IFM],
	float input[BATCHES* PIXELS *IFM],	
	float output[BATCHES* PIXELS *IFM]) {

	// Buffers inputs
	float buffer[IFM];

	// Compute Batch Normalization
	for (vx_int32 i = 0; i < BATCHES * PIXELS; ++i) {

		// Sum for Mean and Variance
		float sum1 = 0, sum2 = 0;

		// Compute sum for Mean
		for (vx_int32 ifm = 0; ifm < IFM; ++ifm) {
			float data = input[(i * IFM) + ifm];
			buffer[ifm] = data;
			sum1 += data;
		}

		// Mean
		float mean = sum1 / IFM;

		// Compute sum for Variance 
		for (vx_int32 ifm = 0; ifm < IFM; ++ifm) {
			float var = buffer[ifm] - mean;
			sum2 += var* var;
		}
			

		// Variance
		float variance = sum2 / IFM;

		// Normalize, Scale, Shift
		for (vx_int32 ifm = 0; ifm < IFM; ++ifm) {
			float normalize = (buffer[ifm] - mean) / sqrtf(variance + 0.000001f);
			float result = weights[WEIGHT_SIZE * ifm + 0] * normalize + weights[WEIGHT_SIZE * ifm + 1];
			output[(i * IFM) + ifm] = result;
		}
	}
}

/*********************************************************************************************************************/
// Softnax Layer
template<vx_int64 BATCHES, vx_int64 FM>
void SwSoftmaxLayer(float src[BATCHES * FM], float dst[BATCHES * FM]) {

	// Buffer the exponential of inputs and the sum of all exponentials
	float* buffer = new float[FM];	
	
	for (vx_int64 b = 0; b < BATCHES; ++b) {
		float sum = 0.0f;

		// Computes: n(i) = exp(src(i)) | N: sum of all n		
		for (vx_int64 fm = 0; fm < FM; ++fm) {
			float input = src[b * FM + fm];
			float data = expf(input);
			sum += data;
			buffer[fm] = data;
		}


		// Computes: m(i) = n(i) / N
		for (vx_int64 fm = 0; fm < FM; ++fm)
			dst[b * FM + fm] = buffer[fm] / sum;
	}

	// Free buffer
	delete[] buffer;
}

/***************************************************************************************************************************************************/
// Fully connected layer (The weights matrix is read row by row, if you consider it as a matrix multiplication)
template<vx_int64 BATCHES, vx_int64 IFM, vx_int64 OFM, vx_int64 BIASES_SIZE, vx_int64 WEIGHTS_SIZE>
void SwFullyConnected(float* src, float* weights, float* biases, float* dst) {

	// Compile time checks
	STATIC_ASSERT((BIASES_SIZE == 0) || (BIASES_SIZE == OFM) || (BIASES_SIZE == (BATCHES*OFM)), BIASES_SIZE_NOT_VALID);

	// Vector * Matrix multiplication
	for (vx_int64 b = 0; b < BATCHES; ++b) {		
		for (vx_int64 row = 0; row < OFM; ++row) {			
			float sum_float = 0.0f;

			for (vx_int64 col = 0; col < IFM; ++col) {
				if (WEIGHTS_SIZE == IFM * OFM)
					sum_float += src[b * IFM + col] * weights[row * IFM + col];
				else if (WEIGHTS_SIZE == BATCHES * IFM * OFM)
					sum_float += src[b * IFM + col] * weights[b * OFM * IFM + row * IFM + col];
			}

			// Add bias if needed			
			if (BIASES_SIZE == OFM)
				sum_float += biases[row];			
			else if (BIASES_SIZE == (BATCHES * OFM))
				sum_float += biases[b * OFM + row];

			// Store result
			dst[b * OFM + row] = sum_float;
		}
	}
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 DST_ROWS, vx_int64 DST_COLS, vx_int64 IFM, vx_int64 OFM,
	vx_int64 KERNEL, vx_int64 BN_WEIGHTS_NUM, vx_nn_activation_function_e ACTIVATION_FUNCTION>
	void SwMnLayer1(TypeData ACTIVATION_PARAM_A, TypeData ACTIVATION_PARAM_B, float* sw_weights, float* sw_biases, float* sw_bn_weights, float* sw_src, float* sw_dst) {

	// Coinstants
	const vx_int64 DST_SIZE = BATCHES * DST_ROWS * DST_COLS * OFM;

	// Allocate
	float* sw_conv_out = new float[DST_SIZE];
	float* sw_bn_out = new float[DST_SIZE];

	// Compute
	SwConvPointWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, OFM, IFM, KERNEL, KERNEL, OFM>(sw_weights, sw_biases, sw_src, sw_conv_out);
	SwBatchNorm2<BATCHES, DST_ROWS * DST_COLS, OFM, BN_WEIGHTS_NUM>(sw_bn_weights, sw_conv_out, sw_bn_out);
	SwActivation<DST_SIZE, ACTIVATION_FUNCTION>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, sw_bn_out, sw_dst);

	// Free
	delete[] sw_conv_out;
	delete[] sw_bn_out;
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 DST_ROWS, vx_int64 DST_COLS, vx_int64 IFM, vx_int64 OFM,
	vx_int64 DW_KERNEL, vx_int64 PW_KERNEL, vx_int64 BN_WEIGHTS_NUM, vx_nn_activation_function_e ACTIVATION_FUNCTION>
	void SwMnLayer2(TypeData ACTIVATION_PARAM_A, TypeData ACTIVATION_PARAM_B, float* dw_sw_weights, float* dw_sw_biases, float* dw_sw_bn_weights, float* pw_sw_weights, float* pw_sw_biases, float* pw_sw_bn_weights, float* sw_src, float* sw_dst) {

	// Constants
	const vx_int64 DST_PIXELS = DST_ROWS * DST_COLS;
	const vx_int64 INT_SIZE = BATCHES * DST_PIXELS * IFM;
	const vx_int64 DST_SIZE = BATCHES * DST_PIXELS * OFM;
	const vx_int64 DW_BIASES_SIZE = IFM;
	const vx_int64 PW_BIASES_SIZE = OFM;

	// Matrixes	
	float* dw_sw_conv_out = new float[INT_SIZE];
	float* dw_sw_bn_out = new float[INT_SIZE];
	float* pw_sw_conv_out = new float[DST_SIZE];
	float* pw_sw_bn_out = new float[DST_SIZE];
	float* sw_internal = new float[INT_SIZE];

	// Compute
	SwConvDepthWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, DW_KERNEL, DW_KERNEL, DW_BIASES_SIZE>(dw_sw_weights, dw_sw_biases, sw_src, dw_sw_conv_out);
	SwBatchNorm2<BATCHES, DST_PIXELS, IFM, BN_WEIGHTS_NUM>(dw_sw_bn_weights, dw_sw_conv_out, dw_sw_bn_out);
	SwActivation<INT_SIZE, ACTIVATION_FUNCTION>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, dw_sw_bn_out, sw_internal);
	SwConvPointWise<BATCHES, DST_ROWS, DST_COLS, DST_ROWS, DST_COLS, OFM, IFM, PW_KERNEL, PW_KERNEL, PW_BIASES_SIZE>(pw_sw_weights, pw_sw_biases, sw_internal, pw_sw_conv_out);
	SwBatchNorm2<BATCHES, DST_PIXELS, OFM, BN_WEIGHTS_NUM>(pw_sw_bn_weights, pw_sw_conv_out, pw_sw_bn_out);
	SwActivation<DST_SIZE, ACTIVATION_FUNCTION>(ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, pw_sw_bn_out, sw_dst);

	// Free
	delete[] dw_sw_conv_out;
	delete[] dw_sw_bn_out;
	delete[] pw_sw_conv_out;
	delete[] pw_sw_bn_out;
	delete[] sw_internal;
}

/***************************************************************************************************************************************************/
template<typename TypeData, vx_int64 BATCHES, vx_int64 SRC_ROWS, vx_int64 SRC_COLS, vx_int64 DST_ROWS, vx_int64 DST_COLS, vx_int64 IFM, vx_int64 OFM,
	vx_int64 PADDING_Y, vx_int64 PADDING_X, vx_nn_pooling_type_e POOLING_TYPE>
	void SwMobileNet3(float* sw_weights, float* sw_biases, float* sw_src, float* sw_dst) {

	// Constants
	const vx_int64 INTERNAL_STREAM = BATCHES * IFM;
	const vx_int64 OUTPUT_STREAM = BATCHES * OFM;
	const vx_int64 KERNEL_Y = SRC_ROWS;
	const vx_int64 KERNEL_X = SRC_COLS;
	const vx_int64 WEIGHTS_SIZE = OFM * IFM;
	const vx_int64 BIASES_SIZE = OFM;

	// Matrixes	
	float* sw_pool = new float[INTERNAL_STREAM];
	float* sw_fully = new float[OUTPUT_STREAM];

	// Compute
	SwPooling<TypeData, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, KERNEL_Y, KERNEL_X, PADDING_Y, PADDING_X, BATCHES, POOLING_TYPE>(sw_src, sw_pool);
	SwFullyConnected<BATCHES, IFM, OFM, BIASES_SIZE, WEIGHTS_SIZE>(sw_pool, sw_weights, sw_biases, sw_fully);
	SwSoftmaxLayer<BATCHES, OFM>(sw_fully, sw_dst);

	// Free
	delete[] sw_pool;
	delete[] sw_fully;
}

/*********************************************************************************************************************/
// Convert to floating point from an image data type
template<typename TypeSrc, vx_int64 FIXED_POINT_POSITION, vx_int64 VECTOR, vx_int64 PARALLEL>
void FixedToFloat(vx_image_data<TypeSrc, PARALLEL> src[VECTOR], float dst[VECTOR * PARALLEL]) {

	// Constants
	const bool is_integer = std::numeric_limits<TypeSrc>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : (1.0f / (float)(1 << FIXED_POINT_POSITION));

	// Convert to floating point from an image data type
	for (vx_int64 i = 0; i < VECTOR; ++i) {
		for (vx_int64 j = 0; j < PARALLEL; ++j) {
			float data = (float)src[i].pixel[j];
			dst[i * PARALLEL + j] = data * SHIFT;
		}
	}
}

/*********************************************************************************************************************/
// Print the single values of a 3d image
template<typename Type, vx_int64 COLS, vx_int64 ROWS, vx_int64 FM, vx_int64 BATCHES>
void Print3d(Type* data, const char* name) {

	// Print name
	printf("%s\n", name);

	// Go through different dimensions
	for (vx_int64 b = 0; b < BATCHES; ++b) {
		for (vx_int64 fm = 0; fm < FM; ++fm) {
			for (vx_int64 y = 0; y < ROWS; ++y) {
				for (vx_int64 x = 0; x < COLS; ++x) {
					printf("%.5f  ", data[(b * ROWS * COLS * FM) + (y * COLS * FM) + (x * FM) + fm]);
				}
				printf("\n");
			}
			printf("\n");
		}
		printf("\n");
	}
	printf("\n");
}

/*********************************************************************************************************************/
// Print the diff of 2 different output 2d images after convolution
template<typename Type, vx_int64 FIXED_POINT_POSITION, vx_int64 BATCHES, vx_int64 ROWS, vx_int64 COLS, vx_int64 OFM>
void Print3dDiff(float* data1, float* data2, const char* name) {

	// Variables
	vx_int64 digits = std::numeric_limits<Type>::digits;
	float min_value = 1.0f / static_cast<float>(1 << digits);
	float abs = 0.0f;
	float avg = 0.0f;
	vx_int64 cnt = 0, cnt2 = 0;

	// Go through different dimensions
	for (vx_int64 b = 0; b < BATCHES; ++b) {
		for (vx_int64 ofm = 0; ofm < OFM; ++ofm) {
			for (vx_int64 y = 0; y < ROWS; ++y) {
				for (vx_int64 x = 0; x < COLS; ++x) {

					float dataA = data1[b * ROWS * COLS * OFM + y * COLS * OFM + x * OFM + ofm];
					float dataB = data2[b * ROWS * COLS * OFM + y * COLS * OFM + x * OFM + ofm];

					// MAPE (Mean absolute percentage error) with division by zero prevention
					float a = std::abs(dataB);
					float b = std::abs(dataA - dataB);

					if (a > 0.0000001) {
						float data = (b / a);
						abs += b;
						avg += data;
						++cnt;
					}
					++cnt2;

					//if (data > 0.01f)
					//	printf("ofm=%4d | x=%4d | y=%4d | batch=%4d | A: %.6f | B: %.6f | error rate: %.6f \n", (vx_int32)ofm, (vx_int32)x, (vx_int32)y, (vx_int32)b, (float)dataA, (float)dataB, data);
				}
			}
		}
	}
	avg /= cnt;
	abs /= cnt;

	printf("%s\t", name);
	printf("MAPE:   %.5f %% \n", avg * 100);
	printf("\n");
}

/***************************************************************************************************************************************************/
// Fill the dat values for weights, biases and input image for 3d convolution
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, 
	vx_int64 BIASES_BATCHES, vx_int64 BIASES_SIZE, vx_int64 BIASES_PARALLEL, vx_int64 WEIGHTS_BATCHES, vx_int64 WEIGHTS_PARALLEL,
	vx_int64 KERNEL_SIZE, vx_int64 IFM, vx_int64 OFM, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL>
void RandomWeightsBiases(float* sw_weights, float* sw_biases, vx_image_data<TypeData, WEIGHTS_PARALLEL>* hw_weights, vx_image_data<TypeData, BIASES_PARALLEL>* hw_biases) {

	// Constants
	const vx_int64 OFM_VECTOR = OFM / OFM_PARALLEL;
	const vx_int64 IFM_VECTOR = IFM / IFM_PARALLEL;
	const vx_int64 CONV_SIZE = KERNEL_SIZE * IFM;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : ((float)(1 << FIXED_POINT_POSITION));
	const vx_int64 CONV_PARALLEL = KERNEL_SIZE * IFM_PARALLEL * OFM_PARALLEL;
	const vx_int64 BIASES_VECTOR = (BIASES_SIZE / OFM_PARALLEL);

	// Convert pointer type
	vx_image_data<TypeData, CONV_PARALLEL>* hw_weights_ptr = (vx_image_data<TypeData, CONV_PARALLEL> *)hw_weights;
	vx_image_data<TypeData, OFM_PARALLEL>* hw_biases_ptr = (vx_image_data<TypeData, OFM_PARALLEL>*)hw_biases;

	// Weights
	vx_int64 sw_ptr = 0;
	for (vx_int64 b = 0; b < WEIGHTS_BATCHES; ++b) {
		for (vx_int64 ofmv = 0; ofmv < OFM_VECTOR; ++ofmv) {
			for (vx_int64 ofmp = 0; ofmp < OFM_PARALLEL; ++ofmp) {
				for (vx_int64 ifmv = 0; ifmv < IFM_VECTOR; ++ifmv) {
					for (vx_int64 ifmp = 0; ifmp < IFM_PARALLEL; ++ifmp) {
						for (vx_int64 k = 0; k < KERNEL_SIZE; ++k) {
							const float weight = (float)(rand() % 192) / (256.0f * (float)CONV_SIZE);
							sw_weights[sw_ptr] = weight;
							hw_weights_ptr[(b * OFM_VECTOR * IFM_VECTOR) + (ofmv * IFM_VECTOR) + ifmv].pixel[(ofmp * IFM_PARALLEL * KERNEL_SIZE) + (ifmp * KERNEL_SIZE) + k] = (TypeData)(weight * SHIFT);
							++sw_ptr;
						}
					}
				}
			}
		}
	}	

	// Biases
	for (vx_int64 b = 0; b < BIASES_BATCHES; ++b) {
		for (vx_int64 i = 0; i < BIASES_VECTOR; ++i) {
			for (vx_int64 j = 0; j < OFM_PARALLEL; ++j) {
				const float bias = (float)(rand() % 256) / 1024.0f;
				sw_biases[b * BIASES_VECTOR * OFM_PARALLEL + i * OFM_PARALLEL + j] = bias;
				hw_biases_ptr[b * BIASES_VECTOR + i].pixel[j] = (TypeData)(bias * SHIFT);
			}
		}
	}
}

/***************************************************************************************************************************************************/
// Fill the dat values for weights, biases and input image for 3d convolution
template<typename TypeData, vx_int64 FIXED_POINT_POSITION,
	vx_int64 WEIGHTS_BATCHES, vx_int64 WEIGHTS_PARALLEL,
	vx_int64 KERNEL_SIZE, vx_int64 IFM, vx_int64 OFM, vx_int64 IFM_PARALLEL, vx_int64 OFM_PARALLEL>
	void RandomWeights(float* sw_weights, vx_image_data<TypeData, WEIGHTS_PARALLEL>* hw_weights) {

	// Constants
	const vx_int64 OFM_VECTOR = OFM / OFM_PARALLEL;
	const vx_int64 IFM_VECTOR = IFM / IFM_PARALLEL;
	const vx_int64 CONV_SIZE = KERNEL_SIZE * IFM;
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : ((float)(1 << FIXED_POINT_POSITION));
	const vx_int64 CONV_PARALLEL = KERNEL_SIZE * IFM_PARALLEL * OFM_PARALLEL;

	// Convert pointer type
	vx_image_data<TypeData, CONV_PARALLEL>* hw_weights_ptr = (vx_image_data<TypeData, CONV_PARALLEL>*)hw_weights;

	// Weights
	vx_int64 sw_ptr = 0;
	for (vx_int64 b = 0; b < WEIGHTS_BATCHES; ++b) {
		for (vx_int64 ofmv = 0; ofmv < OFM_VECTOR; ++ofmv) {
			for (vx_int64 ofmp = 0; ofmp < OFM_PARALLEL; ++ofmp) {
				for (vx_int64 ifmv = 0; ifmv < IFM_VECTOR; ++ifmv) {
					for (vx_int64 ifmp = 0; ifmp < IFM_PARALLEL; ++ifmp) {
						for (vx_int64 k = 0; k < KERNEL_SIZE; ++k) {
							const float weight = (float)(rand() % 192) / (256.0f * (float)CONV_SIZE);
							sw_weights[sw_ptr] = weight;
							hw_weights_ptr[(b * OFM_VECTOR * IFM_VECTOR) + (ofmv * IFM_VECTOR) + ifmv].pixel[(ofmp * IFM_PARALLEL * KERNEL_SIZE) + (ifmp * KERNEL_SIZE) + k] = (TypeData)(weight * SHIFT);
							++sw_ptr;
						}
					}
				}
			}
		}
	}
}

/***************************************************************************************************************************************************/
// Fill the dat values for weights, biases and input image for 3d convolution
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 BIASES_BATCHES, vx_int64 BIASES_SIZE, vx_int64 BIASES_PARALLEL, vx_int64 OFM_PARALLEL>
void RandomBiases(float* sw_biases, vx_image_data<TypeData, BIASES_PARALLEL>* hw_biases) {

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : ((float)(1 << FIXED_POINT_POSITION));
	const vx_int64 BIASES_VECTOR = (BIASES_SIZE / OFM_PARALLEL);

	// Convert pointer type
	vx_image_data<TypeData, OFM_PARALLEL>* hw_biases_ptr = (vx_image_data<TypeData, OFM_PARALLEL>*)hw_biases;

	// Biases
	for (vx_int64 b = 0; b < BIASES_BATCHES; ++b) {
		for (vx_int64 i = 0; i < BIASES_VECTOR; ++i) {
			for (vx_int64 j = 0; j < OFM_PARALLEL; ++j) {
				const float bias = (float)(rand() % 256) / 1024.0f;
				sw_biases[b * BIASES_VECTOR * OFM_PARALLEL + i * OFM_PARALLEL + j] = bias;
				hw_biases_ptr[b * BIASES_VECTOR + i].pixel[j] = (TypeData)(bias * SHIFT);
			}
		}
	}
}

/***************************************************************************************************************************************************/
// 
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 FM, vx_int64 FM_PARALLEL, vx_int64 DMA_PARALLEL, vx_int64 WEIGHT_SIZE>
void RandomBnWeights(float* sw_batch_norm, vx_image_data<TypeData, DMA_PARALLEL>* hw_batch_norm) {

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : ((float)(1 << FIXED_POINT_POSITION));
	const vx_int64 WEIGHTS_PARALLEL = FM_PARALLEL * WEIGHT_SIZE;

	//
	vx_image_data<TypeData, WEIGHTS_PARALLEL>* hw_batch_norm_ptr = (vx_image_data<TypeData, WEIGHTS_PARALLEL>*)hw_batch_norm;

	// Weights values
	for (vx_int64 i = 0; i < (FM * WEIGHT_SIZE) / WEIGHTS_PARALLEL; ++i) {
		for (vx_int64 j = 0; j < WEIGHTS_PARALLEL; ++j) {
			const float val = (float)((rand() % 256) / 768.0f);
			sw_batch_norm[i * WEIGHTS_PARALLEL + j] = val;
			hw_batch_norm_ptr[i].pixel[j] = (TypeData)(val * SHIFT);
		}
	}
}

/***************************************************************************************************************************************************/
// 
template<typename TypeData, vx_int64 FIXED_POINT_POSITION, vx_int64 INPUT_HEIGHT, vx_int64 INPUT_WIDTH, vx_int64 IFM, vx_int64 BATCHES, vx_int64 DMA_PARALLEL>
void RandomInputs(float* sw_input, vx_image_data<TypeData, DMA_PARALLEL>* hw_input) {

	// Constants
	const bool is_integer = std::numeric_limits<TypeData>::is_integer;
	const float SHIFT = (is_integer == false) ? (1.0f) : ((float)(1 << FIXED_POINT_POSITION));
	const vx_int64 TOTAL_ELEMENTS = BATCHES * INPUT_HEIGHT * INPUT_WIDTH * IFM;

	// Inputs
	for (vx_int64 i = 0; i < TOTAL_ELEMENTS / DMA_PARALLEL; ++i) {
		for (vx_int64 j = 0; j < DMA_PARALLEL; ++j) {
			const float value = (float)((rand() % 256) / 256.0f);
			sw_input[i * DMA_PARALLEL + j] = value;
			hw_input[i].pixel[j] = (TypeData)(value * SHIFT);
		}
	}
}

/***************************************************************************************************************************************************/
// Evaluate Time & Error Rate
template<typename TypeData, vx_int64 ITERATIONS, vx_int64 FIXED_POINT_POSITION, vx_int64 BATCHES, vx_int64 ROWS, vx_int64 COLS, vx_int64 FM, vx_int64 FM_PARALLEL>
void Evaluate(float* sw_output, vx_image_data<TypeData, FM_PARALLEL>* hw_output, double* sw_time, double* hw_time, const char* name) {

	// Constants
	const vx_int64 DST_SIZE = BATCHES * ROWS * COLS * FM;

	// Allocate
	float* hw_output_test = new float[DST_SIZE];

	// Avaluate time & error rate
#ifdef __EVAL_TIME__
	EvaluateTime<ITERATIONS>(sw_time, hw_time, name);
#endif
	FixedToFloat<TypeData, FIXED_POINT_POSITION, (DST_SIZE / FM_PARALLEL), FM_PARALLEL>(hw_output, hw_output_test);
	Print3dDiff<TypeData, FIXED_POINT_POSITION, BATCHES, ROWS, COLS, FM>(sw_output, hw_output_test, name);

	// Free memory
	delete[] hw_output_test;
}

#endif // NN_TEST_H_
