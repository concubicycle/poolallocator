/*Implementation based on Game Engine Programming*/

//TODO: un-C-ify maybe

#ifndef ___STLPOOLALLOCATOR_
#define ___STLPOOLALLOCATOR_

#include  <cstdint>
#include <stdlib.h>
#include <iostream>

#include "easylogging++.h"

#include <cstdio>
#include <vector>


#include "allocator.hpp"

namespace memory_management
{

	template<typename T, std::uint32_t size_bytes = 4096>
	class pool_allocator : allocator_base<T>
	{
	private:
		// trying to take this out of the class, which is allowed in c++, breaks msvc compiler. cool. 
		class pool_mem_block
		{
		public:
			std::uint8_t* mem;
			std::uint32_t offset;

			pool_mem_block(uint32_t block_size_bytes)
			{
				//Use non-aligned alloc to get memory
				std::uint64_t raw_address = std::uint64_t(malloc(block_size_bytes));

				//Calculate adjustment by masking off the lower bits of the address,
				//to determine how 'misaligned' it is.
				std::uint64_t mask = (alignof(T)-1);
				std::uint64_t misalignment = (raw_address & mask);
				std::uint64_t adjustment = alignof(T)-misalignment;
				std::uint64_t aligned_address = raw_address + adjustment;

				mem = (std::uint8_t*)aligned_address;
				offset = adjustment;

				LOG(INFO) << "-- Initializing block with size " << size_bytes << "--" << std::endl;
				LOG(INFO) << "block_size_bytes: " << block_size_bytes << std::endl;
				LOG(INFO) << "alignment: " << alignof(T) << std::endl;
				LOG(INFO) << "misalignment: " << misalignment << std::endl;
				LOG(INFO) << "adjustment: " << adjustment << std::endl;
				LOG(INFO) << "raw_address: " << raw_address << std::endl;
				LOG(INFO) << "aligned_address: " << aligned_address << std::endl;
				LOG(INFO) << std::endl;
			}

			pool_mem_block() {}

			~pool_mem_block()
			{
				// shouldn't be deleting the actual memory, even if we move this object. 
			}

			pool_mem_block(const pool_mem_block& other) : //copy
				mem(other.mem), offset(other.offset)
			{
			}

			pool_mem_block(pool_mem_block&& other) noexcept : //move
				mem(other.mem), offset(other.offset)
			{
			}

			pool_mem_block& operator=(const pool_mem_block& other) // copy 
			{
				pool_mem_block tmp(other);  // re-use copy-constructor
				*this = std::move(tmp);		// re-use move-assignment
				return *this;
			}

			pool_mem_block& operator=(pool_mem_block&& other) // move 
			{
				mem = other.mem;
				offset = other.offset;

				return *this;
			}

			void free_block()
			{
				std::uint64_t malloc_ret_address;

				malloc_ret_address = (std::uint64_t)(mem) - offset;
				free((std::uint8_t*)(malloc_ret_address));
			}
		};





		std::uint32_t m_size;
		std::uint32_t m_capacity;

		std::vector<pool_mem_block> m_mem_blocks;
		std::uint16_t	m_mem_block_count;

		std::uint8_t* m_head;



	public:

		pool_allocator()
		{
			if (size_bytes == 0)
				return;

			m_size = 0;

			m_mem_blocks.reserve(128);

			m_capacity = (size_bytes / sizeof(T)) - 1;
			LOG(INFO) << "m_capacity: " << m_capacity << std::endl;

			//Determine total amount of memory to allocate
			std::uint32_t expanded_size_bytes = size_bytes + alignof(T);

			//allocate a pointer to the first memory block.
			m_mem_blocks.emplace_back(pool_mem_block(expanded_size_bytes));
			prepare_memory(m_mem_blocks[0].mem, m_capacity);
			m_head = m_mem_blocks.back().mem;

			return;
		}

		~pool_allocator()
		{
			for (auto block : m_mem_blocks)
				block.free_block();
		}



		void linked_expand(std::uint32_t size_bytes)
		{
			LOG(INFO) << "--Expanding with " << size_bytes << " bytes.--" << std::endl;

			std::uint32_t expanded_size = size_bytes + alignof(T);
			m_mem_blocks.emplace_back(pool_mem_block(expanded_size));

			std::uint32_t count = (size_bytes / sizeof(T)) - 1;
			m_capacity += count;

			prepare_memory(m_mem_blocks.back().mem, count);

			//link memory blocks
			*((std::uint8_t**)m_head) = m_mem_blocks.back().mem;

			LOG(INFO) << "head is: " << (std::uint64_t) m_head << std::endl;
			LOG(INFO) << "m_size is: " << m_size << std::endl;
			LOG(INFO) << "m_capacity is: " << m_capacity << std::endl;

			LOG(INFO)
				<< std::endl
				<< "Expanded pool allocator, linked to mem address "
				<< (std::uint64_t)m_mem_blocks.back().mem
				<< " to "
				<< (std::uint64_t)(m_mem_blocks.back().mem + (count * sizeof(T))) << std::endl;
		}

		T* allocate()
		{
			LOG(INFO) << "--Allocating--" << std::endl;

			if (!m_head) return 0;

			if (m_size >= m_capacity - 1) {
				linked_expand((m_size + 1) * sizeof(T));
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
			LOG(INFO) << "--Freeing: " << (uint64_t)ptr << "--" << std::endl;

			if (!ptr)
				return;

			*((std::uint8_t**)ptr) = m_head;
			m_head = (std::uint8_t*)ptr;
			m_size--;
		}
	};

}




#endif