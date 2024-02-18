#include "key_event_filter.h"

#include "mainwindow.h"



#include<QKeyEvent>


bool ControlKeyEventAgent::eventFilter(QObject* object, QEvent* e)
{
	(void)object;

	QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);

	if (keyEvent == nullptr) {

		return false; //pass-through
	}

	// found a keyEvent

	if (mainWindow->guiInteractiveController.interactiveMode() == GuiInteractiveController::InteractiveMode::GAME_INTERACTIVE) {

		const int key{ keyEvent->key() };

		// check for all arrow keys
		switch (key) {
		case Qt::Key_Up:
			mainWindow->guiInteractiveController.movePiece(tobor::v1_0::direction::NORTH());
			return true; //absorbing eventcase
		case Qt::Key_Down:
			mainWindow->guiInteractiveController.movePiece(tobor::v1_0::direction::SOUTH());
			return true; //absorbing eventcase
		case Qt::Key_Right:
			mainWindow->guiInteractiveController.movePiece(tobor::v1_0::direction::EAST());
			return true; //absorbing eventcase
		case Qt::Key_Left:
			mainWindow->guiInteractiveController.movePiece(tobor::v1_0::direction::WEST());
			return true; //absorbing event
		default:
			break;
		}

		static_assert(std::is_same<decltype(keyEvent->key()), int>::value, "implementation probably ill");

		const auto& raw_color_vector{ mainWindow->guiInteractiveController.current_color_vector.colors };

		if (Qt::Key_A <= key && key <= Qt::Key_Z) {
			for (std::size_t i = 0; i < raw_color_vector.size(); ++i) {

				const int color_char_distance{ raw_color_vector[i].UPPERCASE_shortcut_letter() - 'A' };
				const int input_char_distance{ key - Qt::Key_A };

				if (color_char_distance == input_char_distance) {
					mainWindow->guiInteractiveController.selectPieceByColorId(i);
					return true; //absorbing event
				}
			}
		}

		if (Qt::Key_1 <= key && key <= Qt::Key_9) {
			const int input_char_distance{ key - Qt::Key_1 };

			if (static_cast<std::size_t>(input_char_distance) < raw_color_vector.size()) {
				mainWindow->guiInteractiveController.selectPieceByColorId(static_cast<std::size_t>(input_char_distance));
				return true; //absorbing event
			}
		}

		// backspace still caught in MenuBar?

		return false; //pass-through
	}

	if (mainWindow->guiInteractiveController.interactiveMode() == GuiInteractiveController::InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		return true; //absorbing event
	}

	//switch (keyEvent->key()) {
	//	
	//}

	return false; //pass-through
}
