#include "msdrbg.h"
#include <fstream>

using namespace drbg;
using namespace std;

int main(int argc, char* argv[])
{
	MsDrbg gen;
	gen.instantiate(112, 7, 0, "Micali-Schnorr DRBG Personalization String");

	size_t random_size = 500000000;
	unsigned char* random = nullptr;
	gen.generate_parallel(112, random_size, random, 3);
		
	string program_full_name = string(argv[0]);
	size_t last_backslash = program_full_name.find_last_of("\\");
	string program_path = program_full_name.substr(0,last_backslash+1);

	ofstream file(program_path + "random.bin", ios::binary);
	file.write(reinterpret_cast<char*>(random), random_size);

	delete[] random;

	return 0;
}