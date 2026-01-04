#pragma once

#include <vector>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief Represents a path by its states.
		*/
		template<class Position_Of_Pieces_T>
		class state_path {
		public:

			using positions_of_pieces_type = Position_Of_Pieces_T;

			using vector_type = std::vector<positions_of_pieces_type>;

		private:

			vector_type _state_vector;

		public:

			state_path() {}

			state_path(const vector_type& v) : _state_vector(v) {}

			const vector_type& vector() const { return _state_vector; }

			vector_type& vector() { return _state_vector; }

			inline void make_canonical() {

				typename vector_type::size_type count_duplicates{ 0 };
				typename vector_type::size_type i = 0;

				while (i + count_duplicates + 1 < _state_vector.size()) {
					if (_state_vector[i] == _state_vector[i + count_duplicates + 1]) {
						++count_duplicates;
					}
					else {
						if (count_duplicates)
																																			_state_vector[i + 1] = _state_vector[i + count_duplicates + 1];
						++i;
					}
				}

				// now i + count_duplicates + 1 == _state_vector.size()
				_state_vector.erase(_state_vector.begin() + i + 1, _state_vector.end());
			}

			inline state_path operator +(const state_path& another) const {
				state_path copy{ *this };
				std::copy(another._state_vector.cbegin(), another._state_vector.cend(), std::back_inserter(copy._state_vector));
				return copy;
			}

			inline state_path operator +(const positions_of_pieces_type& s) const {
				state_path copy{ *this };
				copy.vector().push_back(s);
				return copy;
			}

			inline state_path& operator +=(const state_path& another) {
				_state_vector.reserve(_state_vector.size() + another._state_vector.size());
				std::copy(another._state_vector.cbegin(), another._state_vector.cend(), std::back_inserter(_state_vector));
				return *this;
			}

			inline state_path& operator +=(const positions_of_pieces_type& s) {
				_state_vector.push_back(s);
				return *this;
			}

			inline state_path operator *(const state_path& another) {
				if (another._state_vector.empty())
					return *this;
				if (this->_state_vector.empty())
					return another;
				if (_state_vector.back() == another._state_vector.front()) {
					state_path copy = *this;
					std::copy(
						another._state_vector.cbegin() + 1,
						another._state_vector.cend(),
						std::back_inserter(copy._state_vector)
					);
					return copy;
				}
				else {
					return state_path();
				}
			}

		};

	}
}
