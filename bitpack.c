/*
 * COMP40       HW4: arith
 *
 *              bitpack.c
 *
 * Date:        03/03/2022
 * Authors:     Pablo Duran (pduran01) and Abe Park (ypark12)
 *
 * Purpose:     Implementation of bitpack functions. Include implementation
 *              details of the functions introduced in the provided bitpack.h
 *              and commentations to help better understanding.
 */
#include <bitpack.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <except.h>

#define MAX_WIDTH 64


Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Name:                shift_leftu
 * Purpose:             left shift an unsigned integer by the given value
 * Input:               an unsigned integer and an integer that represents
 *                      by how much will the integer be shifted to the left
 * Output:              a left shifted unsigned integer
 */
uint64_t shift_leftu(uint64_t n, unsigned shift)
{
    if (shift == MAX_WIDTH) {
        return 0;
    }

    uint64_t result = n << shift;
    return result;
}

/*
 * Name:                shift_lefts
 * Purpose:             left shift a signed integer by the given value
 * Input:               a signed integer and an integer that represents
 *                      by how much will the integer be shifted to the left
 * Output:              a left shifted signed integer
 */
int64_t shift_lefts(int64_t n, unsigned shift)
{
    if (shift == MAX_WIDTH) {
        return 0;
    }

    int64_t result = n << shift;
    return result;
}

/*
 * Name:                shift_rightu
 * Purpose:             right shift an unsigned integer by the given value
 * Input:               an unsigned integer and an integer that represents
 *                      by how much will the integer be shifted to the right
 * Output:              a right shifted unsigned integer
 */
uint64_t shift_rightu(uint64_t n, unsigned shift)
{
    if (shift == MAX_WIDTH) {
        return 0;
    }

    uint64_t result = n >> shift;
    return result;
}

/*
 * Name:                shift_rightu
 * Purpose:             right shift an unsigned integer by the given value
 * Input:               an unsigned integer and an integer that represents
 *                      by how much will the integer be shifted to the right
 * Output:              a right shifted unsigned integer
 */
int64_t shift_rights(int64_t n, unsigned shift)
{
    if (shift == MAX_WIDTH) {
        return 0;
    }

    int64_t result = n >> shift;
    return result;
}


/*
 * Name:                Bitpack_fitsu
 * Purpose:             check if the given width can represent the given
 *                      unsigned integer
 * Input:               an unsigned integer and the number of bits used
 * Output:              true if the unsigned integer can be represented
 *                      in the number of bits given. false if not
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= MAX_WIDTH);
    /* 2^width - 1 is how many unsigned integers can be represented */
    uint64_t capacity = shift_leftu(1, width) - 1;
    if (n <= capacity) {
        return true;
    } else {
        return false;
    }
}

/*
 * Name:                Bitpack_fitss
 * Purpose:             check if the given width can represent the given
 *                      signed integer
 * Input:               an signed integer and the number of bits used
 * Output:              true if the signed integer can be represented
 *                      in the number of bits given. false if not
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= MAX_WIDTH);
    /* 2^(width-1) - 1 is how many positive integers can be represented
     * -2^(width-1) is how many negative integers can be represented
     * output: true if  -(2^(width-1) - 1) <= n <= 2^(width-1) - 1
     */
    int64_t upperBound = shift_lefts(1, width - 1) - 1;
    int64_t lowerBound = shift_lefts(-1, width - 1);
    if (n <= upperBound && n >= lowerBound) {
        return true;
    } else {
        return false;
    }
}

/*
 * Name:                Bitpack_getu
 * Purpose:             get bits from the given unsigned word, with its index
 *                      being [MAX_WIDTH - witdh, lsb]
 * Input:               uint64_t word, and unsigned (width, lsb)
 * Output:              uint64_t result (== word[MAX_WIDTH - width, lsb])
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= MAX_WIDTH);
    assert((width + lsb) <= MAX_WIDTH);

    if (width == 0) {
        return 0;
    }

    uint64_t mask = ~0;
    mask = shift_rightu(mask, (MAX_WIDTH -  width));
    mask = shift_leftu(mask, lsb);

    uint64_t result = word & mask;
    result = shift_rightu(result, lsb);

    return result;
}

/*
 * Name:                Bitpack_gets
 * Purpose:             get bits from the given signed word, with its index
 *                      being [MAX_WIDTH - witdh, lsb]
 * Input:               uint64_t word, and unsigned (width, lsb)
 * Output:              int64_t result (== word[MAX_WIDTH - width, lsb])
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width <= MAX_WIDTH);
    assert((width + lsb) <= MAX_WIDTH);

    if (width == 0) {
        return 0;
    }

    int64_t result;
    result = shift_lefts(word, (MAX_WIDTH -  width - lsb));
    result = shift_rights(result, (MAX_WIDTH -  width));

    return result;
}

/*
 * Name:                Bitpack_newu
 * Purpose:             renew the bits at lsb of size width in the given
 *                      unsigned word with the given unsigned value
 * Input:               uint64_t word, unsigned (width, lsb), and uint64_t
 *                      value to replace
 * Output:              uint64_t updatedWord with the replaced uint64_t value
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, uint64_t value)
{
    assert(width <= MAX_WIDTH);
    assert((width + lsb) <= MAX_WIDTH);

    if (!(Bitpack_fitsu(value, width))) {
        RAISE(Bitpack_Overflow);
    }
    /* create a mask and have as many 0's as the width starting from lsb
     * create 0's starting from the left of the field to be extracted
     */
    uint64_t u_mask = ~0;
    uint64_t mask_left = shift_leftu(u_mask, (lsb + width));
    uint64_t mask_right = shift_rightu(u_mask, (MAX_WIDTH - lsb));

    uint64_t mask = mask_left | mask_right;
    uint64_t updatedWord = word & mask;

    /* shift the value to the left by lsb */
    value = shift_leftu(value, lsb);
    updatedWord = updatedWord | value;

    return updatedWord;
}

/*
 * Name:                Bitpack_news
 * Purpose:             renew the bits at lsb of size width in the given
 *                      unsigned word with the given signed value
 * Input:               uint64_t word, unsigned (width, lsb), and int64_t
 *                      value to replace
 * Output:              uint64_t updatedWord with the replaced int64_t value
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, int64_t value)
{
    assert(width <= MAX_WIDTH);
    assert((width + lsb) <= MAX_WIDTH);

    if (!(Bitpack_fitss(value, width))) {
        RAISE(Bitpack_Overflow);
    }

    /* create a mask and have as many 0's as the width starting from lsb
     * create 0's starting from the left of the field to be extracted
     */

    int64_t s_mask = ~0;
    /* TODO: shift_lefts or shift_leftu? same thing below */
    int64_t mask_left = shift_lefts(s_mask, (lsb + width));
    int64_t mask_right = shift_rights(s_mask, (MAX_WIDTH - lsb));

    uint64_t mask = mask_left | mask_right;
    uint64_t updatedWord = word & mask;

    /* shift the value to the left by lsb */
    value = shift_lefts(value, lsb);
    updatedWord = updatedWord | value;

    return updatedWord;
}
