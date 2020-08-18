/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_filter_test.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 2.0
* @brief Description:\n
*  These functions have been implemented to test the functionality and accuracy of the filter functions
*/

#ifndef SRC_IMG_FILTER_TEST_H_
#define SRC_IMG_FILTER_TEST_H_

#include "img_filter_base.h"
#include <iostream>

/***************************************************************************************************************************************************/
/* Test Functions Headers */
/***************************************************************************************************************************************************/

template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwBox(Type * input, Type * output);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwConvolve(Type *input, Type *output, const Type imageKernel[KERN_SIZE][KERN_SIZE]);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwDilate(Type *input, Type *output);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwErode(Type *input, Type *output);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwGaussian(Type * input, Type * output);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwMedian(Type *input, Type *output);
template<typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_border_e BORDER_TYPE>
void SwScharr3x3(InType *input, OutType *outputX, OutType *outputY);
template<typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwSobel(InType *input, OutType *output_x, OutType *output_y);
template<typename Type, const vx_int32 WIDTH, const vx_int32 HEIGHT, const vx_int32 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwNonMaximaSuppression(Type* src, Type* dst);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_border_e BORDER_TYPE>
void SwSegmentTestDetector(Type* src, Type* dst);
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE, Type THRESH_LOWER, 
	Type THRESH_UPPER>
void SwHysteresis(Type* src, Type* dst);

template<typename TypeUint, typename TypeInt, vx_uint8 VEC_NUM, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD, vx_uint16 FILTER_SIZE,
	vx_border_e BORDER_TYPE, TypeUint THRESH_LOWER, TypeUint THRESH_UPPER>
void SwFilters(TypeUint convolve_kernel[FILTER_SIZE][FILTER_SIZE], TypeUint* input, TypeUint* output_sw, TypeUint* output_hw, TypeInt* output_sw_x,
	TypeInt* output_sw_y, TypeInt* output_hw_x, TypeInt* output_hw_y);


/***************************************************************************************************************************************************/
/** @brief SW: Computes the maximum absolute error between two images
*/
template <typename Type, const vx_uint32 IMG_COLS, const vx_uint32 IMG_ROWS, const vx_uint32 BORDER_RADIUS>
void CheckError(Type *output_sw, Type *output_hw) {

	float absDifMax = 0;

	for (vx_uint32 y = BORDER_RADIUS; y < IMG_ROWS - BORDER_RADIUS; y++) {
		for (vx_uint32 x = BORDER_RADIUS; x < IMG_COLS - BORDER_RADIUS; x++) {
			vx_uint32 ptr = y * IMG_COLS + x;
			float A = static_cast<float>(output_sw[ptr]);
			float B = static_cast<float>(output_hw[ptr]);
			float absDif = ABS(A - B);
			absDifMax = MAX(absDifMax, absDif);

			if (absDif > 4) {
				printf("%4d%4d:\t %f\t%f\t%f\n", y, x, A, B, absDif);
				//getchar();
			}
		}
	}
	printf("Max. absolut error: %0.2f\n", absDifMax);
}

/***************************************************************************************************************************************************/
/* SW Test Main Function */
/***************************************************************************************************************************************************/

/*! \brief Test all filter functions */
void TestFilterMain(void) {

	/******** Constants to test the functions ********************************************************************/

	// Test parameters
	const vx_uint16 COLS_FHD = 640;
	const vx_uint16 ROWS_FHD = 480;
	const vx_uint16 FILTER_SIZE = 5; // 3, 5, 7, 9, 11
	const vx_uint8 VEC_NUM = 2;
	typedef vx_uint8 imgUintT;
	typedef vx_int8  imgIntT;

	const imgUintT THRESH_LOWER = 20;
	const imgUintT THRESH_UPPER = 40;

	//
	const vx_uint32 PIXELS_FHD = COLS_FHD*ROWS_FHD;

	// Convolution kernel for the HwConvolve filter 
	imgUintT convolve_kernel[FILTER_SIZE][FILTER_SIZE];	
	for (vx_uint16 i = 0; i < FILTER_SIZE; i++) {
		for (vx_uint16 j = 0; j < FILTER_SIZE; j++) {
			convolve_kernel[i][j] = static_cast<imgUintT>(rand() % 256);
		}
	}

	/******** Allocate memory ***********************************************************************/

	imgUintT *input = new imgUintT[PIXELS_FHD];
	imgUintT *output_sw = new imgUintT[PIXELS_FHD];
	imgUintT *output_hw = new imgUintT[PIXELS_FHD];
	imgIntT *output_sw_x = new imgIntT[PIXELS_FHD];
	imgIntT *output_sw_y = new imgIntT[PIXELS_FHD];
	imgIntT *output_hw_x = new imgIntT[PIXELS_FHD];
	imgIntT *output_hw_y = new imgIntT[PIXELS_FHD];

	/******** Read or create image ******************************************************************/

	for (vx_uint32 i = 0; i < PIXELS_FHD; i++)
		input[i] = rand() % 256;

	/******** Test accelerated functions ************************************************************/
#if !defined(__LAST__) && !defined(__LAST__)
	printf("___________________________________________________________\n");
	printf("Border Type: VX_BORDER_UNDEFINED:\n");
	SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_UNDEFINED, THRESH_LOWER, THRESH_UPPER>(convolve_kernel, input, 
		output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);

	printf("___________________________________________________________\n");
	printf("Border Type: VX_BORDER_CONSTANT:\n");
	SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_CONSTANT, THRESH_LOWER, THRESH_UPPER>(convolve_kernel, input, 
		output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);

	printf("___________________________________________________________\n");
	printf("Border Type: VX_BORDER_REPLICATE:\n");
	SwFilters<imgUintT, imgIntT, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, VX_BORDER_REPLICATE, THRESH_LOWER, THRESH_UPPER>(convolve_kernel, input, 
		output_sw, output_hw, output_sw_x, output_sw_y, output_hw_x, output_hw_y);
#endif

	/******** Free memory ***************************************************************************/

	// Free Memory
	delete[] input;
	delete[] output_sw;
	delete[] output_hw;
	delete[] output_sw_x;
	delete[] output_sw_y;
	delete[] output_hw_x;
	delete[] output_hw_y;
}

/***************************************************************************************************************************************************/
/* SW Test Functions */
/***************************************************************************************************************************************************/

/*! \brief Test all filter functions */
template<typename TypeUint, typename TypeInt, vx_uint8 VEC_NUM, vx_uint16 COLS_FHD, vx_uint16 ROWS_FHD, vx_uint16 FILTER_SIZE, 
	vx_border_e BORDER_TYPE, TypeUint THRESH_LOWER, TypeUint THRESH_UPPER>
void SwFilters(TypeUint convolve_kernel[FILTER_SIZE][FILTER_SIZE], TypeUint *input, TypeUint *output_sw, TypeUint *output_hw, TypeInt *output_sw_x, 
	TypeInt *output_sw_y, TypeInt *output_hw_x, TypeInt *output_hw_y) {

	const vx_uint32 BORDER_RADIUS = (BORDER_TYPE == VX_BORDER_UNDEFINED) ? (FILTER_SIZE / 2) : (0);
	const vx_uint32 BORDER_RADIUS_FAST = (BORDER_TYPE == VX_BORDER_UNDEFINED) ? (7 / 2) : (0);

	std::string name01 = "Box Filter:            ";
	std::string name02 = "Box Filter (sep):      ";
	std::string name03 = "Convolve Filter:       ";
	std::string name04 = "Erode Filter:          ";
	std::string name05 = "Dilate Filter:         ";
	std::string name06 = "Gaussian Filter:       ";
	std::string name07 = "Gaussian Filter (sep): ";
	std::string name08 = "Median Filter:         ";
	std::string name09 = "Scharr3x3 Filter:      ";
	std::string name10 = "                       ";
	std::string name11 = "Sobel Filter:          ";
	std::string name12 = "                       ";
	std::string name13 = "Non-max Suppression:   ";
	std::string name14 = "Segment Test Detector: ";
	std::string name15 = "Hysteresis:            ";
	
	// Non-max Suppression Mask
	TypeUint nms_mask[FILTER_SIZE][FILTER_SIZE];
	for (vx_int32 i = 0; i < FILTER_SIZE; ++i) {
		for (vx_int32 j = 0; j < FILTER_SIZE; ++j) {
			nms_mask[i][j] = 0;
		}
	}

	SwBox<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	ImgBox<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_false_e>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name01.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);	
	ImgBox<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_true_e>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name02.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	SwConvolve<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw, convolve_kernel);
	ImgConvolve<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw, convolve_kernel);
	std::cout << name03.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	SwErode<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	ImgErode<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name04.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	SwDilate<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	ImgDilate<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name05.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	std::cout << name06.c_str();
	SwGaussian<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	ImgGaussian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_false_e>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);
	ImgGaussian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, vx_true_e>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name07.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	SwMedian<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	ImgMedian<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input,
		(vx_image_data<TypeUint, VEC_NUM> *)output_hw);
	std::cout << name08.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);

	SwScharr3x3<TypeUint, TypeInt, COLS_FHD, ROWS_FHD, BORDER_TYPE>(input, output_sw_x, output_sw_y);
	ImgScharr3x3<TypeUint, TypeInt, VEC_NUM, COLS_FHD, ROWS_FHD, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeInt, VEC_NUM> *)output_hw_x, (vx_image_data<TypeInt, VEC_NUM> *)output_hw_y);
	std::cout << name09.c_str();
	CheckError<TypeInt, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw_x, output_hw_x);
	std::cout << name10.c_str();
	CheckError<TypeInt, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw_y, output_hw_y);

	SwSobel<TypeUint, TypeInt, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw_x, output_sw_y);
	ImgSobel<TypeUint, TypeInt, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM> *)input, 
		(vx_image_data<TypeInt, VEC_NUM> *)output_hw_x, (vx_image_data<TypeInt, VEC_NUM> *)output_hw_y);
	std::cout << name11.c_str();
	CheckError<TypeInt, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw_x, output_hw_x);
	std::cout << name12.c_str();
	CheckError<TypeInt, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw_y, output_hw_y);
	
	ImgNonMaxSuppression<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM>*)input, 
		(vx_image_data<TypeUint, VEC_NUM>*)output_hw, nms_mask);
	SwNonMaximaSuppression<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE>(input, output_sw);
	std::cout << name13.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);
	
	ImgSegmentTestDetector<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, BORDER_TYPE>((vx_image_data<TypeUint, VEC_NUM>*)input,
		(vx_image_data<TypeUint, VEC_NUM>*)output_hw);
	SwSegmentTestDetector<TypeUint, COLS_FHD, ROWS_FHD, BORDER_TYPE>(input, output_sw);
	std::cout << name14.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS_FAST>(output_sw, output_hw);	
	
	ImgHysteresis<TypeUint, VEC_NUM, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>((vx_image_data<TypeUint, VEC_NUM>*)input, (vx_image_data<TypeUint, VEC_NUM>*)output_hw);
	SwHysteresis<TypeUint, COLS_FHD, ROWS_FHD, FILTER_SIZE, BORDER_TYPE, THRESH_LOWER, THRESH_UPPER>(input, output_sw);
	std::cout << name15.c_str();
	CheckError<TypeUint, COLS_FHD, ROWS_FHD, BORDER_RADIUS>(output_sw, output_hw);
}

/***************************************************************************************************************************************************/
/**
@brief  Computes a Box filter over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwBox(Type * input, Type * output) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;
	const float NORM = 1.0f / powf(KERN_SIZE, 2);

	// Compute box filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Compute single box filter
			float sum = 0.0f;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

					// Replicated border
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						sum += static_cast<float>(input[yi*WIDTH + xj]);

					// Constant border
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT))
							sum += static_cast<float>(input[yi*WIDTH + xj]);

					// Data is passed through if filter exceeds border
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						sum += static_cast<float>(input[yi*WIDTH + xj]);
					}
				}
			}

			// Store result
			output[y*WIDTH + x] = static_cast<Type>(sum * NORM + 0.5f);
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Convolves the input with the client supplied convolution matrix.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
@param output       The convolve kernel
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwConvolve(Type *input, Type *output, const Type kernel[KERN_SIZE][KERN_SIZE]) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;

	float NORM = 0.0f;
	for (vx_int32 i = 0; i < KERN_SIZE; i++) {
		for (vx_int32 j = 0; j < KERN_SIZE; j++) {
			NORM += kernel[i][j];
		}
	}
	NORM = 1.0f / NORM;

	// Compute custom filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Compute single custom filter
			float sum = 0.0f;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

					// Replicated border
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
						sum += (A * B);

						// Constant border
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
							float A = static_cast<float>(input[yi*WIDTH + xj]);
							float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
							sum += (A * B);
						}

						// Data is passed through if filter exceeds border
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
						sum += (A * B);
					}
				}
			}

			// Store result
			output[y*WIDTH + x] = static_cast<Type>(sum * NORM + 0.5f);
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Implements Dilation, which grows the white space in an image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwDilate(Type *input, Type *output) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;

	// Compute Dilate function
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Single Result
			Type result = std::numeric_limits<Type>::min();

			// Compute Dilate value
			for (int i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
				for (int j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

					// Get kernel coordinates
					int yi = i + y;
					int xj = j + x;

					// Update dilate value
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							Type value = input[yi*WIDTH + xj];
							result = MAX(result, value);
						}
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						Type value = input[yi*WIDTH + xj];
						result = MAX(result, value);
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							Type value = input[yi*WIDTH + xj];
							result = MAX(result, value);
						} else {
							result = MAX(result, (Type)0);
						}
					}
				}
			}

			// Write back result
			output[y*WIDTH + x] = result;
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Implements Erosion, which shrinks the white space in an image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwErode(Type *input, Type *output) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;

	// Compute Dilate function
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Single Result
			Type result = std::numeric_limits<Type>::max();

			// Compute Dilate value
			for (int i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
				for (int j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

					// Get kernel coordinates
					int yi = i + y;
					int xj = j + x;

					// Update dilate value
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							Type value = input[yi*WIDTH + xj];
							result = MIN(result, value);
						}
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						Type value = input[yi*WIDTH + xj];
						result = MIN(result, value);
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							Type value = input[yi*WIDTH + xj];
							result = MIN(result, value);
						} else {
							result = MIN(result, (Type)0);
						}
					}
				}
			}

			// Write back result
			output[y*WIDTH + x] = result;
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Computes a Gaussian filter over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwGaussian(Type * input, Type * output) {

	// Constants
	const vx_uint16 KERN_RAD = KERN_SIZE / 2;

	// Kernel Type
	typedef vx_uint16 KernelTypeT;

	// Normalization
	float norm = 0.0;

	// Kernel
	KernelTypeT kernel[KERN_SIZE][KERN_SIZE];

	// Compute kernel and normalization (Compile-Time)
	ComputeGaussianKernels<KernelTypeT, KERN_SIZE>(kernel);

	// Normalization
	for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
		for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
			norm += static_cast<float>(kernel[y][x]);
		}
	}

	// Compute gaussian filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Compute single gaussian filter
			float sum = 0.0f;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
						sum += (A * B);
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
							float A = static_cast<float>(input[yi*WIDTH + xj]);
							float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
							sum += (A * B);
						}
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[i + KERN_RAD][j + KERN_RAD]);
						sum += (A * B);
					}
				}
			}

			// Store result
			output[y*WIDTH + x] = static_cast<Type>(sum / norm + 0.5f);
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Computes a median pixel value over a window of the input image.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output       The output image
*/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwMedian(Type *input, Type *output) {

	// Constants
	const vx_uint16 KERN_RAD = KERN_SIZE / 2;
	const vx_uint16 WIN_SIZE = KERN_SIZE*KERN_SIZE;
	const vx_uint16 MEDIAN = WIN_SIZE / 2;

	// Window
	Type data[WIN_SIZE];

	// Compute Median filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}			

			// Compute single Median filter
			vx_uint16 ptr = 0;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						data[ptr] = input[yi*WIDTH + xj];
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT))
							data[ptr] = input[yi*WIDTH + xj];
						else
							data[ptr] = 0;
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						data[ptr] = input[yi*WIDTH + xj];						
					}
					ptr++;
				}
			}
			sort(data, data + WIN_SIZE);

			// Store result
			output[y*WIDTH + x] = data[MEDIAN];
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Implements the Scharr Image Filter Kernel.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output_x     The output x image
@param output_y     The output y image
*/
template<typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_border_e BORDER_TYPE>
void SwScharr3x3(InType *input, OutType *output_x, OutType *output_y) {

	// Kernel Type
	typedef vx_int16 KernelTypeT;

	// Kernel Parameters
	const vx_uint16 KERN_NUM = 2;
	const vx_uint16 KERN_SIZE = 3;
	const vx_int32 KERN_RAD = KERN_SIZE / 2;
	const float NORM = 32;

	// Function Input
	const KernelTypeT kernel[KERN_NUM][KERN_SIZE][KERN_SIZE] = {
		{
			{ -3, 0, 3 },
			{ -10, 0, 10 },
			{ -3, 0, 3 },
		},
		{
			{ -3, -10, -3 },
			{ 0, 0, 0 },
			{ 3, 10, 3 },
		} 
	};

	// Compute Scharr filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output_x[y*WIDTH + x] = input[y*WIDTH + x];
					output_y[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Compute single custom filter
			float sum_x = 0.0f, sum_y = 0.0f;;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
						float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
						sum_x += (A * B);
						sum_y += (A * C);
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
							float A = static_cast<float>(input[yi*WIDTH + xj]);
							float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
							float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
							sum_x += (A * B);
							sum_y += (A * C);
						}
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
						float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
						sum_x += (A * B);
						sum_y += (A * C);
					}
				}
			}

			// Store result
			output_x[y*WIDTH + x] = static_cast<OutType>(sum_x / NORM + 0.5f);
			output_y[y*WIDTH + x] = static_cast<OutType>(sum_y / NORM + 0.5f);
		}
	}
}

/***************************************************************************************************************************************************/
/**
@brief  Implements the Sobel Image Filter Kernel.
@param Type         The data type of the input and output
@param WIDTH        The image height
@param HEIGHT       The image width
@param KERN_SIZE    The kernel size of the filter
@param BORDER_TYPE  The border type if filter exceeds border
@param input        The input image
@param output_x     The output x image
@param output_y     The output y image
*/
template<typename InType, typename OutType, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwSobel(InType *input, OutType *output_x, OutType *output_y) {

	// Kernel Type
	typedef vx_int16 KernelTypeT;

	// Kernel Parameters
	const vx_uint16 KERN_NUM = 2;
	const vx_int32 KERN_RAD = KERN_SIZE / 2;

	// Normalization
	float norm = 0.0;

	// Function Input
	KernelTypeT kernel[KERN_NUM][KERN_SIZE][KERN_SIZE];

	// Compute kernel and normalization (Compile-Time)
	ComputeSobelKernels<KernelTypeT, KERN_SIZE>(kernel[0], kernel[1]);

	// Normalization
	for (vx_uint16 x = 0; x < KERN_SIZE; x++) {
		for (vx_uint16 y = 0; y < KERN_SIZE; y++) {
			norm += static_cast<float>(abs(kernel[0][y][x]));
		}
	}

	// Compute Sobel filter
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					output_x[y*WIDTH + x] = input[y*WIDTH + x];
					output_y[y*WIDTH + x] = input[y*WIDTH + x];
					continue;
				}
			}

			// Compute single custom filter
			float sum_x = 0.0f, sum_y = 0.0f;;
			for (vx_int32 i = -KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = -KERN_RAD; j <= KERN_RAD; j++) {

					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						vx_int32 yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						vx_int32 xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
						float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
						sum_x += (A * B);
						sum_y += (A * C);
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						if ((xj >= 0) && (xj < WIDTH) && (yi >= 0) && (yi < HEIGHT)) {
							float A = static_cast<float>(input[yi*WIDTH + xj]);
							float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
							float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
							sum_x += (A * B);
							sum_y += (A * C);
						}
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						vx_int32 yi = y + i;
						vx_int32 xj = x + j;
						float A = static_cast<float>(input[yi*WIDTH + xj]);
						float B = static_cast<float>(kernel[0][i + KERN_RAD][j + KERN_RAD]);
						float C = static_cast<float>(kernel[1][i + KERN_RAD][j + KERN_RAD]);
						sum_x += (A * B);
						sum_y += (A * C);
					}
				}
			}

			// Store result
			output_x[y*WIDTH + x] = static_cast<OutType>((sum_x / norm) + 0.5f);
			output_y[y*WIDTH + x] = static_cast<OutType>((sum_y / norm) + 0.5f);
		}
	}
}

/***************************************************************************************************************************************************/
/*! \brief Compute the Non-max supression for 1 pixel */
template<typename Type, vx_int32 KERN_RAD>
bool CheckNonMax(vx_int32 y, vx_int32 x, Type pixel, Type input) {

	bool maximum = false;

	// Check top left
	if ((y < KERN_RAD) || ((y == KERN_RAD) && (x < KERN_RAD))) {
		if (input < pixel)
			maximum = false;

	// Check bottom right
	} else if ((y > KERN_RAD) || ((y == KERN_RAD) && (x > KERN_RAD))) {
		if (input <= pixel)
			maximum = false;
	}

	return maximum;
}


/***************************************************************************************************************************************************/
template<typename Type, const vx_int32 WIDTH, const vx_int32 HEIGHT, const vx_int32 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwNonMaximaSuppression(Type* src, Type* dst) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;
	const Type min_value = ComputeMin<Type>();

	// Compute  function
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					dst[y*WIDTH + x] = src[y*WIDTH + x];
					continue;
				}
			}

			// Variables
			vx_int32 yi = 0, xj = 0;
			Type input = 0;			
			bool maximum = true;

			// Input
			Type pixel = src[y * WIDTH + x];

			// Compute  value
			for (vx_int32 i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

					// Update  value
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						input = src[yi*WIDTH + xj];
						maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);

					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						yi = i + y;
						xj = j + x;
						input = src[yi*WIDTH + xj];
						maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);

					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						yi = i + y;
						xj = j + x;
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							input = src[yi*WIDTH + xj];
							maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, input);
						} else {
							maximum = CheckNonMax<Type, KERN_RAD>(y, x, pixel, (Type)0);
						}
					}
				}
			}



			// Write back result
			dst[y*WIDTH + x] = (maximum == false) ? (min_value) : (input);
		}
	}
}

/***************************************************************************************************************************************************/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE>
void SwNonMaximaSuppressionPhase(Type* input, Type* output) {

	//// Constants
	//const vx_int32 KERN_RAD = KERN_SIZE / 2;

	//// Compute  function
	//for (vx_int32 y = 0; y < HEIGHT; y++) {
	//	for (vx_int32 x = 0; x < WIDTH; x++) {

	//		// Data is passed through if filter exceeds border
	//		if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
	//			if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
	//				output[y*WIDTH + x] = input[y*WIDTH + x];
	//				continue;
	//			}
	//		}

	//		// Single Result
	//		Type result = 0;

	//		// Compute  value
	//		for (vx_int32 i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
	//			for (vx_int32 j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

	//				// Get kernel coordinates
	//				vx_int32 yi = i + y;
	//				vx_int32 xj = j + x;

	//				// Update  value
	//				if (BORDER_TYPE == VX_BORDER_REPLICATE) {
	//					if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
	//						Type value = input[yi*WIDTH + xj];
	//						//result = MIN(result, value);
	//					}
	//				} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
	//					Type value = input[yi*WIDTH + xj];
	//					//result = MIN(result, value);
	//				} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
	//					if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
	//						Type value = input[yi*WIDTH + xj];
	//						//result = MIN(result, value);
	//					} else {
	//						//result = MIN(result, (Type)0);
	//					}
	//				}
	//			}
	//		}

	//		// Write back result
	//		output[y*WIDTH + x] = result;
	//	}
	//}
}

/***************************************************************************************************************************************************/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_uint16 KERN_SIZE, const vx_border_e BORDER_TYPE, Type THRESH_LOWER, Type THRESH_UPPER>
void SwHysteresis(Type* src, Type* dst) {

	// Constants
	const vx_int32 KERN_RAD = KERN_SIZE / 2;
	const Type max_value = ComputeMax<Type>();

	// Compute  function
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					dst[y*WIDTH + x] = src[y*WIDTH + x];
					continue;
				}
			}

			// Variables
			vx_int32 yi = 0, xj = 0;
			Type input = 0;			

			// Input
			Type pixel = src[y * WIDTH + x];

			// Check the strength of the input
			bool strong = (pixel >= THRESH_UPPER);
			bool weak = (pixel >= THRESH_LOWER);

			// Compute  value
			for (vx_int32 i = (-1) * KERN_RAD; i <= KERN_RAD; i++) {
				for (vx_int32 j = (-1) * KERN_RAD; j <= KERN_RAD; j++) {

					// Update  value
					if (BORDER_TYPE == VX_BORDER_REPLICATE) {
						yi = MIN(MAX(y + i, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
						xj = MIN(MAX(x + j, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
						input = src[yi*WIDTH + xj];
					} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
						yi = i + y;
						xj = j + x;
						input = src[yi*WIDTH + xj];
					} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
						yi = i + y;
						xj = j + x;
						if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
							input = src[yi*WIDTH + xj];
						} else {
							input = 0;
						}
					}

					if ((y != 0) && (x != 0)) {
						if ((input >= THRESH_UPPER) && (weak == true)) {
							strong = true;
						}
					}
				}
			}

			// Write back result
			dst[y*WIDTH + x] = (strong == true) ? (max_value) : ((Type)0);
		}
	}
}

/***************************************************************************************************************************************************/
template<typename Type, const vx_uint16 WIDTH, const vx_uint16 HEIGHT, const vx_border_e BORDER_TYPE>
void SwSegmentTestDetector(Type* src, Type* dst) {

	// Constants
	const vx_int32 KERN_RAD = 3;
	const Type max_value = std::numeric_limits<Type>::max();
	const Type min_value = std::numeric_limits<Type>::min();

	// Bresham circle
	Type circle[16] = { 0 };
	const vx_int8 coordinates[16][2] = {
		{-3,  0}, {-3,  1}, {-2,  2}, {-1,  3}, { 0,  3}, { 1,  3}, { 2,  2}, { 3,  1}, 
		{ 3,  0}, { 3, -1}, { 2, -2}, { 1, -3}, { 0, -3}, {-1, -3}, {-2, -2}, {-3, -1}
	};

	// Compute  function
	for (vx_int32 y = 0; y < HEIGHT; y++) {
		for (vx_int32 x = 0; x < WIDTH; x++) {

			// Data is passed through if filter exceeds border
			if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
				if (x < KERN_RAD || x >= WIDTH - KERN_RAD || y < KERN_RAD || y >= HEIGHT - KERN_RAD) {
					dst[y*WIDTH + x] = src[y*WIDTH + x];
					continue;
				}
			}

			// input/output
			vx_float32 pixel = (vx_float32)src[y * WIDTH + x];
			vx_float32 response = 0;

			// Get input data
			for (vx_int32 i = 0; i < 16; ++i) {
				vx_int32 yi = coordinates[i][0] + y;
				vx_int32 xj = coordinates[i][1] + x;
				Type value = 0;

				if (BORDER_TYPE == VX_BORDER_REPLICATE) {
					yi = MIN(MAX(yi, static_cast<vx_int32>(0)), (vx_int32)(HEIGHT - 1));
					xj = MIN(MAX(xj, static_cast<vx_int32>(0)), (vx_int32)(WIDTH - 1));
					value = src[yi * WIDTH + xj];
				} else if (BORDER_TYPE == VX_BORDER_UNDEFINED) {
					value = src[yi * WIDTH + xj];
				} else if (BORDER_TYPE == VX_BORDER_CONSTANT) {
					if (yi >= 0 && xj >= 0 && yi < HEIGHT && xj < WIDTH) {
						value = src[yi * WIDTH + xj];
					} else {
						value = 0;
					}
				}
				circle[i] = value;
			}

			// Compute
			for (vx_int32 i = 0; i < 16; ++i) {
				vx_float32 min_diff = FLT_MAX;
				bool bigger = true;
				bool smaller = true;
				for (vx_int32 j = i; j < (i + 9); ++j) {
					vx_uint8 ptr = j % 16;
					vx_float32 value = (vx_float32)circle[ptr];
					vx_float32 diff = ABS(value - pixel);
					min_diff = MIN(diff, min_diff);
					if (pixel <= value)
						bigger = false;
					if (pixel >= value)
						smaller = false;
				}
				if (bigger == true || smaller == true)
					response = MAX(response, min_diff);
			}

			// Write back result
			dst[y * WIDTH + x] = (Type)response;// MAX(MIN(response, max_value), min_value);
		}
	}
}

#endif /* SRC_IMG_FILTER_TEST_H_ */
