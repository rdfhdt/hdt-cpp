/**
 * \file crc32.h
 * Functions and types for CRC checks.
 *
 * Generated on Tue Aug 14 12:43:17 2012,
 * by pycrc32 v0.7.10, http://www.tty1.net/pycrc32/
 * using the configuration:
 *    Width        = 32
 *    Poly         = 0x1edc6f41
 *    XorIn        = 0xffffffff
 *    ReflectIn    = True
 *    XorOut       = 0xffffffff
 *    ReflectOut   = True
 *    Algorithm    = table-driven
 *****************************************************************************/
#ifndef __CRC32___H__
#define __CRC32___H__
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <stdint.h>

/**
 * The definition of the used algorithm.
 *****************************************************************************/
#define CRC_ALGO_TABLE_DRIVEN 1


/**
 * The type of the CRC values.
 *
 * This type must be big enough to contain at least 32 bits.
 *****************************************************************************/
typedef uint32_t crc32_t;


/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param data         The data word to be reflected.
 * \param data_len     The width of \a data expressed in number of bits.
 * \return             The reflected data.
 *****************************************************************************/
crc32_t crc32_reflect(crc32_t data, size_t data_len);


/**
 * Calculate the initial crc32 value.
 *
 * \return     The initial crc32 value.
 *****************************************************************************/
static inline crc32_t crc32_init(void)
{
    return 0xffffffff;
}

crc32_t crc32_read(std::istream &in);


/**
 * Update the crc32 value with new data.
 *
 * \param crc32      The current crc32 value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc32 value.
 *****************************************************************************/
crc32_t crc32_update(crc32_t crc32, const unsigned char *data, const size_t data_len);


/**
 * Calculate the final crc32 value.
 *
 * \param crc32  The current crc32 value.
 * \return     The final crc32 value.
 *****************************************************************************/
static inline crc32_t crc32_finalize(crc32_t crc32)
{
    return crc32 ^ 0xffffffff;
}

class CRC32 {
private:
	crc32_t crc;

public:
	CRC32() : crc(crc32_init()) {

	}

	inline void update(unsigned char *buf, size_t len) {
		crc = crc32_update(crc, buf, len);
	}

	inline void writeData(std::ostream &out, unsigned char *buf, size_t len) {
		crc = crc32_update(crc, buf, len);

#ifdef WIN32
		// Write by 1Mb blocks
		const size_t BLOCK_SIZE = 8192;
		size_t counter=0;
		char *ptr = (char *)buf;
		while(counter<len && out.good()) {
		    size_t currByt = len-counter > BLOCK_SIZE ? BLOCK_SIZE : len-counter;
		    out.write(&ptr[counter], currByt);
		    counter += currByt;
		}
		if(counter!=len) {
		    throw std::runtime_error("Could not write full buffer");
		}
#else
		out.write((char*)buf, len);
#endif
	}

	inline void writeCRC(std::ostream &out) {
		crc32_t end = crc32_finalize(crc);
		out.write((char*)&end, sizeof(end));
	}

	inline void readData(std::istream &in, unsigned char *buf, size_t len) {
		in.read((char*)buf, len);
		crc = crc32_update(crc, buf, in.gcount());
	}

	inline crc32_t getValue(){
		return crc32_finalize(crc);
	}

	inline void reset() {
		crc = crc32_init();
	}
};


#endif      /* __CRC___H__ */
