#pragma once

#include "../models/state_path.h"
#include "../models/simple_state_digraph.h"

#include <vector>

namespace tobor {
	namespace v1_1 {


		/**
		*	@brief Utility class for classifying paths into equivalence classes.
		*/
		template<class Positions_Of_Pieces_T>
		class path_classificator {

			inline static bool contains(const std::vector<bool>& flag_vector, std::size_t flag_index) {
				return (flag_index < flag_vector.size()) && flag_vector[flag_index];
			}

			inline static void set_flag(std::vector<bool>& flag_vector, std::size_t flag_index, bool value = true, bool default_value = false) {
				if (flag_index < flag_vector.size()) {
					flag_vector[flag_index] = true;
					return;
				}
				const std::size_t insert_size{ flag_index + 1 - flag_vector.size() };
				flag_vector.insert(flag_vector.end(), insert_size, default_value);
				flag_vector[flag_index] = value;
			}

		public:

			using positions_of_pieces_type = Positions_Of_Pieces_T;

			using state_path_type = state_path<positions_of_pieces_type>;

			using state_path_vector_type = typename state_path_type::vector_type;

		private:

			/**
			*	@brief Extracts all state paths of given simple_state_digraph \p source with prefix \p depth_first_path and writes them into \p all_state_paths using emplace_back()
			*/
			template<class State_Label_T>
			static void extract_all_state_paths_helper(
				const simple_state_digraph<positions_of_pieces_type, State_Label_T>& source,
				std::vector<state_path<positions_of_pieces_type>>& all_state_paths,
				state_path_vector_type& depth_first_path
			) {
				auto iter = source.map.find(depth_first_path.back());

				if (iter == source.map.cend()) {
					return; // Never reached by logic when used correctly.
					// should throw std::unexpected();
				}

				if (
					iter->second.successors.empty()
					) {
					all_state_paths.emplace_back(depth_first_path);
					return;
				}
				for (const auto& succ : iter->second.successors) {
					depth_first_path.push_back(succ);
					extract_all_state_paths_helper(source, all_state_paths, depth_first_path);
					depth_first_path.pop_back();
				}
			}

		public:

			/**
			*	@brief Augments the states of \p digraph by flags indicating all the equivalence classes a state belongs to.
			*
			*	@return Number of partitions found.
			*/
			static std::size_t make_state_graph_path_partitioning(simple_state_digraph<positions_of_pieces_type, std::vector<bool>>& digraph) {

				// ## This function needs to be fixed in order to include crossed components of equivalence classes (which are left in this version.)

				/*
				std::vector<position_of_pieces_type> initials;
				std::vector<position_of_pieces_type> finals;

				for (auto& entry : digraph.map) {
					entry.second.labels.clear();
					if (entry.second.predecessors.empty()) {
						initials.push_back(entry.first);
					}
					if (entry.second.successors.empty()) {
						finals.push_back(entry.first);
					}
				}
				*/

				std::size_t flag_index{ 0 };
				for (auto iter = digraph.map.begin(); iter != digraph.map.end(); ++iter) {
					/* while there is a state not being part of any path partition */
					if (iter->second.labels.empty()) {
						// found iter pointing to a state not belonging to any partition/ i.e. has no label

						std::vector<decltype(digraph.map.begin())> exploration_iterator_stack; // collect iterators for elements in partition
						exploration_iterator_stack.reserve(digraph.map.size());
						exploration_iterator_stack.push_back(iter);

						// add new label to *iter state and to all state on some initial path.
						set_flag(iter->second.labels, flag_index, true);

						{
							auto i_back = iter;
							while (!i_back->second.predecessors.empty()) { // can be optimized
								i_back = digraph.map.find(*i_back->second.predecessors.begin());
								// i_back != end() /* assured by logic, also check it here (?)*/
								if (i_back == digraph.map.end()) break;
								set_flag(i_back->second.labels, flag_index, true);
								exploration_iterator_stack.push_back(i_back);
							}
						}
						{
							auto i_forward = iter;
							while (!i_forward->second.successors.empty()) { // can be optimized
								i_forward = digraph.map.find(*i_forward->second.successors.begin());
								if (i_forward == digraph.map.end()) break;
								// i_forward != end() /* assured by logic, also check it here (?)*/
								set_flag(i_forward->second.labels, flag_index, true);
								exploration_iterator_stack.push_back(i_forward);
							}
						}

						// add all other states to this partition which can be reached by true interleaving:

						/*
						Theory:
							A new element can be found from two common states embracing an interleaving
								A	---->	B1
								-			-
								-			-
								->			->
								B2	---->	C

							wlog we already know B1 but not B2. B2 can be found exploring from A and exploring from C

							use the following algorithm
							Put all known states into a stack.
							while stack not empty pop and explore from that element, always explore in both directions
								if found any new state of same partition, push it onto the stack.
							loop until stack empty.

							This way all states of the same partition will be found.

							::PROOF:: fairly simple, using contradiction:

							Assume Partition = A setunion B
							where all A have been found, but none of B.
							Since all of b are reachable by statewise interleaving from A,
							there must be some b2 and a1 ---> a2 ---> a3 such that also a1 ---> b2 ---> a3.
							So assuming b2 was not found when the stack got empty.
							This means that when a3 was popped, at this moment a1 has not yet been found.
							Otherwise exploration would have found also b2.
							So a1 was found after a3 was popped.
							a1 explored after a1 found, a1 found after a3 popped, a3 popped after a3 found.
							Thus, a1 explored after a3 found.
							Hence b2 was found when a1 got explored. Contradiction.
						*/
						while (!exploration_iterator_stack.empty()) {
							auto exploree = exploration_iterator_stack.back();
							exploration_iterator_stack.pop_back();

							for (const auto& candidate : exploree->second.successors) {
								const auto i_candidate = digraph.map.find(candidate);
								if (i_candidate == digraph.map.end()) continue; // never happens by logic if digraph is sound.
								if (contains(i_candidate->second.labels, flag_index)) continue; // state already labeled as part of current equivalence class

								for (const auto& successor : i_candidate->second.successors) {
									auto i_successor = digraph.map.find(successor);
									if (i_successor == digraph.map.end()) continue; // never happens by logic if digraph is sound.

									if (contains(i_successor->second.labels, flag_index)) {
										if (exploree->first.count_changed_pieces(i_successor->first) == 2) { // true interleaving
											// accept candidate here:
											exploration_iterator_stack.push_back(i_candidate);
											set_flag(i_candidate->second.labels, flag_index);
											goto candidate_accepted_1;
										}
									}
								}
							candidate_accepted_1:
								(void)0;
							}
							for (const auto& candidate : exploree->second.predecessors) {
								const auto i_candidate = digraph.map.find(candidate);
								if (i_candidate == digraph.map.end()) continue; // never happens by logic if digraph is sound.
								if (contains(i_candidate->second.labels, flag_index)) continue; // state already labeled as part of current equivalence class

								for (const auto& predecessor : i_candidate->second.predecessors) {
									auto i_predecessor = digraph.map.find(predecessor);
									if (i_predecessor == digraph.map.end()) continue; // never happens by logic if digraph is sound.

									if (contains(i_predecessor->second.labels, flag_index)) {
										if (exploree->first.count_changed_pieces(i_predecessor->first) == 2) { // true interleaving
											// accept candidate here:
											exploration_iterator_stack.push_back(i_candidate);
											set_flag(i_candidate->second.labels, flag_index);
											goto candidate_accepted_2;
										}
									}
								}
							candidate_accepted_2:
								(void)0;
							}
						}

						++flag_index;
					}

				}

				return flag_index;
			}


			/**
			*	@brief Extracts the induced subgraph by the states with flag \p label_index. The extracted subgraph then shows oen single equivalence class.
			*/
			template<class State_Label_T>
			static void extract_subgraph_by_label(
				const simple_state_digraph<positions_of_pieces_type, std::vector<bool>>& source,
				std::size_t label_index,
				simple_state_digraph<positions_of_pieces_type, State_Label_T>& destination
			) {
				destination.map.clear();

				const auto has_label{
					[&](const positions_of_pieces_type& e) {
					auto pos = source.map.find(e);
					const bool found_label = pos != source.map.end() && contains(pos->second.labels, label_index);
					return found_label;
					}
				};

				for (const auto& pair : source.map) {
					if (contains(pair.second.labels, label_index)) {
						auto iter = destination.map.insert(
							destination.map.end(),
							std::make_pair(pair.first, typename simple_state_digraph<positions_of_pieces_type, State_Label_T>::node_links())
						);
						std::copy_if(pair.second.predecessors.cbegin(), pair.second.predecessors.cend(), std::inserter(iter->second.predecessors, iter->second.predecessors.end()), has_label);
						std::copy_if(pair.second.successors.cbegin(), pair.second.successors.cend(), std::inserter(iter->second.successors, iter->second.successors.end()), has_label);
					}
				}
			}

			/**
			*	@brief Returns all state paths of given simple_state_digraph \p source.
			*/
			template<class State_Label_T>
			static std::vector<state_path<positions_of_pieces_type>> extract_all_state_paths(const simple_state_digraph<positions_of_pieces_type, State_Label_T>& source) {
				std::vector<state_path<positions_of_pieces_type>> all_state_paths;

				for (auto iter = source.map.cbegin(); iter != source.map.cend(); ++iter) {
					state_path_vector_type depth_first_path{ iter->first };
					if (iter->second.predecessors.empty()) {
						extract_all_state_paths_helper(source, all_state_paths, depth_first_path);
					}
				}
				return all_state_paths;
			}
		};


	}
}
