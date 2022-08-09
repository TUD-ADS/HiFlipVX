/**
* @file    main.cpp
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
*
* @section LICENSE
* GNU GPLv3:
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @section DESCRIPTION
* This is the main function for testing
*/

/*
ISSUES: SDSoC does not allow:
- constexpr for parameters
- Template test functions that call the accelerator
- Template parameters from namespace constants
- In general there are a lot problem with C++11 in version 2018.3
*/

#include "nn_test.h"
#include "nn_mobile_net.h"

// Global parameters
#define BATCH_NORM1_WEIGHTS_NUM  4 // Amount of weights for batch normalization
#define BATCH_NORM2_WEIGHTS_NUM  2 // Amount of weights for batch normalization
#define ITERATIONS               4 // Iterations for timing evaluation

// Just to test SDSoC outside of the tool
//#define __SDSCC__

/***************************************************************************************************************************************************/
// Manual Parameters for the different layers
/***************************************************************************************************************************************************/

// Shared parameters
typedef vx_uint16 NnType;                                // uint8, uint16, int8, int16, float32
typedef vx_uint32 NnTypeBias;                            // uint8, uint16, int8, int16, float32, int32, uint32
#define NN_BATCHES                                   4  // Amount of images. Can give error if 1 due to weird unrolling of HLS
#define NN_FP_POSITION                              16 // Position of the fixed point. Signed values need at least 1 integer.
#define NN_ROUND_TYPE          VX_ROUND_POLICY_TO_ZERO  // Underflow: vx_round_policy_e
#define NN_OVERFLOW_TYPE    VX_CONVERT_POLICY_SATURATE  // Overflow: vx_convert_policy_e
#define BUFFER_COEFFICIENTS                       true  // Buffers weights, biases internally on first use
#define NN_DMA_PARALLEL           (8 / sizeof(NnType))

// Point-wise convolution paramater
#define C1_SRC_ROWS                                64 // [1 .. 2048] && ((SRC_ROWS % DST_ROWS) == 0) && (SRC_ROWS >= DST_ROWS)
#define C1_SRC_COLS                                64 // [1 .. 2048] && ((SRC_COLS % DST_COLS) == 0) && (SRC_COLS >= DST_COLS)
#define C1_DST_ROWS                                64 // [1 .. 2048]
#define C1_DST_COLS                                64 // [1 .. 2048]
#define C1_IFM                                     32 // [1 .. 2048] && (IFM >= IFM_PARALLEL) && (IFM % IFM_PARALLEL == 0)
#define C1_OFM                                     32 // [1 .. 2048] && (OFM >= OFM_PARALLEL) && (OFM % OFM_PARALLEL == 0)
#define C1_IFM_PARALLEL                             1 // [1 .. 128]
#define C1_OFM_PARALLEL                             1 // [1 .. 128]
#define C1_KERNEL_X                                 3 // [1 .. 9] && ((KERNEL_X % 2) == 1)
#define C1_KERNEL_Y                                 3 // [1 .. 9] && ((KERNEL_Y % 2) == 1)
#define C1_BIASES_SIZE                         C1_OFM // 0, OFM, BATCHES*OFM (if coefficients are not buffered)

// Depth-wise convolution paramater
#define C2_SRC_ROWS                                64 // [1 .. 2048] && (SRC_ROWS >= DST_ROWS)
#define C2_SRC_COLS                                64 // [1 .. 2048] && (SRC_COLS >= DST_COLS)
#define C2_DST_ROWS                                64 // [1 .. 2048]
#define C2_DST_COLS                                64 // [1 .. 2048]
#define C2_FM                                      32 // [1 .. 2048] && (FM >= FM_PARALLEL) && (FM % FM_PARALLEL == 0)
#define C2_FM_PARALLEL                              1 // [1 .. 128]
#define C2_KERNEL_X                                 3 // [1 .. 9] && ((KERNEL_X % 2) == 1)
#define C2_KERNEL_Y                                 3 // [1 .. 9] && ((KERNEL_Y % 2) == 1)
#define C2_BIASES_SIZE                          C2_FM // 0, FM, BATCHES*FM (if coefficients are not buffered)

// Pooling Layer
#define P1_SRC_ROWS                                 64 // [1 .. 2048] && ((SRC_ROWS % DST_ROWS) == 0) && (SRC_ROWS >= DST_ROWS)
#define P1_SRC_COLS                                 64 // [1 .. 2048] && ((SRC_COLS % DST_COLS) == 0) && (SRC_COLS >= DST_COLS)
#define P1_DST_ROWS                                 33 // [1 .. 2048]
#define P1_DST_COLS                                 33 // [1 .. 2048]
#define P1_FM                                       32 // [1 .. 2048] && (FM >= FM_PARALLEL) && (FM % FM_PARALLEL == 0)
#define P1_FM_PARALLEL                               1 // [1 .. 128]
#define P1_KERNEL_Y                                  2 // [1 .. SRC_ROWS] && [1 .. 9]
#define P1_KERNEL_X                                  2 // [1 .. SRC_COLS] && [1 .. 9]
#define P1_PADDING_Y                                 1 // [0 .. POOLING_Y / 2]
#define P1_PADDING_X                                 1 // [0 .. POOLING_X / 2]
#define P1_POOLING_TYPE              VX_NN_POOLING_MAX // vx_nn_pooling_type_e

// Batchnorm Layer
#define B1_PIXELS                               64*64 // (PIXELS > 0)
#define B1_FM                                      32 // (FM >= PARALLEL) && (FM % PARALLEL == 0)
#define B1_PARALLEL                                 1 // (PARALLEL > 0)
#define B1_WEIGHTS_NUM        BATCH_NORM2_WEIGHTS_NUM //

// Activation Layer
#define A1_ELEMENTS                          64*64*32 // (ELEMENTS >= PARALLEL) && (ELEMENTS % PARALLEL == 0)
#define A1_PARALLEL                                 1 // (PARALLEL > 0)
#define A1_RELAXED_MATH                         false // less accurate sqrt if set to true
#define A1_PARAM_A                          (1 << 15) // same data type as image input/output
#define A1_PARAM_B                          (1 << 15) // same data type as image input/output
#define A1_ACTIVATION_FUNCTION  VX_NN_ACTIVATION_RELU // vx_nn_activation_function_e

// Fully Connected Layer
#define F1_IFM                                    256 // (IFM >= PARALLEL) && (IFM % PARALLEL == 0)
#define F1_OFM                                    256 // (OFM >= PARALLEL) && (IFM % PARALLEL == 0)
#define F1_PARALLEL                                 1 // (PARALLEL > 0)
#define F1_BIAS_SIZE                           F1_OFM // 0, OFM, BATCHES*OFM (if coefficients are not buffered)
#define F1_WEIGHT_SIZE                  F1_IFM*F1_OFM // IFM*OFM, BATCHES*IFM*OFM (if coefficients are not buffered)

// Softmax Layer
#define S1_FM                                    256 // (FM >= PARALLEL) && (FM % PARALLEL == 0)
#define S1_PARALLEL                                1 // (PARALLEL > 0)
#define S1_FP_POSTION                             20 // for the internal 32-bit summation

/***************************************************************************************************************************************************/
// Automatic Parameters for the different layers
/***************************************************************************************************************************************************/

// Point-wise convolution Layer
const vx_int64 C1_SRC_SIZE = (vx_int64)NN_BATCHES * (vx_int64)C1_SRC_ROWS * (vx_int64)C1_SRC_COLS * (vx_int64)C1_IFM;
const vx_int64 C1_DST_SIZE = (vx_int64)NN_BATCHES * (vx_int64)C1_DST_ROWS * (vx_int64)C1_DST_COLS * (vx_int64)C1_OFM;
const vx_int64 C1_WEIGHTS_SIZE = (vx_int64)C1_KERNEL_Y * (vx_int64)C1_KERNEL_X * (vx_int64)C1_IFM * (vx_int64)C1_OFM;
const vx_int64 C1_WEIGHTS_PARALLEL = (vx_int64)C1_KERNEL_Y* (vx_int64)C1_KERNEL_X* (vx_int64)C1_IFM_PARALLEL* (vx_int64)C1_OFM_PARALLEL;
#if ((C1_IFM_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnType c1_src_image_type;
#else
typedef vx_image_data<NnType, C1_IFM_PARALLEL> c1_src_image_type;
#endif
#if ((C1_OFM_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnTypeBias c1_dst_image_type_bias;
typedef NnType c1_dst_image_type;
#else
typedef vx_image_data<NnTypeBias, C1_OFM_PARALLEL> c1_dst_image_type_bias;
typedef vx_image_data<NnType, C1_OFM_PARALLEL> c1_dst_image_type;
#endif
#if (((C1_KERNEL_Y * C1_KERNEL_X * C1_IFM_PARALLEL * C1_OFM_PARALLEL) == 1) && defined(__SDSCC__)) 
	typedef NnType c1_weights_image_type1;
#else
	typedef vx_image_data<NnType, C1_WEIGHTS_PARALLEL> c1_weights_image_type1;
#endif
typedef vx_image_data<NnType, NN_DMA_PARALLEL> c1_weights_image_type2;

// Depth-wise convolution Layer
const vx_int64 C2_SRC_SIZE = (vx_int64)NN_BATCHES * (vx_int64)C2_SRC_ROWS * (vx_int64)C2_SRC_COLS * (vx_int64)C2_FM;
const vx_int64 C2_DST_SIZE = (vx_int64)NN_BATCHES * (vx_int64)C2_DST_ROWS * (vx_int64)C2_DST_COLS * (vx_int64)C2_FM;
const vx_int64 C2_WEIGHTS_SIZE = (vx_int64)C2_KERNEL_Y * (vx_int64)C2_KERNEL_X * (vx_int64)C2_FM;
const vx_int64 C2_WEIGHTS_PARALLEL = (vx_int64)C2_KERNEL_Y * (vx_int64)C2_KERNEL_X * (vx_int64)C2_FM_PARALLEL;
#if ((C2_FM_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnTypeBias c2_image_type_bias;
typedef NnType c2_image_type;
#else
typedef vx_image_data<NnTypeBias, C2_FM_PARALLEL> c2_image_type_bias;
typedef vx_image_data<NnType, C2_FM_PARALLEL> c2_image_type;
#endif
typedef vx_image_data<NnType, C2_WEIGHTS_PARALLEL> c2_weights_image_type;

// Pooling Layer
const vx_int64 P1_SRC_SIZE = (vx_int64)NN_BATCHES * (vx_int64)P1_SRC_ROWS * (vx_int64)P1_SRC_COLS * (vx_int64)P1_FM;
const vx_int64 P1_DST_SIZE = (vx_int64)NN_BATCHES * (vx_int64)P1_DST_ROWS * (vx_int64)P1_DST_COLS * (vx_int64)P1_FM;
#if ((P1_FM_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnType p1_image_type;
#else
typedef vx_image_data<NnType, P1_FM_PARALLEL> p1_image_type;
#endif

// Batchnorm Layer
const vx_int64 B1_IMG_SIZE = (vx_int64)NN_BATCHES * (vx_int64)B1_PIXELS * (vx_int64)B1_FM;
const vx_int64 B1_WEIGHTS1_PARALLEL = (vx_int64)BATCH_NORM1_WEIGHTS_NUM * (vx_int64)B1_PARALLEL;
const vx_int64 B1_WEIGHTS2_PARALLEL = (vx_int64)BATCH_NORM2_WEIGHTS_NUM * (vx_int64)B1_PARALLEL;
#if ((B1_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnType b1_image_type;
#else
typedef vx_image_data<NnType, B1_PARALLEL> b1_image_type;
#endif
typedef vx_image_data<NnType, B1_WEIGHTS1_PARALLEL> b1_weights1_image_type;
typedef vx_image_data<NnType, B1_WEIGHTS2_PARALLEL> b1_weights2_image_type;

// Activation Layer
const vx_int64 A1_IMG_SIZE = (vx_int64)NN_BATCHES * (vx_int64)A1_ELEMENTS;
#if ((A1_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnType a1_image_type;
#else
typedef vx_image_data<NnType, A1_PARALLEL> a1_image_type;
#endif

// Fully Connected Layer
const vx_int64 F1_SRC_SIZE = (vx_int64)NN_BATCHES * (vx_int64)F1_IFM;
const vx_int64 F1_DST_SIZE = (vx_int64)NN_BATCHES * (vx_int64)F1_OFM;
#if defined(__SDSCC__)
#if (F1_PARALLEL == 1)
typedef NnType f1_src_image_type;
typedef NnType f1_dst_image_type;
typedef NnTypeBias f1_dst_image_type_bias;
#else
typedef vx_image_data<NnType, F1_PARALLEL> f1_src_image_type;
typedef NnType f1_dst_image_type;
typedef NnTypeBias f1_dst_image_type_bias;
#endif
#else
typedef vx_image_data<NnType, F1_PARALLEL> f1_src_image_type;
typedef vx_image_data<NnType, 1> f1_dst_image_type;
typedef vx_image_data<NnTypeBias, 1> f1_dst_image_type_bias;
#endif

// Softmax
const vx_int64 S1_IMG_SIZE = (vx_int64)NN_BATCHES * (vx_int64)S1_FM;
#if ((S1_PARALLEL == 1) && defined(__SDSCC__)) 
typedef NnType s1_image_type;
#else
typedef vx_image_data<NnType, S1_PARALLEL> s1_image_type;
#endif

/***************************************************************************************************************************************************/
// Manual Parameters for the Mobilenets layers
/***************************************************************************************************************************************************/

// Shared parameters
typedef vx_uint16 MnType;                            // uint8, int8, uin16, int16, float32
const vx_int64 MN_DMA_PARALLEL = 8 / sizeof(MnType); // 64-bit interfaces to global memory
const vx_int64 MN_BATCHES = 4;                       // Amount of images before coefficients are re-read. MN_BATCHES=1 can make problems with SDx
const vx_int64 MN_FP_POSITION = 16;                  // Maximum are 8(uint8), (7)int8, (16)uin16, (15)int16, (24)float32 
const vx_round_policy_e MN_ROUND_POLICY = VX_ROUND_POLICY_TO_ZERO;         // VX_ROUND_POLICY_TO_ZERO, VX_ROUND_POLICY_TO_NEAREST_EVEN
const vx_convert_policy_e MN_OVERFLOW_POLICY = VX_CONVERT_POLICY_SATURATE; // VX_CONVERT_POLICY_WRAP, VX_CONVERT_POLICY_SATURATE

// Mobile Net Layer 1 Parameters
const vx_int64 MN1_SRC_ROWS = 224;  
const vx_int64 MN1_SRC_COLS = 224;  
const vx_int64 MN1_DST_ROWS = 112;  
const vx_int64 MN1_DST_COLS = 112;  
const vx_int64 MN1_IFM = 3;         
const vx_int64 MN1_OFM = 16;        
const vx_int64 MN1_DW_PARALLEL = 1; 
const vx_int64 MN1_IFM_PARALLEL = 3;
const vx_int64 MN1_OFM_PARALLEL = 8;
const vx_int64 MN1_PW_PARALLEL = 2; 

// Mobile Net Layer 2-14 Parameters
const vx_int64 MN2_SRC_ROWS = 112;
const vx_int64 MN2_SRC_COLS = 112;
const vx_int64 MN2_DST_ROWS = 112;
const vx_int64 MN2_DST_COLS = 112;
const vx_int64 MN2_IFM = 16;
const vx_int64 MN2_OFM = 32;
const vx_int64 MN2_DW_PARALLEL = 2;
const vx_int64 MN2_IFM_PARALLEL = 8;
const vx_int64 MN2_OFM_PARALLEL = 8;
const vx_int64 MN2_PW_PARALLEL = 4;

// Mobile Net Layer 15 Parameters
const vx_int64 MN3_SRC_ROWS = 7;
const vx_int64 MN3_SRC_COLS = 7;
const vx_int64 MN3_DST_ROWS = 1;
const vx_int64 MN3_DST_COLS = 1;
const vx_int64 MN3_IFM = 512;
const vx_int64 MN3_OFM = 1000;
//const vx_int64 MN3_DW_PARALLEL = 1;
const vx_int64 MN3_IFM_PARALLEL = 8;
const vx_int64 MN3_OFM_PARALLEL = 1;
//const vx_int64 MN3_PW_PARALLEL = 1;

// Alignment for coefficient buffers
const vx_int64 MN1_COEFF_SIZE = (ALIGN(MN1_OFM * MN1_IFM * 9, MN_DMA_PARALLEL) + ALIGN(MN1_OFM, MN_DMA_PARALLEL) + 
	ALIGN(MN1_OFM * BATCH_NORM1_WEIGHTS_NUM, MN_DMA_PARALLEL)) / MN_DMA_PARALLEL;
const vx_int64 MN2_COEFF_SIZE = (ALIGN(MN2_IFM * 9, MN_DMA_PARALLEL) + ALIGN(MN2_IFM, MN_DMA_PARALLEL) + 
	ALIGN(MN2_IFM * BATCH_NORM1_WEIGHTS_NUM, MN_DMA_PARALLEL) + ALIGN(MN2_OFM * MN2_IFM, MN_DMA_PARALLEL) + ALIGN(MN2_OFM, MN_DMA_PARALLEL) + 
	ALIGN(MN2_OFM * BATCH_NORM1_WEIGHTS_NUM, MN_DMA_PARALLEL)) / MN_DMA_PARALLEL;
const vx_int64 MN3_COEFF_SIZE = (ALIGN(MN3_OFM * MN3_IFM, MN_DMA_PARALLEL) + ALIGN(MN3_OFM, MN_DMA_PARALLEL)) / MN_DMA_PARALLEL;

// Values of the different MobileNets Layers
// mn01 = { 224, 224,   112, 112,     3,   16,   1, 3,  8, 2 };
// mn02 = { 112, 112,   112, 112,    16,   32,   2, 8,  8, 4 };
// mn03 = { 112, 112,    56,  56,    32,   64,   4, 8,  8, 2 };
// mn04 = {  56,  56,    56,  56,    64,   64,   2, 8, 16, 2 };
// mn05 = {  56,  56,    28,  28,    64,  128,   2, 8,  8, 1 };
// mn06 = {  28,  28,    28,  28,   128,  128,   1, 8, 16, 1 };
// mn07 = {  28,  28,    14,  14,   128,  256,   1, 8,  8, 1 };
// mn08 = {  14,  14,    14,  14,   256,  256,   1, 8, 16, 1 };
// mn09 = {  14,  14,    14,  14,   256,  256,   1, 8, 16, 1 };
// mn10 = {  14,  14,    14,  14,   256,  256,   1, 8, 16, 1 };
// mn11 = {  14,  14,    14,  14,   256,  256,   1, 8, 16, 1 };
// mn12 = {  14,  14,    14,  14,   256,  256,   1, 8, 16, 1 };
// mn13 = {  14,  14,     7,   7,   256,  512,   1, 8,  8, 1 };
// mn14 = {   7,   7,     7,   7,   512,  512,   1, 8, 16, 1 };
// mn15 = {   7,   7,     1,   1,   512, 1000,   1, 8,  1, 1 };

/***************************************************************************************************************************************************/
void HwConvPointWise(
	c1_src_image_type         src[C1_SRC_SIZE     / C1_IFM_PARALLEL],
	c1_weights_image_type1 weights[C1_WEIGHTS_SIZE / C1_WEIGHTS_PARALLEL],
	c1_dst_image_type_bias biases[C1_BIASES_SIZE  / C1_OFM_PARALLEL],
	c1_dst_image_type         dst[C1_DST_SIZE     / C1_OFM_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=biases
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=biases
#pragma HLS interface ap_fifo port=dst
#endif
	ImgConvPointWise<NnType, NnTypeBias, NN_FP_POSITION, C1_SRC_ROWS, C1_SRC_COLS, C1_IFM, C1_DST_ROWS, C1_DST_COLS, C1_OFM, C1_KERNEL_X, 
		C1_KERNEL_Y, C1_BIASES_SIZE, NN_BATCHES, C1_IFM_PARALLEL, C1_OFM_PARALLEL, NN_OVERFLOW_TYPE, NN_ROUND_TYPE, BUFFER_COEFFICIENTS>(src, 
		weights, biases, dst);
}

/***************************************************************************************************************************************************/
void HwConvPwDma(
	c1_src_image_type         src[C1_SRC_SIZE     / C1_IFM_PARALLEL],
	c1_weights_image_type2 weights[C1_WEIGHTS_SIZE / NN_DMA_PARALLEL],
	c1_dst_image_type_bias biases[C1_BIASES_SIZE  / C1_OFM_PARALLEL],
	c1_dst_image_type         dst[C1_DST_SIZE     / C1_OFM_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=biases
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=biases
#pragma HLS interface ap_fifo port=dst
#endif
	STATIC_ASSERT((C1_WEIGHTS_SIZE % NN_DMA_PARALLEL) == 0, MISSMATCH);
#ifdef __XILINX__
	vx_image_data<NnType, C1_WEIGHTS_PARALLEL> weights_stream[C1_WEIGHTS_SIZE / C1_WEIGHTS_PARALLEL];
#pragma HLS data_pack variable=weights_stream
#pragma HLS stream variable=weights_stream depth=8
#pragma HLS RESOURCE variable=weights_stream core=FIFO_LUTRAM
#else
	vx_image_data<NnType, C1_WEIGHTS_PARALLEL>* weights_stream = new vx_image_data<NnType, C1_WEIGHTS_PARALLEL>[C1_WEIGHTS_SIZE / C1_WEIGHTS_PARALLEL];
#endif

#pragma HLS DATAFLOW
	ImgVectorSizeConverter<NnType, 1, C1_WEIGHTS_SIZE, C1_WEIGHTS_SIZE, NN_DMA_PARALLEL, C1_WEIGHTS_PARALLEL>(weights, weights_stream);
	ImgConvPointWise<NnType, NnTypeBias, NN_FP_POSITION, C1_SRC_ROWS, C1_SRC_COLS, C1_IFM, C1_DST_ROWS, C1_DST_COLS, C1_OFM, C1_KERNEL_X, 
		C1_KERNEL_Y, C1_BIASES_SIZE, NN_BATCHES, C1_IFM_PARALLEL, C1_OFM_PARALLEL, NN_OVERFLOW_TYPE, NN_ROUND_TYPE, BUFFER_COEFFICIENTS>(src, 
		weights_stream, biases, dst);

#ifndef __XILINX__
	delete[] weights_stream;
#endif
}

/***************************************************************************************************************************************************/
void HwConvDepthWise(
	c2_image_type             src[C2_SRC_SIZE     / C2_FM_PARALLEL],
	c2_weights_image_type weights[C2_WEIGHTS_SIZE / C2_WEIGHTS_PARALLEL],
	c2_image_type_bias     biases[C2_BIASES_SIZE  / C2_FM_PARALLEL],
	c2_image_type             dst[C2_DST_SIZE     / C2_FM_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=biases
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=biases
#pragma HLS interface ap_fifo port=dst
#endif
	ImgConvDepthWise<NnType, NnTypeBias, NN_FP_POSITION, C2_SRC_ROWS, C2_SRC_COLS, C2_DST_ROWS, C2_DST_COLS, C2_FM, C2_KERNEL_X, C2_KERNEL_Y, 
		C2_BIASES_SIZE, NN_BATCHES, C2_FM_PARALLEL, NN_OVERFLOW_TYPE, NN_ROUND_TYPE, BUFFER_COEFFICIENTS>(src, weights, biases, dst);
}

/***************************************************************************************************************************************************/
void HwPooling(
	p1_image_type src[P1_SRC_SIZE / P1_FM_PARALLEL],
	p1_image_type dst[P1_DST_SIZE / P1_FM_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	ImgPooling<NnType, NN_FP_POSITION, P1_SRC_ROWS, P1_SRC_COLS, P1_DST_ROWS, P1_DST_COLS, P1_FM, P1_FM_PARALLEL, P1_KERNEL_Y, P1_KERNEL_X, 
		P1_PADDING_Y, P1_PADDING_X, NN_BATCHES, P1_POOLING_TYPE>(src, dst);
}

/***************************************************************************************************************************************************/
void HwBatchNorm1(
	b1_weights1_image_type weights[B1_FM       / B1_PARALLEL],
	b1_image_type             src[B1_IMG_SIZE / B1_PARALLEL],
	b1_image_type             dst[B1_IMG_SIZE / B1_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	ImgBatchNorm1<NnType, NN_BATCHES, B1_PIXELS, B1_FM, B1_PARALLEL, NN_FP_POSITION, NN_ROUND_TYPE, NN_OVERFLOW_TYPE, BATCH_NORM1_WEIGHTS_NUM,
		BUFFER_COEFFICIENTS>(weights, src, dst);
}

/***************************************************************************************************************************************************/
void HwBatchNorm2(
	b1_weights2_image_type weights[B1_FM / B1_PARALLEL],
	b1_image_type             src[B1_IMG_SIZE / B1_PARALLEL],
	b1_image_type             dst[B1_IMG_SIZE / B1_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	ImgBatchNorm2<NnType, NN_BATCHES, B1_PIXELS, B1_FM, B1_PARALLEL, NN_FP_POSITION, NN_ROUND_TYPE, NN_OVERFLOW_TYPE, BATCH_NORM2_WEIGHTS_NUM,
		BUFFER_COEFFICIENTS>(weights, src, dst);
}

/***************************************************************************************************************************************************/
void HwActivation(
	a1_image_type src[A1_IMG_SIZE / A1_PARALLEL],
	a1_image_type dst[A1_IMG_SIZE / A1_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	ImgActivationLayer<NnType, NN_BATCHES, A1_ELEMENTS, A1_PARALLEL, NN_FP_POSITION, NN_ROUND_TYPE, NN_OVERFLOW_TYPE, A1_ACTIVATION_FUNCTION, 
		A1_RELAXED_MATH>((NnType)A1_PARAM_A, (NnType)A1_PARAM_B, src, dst);
}

/***************************************************************************************************************************************************/
void HwFullyConnected(
	f1_src_image_type         src[F1_SRC_SIZE    / F1_PARALLEL],
	f1_src_image_type     weights[F1_WEIGHT_SIZE / F1_PARALLEL],
	f1_dst_image_type_bias biases[F1_BIAS_SIZE   / 1],
	f1_dst_image_type         dst[F1_DST_SIZE    / 1]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=weights
#pragma HLS interface axis port=biases
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=weights
#pragma HLS interface ap_fifo port=biases
#pragma HLS interface ap_fifo port=dst
#endif
	ImgFullyConnected<NnType, NnTypeBias, NN_BATCHES, F1_IFM, F1_OFM, F1_PARALLEL, F1_BIAS_SIZE, F1_WEIGHT_SIZE, NN_FP_POSITION, NN_ROUND_TYPE, 
		NN_OVERFLOW_TYPE, BUFFER_COEFFICIENTS>(src, weights, biases, dst);
}

/***************************************************************************************************************************************************/
void HwSoftmax(
	s1_image_type src[S1_IMG_SIZE / S1_PARALLEL],
	s1_image_type dst[S1_IMG_SIZE / S1_PARALLEL]) {
#ifndef __SDSCC__ 
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst	
#else
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst	
#endif
	ImgSoftmaxLayer<NnType, NN_BATCHES, S1_FM, S1_PARALLEL, NN_FP_POSITION, NN_ROUND_TYPE>(src, dst);
}

/***************************************************************************************************************************************************/
void MnLayer1(
	vx_image_data<MnType, MN_DMA_PARALLEL> coefficients[MN1_COEFF_SIZE],
	vx_image_data<MnType, MN_DMA_PARALLEL> src[(MN_BATCHES * MN1_SRC_ROWS * MN1_SRC_COLS * MN1_IFM) / MN_DMA_PARALLEL],
	vx_image_data<MnType, MN1_PW_PARALLEL> dst[(MN_BATCHES * MN1_DST_ROWS * MN1_DST_COLS * MN1_OFM) / MN1_PW_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=coefficients
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=coefficients
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	MobileNet1<MnType, MN1_SRC_ROWS, MN1_SRC_COLS, MN1_DST_ROWS, MN1_DST_COLS, MN1_IFM, MN1_OFM, MN1_DW_PARALLEL, MN1_IFM_PARALLEL, MN1_OFM_PARALLEL,
		MN1_PW_PARALLEL, MN_DMA_PARALLEL, MN_BATCHES, MN_FP_POSITION, MN_ROUND_POLICY, MN_OVERFLOW_POLICY>(coefficients, src, dst);
}

/***************************************************************************************************************************************************/
void MnLayer2(
	vx_image_data<MnType, MN_DMA_PARALLEL> coefficients[MN2_COEFF_SIZE],
	vx_image_data<MnType, MN2_DW_PARALLEL> src[(MN_BATCHES * MN2_SRC_ROWS * MN2_SRC_COLS * MN2_IFM) / MN2_DW_PARALLEL],
	vx_image_data<MnType, MN2_PW_PARALLEL> dst[(MN_BATCHES * MN2_DST_ROWS * MN2_DST_COLS * MN2_OFM) / MN2_PW_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=coefficients
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=coefficients
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	MobileNet2<MnType, MN2_SRC_ROWS, MN2_SRC_COLS, MN2_DST_ROWS, MN2_DST_COLS, MN2_IFM, MN2_OFM, MN2_DW_PARALLEL, MN2_IFM_PARALLEL, MN2_OFM_PARALLEL,
		MN2_PW_PARALLEL, MN_DMA_PARALLEL, MN_BATCHES, MN_FP_POSITION, MN_ROUND_POLICY, MN_OVERFLOW_POLICY>(coefficients, src, dst);
}

/***************************************************************************************************************************************************/
void MnLayer3(
	vx_image_data<MnType, MN_DMA_PARALLEL> coefficients[MN3_COEFF_SIZE],
	MnType src[MN_BATCHES * MN3_SRC_ROWS * MN3_SRC_COLS * MN3_IFM],
	vx_image_data<MnType, MN_DMA_PARALLEL> dst[(MN_BATCHES * MN3_OFM) / MN_DMA_PARALLEL]) {
#ifndef __SDSCC__
#pragma HLS interface ap_ctrl_none port=return
#pragma HLS interface axis port=coefficients
#pragma HLS interface axis port=src
#pragma HLS interface axis port=dst
#else
#pragma HLS interface ap_fifo port=coefficients
#pragma HLS interface ap_fifo port=src
#pragma HLS interface ap_fifo port=dst
#endif
	MobileNet3<MnType, MN3_SRC_ROWS, MN3_SRC_COLS, MN3_IFM, MN3_OFM, MN3_IFM_PARALLEL, MN_DMA_PARALLEL, MN_BATCHES,
		MN_FP_POSITION, MN_ROUND_POLICY, MN_OVERFLOW_POLICY>(coefficients, src, dst);
}

/***************************************************************************************************************************************************/
void TestConvPointWise() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	typedef NnTypeBias TypeBias;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 SRC_ROWS = C1_SRC_ROWS;
	const vx_int64 SRC_COLS = C1_SRC_COLS;
	const vx_int64 DST_ROWS = C1_DST_ROWS;
	const vx_int64 DST_COLS = C1_DST_COLS;
	const vx_int64 IFM = C1_IFM;
	const vx_int64 OFM = C1_OFM;
	const vx_int64 IFM_PARALLEL = C1_IFM_PARALLEL;
	const vx_int64 OFM_PARALLEL = C1_OFM_PARALLEL;
	const vx_int64 KERNEL_X = C1_KERNEL_X;
	const vx_int64 KERNEL_Y = C1_KERNEL_Y;
	const vx_int64 BIASES_SIZE = C1_BIASES_SIZE;
	const vx_int64 DMA_PARALLEL = NN_DMA_PARALLEL;

	// Layer parameters (auto)
	typedef c1_src_image_type TypeImageSrc;
	typedef c1_dst_image_type TypeImageDst;
	typedef c1_dst_image_type_bias TypeImageBias;
	typedef c1_weights_image_type1 TypeImageWeights1;
	typedef c1_weights_image_type2 TypeImageWeights2;
	const vx_int64 SRC_SIZE = C1_SRC_SIZE; 
	const vx_int64 DST_SIZE = C1_DST_SIZE; 
	const vx_int64 WEIGHTS_SIZE = C1_WEIGHTS_SIZE;
	const vx_int64 WEIGHTS_PARALLEL = C1_WEIGHTS_PARALLEL;	

	// Constants	
	const vx_int64 KERNEL_SIZE = KERNEL_Y * KERNEL_X;

	// Matrixes
	float *sw_weights1 = new float[WEIGHTS_SIZE];
	float* sw_weights2 = new float[WEIGHTS_SIZE];
	float *sw_biases = new float[BIASES_SIZE];
	float *sw_src = new float[SRC_SIZE];
	float *sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, WEIGHTS_PARALLEL>* hw_weights1 = CreateImage<vx_image_data<TypeData, WEIGHTS_PARALLEL>, WEIGHTS_SIZE, WEIGHTS_PARALLEL>();
	vx_image_data<TypeData, DMA_PARALLEL>* hw_weights2 = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, WEIGHTS_SIZE, DMA_PARALLEL>();
	vx_image_data<TypeBias, OFM_PARALLEL>* hw_biases = CreateImage<vx_image_data<TypeBias, OFM_PARALLEL>, BIASES_SIZE, OFM_PARALLEL>();
	vx_image_data<TypeData, IFM_PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, IFM_PARALLEL>, SRC_SIZE, IFM_PARALLEL>();
	vx_image_data<TypeData, OFM_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, OFM_PARALLEL>, DST_SIZE, OFM_PARALLEL>();

	// Create Inputs, Weights & Biases	
	RandomBiases<TypeBias, FIXED_POINT_POSITION, 1, BIASES_SIZE, OFM_PARALLEL, OFM_PARALLEL>(sw_biases, hw_biases);	
	RandomWeights<TypeData, FIXED_POINT_POSITION, 1, WEIGHTS_PARALLEL, KERNEL_SIZE, IFM, OFM, IFM_PARALLEL, OFM_PARALLEL>(sw_weights1, hw_weights1);
	RandomWeights<TypeData, FIXED_POINT_POSITION, 1, DMA_PARALLEL, KERNEL_SIZE, IFM, OFM, IFM_PARALLEL, OFM_PARALLEL>(sw_weights2, hw_weights2);
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, IFM, BATCHES, IFM_PARALLEL>(sw_src, hw_src);

	// Test SW & HW
	printf("Start Conv Point Wise:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwConvPointWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, OFM, IFM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights1, sw_biases, sw_src, 
			sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwConvPointWise((TypeImageSrc*)hw_src, (TypeImageWeights1*)hw_weights1, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwConvPointWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, OFM, IFM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights1, sw_biases, sw_src, 
		sw_dst);
	HwConvPointWise((TypeImageSrc*)hw_src, (TypeImageWeights1*)hw_weights1, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, OFM, OFM_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, 
		"Conv PW     :");

	// Test SW & HW
	printf("Start Conv Point Wise (dma):\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwConvPointWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, OFM, IFM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights2, sw_biases, sw_src, 
			sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwConvPwDma((TypeImageSrc*)hw_src, (TypeImageWeights2*)hw_weights2, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwConvPointWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, OFM, IFM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights2, sw_biases, sw_src, 
		sw_dst);
	HwConvPwDma((TypeImageSrc*)hw_src, (TypeImageWeights2*)hw_weights2, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, OFM, OFM_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, 
		"Conv PW (dma):");	

	// Free allocated memory
	delete[] sw_weights1;
	delete[] sw_weights2;
	delete[] sw_biases;
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_weights1);
	DestroyImage(hw_weights2);
	DestroyImage(hw_biases);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestConvDepthWise() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	typedef NnTypeBias TypeBias;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 SRC_ROWS = C2_SRC_ROWS;
	const vx_int64 SRC_COLS = C2_SRC_COLS;
	const vx_int64 DST_ROWS = C2_DST_ROWS;
	const vx_int64 DST_COLS = C2_DST_COLS;
	const vx_int64 FM = C2_FM;
	const vx_int64 FM_PARALLEL = C2_FM_PARALLEL;
	const vx_int64 KERNEL_X = C2_KERNEL_X;
	const vx_int64 KERNEL_Y = C2_KERNEL_Y;
	const vx_int64 BIASES_SIZE = C2_BIASES_SIZE;	

	// Layer parameters (auto)
	typedef c2_image_type TypeImage;
	typedef c2_image_type_bias TypeImageBias;
	typedef c2_weights_image_type TypeImageWeights;
	const vx_int64 SRC_SIZE = C2_SRC_SIZE;
	const vx_int64 DST_SIZE = C2_DST_SIZE;
	const vx_int64 WEIGHTS_SIZE = C2_WEIGHTS_SIZE;
	const vx_int64 WEIGHTS_PARALLEL = C2_WEIGHTS_PARALLEL;	

	// Constants	
	const vx_int64 KERNEL_SIZE = KERNEL_Y * KERNEL_X;

	// Matrixes
	float* sw_weights = new float[WEIGHTS_SIZE];
	float* sw_biases = new float[BIASES_SIZE];
	float* sw_src = new float[SRC_SIZE];
	float* sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, WEIGHTS_PARALLEL>* hw_weights = CreateImage<vx_image_data<TypeData, WEIGHTS_PARALLEL>, WEIGHTS_SIZE, WEIGHTS_PARALLEL>();
	vx_image_data<TypeBias, FM_PARALLEL>* hw_biases = CreateImage<vx_image_data<TypeBias, FM_PARALLEL>, BIASES_SIZE, FM_PARALLEL>();
	vx_image_data<TypeData, FM_PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, FM_PARALLEL>, SRC_SIZE, FM_PARALLEL>();
	vx_image_data<TypeData, FM_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, FM_PARALLEL>, DST_SIZE, FM_PARALLEL>();

	// Create Inputs, Weights & Biases
	RandomWeights<TypeData, FIXED_POINT_POSITION, 1, WEIGHTS_PARALLEL, KERNEL_SIZE, 1, FM, 1, FM_PARALLEL>(sw_weights, hw_weights);
	RandomBiases<TypeBias, FIXED_POINT_POSITION, 1, BIASES_SIZE, FM_PARALLEL, FM_PARALLEL>(sw_biases, hw_biases);
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, FM, BATCHES, FM_PARALLEL>(sw_src, hw_src);

	// Test SW & HW
	printf("Start Conv Depth Wise:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwConvDepthWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, FM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights, sw_biases, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwConvDepthWise((TypeImage *)hw_src, (TypeImageWeights *)hw_weights, (TypeImageBias*)hw_biases, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwConvDepthWise<BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, FM, KERNEL_Y, KERNEL_X, BIASES_SIZE>(sw_weights, sw_biases, sw_src, sw_dst);
	HwConvDepthWise((TypeImage*)hw_src, (TypeImageWeights*)hw_weights, (TypeImageBias*)hw_biases, (TypeImage*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, FM, FM_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "Conv DW:");

	// Free allocated memory
	delete[] sw_weights;
	delete[] sw_biases;
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_weights);
	DestroyImage(hw_biases);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestPooling() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;
	const vx_int64 SRC_ROWS = P1_SRC_ROWS;
	const vx_int64 SRC_COLS = P1_SRC_COLS;
	const vx_int64 DST_ROWS = P1_DST_ROWS;
	const vx_int64 DST_COLS = P1_DST_COLS;
	const vx_int64 FM = P1_FM;
	const vx_int64 FM_PARALLEL = P1_FM_PARALLEL;
	const vx_int64 POOLING_Y = P1_KERNEL_Y;
	const vx_int64 POOLING_X = P1_KERNEL_X;
	const vx_int64 PADDING_Y = P1_PADDING_Y;
	const vx_int64 PADDING_X = P1_PADDING_X;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_nn_pooling_type_e POOLING_TYPE = P1_POOLING_TYPE;

	// Layer parameters (auto)
	typedef p1_image_type TypeImage;	
	const vx_int64 SRC_SIZE = P1_SRC_SIZE;
	const vx_int64 DST_SIZE = P1_DST_SIZE;

	// Matrixes
	float* sw_src = new float[SRC_SIZE];
	float* sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, FM_PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, FM_PARALLEL>, SRC_SIZE, FM_PARALLEL>();
	vx_image_data<TypeData, FM_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, FM_PARALLEL>, DST_SIZE, FM_PARALLEL>();

	// Input values
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, FM, BATCHES, FM_PARALLEL>(sw_src, hw_src);

	// Test SW & HW
	printf("Start Pooling:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwPooling<TypeData, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, FM, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X, BATCHES, POOLING_TYPE>(sw_src, 
			sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwPooling((TypeImage*)hw_src, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwPooling<TypeData, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, FM, POOLING_Y, POOLING_X, PADDING_Y, PADDING_X, BATCHES, POOLING_TYPE>(sw_src, 
		sw_dst);
	HwPooling((TypeImage*)hw_src, (TypeImage*)hw_dst);
#endif

	// Compute Diff
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, FM, FM_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "Pooling:");

	// Free memory
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestBatchNorm() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 PIXELS = B1_PIXELS;
	const vx_int64 FM = B1_FM;
	const vx_int64 PARALLEL = B1_PARALLEL;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;
	const vx_int64 BN1_WEIGHT_SIZE = BATCH_NORM1_WEIGHTS_NUM;
	const vx_int64 BN2_WEIGHT_SIZE = BATCH_NORM2_WEIGHTS_NUM;

	// Layer parameters (auto)
	typedef b1_image_type TypeImage;
	typedef b1_weights1_image_type TypeImageWeights1;
	typedef b1_weights2_image_type TypeImageWeights2;
	const vx_int64 TOTAL_ELEMENTS = B1_IMG_SIZE;
	const vx_int64 BN1_WEIGHTS_PARALLEL = B1_WEIGHTS1_PARALLEL;
	const vx_int64 BN2_WEIGHTS_PARALLEL = B1_WEIGHTS2_PARALLEL;

	// Constants
	const vx_int64 BN1_WEIGHTS_TOTAL = BN1_WEIGHT_SIZE * FM;
	const vx_int64 BN2_WEIGHTS_TOTAL = BN2_WEIGHT_SIZE * FM;

	// Matrixes
	float* sw1_weights = new float[BN1_WEIGHTS_TOTAL];
	float* sw2_weights = new float[BN2_WEIGHTS_TOTAL];
	float* sw_src = new float[TOTAL_ELEMENTS];
	float* sw_dst = new float[TOTAL_ELEMENTS];
	vx_image_data<TypeData, BN1_WEIGHTS_PARALLEL>* hw1_weights = CreateImage<vx_image_data<TypeData, BN1_WEIGHTS_PARALLEL>, BN1_WEIGHTS_TOTAL, 
		BN1_WEIGHTS_PARALLEL>();
	vx_image_data<TypeData, BN2_WEIGHTS_PARALLEL>* hw2_weights = CreateImage<vx_image_data<TypeData, BN2_WEIGHTS_PARALLEL>, BN2_WEIGHTS_TOTAL, 
		BN2_WEIGHTS_PARALLEL>();
	vx_image_data<TypeData, PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();
	vx_image_data<TypeData, PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();

	// Input values
	RandomInputs<TypeData, FIXED_POINT_POSITION, 1, PIXELS, FM, BATCHES, PARALLEL>(sw_src, hw_src);
	RandomBnWeights<TypeData, FIXED_POINT_POSITION, FM, PARALLEL, BN1_WEIGHTS_PARALLEL, BN1_WEIGHT_SIZE>(sw1_weights, hw1_weights);
	RandomBnWeights<TypeData, FIXED_POINT_POSITION, FM, PARALLEL, BN2_WEIGHTS_PARALLEL, BN2_WEIGHT_SIZE>(sw2_weights, hw2_weights);

	// Test SW & HW
	printf("Start Batch Norm1:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwBatchNorm1<BATCHES, PIXELS, FM, BN1_WEIGHT_SIZE>(sw1_weights, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwBatchNorm1((TypeImageWeights1*)hw1_weights, (TypeImage *)hw_src, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwBatchNorm1<BATCHES, PIXELS, FM, BN1_WEIGHT_SIZE>(sw1_weights, sw_src, sw_dst);
	HwBatchNorm1((TypeImageWeights1*)hw1_weights, (TypeImage*)hw_src, (TypeImage*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, 1, PIXELS, FM, PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "BatchNorm1:");

	// Test SW & HW
	printf("Start Batch Norm2:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwBatchNorm2<BATCHES, PIXELS, FM, BN2_WEIGHT_SIZE>(sw2_weights, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwBatchNorm2((TypeImageWeights2*)hw2_weights, (TypeImage*)hw_src, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwBatchNorm2<BATCHES, PIXELS, FM, BN2_WEIGHT_SIZE>(sw2_weights, sw_src, sw_dst);
	HwBatchNorm2((TypeImageWeights2*)hw2_weights, (TypeImage*)hw_src, (TypeImage*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, 1, PIXELS, FM, PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "BatchNorm2:");

	// Free memory
	delete[] sw1_weights;
	delete[] sw2_weights;
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw1_weights);
	DestroyImage(hw2_weights);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestActivationLayer() {
	
	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 PIXELS = A1_ELEMENTS;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;
	const vx_int64 PARALLEL = A1_PARALLEL;
	const vx_nn_activation_function_e ACTIVATION_FUNCTION = A1_ACTIVATION_FUNCTION;
	const float param_a = (float)A1_PARAM_A;
	const float param_b = (float)A1_PARAM_B;

	// Layer parameters (auto)
	typedef a1_image_type TypeImage;
	const vx_int64 TOTAL_ELEMENTS = A1_IMG_SIZE;

	// Constants	
	const float MULT = 1.0f / (float)(1 << FIXED_POINT_POSITION);
	const float param_a_ = param_a * MULT;
	const float param_b_ = param_b * MULT;

	// Matrixes
	float* sw_src = new float[TOTAL_ELEMENTS];
	float* sw_dst = new float[TOTAL_ELEMENTS];
	vx_image_data<TypeData, PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();
	vx_image_data<TypeData, PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();

	// Input values
	RandomInputs<TypeData, FIXED_POINT_POSITION, 1, PIXELS, 1, BATCHES, PARALLEL>(sw_src, hw_src);

	// Test SW & HW
	printf("Start Activation:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwActivation<TOTAL_ELEMENTS, ACTIVATION_FUNCTION>(param_a_, param_b_, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwActivation((TypeImage*)hw_src, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwActivation<TOTAL_ELEMENTS, ACTIVATION_FUNCTION>(param_a_, param_b_, sw_src, sw_dst);
	HwActivation((TypeImage*)hw_src, (TypeImage*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, 1, PIXELS, 1, PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "Activation:");

	// Free memory
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestFullyConnected() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;
	typedef NnTypeBias TypeBias;
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 IFM = F1_IFM;
	const vx_int64 OFM = F1_OFM;
	const vx_int64 BIASES_SIZE = F1_BIAS_SIZE;
	const vx_int64 WEIGHTS_SIZE = F1_WEIGHT_SIZE;
	const vx_int64 PARALLEL = F1_PARALLEL;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;

	// Layer parameters (auto)
	typedef f1_src_image_type TypeImageSrc;
	typedef f1_dst_image_type TypeImageDst;
	typedef f1_dst_image_type_bias TypeImageBias;
	const vx_int64 SRC_SIZE = F1_SRC_SIZE;
	const vx_int64 DST_SIZE = F1_DST_SIZE;	

	// Matrixes
	float* sw_src = new float[SRC_SIZE];
	float* sw_weights = new float[WEIGHTS_SIZE];
	float* sw_biases = new float[BIASES_SIZE];
	float* sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, PARALLEL>, SRC_SIZE, PARALLEL>();
	vx_image_data<TypeData, PARALLEL>* hw_weights = CreateImage<vx_image_data<TypeData, PARALLEL>, WEIGHTS_SIZE, PARALLEL>();
	vx_image_data<TypeBias, 1>* hw_biases = CreateImage<vx_image_data<TypeBias, 1>, BIASES_SIZE, 1>();
	vx_image_data<TypeData, 1>* hw_dst = CreateImage<vx_image_data<TypeData, 1>, DST_SIZE, 1>();

	// Input values
	RandomInputs<TypeData, FIXED_POINT_POSITION, 1, 1, IFM, BATCHES, PARALLEL>(sw_src, hw_src);
	RandomBiases<TypeBias, FIXED_POINT_POSITION, 1, BIASES_SIZE, 1, 1>(sw_biases, hw_biases);
	RandomWeights<TypeData, FIXED_POINT_POSITION, 1, PARALLEL, 1, IFM, OFM, PARALLEL, 1>(sw_weights, hw_weights);

	// Test SW & HW
	printf("Start Fully Connected:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwFullyConnected<BATCHES, IFM, OFM, BIASES_SIZE, WEIGHTS_SIZE>(sw_src, sw_weights, sw_biases, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwFullyConnected((TypeImageSrc*)hw_src, (TypeImageSrc*)hw_weights, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwFullyConnected<BATCHES, IFM, OFM, BIASES_SIZE, WEIGHTS_SIZE>(sw_src, sw_weights, sw_biases, sw_dst);
	HwFullyConnected((TypeImageSrc*)hw_src, (TypeImageSrc*)hw_weights, (TypeImageBias*)hw_biases, (TypeImageDst*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, 1, 1, OFM, 1>(sw_dst, hw_dst, sw_time, hw_time, "Fully:   ");

	// Free memory
	delete[] sw_src;
	delete[] sw_weights;
	delete[] sw_biases;
	delete[] sw_dst;
	DestroyImage(hw_src);
	DestroyImage(hw_weights);
	DestroyImage(hw_biases);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestSoftmaxLayer() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef NnType TypeData;	
	const vx_int64 BATCHES = NN_BATCHES;
	const vx_int64 IFM = S1_FM;
	const vx_int64 PARALLEL = S1_PARALLEL;
	const vx_int64 FIXED_POINT_POSITION = NN_FP_POSITION;

	// Layer parameters (auto)
	typedef s1_image_type TypeImage;
	const vx_int64 TOTAL_ELEMENTS = S1_IMG_SIZE;

	// Matrixes
	float* sw_src = new float[TOTAL_ELEMENTS];
	float* sw_dst = new float[TOTAL_ELEMENTS];
	vx_image_data<TypeData, PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();
	vx_image_data<TypeData, PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, PARALLEL>, TOTAL_ELEMENTS, PARALLEL>();

	// Input values
	RandomInputs<TypeData, FIXED_POINT_POSITION, 1, 1, IFM, BATCHES, PARALLEL>(sw_src, hw_src);

	// Test SW & HW
	printf("Start Softmax:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwSoftmaxLayer<BATCHES, IFM>(sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		HwSoftmax((TypeImage*)hw_src, (TypeImage*)hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwSoftmaxLayer<BATCHES, IFM>(sw_src, sw_dst);
	HwSoftmax((TypeImage*)hw_src, (TypeImage*)hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, 1, 1, IFM, PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, "Softmax:");

	// Free memory
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestMnLayer1() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef MnType TypeData;
	const vx_int64 SRC_ROWS = MN1_SRC_ROWS;
	const vx_int64 SRC_COLS = MN1_SRC_COLS;
	const vx_int64 DST_ROWS = MN1_DST_ROWS;
	const vx_int64 DST_COLS = MN1_DST_COLS;
	const vx_int64 IFM = MN1_IFM;
	const vx_int64 OFM = MN1_OFM;
	const vx_int64 IFM_PARALLEL = MN1_IFM_PARALLEL;
	const vx_int64 OFM_PARALLEL = MN1_OFM_PARALLEL;
	const vx_int64 PW_PARALLEL = MN1_PW_PARALLEL;
	const vx_int64 DMA_PARALLEL = MN_DMA_PARALLEL;
	const vx_int64 BATCHES = MN_BATCHES;
	const vx_int64 FIXED_POINT_POSITION = MN_FP_POSITION;

	// Constant parameters
	const vx_int64 KERNEL = 3;
	const vx_int64 BN_WEIGHTS_NUM = BATCH_NORM2_WEIGHTS_NUM;
	const TypeData ACTIVATION_PARAM_A = 0;
	const TypeData ACTIVATION_PARAM_B = 0;
	const vx_nn_activation_function_e ACTIVATION_FUNCTION = VX_NN_ACTIVATION_RELU;

	// Constants (aligned buffers are marked by "__" at the beginning)
	const vx_int64 SRC_SIZE = BATCHES * SRC_ROWS * SRC_COLS * IFM;
	const vx_int64 DST_SIZE = BATCHES * DST_ROWS * DST_COLS * OFM;
	const vx_int64 CONV_SIZE = KERNEL * KERNEL;
	const vx_int64 WEIGHTS_SIZE = OFM * IFM * KERNEL * KERNEL;
	const vx_int64 BIASES_SIZE = OFM;
	const vx_int64 BN_WEIGHTS_SIZE = OFM * BN_WEIGHTS_NUM;
	const vx_int64 __WEIGHTS_SIZE = ALIGN(WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __BIASES_SIZE = ALIGN(BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __BN_WEIGHTS_SIZE = ALIGN(BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __COEFF_SIZE = __WEIGHTS_SIZE + __BIASES_SIZE + __BN_WEIGHTS_SIZE;

	// Pointers to combine coefficients
	const vx_int64 coeff_ptr1 = __WEIGHTS_SIZE / DMA_PARALLEL;
	const vx_int64 coeff_ptr2 = __BIASES_SIZE / DMA_PARALLEL + coeff_ptr1;

	// Matrixes
	float* sw_weights = new float[WEIGHTS_SIZE];
	float* sw_biases = new float[BIASES_SIZE];
	float* sw_bn_weights = new float[BN_WEIGHTS_SIZE];
	float* sw_src = new float[SRC_SIZE];	
	float* sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, DMA_PARALLEL>* hw_coefficients = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, __COEFF_SIZE, DMA_PARALLEL>();
	vx_image_data<TypeData, DMA_PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, SRC_SIZE, DMA_PARALLEL>();
	vx_image_data<TypeData, PW_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, PW_PARALLEL>, DST_SIZE, PW_PARALLEL>();

	// Create Inputs, Weights & Biases	
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, IFM, BATCHES, DMA_PARALLEL>(sw_src, hw_src);
	RandomWeightsBiases<TypeData, FIXED_POINT_POSITION, 1, BIASES_SIZE, DMA_PARALLEL, 1, DMA_PARALLEL, CONV_SIZE, IFM, OFM, IFM_PARALLEL, 
		OFM_PARALLEL>(sw_weights, sw_biases, hw_coefficients, (hw_coefficients + coeff_ptr1));
	RandomBnWeights<TypeData, FIXED_POINT_POSITION, OFM, PW_PARALLEL, DMA_PARALLEL, BN_WEIGHTS_NUM>(sw_bn_weights, (hw_coefficients + coeff_ptr2));

	// Test SW & HW
	printf("Test MnLayer1:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwMnLayer1<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, KERNEL, BN_WEIGHTS_NUM, ACTIVATION_FUNCTION>(
			ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, sw_weights, sw_biases, sw_bn_weights, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		MnLayer1(hw_coefficients, hw_src, hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwMnLayer1<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, KERNEL, BN_WEIGHTS_NUM, ACTIVATION_FUNCTION>(
		ACTIVATION_PARAM_A, ACTIVATION_PARAM_B, sw_weights, sw_biases, sw_bn_weights, sw_src, sw_dst);
	MnLayer1(hw_coefficients, hw_src, hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, OFM, PW_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, 
		"MobileNet 1:");

	// Free allocated memory
	delete[] sw_weights;
	delete[] sw_biases;
	delete[] sw_bn_weights;
	delete[] sw_src;	
	delete[] sw_dst;
	DestroyImage(hw_coefficients);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestMnLayer2() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];

	// Layer parameters
	typedef MnType TypeData;
	const vx_int64 SRC_ROWS = MN2_SRC_ROWS;
	const vx_int64 SRC_COLS = MN2_SRC_COLS;
	const vx_int64 DST_ROWS = MN2_DST_ROWS;
	const vx_int64 DST_COLS = MN2_DST_COLS;
	const vx_int64 IFM = MN2_IFM;
	const vx_int64 OFM = MN2_OFM;
	const vx_int64 DW_PARALLEL = MN2_DW_PARALLEL;
	const vx_int64 IFM_PARALLEL = MN2_IFM_PARALLEL;
	const vx_int64 OFM_PARALLEL = MN2_OFM_PARALLEL;
	const vx_int64 PW_PARALLEL = MN2_PW_PARALLEL;
	const vx_int64 DMA_PARALLEL = MN_DMA_PARALLEL;
	const vx_int64 BATCHES = MN_BATCHES;
	const vx_int64 FIXED_POINT_POSITION = MN_FP_POSITION;

	// Constant parameters
	const vx_int64 DW_KERNEL = 3;
	const vx_int64 PW_KERNEL = 1;
	const vx_int64 BN_WEIGHTS_NUM = BATCH_NORM2_WEIGHTS_NUM;
	const TypeData ACT_PARAM_A = 0;
	const TypeData ACT_PARAM_B = 0;
	const vx_nn_activation_function_e ACTIVATION_FUNCTION = VX_NN_ACTIVATION_RELU;

	// Constants (aligned buffers are marked by "__" at the beginning)
	const vx_int64 DST_PIXELS = DST_ROWS * DST_COLS;
	const vx_int64 SRC_SIZE = BATCHES * SRC_ROWS * SRC_COLS * IFM;
	const vx_int64 DST_SIZE = BATCHES * DST_PIXELS * OFM;
	const vx_int64 DW_CONV_SIZE = DW_KERNEL * DW_KERNEL;
	const vx_int64 PW_CONV_SIZE = PW_KERNEL * PW_KERNEL;
	const vx_int64 DW_WEIGHTS_SIZE = IFM * DW_CONV_SIZE;
	const vx_int64 DW_BIASES_SIZE = IFM;
	const vx_int64 DW_BN_WEIGHTS_SIZE = BN_WEIGHTS_NUM * IFM;
	const vx_int64 __DW_WEIGHTS_SIZE = ALIGN(DW_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __DW_BIASES_SIZE = ALIGN(DW_BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __DW_BN_WEIGHTS_SIZE = ALIGN(DW_BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __DW_COEFF_SIZE = __DW_WEIGHTS_SIZE + __DW_BIASES_SIZE + __DW_BN_WEIGHTS_SIZE;
	const vx_int64 PW_WEIGHTS_SIZE = OFM * IFM * PW_CONV_SIZE;
	const vx_int64 PW_BIASES_SIZE = OFM;
	const vx_int64 PW_BN_WEIGHTS_SIZE = BN_WEIGHTS_NUM * OFM;
	const vx_int64 __PW_WEIGHTS_SIZE = ALIGN(PW_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __PW_BIASES_SIZE = ALIGN(PW_BIASES_SIZE, DMA_PARALLEL);
	const vx_int64 __PW_BN_WEIGHTS_SIZE = ALIGN(PW_BN_WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __PW_COEFF_SIZE = __PW_WEIGHTS_SIZE + __PW_BIASES_SIZE + __PW_BN_WEIGHTS_SIZE;
	const vx_int64 __COEFF_SIZE = __DW_COEFF_SIZE + __PW_COEFF_SIZE;

	// Compile-time checks
	STATIC_ASSERT(((IFM % DW_PARALLEL) == 0), MISSMATCH_IFM_DW_PARALLEL);
	STATIC_ASSERT(((IFM % IFM_PARALLEL) == 0), MISSMATCH_IFM_IFM_PARALLEL);
	STATIC_ASSERT(((OFM % OFM_PARALLEL) == 0), MISSMATCH_OFM_OFM_PARALLEL);
	STATIC_ASSERT(((OFM % PW_PARALLEL) == 0), MISSMATCH_OFM_PW_PARALLEL);

	// Pointers to combine coefficients
	vx_int64 coeff_off[6] = { 0 };
	coeff_off[1] = __DW_WEIGHTS_SIZE / DMA_PARALLEL + coeff_off[0];
	coeff_off[2] = __DW_BIASES_SIZE / DMA_PARALLEL + coeff_off[1];
	coeff_off[3] = __DW_BN_WEIGHTS_SIZE / DMA_PARALLEL + coeff_off[2];
	coeff_off[4] = __PW_WEIGHTS_SIZE / DMA_PARALLEL + coeff_off[3];
	coeff_off[5] = __PW_BIASES_SIZE / DMA_PARALLEL + coeff_off[4];	

	// Matrixes	
	float* dw_sw_weights = new float[DW_WEIGHTS_SIZE];
	float* dw_sw_biases = new float[DW_BIASES_SIZE];
	float* dw_sw_bn_weights = new float[DW_BN_WEIGHTS_SIZE];
	float* pw_sw_weights = new float[PW_WEIGHTS_SIZE];
	float* pw_sw_biases = new float[PW_BIASES_SIZE];
	float* pw_sw_bn_weights = new float[PW_BN_WEIGHTS_SIZE];
	float* sw_src = new float[SRC_SIZE];
	float* sw_dst = new float[DST_SIZE];
	vx_image_data<TypeData, DMA_PARALLEL>* hw_coeffs = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, __COEFF_SIZE, DMA_PARALLEL>();
	vx_image_data<TypeData, DW_PARALLEL>* hw_src = CreateImage<vx_image_data<TypeData, DW_PARALLEL>, SRC_SIZE, DW_PARALLEL>();
	vx_image_data<TypeData, PW_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, PW_PARALLEL>, DST_SIZE, PW_PARALLEL>();

	// Create Inputs, Weights & Biases	
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, IFM, BATCHES, DW_PARALLEL>(sw_src, hw_src);
	RandomWeightsBiases<TypeData, FIXED_POINT_POSITION, 1, DW_BIASES_SIZE, DMA_PARALLEL, 1, DMA_PARALLEL, DW_CONV_SIZE, 1, IFM, 1, DW_PARALLEL>
		(dw_sw_weights, dw_sw_biases, (hw_coeffs + coeff_off[0]), (hw_coeffs + coeff_off[1]));
	RandomWeightsBiases<TypeData, FIXED_POINT_POSITION, 1, PW_BIASES_SIZE, DMA_PARALLEL, 1, DMA_PARALLEL, PW_CONV_SIZE, IFM, OFM, IFM_PARALLEL, 
		OFM_PARALLEL>(pw_sw_weights, pw_sw_biases, (hw_coeffs + coeff_off[3]), (hw_coeffs + coeff_off[4]));
	RandomBnWeights<TypeData, FIXED_POINT_POSITION, IFM, DW_PARALLEL, DMA_PARALLEL, BN_WEIGHTS_NUM>(dw_sw_bn_weights, (hw_coeffs + coeff_off[2]));
	RandomBnWeights<TypeData, FIXED_POINT_POSITION, OFM, PW_PARALLEL, DMA_PARALLEL, BN_WEIGHTS_NUM>(pw_sw_bn_weights, (hw_coeffs + coeff_off[5]));

	// Test SW & HW
	printf("Test MnLayer2:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwMnLayer2<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, DW_KERNEL, PW_KERNEL, BN_WEIGHTS_NUM, ACTIVATION_FUNCTION>
			(ACT_PARAM_A, ACT_PARAM_B, dw_sw_weights, dw_sw_biases, dw_sw_bn_weights, pw_sw_weights, pw_sw_biases, pw_sw_bn_weights, sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		MnLayer2(hw_coeffs, hw_src, hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwMnLayer2<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, DW_KERNEL, PW_KERNEL, BN_WEIGHTS_NUM, ACTIVATION_FUNCTION>
		(ACT_PARAM_A, ACT_PARAM_B, dw_sw_weights, dw_sw_biases, dw_sw_bn_weights, pw_sw_weights, pw_sw_biases, pw_sw_bn_weights, sw_src, sw_dst);
	MnLayer2(hw_coeffs, hw_src, hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, OFM, PW_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, 
		"MobileNet 2:");

	// Free allocated memory
	delete[] dw_sw_weights;
	delete[] dw_sw_biases;
	delete[] dw_sw_bn_weights;
	delete[] pw_sw_weights;
	delete[] pw_sw_biases;
	delete[] pw_sw_bn_weights;
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_coeffs);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}

/***************************************************************************************************************************************************/
void TestMnLayer3() {

	// Variables for timing
#ifdef __EVAL_TIME__
#ifdef __XILINX__
	XTime t1;
#else
	std::chrono::high_resolution_clock::time_point t1;
#endif
#endif
	double sw_time[ITERATIONS], hw_time[ITERATIONS];


	// Layer parameters
	typedef MnType TypeData;
	const vx_int64 SRC_ROWS = MN3_SRC_ROWS;
	const vx_int64 SRC_COLS = MN3_SRC_COLS;
	const vx_int64 DST_ROWS = MN3_DST_ROWS;
	const vx_int64 DST_COLS = MN3_DST_COLS;
	const vx_int64 IFM = MN3_IFM;
	const vx_int64 OFM = MN3_OFM;
	const vx_int64 IFM_PARALLEL = MN3_IFM_PARALLEL;
	const vx_int64 OFM_PARALLEL = MN3_OFM_PARALLEL;
	const vx_int64 DMA_PARALLEL = MN_DMA_PARALLEL;
	const vx_int64 BATCHES = MN_BATCHES;
	const vx_int64 FIXED_POINT_POSITION = MN_FP_POSITION;

	// Constant parameters
	const vx_int64 PADDING_Y = 0;
	const vx_int64 PADDING_X = 0;
	const vx_nn_pooling_type_e POOLING_TYPE = VX_NN_POOLING_AVG;

	// Constants
	const vx_int64 INPUT_STREAM = BATCHES * SRC_ROWS * SRC_COLS * IFM;
	const vx_int64 OUTPUT_STREAM = BATCHES * OFM * DST_ROWS * DST_COLS;
	const vx_int64 CONV_SIZE = 1;
	const vx_int64 WEIGHTS_SIZE = OFM * IFM;
	const vx_int64 BIASES_SIZE = OFM;
	const vx_int64 __WEIGHTS_SIZE = ALIGN(WEIGHTS_SIZE, DMA_PARALLEL);
	const vx_int64 __BIASES_SIZE = ALIGN(BIASES_SIZE, DMA_PARALLEL);

	// Pointers to combine coefficients
	const vx_int64 coeff_ptr1 = __WEIGHTS_SIZE / DMA_PARALLEL;

	// Matrixes
	float* sw_weights = new float[WEIGHTS_SIZE];
	float* sw_biases = new float[BIASES_SIZE];
	float* sw_src = new float[INPUT_STREAM];
	float* sw_dst = new float[OUTPUT_STREAM];
	vx_image_data<TypeData, DMA_PARALLEL>* hw_coefficients = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, (__WEIGHTS_SIZE + __BIASES_SIZE), 
		DMA_PARALLEL>();
	vx_image_data<TypeData, 1>* hw_src = CreateImage<vx_image_data<TypeData, 1>, INPUT_STREAM, 1>();
	vx_image_data<TypeData, DMA_PARALLEL>* hw_dst = CreateImage<vx_image_data<TypeData, DMA_PARALLEL>, OUTPUT_STREAM, DMA_PARALLEL>();

	// Create Inputs, Weights & Biases	
	RandomInputs<TypeData, FIXED_POINT_POSITION, SRC_ROWS, SRC_COLS, IFM, BATCHES, 1>(sw_src, hw_src);
	RandomWeightsBiases<TypeData, FIXED_POINT_POSITION, 1, BIASES_SIZE, DMA_PARALLEL, 1, DMA_PARALLEL, CONV_SIZE, IFM, OFM, IFM_PARALLEL, 
		OFM_PARALLEL>(sw_weights, sw_biases, hw_coefficients, (hw_coefficients + coeff_ptr1));

	// Test SW & HW
	printf("Test MnLayer3:\n");
#ifdef __EVAL_TIME__
	for (vx_int64 i = 0; i < ITERATIONS; ++i) {
		StartTime(t1);
		SwMobileNet3<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, PADDING_Y, PADDING_X, POOLING_TYPE>(sw_weights, sw_biases, 
			sw_src, sw_dst);
		sw_time[i] = EndTime(t1, "");
		StartTime(t1);
		MnLayer3(hw_coefficients, (TypeData*)hw_src, hw_dst);
		hw_time[i] = EndTime(t1, "");
	}
#else
	SwMobileNet3<TypeData, BATCHES, SRC_ROWS, SRC_COLS, DST_ROWS, DST_COLS, IFM, OFM, PADDING_Y, PADDING_X, POOLING_TYPE>(sw_weights, sw_biases, 
		sw_src, sw_dst);
	MnLayer3(hw_coefficients, (TypeData*)hw_src, hw_dst);
#endif

	// Evaluate Time & Error Rate
	Evaluate<TypeData, ITERATIONS, FIXED_POINT_POSITION, BATCHES, DST_ROWS, DST_COLS, OFM, DMA_PARALLEL>(sw_dst, hw_dst, sw_time, hw_time, 
		"MobileNet 3:");

	// Free allocated memory
	delete[] sw_weights;
	delete[] sw_biases;
	delete[] sw_src;
	delete[] sw_dst;
	DestroyImage(hw_coefficients);
	DestroyImage(hw_src);
	DestroyImage(hw_dst);
}


/***************************************************************************************************************************************************/
// Main function (uncomments functions that should be tested)
/***************************************************************************************************************************************************/
int main(void) {

	// Test convolutional NN Layers
	TestConvPointWise();
	TestConvDepthWise();
	TestPooling();

	// Test other NN Layers
	TestBatchNorm();
	TestActivationLayer();	
	TestFullyConnected();
	TestSoftmaxLayer();

	// Test Mobile Net
	TestMnLayer1();
	TestMnLayer2();	
	TestMnLayer3();

	return 0;
}
