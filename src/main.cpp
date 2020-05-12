#include "md5.h"
#include <cstdio>
#include <sstream>
#include <fstream>

void p(word_t a) {
	uchar *p = (uchar *) &a;
	for (int i=0; i<4; i++) {
		printf("%02x", p[i]);
	}
}

int main (int argc, char *argv[]) {
	using namespace std;

	// istringstream is("");
	ifstream is(argv[1], ios_base::in | ios_base::binary);
	MD5BlockReader r(&is);
	auto s = md5(r);
	// printf("%08x %08x %08x %08x\n", s.a, s.b, s.c, s.d);
	p(s.a);
	p(s.b);
	p(s.c);
	p(s.d);
	puts("");

	return 0;
}
