#include "msdrbg.h"
#include <fstream>

using namespace drbg;
using namespace std;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		string exec_path(argv[0]);
		string exec_name = exec_path.substr(exec_path.rfind('\\') + 1);
		cout << "Usage: " << exec_name << " " << "<number_of_bytes>" << endl;
		return 1;
	}

	size_t random_size = stoul(argv[1], nullptr, 10);

	MsDrbg gen;
	gen.instantiate(112, 7, 0, "Micali-Schnorr DRBG Personalization String");

	unsigned char* random = nullptr;
	gen.generate_parallel(112, random_size, random, 3);
		
	string program_full_name = string(argv[0]);
	size_t last_backslash = program_full_name.find_last_of("\\");
	string program_path = program_full_name.substr(0,last_backslash+1);

	ofstream file(program_path + "msrnd.dat", ios::binary);
	file.write(reinterpret_cast<char*>(random), random_size);

	delete[] random;

	return 0;
}