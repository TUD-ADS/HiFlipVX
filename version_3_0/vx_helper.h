/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    vx_helper.h
 * @author  Lester Kalms <lester.kalms@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are helper functions needed by different functions (e.g. static assertion).
 * All own Enumerations, Typedefs, Macros and Namespaces are contained in this file.
 */

#ifndef VX_HELPER_H_
#define VX_HELPER_H_

//#define __XILINX__ // Set this flag when using Xilinx tool: HLS & SDX
//#define __LAST__   // Creates a LAST signal that indicates EOF (start of frame) - Set for VDMA & DMA
//#define __USER__      // Creates a USER signal that indicates SOF (start of frame) - Set for VDMA
//#define __EVAL_TIME__ // Turn on time measurements for NN library
//#define __URAM__ // turn on URAM usage

/*********************************************************************************************************************/
/* All includes libraries are here */
/*********************************************************************************************************************/

#define _USE_MATH_DEFINES
#define HALF_ENABLE_CPP11_CMATH 0

#ifdef __clang__
class type_info;
#endif

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef __XILINX__
#include <iomanip>
#include <iostream>
#endif
#include <cmath>
#include <float.h>
#include <limits>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __XILINX__
#include <algorithm>
#endif
#include "vx_types.h"

#ifdef __SDSCC__
#include "sds_lib.h"
#endif

#ifdef __EVAL_TIME__
#ifdef __XILINX__
#include "xtime_l.h"
#else
#include <chrono>
#endif
#endif

#if defined(__SYNTHESIS__) || defined(__RTL_SIMULATION__) || defined(__XILINX__)
#include <ap_int.h>
#include <hls_math.h>
#include <hls_stream.h>
#endif

/*********************************************************************************************************************/
/* Own Macros */
/*********************************************************************************************************************/

// Maximum and Minumum values of datatypes
#define VX_INT8_MIN static_cast<int8_t>(0 - 128)          /*!< \brief Minimum of signed 8 bit type */
#define VX_INT16_MIN static_cast<int16_t>(0 - 32768)      /*!< \brief Minimum of signed 16 bit type */
#define VX_INT32_MIN static_cast<int32_t>(0 - 2147483648) /*!< \brief Minimum of signed 32 bit type */
#define VX_INT8_MAX static_cast<int8_t>(127)              /*!< \brief Maximum of signed 8 bit type */
#define VX_INT16_MAX static_cast<int16_t>(32767)          /*!< \brief Maximum of signed 16 bit type */
#define VX_INT32_MAX static_cast<int32_t>(2147483647)     /*!< \brief Maximum of signed 32 bit type */
#define VX_UINT8_MAX static_cast<uint8_t>(0xff)           /*!< \brief Maximum of unsigned 8 bit type */
#define VX_UINT16_MAX static_cast<uint16_t>(0xffff)       /*!< \brief Maximum of unsigned 16 bit type */
#define VX_UINT32_MAX static_cast<uint32_t>(0xffffffff)   /*!< \brief Maximum of unsigned 32 bit type */

#ifndef M_PI
#define M_PI 3.14159265358979323846 /*!< \brief PI if not defined by cmath */
#endif

// Minimum and Maximu and Absolute
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ABS(a) ((a < 0) ? (-a) : (a))

// GCD and LCM
#define GCD(a, b) ((a >= b) * GCD_1(a, b) + (a < b) * GCD_1(b, a))
#define GCD_1(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_2((b), (a) % ((b) + !(b))))
#define GCD_2(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_3((b), (a) % ((b) + !(b))))
#define GCD_3(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_4((b), (a) % ((b) + !(b))))
#define GCD_4(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_5((b), (a) % ((b) + !(b))))
#define GCD_5(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_6((b), (a) % ((b) + !(b))))
#define GCD_6(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_7((b), (a) % ((b) + !(b))))
#define GCD_7(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_8((b), (a) % ((b) + !(b))))
#define GCD_8(a, b) ((((!(b))) * (a)) + (!!(b)) * GCD_last((b), (a) % ((b) + !(b))))
#define GCD_last(a, b) (a)
#define LCM(a, b) (((a) * (b)) / GCD(a, b))

// For alignment
#define ALIGN(val, parallel) (((val % parallel) == 0) ? (val) : ((val / parallel + 1) * parallel))

/*! \brief Gets the datatype of an integer:
 * \details The output is of type <tt>\ref vx_type_e</tt> and is 8, 16, 32, 64 bit signed or unsigned
 */
#define GET_TYPE(TYPE)                                                                                                 \
    (const vx_type_e)(                                                                                                 \
        (std::numeric_limits<TYPE>::is_integer == true)                                                                \
            ? ((std::numeric_limits<TYPE>::is_signed == false)                                                         \
                   ? ((sizeof(TYPE) == 1)                                                                              \
                          ? (VX_TYPE_UINT8)                                                                            \
                          : ((sizeof(TYPE) == 2) ? (VX_TYPE_UINT16)                                                    \
                                                 : ((sizeof(TYPE) == 4) ? (VX_TYPE_UINT32)                             \
                                                                        : ((sizeof(TYPE) == 8) ? (VX_TYPE_UINT64)      \
                                                                                               : (VX_TYPE_INVALID))))) \
                   : ((sizeof(TYPE) == 1)                                                                              \
                          ? (VX_TYPE_INT8)                                                                             \
                          : ((sizeof(TYPE) == 2)                                                                       \
                                 ? (VX_TYPE_INT16)                                                                     \
                                 : ((sizeof(TYPE) == 4)                                                                \
                                        ? (VX_TYPE_INT32)                                                              \
                                        : ((sizeof(TYPE) == 8) ? (VX_TYPE_INT64) : (VX_TYPE_INVALID))))))              \
            : (VX_TYPE_INVALID))

#define VECTOR_PIXELS(HEIGHT, WIDTH, VEC_NUM)                                                                          \
    (const vx_uint32)((static_cast<vx_uint32>(HEIGHT) * static_cast<vx_uint32>(WIDTH)) /                               \
                      static_cast<vx_uint32>(VEC_NUM))

#define SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, S) ((VEC_NUM_IN > S) ? (VEC_NUM_IN / S) : (1))
#define SCALE_DOWN_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, SCALE_FACTOR)                                                \
    ((const vx_uint32)(VECTOR_PIXELS(IMG_ROWS, IMG_COLS, SCALE_DOWN_VEC_NUM_OUT(VEC_NUM_IN, SCALE_FACTOR)) /           \
                       (SCALE_FACTOR * SCALE_FACTOR)))

#define STEP_VEC_NUM_OUT(VEC_NUM_IN, S) ((VEC_NUM_IN > S + 1) ? (VEC_NUM_IN / (S + 1)) : (1))
#define STEP_PIXELS(IMG_ROWS, IMG_COLS, VEC_NUM_IN, STEP)                                                              \
    ((const vx_uint32)(VECTOR_PIXELS(IMG_ROWS, IMG_COLS, STEP_VEC_NUM_OUT(VEC_NUM_IN, STEP)) /                         \
                       ((STEP + 1) * (STEP + 1))))

/*********************************************************************************************************************/
/* Image Data Types */
/*********************************************************************************************************************/

/*! \brief Needed as image type for vectorization
@param TYPE The data type for the pixels
@param SIZE The vectorization degree
*/
template <class TYPE, const size_t SIZE> //
struct vx_image_data {                   //
    TYPE pixel[SIZE];

#ifndef __SDSCC__
#ifdef __LAST__
#ifdef __XILINX__
    ap_uint<1> last;
#else
    vx_uint8 last;
#endif
#endif
#ifdef __USER__
#ifdef __XILINX__
    ap_uint<1> user;
#else
    vx_uint8 user;
#endif
#endif
#endif

#ifndef __SDSCC__
    // = Operator
    inline vx_image_data &operator=(const vx_image_data &a) {
#pragma HLS INLINE
        for (size_t i = 0; i < SIZE; ++i) {
#pragma HLS UNROLL
            pixel[i] = a.pixel[i];
        }
#ifndef __SDSCC__
#ifdef __LAST__
        last = a.last;
#endif
#ifdef __USER__
        user = a.user;
#endif
#endif
        return *this;
    }
#endif
};

template <class TYPE, const size_t SIZE> //
struct vx_data_pack {                    //
    TYPE pixel[SIZE];
};

struct KeyPoint {
    vx_uint16 x;
    vx_uint16 y;
    vx_uint16 response;
    vx_uint8 scale; // classId or evolution level
    vx_uint8 orientation;
};
union KeyPointConverter {
    KeyPoint keypoint;
    vx_uint64 data;
    vx_uint8 bytes[8];
};

/*********************************************************************************************************************/
/* Allocate/Free Memory */
/*********************************************************************************************************************/

/*! \brief Creates an image object
@param  ImageType The output image type (use "struct vx_image<...>" for vectorization of 2, 4 and 8)
@param  PixelType The data type of the image pixels
@param  COLS      The image columns
@param  ROWS      The image rows
@return           Pointer to the allocated memory
*/
template <typename ImageType, vx_uint32 PIXELS, vx_uint32 PARALLEL> __inline ImageType *CreateImage() {
#ifdef __SDSCC__
    return (ImageType *)sds_alloc((sizeof(ImageType) * PIXELS) / PARALLEL);
#else
    return (ImageType *)(new ImageType[PIXELS / PARALLEL]);
#endif
}

/*! \brief Destroys an image object
@param  ImageType The image type
@param  image     The image
*/
template <typename ImageType> __inline void DestroyImage(ImageType *image) {
#ifdef __SDSCC__
    return sds_free(image);
#else
    return delete[] image;
#endif
}

/*********************************************************************************************************************/
/* Measure Time */
/*********************************************************************************************************************/

// Timing functions
#ifdef __EVAL_TIME__
#ifdef __XILINX__
void StartTime(XTime &t1) {
    XTime_SetTime(0);
    XTime_GetTime(&t1);
}
double EndTime(XTime t1, const char *name) {
    XTime t2;
    XTime_GetTime(&t2);
    double total_time = (double)(t2 - t1);
    return (total_time / (double)COUNTS_PER_SECOND) * 1000000;
}
#else
void StartTime(std::chrono::high_resolution_clock::time_point &t1) { t1 = std::chrono::high_resolution_clock::now(); }
double EndTime(std::chrono::high_resolution_clock::time_point t1, const char *name) {
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    return time_span.count() * 1000000000.0;
}
#endif
#endif

// Evaluation of the timing
template <vx_int64 NUM> void EvaluateTime(double sw[NUM], double hw[NUM], const char *name) {

    double sw_time_max = DBL_MIN;
    double sw_time_min = DBL_MAX;
    double sw_time_avg = 0.0;
    double hw_time_max = DBL_MIN;
    double hw_time_min = DBL_MAX;
    double hw_time_avg = 0.0;

    for (vx_int64 i = 0; i < NUM; ++i) {
        double sw_time = sw[i];
        sw_time_max = MAX(sw_time_max, sw_time);
        sw_time_min = MIN(sw_time_min, sw_time);
        sw_time_avg += sw_time;
        double hw_time = hw[i];
        hw_time_max = MAX(hw_time_max, hw_time);
        hw_time_min = MIN(hw_time_min, hw_time);
        hw_time_avg += hw_time;
    }
    sw_time_avg /= NUM;
    hw_time_avg /= NUM;

#ifdef __XILINX__
    printf("%s\tTime (us):", name);
    printf("SW: %9.0f(max)   %9.0f(avg)   %9.0f(min) \n", sw_time_max, sw_time_avg, sw_time_min);
    printf("%s\tTime (us):", name);
    printf("HW: %9.0f(max)   %9.0f(avg)   %9.0f(min) \n", hw_time_max, hw_time_avg, hw_time_min);
#else
    printf("%s\tTime (ns):  ", name);
    printf("SW: %9.0f(max)   %9.0f(avg)   %9.0f(min) \n", sw_time_max, sw_time_avg, sw_time_min);
    printf("%s\tTime (ns):  ", name);
    printf("HW: %9.0f(max)   %9.0f(avg)   %9.0f(min) \n", hw_time_max, hw_time_avg, hw_time_min);
#endif
}

void checkSuccess(vx_bool success) {
    if (!success) {
        printf("\nError was larger than allowed! Stopped testing!\n");
        exit(EXIT_FAILURE);
    }
}

/*********************************************************************************************************************/
/* NN Structs */
/*********************************************************************************************************************/

// Mobile Net Paramaters
struct MobileNetParam1 {
    const vx_int64 SRC_ROWS;
    const vx_int64 SRC_COLS;
    const vx_int64 DST_ROWS;
    const vx_int64 DST_COLS;
    const vx_int64 IFM;
    const vx_int64 OFM;
    const vx_int64 DW_PARALLEL;
    const vx_int64 IFM_PARALLEL;
    const vx_int64 OFM_PARALLEL;
    const vx_int64 PW_PARALLEL;
    const vx_int64 DMA_PARALLEL;
    const vx_int64 BATCHES;
    const vx_int64 FIXED_POINT_POSITION;
    const vx_round_policy_e ROUND_POLICY;
    const vx_convert_policy_e OVERFLOW_POLICY;
};

// Parameters for Convolution (Depth-Wise Point-Wise) & Pooling
struct NeuralNetworkParam1 {
    const vx_int64 BATCHES;
    const vx_int64 SRC_ROWS;
    const vx_int64 SRC_COLS;
    const vx_int64 DST_ROWS;
    const vx_int64 DST_COLS;
    const vx_int64 IFM;
    const vx_int64 OFM;
    const vx_int64 IFM_PARALLEL;
    const vx_int64 OFM_PARALLEL;
    const vx_int64 KERNEL_X;
    const vx_int64 KERNEL_Y;
    const vx_int64 BIASES_SIZE;
    const vx_int64 FIXED_POINT_POSITION;
    const vx_convert_policy_e OVERFLOW_POLICY;
    const vx_round_policy_e ROUNDING_POLICY;
    const vx_int64 PADDING_Y;
    const vx_int64 PADDING_X;
    const vx_nn_pooling_type_e POOLING_TYPE;
};

// Parameters for Activation, Softmax, Batch Normalization, Fully Connected
template <typename T> struct NeuralNetworkParam2 {
    const vx_int64 BATCHES;
    const vx_int64 PIXELS;
    const vx_int64 IFM;
    const vx_int64 OFM;
    const vx_int64 PARALLEL;
    const vx_int64 BIASES_SIZE;
    const vx_int64 FIXED_POINT_POSITION;
    const vx_round_policy_e ROUNDING_POLICY;
    const vx_convert_policy_e OVERFLOW_POLICY;
    const T PARAM_A;
    const T PARAM_B;
    const vx_nn_activation_function_e ACTIVATION_FUNCTION;
    const bool RELAXED_MATH;
};

/*********************************************************************************************************************/
/* Own enums  */
/*********************************************************************************************************************/

/*! \brief Contains the different pixelwise and filter operation types
 */
namespace HIFLIPVX {

/*! \brief Perfroms a windowed filter function on an image
 */
enum FilterOperation {
    GAUSSIAN_FILTER,      /*!< \brief Gaussian filter (optimized for kernel structure) */
    DERIVATIVE_X,         /*!< \brief Scharr/Sobel derivative filter (optimized for kernel structure) */
    DERIVATIVE_Y,         /*!< \brief Scharr/Sobel derivative filter (optimized for kernel structure) */
    CUSTOM_CONVOLUTION_U, /*!< \brief Costum convolution filter */
    CUSTOM_CONVOLUTION_S, /*!< \brief Costum convolution filter */
    BOX_FILTER,           /*!< \brief Box filter (optimized for kernel structure) */
    MEDIAN_FILTER_NxN,    /*!< \brief Median filter (optimized for kernel structure) */
    MEDIAN_FILTER_3x3,    /*!< \brief Median filter (optimized for kernel structure) */
    ERODE_IMAGE,          /*!< \brief Erode image filter (sperable kernel only) */
    DILATE_IMAGE,         /*!< \brief Dilate image filter (sperable kernel only) */
    NON_MAX_SUPPRESSION,  /*!< \brief Do Non-Maxima Suppression */
    SEGMENT_TEST_DETECTOR,
    HYSTERESIS,
    ORIENTED_NON_MAX_SUPPRESSION,
    FED,
    DOH,
    PERONA_MALIK_WIDE_REGION,
    FORWARD1, // forwards 1. input
    FORWARD2, // forwards 2. input
};

/*! \brief Performs an pixelwise operation on an image (same data type for input/output)
 */
enum PixelwiseOperationA {
    COPY_DATA,              /*!< \brief Copy a data object to another. */
    BITWISE_NOT,            /*!< \brief Performs a bitwise NOT operation on a input images. */
    BITWISE_AND,            /*!< \brief Performs a bitwise AND operation between two unsigned images. */
    BITWISE_OR,             /*!< \brief Performs a bitwise INCLUSIVE OR operation between two unsigned images. */
    BITWISE_XOR,            /*!< \brief Performs a bitwise EXCLUSIVE OR (XOR) operation between two unsigned images. */
    MAX,                    /*!< \brief Implements a pixel-wise maximum kernel. */
    MIN,                    /*!< \brief Implements a pixel-wise minimum kernel. */
    ABSOLUTE_DIFFERENCE,    /*!< \brief Computes the absolute difference between two images */
    ARITHMETIC_ADDITION,    /*!< \brief Performs addition between two images */
    ARITHMETIC_SUBTRACTION, /*!< \brief Performs subtraction between two images */
    MAGNITUDE,              /*!< \brief Implements the Gradient Magnitude Computation Kernel */
    MULTIPLY,               /*!< \brief Performs element-wise multiplication between two images and a scalar value. */
    THRESHOLD,              /*!< \brief  Thresholds an input image and produces an output Boolean image. */
    WEIGHTED_AVERAGE,       /*!< \brief  Accumulates a weighted value from an input image to an output image.  */
    PHASE,
    MULTIPLY_CONSTANT,
};

enum NonMaxSuppressionMask {
    SQUARE,
    CIRCLE,
    USER_DEFINED,
    NO_NMS,
};

/*! \brief The Scatter Policy Enumeration. (TODO: own)
 */
enum vx_scatter_mode_e {
    VX_SCATTER_GATHER_MODE_BLOCK = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCATTER_GATHER_MODE) + 0x0,
    VX_SCATTER_GATHER_MODE_CYCLIC = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCATTER_GATHER_MODE) + 0x1
};

} // namespace HIFLIPVX

/*********************************************************************************************************************/
/* Own PRAGMAs to use different FPGA toolchains */
/*********************************************************************************************************************/

#define CREATE_STRING(STR) #STR
#ifdef __URAM__
#define PRAGMA_HLS_MEM_CORE(VAR, DATATYPE, ELEMENTS)                                                                   \
    _Pragma(CREATE_STRING(HLS DATA_PACK variable = VAR)) const vx_uint32 RAM_BYTES =                                   \
        (MIN(sizeof(DATATYPE), 4)) * (ELEMENTS);                                                                       \
    if (RAM_BYTES >= 8192) {                                                                                           \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = XPM_MEMORY uram))                                     \
    } else if (RAM_BYTES >= 128 && ELEMENTS >= 32) {                                                                   \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = RAM_2P_BRAM))                                         \
    } else if (ELEMENTS >= 4) {                                                                                        \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = RAM_2P_LUTRAM))                                       \
    } else {                                                                                                           \
        _Pragma(CREATE_STRING(HLS ARRAY_PARTITION variable = VAR complete))                                            \
    }
#else
#define PRAGMA_HLS_MEM_CORE(VAR, DATATYPE, ELEMENTS)                                                                   \
    _Pragma(CREATE_STRING(HLS DATA_PACK variable = VAR)) const vx_uint32 RAM_BYTES =                                   \
        (MIN(sizeof(DATATYPE), 4)) * (ELEMENTS);                                                                       \
    if (RAM_BYTES >= 8192) {                                                                                           \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = RAM_2P_BRAM))                                         \
    } else if (RAM_BYTES >= 128 && ELEMENTS >= 32) {                                                                   \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = RAM_2P_BRAM))                                         \
    } else if (ELEMENTS >= 4) {                                                                                        \
        _Pragma(CREATE_STRING(HLS RESOURCE variable = VAR core = RAM_2P_LUTRAM))                                       \
    } else {                                                                                                           \
        _Pragma(CREATE_STRING(HLS ARRAY_PARTITION variable = VAR complete))                                            \
    }
#endif /* __URAM__ */

#define DO_PRAGMA_(x) _Pragma(#x)
#define DO_PRAGMA(x) DO_PRAGMA_(x)
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, NAME, ...) NAME
#if defined(__VITIS_HLS__)
#define DATAPACK1(var1) DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)
#define DATAPACK2(var1, var2)                                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)
#define DATAPACK3(var1, var2, var3)                                                                                    \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)
#define DATAPACK4(var1, var2, var3, var4)                                                                              \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var4 compact = bit)
#define DATAPACK5(var1, var2, var3, var4, var5)                                                                        \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var4 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var5 compact = bit)
#define DATAPACK6(var1, var2, var3, var4, var5, var6)                                                                  \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var4 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var5 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var6 compact = bit)
#define DATAPACK7(var1, var2, var3, var4, var5, var6, var7)                                                            \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var4 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var5 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var6 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var7 compact = bit)
#define DATAPACK8(var1, var2, var3, var4, var5, var6, var7, var8)                                                      \
    DO_PRAGMA(HLS AGGREGATE variable = var1 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var2 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var3 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var4 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var5 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var6 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var7 compact = bit)                                                             \
    DO_PRAGMA(HLS AGGREGATE variable = var8 compact = bit)
#define DATAPACK9(var1, var2, var3, var4, var5, var6, var7, var8, var9)                                                \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)
#define DATAPACK10(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10)                                        \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)
#define DATAPACK11(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11)                                 \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)
#define DATAPACK12(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12)                          \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)
#define DATAPACK13(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13)                   \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var13)
#define DATAPACK14(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14)            \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var13)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var14)
#define DATAPACK15(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15)     \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var13)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var14)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var15)
#define DATAPACK16(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15,     \
                   var16)                                                                                              \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var13)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var14)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var15)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var16)
#define DATAPACK17(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15,     \
                   var16, var17)                                                                                       \
    DO_PRAGMA(HLS AGGREGATE variable = var1)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var2)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var3)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var4)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var5)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var6)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var7)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var8)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var9)                                                                           \
    DO_PRAGMA(HLS AGGREGATE variable = var10)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var11)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var12)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var13)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var14)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var15)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var16)                                                                          \
    DO_PRAGMA(HLS AGGREGATE variable = var17)
#else
#define DATAPACK1(var1) DO_PRAGMA(HLS DATA_PACK variable = var1)
#define DATAPACK2(var1, var2)                                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)
#define DATAPACK3(var1, var2, var3)                                                                                    \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)
#define DATAPACK4(var1, var2, var3, var4)                                                                              \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)
#define DATAPACK5(var1, var2, var3, var4, var5)                                                                        \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)
#define DATAPACK6(var1, var2, var3, var4, var5, var6)                                                                  \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)
#define DATAPACK7(var1, var2, var3, var4, var5, var6, var7)                                                            \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)
#define DATAPACK8(var1, var2, var3, var4, var5, var6, var7, var8)                                                      \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)
#define DATAPACK9(var1, var2, var3, var4, var5, var6, var7, var8, var9)                                                \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)
#define DATAPACK10(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10)                                        \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)
#define DATAPACK11(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11)                                 \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)
#define DATAPACK12(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12)                          \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)
#define DATAPACK13(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13)                   \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var13)
#define DATAPACK14(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14)            \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var13)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var14)
#define DATAPACK15(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15)     \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var13)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var14)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var15)
#define DATAPACK16(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15,     \
                   var16)                                                                                              \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var13)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var14)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var15)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var16)
#define DATAPACK17(var1, var2, var3, var4, var5, var6, var7, var8, var9, var10, var11, var12, var13, var14, var15,     \
                   var16, var17)                                                                                       \
    DO_PRAGMA(HLS DATA_PACK variable = var1)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var2)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var3)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var4)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var5)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var6)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var7)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var8)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var9)                                                                           \
    DO_PRAGMA(HLS DATA_PACK variable = var10)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var11)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var12)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var13)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var14)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var15)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var16)                                                                          \
    DO_PRAGMA(HLS DATA_PACK variable = var17)
#endif

#define HIFLIPVX_DATAPACK(...)                                                                                         \
    GET_MACRO(__VA_ARGS__, DATAPACK17, DATAPACK16, DATAPACK15, DATAPACK14, DATAPACK13, DATAPACK12, DATAPACK11,         \
              DATAPACK10, DATAPACK9, DATAPACK8, DATAPACK7, DATAPACK6, DATAPACK5, DATAPACK4, DATAPACK3, DATAPACK2,      \
              DATAPACK1)                                                                                               \
    (__VA_ARGS__)

/*********************************************************************************************************************/
/* Static Assertion (COMPILE TIME) */
/*********************************************************************************************************************/

// note that we wrap the non existing type inside a struct to avoid warning
// messages about unused variables when static assertions are used at function
// scope
// the use of sizeof makes sure the assertion error is not ignored by SFINAE
template <bool> struct StaticAssertion;
template <> struct StaticAssertion<true> {};    // StaticAssertion<true>
template <int i> struct StaticAssertionTest {}; // StaticAssertionTest<int>

#define CONCATENATE(arg1, arg2) CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

/*! \brief Pre-C++11 Static Assertions Without Boost
 * \details: from  http://stackoverflow.com/questions/1980012/boost-static-assert-without-boost/1980156
 *
 * <code>STATIC_ASSERT(expression, message)</code>
 *
 * When the static assertion test fails, a compiler error message that somehow
 * contains the "STATIC_ASSERTION_FAILED_AT_LINE_xxx_message" is generated.
 *
 * /!\ message has to be a valid C++ identifier, that is to say it must not
 * contain space characters, cannot start with a digit, etc.
 *
 * STATIC_ASSERT(true, this_message_will_never_be_displayed);
 */
#define STATIC_ASSERT(expression, message)                                                                             \
    struct CONCATENATE(__static_assertion_at_line_, __LINE__) {                                                        \
        StaticAssertion<static_cast<bool>((expression))>                                                               \
            CONCATENATE(CONCATENATE(CONCATENATE(STATIC_ASSERTION_FAILED_AT_LINE_, __LINE__), _), message);             \
    };                                                                                                                 \
    typedef StaticAssertionTest<sizeof(CONCATENATE(__static_assertion_at_line_, __LINE__))> CONCATENATE(               \
        __static_assertion_test_at_line_, __LINE__)

/*********************************************************************************************************************/
/* HW: Helper Functions for arithmetic computation  */
/*********************************************************************************************************************/

// Set user(SOF) & last(EOF)
template <typename T, vx_int32 VEC_NUM>                                                                         //
void GenerateDmaSignal(const bool condition_first, const bool condition_last, vx_image_data<T, VEC_NUM> &dst) { //
#pragma HLS INLINE
#ifndef __SDSCC__
#ifdef __LAST__
    dst.last = (condition_last == true) ? (1) : (0);
#endif
#ifdef __USER__
    dst.user = (condition_first == true) ? (1) : (0);
#endif
#endif
}

/*! \brief Gets the maximum value of a certain data type
@param Type  The data type that should be evaluated
@return      The maximum value for "Type"
*/
template <typename Type> Type ComputeMax() {
#pragma HLS INLINE
    const vx_type_e MAX_TYPE = GET_TYPE(Type);
    return (MAX_TYPE == VX_TYPE_UINT8)                                      //
               ? (VX_UINT8_MAX)                                             //
               : ((MAX_TYPE == VX_TYPE_UINT16)                              //
                      ? (VX_UINT16_MAX)                                     //
                      : ((MAX_TYPE == VX_TYPE_UINT32)                       //
                             ? (VX_UINT32_MAX)                              //
                             : ((MAX_TYPE == VX_TYPE_INT8)                  //
                                    ? (VX_INT8_MAX)                         //
                                    : ((MAX_TYPE == VX_TYPE_INT16)          //
                                           ? (VX_INT16_MAX)                 //
                                           : ((MAX_TYPE == VX_TYPE_INT32)   //
                                                  ? (VX_INT32_MAX)          //
                                                  : (VX_TYPE_INVALID)))))); //
}

/*! \brief Gets the minimum value of a certain data type
@param Type  The data type that should be evaluated
@return      The minimum value for "Type"
*/
template <typename Type> Type ComputeMin() {
#pragma HLS INLINE
    const vx_type_e MIN_TYPE = GET_TYPE(Type);
    return (MIN_TYPE == VX_TYPE_UINT8)                                      //
               ? (0)                                                        //
               : ((MIN_TYPE == VX_TYPE_UINT16)                              //
                      ? (0)                                                 //
                      : ((MIN_TYPE == VX_TYPE_UINT32)                       //
                             ? (0)                                          //
                             : ((MIN_TYPE == VX_TYPE_INT8)                  //
                                    ? (VX_INT8_MIN)                         //
                                    : ((MIN_TYPE == VX_TYPE_INT16)          //
                                           ? (VX_INT16_MIN)                 //
                                           : ((MIN_TYPE == VX_TYPE_INT32)   //
                                                  ? (VX_INT32_MIN)          //
                                                  : (VX_TYPE_INVALID)))))); //
}

/*! \brief Saturates a function to a maximum and minimum value
@param Type    The data type of the value
@param data    The input value
@param max_val The maximum value
@param min_val The minimum value
@return        The saturated value
*/
template <typename Type> Type SaturateMaxMin(Type data, Type max_val, Type min_val) {
#pragma HLS INLINE
    if (data > max_val)
        return max_val;
    else if (data < min_val)
        return min_val;
    else
        return data;
}

/*! \brief An arithmetic right shift that simulates the behavior of division.
           For signed values there is a different behavior between shift and division when
rounding.
@param Type             The data type of the value
@param SIGNED_DATA_TYPE If value is signed
@param data             The input value
@param shift            The shift value
@return                 The result
*/
template <typename Type, bool SIGNED_DATA_TYPE> Type ShiftArithmeticRight(Type data, vx_uint16 shift) {
#pragma HLS INLINE
    Type a = data >> shift;
    if ((SIGNED_DATA_TYPE == true) && (a < 0)) {
        a += 1;
    }
    return a;
}

/** @brief Computes the ATAN2(x,y) using the cordic algorithm
@param x  x-vector
@param y  y_vector
@return	  atan2(x,y)
*/
template <typename CompType, vx_uint16 STEPS> CompType atan2Cordic(CompType x, CompType y) {
#pragma HLS INLINE

    CompType sgn, xh, yh, winkel;
    CompType i_atantab[] = {16384, 9672, 5110, 2594, 1302, 652, 326, 163, 81, 41, 20, 10, 5, 3, 1, 1};

    // Initalization
    sgn = (y >= 0) ? -1 : 1;
    xh = -sgn * y;
    yh = +sgn * x;
    x = xh;
    y = yh;
    winkel = sgn * i_atantab[0];

    // Iteration maximum: i<31
    for (vx_uint16 i = 1, k = 0; i <= STEPS; i++, k++) {
#pragma HLS unroll
        sgn = (y >= 0) ? -1 : 1;
        winkel += sgn * i_atantab[i];
        xh = x - sgn * (y >> k);
        yh = y + sgn * (x >> k);
        x = xh;
        y = yh;
        if (y == 0)
            break;
    }

    return -winkel;
}

/** @brief Computes the ATAN2(x,y) using the cordic algorithm
@param x  x-vector
@param y  y_vector
@return	  atan2(x,y)
*/
vx_int64 atan2CordicAccurate(vx_int64 x, vx_int64 y) {
#pragma HLS INLINE

    vx_int64 sgn, xh, yh, winkel;
    vx_int64 i_atantab[] = {1073741824, 536870912, 316933406, 167458907, 85004756, 42667331, 21354465, 10679838,
                            5340245,    2670163,   1335087,   667544,    333772,   166886,   83443,    41722,
                            20861,      10430,     5215,      2608,      1304,     652,      326,      163,
                            81,         41,        20,        10,        5,        3,        1};

    // Initalization
    sgn = (y >= 0) ? -1 : 1;
    xh = -sgn * y;
    yh = +sgn * x;
    x = xh;
    y = yh;
    winkel = sgn * i_atantab[0];

    // Iteration maximum: i<31
    for (vx_uint16 i = 1, k = 0; i <= 24; i++, k++) {
#pragma HLS unroll
        sgn = (y >= 0) ? -1 : 1;
        winkel += sgn * i_atantab[i];
        xh = x - sgn * (y >> k);
        yh = y + sgn * (x >> k);
        x = xh;
        y = yh;
        if (y == 0)
            break;
    }

    return -winkel;
}

/** @brief Computes square root: Rounding to floor or to nearest integer
@param OutType		The data type of the output
@param InType		The data type of the input
@param ROUND_POLICY Rounding policy (to zero & nearest even)
@param CHECK_MAX	Is true, if the maximum value should be checked (needed for rounding to nearest even)
@param value		The input value
@return				The square root of the input value
*/
template <typename OutType, typename InType, bool CHECK_MAX, vx_round_policy_e ROUND_POLICY,
          vx_uint8 OUTPUT_BIT_WIDTH> //
OutType SqrtLester(InType value) {   //
#pragma HLS INLINE

    const OutType MAX_VAL = ComputeMax<OutType>();

    // Number of stages (latency)
    const vx_uint8 N = OUTPUT_BIT_WIDTH; // sizeof(OutType) * 8;

    // Variables
    OutType A1 = 0; // A^1 Intermediate result
    InType A2 = 0;  // A^2 Square of the intermediate result

    // Each stage computes 1 bit of the resulting vector
    for (vx_uint8 n = N - 1; n < N; n--) {
#pragma HLS unroll

        // Add new bit of position n and compute (A1 + B1)^2
        OutType B1 = static_cast<OutType>(1) << (n);
        InType B2 = static_cast<InType>(B1) << (n);
        InType AB = static_cast<InType>(A1) << (n);
        InType A2_next = A2 + B2 + (AB << 1); // A*A + B*B + 2*A*B

        // Store if tmp does not exceed value
        if (A2_next <= value) {
            A1 |= B1;
            A2 = A2_next;
        }
    }

    // Round to the nearest integer and check for overflow
    if (ROUND_POLICY == VX_ROUND_POLICY_TO_NEAREST_EVEN) {
        if (CHECK_MAX) {
            if (((value - A2) > static_cast<InType>(A1)) && (A1 != MAX_VAL))
                A1++;
        } else {
            if ((value - A2) > static_cast<InType>(A1))
                A1++;
        }
    }

    // Return result
    return A1;
}

/*********************************************************************************************************************/
/* HW: Helper Functions to Create Kernels (COMPILE TIME) */
/*********************************************************************************************************************/

/** @brief Computes the Gaussian Kernel using the Sigma values (not compile time in all SDx versions)
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The computed gaussian kernel
@param sigma      The input sigma value
@return           The fraction size of the fixed-point result
*/
template <typename ScalarType, const vx_uint8 KERN_SIZE>
const vx_uint16 ComputeGaussianKernelSigma(ScalarType kernel[KERN_SIZE][KERN_SIZE], const double sigma) {
#pragma HLS INLINE

    // Constants
    const vx_uint16 K_RAD = (vx_uint16)KERN_SIZE / (vx_uint16)2;
    const double s = 2.0 * sigma * sigma;

    // Variables
    double sum = 0.0;
    double doubleKernel[KERN_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = doubleKernel complete dim = 0

    // Creates the gaussian kernel
    for (vx_uint8 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
        for (vx_uint8 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
            const vx_uint32 a = static_cast<vx_uint32>((vx_uint16)x - K_RAD);
            const vx_uint32 b = static_cast<vx_uint32>((vx_uint16)y - K_RAD);
            const double c = static_cast<double>(a * a + b * b);
            const double r = sqrt(c);
            doubleKernel[x][y] = (exp(-(r * r) / s)) / (M_PI * s);
            sum += doubleKernel[x][y];
        }
    }

    // Normalizes the gaussian kernel
    for (vx_uint8 x = 0; x < KERN_SIZE; x++) {
#pragma HLS unroll
        for (vx_uint8 y = 0; y < KERN_SIZE; y++) {
#pragma HLS unroll
            doubleKernel[x][y] /= sum;
        }
    }

    // Computes the fraction for the fixed point representation
    vx_int32 e_max = VX_INT32_MIN;
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            int e = 0;
            frexp(doubleKernel[i][j], &e);
            e_max = MAX(e_max, (vx_int32)e);
        }
    }
    const vx_int32 fraction = 8 * sizeof(ScalarType) - e_max;
    const vx_uint16 kernFraction = static_cast<vx_uint16>(MIN(MAX(fraction, (vx_int32)0), (vx_int32)31));

    // Computes and stores the fixed point kernel
    vx_uint32 shift = 1;
    for (vx_uint16 i = 0; i < kernFraction; i++) {
#pragma HLS unroll
        shift *= 2;
    }
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            kernel[i][j] = static_cast<ScalarType>(doubleKernel[i][j] * static_cast<double>(shift));
        }
    }

    // Returns the fraction
    return kernFraction;
}

/** @brief Convolute an array with the convolution array [1 2 1]
@param ScalarType      Data type of the kernel
@param KERN_SIZE       The size of the final kernel
@param BUFFER_SIZE     The amount of kernels that can be stored in "kernel_buffer"
@param kernel_buffer   Buffers input and output kernel
@param cur_kernel_ptr  Pointer where the input kernel is stored
@param cur_kernel_size The size of the input kernel
*/
template <typename ScalarType, vx_uint8 KERN_SIZE, vx_int16 BUFFER_SIZE>
void ComputeConvolution(ScalarType kernel_buffer[BUFFER_SIZE][KERN_SIZE], const vx_int16 cur_kernel_ptr,
                        const vx_int16 cur_kernel_size) {
#pragma HLS INLINE

    // Constants
    const vx_int16 kernel_size = cur_kernel_size + 2;
    const vx_int16 kernel_rad = kernel_size >> 1;

    // Standard convolution kernel
    ScalarType convoltion[3] = {1, 2, 1};
#pragma HLS array_partition variable = convoltion complete dim = 0

    // convolute kernel
    for (vx_int32 j = 0; j < kernel_size; j++) {
#pragma HLS unroll
        const vx_int32 range = MIN((j < kernel_rad) ? (j + 1) : (kernel_size - j), (vx_int32)3);
        const vx_int32 start_0 = MAX(2 - j, (vx_int32)0);
        const vx_int32 start_1 = MAX(j - 2, (vx_int32)0);
        ScalarType sum = 0;
        for (vx_int32 k = 0; k < range; k++) {
#pragma HLS unroll
            sum += (convoltion[start_0 + k] * kernel_buffer[cur_kernel_ptr][start_1 + k]);
        }
        kernel_buffer[cur_kernel_ptr + 1][j] = sum;
    }
}

/** @brief Computes 2d kernel from two 1d kernels
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param src1       The 1. 1d kernel
@param src2       The 2. 1d kernel
@param dst        The resulting 2d kernel
*/
template <typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeKernel(ScalarType src1[KERN_SIZE], ScalarType src2[KERN_SIZE], ScalarType dst[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            dst[i][j] = src1[i] * src2[j];
        }
    }
}

/** @brief Computes the 1d kernels for the sobel filter
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param sobel_x    Stores the sobel kernel in x-direction
@param sobel_y    Stores the sobel kernel in y-direction
*/
template <typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeSobelKernels(ScalarType sobel_x[KERN_SIZE][KERN_SIZE], ScalarType sobel_y[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    // Constants and variables
    const vx_int16 ONE = static_cast<vx_int16>(1);
    const vx_int16 THREE = static_cast<vx_int16>(3);
    const vx_int16 BUFFER_SIZE = (((vx_int16)KERN_SIZE - THREE) >> ONE) + ONE;
    vx_int16 buffer_ptr = 0;

    // Sobel 1d kernel
    ScalarType buffer_0[BUFFER_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = buffer_0 complete dim = 0
    buffer_0[0][0] = 1;
    buffer_0[0][1] = 2;
    buffer_0[0][2] = 1;
    ScalarType buffer_1[BUFFER_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = buffer_1 complete dim = 0
    buffer_1[0][0] = -1;
    buffer_1[0][1] = 0;
    buffer_1[0][2] = 1;

    // vx_uint8 kernel for higher kernel size
    for (vx_uint8 i = 3; i < KERN_SIZE; i += 2) {
#pragma HLS unroll
        ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_0, buffer_ptr, (vx_int16)i);
        ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_1, buffer_ptr, (vx_int16)i);
        buffer_ptr++;
    }

    // Compute the 2d kernel
    ComputeKernel<ScalarType, KERN_SIZE>(buffer_0[buffer_ptr], buffer_1[buffer_ptr], sobel_x);
    ComputeKernel<ScalarType, KERN_SIZE>(buffer_1[buffer_ptr], buffer_0[buffer_ptr], sobel_y);
}

/** @brief Computes the Gaussian kernel
@param ScalarType Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param gaussian   Stores the gaussian kernel
*/
template <typename ScalarType, vx_uint8 KERN_SIZE>
void ComputeGaussianKernels(ScalarType gaussian[KERN_SIZE][KERN_SIZE]) {
#pragma HLS INLINE

    const vx_int16 ONE = static_cast<vx_int16>(1);
    const vx_int16 THREE = static_cast<vx_int16>(3);

    // Constants and variables
    const vx_int16 BUFFER_SIZE = (((vx_int16)KERN_SIZE - THREE) >> ONE) + ONE;
    vx_int16 buffer_ptr = 0;

    // Gaussian 1d kernel
    ScalarType buffer_0[BUFFER_SIZE][KERN_SIZE];
#pragma HLS array_partition variable = buffer_0 complete dim = 0
    buffer_0[0][0] = 1;
    buffer_0[0][1] = 2;
    buffer_0[0][2] = 1;

    // Computer Gaussian kernel with bigger kernel sizes
    for (vx_uint8 i = 3; i < KERN_SIZE; i += 2) {
#pragma HLS unroll
        ComputeConvolution<ScalarType, KERN_SIZE, BUFFER_SIZE>(buffer_0, buffer_ptr, (vx_int16)i);
        buffer_ptr++;
    }

    // Compute the 2d kernel from the 1d kernel
    ComputeKernel<ScalarType, KERN_SIZE>(buffer_0[buffer_ptr], buffer_0[buffer_ptr], gaussian);
}

/** @brief Computes the normalization with given sum of kernel coefficients
@param kernel_sum Sum of the kernel coefficients
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
void ComputeNormalization(const vx_uint64 kernel_sum, vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

    // Constants
    const vx_uint32 MAX_SHIFT = static_cast<vx_uint32>(48);
    const vx_uint32 MIN_SHIFT = static_cast<vx_uint32>(15);
    const vx_uint64 ONE = static_cast<vx_uint64>(1);
    const vx_uint64 ZERO = static_cast<vx_uint64>(0);

    // Variables
    vx_uint16 new_shift = 0;
    vx_uint16 new_mult = 1;
    vx_uint32 old_shift = 0;
    vx_uint64 old_mult = 1;
    vx_uint64 norm_mult_a = 1;
    vx_uint32 norm_shift_a = 0;
    vx_uint64 norm_mult_b = 1;
    vx_uint32 norm_shift_b = 0;
    vx_uint64 temp = 0;
    bool is_power_two = false;

    // Compute Mult and Shift (Not Power of two)
    old_mult = temp = (ONE << MAX_SHIFT) / kernel_sum;
    for (vx_uint32 i = 0; i < MAX_SHIFT; i++) {
#pragma HLS unroll
        if (temp > ZERO) {
            temp = temp >> static_cast<vx_uint32>(1);
            old_shift = i;
        }
    }
    if (old_shift > 16) {
        new_shift = static_cast<vx_uint16>(MAX_SHIFT - (old_shift - MIN_SHIFT));
        new_mult = static_cast<vx_uint16>(old_mult >> (old_shift - MIN_SHIFT));
    } else {
        new_shift = static_cast<vx_uint16>(MAX_SHIFT);
        new_mult = static_cast<vx_uint16>(old_mult);
    }
    norm_mult_a = static_cast<vx_uint64>(new_mult);
    norm_shift_a = static_cast<vx_uint32>(new_shift);

    // Compute Mult and Shift (Power of two)
    for (vx_uint32 i = 0; i < sizeof(vx_uint32) * 8; i++) {
#pragma HLS unroll
        if ((kernel_sum & (ONE << i)) != ZERO)
            norm_shift_b = i;
    }

    // Check if scaling is by a power of two
    is_power_two = (kernel_sum & (kernel_sum - ONE)) == ZERO;

    // Compute Normalization and Multiplication
    norm_mult = (is_power_two) ? (norm_mult_b) : (norm_mult_a);
    norm_shift = (is_power_two) ? (norm_shift_b) : (norm_shift_a);
}

/** @brief Computes the normalization of a 2d kernel ((a * norm_mult) >> norm_shift)
@param KernType   Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The kernel itself
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
template <typename KernType, vx_uint8 KERN_SIZE>
void ComputeNormalization2d(KernType kernel[KERN_SIZE][KERN_SIZE], vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

    // Variables
    vx_uint64 kernel_sum = 0;

    // Sum the kernel values
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < KERN_SIZE; j++) {
#pragma HLS unroll
            kernel_sum += static_cast<vx_uint64>(ABS(static_cast<vx_int64>(kernel[i][j])));
        }
    }

    // Compute Normalization
    ComputeNormalization(kernel_sum, norm_mult, norm_shift);
}

/** @brief Computes the normalization of a 1d kernel ((a * norm_mult) >> norm_shift)
@param KernType   Data type of the kernel
@param KERN_SIZE  The size of the kernel
@param kernel     The kernel itself
@param norm_mult  For normalization: multiply with this value
@param norm_shift For normalization: shift the multiplied result with this value
*/
template <typename KernType, vx_uint8 KERN_SIZE>
void ComputeNormalization1d(KernType kernel[KERN_SIZE], vx_uint64 &norm_mult, vx_uint32 &norm_shift) {
#pragma HLS INLINE

    // Variables
    vx_uint64 kernel_sum = 0;

    // Sum the kernel values for normalization (pre-compile)
    for (vx_uint8 i = 0; i < KERN_SIZE; i++) {
#pragma HLS unroll
        kernel_sum += static_cast<vx_uint64>(abs(static_cast<vx_int64>(kernel[i])));
    }

    // Compute Normalization
    ComputeNormalization(kernel_sum, norm_mult, norm_shift);
}

#endif // VX_HELPER_H_
