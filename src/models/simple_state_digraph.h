#pragma once


#include <set>
#include <map>

namespace tobor {
	namespace v1_1 {

		template<class State_T, class State_Label_T = void>
		class simple_state_digraph;

		template<class State_T>
		class simple_state_digraph<State_T, void> {
		public:

			using state_type = State_T;
			using state_set_type = std::set<state_type>;
			using state_label_type = void;

			struct node_links {
				std::set<state_type> predecessors;
				std::set<state_type> successors;
			};

			using map_type = std::map<state_type, node_links>;
			using map_iterator_type = typename map_type::iterator;
			using map_const_iterator_type = typename map_type::const_iterator;

			map_type map;

			inline void clear() {
				return map.clear();
			}
		};

		template<class State_T, class State_Label_T>
		class simple_state_digraph {
		public:
			using state_type = State_T;
			using state_set_type = std::set<state_type>;
			using state_label_type = State_Label_T;

			struct node_links {
				state_set_type predecessors;
				state_set_type successors;
				state_label_type labels;
			};

			using map_type = std::map<state_type, node_links>;
			using map_iterator_type = typename map_type::iterator;
			using map_const_iterator_type = typename map_type::const_iterator;

			map_type map;

			inline void clear() {
				return map.clear();
			}
		};

	}
}
