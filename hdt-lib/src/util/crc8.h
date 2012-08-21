/**
 * \file crc8.h
 * Functions and types for CRC checks.
 *
 * Generated on Tue Aug 14 12:44:19 2012,
 * by pycrc8 v0.7.10, http://www.tty1.net/pycrc8/
 * using the configuration:
 *    Width        = 8
 *    Poly         = 0x07
 *    XorIn        = 0x00
 *    ReflectIn    = False
 *    XorOut       = 0x00
 *    ReflectOut   = False
 *    Algorithm    = table-driven
 *****************************************************************************/
#ifndef __CRC8__H__
#define __CRC8__H__

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>


/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 8 bits.
 *****************************************************************************/
typedef uint8_t crc8_t;


/**
 * Calculate the initial crc8 value.
 *
 * \return     The initial crc8 value.
 *****************************************************************************/
static inline crc8_t crc8_init(void)
{
    return 0x00;
}


/**
 * Update the crc8 value with new data.
 *
 * \param crc8      The current crc8 value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc8 value.
 *****************************************************************************/
crc8_t crc8_update(crc8_t crc8, const unsigned char *data, const size_t data_len);


crc8_t crc8_read(std::istream &in);


/**
 * Calculate the final crc8 value.
 *
 * \param crc8  The current crc8 value.
 * \return     The final crc8 value.
 *****************************************************************************/
static inline crc8_t crc8_finalize(crc8_t crc8)
{
    return crc8;
}

class CRC8 {
private:
	crc8_t crc;

public:
	CRC8() : crc(crc8_init()) {

	}

    inline void update(const unsigned char *buf, size_t len) {
		crc = crc8_update(crc, buf, len);
	}

    inline void writeData(std::ostream &out, const unsigned char *buf, size_t len) {
		crc = crc8_update(crc, buf, len);
		out.write((char*)buf, len);
	}

	inline void writeCRC(std::ostream &out) {
		crc8_t end = crc8_finalize(crc);
		out.write((char*)&end, sizeof(end));
	}

	inline void readData(std::istream &in, unsigned char *buf, size_t len) {
		in.read((char*)buf, len);
		crc = crc8_update(crc, buf, in.gcount());
	}

	inline crc8_t getValue(){
		return crc8_finalize(crc);
	}

	inline void reset() {
		crc = crc8_init();
	}

	inline bool equal(CRC8 &other) {
		return other.crc==crc;
	}
};

#endif      /* __CRC__H__ */
