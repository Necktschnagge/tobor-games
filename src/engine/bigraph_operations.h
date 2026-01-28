#pragma once

#include "move_engine.h"
#include "../models/simple_state_digraph.h"

namespace tobor {
	namespace v1_1 {

		class bigraph_operations {
		public:

			/**
			*	@brief requires single initial state in source bigraph
			*/
			template<
				class Source_State_T,
				class Source_Decoration_T,
				class Destination_State_T,
				class Destination_Decoration_T,
				class Cell_Id_T,
				class Quick_Move_Cache_T,
				class Piece_Move_T
			>
				requires std::same_as<typename Source_State_T::cell_id_type, Cell_Id_T>
			&&
				std::same_as<typename Destination_State_T::cell_id_type, Cell_Id_T>

				struct bigraph_simulation_copy {

				using source_bigraph_type = tobor::v1_1::simple_state_bigraph<Source_State_T, Source_Decoration_T>;
				using destination_bigraph_type = tobor::v1_1::simple_state_bigraph<Destination_State_T, Destination_Decoration_T>;

				using source_map_const_iterator = typename source_bigraph_type::map_const_iterator_type;
				using destination_map_iterator = typename destination_bigraph_type::map_iterator_type;

				using source_state_set_const_iterator = typename source_bigraph_type::state_set_type::const_iterator;

				struct simulation_copy_df_record {

					source_map_const_iterator source_map_it;
					source_state_set_const_iterator source_succ_it;
					destination_map_iterator destination_map_it;

				};

				using simulation_links_type = std::map<Source_State_T, std::set<Destination_State_T>>;

				bigraph_simulation_copy() = delete;

				inline static destination_map_iterator copy(
					const source_bigraph_type& source_bigraph,
					destination_bigraph_type& destination_bigraph,
					const Destination_State_T& initial_state_destination,
					const move_engine<Cell_Id_T, Quick_Move_Cache_T, Piece_Move_T>& engine
				) {
					destination_bigraph.clear();

					// find the initial state in source bigraph:
					source_map_const_iterator source_initial_state_iterator{ source_bigraph.map.cend() };
					for (auto iter = source_bigraph.map.cbegin(); iter != source_bigraph.map.cend(); ++iter) {
						if (iter->second.predecessors.empty()) {
							if (source_initial_state_iterator != source_bigraph.map.cend()) {
								throw 0; // error: more than 1 initial state
							}
							source_initial_state_iterator = iter;
						}
					}

					destination_map_iterator iter_destination_map_initial_state = destination_bigraph.map.insert(
						destination_bigraph.map.begin(),
						std::make_pair(initial_state_destination, typename destination_bigraph_type::node_links())
					);

					//auto iter = source_initial_state_iterator;

					simulation_links_type simulation_links; // should also be part of return value


					std::vector<simulation_copy_df_record> df_exploration_stack;
					df_exploration_stack.emplace_back();

					df_exploration_stack.back().source_map_it = source_initial_state_iterator;
					df_exploration_stack.back().source_succ_it = source_initial_state_iterator->second.successors.cbegin();
					df_exploration_stack.back().destination_map_it = iter_destination_map_initial_state;

					simulation_links[source_initial_state_iterator->first].insert(iter_destination_map_initial_state->first);


					while (!df_exploration_stack.empty()) {
						simulation_copy_df_record& current{ df_exploration_stack.back() };

						if (current.source_succ_it == current.source_map_it->second.successors.cend()) {
							// all successors have been explored here
							df_exploration_stack.pop_back();
							continue;
						}

						const auto& source_curr_state{ current.source_map_it->first };
						const auto& source_succ_state{ *current.source_succ_it };
						auto& destination_curr_state{ current.destination_map_it->first };

						++current.source_succ_it;

						Piece_Move_T move{ engine.state_minus_state(source_succ_state, source_curr_state) };

						Destination_State_T similar_successor_state = engine.successor_state(destination_curr_state, move);


						//auto& destination_succ_map_value = destination_bigraph.map[similar_successor_state]; // insert successor state, if not present
						auto [destination_succ_map_iter, inserted_destination_succ] = destination_bigraph.map.insert(
							std::make_pair(similar_successor_state, typename destination_bigraph_type::node_links())
						); // insert successor state, if not present

						//destination_succ_map_value.predecessors.insert(destination_curr_state); // insert predecessor state, if not present
						destination_succ_map_iter->second.predecessors.insert(destination_curr_state); // insert predecessor state, if not present

						current.destination_map_it->second.successors.insert(similar_successor_state); // insert successor state, if not present

						auto [pos, insert_took_place] = simulation_links[source_succ_state].insert(similar_successor_state);

						if (insert_took_place) {
							// found a new simulation_link, so recursive df exploration has to be made there:
							df_exploration_stack.emplace_back(); // invalidates current
							simulation_copy_df_record& sub{ df_exploration_stack.back() };

							sub.source_map_it = source_bigraph.map.find(source_succ_state); // ### not yet checked for end iterator!!!
							sub.source_succ_it = sub.source_map_it->second.successors.cbegin();
							sub.destination_map_it = destination_succ_map_iter;
						}

					}


					return iter_destination_map_initial_state;
				}
			};

		};


	}
}
