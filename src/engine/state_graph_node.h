#pragma once


#include <utility>
#include <vector>

namespace tobor {
	namespace v1_0 {


		/**
		* @brief A struct for all information on a state graph node:
			Iterators to optimal predecessor states and the connecting move for each predecessor,
			The number of successors where this is an optimal predecessor
			The optimal number of steps from initial state
		*/
		template <class Positions_Of_Pieces_Type, class Piece_Move_Type>
		struct state_graph_node {
		public:

			/* dependent types... */

			using type = state_graph_node<Positions_Of_Pieces_Type, Piece_Move_Type>;

			using positions_of_pieces_type = Positions_Of_Pieces_Type;

			using cell_id_type = typename positions_of_pieces_type::cell_id_type;

			using piece_move_type = Piece_Move_Type;



			/* type consistency */

			static_assert(
				std::is_same<typename positions_of_pieces_type::pieces_quantity_type, typename piece_move_type::pieces_quantity_type>::value,
				"Incompatible template arguments: typename Position_Of_Pieces_T::pieces_quantity_type must equal typenname Piece_Move_Type::pieces_quantity_type"
				);



			/* introduced types... */

			using partial_solutions_map_type = std::map<positions_of_pieces_type, type>;

			using map_iterator_type = typename partial_solutions_map_type::iterator;

			using predecessor_tuple = std::tuple<map_iterator_type, piece_move_type>;

			using size_type = std::size_t;

			static constexpr size_type MAX{ std::numeric_limits<size_type>::max() };


			struct move_candidate {

				/** The move consisting of piece_id and direction. */
				piece_move_type move;

				/** The successor state */
				positions_of_pieces_type successor_state;

				/** True if the position indeed changes on this move */
				bool is_true_move;

				move_candidate(
					const piece_move_type& move,
					const std::pair<positions_of_pieces_type, bool>& n
				) :
					move(move),
					successor_state(n.first),
					is_true_move(n.second)
				{
				}

			};


			/* data... */

			std::vector<predecessor_tuple> optimal_predecessors;

			size_type smallest_seen_step_distance_from_initial_state{ MAX };

			size_type count_successors_where_this_is_one_optimal_predecessor{ 0 }; // is leaf iff == 0, warning: can be 0 before exploration or after
		};

	}
}
