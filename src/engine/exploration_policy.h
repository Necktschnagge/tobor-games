#pragma once

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief Describes restrictions for exploration.
		*/
		class exploration_policy {
		public:

			using size_type = std::size_t;

			static constexpr size_type SIZE_TYPE_MAX{ std::numeric_limits<size_type>::max() };

		private:

			size_type _state_count_threshold{ 0 };
			size_type _max_depth{ 0 };

			constexpr  exploration_policy(size_type state_count_threshold, size_type max_depth) : _state_count_threshold(state_count_threshold), _max_depth(max_depth) {}

		public:
			exploration_policy() = delete;



			static constexpr exploration_policy ONLY_CASHED() { return exploration_policy(0, 0); }

			static constexpr exploration_policy ONLY_EXPLORED() { return exploration_policy(0, SIZE_TYPE_MAX); }

			static constexpr exploration_policy FORCE_EXPLORATION_UNRESTRICTED() { return exploration_policy(SIZE_TYPE_MAX, SIZE_TYPE_MAX); }

			static constexpr exploration_policy FORCE_EXPLORATION_UNTIL_DEPTH(size_type max_depth) { return exploration_policy(SIZE_TYPE_MAX, max_depth); }

			static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD(size_type state_count_threshold) { return exploration_policy(std::max(state_count_threshold, size_type(1)), SIZE_TYPE_MAX); }

			static constexpr exploration_policy FORCE_EXPLORATION_STATE_THRESHOLD_UNTIL_DEPTH(size_type state_count_threshold, size_type max_depth) { return exploration_policy(std::max(state_count_threshold, size_type(1)), max_depth); }



			inline bool operator ==(const exploration_policy& another) const noexcept { return _state_count_threshold == another._state_count_threshold && _max_depth == another._max_depth; }

			inline size_type state_count_threshold() const { return _state_count_threshold; }

			inline size_type max_depth() const { return _max_depth; }

		};
	}
}
