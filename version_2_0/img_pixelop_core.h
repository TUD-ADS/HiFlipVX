/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_pixelop_core.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These are all core functions needed for the pixel operation functions (Do not call functions from here)
*/

#ifndef SRC_IMG_PIXELOP_CORE_H_
#define SRC_IMG_PIXELOP_CORE_H_

#include "vx_helper.h"

/***************************************************************************************************************************************************/
/* Compute Magnitude */
/***************************************************************************************************************************************************/

/*! \brief Compute Magnitude of one pixel (signed) 
    \details Tested for 8, 16 and 32 bit | exact OpenVX results
@param ScalarInt       Data type of the image pixels
@param ScalarUint      Data type of the image pixels
@param CompInt         Data type for the computation
@param CompUint        Data type for the computation
@param CONV_POLICY     Conversion policy (wrap & saturate)
@param ROUND_POLICY    Rounding policy (to zero & nearest even)
@param input1          1. input pixel
@param input2          2. input pixel
@return                Result
*/
template<typename ScalarInt, typename ScalarUint, typename CompInt, typename CompUint, vx_convert_policy_e CONV_POLICY, 
	vx_round_policy_e ROUND_POLICY>
ScalarInt MagnitudeSigned(ScalarInt input1, ScalarInt input2) {
#pragma HLS INLINE

	const vx_uint32 COMPUTE_TYPE = sizeof(CompUint);

	// Get max value for overflow
	const ScalarUint MAX_VAL = static_cast<ScalarUint>(ComputeMax<ScalarInt>());

	// Compute magnitude for signed input/output
	CompInt A = static_cast<CompInt>(input1);
	CompInt B = static_cast<CompInt>(input2);
	CompUint AA = static_cast<CompUint>(A*A);
	CompUint BB = static_cast<CompUint>(B*B);
	CompUint C = AA + BB;

	// Square Root
	ScalarUint D = 0;
	if (COMPUTE_TYPE == 8) {
		if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
			D = static_cast<ScalarUint>(sqrt(static_cast<double>(C)) + 0.5);
		else
			D = static_cast<ScalarUint>(sqrt(static_cast<double>(C)));
	} else {
		D = SqrtLester<ScalarUint, CompUint, true, ROUND_POLICY, sizeof(ScalarUint) * 8>(C);
	}

	// Overflow (Saturation) - due to conversion back to signed
	ScalarUint E = 0;
	if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
		E = MIN(D, MAX_VAL);
	else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
		E = D;

	// Return result
	return static_cast<ScalarInt>(E);
}

/*! \brief Compute Magnitude of one pixel (unsigned) 
    \details Tested for 8, 16 and 32 bit | exact OpenVX results if MAX_VALUE and CHECK_MAX are true \n
@param ScalarUint      Data type of the image pixels
@param CompUint        Data type for the computation
@param CONV_POLICY     Conversion policy (wrap & saturate)
@param ROUND_POLICY    Rounding policy (to zero & nearest even)
@param input1          1. input pixel
@param input2          2. input pixel
@return                Result
*/
template<typename ScalarUint, typename CompUint, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY>
ScalarUint MagnitudeUnsigned(ScalarUint input1, ScalarUint input2) {
#pragma HLS INLINE

	const vx_uint32 COMPUTE_TYPE = sizeof(CompUint);

	// Get max value for overflow
	const ScalarUint MAX_VAL = static_cast<ScalarUint>(ComputeMax<ScalarUint>());

	// Compute magnitude for unsigned input/output
	CompUint A = static_cast<CompUint>(input1);
	CompUint B = static_cast<CompUint>(input2);
	CompUint AA = A*A;
	CompUint BB = B*B;
	CompUint C = AA + BB;

	// Square Root
	ScalarUint D = 0;
	if (COMPUTE_TYPE == 8) {
		if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
			D = static_cast<ScalarUint>(sqrt(static_cast<double>(C)) + 0.5);
		else
			D = static_cast<ScalarUint>(sqrt(static_cast<double>(C)));
	} else {
		D = SqrtLester<ScalarUint, CompUint, true, ROUND_POLICY, sizeof(ScalarUint) * 8>(C);
	}

	// Overflow (Saturation) - if C alrady has overflow
	ScalarUint E = 0;
	if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
		E = (C >= AA && C >= BB) ? (D) : (MAX_VAL);
	else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
		E = D;

	// Return result
	return static_cast<ScalarUint>(E);
}

/***************************************************************************************************************************************************/
/* Compute Pixel-wise Multiplication */
/***************************************************************************************************************************************************/

/** @brief Performs element-wise multiplication between two images and a scalar value.
@param ScalarType      Data type of the image pixels
@param CompType        Data type for the computation
@param IS_SIGNED       Is true if it is a signed value
@param CONV_POLICY     Conversion policy (wrap & saturate)
@param ROUND_POLICY    Rounding policy (to zero & nearest even)
@param SCALE           A positive fixed-point (16-bit fraction) number multiplied to each product before overflow handling (between 0.0 and 1.0)
@param input1          1. input pixel
@param input2          2. input pixel
@return                Result
*/
template<typename ScalarType, typename CompType, bool IS_SIGNED, vx_type_e SCALAR_TYPE, vx_convert_policy_e CONV_POLICY, 
	vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
ScalarType Multiply(ScalarType input1, ScalarType input2) {
#pragma HLS INLINE

	// Check function parameters/types
	STATIC_ASSERT(SCALE <= 0x10000, scale_value_must_be_between_1_and_0_It_is_fixed_point_and_has_16_bit_fraction);

	// Get (Max and Min)
	const CompType MIN_VAL = static_cast<CompType>(ComputeMin<ScalarType>());
	const CompType MAX_VAL = static_cast<CompType>(ComputeMax<ScalarType>());

	// Check if scaling is by a power of two
	const bool IS_POWER_TWO = (SCALE & (SCALE - 1)) == 0;

	// Compute the amount of bits data needs to be shiftet, if it is power of two
	vx_uint32 SHIFT = 0;
	for (vx_uint32 i = 0; i < sizeof(SCALE) * 8; i++) {
#pragma HLS unroll
		if ((SCALE & static_cast<vx_uint32>(1 << i)) != 0)
			SHIFT = 16 - i;
	}

	// Constants	
	const CompType MULT_INT = static_cast<CompType>(SCALE);
	const CompType ROUND_POWER_2 = (SHIFT > 0) ? (static_cast<CompType>(1) << (SHIFT - 1)) : (static_cast<CompType>(0));
	const CompType ROUND_NORMAL = static_cast<CompType>(1) << (15);
	const vx_uint16 SHIFT_POWER_2 = static_cast<vx_uint16>(SHIFT);
	const vx_uint16 SHIFT_NORMAL = 16;
	
	// Store result
	CompType D = 0, result = 0;

	// Compute Pixelwise Multiplication
	if (MULT_INT > 0) {

		// Compute Multiplicaion
		CompType A = static_cast<CompType>(input1);
		CompType B = static_cast<CompType>(input2);
		CompType C = A*B;

		// Multiply and round to nearest integer
		if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {

			// Shift bits, if scale it is a power of two
			if (IS_POWER_TWO) {
				D = (C + ROUND_POWER_2);				
				D = ShiftArithmeticRight<CompType, IS_SIGNED>(D, SHIFT_POWER_2);

			// Multiply by scale, if it is not a power of two
			} else {
				if ((SCALAR_TYPE == VX_TYPE_UINT32) || (SCALAR_TYPE == VX_TYPE_INT32)) {
					D = ShiftArithmeticRight<CompType, IS_SIGNED>(C, SHIFT_NORMAL);
					D = (D * MULT_INT + ROUND_NORMAL);
				} else {
					D = (C * MULT_INT + ROUND_NORMAL);
					D = ShiftArithmeticRight<CompType, IS_SIGNED>(D, SHIFT_NORMAL);
				}
			}		

		// Multiply and truncate the least significant values
		} else if (ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) {

			// Shift bits, if scale it is a power of two
			if (IS_POWER_TWO) {
				D = ShiftArithmeticRight<CompType, IS_SIGNED>(C, SHIFT_POWER_2);

			// Multiply by scale, if it is not a power of two
			} else {
				if ((SCALAR_TYPE == VX_TYPE_UINT32) || (SCALAR_TYPE == VX_TYPE_INT32)) {
					D = ShiftArithmeticRight<CompType, IS_SIGNED>(C, SHIFT_NORMAL);
					D = (D * MULT_INT);
				} else {
					D = (C * MULT_INT);
					D = ShiftArithmeticRight<CompType, IS_SIGNED>(D, SHIFT_NORMAL);
				}
			}
		}
	
		// Saturate the result 
		if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
			if (IS_SIGNED) {
				result = SaturateMaxMin<CompType>(D, MAX_VAL, MIN_VAL);
			} else {
				result = MIN(D, MAX_VAL);
			}
		} else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP) {
			result = D;
		}
	}

	// Return result
	return static_cast<ScalarType>(result);
}


/***************************************************************************************************************************************************/
/* Compute Phase */
/***************************************************************************************************************************************************/

/** @brief Computes the orientation of a pixel.
@param DataType            Data type of the image pixels
@param CompType            Data type for the computation
@param PHASE_QUANTIZATION  2^PHASE_QUANTIZATION different possibilities for the output
@param input1              1. input pixel
@param input2              2. input pixel
@return                    Result
*/
template<typename DataType, typename CompType, vx_uint8 PHASE_QUANTIZATION>
CompType Phase(CompType input1, CompType input2) {
#pragma HLS INLINE
	// Constants
	const vx_int32 ACCURACY = 7 + PHASE_QUANTIZATION;
	const vx_int32 SHIFT = (vx_int32)1 << ACCURACY;
	const vx_int32 TURN_POS = SHIFT / (vx_int32)(2 << (vx_int32)PHASE_QUANTIZATION);
	const vx_int32 TURN_NEG = SHIFT - TURN_POS;
	const vx_int32 QUANTIZATION = ACCURACY - (vx_int32)PHASE_QUANTIZATION;
	const vx_int32 ROUNDING = (vx_int32)1 << (QUANTIZATION - (vx_int32)1);

	// Computation
	CompType a1 = atan2Cordic<CompType, 8>((CompType)input1, (CompType)input2); // Compute angle
	CompType a2 = a1 >> (16 - ACCURACY) - ROUNDING;                    // Reduce Precision and round
	CompType a3 = (a2 < 0) ? (a2 + SHIFT) : (a2);                      // Convert negative degree to positiv	
	CompType a4 = (a3 > TURN_NEG) ? (a3 - TURN_NEG) : (a3 + TURN_POS); // Setting 0 degree to east
	CompType a5 = (a4 >= SHIFT) ? (0) : (a4);                          // Setting 360 degree to 0
	return (a5) >> QUANTIZATION;                                       // Do quantization
}


/***************************************************************************************************************************************************/
/* Arithmetic Operations */
/***************************************************************************************************************************************************/

/** @brief Computes an arithmetic operation
@param ScalarType      Data type of the image pixels
@param OPERATION_TYPE  Operation type (e.g. arithmetic add, sub, mul)
@param CONV_POLICY     Conversion policy (wrap & saturate)
@param ROUND_POLICY    Rounding policy (to zero & nearest even)
@param SCALE           A positive fixed-point (16-bit fraction) number multiplied to each product before overflow handling (between 0.0 and 1.0)
@param input1          1. input pixel
@param input2          2. input pixel
@return                result
*/
template<typename ScalarType, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, 
	vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_VALUE, ScalarType THRESH_UPPER, ScalarType THRESH_LOWER, vx_uint32 ALPHA, 
	vx_uint8 PHASE_QUANTIZATION>
ScalarType Arithmetic(ScalarType input1, ScalarType input2) {
#pragma HLS INLINE

	// Compile-time-checks
	STATIC_ASSERT((PHASE_QUANTIZATION <= 8) && (PHASE_QUANTIZATION > 0), THE_QUANTIZATION_OF_THE_PHASE_FUNCTION_NEEDS_TO_BE_BETWEEN_1_AND_8);

	// Constants
	const bool IS_SIGNED = std::numeric_limits<ScalarType>::is_signed;
	const vx_type_e SCALAR_TYPE = GET_TYPE(ScalarType);
	
	// Convert for computation
	const vx_int64 a = static_cast<vx_int64>(input1);
	const vx_int64 b = static_cast<vx_int64>(input2);
	
	// Get (Max and Min)
	const vx_int64 MIN_VAL = static_cast<vx_int64>(ComputeMin<ScalarType>());
	const vx_int64 MAX_VAL = static_cast<vx_int64>(ComputeMax<ScalarType>());

	// Result
	vx_int64 result = 0;	

	// ABSOLUTE_DIFFERENCE
	if (OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
		const vx_int64 c = abs(a - b);

		if (IS_SIGNED) {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)				
				result = MIN(c , MAX_VAL);
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		} else {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
				result = c;
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		}

	// ARITHMETIC_ADDITION
	} else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
		const vx_int64 c = a + b;

		if (IS_SIGNED) {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
				result = SaturateMaxMin<vx_int64>(c, MAX_VAL, MIN_VAL);
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		} else {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
				result = MIN(c, MAX_VAL);
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		}

	// ARITHMETIC_SUBTRACTION
	} else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
		const vx_int64 c = a - b;

		if (IS_SIGNED) {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
				result = SaturateMaxMin<vx_int64>(c, MAX_VAL, MIN_VAL);
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		} else {
			if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE)
				result = MAX(c, MIN_VAL);
			else if (CONV_POLICY == VX_CONVERT_POLICY_WRAP)
				result = c;
		}

	// MAGNITUDE
	} else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {		
		if (SCALAR_TYPE == VX_TYPE_INT8)
			result = MagnitudeSigned<ScalarType, vx_uint8, vx_int16, vx_uint16, CONV_POLICY, ROUND_POLICY>(input1, input2);
		else if (SCALAR_TYPE == VX_TYPE_INT16)
			result = MagnitudeSigned<ScalarType, vx_uint16, vx_int32, vx_uint32, CONV_POLICY, ROUND_POLICY>(input1, input2);
		else if (SCALAR_TYPE == VX_TYPE_INT32)
			result = MagnitudeSigned<ScalarType, vx_uint32, vx_int64, vx_uint64, CONV_POLICY, ROUND_POLICY>(input1, input2);
		else if (SCALAR_TYPE == VX_TYPE_UINT8)
			result = MagnitudeUnsigned<ScalarType, vx_uint16, CONV_POLICY, ROUND_POLICY>(input1, input2);
		else if (SCALAR_TYPE == VX_TYPE_UINT16)
			result = MagnitudeUnsigned<ScalarType, vx_uint32, CONV_POLICY, ROUND_POLICY>(input1, input2);
		else if (SCALAR_TYPE == VX_TYPE_UINT32)
			result = MagnitudeUnsigned<ScalarType, vx_uint64, CONV_POLICY, ROUND_POLICY>(input1, input2);
	
	// MULTIPLY
	} else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
		if (IS_SIGNED) {
			result = Multiply<ScalarType, vx_int64, IS_SIGNED, SCALAR_TYPE, CONV_POLICY, ROUND_POLICY, SCALE>(input1, input2);
		} else {
			result = Multiply<ScalarType, vx_uint64, IS_SIGNED, SCALAR_TYPE, CONV_POLICY, ROUND_POLICY, SCALE>(input1, input2);
		}

	// THRESHOLD
	} else if (OPERATION_TYPE == HIFLIPVX::THRESHOLD) {
		if (IS_SIGNED == true) {
			if (THRESH_TYPE == VX_THRESHOLD_TYPE_BINARY)
				result = (input1 > THRESH_VALUE) ? (-1) : (0);
			else if (THRESH_TYPE == VX_THRESHOLD_TYPE_RANGE)
				result = ((input1 > THRESH_UPPER) || (input1 < THRESH_LOWER)) ? (0) : (-1);
		} else {
			if (THRESH_TYPE == VX_THRESHOLD_TYPE_BINARY)
				result = (input1 > THRESH_VALUE) ? (MAX_VAL) : (0);
			else if (THRESH_TYPE == VX_THRESHOLD_TYPE_RANGE)
				result = ((input1 > THRESH_UPPER) || (input1 < THRESH_LOWER)) ? (0) : (MAX_VAL);
		}

	// WEIGHTED AVERAGE
	} else if (OPERATION_TYPE == HIFLIPVX::WEIGHTED_AVERAGE) {
		vx_int64 alpha = ALPHA;
		vx_int64 one = (1 << 16);
		vx_int64 half = (1 << 15);
		vx_int64 src1 = input1;
		vx_int64 src2 = input2;
		if (ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) {
			result = ((one - alpha) * src2 + alpha * src1);
			result = ShiftArithmeticRight<vx_int64, IS_SIGNED>(result, 16);
		} else if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
			result = ((one - alpha) * src2 + alpha * src1 + half);
			result = ShiftArithmeticRight<vx_int64, IS_SIGNED>(result, 16);
		}

	// PHASE
	} else if (OPERATION_TYPE == HIFLIPVX::PHASE) {
		if ((SCALAR_TYPE == VX_TYPE_UINT8) || (SCALAR_TYPE == VX_TYPE_INT8)) {			
			result = Phase<ScalarType, vx_int16, PHASE_QUANTIZATION>((vx_int16)input1, (vx_int16)input2);
		} else if((SCALAR_TYPE == VX_TYPE_UINT16) || (SCALAR_TYPE == VX_TYPE_INT16)) {
			result = Phase<ScalarType, vx_int32, PHASE_QUANTIZATION>((vx_int32)input1, (vx_int32)input2);
		} else if ((SCALAR_TYPE == VX_TYPE_UINT32) || (SCALAR_TYPE == VX_TYPE_INT32)) {
			result = Phase<ScalarType, vx_int64, PHASE_QUANTIZATION>((vx_int64)input1, (vx_int64)input2);
		}
	}

	return static_cast<ScalarType>(result);
}

/***************************************************************************************************************************************************/
/* Pixelwise Operations */
/***************************************************************************************************************************************************/

/** @brief Performs a pixelwise operation on an image or between two images
@param ScalarType      Data type of the image pixels
@param OPERATION_TYPE  Operation type (e.g. bitwise or, and, xor, not | arithmetic add, sub, mul)
@param CONV_POLICY     Conversion policy (wrap & saturate)
@param ROUND_POLICY    Rounding policy (to zero & nearest even)
@param SCALE           A positive fixed-point (16-bit fraction) number multiplied to each product before overflow handling (between 0.0 and 1.0)
@param src1            1. input pixel
@param src2            2. input pixel
@param dst             result
*/
template<typename ScalarType, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, 
	vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_VALUE, ScalarType THRESH_UPPER, ScalarType THRESH_LOWER, vx_uint32 ALPHA, 
	vx_uint8 PHASE_QUANTIZATION>
void PixelwiseComputeSameType(ScalarType src1, ScalarType src2, ScalarType &dst) {
#pragma HLS INLINE

	// Constants
	const bool is_arithmetic_operation = ((OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) || (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) ||
		(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) || (OPERATION_TYPE == HIFLIPVX::MULTIPLY) || (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) ||
		(OPERATION_TYPE == HIFLIPVX::THRESHOLD) || (OPERATION_TYPE == HIFLIPVX::WEIGHTED_AVERAGE) || (OPERATION_TYPE == HIFLIPVX::PHASE));

	// Copy a data object to another.
	if (OPERATION_TYPE == HIFLIPVX::COPY_DATA)
		dst = src1;

	// Performs a bitwise NOT operation on a input images.
	else if (OPERATION_TYPE == HIFLIPVX::BITWISE_NOT)
		dst = ~(src1);

	// Performs a bitwise AND operation between two unsigned images.
	else if (OPERATION_TYPE == HIFLIPVX::BITWISE_AND)
		dst = src1 & src2;

	// Performs a bitwise INCLUSIVE OR operation between two unsigned images.
	else if (OPERATION_TYPE == HIFLIPVX::BITWISE_OR)
		dst = src1 | src2;

	// Performs a bitwise EXCLUSIVE OR (XOR) operation between two unsigned images.
	else if (OPERATION_TYPE == HIFLIPVX::BITWISE_XOR)
		dst = src1 ^ src2;

	// Implements a pixel-wise maximum kernel.
	else if (OPERATION_TYPE == HIFLIPVX::MAX)
		dst = MAX(src1, src2);

	// Implements a pixel - wise minimum kernel.
	else if (OPERATION_TYPE == HIFLIPVX::MIN)
		dst = MIN(src1, src2);

	// Computes arithmetic operations
	else if (is_arithmetic_operation == true)
		dst = Arithmetic<ScalarType, OPERATION_TYPE, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE, THRESH_VALUE, THRESH_UPPER, THRESH_LOWER, ALPHA, 
			PHASE_QUANTIZATION>(src1, src2);

	// Operation not found
	else
		dst = 0;
}

/** @brief  Checks if the input parameters are valid
@param ScalarType      Data type of the image pixels
@param VEC_SIZE        Amount of pixels computed in parallel
@param IMG_PIXEL       Amount of pixels in the image
@return                Data beats (IMG_PIXEL / VEC_SIZE)
*/
template<typename ScalarType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL>
vx_uint32 PixelwiseCheckSameType() {
#pragma HLS INLINE

	// Constants
	const vx_type_e SCALAR_TYPE = GET_TYPE(ScalarType);
	const bool SCALAR_TYPE_CORRECT =
		(SCALAR_TYPE == VX_TYPE_UINT8) || (SCALAR_TYPE == VX_TYPE_UINT16) || (SCALAR_TYPE == VX_TYPE_UINT32) ||
		(SCALAR_TYPE == VX_TYPE_INT8) || (SCALAR_TYPE == VX_TYPE_INT16) || (SCALAR_TYPE == VX_TYPE_INT32);

	// Check function parameters/types
	STATIC_ASSERT(SCALAR_TYPE_CORRECT, data_type_error__uint_and_int_8_to_32_bit_allowed);
	STATIC_ASSERT((IMG_PIXEL % VEC_SIZE) == 0, image_pixels_are_not_multiple_of_vector_size);
	STATIC_ASSERT((VEC_SIZE == 1) || (VEC_SIZE == 2) || (VEC_SIZE == 4) || (VEC_SIZE == 8), _vec_size_musst_be_1_2_4_8__use_vx_image_for_2_4_8);

	// Return vector pixel
	return IMG_PIXEL / static_cast<vx_uint32>(VEC_SIZE);
}

/***************************************************************************************************************************************************/
/* Pixelwise Same Data Type */
/***************************************************************************************************************************************************/

/** @brief Performs a pixelwise operation on an image or between two images (Top function)
@param ScalarType         Data type of the image pixels
@param VEC_SIZE           Amount of pixels computed in parallel
@param IMG_PIXEL          Amount of pixels in the image
@param OPERATION_TYPE     Operation type (e.g. bitwise or, and, xor, not | arithmetic add, sub, mul)
@param CONV_POLICY        Conversion policy (wrap & saturate)
@param ROUND_POLICY       Rounding policy (to zero & nearest even)
@param SCALE              A positive fixed-point (16-bit fraction) number multiplied to each product before overflow handling (between 0.0 and 1.0)
@param THRESH_TYPE        The type of thresholding (VX_THRESHOLD_TYPE_BINARY, VX_THRESHOLD_TYPE_RANGE)
@param THRESH_VALUE       The threshold value if threshold type is VX_THRESHOLD_TYPE_BINARY
@param THRESH_UPPER       The upper threshold value if threshold type is VX_THRESHOLD_TYPE_RANGE
@param THRESH_LOWER       The lower threshold value if threshold type is VX_THRESHOLD_TYPE_RANGE
@param ALPHA              The weighted value in fixed-point between 0 and 1 using 16-bit fraction
@param PHASE_QUANTIZATION The quantization for the different amount of orientations
@param input1             1. input image
@param input2             2. input image
@param output             Output image
*/
template<typename ScalarType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE, vx_convert_policy_e CONV_POLICY, 
	vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_VALUE, ScalarType THRESH_UPPER, 
	ScalarType THRESH_LOWER, vx_uint32 ALPHA, vx_uint8 PHASE_QUANTIZATION>
void PixelwiseSameType(
	vx_image_data<ScalarType, VEC_SIZE> input1[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<ScalarType, VEC_SIZE> input2[IMG_PIXEL / VEC_SIZE], 
	vx_image_data<ScalarType, VEC_SIZE> output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE

	// Check if input correct
	const vx_uint32 vector_pixels = PixelwiseCheckSameType<ScalarType, VEC_SIZE, IMG_PIXEL>();

	// Computes pixelwise operations (pipelined)
	for (vx_uint32 i = 0; i < vector_pixels; i++) {
#pragma HLS PIPELINE II=1

		// Variables
		vx_image_data<ScalarType, VEC_SIZE> src1, src2, dst;

		// Read input
		src1 = input1[i];
		if ((OPERATION_TYPE != HIFLIPVX::BITWISE_NOT) || (OPERATION_TYPE == HIFLIPVX::COPY_DATA) || (OPERATION_TYPE == HIFLIPVX::THRESHOLD))
			src2 = input2[i];

		// Computes a vector of pixelwise operations in parallel
		for (vx_uint16 j = 0; j < VEC_SIZE; j++) {
#pragma HLS unroll
			PixelwiseComputeSameType<ScalarType, OPERATION_TYPE, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE, THRESH_VALUE, THRESH_UPPER, 
				THRESH_LOWER, ALPHA, PHASE_QUANTIZATION>(src1.pixel[j], src2.pixel[j], dst.pixel[j]);
		}

		// Set user(SOF) & last(EOF)				
		GenerateDmaSignal<ScalarType, VEC_SIZE>((i == 0), (i == vector_pixels - 1), dst);

		// Write output
		output[i] = dst;
	}
}
template<typename ScalarType, vx_uint8 VEC_SIZE, vx_uint32 IMG_PIXEL, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE, vx_convert_policy_e CONV_POLICY, 
	vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE, vx_threshold_type_e THRESH_TYPE, ScalarType THRESH_VALUE, ScalarType THRESH_UPPER, 
	ScalarType THRESH_LOWER, vx_uint32 ALPHA, vx_uint8 PHASE_QUANTIZATION>
void PixelwiseSameType(
	ScalarType input1[IMG_PIXEL / VEC_SIZE],
	ScalarType input2[IMG_PIXEL / VEC_SIZE],
	ScalarType output[IMG_PIXEL / VEC_SIZE]) {
#pragma HLS INLINE

	// Check if input correct
	const vx_uint32 vector_pixels = PixelwiseCheckSameType<ScalarType, VEC_SIZE, IMG_PIXEL>();

	// Computes pixelwise operations (pipelined)
	for (vx_uint32 i = 0; i < vector_pixels; i++) {
#pragma HLS PIPELINE II=1

		// Variables
		ScalarType src1 = 0, src2 = 0, dst = 0;

		// Read input
		src1 = input1[i];
		if ((OPERATION_TYPE != HIFLIPVX::BITWISE_NOT) || (OPERATION_TYPE == HIFLIPVX::COPY_DATA) || (OPERATION_TYPE == HIFLIPVX::THRESHOLD))
			src2 = input2[i];

		// Computes a vector of pixelwise operations in parallel
		PixelwiseComputeSameType<ScalarType, OPERATION_TYPE, CONV_POLICY, ROUND_POLICY, SCALE, THRESH_TYPE, THRESH_VALUE, THRESH_UPPER, THRESH_LOWER, 
			ALPHA, PHASE_QUANTIZATION>(src1, src2, dst);

		// Write output
		output[i] = dst;
	}
}

#endif /* SRC_IMG_PIXELOP_CORE_H_ */
