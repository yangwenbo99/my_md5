#include "md5.h"
#include <cassert>
#include <cstring>

word_t Ks[] = {
		0xd76aa478U, 0xe8c7b756U, 0x242070dbU, 0xc1bdceeeU,
		0xf57c0fafU, 0x4787c62aU, 0xa8304613U, 0xfd469501U,
		0x698098d8U, 0x8b44f7afU, 0xffff5bb1U, 0x895cd7beU,
		0x6b901122U, 0xfd987193U, 0xa679438eU, 0x49b40821U,
		0xf61e2562U, 0xc040b340U, 0x265e5a51U, 0xe9b6c7aaU,
		0xd62f105dU, 0x02441453U, 0xd8a1e681U, 0xe7d3fbc8U,
		0x21e1cde6U, 0xc33707d6U, 0xf4d50d87U, 0x455a14edU,
		0xa9e3e905U, 0xfcefa3f8U, 0x676f02d9U, 0x8d2a4c8aU,
		0xfffa3942U, 0x8771f681U, 0x6d9d6122U, 0xfde5380cU,
		0xa4beea44U, 0x4bdecfa9U, 0xf6bb4b60U, 0xbebfbc70U,
		0x289b7ec6U, 0xeaa127faU, 0xd4ef3085U, 0x04881d05U,
		0xd9d4d039U, 0xe6db99e5U, 0x1fa27cf8U, 0xc4ac5665U,
		0xf4292244U, 0x432aff97U, 0xab9423a7U, 0xfc93a039U,
		0x655b59c3U, 0x8f0ccc92U, 0xffeff47dU, 0x85845dd1U,
		0x6fa87e4fU, 0xfe2ce6e0U, 0xa3014314U, 0x4e0811a1U,
		0xf7537e82U, 0xbd3af235U, 0x2ad7d2bbU, 0xeb86d391U };

int rots[] = {
	7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
	5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
	4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
	6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

inline word_t process_f (const word_t b, const word_t c, const word_t d) {
	return (b & c) | (~b & d);
}
inline word_t process_g (const word_t b, const word_t c, const word_t d) {
	return (b & d) | (c & ~d);
}
inline word_t process_h (const word_t b, const word_t c, const word_t d) {
	return b ^ c ^ d;
}
inline word_t process_i (const word_t b, const word_t c, const word_t d) {
	return c ^ (b | ~d);
}

state_t md5 (MD5BlockReader &reader) {
	state_t state{0x67452301U, 0xefcdab89U, 0x98badcfeU, 0x10325476U};
	while (reader.has_next()) {
		state_t tmp_state = state;
		auto block = reader.get_next();
		for (int i=0; i<64; i++) {
			word_t fv;
			int g;
			if (i < 16) {
				fv = process_f(tmp_state.b, tmp_state.c, tmp_state.d);
				g = i;
			} else if (i < 32) {
				fv = process_g(tmp_state.b, tmp_state.c, tmp_state.d);
				g = (5 * i + 1) % 16;
			} else if (i < 48) {
				fv = process_h(tmp_state.b, tmp_state.c, tmp_state.d);
				g = (3 * i + 5) % 16;
			} else {
				fv = process_i(tmp_state.b, tmp_state.c, tmp_state.d);
				g = (7 * i) % 16;
			}
			tmp_state.forward(
					fv,
					Ks[i], block.body[g], rots[i]);
		}
		state.embrace(tmp_state);
	}
	return state;
}

int MD5BlockReader::has_next() {
	return (*_is) || _remaining_block_num > 0;
}

block_t MD5BlockReader::get_next() {
	block_t block;
	if (!(*_is)) {
		if (_remaining_block_num == 1) {
			_remaining_block_num--;
			memcpy(
					(void *) block.body, 
					(void *) (_buf + BLOCK_BYTE_SIZE), 
					BLOCK_BYTE_SIZE);
		} else {
			throw NoBlockException();
		}
	}
	if (_is->read((char *)_buf, BLOCK_BYTE_SIZE)) {
		_size += BLOCK_BYTE_SIZE;
	} else {
		size_t l = _is->gcount();
		_size += l;
		size_t zs = l + 1;
		size_t ze;
		if (l <= (BLOCK_BYTE_SIZE - 8 - 1)) {
			ze = BLOCK_BYTE_SIZE - 8;
		} else {
			ze = 2 * BLOCK_BYTE_SIZE - 8;
			_remaining_block_num = 1;
		}
		_buf[l] = 0x80U;
		std::fill(_buf + zs, _buf + ze, 0);
		*((uint64_t *) (_buf + ze)) = _size * 8;
	}
#ifdef DEBUG
	std::cout << "CPY" << std::endl;
#endif
	memcpy(
			(void *) block.body, 
			(void *) _buf, 
			BLOCK_BYTE_SIZE);

#ifdef DEBUG
	printf("%ld Block:", _is->gcount());
	for (int i=0; i<BLOCK_SIZE; i++) {
		printf("%08x ", block.body[i]);
	}
	printf("\n");
#endif
	return block;
}
