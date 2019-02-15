/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_pixelop_help.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.3
* @brief Description:\n
*  These functions test the pixel operation functions
*/

#ifndef SRC_IMG_PIXELOP_TEST_H_
#define SRC_IMG_PIXELOP_TEST_H_

#include "img_pixelop_base.h"

/*********************************************************************************************************************/
/* SW Test Functions */
/*********************************************************************************************************************/

/*! \brief Check if pixelwise function was correct */
template<typename Intype, typename AcType, vx_uint32 PIXELS>
void TestPixel(Intype *inputA, AcType *inputB, AcType *outputA, AcType *outputB, std::string &name) {
	vx_int64 absDifMax = 0;
	for (vx_uint32 i = 0; i < PIXELS; i++) {
		vx_int64 absDif = abs(static_cast<vx_int64>(outputA[i]) - static_cast<vx_int64>(outputB[i]));
		absDifMax = max(absDifMax, absDif);
		if (absDif > 10000) {
			vx_int64 A = static_cast<vx_int64>(inputA[i]);
			vx_int64 B = static_cast<vx_int64>(inputB[i]);
			vx_int64 C = static_cast<vx_int64>(outputA[i]);
			vx_int64 D = static_cast<vx_int64>(outputB[i]);			
			printf("%4d: (%10lld, %10lld) - (%10lld, %10lld) - (%10lld), \n", i, A, B, C, D, absDif);
			getchar();
		}
	}
	//if(absDifMax > 0)
		cout << "    " << name.c_str() << "Max. absolut error: " << absDifMax << endl;
}

/*! \brief Test pixelwise function */
template<typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, HIFLIPVX::PixelwiseOperationA OPERATION_TYPE, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
void TestPixelwiseA(ScalarType *inputA, ScalarType *inputB, ScalarType *outputA, ScalarType *outputB, std::string &name) {

	// Get (Max and Min)
	const double sat_min = static_cast<double>(ComputeMin<ScalarType>());
	const double sat_max = static_cast<double>(ComputeMax<ScalarType>());

	// Get (Max and Min)
	const vx_int64 MIN_VAL = static_cast<vx_int64>(ComputeMin<ScalarType>());
	const vx_int64 MAX_VAL = static_cast<vx_int64>(ComputeMax<ScalarType>());

	// Compute Software
	for (vx_uint32 i = 0; i<PIXELS; i++) {
		vx_int64 A = static_cast<vx_int64>(inputA[i]);
		vx_int64 B = static_cast<vx_int64>(inputB[i]);
		double C = 0.0;
		vx_int64 D = 0, E = 0;

		// Computation including rounding
		if (OPERATION_TYPE == HIFLIPVX::COPY_DATA) {
			D = A;
		} else if (OPERATION_TYPE == HIFLIPVX::MAX) {
			D = max(A, B);
		} else if (OPERATION_TYPE == HIFLIPVX::MIN) {
			D = min(A, B);
		} else if(OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
			D = llabs(A - B);
		} else if(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
			D = A + B;
		} else if (OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
			D = A - B;
		} else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {
			vx_uint64 A1 = static_cast<vx_uint64>(abs(A));
			vx_uint64 B1 = static_cast<vx_uint64>(abs(B));
			double A2 = static_cast<double>(A1*A1);
			double B2 = static_cast<double>(B1*B1);
			C = sqrt(A2 + B2);
			if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
				C += 0.5;
			D = static_cast<vx_int64>(C);
		} else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
			C = static_cast<double>(A)*static_cast<double>(B) * (static_cast<double>(SCALE) / pow(2, 16));
			if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN)
				C += 0.5;
			D = static_cast<vx_int64>(C);
		}			

		// Conversion Policy
		if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
			if (D > MAX_VAL)
				E = MAX_VAL;
			else if (D < MIN_VAL)
				E = MIN_VAL;
			else
				E = D;
		} else { 
			E = D;		
		}
		outputA[i] = static_cast<ScalarType>(E);
	}
	
	// Compute Hardware
	if (VEC_SIZE == 1) {
		if (OPERATION_TYPE == HIFLIPVX::COPY_DATA) {
			ImgCopy<ScalarType, VEC_SIZE, PIXELS>((ScalarType *)inputA, (ScalarType *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MAX) {
			ImgMax<ScalarType, VEC_SIZE, PIXELS>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::MIN) {
			ImgMin<ScalarType, VEC_SIZE, PIXELS>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
			ImgAbsDiff<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
			ImgAdd<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
			ImgSubtract<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {
			ImgMagnitude<ScalarType, VEC_SIZE, PIXELS, ROUND_POLICY>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
			ImgMultiply<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY, ROUND_POLICY, SCALE>((ScalarType *)inputA, (ScalarType *)inputB, (ScalarType *)outputB);
		}
	} else {
		if (OPERATION_TYPE == HIFLIPVX::COPY_DATA) {
			ImgCopy<ScalarType, VEC_SIZE, PIXELS>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MAX) {
			ImgMax<ScalarType, VEC_SIZE, PIXELS>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::MIN) {
			ImgMin<ScalarType, VEC_SIZE, PIXELS>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ABSOLUTE_DIFFERENCE) {
			ImgAbsDiff<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_ADDITION) {
			ImgAdd<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if(OPERATION_TYPE == HIFLIPVX::ARITHMETIC_SUBTRACTION) {
			ImgSubtract<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MAGNITUDE) {
			ImgMagnitude<ScalarType, VEC_SIZE, PIXELS, ROUND_POLICY>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		} else if (OPERATION_TYPE == HIFLIPVX::MULTIPLY) {
			ImgMultiply<ScalarType, VEC_SIZE, PIXELS, CONV_POLICY, ROUND_POLICY, SCALE>((vx_image<ScalarType, VEC_SIZE> *)inputA, (vx_image<ScalarType, VEC_SIZE> *)inputB, (vx_image<ScalarType, VEC_SIZE> *)outputB);
		}
	}
	// Test result
	TestPixel<ScalarType, ScalarType, PIXELS>(inputA, inputB, outputA, outputB, name);
}

/*! \brief Test all pixelwise functions dependent on policy */
template<typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, vx_convert_policy_e CONV_POLICY, vx_round_policy_e ROUND_POLICY, vx_uint32 SCALE>
void TestPolicyA(ScalarType *inputA, ScalarType *inputB, ScalarType *outputA, ScalarType *outputB, std::string &name) {

	// Write name of upper test case
	cout << "  " << name.c_str() << endl;

	// Function test scenarios
	string name0 = "Copy: ";
	string name1 = "Max:  ";
	string name2 = "Min:  ";
	string name3 = "Abs:  ";
	string name4 = "Add:  ";
	string name5 = "Sub:  ";
	string name6 = "Mag:  ";
	string name7 = "Mul:  ";

	// Test functions for possible policies
	if ((ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) && (CONV_POLICY == VX_CONVERT_POLICY_WRAP)) {
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::COPY_DATA, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name0);
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MAX, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name1);
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MIN, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name2);
	}
	if (ROUND_POLICY == VX_ROUND_POLICY_TO_ZERO) {
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ABSOLUTE_DIFFERENCE, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name3);
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ARITHMETIC_ADDITION, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name4);
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::ARITHMETIC_SUBTRACTION, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name5);
	}
	if (CONV_POLICY == VX_CONVERT_POLICY_SATURATE) {
		TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MAGNITUDE, CONV_POLICY, ROUND_POLICY, 0>(inputA, inputB, outputA, outputB, name6);
	}
	TestPixelwiseA<ScalarType, PIXELS, VEC_SIZE, HIFLIPVX::MULTIPLY, CONV_POLICY, ROUND_POLICY, SCALE>(inputA, inputB, outputA, outputB, name7);
}

/*! \brief Test all pixelwise functions for all possible policies for 1 data type */
template<typename ScalarType, vx_uint32 PIXELS, vx_uint8 VEC_SIZE, vx_uint32 SCALE>
void TestDataTypeA(std::string &name) {

	// Test images
	ScalarType *inputA = new ScalarType[PIXELS];
	ScalarType *inputB = new ScalarType[PIXELS];
	ScalarType *outputA = new ScalarType[PIXELS];
	ScalarType *outputB = new ScalarType[PIXELS];

	// Write test image data
	const vx_int64 MIN_VAL = static_cast<vx_int64>(ComputeMin<ScalarType>());
	const vx_int64 MAX_VAL = static_cast<vx_int64>(ComputeMax<ScalarType>());
	const double max_rand = static_cast<double>(RAND_MAX);
	const double min_val = static_cast<double>(MIN_VAL);
	const double max_val = static_cast<double>(MAX_VAL);
	double A, B, C, D;
	vx_int64 E, F;
	vx_int64 maximun = MAX_VAL;
	vx_int64 minimum = MIN_VAL;

	for (vx_uint32 i = 0; i < PIXELS; i++) {
		A = static_cast<double>(rand());
		B = A / max_rand;
		C = B * (max_val - min_val);
		D = C + min_val;
		E = static_cast<vx_int64>(D + 0.5);
		F = max(min(E, MAX_VAL), MIN_VAL);
		inputA[i] = static_cast<ScalarType>(F);
		maximun = max(F, maximun);
		minimum = min(F, minimum);
		A = static_cast<double>(rand());
		B = A / max_rand;
		C = B * (max_val - min_val);
		D = C + min_val;
		E = static_cast<vx_int64>(D + 0.5);
		F = max(min(E, MAX_VAL), MIN_VAL);
		inputB[i] = static_cast<ScalarType>(F);
		maximun = max(F, maximun);
		minimum = min(F, minimum);
	}	

	// Write name of upper test case
	cout << "" << name.c_str();
	cout << " | input data range: [" << minimum << ".." << maximun << "]"<< endl;

	// Policy test scenarios
	string name1 = "CONVERT_POLICY_WRAP and ROUND_POLICY_TO_ZERO:             ";
	string name2 = "CONVERT_POLICY_WRAP and ROUND_POLICY_TO_NEAREST_EVEN:     ";
	string name3 = "CONVERT_POLICY_SATURATE and ROUND_POLICY_TO_ZERO:         ";
	string name4 = "CONVERT_POLICY_SATURATE and ROUND_POLICY_TO_NEAREST_EVEN: ";

	// Test all policies for a data type
	TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_ZERO, SCALE>(inputA, inputB, outputA, outputB, name1);
	TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_WRAP, VX_ROUND_POLICY_TO_NEAREST_EVEN, SCALE>(inputA, inputB, outputA, outputB, name2);
	TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_SATURATE, VX_ROUND_POLICY_TO_ZERO, SCALE>(inputA, inputB, outputA, outputB, name3);
	TestPolicyA<ScalarType, PIXELS, VEC_SIZE, VX_CONVERT_POLICY_SATURATE, VX_ROUND_POLICY_TO_NEAREST_EVEN, SCALE>(inputA, inputB, outputA, outputB, name4);

	// Delete memory
	delete[] inputA;
	delete[] inputB;
	delete[] outputA;
	delete[] outputB;

	// Newline
	cout << endl;
}

/*! \brief Calls all Software Test Functions */
void SwTestPixelopMain(void) {

	// Image Size
	const vx_uint32 COLS = 256;
	const vx_uint32 ROWS = 256;
	const vx_uint32 PIXELS = COLS*ROWS;
	const vx_uint8 VEC_SIZE = 1;
	const vx_uint32 SCALE = static_cast<vx_uint32>(256);
	const vx_uint8  SHIFT = static_cast<vx_uint32>(8); // 0 .. 15
	const vx_uint32 ALPHA = static_cast<vx_uint32>(1000); // 0 .. 65536

#define THRESHOLD_VALUE  100
#define THRESHOLD_UPPER  150
#define THRESHOLD_LOWER  20

	cout << "Resolution:   " << COLS << " x " << ROWS << endl;
	cout << "Vector Size:  " << static_cast<vx_uint16>(VEC_SIZE) << endl;
	cout << "Scale (mult): " << SCALE << endl << endl;

	string name01 = "UNSIGNED INTEGER 8-Bit:  ";
	string name02 = "UNSIGNED INTEGER 16-Bit: ";
	string name03 = "UNSIGNED INTEGER 32-Bit: ";
	string name04 = "SIGNED INTEGER 8-Bit:    ";
	string name05 = "SIGNED INTEGER 16-Bit:   ";
	string name06 = "SIGNED INTEGER 32-Bit:   ";

	TestDataTypeA<vx_uint8, PIXELS, VEC_SIZE, SCALE>(name01);
	TestDataTypeA<vx_uint16, PIXELS, VEC_SIZE, SCALE>(name02);
	TestDataTypeA<vx_uint32, PIXELS, VEC_SIZE, SCALE>(name03);
	TestDataTypeA<vx_int8, PIXELS, VEC_SIZE, SCALE>(name04);
	TestDataTypeA<vx_int16, PIXELS, VEC_SIZE, SCALE>(name05);
	TestDataTypeA<vx_int32, PIXELS, VEC_SIZE, SCALE>(name06);
}

#endif /* SRC_PIXELOP_TEST_H_ */
