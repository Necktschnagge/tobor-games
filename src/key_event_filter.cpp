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


		// check for all arrow keys
		switch (keyEvent->key()) {
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

		// obtain a coloring vector from the current game

		// check if key is a character and check if it contained in color vector.

		// also select color by 1 2 3 4 5 ...

		// backspace still caught in MenuBar?

		return true; //absorbing event
	}

	if (mainWindow->guiInteractiveController.interactiveMode() == GuiInteractiveController::InteractiveMode::SOLVER_INTERACTIVE_STEPS) {

		return true; //absorbing event
	}

	//switch (keyEvent->key()) {
	//	
	//}

	return false; //pass-through
}
