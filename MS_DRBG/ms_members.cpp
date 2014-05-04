#include <omp.h>
#include "msdrbg.h"

namespace drbg
{
	const size_t ms_drbg::reseed_interval = 50000;

	ms_drbg::ms_drbg()
	{
		last_error = "";
		instantiated = false;
	}

	string ms_drbg::get_last_error() const
	{
		return last_error;
	}

	bool ms_drbg::is_instantiated() const
	{
		return instantiated;
	}

	bool ms_drbg::instantiate(const size_t requested_strength,
	                                 const size_t requested_e,
	                                 const size_t requested_k,
	                                 const string& personalization_string)
	{
		if(requested_strength > MAX_SECURITY_STRENGTH)
		{
			last_error = "Invalid requested_strength";
			return 0;
		}

		if(requested_strength <= 80)
		{
			security_strength = 80;
			n = 1024;
			N = N_1024;
		}
		else if(requested_strength <= 112)
		{
			security_strength = 112;
			n = 2048;
			N = N_2048;
		}
		else if(requested_strength <= 128)
		{
			security_strength = 128;
			n = 3072;
			N = N_3072;
		}
		else
		{
			last_error = "Invalid requested_strength";
			return 0;
		}
		n_byte = n/8;

		size_t exp;
		if(requested_e == 0)
		{
			exp = 3;
			e = exp;
		}
		else
		{
			if(requested_e < 3 || requested_e%2 == 0)
			{
				last_error = "Invalid requested_e";
				return 0;
			}

			exp = requested_e;
			e = requested_e;
		}

		if(requested_k == 0)
		{
			k = min(n-2*security_strength, (size_t)floor(n*(1-2/(double)exp)));
			k_byte = k/8;
			k = k_byte*8;
		}
		else
		{
			if(requested_k > min(n-2*security_strength, (size_t)floor(n*(1-2/(double)exp)))
				|| requested_k%8 != 0)
			{
				last_error = "Invalid requested_k";
				return 0;
			}

			k = requested_k;
		}

		r=n-k;
		r_byte = r/8;

		max_num_of_threads = n_byte / r_byte;

		unsigned char* entropy_input = get_entropy(r_byte);
		unsigned char* entropy_personalization = concatenate(entropy_input,
		                                                     r_byte, personalization_string.c_str(),
															 personalization_string.length());
		s = hash_df_hex(entropy_personalization, r_byte + personalization_string.length(), r_byte);

		delete[] entropy_input;
		delete[] entropy_personalization;

		reseed_counter = 0;
		instantiated = true;

		return 1;
	}	

	bool ms_drbg::generate(const size_t requested_strength,
	                              const size_t requested_num_of_bytes, string& hex_string)
	{
		unsigned char* buffer = nullptr;
		bool status = generate(requested_strength, requested_num_of_bytes, buffer);
		if(!status)
			return 0;

		stringstream strstream;
		strstream << hex;
		for(size_t i=0; i < requested_num_of_bytes; ++i)
			strstream << static_cast<size_t>(buffer[i]);

		hex_string = strstream.str();
		delete[] buffer;

		return 1;
	}

	bool ms_drbg::generate(const size_t requested_strength,
		              const size_t requested_num_of_bytes, unsigned char*& buffer)
	{
		if(!instantiated)
		{
			last_error = "DRBG is not instantiated";
			return 0;
		}

		if(requested_strength > security_strength)
		{
			last_error = "Invalid requested_strength";
			return 0;
		}

		size_t num_of_steps = requested_num_of_bytes/k_byte + 1;
		buffer = new unsigned char[num_of_steps*k_byte];
		for(size_t i=0; i < num_of_steps; ++i)
		{
			while(reseed_counter >= reseed_interval || s == biglong::one)
			{
				reseed(s, r_byte, "");
				reseed_counter = 0;
			}

			unsigned char* s_power_mod_bytes = nullptr;
			size_t s_power_mod_size = (s.power_mod(e, N))
				.get_raw_bytes(s_power_mod_bytes, n_byte);
						
			memcpy(buffer+i*k_byte, s_power_mod_bytes, k_byte);
			s = biglong(s_power_mod_bytes+k_byte, r_byte);
			++s;

			++reseed_counter;

			delete[] s_power_mod_bytes;
		}
		
		return 1;
	}

	bool ms_drbg::generate_parallel(const size_t requested_strength,
	                                       const size_t requested_num_of_bytes, string& hex_string,
	                                       const size_t requested_num_of_threads)
	{
		unsigned char* buffer = nullptr;
		bool status = generate_parallel(requested_strength, requested_num_of_bytes,
		                                buffer, requested_num_of_threads);
		if(!status)
			return 0;

		stringstream strstream;
		strstream << hex;
		for(size_t i=0; i < requested_num_of_bytes; ++i)
			strstream << static_cast<size_t>(buffer[i]);

		hex_string = strstream.str();
		delete[] buffer;

		return 1;
	}

	bool ms_drbg::generate_parallel(const size_t requested_strength,
	                                       const size_t requested_num_of_bytes, unsigned char*& buffer,
	                                       const size_t requested_num_of_threads)
	{
		if(requested_num_of_threads == 1)
			return generate(requested_strength, requested_num_of_bytes, buffer);
		
		if(!instantiated)
		{
			last_error = "DRBG is not instantiated";
			return 0;
		}

		if(requested_strength > security_strength)
		{
			last_error = "Invalid requested_strength";
			return 0;
		}

		size_t num_of_threads;
		size_t num_of_procs = static_cast<size_t>(omp_get_num_procs());
		if(requested_num_of_threads == 0)
			num_of_threads = min(max_num_of_threads, num_of_procs);
		else
			num_of_threads = min(min(max_num_of_threads, num_of_procs), requested_num_of_threads);
		omp_set_num_threads(static_cast<int>(num_of_threads));
		
		while(reseed_counter == reseed_interval || s == biglong::one)
		{
			reseed(s, r_byte, "");
			reseed_counter = 0;
		}

		unsigned char* s_power_mod_bytes = nullptr;
		size_t s_power_mod_size = (s.power_mod(e, N))
			.get_raw_bytes(s_power_mod_bytes, n_byte);

		size_t num_of_bytes_per_thread = requested_num_of_bytes / num_of_threads + 1;
		size_t num_of_steps_per_thread = num_of_bytes_per_thread / k_byte + 1;
		buffer = new unsigned char[num_of_steps_per_thread*k_byte*num_of_threads];

		biglong thread_s;
		size_t thread_reseed_counter = reseed_counter;
#pragma omp parallel private(thread_s) shared(s_power_mod_bytes, num_of_steps_per_thread) firstprivate(thread_reseed_counter)
		{
			size_t thread_num = omp_get_thread_num();
			thread_s = biglong(s_power_mod_bytes +
			                   s_power_mod_size - (thread_num+1)*r_byte, r_byte);
			++thread_s;
			unsigned char* buffer_w_thread_offset =
				buffer + num_of_steps_per_thread*k_byte*thread_num;

			for(size_t i=0; i < num_of_steps_per_thread; ++i)
			{
				while(thread_reseed_counter >= reseed_interval || thread_s == biglong::one)
				{
					char thread_num_str[10];
					sprintf_s(thread_num_str, sizeof(thread_num_str),"%d", thread_num);
					reseed(thread_s, r_byte, thread_num_str);
					thread_reseed_counter = 0;
				}

				unsigned char* thread_s_power_mod_bytes = nullptr;
				size_t thread_s_power_mod_size = (thread_s.power_mod(e, N))
					.get_raw_bytes(thread_s_power_mod_bytes, n_byte);

				memcpy(buffer_w_thread_offset+i*k_byte, thread_s_power_mod_bytes, k_byte);
				thread_s = biglong(thread_s_power_mod_bytes+k_byte, r_byte);
				++thread_s;

				++thread_reseed_counter;

				delete[] thread_s_power_mod_bytes;
			}

#pragma omp master
			{
				s = thread_s;
				reseed_counter = thread_reseed_counter;
			}
		}
		delete[] s_power_mod_bytes;

		return 1;
	}
}