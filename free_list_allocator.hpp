#include  <cstdint>
#include <stdlib.h>
#include <iostream>

#include "easylogging++.h"

#include <cstdio>
#include <vector>


#include "allocator.hpp"
#include "poolallocator.hpp"

namespace memory_management
{

	template<typename T>
	class object_traits
	{
	public:		

		template<typename U>
		struct rebind
		{
			typedef object_traits<U> other;
		};

		// Constructor
		object_traits(void) {}

		// Copy Constructor
		template<typename U>
		object_traits(object_traits<U> const& other) {}

		// Address of object
		T*       address(T&       obj) const { return &obj; }
		T const* address(T const& obj) const { return &obj; }

		// Construct object
		void construct(T* ptr, T const& ref) const
		{
			// In-place copy construct
			new(ptr) T(ref);
		}

		// Destroy object
		void destroy(T* ptr) const
		{
			// Call destructor
			ptr->~T();
		}
	};


	template<typename T>
	struct max_allocations
	{
		enum { value = static_cast<std::size_t>(-1) / sizeof(T) };
	};


	
	template<typename T>
	class free_list
	{
	private:

		struct chunk
		{
			chunk* next;
			std::size_t size;
		};

		// free_list is meant to be used for sizable chunks of memory. 
		// small individual objects like floats and integers are problematic, 
		// because this free_list stores a header in the chunks of memory it doles 
		// out (a few bytes before the address it returns). so, if an individual object
		// is requested, we'll use a pool allocator instead. 
		pool_allocator<T> m_single_element_allocator;

		chunk* m_head;

		


	public:

		template<typename U>
		struct rebind
		{
			typedef free_list<U> other;
		};

		// Default Constructor
		free_list(void) {}

		// Copy Constructor
		template<typename U>
		free_list(free_list<U> const& other) {}

		// Allocate memory
		T* allocate(std::size_t count, const T* /* hint */ = 0)
		{
			if (count == 0)
				return nullptr;

			if (count == 1)
				return m_single_element_allocator.allocate();

			//get size of allocation
			auto allocation_size = (sizeof(T) + alignof(T)) * count;

			// find free chunk



			/*if (count > max_size()) { throw std::bad_alloc(); }
			return static_cast<T*>(::operator new(count * sizeof(type), ::std::nothrow));*/
		}

		// Delete memory
		void deallocate(T* ptr, std::size_t /* count */)
		{
			::operator delete(ptr);
		}

		// Max number of objects that can be allocated in one call
		std::size_t max_size(void) const { return max_allocations<T>::value; }
	};


}