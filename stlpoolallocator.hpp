/*Implementation based on Game Engine Programming*/

//TODO: un-C-ify maybe

#ifndef ___STLPOOLALLOCATOR_
#define ___STLPOOLALLOCATOR_


#include  <cstdint>
#include <stdlib.h>
#include <iostream>

#include "easylogging++.h"


#include <cstdio>



typedef struct pool_mem_block
{
private:
	static std::uint32_t next_id;
	
public:
	std::uint8_t* mem;
	//std::uint32_t size;
	std::uint32_t offset;	
} pool_mem_block;





template<typename T, std::uint32_t size_bytes = 1024>
class pool_allocator
{
private:
	std::uint32_t m_size;
	std::uint32_t m_capacity;

	//new:
	pool_mem_block* m_mem_blocks;
	std::uint16_t	m_mem_block_count;
	std::uint8_t* m_head;
	std::uint8_t* m_mem_block;


	void prepare_memory(std::uint8_t* mem, std::uint32_t number_of_chunks)
	{	
		LOG(INFO) << std::endl;

		for (std::uint32_t chunk_index = 0; chunk_index<number_of_chunks; ++chunk_index)
		{
			LOG(INFO) << "preparing address: "  << (std::uint64_t)mem + (chunk_index * sizeof(T)) << std::endl;
			std::uint8_t* current_chunk = mem + (chunk_index * sizeof(T));


			LOG(INFO) << "setting linked list pointer: " << (std::uint64_t) current_chunk << std::endl;
			*((std::uint8_t**)current_chunk) = current_chunk + sizeof(T);	


			LOG(INFO) << "setting terminatin null " << std::endl;		
			*((std::uint8_t**)&mem[number_of_chunks * sizeof(T)]) = 0; /* terminating NULL */	
		}

		LOG(INFO) << std::endl;	
	}

public:
	pool_allocator()  
	{
		if (size_bytes == 0)
			return;
		m_capacity = (size_bytes / sizeof(T)) - 1;

		//Determine total amount of memory to allocate
		std::uint32_t expanded_size_bytes = size_bytes + alignof(T);

		//Use non-alligned alloc to get memory
		std::uint64_t raw_address = std::uint64_t(malloc(expanded_size_bytes));

		//Calculate adjustment by masking off the lower bits of the address,
		//to determine how 'misaligned' it is.
		std::uint64_t mask = (alignof(T)-1);
		std::uint64_t misalignment = (raw_address & mask);
		std::uint64_t adjustment = alignof(T)-misalignment;
		std::uint64_t aligned_address = raw_address + adjustment;

		LOG(INFO) << "Initializing with size: " << size_bytes << std::endl;
		LOG(INFO) << "m_capacity: " << m_capacity << std::endl;
		LOG(INFO) << "expanded_size_bytes: " << expanded_size_bytes << std::endl;
		LOG(INFO) << "alignment: " << alignof(T) << std::endl;
		LOG(INFO) << "misalignment: " << misalignment << std::endl;
		LOG(INFO) << "adjustment: " << adjustment << std::endl;
		LOG(INFO) << "raw_address: " << raw_address << std::endl;
		LOG(INFO) << "aligned_address: " << aligned_address << std::endl;
		LOG(INFO) << std::endl;


		//allocate a pointer to the first memory block.
		m_mem_blocks = (pool_mem_block*)malloc(sizeof(pool_mem_block));
		m_mem_block_count = 1;

		//init the memblock
		m_mem_blocks[0].mem = (std::uint8_t*)aligned_address;
		//m_mem_blocks[0].size = expanded_size_bytes - adjustment;
		m_mem_blocks[0].offset = adjustment;

		prepare_memory(m_mem_blocks[0].mem, m_capacity);
		m_mem_block = m_head = m_mem_blocks[0].mem;
		m_size = 0;
		return;
	}

	~pool_allocator() 
	{
		free_pool();
	}



	void linked_expand(std::uint32_t size_bytes)
	{
		LOG(INFO) << "Expanding with " << size_bytes << " bytes." << std::endl;

		
		std::uint32_t count = (size_bytes / sizeof(T)) - 1;
				
		//Determine total amount of memory to allocate
		std::uint32_t expanded_size = size_bytes + alignof(T);



		LOG(INFO) << "Allocating " << expanded_size << " bytes." << std::endl;


		//Use non-alligned alloc to get memory
		std::uint64_t raw_address = std::uint64_t(malloc(expanded_size));		


		//Calculate adjustment by masking off the lower bits of the address,
		//to determine how 'misaligned' it is.
		std::uint64_t mask = (alignof(T)-1);
		std::uint64_t misalignment = (raw_address & mask);
		std::uint32_t adjustment = alignof(T)-misalignment;
		std::uint64_t aligned_address = raw_address + adjustment;
		std::uint8_t* new_mem_block = (std::uint8_t*)aligned_address;



		LOG(INFO) << (void*) new_mem_block << std::endl;
		LOG(INFO) << "alignment: "  << alignof(T) << std::endl;		
		LOG(INFO) << "misalignment: "  << misalignment << std::endl;
		LOG(INFO) << "adjustment: "  << adjustment << std::endl;	
		LOG(INFO) << "raw_address: "  << raw_address << std::endl;
		LOG(INFO) << "aligned_address: "  << aligned_address << std::endl;		


		prepare_memory(new_mem_block, count);


		LOG(INFO) 
			<< std::endl
			<< "Expanded pool allocator, linked to mem address " 
			<< (std::uint64_t)new_mem_block 
		 	<< " to " 
		 	<< (std::uint64_t)(new_mem_block + (count * sizeof(T))) << std::endl;


		m_mem_block_count++;
		m_mem_blocks = (pool_mem_block*)realloc(m_mem_blocks, sizeof(pool_mem_block) * m_mem_block_count);

		m_mem_blocks[m_mem_block_count - 1].mem = new_mem_block;
		//m_mem_blocks[m_mem_block_count - 1].size = expanded_size - adjustment;
		m_mem_blocks[m_mem_block_count - 1].offset = adjustment;

		std::uint8_t* last_mem_block = m_mem_blocks[m_mem_block_count - 2].mem;
		//std::uint32_t lastm_mem_blocksize = m_mem_blocks[m_mem_block_count - 2].size;

		//link memory blocks
		*((std::uint8_t**)m_head) = new_mem_block;


		m_capacity += count;

		LOG(INFO) << "head is: " << (std::uint64_t) m_head << std::endl;
		LOG(INFO)<<"m_size is: " << m_size << std::endl;
		LOG(INFO)<<"m_capacity is: " << m_capacity << std::endl;
	}

	T* allocate()
	{
		LOG(INFO) << "Allocating" << std::endl;



		if (!m_head) return 0;

		if(m_size == m_capacity-1) {
			linked_expand((m_size+1) * sizeof(T));
		}

		std::uint8_t* current = m_head;

		m_head = *((std::uint8_t**)m_head);

		
		if (m_head)
			LOG(INFO) << "m_head is now " << (std::uint64_t)m_head << std::endl;
		else
			LOG(INFO) << "m_head is now null, allocator out of memory" << std::endl;

		m_size++;

		LOG(INFO) << "Returning address " << (std::uint64_t)current << std::endl;
		return (T*)current;
	}

	void deallocate(T* ptr)
	{
		LOG(INFO) << "Freeing: " << (uint64_t)ptr << std::endl;

		if (!ptr)
		return;
		
		*((std::uint8_t**)ptr) = m_head;
		m_head = (std::uint8_t*)ptr;
		m_size--;
	}
	
	
	void free_pool()
	{
		std::uint64_t malloc_ret_address;
		
		for (int i = 0; i < m_mem_block_count; i++)
		{
			malloc_ret_address = (std::uint64_t)(m_mem_blocks[i].mem) - m_mem_blocks[i].offset;
			free((std::uint8_t*)(malloc_ret_address));
		}

		free(m_mem_blocks);
	}
};



#endif