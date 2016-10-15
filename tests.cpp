#include <cstdio>
#include <vector>


// comment out for logging
#define ELPP_DISABLE_LOGS 1
#define ELPP_DISABLE_INFO_LOGS 1

#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


#include "poolallocator.hpp"
#include "free_list_allocator.hpp"

#include <ctime>
#include <chrono>


using namespace memory_management;



typedef struct sample_data_structure
{
	float some_float;
	std::uint64_t some_word;
	std::uint8_t some_byte;
	void* some_pointer;

} sample_data_structure;



void configure_logging()
{
	el::Configurations defaultConf;
	defaultConf.setToDefault();
	
	// Values are always std::string
	defaultConf.set(el::Level::Info,
		el::ConfigurationType::Format, "%datetime %level %msg");

	// default logger uses default configurations
	el::Loggers::reconfigureLogger("default", defaultConf);
	el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
	el::Loggers::removeFlag(el::LoggingFlag::NewLineForContainer);
	el::Loggers::reconfigureAllLoggers(el::ConfigurationType::Format, "%msg");
}



int main()
{
	configure_logging();




	int allocations = 1000000;
	int iterations = 10;

	fflush(stdin);

	pool_allocator<sample_data_structure> allocator;




	std::vector<sample_data_structure*> elements;

	auto start = std::chrono::system_clock::now();

	while(iterations-- > 0)
	{
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i;
			elements.push_back(allocator.allocate());
		}

		LOG(INFO) << std::endl;
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << (uint64_t)elements[i] << std::endl;
		}		


		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i;
			allocator.deallocate(elements[i]);
		}


		elements.clear();


		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i;
			elements.push_back(allocator.allocate());
		}
		for(int i = 0; i < allocations; i++)
		{
			LOG(INFO) << i;
			allocator.deallocate(elements[i]);
		}
	}

	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::system_clock::now() - start);

	std::cout << std::endl << std::endl << "ELAPSED TIME (ms): " << duration.count() << std::endl;

	std::getchar();
	return 0;
}