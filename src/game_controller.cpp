#include "predefined.h"

#include "game_controller.h"


#if false
GameController::move_path_type& GameController::get_selected_solution_representant(std::size_t index) {
	for (auto& pair : optional_classified_move_paths.value()) {
		//for (auto& equi_class : pair.second) {
		if (index < pair.second.size()) {
			return pair.second[index].front();
		}
		index -= pair.second.size();
		//}

	}
	// out of range
	showErrorDialog("selected solution index out of range, try to use the first solution instead.");
	if (optional_classified_move_paths.value().empty()) {
		showErrorDialog("no solution found at all [1]");
		throw std::runtime_error("should select a solution where there is no solution [1]");
	}

	if (optional_classified_move_paths.value().begin()->second.empty()) {
		showErrorDialog("no solution found in first class [2]");
		throw std::runtime_error("should select a solution where there is no solution [2]");
	}
	if (optional_classified_move_paths.value().begin()->second.front().empty()) {
		showErrorDialog("no solution found in first class [3]");
		throw std::runtime_error("should select a solution where there is no solution [3]");
	}
	return optional_classified_move_paths.value().begin()->second.front().front();
}
#endif

void GameController::start_solver(std::function<void(const std::string&)> status_callback) {
	/* callback looks like this:
	mw->statusBar()->showMessage("Extracting solution state graph...");
	mw->repaint();
	*/

	_solver.emplace(current_state(),_target_cell,_move_engine, status_callback);

	_solver_begin_index = _path.vector().size();

	if (_solver.value().solutions_size() == 0) {
		// error dialog message!!!####
		return stop_solver();
	}

	if (status_callback) status_callback("Successfully executed solver.");
}

void GameController::move_by_solver(bool forward) {
	if (!_solver) return;

	const auto index_next_move = _path.vector().size() - _solver_begin_index;

	const auto index_next_state = index_next_move + 1;

	if (forward) {
		if (is_final())
			return;
		_path += _solver.value().get_solution_state_path(_solution_index).vector()[index_next_state];
	}
	else { // back
		if (index_next_move == 0) // already at solver start
			return;
		_path.vector().pop_back();
	}
}


