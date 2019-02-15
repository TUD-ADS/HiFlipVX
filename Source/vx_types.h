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
#define VX_DF_IMAGE(a,b,c,d)                  ((a) | (b << 8) | (c << 16) | (d << 24))

/*! \def VX_ATTRIBUTE_BASE
* \brief Defines the manner in which to combine the Vendor and Object IDs to get
* the base value of the enumeration.
* \ingroup group_basic_features
*/
#define VX_ATTRIBUTE_BASE(vendor, object)   (((vendor) << 20) | (object << 8))

/*! \def VX_ENUM_BASE
* \brief Defines the manner in which to combine the Vendor and Object IDs to get
* the base value of the enumeration.
* \details From any enumerated value (with exceptions), the vendor, and enumeration
* type should be extractable. Those types that are exceptions are
* <tt>\ref vx_vendor_id_e</tt>, <tt>\ref vx_type_e</tt>, <tt>\ref vx_enum_e</tt>, <tt>\ref vx_df_image_e</tt>, and \c vx_bool.
* \ingroup group_basic_features
*/
#define VX_ENUM_BASE(vendor, id)            (((vendor) << 20) | (id << 12)) 

/*********************************************************************************************************************/
/* OpenVX typedefs */
/*********************************************************************************************************************/

/*! \brief An 8 bit ASCII character.
* \ingroup group_basic_features
*/
typedef char     vx_char;

/*! \brief An 8-bit unsigned value.
* \ingroup group_basic_features
*/
typedef uint8_t  vx_uint8;

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
typedef int8_t   vx_int8;

/*! \brief A 16-bit signed value.
* \ingroup group_basic_features
*/
typedef int16_t  vx_int16;

/*! \brief A 32-bit signed value.
* \ingroup group_basic_features
*/
typedef int32_t  vx_int32;

/*! \brief A 64-bit signed value.
* \ingroup group_basic_features
*/
typedef int64_t  vx_int64;

/*! \brief A 32-bit float value.
* \ingroup group_basic_features
*/
typedef float    vx_float32;

/*! \brief A 64-bit float value (aka double).
* \ingroup group_basic_features
*/
typedef double   vx_float64;

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

/*! \brief Used to hold a VX_DF_IMAGE code to describe the pixel format and color space.
* \ingroup group_basic_features
*/
typedef uint32_t vx_df_image;

/*! \brief A Boolean value.
* This allows 0 to be FALSE, as it is in C, and any non-zero to be TRUE.
* \code
* vx_bool ret = vx_true_e;
* if (ret) printf("true!\n");
* ret = vx_false_e;
* if (!ret) printf("false!\n");
* \endcode
* This would print both strings.
* \ingroup group_basic_features
*/
typedef enum _vx_bool_e {
	/*! \brief The "false" value. */
	vx_false_e = 0,
	/*! \brief The "true" value. */
	vx_true_e,
} vx_bool;

/*! \brief The type enumeration lists all the known types in OpenVX.
* \ingroup group_basic_features
*/
enum vx_type_e {
	VX_TYPE_INVALID = 0x000,/*!< \brief An invalid type value. When passed an error must be returned. */
	VX_TYPE_CHAR = 0x001,/*!< \brief A <tt>\ref vx_char</tt>. */
	VX_TYPE_INT8 = 0x002,/*!< \brief A <tt>\ref vx_int8</tt>. */
	VX_TYPE_UINT8 = 0x003,/*!< \brief A <tt>\ref vx_uint8</tt>. */
	VX_TYPE_INT16 = 0x004,/*!< \brief A <tt>\ref vx_int16</tt>. */
	VX_TYPE_UINT16 = 0x005,/*!< \brief A <tt>\ref vx_uint16</tt>. */
	VX_TYPE_INT32 = 0x006,/*!< \brief A <tt>\ref vx_int32</tt>. */
	VX_TYPE_UINT32 = 0x007,/*!< \brief A <tt>\ref vx_uint32</tt>. */
	VX_TYPE_INT64 = 0x008,/*!< \brief A <tt>\ref vx_int64</tt>. */
	VX_TYPE_UINT64 = 0x009,/*!< \brief A <tt>\ref vx_uint64</tt>. */
	VX_TYPE_FLOAT32 = 0x00A,/*!< \brief A <tt>\ref vx_float32</tt>. */
	VX_TYPE_FLOAT64 = 0x00B,/*!< \brief A <tt>\ref vx_float64</tt>. */
	VX_TYPE_ENUM = 0x00C,/*!< \brief A <tt>\ref vx_enum</tt>. Equivalent in size to a <tt>\ref vx_int32</tt>. */
	VX_TYPE_SIZE = 0x00D,/*!< \brief A <tt>\ref vx_size</tt>. */
	VX_TYPE_DF_IMAGE = 0x00E,/*!< \brief A <tt>\ref vx_df_image</tt>. */
	VX_TYPE_BOOL = 0x010,/*!< \brief A <tt>\ref vx_bool</tt>. */
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
	VX_ENUM_INTERPOLATION = 0x04, /*!< \brief Interpolation Types. */
	VX_ENUM_CONVERT_POLICY = 0x0A, /*!< \brief Convert Policy. */
	VX_ENUM_THRESHOLD_TYPE = 0x0B, /*!< \brief Threshold Type List. */
	VX_ENUM_BORDER = 0x0C, /*!< \brief Border Mode List. */
	VX_ENUM_ROUND_POLICY = 0x12, /*!< \brief Rounding Policy. */
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

/*! \brief The 2D Coordinates structure. HINT: Updated by template parameter
* \ingroup group_basic_features
*/
template<typename T>
struct vx_coordinates2d_t {
	T x;    /*!< \brief The X coordinate. */
	T y;    /*!< \brief The Y coordinate. */
};

#endif
