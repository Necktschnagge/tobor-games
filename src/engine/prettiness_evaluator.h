#pragma once

#include "bigraph_operations.h"
#include "quick_move_cache.h"
#include "../models/augmented_positions_of_pieces.h"
#include "../models/piece_id.h"
#include "../models/piece_move.h"
#include "../models/state_path.h"


#include <exception>

namespace tobor {
	namespace v1_1 {

		/**
		*	@brief A Collection of static member functions to obtain prettiest paths in (sub)graphs to reach a final state from the initial state.
		*/
		template<class Pieces_Quantity_T>
		class prettiness_evaluator {
		public:

			/*** TYPES ***/

			using pieces_quantity_type = Pieces_Quantity_T;

			using world_type = tobor::v1_1::dynamic_rectangle_world<uint16_t, uint8_t>;

			using cell_id_type = tobor::v1_1::min_size_cell_id<world_type>;

			using positions_of_pieces_type_interactive = tobor::v1_1::augmented_positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

			using state_path_type_interactive = tobor::v1_1::state_path<positions_of_pieces_type_interactive>;

			using positions_of_pieces_type_solver = tobor::v1_1::positions_of_pieces<pieces_quantity_type, cell_id_type, true, true>;

			using quick_move_cache_type = tobor::v1_1::quick_move_cache<world_type>;

			using piece_id_type = tobor::v1_1::piece_id<pieces_quantity_type>;

			using piece_move_type = tobor::v1_1::piece_move<piece_id_type>;

			using move_engine_type = tobor::v1_1::move_engine<cell_id_type, quick_move_cache_type, piece_move_type>;

			using  piece_quantity_int_type = typename pieces_quantity_type::int_type;


			/**
			*	@brief Node annotation for state graphs.
			*/
			struct piece_change_decoration {
				static constexpr std::size_t MAX{ std::numeric_limits<std::size_t>::max() };

				std::size_t min_piece_change_distance;
				std::vector<positions_of_pieces_type_interactive> optimal_successors;
				//std::size_t count_total_paths_from_here;

				piece_change_decoration(
					std::size_t min_piece_change_distance,
					std::vector<positions_of_pieces_type_interactive> optimal_successors
					//, std::size_t count_total_paths_from_here
				) :
					min_piece_change_distance(min_piece_change_distance),
					optimal_successors(optimal_successors)
					//, count_total_paths_from_here(count_total_paths_from_here)
				{}
			};


			using piece_change_decoration_vector = std::vector<piece_change_decoration>;

			using naked_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_solver, void>;

			/**
			*	@details Each state annotation is a vector which maps each piece [piece_id == vector's index] to the number of piece changes we need at minimum to move to final state
			*/
			using pretty_evaluation_bigraph_type = tobor::v1_1::simple_state_bigraph<positions_of_pieces_type_interactive, piece_change_decoration_vector>;

			using pretty_evaluation_bigraph_map_iterator_type = typename pretty_evaluation_bigraph_type::map_iterator_type;


			prettiness_evaluator() = delete;


			/**
			*	@brief Explores from map_iter_root recursively via successor states until final states and build decorations from final states to given initial state \p map_iter_root
			*	@details Purpose is counting min piece changes until final state for each currently selected piece from each state.
			*	Invariant that must be provided: If a state has labels then this state and all its direct and indirect successors must have been evaluated and their labels are set correclty.
			*	A state of \p pretty_evaluation_bigraph has labels by defintion if and only if the label vector is not empty.
			*	To build the entire decoration one should pass \p pretty_evaluation_bigraph with every label being an empty vector and \p map_iter_root pointing to the unique initial state.
			*
			*	@throw std::logic_error in case the given \p pretty_evaluation_bigraph is invalid.
			*	@throw move_engine::arithmetic_error::no_move in case given \p pretty_evaluation_bigraph refers to invalid moves in underlaying game.
			*	@throw move_engine::arithmetic_error::multi_move in case the given \p pretty_evaluation_bigraph refers to invalid moves in underlaying game.
			*/
			inline static void build_prettiness_decoration(pretty_evaluation_bigraph_type& pretty_evaluation_bigraph, pretty_evaluation_bigraph_map_iterator_type map_iter_root, const move_engine_type& engine) {
				if (!map_iter_root->second.labels.empty()) {
					return; // this map entry and all reachable direct and indirect successor states must have been decorated correctly
				}
				if (map_iter_root->second.successors.empty()) {
					// This is a final state, not yet decorated.
					for (std::size_t n{ 0 }; n < pieces_quantity_type::COUNT_ALL_PIECES; ++n) {
						map_iter_root->second.labels.emplace_back(
							0, // 0 piece changes left when in final state
							std::vector<positions_of_pieces_type_interactive>() // no successors
						);
					}
					return;
				}
				// we now have an undecorated state that is not final.
				//first make sure all successors have been decorated:
				for (const auto& succ : map_iter_root->second.successors) {
					auto map_jter = pretty_evaluation_bigraph.map.find(succ); ///// note this is slow since the bigraph does not use pointers, only values.
					if (map_jter == pretty_evaluation_bigraph.map.end()) {
						throw std::logic_error("Invalid bigraph! Successor announced which cannot be found in the graph.");
					}
					build_prettiness_decoration(pretty_evaluation_bigraph, map_jter, engine);
				}

				//now calculate current state's decoration using the successor decorations.

				// initialize with MAX distance
				for (std::size_t i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
					map_iter_root->second.labels.emplace_back(
						piece_change_decoration::MAX,
						std::vector<positions_of_pieces_type_interactive>() // no successors
					);
				}

				for (const auto& succ_state : map_iter_root->second.successors) {

					// find successor
					auto succ_jter = pretty_evaluation_bigraph.map.find(succ_state); ///// note this is slow since the bigraph does not use pointers, only values.

					// obtain SELECTED_PIECE id
					piece_move_type move = engine.state_minus_state(succ_state, map_iter_root->first); // throws move_engine::arithmetic_error::no_move, move_engine::arithmetic_error::multi_move
					const piece_quantity_int_type SELECTED_PIECE = move.pid.value;

					// obtain SELECTED_PIECE id after move
					positions_of_pieces_type_interactive from_state(map_iter_root->first);
					from_state.reset_permutation();
					positions_of_pieces_type_interactive to_state = engine.successor_state(from_state, move);

					const piece_quantity_int_type SELECTED_PIECE_AFTER{
						[&]() {
							for (piece_quantity_int_type i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) {
								if (to_state.permutation()[i] == SELECTED_PIECE) {
									return i;
								}
							}
							throw std::logic_error("Internal logic error during prettiness decoration.");
						}()
					};

					const std::size_t SUB_DISTANCE{ succ_jter->second.labels[SELECTED_PIECE_AFTER].min_piece_change_distance };

					for (piece_quantity_int_type i{ 0 }; i < pieces_quantity_type::COUNT_ALL_PIECES; ++i) { ///// can be optimized, we have the same loop when initializing SELECTED_PIECE_AFTER
						const std::size_t UPDATE_DISTANCE{ SUB_DISTANCE + (SELECTED_PIECE != i) };
						if (UPDATE_DISTANCE < map_iter_root->second.labels[i].min_piece_change_distance) {

							map_iter_root->second.labels[i].optimal_successors.clear();
							map_iter_root->second.labels[i].min_piece_change_distance = UPDATE_DISTANCE;
							map_iter_root->second.labels[i].optimal_successors.push_back(succ_state);

						}
					}
				}
			}

			/**
			*	@brief Returns one state path with max prettiness rating
			*	@param pretty_evaluation_bigraph needs to be decorated correctly by build_prettiness_decoration(...)
			*
			*	@thow std::logic_error in case given \p pretty_evaluation_bigraph has missing labels
			*	@thow std::logic_error in case given \p pretty_evaluation_bigraph has errors regarding successors
			*/
			inline static state_path_type_interactive get_representant(pretty_evaluation_bigraph_type& pretty_evaluation_bigraph, pretty_evaluation_bigraph_map_iterator_type map_iter_root) {
				state_path_type_interactive result;

				while (true) {
					if (map_iter_root == pretty_evaluation_bigraph.map.cend()) {
						throw std::logic_error("Successor not found.");
					}

					result.vector().push_back(map_iter_root->first);
					if (map_iter_root->second.labels.empty()) {
						throw std::logic_error("Label missing.");
					}

					auto piece_select_iter = std::min_element(
						map_iter_root->second.labels.cbegin(),
						map_iter_root->second.labels.cend(),
						[](const piece_change_decoration& l, const piece_change_decoration& r) { return l.min_piece_change_distance < r.min_piece_change_distance; }
					);

					if (piece_select_iter->optimal_successors.empty()) {
						return result;
					}
					const auto successor_state{ piece_select_iter->optimal_successors.front() };
					map_iter_root = pretty_evaluation_bigraph.map.find(successor_state);
				}
			}

			// TODO add another function to get all prettiest representants
			// TODO add another function returning all representants partitioned by their prettiness.

		};
	}
}
