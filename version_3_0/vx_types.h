/**
 * Copyright (c) 2012-2017 The Khronos Group Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _OPENVX_TYPES_H_
#define _OPENVX_TYPES_H_

/*!
 * \file vx_types.h
 * \brief The type definitions required by OpenVX Library.
 */

#include <stdint.h>

/*********************************************************************************************************************/
/* OpenVX Macros */
/*********************************************************************************************************************/

/*! \def VX_DF_IMAGE
 * \brief Converts a set of four chars into a \c uint32_t container of a VX_DF_IMAGE code.
 * \note Use a <tt>\ref vx_df_image</tt> variable to hold the value.
 * \ingroup group_basic_features
 */
#define VX_DF_IMAGE(a, b, c, d) ((a) | (b << 8) | (c << 16) | (d << 24))

/*! \def VX_ATTRIBUTE_BASE
 * \brief Defines the manner in which to combine the Vendor and Object IDs to get
 * the base value of the enumeration.
 * \ingroup group_basic_features
 */
#define VX_ATTRIBUTE_BASE(vendor, object) (((vendor) << 20) | (object << 8))

/*! \def VX_ENUM_BASE
 * \brief Defines the manner in which to combine the Vendor and Object IDs to get
 * the base value of the enumeration.
 * \details From any enumerated value (with exceptions), the vendor, and enumeration
 * type should be extractable. Those types that are exceptions are
 * <tt>\ref vx_vendor_id_e</tt>, <tt>\ref vx_type_e</tt>, <tt>\ref vx_enum_e</tt>, <tt>\ref vx_df_image_e</tt>, and \c vx_bool.
 * \ingroup group_basic_features
 */
#define VX_ENUM_BASE(vendor, id) (((vendor) << 20) | (id << 12))

/*********************************************************************************************************************/
/* OpenVX typedefs */
/*********************************************************************************************************************/

/*! \brief An 8 bit ASCII character.
 * \ingroup group_basic_features
 */
typedef char vx_char;

/*! \brief An 8-bit unsigned value.
 * \ingroup group_basic_features
 */
typedef uint8_t vx_uint8;

/*! \brief A 16-bit unsigned value.
 * \ingroup group_basic_features
 */
typedef uint16_t vx_uint16;

/*! \brief A 32-bit unsigned value.
 * \ingroup group_basic_features
 */
typedef uint32_t vx_uint32;

/*! \brief A 64-bit unsigned value.
 * \ingroup group_basic_features
 */
typedef uint64_t vx_uint64;

/*! \brief An 8-bit signed value.
 * \ingroup group_basic_features
 */
typedef int8_t vx_int8;

/*! \brief A 16-bit signed value.
 * \ingroup group_basic_features
 */
typedef int16_t vx_int16;

/*! \brief A 32-bit signed value.
 * \ingroup group_basic_features
 */
typedef int32_t vx_int32;

/*! \brief A 64-bit signed value.
 * \ingroup group_basic_features
 */
typedef int64_t vx_int64;

/*! \brief A 32-bit float value.
 * \ingroup group_basic_features
 */
typedef float vx_float32;

/*! \brief A 64-bit float value (aka double).
 * \ingroup group_basic_features
 */
typedef double vx_float64;

/*! \brief Sets the standard enumeration type size to be a fixed quantity.
 * \details All enumerable fields must use this type as the container to
 * enforce enumeration ranges and sizeof() operations.
 * \ingroup group_basic_features
 */
typedef int32_t vx_enum;

/*! \brief A wrapper of <tt>size_t</tt> to keep the naming convention uniform.
 * \ingroup group_basic_features
 */
typedef size_t vx_size;

/*! \brief A formal boolean type with known fixed size.
 * \see vx_bool_e
 * \ingroup group_basic_features
 */
typedef vx_enum vx_bool;

enum vx_bool_e {
    /*! \brief The "false" value. */
    vx_false_e = 0,
    /*! \brief The "true" value. */
    vx_true_e = 1,
};

/*! \brief The type enumeration lists all the known types in OpenVX.
 * \ingroup group_basic_features
 */
enum vx_type_e {
    VX_TYPE_INVALID = 0x000,  /*!< \brief An invalid type value. When passed an error must be returned. */
    VX_TYPE_CHAR = 0x001,     /*!< \brief A <tt>\ref vx_char</tt>. */
    VX_TYPE_INT8 = 0x002,     /*!< \brief A <tt>\ref vx_int8</tt>. */
    VX_TYPE_UINT8 = 0x003,    /*!< \brief A <tt>\ref vx_uint8</tt>. */
    VX_TYPE_INT16 = 0x004,    /*!< \brief A <tt>\ref vx_int16</tt>. */
    VX_TYPE_UINT16 = 0x005,   /*!< \brief A <tt>\ref vx_uint16</tt>. */
    VX_TYPE_INT32 = 0x006,    /*!< \brief A <tt>\ref vx_int32</tt>. */
    VX_TYPE_UINT32 = 0x007,   /*!< \brief A <tt>\ref vx_uint32</tt>. */
    VX_TYPE_INT64 = 0x008,    /*!< \brief A <tt>\ref vx_int64</tt>. */
    VX_TYPE_UINT64 = 0x009,   /*!< \brief A <tt>\ref vx_uint64</tt>. */
    VX_TYPE_FLOAT32 = 0x00A,  /*!< \brief A <tt>\ref vx_float32</tt>. */
    VX_TYPE_FLOAT64 = 0x00B,  /*!< \brief A <tt>\ref vx_float64</tt>. */
    VX_TYPE_ENUM = 0x00C,     /*!< \brief A <tt>\ref vx_enum</tt>. Equivalent in size to a <tt>\ref vx_int32</tt>. */
    VX_TYPE_SIZE = 0x00D,     /*!< \brief A <tt>\ref vx_size</tt>. */
    VX_TYPE_DF_IMAGE = 0x00E, /*!< \brief A <tt>\ref vx_df_image</tt>. */
    VX_TYPE_BOOL = 0x010,     /*!< \brief A <tt>\ref vx_bool</tt>. */
};

/*********************************************************************************************************************/
/* OpenVX enums (TODO: Change Vendor ID) */
/*********************************************************************************************************************/

/*! \brief The Vendor ID of the Implementation. As new vendors submit their
 * implementations, this enumeration will grow.
 * \ingroup group_basic_features
 */
enum vx_vendor_id_e {
    VX_ID_KHRONOS = 0x000, /*!< \brief The Khronos Group */
};

/*! \brief The set of supported enumerations in OpenVX.
 * \details These can be extracted from enumerated values using <tt>\ref VX_ENUM_TYPE</tt>.
 * \ingroup group_basic_features
 */
enum vx_enum_e {
    VX_ENUM_HINT = 0x02,             /*!< \brief Hint Values. */
    VX_ENUM_INTERPOLATION = 0x04,    /*!< \brief Interpolation Types. */
    VX_ENUM_CONVERT_POLICY = 0x0A,   /*!< \brief Convert Policy. */
    VX_ENUM_THRESHOLD_TYPE = 0x0B,   /*!< \brief Threshold Type List. */
    VX_ENUM_BORDER = 0x0C,           /*!< \brief Border Mode List. */
    VX_ENUM_ROUND_POLICY = 0x12,     /*!< \brief Rounding Policy. */
    VX_ENUM_SCALAR_OPERATION = 0x20, /*!< \brief Scalar operation list. */

    /* HiFlipVX extension */
    VX_ENUM_SCATTER_GATHER_MODE = 0x40 /*!< \brief Scatter Mode. */
};

/*! \brief The Conversion Policy Enumeration.
 * \ingroup group_basic_features
 */
enum vx_convert_policy_e {
    /*! \brief Results are the least significant bits of the output operand, as if
     * stored in two's complement binary format in the size of its bit-depth.
     */
    VX_CONVERT_POLICY_WRAP = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVERT_POLICY) + 0x0,
    /*! \brief Results are saturated to the bit depth of the output operand. */
    VX_CONVERT_POLICY_SATURATE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_CONVERT_POLICY) + 0x1,
};

/*! \brief Based on the VX_DF_IMAGE definition.
 * \note Use <tt>\ref vx_df_image</tt> to contain these values.
 * \ingroup group_basic_features
 */
enum vx_df_image_e {
    /*! \brief A single plane of 24-bit pixel as 3 interleaved 8-bit units of
     * R then G then B data. This uses the BT709 full range by default.
     */
    VX_DF_IMAGE_RGB = VX_DF_IMAGE('R', 'G', 'B', '2'),
    /*! \brief A single plane of 32-bit pixel as 4 interleaved 8-bit units of
     * R then G then B data, then a <i>don't care</i> byte.
     * This uses the BT709 full range by default.
     */
    VX_DF_IMAGE_RGBX = VX_DF_IMAGE('R', 'G', 'B', 'A'),
    /*! \brief A single plane of unsigned 8-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_U8 = VX_DF_IMAGE('U', '0', '0', '8'),
    /*! \brief A single plane of unsigned 8-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_S8 = VX_DF_IMAGE('S', '0', '0', '8'),
    /*! \brief A single plane of signed 16-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_U16 = VX_DF_IMAGE('U', '0', '1', '6'),
    /*! \brief A single plane of signed 16-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_S16 = VX_DF_IMAGE('S', '0', '1', '6'),
    /*! \brief A single plane of unsigned 32-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_U32 = VX_DF_IMAGE('U', '0', '3', '2'),
    /*! \brief A single plane of unsigned 32-bit data.
     * The range of data is not specified, as it may be extracted from a YUV or
     * generated.
     */
    VX_DF_IMAGE_S32 = VX_DF_IMAGE('S', '0', '3', '2'),
};

/*! \brief The Threshold types.
 * \ingroup group_threshold
 */
enum vx_threshold_type_e {
    /*! \brief A threshold with only 1 value. */
    VX_THRESHOLD_TYPE_BINARY = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_THRESHOLD_TYPE) + 0x0,
    /*! \brief A threshold with 2 values (upper/lower). Use with Canny Edge Detection. */
    VX_THRESHOLD_TYPE_RANGE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_THRESHOLD_TYPE) + 0x1,
};

/*! \brief The image reconstruction filters supported by image resampling operations.
 *
 * The edge of a pixel is interpreted as being aligned to the edge of the image.
 * The value for an output pixel is evaluated at the center of that pixel.
 *
 * This means, for example, that an even enlargement of a factor of two in nearest-neighbor
 * interpolation will replicate every source pixel into a 2x2 quad in the destination, and that
 * an even shrink by a factor of two in bilinear interpolation will create each destination pixel
 * by average a 2x2 quad of source pixels.
 *
 * Samples that cross the boundary of the source image have values determined by the border
 * mode - see <tt>\ref vx_border_e</tt> and <tt>\ref VX_NODE_BORDER</tt>.
 * \see vxuScaleImage
 * \see vxScaleImageNode
 * \see VX_KERNEL_SCALE_IMAGE
 * \see vxuWarpAffine
 * \see vxWarpAffineNode
 * \see VX_KERNEL_WARP_AFFINE
 * \see vxuWarpPerspective
 * \see vxWarpPerspectiveNode
 * \see VX_KERNEL_WARP_PERSPECTIVE
 * \ingroup group_basic_features
 */
enum vx_interpolation_type_e {
    /*! \brief Output values are defined to match the source pixel whose center is nearest to the sample position. */
    VX_INTERPOLATION_NEAREST_NEIGHBOR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_INTERPOLATION) + 0x0,
    /*! \brief Output values are defined by bilinear interpolation between the pixels whose centers are closest
     * to the sample position, weighted linearly by the distance of the sample from the pixel centers. */
    VX_INTERPOLATION_BILINEAR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_INTERPOLATION) + 0x1,
    /*! \brief Output values are determined by averaging the source pixels whose areas fall under the
     * area of the destination pixel, projected onto the source image. */
    VX_INTERPOLATION_AREA = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_INTERPOLATION) + 0x2,
    /*
     * Not part of the original OpenVX standard. Compute the down scaled pixel by computing the mean value.
     */
    VX_INTERPOLATION_AREA_FAST = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_INTERPOLATION) + 0x3,
};

/*! \brief The border mode list.
 * \ingroup group_borders
 */
enum vx_border_e {
    /*! \brief No defined border mode behavior is given. */
    VX_BORDER_UNDEFINED = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_BORDER) + 0x0,
    /*! \brief For nodes that support this behavior, a constant value is
     * \e filled-in when accessing out-of-bounds pixels.
     */
    VX_BORDER_CONSTANT = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_BORDER) + 0x1,
    /*! \brief For nodes that support this behavior, a replication of the nearest
     * edge pixels value is given for out-of-bounds pixels.
     */
    VX_BORDER_REPLICATE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_BORDER) + 0x2,
};

/*! \brief The Round Policy Enumeration.
 * \ingroup group_context
 */
enum vx_round_policy_e {
    /*! \brief When scaling, this truncates the least significant values that are lost in operations. */
    VX_ROUND_POLICY_TO_ZERO = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_ROUND_POLICY) + 0x1,
    /*! \brief When scaling, this rounds to nearest even output value. */
    VX_ROUND_POLICY_TO_NEAREST_EVEN = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_ROUND_POLICY) + 0x2,
};

/*! \brief These enumerations are given to the <tt>\ref vxHint</tt> API to enable/disable platform
 * optimizations and/or features. Hints are optional and usually are vendor-specific.
 * \see <tt>\ref vxHint</tt>
 * \ingroup group_hint
 */
enum vx_hint_e {
    /*! \brief Indicates to the implementation that user do not apply any specific
     *  requirements for performance.
     */
    VX_HINT_PERFORMANCE_DEFAULT = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_HINT) + 0x1,
    /*! \brief Indicates the user preference is low power consumption versus
     * highest performance.
     */
    VX_HINT_PERFORMANCE_LOW_POWER = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_HINT) + 0x2,
    /*! \brief Indicates the user preference for highest performance over
     * low power consumption.
     */
    VX_HINT_PERFORMANCE_HIGH_SPEED = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_HINT) + 0x3,
};

/*! \brief The 2D Coordinates structure. HINT: Updated by template parameter
 * \ingroup group_basic_features
 */
template <typename T> struct vx_coordinates2d_t {
    T x; /*!< \brief The X coordinate. */
    T y; /*!< \brief The Y coordinate. */
};

enum vx_scalar_operation_e {
    VX_SCALAR_OP_AND = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x0,
    VX_SCALAR_OP_OR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x1,
    VX_SCALAR_OP_XOR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x2,
    VX_SCALAR_OP_NAND = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x3,
    VX_SCALAR_OP_EQUAL = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x4,
    VX_SCALAR_OP_NOTEQUAL = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x5,
    VX_SCALAR_OP_LESS = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x6,
    VX_SCALAR_OP_LESSEQ = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x7,
    VX_SCALAR_OP_GREATER = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x8,
    VX_SCALAR_OP_GREATEREQ = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x9,
    VX_SCALAR_OP_ADD = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xA,
    VX_SCALAR_OP_SUBTRACT = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xB,
    VX_SCALAR_OP_MULTIPLY = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xC,
    VX_SCALAR_OP_DIVIDE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xD,
    VX_SCALAR_OP_MODULUS = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xE,
    VX_SCALAR_OP_MIN = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0xF,
    VX_SCALAR_OP_MAX = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_SCALAR_OPERATION) + 0x10,
};

/*********************************************************************************************************************/
/* OpenVX NN enums  */
/*********************************************************************************************************************/

/*! \brief NN extension type enums.
 * \ingroup group_cnn
 */
enum vx_nn_enum_e {
    VX_ENUM_NN_ROUNDING_TYPE = 0x1A,
    VX_ENUM_NN_POOLING_TYPE = 0x1B,
    VX_ENUM_NN_NORMALIZATION_TYPE = 0x1C,
    VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE = 0x1D,
};

/*! \brief down scale rounding.
 * \details Due to different scheme of downscale size calculation in the various training frameworks. Implementation must support 2 rounding methods for down
 * scale calculation. The floor and the ceiling. In convolution and pooling functions. Relevant when input size is even. \ingroup group_cnn
 */
enum vx_nn_rounding_type_e {
    /*! \brief floor rounding  */
    VX_NN_DS_SIZE_ROUNDING_FLOOR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ROUNDING_TYPE) + 0x0,
    /*! \brief ceil rounding */
    VX_NN_DS_SIZE_ROUNDING_CEILING = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ROUNDING_TYPE) + 0x1
};

/*! \brief The Neural Network pooling type list.
 * \details kind of pooling done in pooling function
 * \ingroup group_cnn
 */
enum vx_nn_pooling_type_e {
    /*! \brief max pooling*/
    VX_NN_POOLING_MAX = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_POOLING_TYPE) + 0x0,
    /*! \brief average pooling*/
    VX_NN_POOLING_AVG = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_POOLING_TYPE) + 0x1
};

/*! \brief The Neural Network activation functions list.
 * \details
 * <table>
 * <tr><td> <B>Function name </B> <td> <B>Mathematical definition</B> <td> <B>Parameters</B> <td> <B>Parameters type</B>
 * <tr><td>logistic <td> \f$f(x)=1/(1+e^{-x}) \f$  <td>  <td>
 * <tr><td>hyperbolic tangent <td> \f$f(x)=a\cdot tanh(b\cdot x) \f$  <td> a,b  <td> VX_FLOAT32
 * <tr><td>relu <td> \f$f(x)=max(0,x)\f$  <td>  <td>
 * <tr><td>bounded relu <td> \f$f(x)=min(a,max(0,x)) \f$  <td> a  <td> VX_FLOAT32
 * <tr><td>soft relu <td> \f$f(x)=log(1+e^{x}) \f$  <td>  <td>
 * <tr><td>abs <td> \f$f(x)=\mid x\mid \f$  <td>  <td>
 * <tr><td>square <td> \f$f(x)= x^2 \f$  <td>  <td>
 * <tr><td>square root <td> \f$f(x)=\sqrt{x} \f$  <td>  <td>
 * <tr><td>linear <td> \f$f(x)=ax+b \f$  <td>  a,b  <td> VX_FLOAT32
 * </table>
 * \ingroup group_cnn
 */
enum vx_nn_activation_function_e {
    VX_NN_ACTIVATION_LOGISTIC = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x0,
    VX_NN_ACTIVATION_HYPERBOLIC_TAN = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x1,
    VX_NN_ACTIVATION_RELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x2,
    VX_NN_ACTIVATION_BRELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x3,
    VX_NN_ACTIVATION_SOFTRELU = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x4,
    VX_NN_ACTIVATION_ABS = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x5,
    VX_NN_ACTIVATION_SQUARE = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x6,
    VX_NN_ACTIVATION_SQRT = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x7,
    VX_NN_ACTIVATION_LINEAR = VX_ENUM_BASE(VX_ID_KHRONOS, VX_ENUM_NN_ACTIVATION_FUNCTION_TYPE) + 0x8,
};

#endif
