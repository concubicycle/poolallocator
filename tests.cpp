
#include <cstdio>

#include <stdio.h>

#include <vector>



// comment out for logging
#define ELPP_DISABLE_LOGS 1
#define ELPP_DISABLE_INFO_LOGS 1

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


#include "stlpoolallocator.hpp"
#include <ctime>
#include <chrono>






typedef struct sample_data_structure
{
	float some_float;
	std::uint64_t some_word;
	std::uint8_t some_byte;
	void* some_pointer;

} sample_data_structure;



int main()
{
	int allocations = 100000;
	int iterations = 1;

	fflush(stdin);

	pool_allocator<sample_data_structure> allocator;
	

	std::vector<sample_data_structure*> elements;

	auto start = std::chrono::system_clock::now();

	while(iterations-- > 0)
	{
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i << std::endl; 
			elements.push_back(allocator.allocate());
		}

		LOG(INFO) << std::endl;
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << (uint64_t)elements[i] << std::endl;
		}
		LOG(INFO) << std::endl;


		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i << std::endl; 
			allocator.deallocate(elements[i]);
		}


		elements.clear();


		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i << std::endl; 
			elements.push_back(allocator.allocate());
		}
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i << std::endl; 
			allocator.deallocate(elements[i]);
		}
	}

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now() - start);

	std::cout << std::endl << std::endl << "ELAPSED TIME (ms): " << duration.count() << std::endl;

	std::getchar();
	return 0;
}