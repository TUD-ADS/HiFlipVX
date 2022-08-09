/**
 * Licence: GNU GPLv3 \n
 * You may copy, distribute and modify the software as long as you track
 * changes/dates in source files. Any modifications to or software
 * including (via compiler) GPL-licensed code must also be made available
 * under the GPL along with build & install instructions.
 *
 * @file    img_mt_helper.h
 * @author  Matthias Nickel <matthias.nickel@tu-dresden.de>
 * @version 3.0
 * @brief Description:\n
 *  These are helper functions needed by different functions.
 */

#ifndef __SRC_IMG_MT_HELPER_H__
#define __SRC_IMG_MT_HELPER_H__

#include "vx_helper.h"

#ifdef __SYNTHESIS__
#define INLINE inline __attribute__((always_inline))
#else
#define INLINE inline
#endif

#define BYTES_IN_BITS(VAL) ((VAL) << 3)

#define VX_AP_MAX(A, B) ((A < B) ? (B) : (A))
#define VX_AP_MIN(A, B) ((A < B) ? (A) : (B))

// next larger unsigned data type
template <typename TYPE> struct next_larger_unsigned;

// next larger signed data type
template <typename TYPE> struct next_larger_signed;

// next smaller unsigned data type
template <typename TYPE> struct next_smaller_unsigned;

template <typename TYPE> struct next_smaller_signed;

// next larger data type of signed or unsigned
template <typename TYPE> struct next_larger;

// next smaller data type of signed or unsigned
template <typename TYPE> struct next_smaller;

// change input data type to signed variant of it
template <typename TYPE> struct to_signed;
// change input data type to unsigned variant of it
template <typename TYPE> struct to_unsigned;

#define NEXT_LARGER_UNSIGNED(S, T)                                                                                     \
    template <> struct next_larger_unsigned<S> { typedef T DataType; };

NEXT_LARGER_UNSIGNED(vx_uint8, vx_uint16)
NEXT_LARGER_UNSIGNED(vx_uint16, vx_uint32)
NEXT_LARGER_UNSIGNED(vx_uint32, vx_uint64)
NEXT_LARGER_UNSIGNED(vx_uint64, vx_uint64)
NEXT_LARGER_UNSIGNED(vx_int8, vx_uint16)
NEXT_LARGER_UNSIGNED(vx_int16, vx_uint32)
NEXT_LARGER_UNSIGNED(vx_int32, vx_uint64)
NEXT_LARGER_UNSIGNED(vx_int64, vx_uint64)

#undef NEXT_LARGER_UNSIGNED

#define NEXT_LARGER_SIGNED(S, T)                                                                                       \
    template <> struct next_larger_signed<S> { typedef T DataType; };

NEXT_LARGER_SIGNED(vx_uint8, vx_int16)
NEXT_LARGER_SIGNED(vx_uint16, vx_int32)
NEXT_LARGER_SIGNED(vx_uint32, vx_int64)
NEXT_LARGER_SIGNED(vx_uint64, vx_int64)
NEXT_LARGER_SIGNED(vx_int8, vx_int16)
NEXT_LARGER_SIGNED(vx_int16, vx_int32)
NEXT_LARGER_SIGNED(vx_int32, vx_int64)
NEXT_LARGER_SIGNED(vx_int64, vx_int64)

#undef NEXT_LARGER_SIGNED

#define NEXT_SMALLER_UNSIGNED(S, T)                                                                                    \
    template <> struct next_smaller_unsigned<S> { typedef T DataType; };

NEXT_SMALLER_UNSIGNED(vx_uint8, vx_uint8)
NEXT_SMALLER_UNSIGNED(vx_uint16, vx_uint8)
NEXT_SMALLER_UNSIGNED(vx_uint32, vx_uint16)
NEXT_SMALLER_UNSIGNED(vx_uint64, vx_uint32)
NEXT_SMALLER_UNSIGNED(vx_int8, vx_uint8)
NEXT_SMALLER_UNSIGNED(vx_int16, vx_uint8)
NEXT_SMALLER_UNSIGNED(vx_int32, vx_uint16)
NEXT_SMALLER_UNSIGNED(vx_int64, vx_uint32)

#undef NEXT_SMALLER_UNSIGNED

#define NEXT_SMALLER_SIGNED(S, T)                                                                                      \
    template <> struct next_smaller_signed<S> { typedef T DataType; };

NEXT_SMALLER_SIGNED(vx_uint8, vx_int8)
NEXT_SMALLER_SIGNED(vx_uint16, vx_int8)
NEXT_SMALLER_SIGNED(vx_uint32, vx_int16)
NEXT_SMALLER_SIGNED(vx_uint64, vx_int32)
NEXT_SMALLER_SIGNED(vx_int8, vx_int8)
NEXT_SMALLER_SIGNED(vx_int16, vx_int8)
NEXT_SMALLER_SIGNED(vx_int32, vx_int16)
NEXT_SMALLER_SIGNED(vx_int64, vx_int32)

#undef NEXT_SMALLER_SIGNED

#define NEXT_LARGER(S, T)                                                                                              \
    template <> struct next_larger<S> { typedef T DataType; };

NEXT_LARGER(vx_uint8, vx_uint16)
NEXT_LARGER(vx_uint16, vx_uint32)
NEXT_LARGER(vx_uint32, vx_uint64)
NEXT_LARGER(vx_uint64, vx_uint64)
NEXT_LARGER(vx_int8, vx_int16)
NEXT_LARGER(vx_int16, vx_int32)
NEXT_LARGER(vx_int32, vx_int64)
NEXT_LARGER(vx_int64, vx_int64)

#undef NEXT_LARGER

#define NEXT_SMALLER(S, T)                                                                                             \
    template <> struct next_smaller<S> { typedef T DataType; };

NEXT_SMALLER(vx_uint8, vx_uint8)
NEXT_SMALLER(vx_uint16, vx_uint8)
NEXT_SMALLER(vx_uint32, vx_uint16)
NEXT_SMALLER(vx_uint64, vx_uint32)
NEXT_SMALLER(vx_int8, vx_int8)
NEXT_SMALLER(vx_int16, vx_int8)
NEXT_SMALLER(vx_int32, vx_int16)
NEXT_SMALLER(vx_int64, vx_int32)

#undef NEXT_SMALLER

#define TO_UNSIGNED(S, T)                                                                                              \
    template <> struct to_unsigned<S> { typedef T DataType; };

TO_UNSIGNED(vx_uint8, vx_uint8)
TO_UNSIGNED(vx_uint16, vx_uint16)
TO_UNSIGNED(vx_uint32, vx_uint32)
TO_UNSIGNED(vx_uint64, vx_uint64)
TO_UNSIGNED(vx_int8, vx_uint8)
TO_UNSIGNED(vx_int16, vx_uint16)
TO_UNSIGNED(vx_int32, vx_uint32)
TO_UNSIGNED(vx_int64, vx_uint64)

#undef TO_UNSIGNED

#define TO_SIGNED(S, T)                                                                                                \
    template <> struct to_signed<S> { typedef T DataType; };

TO_SIGNED(vx_uint8, vx_int8)
TO_SIGNED(vx_uint16, vx_int16)
TO_SIGNED(vx_uint32, vx_int32)
TO_SIGNED(vx_uint64, vx_int64)
TO_SIGNED(vx_int8, vx_int8)
TO_SIGNED(vx_int16, vx_int16)
TO_SIGNED(vx_int32, vx_int32)
TO_SIGNED(vx_int64, vx_int64)

#undef TO_SIGNED

/*
 * Compute the binary logarithm. Can be computed at compile time.
 */
INLINE vx_uint8 int_floor_log2(const vx_uint64 value) {

    const vx_uint8 MAX_NUMBER_OF_BITS = 64;

    vx_uint8 ld = 0;
    vx_uint64 v = value;

    for (vx_uint8 i = 0; i < MAX_NUMBER_OF_BITS; i++) {
#pragma HLS unroll
        ld += (v > 1) ? (1) : (0);
        v >>= 1;
    }

    return ld;
}

INLINE vx_uint8 int_ceil_log2(const vx_uint64 value) {
    const vx_uint8 ld = int_floor_log2(value);
    const vx_uint64 v = static_cast<vx_uint64>(1) << ld;
    return (value - v > 0) ? (ld + 1) : (ld);
}

/*
 * Compute binary definitely at compile time, so it can be used for template parameter.
 */

template <vx_uint64 VAL> struct comp_log2;

template <> struct comp_log2<0> {
    enum {
        log2_floor = 0,
        log2_ceil = 0,
    };
};

template <> struct comp_log2<1> {
    enum {
        log2_floor = 0,
        log2_ceil = 0,
    };
};

template <vx_uint64 VAL> struct comp_log2 {
    enum {
        log2_floor = 1 + comp_log2<(VAL >> 1)>::log2_floor,
        log2_ceil = (VAL - (1 << log2_floor) == 0) ? (log2_floor) : (log2_floor + 1),
    };
};

// convert floating point fraction to integer
template <typename OutType> INLINE OutType floatToFraction(const float value) {

    typedef typename next_larger<OutType>::DataType nextLargerT;
    /*
     * Shift 1 to the next highest bit position which is not support by the data type of OutType
     * and multiply it with the input value of type float to get the fraction into the integer range.
     * E.g. OuType is of vx_uint8 and value is 0.75f
     * 1 << 8 -->  0000 ... 0000 0001 0000 0000 = 256
     * 256 * 0.75f = 192 = 1100 0000
     *
     * Support of vx_uint8, vx_uint16 and vx_uint32.
     */
    return static_cast<OutType>((static_cast<nextLargerT>(1) << (sizeof(OutType) << 3)) * value);
}

template <typename OutType, vx_uint8 FRACTION_BITS> INLINE OutType floatToFraction(const float value) {

    typedef typename next_larger<OutType>::DataType nextLargerT;
    return static_cast<OutType>((static_cast<nextLargerT>(1) << FRACTION_BITS) * value);
}

// convert fraction (of integer type) to floating point
template <typename InType> INLINE float fractionToFloat(const InType value) {

    typedef typename next_larger<InType>::DataType nextLargerT;

    /*
     * Shift 1 to the next highest bit position which is not support by the data type of InType
     * and divide the input (value) which represents a fraction using a integer data type with it to convert the
     * fraction to a floating point representation. E.g. InType is of uint8 and value is 192 (1100 0000) representing
     * 0.75 1 << 8 -->  0000 ... 0000 0001 0000 0000 = 256 192f / 256f = 0.75f
     *
     * Support of vx_uint8, vx_uint16 and vx_uint32.
     */
    return static_cast<float>(static_cast<float>(value) /
                              static_cast<float>(static_cast<nextLargerT>(1) << (sizeof(InType) << 3)));
}

template <typename InType, vx_uint8 FRACTION_BITS> INLINE float fractionToFloat(const InType value) {

    typedef typename next_larger<InType>::DataType nextLargerT;
    return static_cast<float>(static_cast<float>(value) /
                              static_cast<float>(static_cast<nextLargerT>(1) << FRACTION_BITS));
}

template <typename DataType> INLINE typename to_unsigned<DataType>::DataType abs_int(const DataType value) {

    typedef typename to_unsigned<DataType>::DataType unsignedDataType;
    return static_cast<unsignedDataType>((value < 0) ? unsignedDataType(-value) : unsignedDataType(value));
}

/*
 * Checks if signed shift is supported and chooses shift operation accordingly.
 * Right shifts with signed value is undefined by the standard, therefore its not ensured how the compiler implements
 * this case.
 */
template <typename T> INLINE T signedRightShift(const T value, const vx_uint8 shift) {

#ifdef NO_SIGNED_SHIFT
    typedef typename to_unsigned<T>::DataType unsignedT;
    const unsignedT result = abs_int(value) >> shift;
    const vx_bool sign = (value < 0);
    return (sign) ? (-static_cast<T>(result)) : (static_cast<T>(result));
#else
    return (value >> shift);
#endif
}

KeyPoint createKeypoint(const vx_uint16 x, const vx_uint16 y, const vx_uint16 response, const vx_uint8 scale,
                        const vx_uint8 orientation) {
#pragma HLS INLINE

#ifdef __SYNTHESIS__
    return (KeyPoint){x, y, response, scale, orientation};
#else
    return {x, y, response, scale, orientation};
#endif
}

template <typename DataType, vx_uint8 VEC_NUM>
void readImageData(DataType dst[VEC_NUM], const vx_image_data<DataType, VEC_NUM> src) {
#pragma HLS INLINE
    for (vx_uint8 i = 0; i < VEC_NUM; i++) {
#pragma HLS unroll
        dst[i] = src.pixel[i];
    }
}

template <typename DataType, vx_uint8 VEC_NUM>
void writeImageData(vx_image_data<DataType, VEC_NUM> dst, const DataType src[VEC_NUM]) {
#pragma HLS INLINE
    for (vx_uint8 i = 0; i < VEC_NUM; i++) {
#pragma HLS unroll
        dst.pixel[i] = src[i];
    }
}

template <typename DataType, vx_uint8 VEC_NUM> void copyVector(DataType dst[VEC_NUM], const DataType src[VEC_NUM]) {
#pragma HLS INLINE
    for (vx_uint8 i = 0; i < VEC_NUM; i++) {
#pragma HLS unroll
        dst[i] = src[i];
    }
}

/*
 * Define unsigned and signed arbitrary precision (up to 64 bits) data types
 */
template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING> struct vx_ap_int;

template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING> struct vx_ap_uint;

/*
 * Select smallest possible for return value
 */
template <vx_uint8 BYTES, vx_bool IS_SIGNED> struct vx_ap_select_smallest_data_type;

#define VX_AP_SELECT_SMALLEST_DATA_TYPE(BYTES, SIGN, TYPE)                                                             \
    template <> struct vx_ap_select_smallest_data_type<BYTES, SIGN> { typedef TYPE DataType; };

// up to 8 bits
VX_AP_SELECT_SMALLEST_DATA_TYPE(1, true, vx_int8)
VX_AP_SELECT_SMALLEST_DATA_TYPE(1, false, vx_uint8)
// up to 16 bits
VX_AP_SELECT_SMALLEST_DATA_TYPE(2, true, vx_int16)
VX_AP_SELECT_SMALLEST_DATA_TYPE(2, false, vx_uint16)
// up to 32 bits
VX_AP_SELECT_SMALLEST_DATA_TYPE(3, true, vx_int32)
VX_AP_SELECT_SMALLEST_DATA_TYPE(3, false, vx_uint32)
VX_AP_SELECT_SMALLEST_DATA_TYPE(4, true, vx_int32)
VX_AP_SELECT_SMALLEST_DATA_TYPE(4, false, vx_uint32)
// up to 64 bits
VX_AP_SELECT_SMALLEST_DATA_TYPE(5, true, vx_int64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(5, false, vx_uint64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(6, true, vx_int64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(6, false, vx_uint64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(7, true, vx_int64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(7, false, vx_uint64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(8, true, vx_int64)
VX_AP_SELECT_SMALLEST_DATA_TYPE(8, false, vx_uint64)

#undef VX_AP_RETURN_TYPE
/*
 * Defines data type factories to build vx_ap_int and vx_ap_uint.
 */
template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING, vx_bool IS_SIGNED> struct vx_ap_int_factory;

template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING> struct vx_ap_int_factory<BIT_WIDTH, WITH_ROUNDING, vx_true_e> {
    typedef vx_ap_int<BIT_WIDTH, WITH_ROUNDING> DataType;
};

template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING>
struct vx_ap_int_factory<_BIT_WIDTH, _WITH_ROUNDING, vx_false_e> {
    typedef vx_ap_uint<_BIT_WIDTH, _WITH_ROUNDING> DataType;
};

template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING, vx_bool IS_SIGNED> struct vx_ap_base {

#define VX_AP_MAX_BIT_WIDTH 64

#define VX_AP_DATA_TYPE_BIT_WIDTH(TYPE) (sizeof(TYPE) << 3)

#define VX_AP_BASE_STD_RHS_TEMPLATE_LIST template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING, vx_bool _IS_SIGNED>
#define VX_AP_BASE_STD_RHS_DATA_TYPE vx_ap_base<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>
#define VX_AP_BASE_STD_OWN_DATA_TYPE vx_ap_base<BIT_WIDTH, WITH_ROUNDING, IS_SIGNED>

    STATIC_ASSERT((BIT_WIDTH <= VX_AP_MAX_BIT_WIDTH), maximum_64_bits_allowed);

    /*
     * Defines if used 64 bit data type is signed or not.
     */
    typedef typename vx_ap_select_smallest_data_type<((BIT_WIDTH + 7) >> 3), IS_SIGNED>::DataType vx_ap_baseT;
    typedef typename to_unsigned<vx_ap_baseT>::DataType vx_ap_base_roundT;
    typedef typename vx_ap_int_factory<BIT_WIDTH, WITH_ROUNDING, IS_SIGNED>::DataType vx_ap_derivT;

    vx_ap_baseT value;

    /*
     * Adapted versions of the clearUnusedBits orginating from Xilinx's ap_private
     */
    INLINE void clearUnusedBits() {
        enum {
            excess_bits = (BIT_WIDTH % VX_AP_MAX_BIT_WIDTH) ? VX_AP_MAX_BIT_WIDTH - BIT_WIDTH % VX_AP_MAX_BIT_WIDTH : 0,
            unsigned_mask = static_cast<vx_uint64>(-1) >> (VX_AP_MAX_BIT_WIDTH - BIT_WIDTH),
        };
        value = (vx_ap_baseT)((IS_SIGNED) ? ((((vx_int64)value) << (excess_bits)) >> (excess_bits))
                                          : value & unsigned_mask);
    }

    INLINE void clearUnusedBits(vx_ap_baseT v) {
        enum {
            excess_bits = (BIT_WIDTH % VX_AP_MAX_BIT_WIDTH) ? VX_AP_MAX_BIT_WIDTH - BIT_WIDTH % VX_AP_MAX_BIT_WIDTH : 0,
            unsigned_mask = static_cast<vx_uint64>(-1) >> (VX_AP_MAX_BIT_WIDTH - BIT_WIDTH)
        };
        value = (vx_ap_baseT)((IS_SIGNED) ? ((((vx_int64)v) << (excess_bits)) >> (excess_bits)) : v & unsigned_mask);
    }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    struct ReturnType {
        enum {
            /*
             * Compute new bit width of binary operations. But restrict to most 64 bits. Meaning the user hast to take
             * care that the new bit width does not exceed the the maximum supported bitwidth.
             */
            add_bit_width = VX_AP_MIN(VX_AP_MAX_BIT_WIDTH, VX_AP_MAX(BIT_WIDTH, _BIT_WIDTH) + 1),
            // if
            sub_bit_width = VX_AP_MIN(VX_AP_MAX_BIT_WIDTH, VX_AP_MAX(BIT_WIDTH, _BIT_WIDTH) + (IS_SIGNED | _IS_SIGNED)),
            mul_bit_width = VX_AP_MIN(VX_AP_MAX_BIT_WIDTH, BIT_WIDTH + _BIT_WIDTH),
            // if the denominator is signed save the signed bit
            div_bit_width = VX_AP_MIN(VX_AP_MAX_BIT_WIDTH, BIT_WIDTH + _IS_SIGNED),
            // if the right-hand side operand has a smaller bit width, the returned data type uses its bit width (also
            // consider signed bit of left hand side operand)
            mod_bit_width = VX_AP_MIN(BIT_WIDTH, _BIT_WIDTH + (IS_SIGNED & !_IS_SIGNED)),
            // if they operand with the smaller bit width is signed, than add a bit to save the signed bit.
            logic_bit_width = VX_AP_MIN(VX_AP_MAX_BIT_WIDTH, VX_AP_MAX(BIT_WIDTH + (_IS_SIGNED & !IS_SIGNED),
                                                                       _BIT_WIDTH + (IS_SIGNED & !_IS_SIGNED))),
            /*
             * Determine if the returned data type of a binary operation is signed or not.
             */

            // if one of the data types is signed return signed (only exception is the result of a subtraction which
            // always will be considered as a signed data type)
            add_sign = IS_SIGNED | _IS_SIGNED,
            sub_sign = vx_true_e,
            mul_sign = IS_SIGNED | _IS_SIGNED,
            div_sign = IS_SIGNED | _IS_SIGNED,
            mod_sign = IS_SIGNED,
            logic_sign = IS_SIGNED | _IS_SIGNED,

            /*
             * If at least one of the operand supports rounding than does the resulting data type also support rounding.
             */
            with_rounding = WITH_ROUNDING | _WITH_ROUNDING

        };

        /*
         * Define return data type depending on operation
         */
        typedef typename vx_ap_int_factory<add_bit_width, with_rounding, add_sign>::DataType addT;
        typedef typename vx_ap_int_factory<sub_bit_width, with_rounding, sub_sign>::DataType subT;
        typedef typename vx_ap_int_factory<mul_bit_width, with_rounding, mul_sign>::DataType mulT;
        typedef typename vx_ap_int_factory<div_bit_width, with_rounding, div_sign>::DataType divT;
        typedef typename vx_ap_int_factory<mod_bit_width, with_rounding, mod_sign>::DataType modT;
        typedef typename vx_ap_int_factory<logic_bit_width, with_rounding, logic_sign>::DataType logicT;
    };

#define SUPPORTED_CTOR_DATA_TYPES(FCT)                                                                                 \
    FCT(vx_uint8)                                                                                                      \
    FCT(vx_uint16)                                                                                                     \
    FCT(vx_uint32)                                                                                                     \
    FCT(vx_uint64)                                                                                                     \
    FCT(vx_int8)                                                                                                       \
    FCT(vx_int16)                                                                                                      \
    FCT(vx_int32)                                                                                                      \
    FCT(vx_int64)

    //#define OP_ASSIGN_BODY(VAL) (value = VAL & bit_mask)

    /*
     * Constructors
     * Note OP_ASSIGN_BODY applies bit mask
     */

    /*
     * Default constructor
     */

    INLINE vx_ap_base() { clearUnusedBits(); }

#define CTOR(TYPE)                                                                                                     \
    INLINE vx_ap_base(TYPE _value) {                                                                                   \
        /*OP_ASSIGN_BODY(_value);*/                                                                                    \
        clearUnusedBits(_value);                                                                                       \
    }

    /*
     * Create constructors for primitive data types
     */
    SUPPORTED_CTOR_DATA_TYPES(CTOR)

    /*
     * Copy constructor.
     */
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_base(const VX_AP_BASE_STD_RHS_DATA_TYPE &p) {
        // OP_ASSIGN_BODY(p.value);
        clearUnusedBits(p.value);
    }

    /*
     * End constructor
     */

    /*
     *  Assignment operation
     *  note OP_ASSIGN_BODY calls getMask
     */

#define OP_ASSIGN(TYPE)                                                                                                \
    INLINE void operator=(const TYPE rhs) {                                                                            \
        /*OP_ASSIGN_BODY(rhs);*/                                                                                       \
        clearUnusedBits(rhs);                                                                                          \
    }

    SUPPORTED_CTOR_DATA_TYPES(OP_ASSIGN)

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_BASE_STD_RHS_DATA_TYPE &rhs) const {
        // OP_ASSIGN_BODY(rhs.value);
        clearUnusedBits(rhs.value);
    }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_BASE_STD_RHS_DATA_TYPE &rhs) {
        // OP_ASSIGN_BODY(rhs.value);
        clearUnusedBits(rhs.value);
    }

/*
 * end assignment operation
 */

/*
 * Arithmetic operations
 * note: mask called by constructor
 */

/*
 * Applies binary operation.
 * OP...	binary operator
 * VAL...	value of right-hand side operator
 */
// use optimization for arithmetic operations
#ifdef __XILINX__

// use Xilinx math division for optimization if possible
#define XILINX_OP_DIV(LHS, RHS) hls::divide(LHS, RHS)

// call any in XOP defined specialized operation
#define XILINX_OP(XOP, _BIT_WIDTH, _IS_SIGNED, VAL, RES)                                                               \
    const vx_uint8 MAX_BITS = VX_AP_MAX(BIT_WIDTH, _BIT_WIDTH);                                                        \
    const vx_bool SIGNED = IS_SIGNED | _IS_SIGNED;                                                                     \
    if (SIGNED) {                                                                                                      \
        RES = XOP(ap_int<MAX_BITS>(value), ap_int<MAX_BITS>(VAL)).to_int();                                            \
        /*ap_uint if unsigned*/                                                                                        \
    } else {                                                                                                           \
        RES = XOP(ap_uint<MAX_BITS>(value), ap_uint<MAX_BITS>(VAL)).to_uint();                                         \
    }
#endif /* __XILINX__ */

/*
 * Binary operation which cast to the next higher data type to avoid overflow
 */
#define BIN_OP_OVERFLOW(OP, RES, LHS, RHS, LHS_SIGN, RHS_SIGN)                                                         \
    /*Cast to the next highest primitive data type to avoid overflow during calculation.*/                             \
    typedef typename vx_ap_select_smallest_data_type<((VX_AP_DATA_TYPE_BIT_WIDTH(LHS) + 1) >> 3), LHS_SIGN>::DataType  \
        nl_lhsT;                                                                                                       \
    typedef typename vx_ap_select_smallest_data_type<((VX_AP_DATA_TYPE_BIT_WIDTH(RHS) + 1) >> 3), RHS_SIGN>::DataType  \
        nl_rhsT;                                                                                                       \
    RES = static_cast<nl_lhsT>(LHS) OP static_cast<nl_rhsT>(RHS);

/*
 * Binary operation without overflow prevention
 */
#define BIN_OP(OP, RES, LHS, RHS) RES = LHS OP RHS;

/*
 * Creates a binary operation function for primitive data types on the right-hand side of the operation and returns new
 * created instance/value of this operation. OP... 		binary arithmetic or logic operator
 * RET_TYPE		data type of return value (defined by the arithmetic operation, e.g. addT)
 * FCT_ATTR...	special function attributes, like const
 * RHS_TYPE...	data type of right-hand side operand
 * RHS_BW...	bit width of right-hand side operand
 * RHS_ROUND...	defines if right-hand side operand supports rounding
 * RHS_SIGN...	defines if right-hand side operand a signed or unsigned type
 */
#define PRIM_BIN_OP_FCT_RHS(OP, RET_TYPE, FCT_ATTR, RHS_TYPE, RHS_BW, RHS_ROUND, RHS_SIGN)                             \
    INLINE typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<RHS_BW, RHS_ROUND, RHS_SIGN>::RET_TYPE           \
    operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                               \
        typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<RHS_BW, RHS_ROUND, RHS_SIGN>::RET_TYPE res;         \
        BIN_OP_OVERFLOW(OP, res, value, rhs, IS_SIGNED, RHS_SIGN)                                                      \
        return res;                                                                                                    \
    }

/*
 * Creates a binary operation function for primitive data types on the lhs-hand side of the operation and returns new
 * created instance/value of this operation. OP... 		binary arithmetic or logic operator
 * RET_TYPE		data type of return value (defined by the arithmetic operation, e.g. addT)
 * LHS_TYPE...	data type of left-hand side operand
 * LHS_BW...	bit width of left-hand side operand
 * LHS_ROUND...	defines if left-hand side operand supports rounding
 * LHS_SIGN...	defines if left-hand side operand a signed or unsigned type
 */
#define PRIM_BIN_OP_FCT_LHS(OP, RET_TYPE, LHS_TYPE, LHS_BW, LHS_ROUND, LHS_SIGN)                                       \
    INLINE friend typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<LHS_BW, LHS_ROUND, LHS_SIGN>::RET_TYPE    \
    operator OP(LHS_TYPE lhs, VX_AP_BASE_STD_OWN_DATA_TYPE rhs) {                                                      \
        typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<LHS_BW, LHS_ROUND, LHS_SIGN>::RET_TYPE res;         \
        BIN_OP_OVERFLOW(OP, res, lhs, rhs.value, LHS_SIGN, IS_SIGNED)                                                  \
        return res;                                                                                                    \
    }

/*
 * Creates a binary operation function for primitive data types on the lhs-hand side of the operation and returns new
 * created instance/value of this operation. OP... 			binary arithmetic or logic operator RET_TYPE
 * data type of return value (defined by the arithmetic operation, e.g. addT) FCT_ATTR...		special function
 * attributes, like const PRIM_TYPE...		data type of right-/left-hand side operand
 * PRIM_BW...		bit width of right-/left-hand side operand
 * PRIM_ROUND...	defines if right-/left-hand side operand supports rounding
 * PRIM_SIGN...		defines if right-/left-hand side operand a signed or unsigned type
 */
#define PRIM_BIN_OP_FCT(OP, RET_TYPE, FCT_ATTR, PRIM_TYPE, PRIM_BW, PRIM_ROUND, PRIM_SIGN)                             \
    PRIM_BIN_OP_FCT_RHS(OP, RET_TYPE, FCT_ATTR, PRIM_TYPE, PRIM_BW, PRIM_ROUND, PRIM_SIGN)                             \
    PRIM_BIN_OP_FCT_LHS(OP, RET_TYPE, PRIM_TYPE, PRIM_BW, PRIM_ROUND, PRIM_SIGN)

#ifdef __XILINX__
/*
 * use Xilinx operations for optimization if possible
 * additional parameter:
 * XOP... name of macro with specialized operation called, e.g. XILINX_OP_DIV
 */
#define PRIM_BIN_XILINX_OP_FCT(OP, XOP, RET_TYPE, FCT_ATTR, RHS_TYPE, RHS_BW, RHS_ROUND, RHS_SIGN)                     \
    INLINE typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<RHS_BW, RHS_ROUND, RHS_SIGN>::RET_TYPE           \
    operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                               \
        typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<RHS_BW, RHS_ROUND, RHS_SIGN>::RET_TYPE res;         \
        XILINX_OP(XOP, RHS_BW, RHS_SIGN, rhs, res)                                                                     \
        return res;                                                                                                    \
    }
#endif /* __XILINX__ */

/*
 * Creates a binary operation function for vx_ap_int_base data type on the right-hand side of the operation and returns
 * new created instance/value of this operation. OP... 				binary arithmetic or logic operator
 * RET_TYPE				data type of return value
 * FCT_ATTR...			special function attributes, like const
 * RHS_TYPE...			data type of right-hand side operand
 */
#define VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(OP, RET_TYPE, FCT_ATTR, RHS_TYPE)                                             \
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST                                                                                   \
    INLINE                                                                                                             \
    typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>::RET_TYPE       \
    operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                               \
        typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>::RET_TYPE   \
            res;                                                                                                       \
        BIN_OP_OVERFLOW(OP, res, value, rhs.value, IS_SIGNED, _IS_SIGNED)                                              \
        return res;                                                                                                    \
    }

#ifdef __XILINX__
/*
 * use Xilinx operations for optimization if possible
 * additional parameter:
 * XOP... name of macro with specialized operation called, e.g. XILINX_OP_DIV
 */
#define VX_AP_INT_BIN_XILINX_OP_FCT(OP, XOP, RET_TYPE, FCT_ATTR, RHS_TYPE)                                             \
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST                                                                                   \
    INLINE                                                                                                             \
    typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>::RET_TYPE       \
    operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                               \
        typename VX_AP_BASE_STD_OWN_DATA_TYPE::template ReturnType<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>::RET_TYPE   \
            res;                                                                                                       \
        XILINX_OP(XOP, _BIT_WIDTH, _IS_SIGNED, rhs.value, res)                                                         \
        return res;                                                                                                    \
    }
#endif /* __XILINX__ */

/*
 * Creates a binary operation function for right-hand side primitive data types and assigns result to the left hand side
 * operand of the operation. OP... 		binary arithmetic or logic operator FCT_ATTR...	special function
 * attributes, like const RHS_TYPE...	data type of right-hand side operator
 */
#define PRIM_BIN_OP_FCT_SE_RHS(OP, FCT_ATTR, RHS_TYPE)                                                                 \
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator OP(RHS_TYPE rhs) FCT_ATTR {                                          \
        BIN_OP(OP, value, value, rhs)                                                                                  \
        clearUnusedBits();                                                                                             \
        return *this;                                                                                                  \
    }

/*
 * Creates a binary operation function for left-hand side primitive data types and assigns result to the left hand side
 * operand of the operation. OP... 		binary arithmetic or logic operator LHS_TYPE...	data type of left-hand
 * side operator
 */
#define PRIM_BIN_OP_FCT_SE_LHS(OP, LHS_TYPE)                                                                           \
    INLINE friend void operator OP(LHS_TYPE lhs, VX_AP_BASE_STD_OWN_DATA_TYPE rhs) { BIN_OP(OP, lhs, lhs, rhs.value) }

/*
 * Creates a binary operation functions for primitive data types on the left- and right-hand side and assigns result to
 * the left hand side operand of the operation. OP... 		binary arithmetic or logic operator FCT_ATTR... special
 * function attributes, like const PRIM_TYPE...	primitive data type
 */
#define PRIM_BIN_OP_FCT_SE(OP, FCT_ATTR, PRIM_TYPE)                                                                    \
    PRIM_BIN_OP_FCT_SE_RHS(OP, FCT_ATTR, PRIM_TYPE)                                                                    \
    PRIM_BIN_OP_FCT_SE_LHS(OP, PRIM_TYPE)

#ifdef __XILINX__
    /*
     * use Xilinx operations for optimization if possible
     * additional parameter:
     * XOP... name of macro with specialized operation called, e.g. XILINX_OP_DIV
     */
#define PRIM_BIN_XILINX_OP_FCT_SE(OP, XOP, FCT_ATTR, RHS_TYPE, RHS_BW, RHS_SIGN)                                       \
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator OP(RHS_TYPE rhs) FCT_ATTR {                                          \
        vx_ap_baseT res;                                                                                               \
        XILINX_OP(XOP, RHS_BW, RHS_SIGN, rhs, res)                                                                     \
        clearUnusedBits(res);                                                                                          \
        return *this;                                                                                                  \
    }
#endif /* __XILINX__ */

/*
 * Creates a binary operation function forvx_ap_int_base data type on the right-hand side of the operation and assigns
 * result to left hand side operand of operation. OP... 		binary arithmetic or logic operator FCT_ATTR...
 * special function attributes, like const RHS_TYPE...	data type of right-hand side operator RHS_MOD...	special
 * modifier for right hand side operand, like .value to access the value stored in a vx_prec_base
 */
#define VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(OP, FCT_ATTR, RHS_TYPE)                                                    \
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST                                                                                   \
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator OP(RHS_TYPE rhs) FCT_ATTR {                                          \
        BIN_OP(OP, value, value, rhs.value)                                                                            \
        clearUnusedBits();                                                                                             \
        return *this;                                                                                                  \
    }

#ifdef __XILINX__
    /*
     * use Xilinx operations for optimization if possible
     * additional parameter:
     * XOP... name of macro with specialized operation called, e.g. XILINX_OP_DIV
     */
#define VX_AP_BASE_XILINX_ARITHMETIC_LOGIC_FCTS(OP, XOP, FCT_ATTR, RHS_TYPE)                                           \
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST                                                                                   \
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator OP(RHS_TYPE rhs) FCT_ATTR {                                          \
        vx_ap_baseT res;                                                                                               \
        XILINX_OP(XOP, _BIT_WIDTH, _IS_SIGNED, rhs.value, res)                                                         \
        clearUnusedBits(res);                                                                                          \
        return *this;                                                                                                  \
    }
#endif /* __XILINX__ */

/*
 * Defines supported arithmetic and logic functions for primitive data types
 * RHS_TYPE...	data type of right-hand side operand
 * RHS_BW...	bit width of right-hand side operand
 * RHS_ROUND...	defines if right-hand side operand supports rounding
 * RHS_SIGN...	defines if right-hand side operand a signed or unsigned type
 */
#ifndef __XILINX__
#define PRIM_ARITHMETIC_LOGIC_FCTS(RHS_TYPE, RHS_BW, RHS_ROUND, RHS_SIGN)                                              \
    PRIM_BIN_OP_FCT(+, addT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* addition */                    \
    PRIM_BIN_OP_FCT(-, subT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* subtraction */                 \
    PRIM_BIN_OP_FCT(*, mulT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* multiplication */              \
    PRIM_BIN_OP_FCT(/, divT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* division */                    \
    PRIM_BIN_OP_FCT(%, modT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* modulo */                      \
    PRIM_BIN_OP_FCT(&, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic and */                   \
    PRIM_BIN_OP_FCT(|, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic or */                    \
    PRIM_BIN_OP_FCT(^, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic xor */                   \
    PRIM_BIN_OP_FCT_SE(+=, , const RHS_TYPE &) /* addition with self assignment */                                     \
    PRIM_BIN_OP_FCT_SE(-=, , const RHS_TYPE &) /* subtraction with self assignment */                                  \
    PRIM_BIN_OP_FCT_SE(*=, , const RHS_TYPE &) /* multiplication with self assignment */                               \
    PRIM_BIN_OP_FCT_SE(/=, , const RHS_TYPE &) /* division with self assignment */                                     \
    PRIM_BIN_OP_FCT_SE(%=, , const RHS_TYPE &) /* modulo with self assignment */                                       \
    PRIM_BIN_OP_FCT_SE(&=, , const RHS_TYPE &) /* logic and with self assignment */                                    \
    PRIM_BIN_OP_FCT_SE(|=, , const RHS_TYPE &) /* logic or with self assignment */                                     \
    PRIM_BIN_OP_FCT_SE(^=, , const RHS_TYPE &) /* logic xor with self assignment */
#else
    /*
     * Replaces generic operations with optimized operations
     */
#define PRIM_ARITHMETIC_LOGIC_FCTS(RHS_TYPE, RHS_BW, RHS_ROUND, RHS_SIGN)                                              \
    PRIM_BIN_OP_FCT(+, addT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* addition */                      \
    PRIM_BIN_OP_FCT(-, subT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* subtraction */                   \
    PRIM_BIN_OP_FCT(*, mulT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* multiplication */                \
    PRIM_BIN_XILINX_OP_FCT(/, XILINX_OP_DIV, divT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND,                         \
                           RHS_SIGN) /* division using Xilinx optimized division*/                                     \
    PRIM_BIN_OP_FCT(%, modT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN)   /* modulo */                      \
    PRIM_BIN_OP_FCT(&, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic and */                   \
    PRIM_BIN_OP_FCT(|, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic or */                    \
    PRIM_BIN_OP_FCT(^, logicT, const, const RHS_TYPE &, RHS_BW, RHS_ROUND, RHS_SIGN) /* logic xor */                   \
    PRIM_BIN_OP_FCT_SE(+=, , const RHS_TYPE &) /* addition with self assignment */                                     \
    PRIM_BIN_OP_FCT_SE(-=, , const RHS_TYPE &) /* subtraction with self assignment */                                  \
    PRIM_BIN_OP_FCT_SE(*=, , const RHS_TYPE &) /* multiplication with self assignment */                               \
    PRIM_BIN_XILINX_OP_FCT_SE(/=, XILINX_OP_DIV, , const RHS_TYPE &, RHS_BW,                                           \
                              RHS_SIGN)        /* division with self assignment using Xilinx optimized division*/      \
    PRIM_BIN_OP_FCT_SE(%=, , const RHS_TYPE &) /* modulo with self assignment */                                       \
    PRIM_BIN_OP_FCT_SE(&=, , const RHS_TYPE &) /* logic and with self assignment */                                    \
    PRIM_BIN_OP_FCT_SE(|=, , const RHS_TYPE &) /* logic or with self assignment */                                     \
    PRIM_BIN_OP_FCT_SE(^=, , const RHS_TYPE &) /* logic xor with self assignment */
#endif                                         /* __XILINX__ */
/*
 * Create binary compare functions for primitive data type on the right-hand side of the operation.
 * OP... 		binary operator compare operator
 * FCT_ATTR...	special function attributes, like const
 * RHS_TYPE...	data type of right-hand side operand
 */
#define PRIM_CMP_OP_FCT_RHS(OP, FCT_ATTR, RHS_TYPE)                                                                    \
    INLINE vx_bool operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                \
        vx_bool res;                                                                                                   \
        BIN_OP(OP, res, value, rhs)                                                                                    \
        return res;                                                                                                    \
    }

/*
 * Create binary compare functions for primitive data type on the left-hand side of the operation.
 * OP... 		binary operator compare operator
 * LHS_TYPE...	data type of left-hand side operand
 */
#define PRIM_CMP_OP_FCT_LHS(OP, LHS_TYPE)                                                                              \
    INLINE friend vx_bool operator OP(LHS_TYPE lhs, VX_AP_BASE_STD_OWN_DATA_TYPE rhs) {                                \
        vx_bool res;                                                                                                   \
        BIN_OP(OP, res, lhs, rhs.value)                                                                                \
        return res;                                                                                                    \
    }

/*
 * Create binary compare functions for primitive data type on the right-/left-hand side of the operation.
 * OP... 		binary arithmetic or logic operator
 * FCT_ATTR...	special function attributes, like const
 * PRIM_TYPE...	primitive data type
 */
#define PRIM_CMP_OP_FCT(OP, FCT_ATTR, PRIM_TYPE)                                                                       \
    PRIM_CMP_OP_FCT_RHS(OP, FCT_ATTR, PRIM_TYPE)                                                                       \
    PRIM_CMP_OP_FCT_LHS(OP, PRIM_TYPE)

/*
 * Create binary compare functions for primitive data type on the right-hand side of the operation.
 * OP... 		binary operator compare operator
 * FCT_ATTR...	special function attributes, like const
 * RHS_TYPE...	data type of right-hand side operand
 */
#define VX_AP_BASE_COMPARE_FCTS(OP, FCT_ATTR, RHS_TYPE)                                                                \
    VX_AP_BASE_STD_RHS_TEMPLATE_LIST                                                                                   \
    INLINE vx_bool operator OP(RHS_TYPE rhs) FCT_ATTR {                                                                \
        vx_bool res;                                                                                                   \
        BIN_OP(OP, res, value, rhs.value)                                                                              \
        return res;                                                                                                    \
    }

/*
 * Defines supported arithmetic compare functions.
 * RHS_TYPE...	data type of right-hand side operand
 * RHS_MOD...	special modifier for right hand side operand, like .value to access the value stored in a vx_prec_base
 */
#define PRIM_COMPARE_FCTS(PRIM_TYPE)                                                                                   \
    PRIM_CMP_OP_FCT(<, const, const PRIM_TYPE &)  /* larger */                                                         \
    PRIM_CMP_OP_FCT(>, const, const PRIM_TYPE &)  /* smaller */                                                        \
    PRIM_CMP_OP_FCT(<=, const, const PRIM_TYPE &) /* smaller equals */                                                 \
    PRIM_CMP_OP_FCT(>=, const, const PRIM_TYPE &) /* larger equals */                                                  \
    PRIM_CMP_OP_FCT(==, const, const PRIM_TYPE &) /* equals */                                                         \
    PRIM_CMP_OP_FCT(!=, const, const PRIM_TYPE &) /* equals not*/

    /*
     * Build arithmetic, logic, assigment and compare functions for supported primitive data types (vx_uint8, vx_uint16,
     * vx_uint32 and vx_uint64).
     */
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_uint8, VX_AP_DATA_TYPE_BIT_WIDTH(vx_uint8), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_uint16, VX_AP_DATA_TYPE_BIT_WIDTH(vx_uint16), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_uint32, VX_AP_DATA_TYPE_BIT_WIDTH(vx_uint32), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_uint64, VX_AP_DATA_TYPE_BIT_WIDTH(vx_uint64), vx_false_e, vx_false_e)

    PRIM_ARITHMETIC_LOGIC_FCTS(vx_int8, VX_AP_DATA_TYPE_BIT_WIDTH(vx_int8), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_int16, VX_AP_DATA_TYPE_BIT_WIDTH(vx_int16), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_int32, VX_AP_DATA_TYPE_BIT_WIDTH(vx_int32), vx_false_e, vx_false_e)
    PRIM_ARITHMETIC_LOGIC_FCTS(vx_int64, VX_AP_DATA_TYPE_BIT_WIDTH(vx_int64), vx_false_e, vx_false_e)

    PRIM_COMPARE_FCTS(vx_uint8)
    PRIM_COMPARE_FCTS(vx_uint16)
    PRIM_COMPARE_FCTS(vx_uint32)
    PRIM_COMPARE_FCTS(vx_uint64)

    PRIM_COMPARE_FCTS(vx_int8)
    PRIM_COMPARE_FCTS(vx_int16)
    PRIM_COMPARE_FCTS(vx_int32)
    PRIM_COMPARE_FCTS(vx_int64)

    /*
     * Build arithmetic, logic  and compare functions for the vx_ap_base data type
     */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(+, addT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* addition */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(-, subT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* subtraction */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(*, mulT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* multiplication */
#ifndef __XILINX__
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(/, divT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* generic division */
#else
    VX_AP_INT_BIN_XILINX_OP_FCT(/, XILINX_OP_DIV, divT, const,
                                const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* Xilinx optimized division */
#endif                                                                                       /* __XILINX__ */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(%, modT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &)   /* modulo */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(&, logicT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic and */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(|, logicT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic or */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS(^, logicT, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic xor */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(+=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* addition with self assignment */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(-=, ,
                                        const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* subtraction with self assignment */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(*=, ,
                                        const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* multiplication with self assignment */
#ifndef __XILINX__
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(
        /=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* generic division with self assignment */
#else
    VX_AP_BASE_XILINX_ARITHMETIC_LOGIC_FCTS(
        /=, XILINX_OP_DIV, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* Xilinx optimized division with self assignment */
#endif                                                                              /* __XILINX__ */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(%=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* modulo with self assignment */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(&=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic and with self assignment */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(|=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic or with self assignment */
    VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE(^=, , const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* logic xor with self assignment */

    VX_AP_BASE_COMPARE_FCTS(<, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &)  /* larger */
    VX_AP_BASE_COMPARE_FCTS(>, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &)  /* smaller */
    VX_AP_BASE_COMPARE_FCTS(<=, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* smaller equals */
    VX_AP_BASE_COMPARE_FCTS(>=, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* larger equals */
    VX_AP_BASE_COMPARE_FCTS(==, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* equals */
    VX_AP_BASE_COMPARE_FCTS(!=, const, const VX_AP_BASE_STD_RHS_DATA_TYPE &) /* equals not*/

    /*
     * Unary operations
     */

    /*
     * Prefix increment and decrement
     */
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator--() {
        value--;
        clearUnusedBits();
        return *this;
    }

    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator++() {
        value++;
        clearUnusedBits();
        return *this;
    }

    /*
     * Postfix increment and decrement
     */
    INLINE const vx_ap_derivT operator--(int) {
        vx_ap_derivT pre_decr = *this;
        clearUnusedBits(value - 1);
        return pre_decr;
    }

    INLINE const vx_ap_derivT operator++(int) {
        vx_ap_derivT pre_incr = *this;
        clearUnusedBits(value + 1);
        return pre_incr;
    }
    /*
     * Unary prefix + and -
     *
     * Increment bit width for prefix by one bit due to possible signed bit
     */
    INLINE typename ReturnType<1, WITH_ROUNDING, vx_false_e>::subT operator-() const {
        vx_ap_derivT neg(-value);
        return neg;
    }

    INLINE vx_ap_derivT operator+() const { return *this; }

    /*
     * Bitwise and logical not
     */
    INLINE vx_ap_derivT operator~() const {
        vx_ap_derivT inv(~value);
        return inv;
    }

    INLINE vx_bool operator!() const { return (value == 0); }

    /*
     * Index operator to access a specific bit
     */
    INLINE vx_bool operator[](const vx_uint8 index) { return ((value >> index) & 0x1); }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE vx_bool operator[](const VX_AP_BASE_STD_RHS_DATA_TYPE index) { return ((value >> index.value) & 0x1); }

    /*
     * Methods to address specific bits
     */

    INLINE vx_ap_baseT get_bit_sequence(const vx_uint8 start_index) { return (value >> start_index); }

    INLINE vx_ap_baseT get_bit_sequence(const vx_uint8 start_index, const vx_uint8 number_of_bits) {
        return get_bit_sequence(start_index) &
               ((1 << number_of_bits) - 1); // the second part is responsible to delete the higher bits
    }

    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit_sequence(const vx_uint8 start_index, const vx_uint8 number_of_bits,
                                                         const vx_ap_baseT bit_sequence) {
        VX_AP_BASE_STD_OWN_DATA_TYPE pre_set = *this;
        const vx_ap_baseT upper_mask = (std::numeric_limits<vx_ap_baseT>::max() << number_of_bits) << start_index;
        const vx_ap_baseT lower_mask = ~(std::numeric_limits<vx_ap_baseT>::max() << start_index);
        const vx_ap_baseT seq_mask = ~(std::numeric_limits<vx_ap_baseT>::max() << number_of_bits);
        const vx_ap_baseT mask = upper_mask | lower_mask;
        value = (value & mask) | ((bit_sequence & seq_mask) << start_index);
        clearUnusedBits();
        return pre_set;
    }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit_sequence(const vx_uint8 start_index,
                                                         const VX_AP_BASE_STD_RHS_DATA_TYPE bit_sequence) {
        return set_bit_sequence(start_index, _BIT_WIDTH, bit_sequence.value);
    }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit_sequence(const vx_uint8 start_index, const vx_uint8 number_of_bits,
                                                         const VX_AP_BASE_STD_RHS_DATA_TYPE bit_sequence) {
        return set_bit_sequence(start_index, number_of_bits, bit_sequence);
    }

    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit_sequence_by_range(const vx_uint8 start_index, const vx_uint8 end_index,
                                                                  const vx_ap_baseT bit_sequence) {
        return set_bit_sequence(start_index, end_index - start_index + 1, bit_sequence);
    }

    VX_AP_BASE_STD_RHS_TEMPLATE_LIST
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit_sequence_by_range(const vx_uint8 start_index, vx_uint8 end_index,
                                                                  const VX_AP_BASE_STD_RHS_DATA_TYPE bit_sequence) {
        return set_bit_sequence(start_index, end_index - start_index + 1, bit_sequence.value);
    }

    INLINE vx_ap_baseT get_bit(const vx_uint8 index) { return ((value >> index) & 0x1); }

    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE set_bit(const vx_uint8 index, const vx_bool new_bit) {
        return set_bit_sequence(index, 1, new_bit);
    }

    /*
     * Shift operations
     */

    INLINE vx_ap_baseT signed_right_shift(const vx_uint8 shift) const {
        const vx_ap_base_roundT rounding =
            (WITH_ROUNDING & (shift > 0)) ? (static_cast<vx_ap_base_roundT>(1) << (shift - 1)) : (0);
        return (IS_SIGNED) ? (signedRightShift<vx_ap_baseT>(value + rounding, shift)) : ((value + rounding) >> shift);
    }

    INLINE vx_ap_derivT operator>>(const vx_uint8 shift) const { return signed_right_shift(shift); }

    INLINE vx_ap_derivT operator<<(const vx_uint8 shift) const { return VX_AP_BASE_STD_OWN_DATA_TYPE(value << shift); }

    /*
     * Shift operations with self-assignment
     */
    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator>>=(const vx_uint8 shift) {
        value = signed_right_shift(shift);
        return *this;
    }

    INLINE VX_AP_BASE_STD_OWN_DATA_TYPE &operator<<=(const vx_uint8 shift) {
        clearUnusedBits(value << shift);
        return *this;
    }

    /*
     * Additional shift operations
     */
#define SHIFT_RETURN_TYPE(NEW_BIT_WIDTH) typename vx_ap_int_factory<NEW_BIT_WIDTH, WITH_ROUNDING, IS_SIGNED>::DataType

    template <vx_uint8 TARGET_BIT_WIDTH> INLINE SHIFT_RETURN_TYPE(TARGET_BIT_WIDTH) shift_to() const {
        const vx_bool is_smaller = TARGET_BIT_WIDTH < BIT_WIDTH;
        const vx_uint8 diff = (is_smaller) ? (BIT_WIDTH - TARGET_BIT_WIDTH) : (TARGET_BIT_WIDTH - BIT_WIDTH);
        const vx_ap_baseT v = (is_smaller) ? (signed_right_shift(diff)) : (value << diff);
        return SHIFT_RETURN_TYPE(TARGET_BIT_WIDTH)(v);
    }

    template <vx_uint8 TARGET_BIT_WIDTH>
    INLINE SHIFT_RETURN_TYPE(VX_AP_MIN(TARGET_BIT_WIDTH, BIT_WIDTH)) shift_to_right_only() const {
        const vx_bool is_smaller = TARGET_BIT_WIDTH < BIT_WIDTH;
        const vx_uint8 diff = BIT_WIDTH - TARGET_BIT_WIDTH;
        const vx_ap_baseT v = (is_smaller) ? (signed_right_shift(diff)) : (value);
        return SHIFT_RETURN_TYPE(VX_AP_MIN(TARGET_BIT_WIDTH, BIT_WIDTH))(v);
    }

    template <vx_uint8 TARGET_BIT_WIDTH>
    INLINE SHIFT_RETURN_TYPE(VX_AP_MAX(TARGET_BIT_WIDTH, BIT_WIDTH)) shift_to_left_only() const {
        // shift only to left if new data type has more bits else give original value with same bit width back
        const vx_uint64 v = (TARGET_BIT_WIDTH < BIT_WIDTH)
                                ? (static_cast<vx_uint64>(value))
                                : (static_cast<vx_uint64>(value) << (TARGET_BIT_WIDTH - BIT_WIDTH));
        return SHIFT_RETURN_TYPE(VX_AP_MAX(TARGET_BIT_WIDTH, BIT_WIDTH))(v);
    }

    INLINE typename to_unsigned<vx_ap_baseT>::DataType to_uint() {
        return static_cast<typename to_unsigned<vx_ap_baseT>::DataType>(value);
    }

    INLINE typename to_signed<vx_ap_baseT>::DataType to_int() {
        return static_cast<typename to_signed<vx_ap_baseT>::DataType>(value);
    }

    INLINE typename to_unsigned<vx_ap_baseT>::DataType abs_int() {
        return static_cast<typename to_unsigned<vx_ap_baseT>::DataType>(abs_int(value));
    }

#undef SHIFT_RETURN_TYPE

    INLINE static vx_ap_baseT mask() { return static_cast<vx_uint64>(-1) >> (VX_AP_MAX_BIT_WIDTH - BIT_WIDTH); }

    INLINE static vx_ap_baseT max() {
        const vx_uint64 max = mask();
        return (IS_SIGNED) ? (max >> 1) : (max);
    }

    INLINE static vx_ap_baseT min() {
        const vx_uint64 min = mask();
        return (IS_SIGNED) ? (min) : (0);
    }

    INLINE static vx_uint8 bit_width() { return BIT_WIDTH; }

    INLINE static vx_bool is_signed_data_type() { return IS_SIGNED; }

    INLINE vx_bool is_signed() {
        const vx_bool sign_bit = signed_right_shift(BIT_WIDTH - 1);
        return IS_SIGNED & sign_bit;
    }

    INLINE vx_ap_baseT get() { return value; }

    INLINE vx_ap_baseT set(vx_ap_baseT new_value) {
        const vx_ap_baseT old_value = value;
        clearUnusedBits(new_value);
        return old_value;
    }

#undef VX_AP_MAX_BIT_WIDTH

#undef VX_AP_DATA_TYPE_BIT_WIDTH

#undef SUPPORTED_CTOR_DATA_TYPES

#undef SMALLEST_TYPE

#undef CTOR
#undef OP_ASSIGN_BODY
#undef OP_ASSIGN

#undef BIN_OP

#undef PRIM_BIN_OP_FCT
#undef VX_AP_BASE_ARITHMETIC_LOGIC_FCTS

#undef PRIM_BIN_OP_FCT_SE
#undef VX_AP_BASE_ARITHMETIC_LOGIC_FCTS_SE

#undef PRIM_ARITHMETIC_LOGIC_FCTS

#undef PRIM_CMP_OP_FCT
#undef VX_AP_BASE_COMPARE_FCTS
#undef PRIM_COMPARE_FCTS

#undef SHIFT_RETURN_TYPE

#undef VX_AP_BASE_STD_RHS_TEMPLATE_LIST
#undef VX_AP_BASE_STD_RHS_DATA_TYPE
#undef VX_AP_BASE_STD_OWN_DATA_TYPE
};

template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING = vx_false_e>
struct vx_ap_uint : vx_ap_base<BIT_WIDTH, WITH_ROUNDING, vx_false_e> {

    typedef vx_ap_base<BIT_WIDTH, WITH_ROUNDING, vx_false_e> Base;

#define VX_AP_UINT_STD_RHS_TEMPLATE_LIST template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING>
#define VX_AP_UINT_STD_RHS_DATA_TYPE vx_ap_uint<_BIT_WIDTH, _WITH_ROUNDING>
#define VX_AP_INT_STD_RHS_TEMPLATE_LIST template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING>
#define VX_AP_INT_STD_RHS_DATA_TYPE vx_ap_int<_BIT_WIDTH, _WITH_ROUNDING>

#define SUPPORTED_CTOR_DATA_TYPES(FCT)                                                                                 \
    FCT(vx_uint8)                                                                                                      \
    FCT(vx_uint16)                                                                                                     \
    FCT(vx_uint32)                                                                                                     \
    FCT(vx_uint64)                                                                                                     \
    FCT(vx_int8)                                                                                                       \
    FCT(vx_int16)                                                                                                      \
    FCT(vx_int32)                                                                                                      \
    FCT(vx_int64)

    /*
     * Constructors
     */

#define CTOR(TYPE)                                                                                                     \
    INLINE vx_ap_uint(TYPE _value) : Base(_value) {}

    /*
     * Standard constructor
     */
    INLINE vx_ap_uint() : Base() {}

    /*
     * Create constructors for primitive data types
     */
    SUPPORTED_CTOR_DATA_TYPES(CTOR)

    /*
     * Constructors for vx_ap_uint and vx_ap_int;
     */
    VX_AP_UINT_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_uint(const VX_AP_UINT_STD_RHS_DATA_TYPE &p) : Base(p.value) {
        // Base::value = p.value;
    }

    VX_AP_INT_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_uint(const VX_AP_INT_STD_RHS_DATA_TYPE &p) : Base(p.value) {
        // Base::value = p.value;
    }

    /*
     * Assign operator
     */

    VX_AP_UINT_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_UINT_STD_RHS_DATA_TYPE &rhs) const { this(rhs.value); }

    VX_AP_INT_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_INT_STD_RHS_DATA_TYPE &rhs) const { this(rhs.value); }

#undef VX_AP_UINT_STD_RHS_TEMPLATE_LIST
#undef VX_AP_UINT_STD_RHS_DATA_TYPE
#undef VX_AP_INT_STD_RHS_TEMPLATE_LIST
#undef VX_AP_INT_STD_RHS_DATA_TYPE

#undef CTOR
#undef SUPPORTED_CTOR_DATA_TYPES
};

template <vx_uint8 BIT_WIDTH, vx_bool WITH_ROUNDING = vx_false_e>
struct vx_ap_int : vx_ap_base<BIT_WIDTH, WITH_ROUNDING, vx_true_e> {

    typedef vx_ap_base<BIT_WIDTH, WITH_ROUNDING, vx_true_e> Base;

#define VX_AP_UINT_STD_RHS_TEMPLATE_LIST template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING>
#define VX_AP_UINT_STD_RHS_DATA_TYPE vx_ap_uint<_BIT_WIDTH, _WITH_ROUNDING>
#define VX_AP_INT_STD_RHS_TEMPLATE_LIST template <vx_uint8 _BIT_WIDTH, vx_bool _WITH_ROUNDING>
#define VX_AP_INT_STD_RHS_DATA_TYPE vx_ap_int<_BIT_WIDTH, _WITH_ROUNDING>

#define SUPPORTED_CTOR_DATA_TYPES(FCT)                                                                                 \
    FCT(vx_uint8)                                                                                                      \
    FCT(vx_uint16)                                                                                                     \
    FCT(vx_uint32)                                                                                                     \
    FCT(vx_uint64)                                                                                                     \
    FCT(vx_int8)                                                                                                       \
    FCT(vx_int16)                                                                                                      \
    FCT(vx_int32)                                                                                                      \
    FCT(vx_int64)

    /*
     * Constructors
     */

#define CTOR(TYPE)                                                                                                     \
    INLINE vx_ap_int(TYPE _value) : Base(_value) {}

    /*
     * Standard constructor
     */
    INLINE vx_ap_int() : Base() {}

    /*
     * Create constructors for primitive data types
     */
    SUPPORTED_CTOR_DATA_TYPES(CTOR)

    /*
     * Constructors for vx_ap_uint and vx_ap_int;
     */
    VX_AP_UINT_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_int(const VX_AP_UINT_STD_RHS_DATA_TYPE &p) : Base(p.value) {
        //		Base::value = p.value;
    }

    VX_AP_INT_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_int(const VX_AP_INT_STD_RHS_DATA_TYPE &p) : Base(p.value) {
        //		Base::value = p.value;
    }

    /*
     * Assign operator
     */

    VX_AP_UINT_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_UINT_STD_RHS_DATA_TYPE &rhs) const { this(rhs.value); }

    VX_AP_INT_STD_RHS_TEMPLATE_LIST
    INLINE void operator=(const VX_AP_INT_STD_RHS_DATA_TYPE &rhs) const { this(rhs.value); }

#undef VX_AP_UINT_STD_RHS_TEMPLATE_LIST
#undef VX_AP_UINT_STD_RHS_DATA_TYPE
#undef VX_AP_INT_STD_RHS_TEMPLATE_LIST
#undef VX_AP_INT_STD_RHS_DATA_TYPE

#undef CTOR
#undef SUPPORTED_CTOR_DATA_TYPES
};

template <vx_uint8 BIT_WIDTH, vx_uint8 EXP_BITS, vx_bool WITH_ROUNDING, vx_bool IS_SIGNED> struct vx_ap_float_base {

#define VX_AP_FLOAT_BASE_RETURN_EXP_TYPE vx_ap_int<VX_AP_MAX(EXP_BITS, _EXP_BITS), vx_false_e>
#define VX_AP_FLOAT_BASE_RETURN_TYPE                                                                                   \
    vx_ap_float_base<VX_AP_MAX(BIT_WIDTH, _BIT_WIDTH), VX_AP_MAX(EXP_BITS, _EXP_BITS), WITH_ROUNDING | _WITH_ROUNDING, \
                     IS_SIGNED | _IS_SIGNED>
#define VX_AP_FLOAT_BASE_OWN_DATA_TYPE vx_ap_float_base<BIT_WIDTH, EXP_BITS, WITH_ROUNDING, IS_SIGNED>
#define VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE vx_ap_float_base<_BIT_WIDTH, _EXP_BITS, _WITH_ROUNDING, _IS_SIGNED>
#define VX_AP_FLOAT_BASE_STD_RHS_TEMPLATE_LIST                                                                         \
    template <vx_uint8 _BIT_WIDTH, vx_uint8 _EXP_BITS, vx_bool _WITH_ROUNDING, vx_bool _IS_SIGNED>
#define VX_AP_FLOAT_STD_OP_TEMPLATE_LIST template <typename RhsType, typename RetType>
#define VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST template <typename ResType, typename RhsType, typename RetType>

    typedef vx_ap_base<BIT_WIDTH, WITH_ROUNDING, IS_SIGNED> valueT;
    typedef vx_ap_int<EXP_BITS, vx_false_e> expT;

    // number is: value * 2^exp
    valueT value;
    expT exp;

    INLINE vx_ap_float_base() {}

    INLINE vx_ap_float_base(const valueT &v, const expT &e) {
        value = v;
        exp = e;
    }

    /*
     * Copy constructor
     */
    VX_AP_FLOAT_BASE_STD_RHS_TEMPLATE_LIST
    INLINE vx_ap_float_base(const VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE &rhs) {
        value = rhs.value;
        exp = rhs.exp;
    }

    /*
     * Value functions
     */
    VX_AP_FLOAT_STD_OP_TEMPLATE_LIST
    INLINE RetType op_add(const RhsType &rhs) {
        // TODO
        return RetType(0);
    }

    VX_AP_FLOAT_STD_OP_TEMPLATE_LIST
    INLINE RetType op_sub(const RhsType &rhs) {
        // TODO
        return RetType(0);
    }

    VX_AP_FLOAT_STD_OP_TEMPLATE_LIST
    INLINE RetType op_mul(const RhsType &rhs) {
        // TODO
        return RetType(0);
    }

    VX_AP_FLOAT_STD_OP_TEMPLATE_LIST
    INLINE RetType op_div(const RhsType &rhs) {
        // TODO
        return RetType(0);
    }

    /*
     * Exponent functions
     */
    VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST
    INLINE RetType exp_add(const ResType &res, const RhsType &rhs) const {
        // if (res.is_signed_data_type()) {
        //	const vx_bool lhs_is_signed = value.is_signed();
        //	const vx_bool rhs_is_signed = rhs.value.is_signed();
        //	const vx_bool res_smsb = (res >> (res.bit_width() - 2)).value;	//get second most significiant bit (bit
        // width of result is for addítion increased by 1 by default 	const vx_bool unsigned_overflow = !lhs_is_signed
        // &
        //! rhs_is_signed & res_smsb; 	const vx_bool signed_overflow = lhs_is_signed & rhs_is_signed & !res_smsb;
        //! return
        // ExpType(VX_AP_MAX(exp, rhs.exp) + unsigned_overflow - signed_overflow;
        //}
        // else {
        //	//TODO
        //	/*return ExpType(VX_AP_MAX(exp, rhs.exp))
        //	exp += (res >> (res.bit_width() - 1)).value;*/
        //}
        return RetType(0);
    }

    VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST
    INLINE RetType exp_sub(const ResType &res, const RhsType &rhs) const {
        // const vx_uint64 highest_bit = static_cast<vx_uint64>(1) << (res.bit_width() - 2);
        // const vx_bool incr = res.value & highest_bit;
        // exp -= incr;
        // TODO overflow check subtraction
        return RetType(0);
    }

    VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST
    INLINE RetType exp_mul(const ResType &res, const RhsType &rhs) const { return ExpType(exp + rhs.exp); }

    VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST
    INLINE RetType exp_div(const ResType &res, const RhsType &rhs) const { return ExpType(exp - rhs.exp); }

#define OPERATION(OP, OP_TYPE, OP_FCT, EXP_FCT, FCT_ATTR, RHS_TYPE)                                                    \
    VX_AP_FLOAT_BASE_STD_RHS_TEMPLATE_LIST                                                                             \
    INLINE VX_AP_FLOAT_BASE_RETURN_TYPE operator OP(RHS_TYPE rhs) FCT_ATTR {                                           \
        typedef typename valueT::template ReturnType<_BIT_WIDTH, _WITH_ROUNDING, _IS_SIGNED>::OP_TYPE res_type;        \
        const res_type op_res_value = OP_FCT<VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE, res_type>(rhs);                       \
        const VX_AP_FLOAT_BASE_RETURN_EXP_TYPE return_exp =                                                            \
            EXP_FCT<res_type, VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE, VX_AP_FLOAT_BASE_RETURN_EXP_TYPE>(op_res_value,      \
                                                                                                    rhs);              \
        const VX_AP_FLOAT_BASE_RETURN_TYPE return_value =                                                              \
            op_res_value.template shift_to<VX_AP_MAX(BIT_WIDTH, _BIT_WIDTH)>();                                        \
        return vx_ap_float_base(return_value, return_exp);                                                             \
    }

    OPERATION(+, addT, op_add, exp_add, const, const VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE &) /* addition */
    // OPERATION(-, subT, op_sub, exp_sub, const, const VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE&) 		/* subtraction
    // */ OPERATION(*, mulT, op_mul, exp_mul, const, const VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE&) 		/*
    // multiplication */ OPERATION(/ , divT, op_div, exp_div, const, const VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE&) /*
    // division */

#undef OPERATION

    /*
     * Shift operations
     */
    INLINE VX_AP_FLOAT_BASE_OWN_DATA_TYPE operator>>(const vx_uint8 shift) const {
        return VX_AP_FLOAT_BASE_OWN_DATA_TYPE(value, exp - shift);
    }

    INLINE VX_AP_FLOAT_BASE_OWN_DATA_TYPE operator<<(const vx_uint8 shift) const {
        return VX_AP_FLOAT_BASE_OWN_DATA_TYPE(value, exp + shift);
    }

    /*
     * Shift operations with self-assignment
     */
    INLINE VX_AP_FLOAT_BASE_OWN_DATA_TYPE &operator>>=(const vx_uint8 shift) {
        exp -= shift;
        return *this;
    }

    INLINE VX_AP_FLOAT_BASE_OWN_DATA_TYPE &operator<<=(const vx_uint8 shift) {
        exp += shift;
        return *this;
    }

#undef VX_AP_FLOAT_BASE_OWN_DATA_TYPE
#undef VX_AP_FLOAT_BASE_STD_RHS_DATA_TYPE
#undef VX_AP_FLOAT_BASE_STD_RHS_TEMPLATE_LIST
#undef VX_AP_FLOAT_STD_OP_EXP_TEMPLATE_LIST
};

template <typename T, vx_uint8 SIZE> struct vx_ra_shift_register {

    typedef vx_ra_shift_register<T, SIZE> ownT;

    T shift_register[SIZE];

    /*
     * Default constructor
     */
    INLINE vx_ra_shift_register() {}

    INLINE void initialize(const T init_value) {
        for (vx_uint8 i = 0; i < SIZE; i++) {
#pragma HLS UNROLL
            shift_register[i] = init_value;
        }
    }

    template <vx_bool SHIFT_FROM_LOWEST_TO_HIGHEST = vx_true_e> INLINE T shift(const T push_element) {
        if (SHIFT_FROM_LOWEST_TO_HIGHEST) {
            return shift_from_lowest_to_highest(push_element);
        } else {
            return shift_from_highest_to_lowest(push_element);
        }
    }

    INLINE T shift_from_lowest_to_highest(const T push_element) {

        T e1 = push_element;

        for (vx_uint8 i = 0; i < SIZE; i++) {
#pragma HLS UNROLL
            const T e2 = shift_register[i];
            shift_register[i] = e1;
            e1 = e2;
        }

        return e1;
    }

    INLINE T shift_from_highest_to_lowest(const T push_element) {

        T e1 = push_element;

        for (vx_int16 i = SIZE - 1; i >= 0; i--) {
#pragma HLS UNROLL
            const T e2 = shift_register[i];
            shift_register[i] = e1;
            e1 = e2;
        }

        return e1;
    }

    INLINE T read_at(const vx_uint8 index) { return shift_register[index]; }

    INLINE T write_at(const vx_uint8 index, const T new_value) {
        const T old_value = shift_register[index];
        shift_register[index] = new_value;
        return old_value;
    }

    template <vx_bool SHIFT_FROM_LOWEST_TO_HIGHEST> INLINE T get_newest_element() {
        return read_at((SHIFT_FROM_LOWEST_TO_HIGHEST) ? (0) : (SIZE - 1));
    }

    template <vx_bool SHIFT_FROM_LOWEST_TO_HIGHEST> INLINE T get_oldest_element() {
        return read_at((SHIFT_FROM_LOWEST_TO_HIGHEST) ? (SIZE - 1) : (0));
    }

    INLINE static void copy(T dst_register[SIZE], const T src_register[SIZE]) {
        for (vx_int8 i = 0; i < SIZE; i++) {
#pragma HLS UNROLL
            dst_register[i] = src_register[i];
        }
    }

    INLINE static void copy(ownT &dst_register, const ownT src_register) {
        ownT::copy(dst_register.shift_register, src_register.shift_register);
    }

    INLINE void copy_from(const T src_register[SIZE]) { ownT::copy(shift_register, src_register); }

    INLINE void copy_from(const ownT src_register) { ownT::copy(*this, src_register); }

    INLINE void copy_to(T dst_register[SIZE]) { ownT::copy(dst_register, shift_register); }

    INLINE void copy_to(ownT &dst_register) { ownT::copy(dst_register, *this); }

    INLINE T &operator[](const vx_uint8 index) { return shift_register[index]; }
};

template <typename T, vx_uint32 SIZE> struct vx_ra_ring_buffer {

#define LOG2_CEIL_SIZE (comp_log2<SIZE>::log2_ceil)

    typedef vx_ap_uint<LOG2_CEIL_SIZE> numT;
    typedef typename vx_ap_select_smallest_data_type<((LOG2_CEIL_SIZE + 7) >> 3), vx_false_e>::DataType num_base_T;

#undef LOG2_CEIL_SIZE

    T buffer[SIZE];
    numT nr_elements;
    numT ptr_next_element;

    /*
     * Default constructor
     */
    INLINE vx_ra_ring_buffer() {
#pragma HLS UNROLL
        nr_elements = 0;
        ptr_next_element = 0;
    }

    INLINE vx_bool is_full() { return (nr_elements == SIZE); }

    INLINE vx_bool is_empty() { return (nr_elements == 0); }

    INLINE num_base_T fill_level() { return nr_elements.value; }

    INLINE num_base_T get_index_of_next_element() { return ptr_next_element.value; }

    INLINE num_base_T get_index_of_current_element() {
        return (ptr_next_element > 0) ? ((ptr_next_element - 1).value) : (static_cast<num_base_T>(SIZE - 1));
    }

    INLINE T read_at(const numT index) { return buffer[index.value]; }

    INLINE T read_at_with_positive_offset(const numT index, const numT offset) {
        return read_at(((index + offset) % SIZE));
    }

    INLINE T read_at_with_negative_offset(const numT index, const numT offset) {
        return read_at((index < offset) ? (numT(numT(SIZE) - offset + index)) : (numT(index - offset)));
    }

    INLINE T write_at(const numT index, const T new_element) {
        const T old_element = buffer[index.value];
        buffer[index.value] = new_element;
        return old_element;
    }

    INLINE T write_at_with_positive_offset(const numT index, const numT offset, const T new_element) {
        return write_at(((index + offset) % SIZE), new_element);
    }

    INLINE T write_at_with_negative_offset(const numT index, const numT offset, const T new_element) {
        return write_at((index < offset) ? (numT(numT(SIZE) - offset + index)) : (numT(index - offset)), new_element);
    }

    INLINE T push(const T push_element) {
        const T pop_element = write_at(ptr_next_element, push_element);
        ptr_next_element = (ptr_next_element < SIZE - 1) ? (numT(ptr_next_element + 1)) : (numT(0));
        if (nr_elements < SIZE)
            ++nr_elements;
        return pop_element;
    }

    INLINE T pop() {
        ptr_next_element = (ptr_next_element > 0) ? (numT(ptr_next_element - 1)) : (numT(SIZE - 1));
        if (nr_elements > 0)
            --nr_elements;
        return read_at(ptr_next_element);
    }

    INLINE T pop_reverse() {
        const numT revers_index = (nr_elements > ptr_next_element) ? (numT(numT(SIZE) - nr_elements + ptr_next_element))
                                                                   : (numT(ptr_next_element - nr_elements));
        if (nr_elements > 0)
            --nr_elements;
        return read_at(revers_index);
    }

    INLINE void reset() {
        ptr_next_element = 0;
        nr_elements = 0;
    }

    INLINE void flush(const T flush_value) {
        for (vx_uint32 i = 0; i < SIZE; i++) {
#pragma HLS PIPELINE II = 1
            buffer[i] = flush_value;
        }
        reset();
    }

    INLINE T &operator[](const numT index) { return buffer[index.value]; }

#undef LOG2_CEIL_SIZE
};

template <typename T, vx_uint64 SIZE> struct vx_ping_pong {

    vx_bool selected_write_buffer = vx_false_e;
    T buffer[2][SIZE];

    INLINE vx_ping_pong() {}

    INLINE vx_ping_pong(const T init_data) { initialize(init_data); }

    INLINE void switch_buffer() { selected_write_buffer = !selected_write_buffer; }

    INLINE T read_from(const vx_uint64 index) { return buffer[!selected_write_buffer][index]; }

    INLINE T write_to(const vx_uint64 index, const T new_data) {
        const T tmp = buffer[selected_write_buffer][index];
        buffer[selected_write_buffer][index] = new_data;
        return tmp;
    }

    INLINE void initialize(const T init_data) {
        for (vx_uint64 i = 0; i < SIZE; i++) {
#pragma HLS pipeline II = 1
            buffer[0][i] = init_data;
            buffer[1][i] = init_data;
        }
    }

    INLINE T *get_read_buffer() { return buffer[!selected_write_buffer]; }

    INLINE T *get_write_buffer() { return buffer[selected_write_buffer]; }

    INLINE T *operator[](const vx_bool buffer_id) { return buffer[buffer_id]; }
};

#undef INLINE
#undef VX_AP_MAX
#undef VX_AP_MIN

/********************************************************************************************************************************/
/**
 * @brief Compile time solvable function to convolve two kernels.
 * @tparam InputKernelType Data type of the first input kernel.
 * @tparam OutputKernelType Data type of the second input kernel.
 * @tparam FIRST_INPUT_KERN_SIZE Size of first kernel in one dimension.
 * @tparam SECOND_INPUT_KERN_SIZE Size of second kernel in one dimension.
 * @param first_input_kernel First input kernel which shall be convolved with second one.
 * @param second_input_kernel Second input kernel which shall be conolved with first one.
 * @param output_kernel Kernel which was created by the convolution of both input kernels.
 */
template <typename InputKernelType, typename OutputKernelType, vx_uint8 FIRST_INPUT_KERN_SIZE,
          vx_uint8 SECOND_INPUT_KERN_SIZE>
void ConvolveKernel(const InputKernelType first_input_kernel[FIRST_INPUT_KERN_SIZE][FIRST_INPUT_KERN_SIZE],
                    const InputKernelType second_input_kernel[SECOND_INPUT_KERN_SIZE][SECOND_INPUT_KERN_SIZE],
                    OutputKernelType output_kernel[FIRST_INPUT_KERN_SIZE + SECOND_INPUT_KERN_SIZE - 1]
                                                  [FIRST_INPUT_KERN_SIZE + SECOND_INPUT_KERN_SIZE - 1]) {
#pragma HLS INLINE

    const vx_uint8 OUTPUT_KERN_SIZE = FIRST_INPUT_KERN_SIZE + SECOND_INPUT_KERN_SIZE - 1;

    const vx_uint8 FIRST_INPUT_KERN_RAD = (FIRST_INPUT_KERN_SIZE >> 1);

    // virtually embed the second kernel into the output kernel
    const vx_uint8 VIRT_START_XY = ((OUTPUT_KERN_SIZE - SECOND_INPUT_KERN_SIZE) >> 1);
    const vx_uint8 VIRT_END_XY = SECOND_INPUT_KERN_SIZE + VIRT_START_XY;

    // initialize the output kernel with zeros
    for (vx_uint8 i = 0; i < OUTPUT_KERN_SIZE; i++) {
#pragma HLS unroll
        for (vx_uint8 j = 0; j < OUTPUT_KERN_SIZE; j++) {
#pragma HLS unroll
            output_kernel[i][j] = 0;
        }
    }

    // compute the convolution
    for (vx_int8 oy = 0; oy < OUTPUT_KERN_SIZE; oy++) {
#pragma HLS unroll
        for (vx_int8 ox = 0; ox < OUTPUT_KERN_SIZE; ox++) {
#pragma HLS unroll
            for (vx_int8 fy = -FIRST_INPUT_KERN_RAD; fy <= FIRST_INPUT_KERN_RAD; fy++) {
#pragma HLS unroll
                for (vx_int8 fx = -FIRST_INPUT_KERN_RAD; fx <= FIRST_INPUT_KERN_RAD; fx++) {
#pragma HLS unroll
                    const vx_int8 ty = oy + fy;
                    const vx_int8 tx = ox + fx;
                    if ((ty >= VIRT_START_XY) && (tx >= VIRT_START_XY) && (ty < VIRT_END_XY) && (tx < VIRT_END_XY)) {
                        output_kernel[oy][ox] +=
                            first_input_kernel[FIRST_INPUT_KERN_RAD - fy][FIRST_INPUT_KERN_RAD - fx] *
                            second_input_kernel[ty - VIRT_START_XY][tx - VIRT_START_XY];
                    }
                }
            }
        }
    }
}

typedef vx_uint16 contrastT;
typedef vx_uint32 contrastSquareT;
typedef vx_uint16 tauT;

#define BYTES_IN_BITS(VAL) ((VAL) << 3)

#define INVALID_KEYPOINT_DATA 0xFFFFFFFFFFFFFFFF

template <vx_uint16 BUFFER_IMG_COLS> struct select_buffer_data_type;

#define SELECT_BUFFER_DATA_TYPE(S, T)                                                                                  \
    template <> struct select_buffer_data_type<S> { typedef T DataType; };

SELECT_BUFFER_DATA_TYPE(4, vx_uint32)
SELECT_BUFFER_DATA_TYPE(2, vx_uint16)
SELECT_BUFFER_DATA_TYPE(1, vx_uint8)

#undef SELECT_BUFFER_DATA_TYPE

/*
 * Calculate size of scaled kernel
 * K... kernel size
 * S... scale factor
 */
#define SCALE_KERNEL(K, S) ((K - 1) * S + 1)

#define SCHARR_X 0
#define SCHARR_Y 1

#define DOH_XX 0
#define DOH_XY 1
#define DOH_YY 2

#define SCHARR_XY_3x3                                                                                                  \
    { SCHARR_X_3x3, SCHARR_Y_3x3 }
#define SCHARR_X_3x3                                                                                                   \
    {                                                                                                                  \
        {3, 0, -3}, {10, 0, -10}, { 3, 0, -3 }                                                                         \
    }
#define SCHARR_Y_3x3                                                                                                   \
    {                                                                                                                  \
        {3, 10, 3}, {0, 0, 0}, { -3, -10, -3 }                                                                         \
    }

/*
 * Calculate new number of pixel using stride
 * P... number of pixel
 * S... stride
 */
#define STEP_PIXEL(P, S) (P / (S + 1))

// Computes the minimum kernel radius/size for a given radius (with fraction). Rounds up to the next nearest integer if
// the fractional part is larger or equal 0.5.
#define MINIMUM_KERNEL_RADIUS_FOR_RADIUS(RADIUS_WITH_FRACTION, RADIUS_FRACTION_BITS, WITH_ROUNDING)                    \
    ((RADIUS_WITH_FRACTION >> RADIUS_FRACTION_BITS) +                                                                  \
     ((((0x1 << (RADIUS_FRACTION_BITS - 1)) & RADIUS_WITH_FRACTION) > 0) && WITH_ROUNDING))
#define MINIMUM_KERNEL_SIZE_FOR_RADIUS(RADIUS_WITH_FRACTION, RADIUS_FRACTION_BITS, WITH_ROUNDING)                      \
    ((MINIMUM_KERNEL_RADIUS_FOR_RADIUS(RADIUS_WITH_FRACTION, RADIUS_FRACTION_BITS, WITH_ROUNDING) << 1) + 1)

/*
 * Compute the maximum possible number of detectable keypoints per cycle depending on the input vector sizeand the
 *kernel size. Worst case:
 *				First Keypoint									Second
 *Keypoint Third Keypoint & s.o.
 * <------------------------------------------------> <---------------------------------> <-----------------
 *			|			|	X		|		|		|	X
 *|
 *|
 *|
 *	-(K-1)	|	-(K-2)	| -(K-1)/2	|	...	|	0	|	1	|	...	|
 *(K - 1)/2 + 1	|	(K - 1)/2 + 2 |	...	|	Vin - 2 | Vin - 1
 *
 *					1							+			(Vin
 *- 1)
 * /
 *((K
 *- 1)/2
 *+ 1)
 *
 */
#define FIND_EXTREMA_VEC_NUM_OUT(VEC_NUM_IN, KERNEL_SIZE) (((VEC_NUM_IN - 1) / ((KERNEL_SIZE >> 1) + 1)) + 1)

#endif /* SRC_IMG_MT_HELPER_H_ */
