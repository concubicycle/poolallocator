#ifndef ___ALLOCATOR_BASE_H_
#define ___ALLOCATOR_BASE_H_





namespace memory_management
{

	template<typename T>
	class allocator_base
	{
	protected: 

		void prepare_memory(std::uint8_t* mem, std::uint32_t number_of_chunks)
		{
			LOG(INFO) << "--Preparing Memory--" << std::endl;
			for (std::uint32_t chunk_index = 0; chunk_index<number_of_chunks; ++chunk_index)
			{
				LOG(INFO) << "preparing address: " << (std::uint64_t)mem + (chunk_index * sizeof(T)) << std::endl;
				std::uint8_t* current_chunk = mem + (chunk_index * sizeof(T));


				LOG(INFO) << "setting linked list pointer: " << (std::uint64_t) current_chunk << std::endl;
				*((std::uint8_t**)current_chunk) = current_chunk + sizeof(T);


				LOG(INFO) << "setting terminatin null " << std::endl;
				*((std::uint8_t**)&mem[number_of_chunks * sizeof(T)]) = 0; /* terminating NULL */
			}
		}
	};


}
#endif