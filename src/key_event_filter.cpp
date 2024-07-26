#include "key_event_filter.h"

#include "mainwindow.h"

#include "models_1_1.h"


#include<QKeyEvent>


bool ControlKeyEventAgent::eventFilter(QObject* object, QEvent* e)
{
	(void)object;

	QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);

	if (keyEvent == nullptr) {
		return false; //pass-through
	}

	// see https://doc.qt.io/qt-6/qevent.html for event types

	// keyEvent->type()
	// is one of
	// QEvent::KeyRelease, QEvent::ShortcutOverride, QEvent::KeyPress

	if (keyEvent->type() != QEvent::KeyPress) {
		return false; //pass-through
	}

	// found a keyEvent
	const auto key{ keyEvent->key() };

	auto game = mainWindow->current_game;

	if (!game) {
		return false; //pass-through
	}


	if (!mainWindow->current_game->solver()) {

		// check for all arrow keys
		switch (key) {
		case Qt::Key_Up:
			mainWindow->movePiece(tobor::v1_0::direction::NORTH());
			return true; //absorbing eventcase
		case Qt::Key_Down:
			mainWindow->movePiece(tobor::v1_0::direction::SOUTH());
			return true; //absorbing eventcase
		case Qt::Key_Right:
			mainWindow->movePiece(tobor::v1_0::direction::EAST());
			return true; //absorbing eventcase
		case Qt::Key_Left:
			mainWindow->movePiece(tobor::v1_0::direction::WEST());
			return true; //absorbing event
		default:
			break;
		}

		const auto& raw_color_vector{ mainWindow->current_color_vector.colors };

		if (Qt::Key_A <= key && key <= Qt::Key_Z) {
			for (std::size_t i = 0; i < raw_color_vector.size(); ++i) {

				const int color_char_distance{ raw_color_vector[i].UPPERCASE_shortcut_letter() - 'A' };
				const int input_char_distance{ key - Qt::Key_A };

				if (color_char_distance == input_char_distance) {
					mainWindow->selectPieceByColorId(i);
					return true; //absorbing event
				}
			}
		}

		if (Qt::Key_1 <= key && key <= Qt::Key_9) {
			const int input_char_distance{ key - Qt::Key_1 };

			if (static_cast<std::size_t>(input_char_distance) < raw_color_vector.size()) {
				mainWindow->selectPieceByColorId(static_cast<std::size_t>(input_char_distance));
				return true; //absorbing event
			}
		}

		// backspace still caught in MenuBar?

		return false; //pass-through
	}


	// else: solver mode

		// check for all arrow keys
	switch (key) {
	case Qt::Key_Right:
		mainWindow->current_game->move_by_solver(true);
		mainWindow->refreshAll();
		return true; //absorbing eventcase
	case Qt::Key_Left:
		mainWindow->current_game->move_by_solver(false);
		mainWindow->refreshAll();
		return true; //absorbing event
	default:
		break;
	}

	return false; //pass-through


}
