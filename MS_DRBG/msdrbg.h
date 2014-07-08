#include "biglong.h"

#ifndef MSDRBG
#define MSDRBG

namespace drbg
{
	using namespace blong;
	using namespace std;

	const string N_1024 =
		string("b66fbfdafbac2fd82eb13dc44fa170ffc9f7c7b51d55b214") +
		"4cc2257b29df3f62b421b1580753f304a671ff8b55dd8abf" +
		"b53d31aba0ad742f21857acf814af3f1e126d771a61eca54" +
		"e62bfdb585c311b058e9cd3faab758a5e28968496ec1dd51" +
		"d0355aa155d4d9126140dcfab9b03f62a5032d06536d8574" +
		"0988f38427f35885";

	const string N_2048 =
		string("c11a01f25daf396aa927157baf6f504f78cba32457b58c6b") +
		"f7d851af42385cc7905b06f41f6d47ab1b3a2c1217d14d15" +
		"070c9da524734ada2fe17a95e600ae9a4f8b1a6696661e40" +
		"7d3043ecd10231265d8ea0d181cf23c6dd3dec9eb3fce204" +
		"5b9299bbcca63dee435a2251ad0765d49d29db2ef5aba161" +
		"279aeb5f6899fe487973e36c1fb13086d9231b6b925a8495" +
		"4ba0fbcafea844ea77a9f852f86915a4e71bd0bab9b269c3" +
		"9a7a827a41311ffa4470140c8b6509fe5dbd39e3ec816066" +
		"2d036e130e07e23306a39b18db0e8efe6441888081ac3673" +
		"2b4091f663690d033b486d74371a20fc3e214bce7ed0e797" +
		"5ea44453cd161d32e818520459896571";

	const string N_3072 =
		string("c6046ba68beaa061c468a9a74da34d6421398c73020837c7") +
		"d2a4042bdd9a7628cab8022e5bc4246f75da8d2603da8021" +
		"41c5d112835e6bdb57ed799e28d6fa49c3d0f5b5f9776c14" +
		"0a901bf773ae311335d0470eda91b442dbac621acdd324e2" +
		"a70244d7cb155adc4b77dd94fafe069d5b5cc49486e9fe61" +
		"c5081190abb24f542d7d21e9c90453c69ac63143401d6b35" +
		"e456ea2f64ae76f92df80328b48f7962d5c9b779b2078496" +
		"7d374f0206b8afbf678d7f5f36c3d84ec9e55c287ce5c668" +
		"17ee05b41059168fb5c5e2a36bc2f6ce3b43bd1456eebdd5" +
		"70ffe61e5a7023a904d98f8a96bfaf5555a12f815561b401" +
		"63f3a50ea1e16a363f5cddd4a1db275c4fc2d650d51f1e93" +
		"f5fd7631ca45914ff6fe62a0be55b9975f6566bb47e76276" +
		"f4e3b2eb837bf0da9d824687042479a3041473992d814a3a" +
		"7be7bc3d06992df66c1d7d06f8c1410e2bbb573a0e278e7a" +
		"daa600f32577030e95b73dd996b65f984740a485e27138bd" +
		"d5f0252209bcf0056640a1b3b1dd97ad7c187e0401ba817d";

	class MsDrbg
	{
		static const size_t MAX_SECURITY_STRENGTH = 128;
		static const size_t reseed_interval = 50000;

		biglong e;
		biglong N;
		size_t n;
		size_t k;
		size_t r;
		size_t n_byte;
		size_t k_byte;
		size_t r_byte;

		biglong s;
		size_t reseed_counter;

		size_t security_strength;
		size_t max_num_of_threads;		

		string last_error;
		bool instantiated;

		static unsigned char* get_entropy(const size_t length);

		static string hash_df_hex(const unsigned char* seed_material,
		                          const size_t seed_material_size,
								  const size_t seedlen);

		static void reseed(biglong& int_state, const size_t byte_len,
		                   const string& additional_input_string);

		static unsigned char* get_raw_bytes(const size_t value);

		static unsigned char* concatenate(const void* left, const size_t left_size,
		                                  const void* right, const size_t right_size);

	public :
		MsDrbg();

		bool instantiate(const size_t requested_strength, const size_t requested_e,
		                 const size_t requested_k, const string& personalization_string);

		bool generate(const size_t requested_strength,
		              const size_t requested_num_of_bytes, string& hex_string);

		bool generate(const size_t requested_strength,
		              const size_t requested_num_of_bytes, unsigned char*& buffer);

		bool generate_parallel(const size_t requested_strength,
		                       const size_t requested_num_of_bytes, string& hex_string,
		                       const size_t requested_num_of_threads);

		bool generate_parallel(const size_t requested_strength,
		              const size_t requested_num_of_bytes, unsigned char*& buffer,
					  const size_t requested_num_of_threads);

		string get_last_error() const;

		bool is_instantiated() const;
	};
}
#endif