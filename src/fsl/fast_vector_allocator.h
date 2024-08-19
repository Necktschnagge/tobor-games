#pragma once
#include <vector>
#include <list>
#include <mutex>
#include <map>

namespace fsl {

	static constexpr std::size_t KiB_1_EXPONENT{ 10 };
	static constexpr std::size_t MeB_1_EXPONENT{ 20 };
	static constexpr std::size_t MeB_16_EXPONENT{ 24 };
	static constexpr std::size_t MeB_128_EXPONENT{ 27 };
	static constexpr std::size_t GiB_1_EXPONENT{ 30 };
	static constexpr std::size_t GiB_2_EXPONENT{ 31 };
	static constexpr std::size_t GiB_4_EXPONENT{ 32 };

	template<std::size_t Block_Size_Exponent>
	class fast_vector_allocator {

	public:

		static constexpr std::size_t BLOCK_SIZE_EXPONENT{ Block_Size_Exponent };
		static_assert(BLOCK_SIZE_EXPONENT > 0, "Exponent must be greater zero.");

		static constexpr std::size_t BLOCK_SIZE{ 1 << (BLOCK_SIZE_EXPONENT - 1) };

		using vector_type = std::vector<char>;
		static_assert(sizeof(char) == 1, "Char must be of size 1 byte here");

		class mem_controller {

			struct entry {
				std::mutex _mutex;
				vector_type _vec;
				std::vector <char*> _active_allocations;
				char* _next_free;

				inline char* _end() { return _vec.end(); }

				inline bool _empty() { return _active_allocations.empty(); }

				inline std::size_t _free_space() { return _end() - _next_free; }

				/**
				* Does not check if enough space avaiblable!
				*/
				inline char* _alloc(std::size_t total_size) {
					char* const ptr{ _next_free };
					_next_free += total_size;
					_active_allocations.push_back(ptr);
					return ptr;
				}

				inline void _dealloc(char* ptr) {
					// ### log-time search for ptr in _active_allocations and delete it, following is linear:
					std::remove(_active_allocations.begin(), _active_allocations.end(), ptr);
					if (_empty()) _next_free = _vec.begin();
				}

				entry() :
					_vec(BLOCK_SIZE, char(0)),
					_active_allocations()
				{
					_next_free = _vec.begin();
				}

				entry(std::size_t min_size) :
					_vec(std::max(BLOCK_SIZE, min_size), char(0)),
					_active_allocations()
				{
					_next_free = _vec.begin();
				}
			};


			std::list<entry> _entries;
			std::mutex _entries_mutex;
			std::map<char*, typename std::list<entry>::iterator> _entry_finder;

			std::pair<entry&, std::lock_guard<std::mutex>> back_entry() {
				auto lock = std::lock_guard<std::mutex>(_entries_mutex);
				return std::make_pair(_entries.back(), std::lock_guard<std::mutex>(_entries.back()._mutex));
			}

		public:
			mem_controller() {}


			inline char* alloc(std::size_t total_size) {
				std::pair<entry&, std::lock_guard<std::mutex>> entry{ back_entry() };

				if (entry.first._free_space() < total_size) {
					// add a new entry and allocate there.
				}

				// allocate here
				return entry.first._alloc(total_size);
			}
			
			inline void dealloc(char* ptr) {
				// determine which entry is affected.
				// dealloc inside the right entry.

				// build a lookup table for entries by giving vector begin or vector end pointer.
				// check where given ptr belongs to.

				// check if entry is empty and can be deleted (must not be the last entry!)
			}

		};

		// static mem_controller mctrlr;

		template<class T>
		struct allocator {

			using value_type = T;

			allocator() {}

			template<class U>
			constexpr allocator(const allocator<U>&) noexcept {}

			[[nodiscard]] value_type* allocate(std::size_t n) {

			}

		}

	};

}
