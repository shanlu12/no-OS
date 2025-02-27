/***************************************************************************//**
 *   @file   no_os_util.c
 *   @brief  Implementation of utility functions.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2018(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "no_os_util.h"
#include "errno.h"
/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * Find first set bit in word.
 */
uint32_t no_os_find_first_set_bit(uint32_t word)
{
	uint32_t first_set_bit = 0;

	while (word) {
		if (word & 0x1)
			return first_set_bit;
		word >>= 1;
		first_set_bit ++;
	}

	return 32;
}

/**
 * Find last set bit in word.
 */
uint32_t no_os_find_last_set_bit(uint32_t word)
{
	uint32_t bit = 0;
	uint32_t last_set_bit = 32;

	while (word) {
		if (word & 0x1)
			last_set_bit = bit;
		word >>= 1;
		bit ++;
	}

	return last_set_bit;
}

/**
 * Locate the closest element in an array.
 */
uint32_t no_os_find_closest(int32_t val,
			    const int32_t *array,
			    uint32_t size)
{
	int32_t diff = abs(array[0] - val);
	uint32_t ret = 0;
	uint32_t i;

	for (i = 1; i < size; i++) {
		if (abs(array[i] - val) < diff) {
			diff = abs(array[i] - val);
			ret = i;
		}
	}

	return ret;
}

/**
 * Shift the value and apply the specified mask.
 */
uint32_t no_os_field_prep(uint32_t mask, uint32_t val)
{
	return (val << no_os_find_first_set_bit(mask)) & mask;
}

/**
 * Get a field specified by a mask from a word.
 */
uint32_t no_os_field_get(uint32_t mask, uint32_t word)
{
	return (word & mask) >> no_os_find_first_set_bit(mask);
}

/**
 * Log base 2 of the given number.
 */
int32_t no_os_log_base_2(uint32_t x)
{
	return no_os_find_last_set_bit(x);
}

/**
 * Find greatest common divisor of the given two numbers.
 */
uint32_t no_os_greatest_common_divisor(uint32_t a,
				       uint32_t b)
{
	uint32_t div;
	uint32_t common_div = 1;

	if ((a == 0) || (b == 0))
		return no_os_max(a, b);

	for (div = 1; (div <= a) && (div <= b); div++)
		if (!(a % div) && !(b % div))
			common_div = div;

	return common_div;
}

/**
 * Calculate best rational approximation for a given fraction.
 */
void no_os_rational_best_approximation(uint32_t given_numerator,
				       uint32_t given_denominator,
				       uint32_t max_numerator,
				       uint32_t max_denominator,
				       uint32_t *best_numerator,
				       uint32_t *best_denominator)
{
	uint32_t gcd;

	gcd = no_os_greatest_common_divisor(given_numerator, given_denominator);

	*best_numerator = given_numerator / gcd;
	*best_denominator = given_denominator / gcd;

	if ((*best_numerator > max_numerator) ||
	    (*best_denominator > max_denominator)) {
		*best_numerator = 0;
		*best_denominator = 0;
	}
}

/**
 * Calculate the number of set bits.
 */
uint32_t no_os_hweight8(uint32_t word)
{
	uint32_t count = 0;

	while (word) {
		if (word & 0x1)
			count++;
		word >>= 1;
	}

	return count;
}

/**
 * Calculate the quotient and the remainder of an integer division.
 */
uint64_t no_os_do_div(uint64_t* n,
		      uint64_t base)
{
	uint64_t mod = 0;

	mod = *n % base;
	*n = *n / base;

	return mod;
}

/**
 * Unsigned 64bit divide with 64bit divisor and remainder
 */
uint64_t no_os_div64_u64_rem(uint64_t dividend, uint64_t divisor,
			     uint64_t *remainder)
{
	*remainder = dividend % divisor;

	return dividend / divisor;
}

/**
 * Unsigned 64bit divide with 32bit divisor with remainder
 */
uint64_t no_os_div_u64_rem(uint64_t dividend, uint32_t divisor,
			   uint32_t *remainder)
{
	*remainder = no_os_do_div(&dividend, divisor);

	return dividend;
}

/**
 * Signed 64bit divide with 32bit divisor with remainder
 */
int64_t no_os_div_s64_rem(int64_t dividend, int32_t divisor, int32_t *remainder)
{
	*remainder = dividend % divisor;
	return dividend / divisor;
}

/**
 * Unsigned 64bit divide with 32bit divisor
 */
uint64_t no_os_div_u64(uint64_t dividend, uint32_t divisor)
{
	uint32_t remainder;

	return no_os_div_u64_rem(dividend, divisor, &remainder);
}

/**
 * Signed 64bit divide with 32bit divisor
 */
int64_t no_os_div_s64(int64_t dividend, int32_t divisor)
{
	int32_t remainder;
	return no_os_div_s64_rem(dividend, divisor, &remainder);
}

/**
 * Converts from string to int32_t
 * @param *str
 * @return int32_t
 */
int32_t no_os_str_to_int32(const char *str)
{
	char *end;
	int32_t value = strtol(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}

/**
 * Converts from string to uint32_t
 * @param *str
 * @return uint32_t
 */
uint32_t no_os_str_to_uint32(const char *str)
{
	char *end;
	uint32_t value = strtoul(str, &end, 0);

	if (end == str)
		return -EINVAL;
	else
		return value;
}

void no_os_put_unaligned_be16(uint16_t val, uint8_t *buf)
{
	buf[1] = val & 0xFF;
	buf[0] = val >> 8;
}

uint16_t no_os_get_unaligned_be16(uint8_t *buf)
{
	return buf[1] | ((uint16_t)buf[0] << 8);
}

void no_os_put_unaligned_le16(uint16_t val, uint8_t *buf)
{
	buf[0] = val & 0xFF;
	buf[1] = val >> 8;
}

uint16_t no_os_get_unaligned_le16(uint8_t *buf)
{
	return buf[0] | ((uint16_t)buf[1] << 8);
}

void no_os_put_unaligned_be24(uint32_t val, uint8_t *buf)
{
	buf[2] = val & 0xFF;
	buf[1] = (val >> 8) && 0xFF;
	buf[0] = val >> 16;
}

uint32_t no_os_get_unaligned_be24(uint8_t *buf)
{
	return buf[2] | ((uint16_t)buf[1] << 8) | ((uint32_t)buf[0] << 16);
}

void no_os_put_unaligned_le24(uint32_t val, uint8_t *buf)
{
	buf[0] = val & 0xFF;
	buf[1] = (val >> 8) && 0xFF;
	buf[2] = val >> 16;
}

uint32_t no_os_get_unaligned_le24(uint8_t *buf)
{
	return buf[0] | ((uint16_t)buf[1] << 8) | ((uint32_t)buf[2] << 16);
}

void no_os_put_unaligned_be32(uint32_t val, uint8_t *buf)
{
	buf[3] = val & 0xFF;
	buf[2] = (val >> 8) && 0xFF;
	buf[1] = (val >> 16) && 0xFF;
	buf[0] = val >> 24;
}

uint32_t no_os_get_unaligned_be32(uint8_t *buf)
{
	return buf[3] | ((uint16_t)buf[2] << 8) | ((uint32_t)buf[1] << 16)
	       | ((uint32_t)buf[0] << 24);
}

void no_os_put_unaligned_le32(uint32_t val, uint8_t *buf)
{
	buf[0] = val & 0xFF;
	buf[1] = (val >> 8) && 0xFF;
	buf[2] = (val >> 16) && 0xFF;
	buf[3] = val >> 24;
}

uint32_t no_os_get_unaligned_le32(uint8_t *buf)
{
	return buf[0] | ((uint16_t)buf[1] << 8) | ((uint32_t)buf[2] << 16)
	       | ((uint32_t)buf[3] << 24);
}

int32_t no_os_sign_extend32(uint32_t value, int index)
{
	uint8_t shift = 31 - index;
	return (int32_t)(value << shift) >> shift;
}

uint64_t no_os_mul_u32_u32(uint32_t a, uint32_t b)
{
	return (uint64_t)a * b;
}

uint64_t no_os_mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift)
{
	uint32_t ah, al;
	uint64_t ret;

	al = a;
	ah = a >> 32;

	ret = no_os_mul_u32_u32(al, mul) >> shift;
	if (ah)
		ret += no_os_mul_u32_u32(ah, mul) << (32 - shift);

	return ret;
}
