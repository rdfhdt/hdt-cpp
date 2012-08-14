/**
 * \file crc16.h
 * Functions and types for CRC checks.
 *
 * Generated on Tue Aug 14 12:44:08 2012,
 * by pycrc16 v0.7.10, http://www.tty1.net/pycrc16/
 * using the configuration:
 *    Width        = 16
 *    Poly         = 0x8005
 *    XorIn        = 0x0000
 *    ReflectIn    = True
 *    XorOut       = 0x0000
 *    ReflectOut   = True
 *    Algorithm    = table-driven
 *****************************************************************************/
#ifndef __CRC___H__
#define __CRC___H__

#include <stdlib.h>
#include <stdint.h>

/**
 * The definition of the used algorithm.
 *****************************************************************************/
#define CRC_ALGO_TABLE_DRIVEN 1


/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 16 bits.
 *****************************************************************************/
typedef uint16_t crc16_t;


/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param data         The data word to be reflected.
 * \param data_len     The width of \a data expressed in number of bits.
 * \return             The reflected data.
 *****************************************************************************/
crc16_t crc16_reflect(crc16_t data, size_t data_len);


/**
 * Calculate the initial crc16 value.
 *
 * \return     The initial crc16 value.
 *****************************************************************************/
static inline crc16_t crc16_init(void)
{
    return 0x0000;
}


/**
 * Update the crc16 value with new data.
 *
 * \param crc16      The current crc16 value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc16 value.
 *****************************************************************************/
crc16_t crc16_update(crc16_t crc16, const unsigned char *data, const size_t data_len);


/**
 * Calculate the final crc16 value.
 *
 * \param crc16  The current crc16 value.
 * \return     The final crc16 value.
 *****************************************************************************/
static inline crc16_t crc16_finalize(crc16_t crc16)
{
    return crc16 ^ 0x0000;
}

#endif      /* __CRC___H__ */
