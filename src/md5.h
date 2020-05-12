#ifndef MD5_H
#define MD5_H

#include <cstdint>
#include <iostream>

typedef unsigned char uchar;

#define BLOCK_SIZE 16

#define WORD_LEN 32

#define BLOCK_BYTE_SIZE (BLOCK_SIZE * sizeof(word_t))

typedef uint32_t word_t;

typedef word_t (*state_processor) (
		const word_t b,
		const word_t c,
		const word_t d);

struct block_t {
	word_t body[BLOCK_SIZE];
};

struct state_t {
	word_t a, b, c, d;

	void forward(word_t fv, word_t k, word_t m, int rotn) {
		auto pf = a + fv + m + k;
		auto rpv = (pf << rotn) | (pf >> (WORD_LEN - rotn));
		a = d;
		d = c;
		c = b;
		b = b + rpv;
	}

	void embrace(state_t &other) {
		a += other.a;
		b += other.b;
		c += other.c;
		d += other.d;
	}
};

class NoBlockException : std::exception {
};

class MD5BlockReader {
private:
	std::istream *_is;
	uint64_t _size = 0;
	uchar _buf[2 * BLOCK_BYTE_SIZE];
	int _remaining_block_num = 0;
public:
	MD5BlockReader (std::istream *is) : _is(is) { }
	block_t get_next();
	int has_next();
};



state_t md5 (MD5BlockReader &reader);

#endif
