#include "debug_utils.h"

#ifdef DEBUG_EVENT_LOGGER_ENABLE

#include "mainwindow.h"


#include <QGraphicsView>
#include <QListView>
#include <QTreeView>

bool DebugEventLogger::eventFilter(QObject* object, QEvent* e)
{
	QString event_name = typeid(*e).name();

	if (
		dynamic_cast<QGraphicsView*>(object) != nullptr ||
		dynamic_cast<QGraphicsScene*>(object) != nullptr ||
		dynamic_cast<QListView*>(object) != nullptr ||
		dynamic_cast<QTreeView*>(object) != nullptr ||
		false
		) {
		if (e->type() == QEvent::FocusIn) {
			// exited MenuBar
			mainWindow->setWindowTitle("----------------");
		}
	}

	if (
		dynamic_cast<QMenuBar*>(object) != nullptr ||
		dynamic_cast<QMenu*>(object) != nullptr ||
		//dynamic_cast<QMenuBar*>(object) != nullptr ||
		false
		) {
		if (e->type() == QEvent::FocusIn || e->type() == QEvent::MouseButtonPress) {
			// entered MenuBar
			mainWindow->setWindowTitle("++++++++++++++++");

		}
	}

	if (dynamic_cast<QKeyEvent*>(e) != nullptr) { // replace to print all events

		try {
			QString x = object->metaObject()->className();
			qDebug() << event_name << " " << x;
		}
		catch (...) {
			QString x = QString::number(typeid(*object).hash_code());
			qDebug() << event_name << " " << x;
		}
	}

	return false;
}

#endif // DEBUG_EVENT_LOGGER_ENABLE


