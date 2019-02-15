/**
* Licence: GNU GPLv3 \n
* You may copy, distribute and modify the software as long as you track
* changes/dates in source files. Any modifications to or software
* including (via compiler) GPL-licensed code must also be made available
* under the GPL along with build & install instructions.
*
* @file    img_other_test.h
* @author  Lester Kalms <lester.kalms@tu-dresden.de>
* @version 1.1
* @brief Description:\n
*  These functions test the remaining image processing functions
*/

#ifndef SRC_IMG_OTHER_TEST_H_
#define SRC_IMG_OTHER_TEST_H_

#include "img_other_base.h"

/*********************************************************************************************************************/
/* Test Functions Headers */
/*********************************************************************************************************************/

void swTestOtherMain(void);
void swTestColorConversion(void);
void swTestScaleDown(void);
void swTestIntegral(void);
void swTestHistogram(void);
void swTestTableLookup(void);

/*********************************************************************************************************************/
/* SW Test Functions */
/*********************************************************************************************************************/

/*! \brief Calls all Software Test Functions */
void swTestOtherMain(void) {
	//swTestColorConversion();
	//swTestIntegral();
	//swTestScaleDown();
	//swTestHistogram();
	//swTestTableLookup();
}

/*! \brief Tests the Color Conversion Function */
void swTestColorConversion(void) {

	const vx_uint32 MATRIX_LENGHT_A = 16;
	const vx_uint32 MATRIX_LENGHT_B = 8;
	const vx_uint32 MATRIX_SIZE = MATRIX_LENGHT_A *MATRIX_LENGHT_B;

	vx_uint8 src_a[MATRIX_LENGHT_A * MATRIX_LENGHT_A];
	vx_uint32 dst_a[MATRIX_LENGHT_A * MATRIX_LENGHT_A];
	vx_uint32 src_b[MATRIX_LENGHT_B * MATRIX_LENGHT_B];
	vx_uint8 dst_b[MATRIX_LENGHT_B * MATRIX_LENGHT_B];

	// Test Grayscale to RGB/RGBX Conversion
	for (vx_uint32 k = 0; k < 2; k++) {

		// Create Input Data
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_A; i++)
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_A; j++)
				src_a[i * MATRIX_LENGHT_A + j] = (vx_uint8)(i * MATRIX_LENGHT_A + j);

		// Reset Output Data
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_A; i++)
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_A; j++)
				dst_a[i * MATRIX_LENGHT_A + j] = 0;

		// Compute the color conversion
		if (k == 0) {
			printf("Color Conversion Grayscale to RGBX:\n");
			ImgConvertColor<vx_uint8, vx_uint32, MATRIX_SIZE, VX_DF_IMAGE_U8, VX_DF_IMAGE_RGBX>(src_a, dst_a);
		}
		if (k == 1) {
			printf("Color Conversion Grayscale to RGB:\n");
			ImgConvertColor<vx_uint8, vx_uint32, MATRIX_SIZE, VX_DF_IMAGE_U8, VX_DF_IMAGE_RGB>(src_a, dst_a);
		}

		// Print the Input Data
		printf(" Input:\n");
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_A; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_A; j++) {
				printf("%4d", src_a[i*MATRIX_LENGHT_A + j]);
			}
			printf("\n");
		}

		// Print the Output Data
		printf(" Output:\n");
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_A; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_A; j++) {
				vx_uint32 data = dst_a[i * MATRIX_LENGHT_A + j];
				vx_uint8 r = (data >> 0) & 0xFF;
				vx_uint8 g = (data >> 8) & 0xFF;
				vx_uint8 b = (data >> 16) & 0xFF;
				vx_uint8 x = (data >> 24) & 0xFF;
				printf("%4d%4d%4d%4d", r, g, b, x);
			}
			printf("\n");
		}
		printf("\n");
	}

	// Test RGBX/RGB to Grayscale Conversion
	for (vx_uint32 k = 0; k < 2; k++) {

		// Create Input Data
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_B; i++)
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_B; j++) {
				vx_uint32 data = i * MATRIX_LENGHT_B + j;
				vx_uint32 r = ((data << 2) + 0) << 0;
				vx_uint32 g = ((data << 2) + 1) << 8;
				vx_uint32 b = ((data << 2) + 2) << 16;
				vx_uint32 x = ((data << 2) + 3) << 24;
				src_b[i * MATRIX_LENGHT_B + j] = r + b + g + x;
			}

		// Reset Output Data
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_B; i++)
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_B; j++)
				dst_b[i * MATRIX_LENGHT_B + j] = 0;

		// Compute the color conversion
		if (k == 0) {
			printf("Color Conversion RGBX to Grayscale:\n");
			ImgConvertColor<vx_uint32, vx_uint8, MATRIX_SIZE, VX_DF_IMAGE_RGBX, VX_DF_IMAGE_U8>(src_b, dst_b);
		}
		if (k == 1) {
			printf("Color Conversion RGB to Grayscale:\n");
			ImgConvertColor<vx_uint32, vx_uint8, MATRIX_SIZE, VX_DF_IMAGE_RGB, VX_DF_IMAGE_U8>(src_b, dst_b);
		}

		// Print the Input Data
		printf(" Input:\n");
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_B; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_B; j++) {
				vx_uint32 data = src_b[i*MATRIX_LENGHT_B + j];
				vx_uint8 r = (data >> 0) & 0xFF;
				vx_uint8 g = (data >> 8) & 0xFF;
				vx_uint8 b = (data >> 16) & 0xFF;
				vx_uint8 x = (data >> 24) & 0xFF;
				printf("%4d%4d%4d%4d |", r, g, b, x);
			}
			printf("\n");
		}

		// Print the Output Data
		printf(" Output:\n");
		for (vx_uint32 i = 0; i < MATRIX_LENGHT_B; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT_B; j++) {
				printf("%4d", dst_b[i*MATRIX_LENGHT_B + j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

/*! \brief Tests the Scale Down Function */
void swTestScaleDown(void) {

	const vx_uint32 SRC_SIZE = 16;
	const vx_uint32 DST_SIZE = 12;

	vx_uint8 in_array[SRC_SIZE * SRC_SIZE];
	vx_uint8 out_array[DST_SIZE * DST_SIZE];

	// Test to Scale Down Image
	for (vx_uint32 k = 0; k < 2; k++) {

		// Create Input Data
		for (vx_uint32 i = 0; i < SRC_SIZE; i++)
			for (vx_uint32 j = 0; j < SRC_SIZE; j++)
				in_array[i * SRC_SIZE + j] = (vx_uint8)(i * SRC_SIZE + j);

		// Reset Output Data
		for (vx_uint32 i = 0; i < DST_SIZE; i++)
			for (vx_uint32 j = 0; j < DST_SIZE; j++)
				out_array[i * DST_SIZE + j] = 0;

		// Scale Down Image
		if (k == 0) {
			printf("Scale image using Nearest Neighbor Interpolation:\n");
			ImgScaleImage<SRC_SIZE, SRC_SIZE, DST_SIZE, DST_SIZE, VX_INTERPOLATION_NEAREST_NEIGHBOR>(in_array, out_array);
		}
		if (k == 1) {
			printf("Scale image using Bilinear Interpolation:\n");
			ImgScaleImage<SRC_SIZE, SRC_SIZE, DST_SIZE, DST_SIZE, VX_INTERPOLATION_BILINEAR>(in_array, out_array);
		}

		// Print the Input Data
		printf(" Input:\n");
		for (vx_uint32 i = 0; i < SRC_SIZE; i++) {
			for (vx_uint32 j = 0; j < SRC_SIZE; j++) {
				printf("%4d", in_array[i*SRC_SIZE + j]);
			}
			printf("\n");
		}

		// Print the Output Data
		printf(" Output:\n");
		for (vx_uint32 i = 0; i < DST_SIZE; i++) {
			for (vx_uint32 j = 0; j < DST_SIZE; j++) {
				printf("%4d", out_array[i * DST_SIZE + j]);
			}
			printf("\n");
		}
		printf("\n");
	}
}

/*! \brief Tests the Integral Function */
void swTestIntegral(void) {

	const vx_uint32 MATRIX_LENGHT = 16;

	vx_uint8 in_array[MATRIX_LENGHT * MATRIX_LENGHT];
	vx_uint32 out_array[MATRIX_LENGHT * MATRIX_LENGHT];

	printf("Integral Image:\n");

	// Create Input Data
	for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++)
		for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++)
			in_array[i * MATRIX_LENGHT + j] = (vx_uint8)(i * MATRIX_LENGHT + j);

	// Reset Output Data
	for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++)
		for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++)
			out_array[i * MATRIX_LENGHT + j] = 0;

	// Compute Integral Image
	ImgIntegral<MATRIX_LENGHT, MATRIX_LENGHT>(in_array, out_array);

	// Print the Input Data
	printf(" Input:\n");
	for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++) {
		for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++) {
			printf("%4d", in_array[i*MATRIX_LENGHT + j]);
		}
		printf("\n");
	}

	// Print the Output Data
	printf(" Output:\n");
	for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++) {
		for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++) {
			printf("%6d", out_array[i * MATRIX_LENGHT + j]);
		}
		printf("\n");
	}
	printf("\n");
}

/*! \brief Tests the Histogram Function */
void swTestHistogram(void) {

	const vx_uint32 OFFSET = 0;
	const vx_uint32 BINS = 256;
	const vx_uint32 RANGE_8 = 256;
	const vx_uint32 MATRIX_LENGHT_16 = 16;
	const vx_uint32 RANGE_16 = 65536;
	const vx_uint32 MATRIX_LENGHT_256 = 256;

	vx_uint8 in_array_8bit[MATRIX_LENGHT_16 * MATRIX_LENGHT_16];
	vx_uint16 in_array_16bit[MATRIX_LENGHT_256 * MATRIX_LENGHT_256];
	vx_uint32 out_array[BINS];

	// Test to create Histogram
	for (vx_uint32 k = 0; k < 2; k++) {

		// Constant
		const vx_uint32 MATRIX_LENGHT = (k == 0) ? (MATRIX_LENGHT_16) : (MATRIX_LENGHT_256);

		// Create Input Data
		for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++) {
				if (k == 0)
					in_array_8bit[i * MATRIX_LENGHT + j] = (vx_uint8)(i * MATRIX_LENGHT + j);
				if (k == 1)
					in_array_16bit[i * MATRIX_LENGHT + j] = (vx_uint16)(i * MATRIX_LENGHT + j);
			}
		}

		// Reset Output Data
		for (vx_uint32 i = 0; i < BINS; i++)
			out_array[i] = 0;

		// Compute Histogram
		if (k == 0) {
			printf("Compute Histogram for BIN=256, RANGE=256, TYPE=UINT8, MATRIX=16x16, OFFSET=0:\n");
			ImgHistogram<vx_uint8, MATRIX_LENGHT_16 * MATRIX_LENGHT_16, BINS, RANGE_8, OFFSET>(in_array_8bit, out_array);
		}
		if (k == 1) {
			printf("Compute Histogram for BIN=256, RANGE=65536, TYPE=UINT16, MATRIX=256x256, OFFSET=0:\n");
			ImgHistogram<vx_uint16, MATRIX_LENGHT_256 * MATRIX_LENGHT_256, BINS, RANGE_16, OFFSET>(in_array_16bit, out_array);
		}

		// Print the Input Data
		printf(" Input:\n");
		for (vx_uint32 i = 0; i < MATRIX_LENGHT; i++) {
			for (vx_uint32 j = 0; j < MATRIX_LENGHT; j++) {
				if (k == 0)
					printf("%6d", in_array_8bit[i*MATRIX_LENGHT + j]);
				if (k == 1)
					printf("%6d", in_array_16bit[i*MATRIX_LENGHT + j]);
			}
			printf("\n");
		}

		// Print the Output Data
		printf(" Output:\n");
		for (vx_uint32 i = 0; i < BINS; i++) {
			printf("%4d", out_array[i]);
		}
		printf("\n");
	}
}

/*! \brief Tests the Table Lookup Function */
void swTestTableLookup(void) {

	// Constants
	typedef vx_int16 DATA_TYPE;	
	const vx_uint32 IMG_PIXELS = 64;
	const vx_uint32 LUT_COUNT = 256;
	const vx_uint32 LUT_OFFSET = 4;	
	const vx_uint32 MAX_DATA_SIZE = VX_INT16_MAX;

	// Arrays
	DATA_TYPE input[IMG_PIXELS];
	DATA_TYPE lut[LUT_COUNT];
	DATA_TYPE output[IMG_PIXELS];
	
	// Create Input Data
	for (vx_int32 i = 0; i < IMG_PIXELS; i++)
		input[i] = (DATA_TYPE)(i - 8);

	// Create LUT
	for (vx_uint32 i = 0; i < LUT_COUNT; i++) 
		lut[i] = rand() % MAX_DATA_SIZE;

	// Reset Output Data
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++)
		output[i] = 0;

	printf("Table Lookup: DATA_TYPE=vx_int16, IMG_PIXELS=64, LUT_COUNT=256, LUT_OFFSET=4\n");
	ImgTableLookup<DATA_TYPE, IMG_PIXELS, LUT_COUNT, LUT_OFFSET>(input, lut, output);

	// Print the Input Data
	printf(" Input:\n");
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++)
		printf("%4d\t", input[i]);
	printf("\n");

	// Print the Lut Data
	printf(" Lut:\n");
	for (vx_uint32 i = 0; i < LUT_COUNT; i++)
		printf("%5d\t", lut[i]);
	printf("\n");

	// Print the Output Data
	printf(" Output:\n");
	for (vx_uint32 i = 0; i < IMG_PIXELS; i++)
		printf("%5d\t", output[i]);
	printf("\n");
}

#endif /* SRC_IMG_OTHER_TEST_H_ */
