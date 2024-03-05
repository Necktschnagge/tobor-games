#pragma once

//#define DEBUG_EVENT_LOGGER_ENABLE



#ifdef DEBUG_EVENT_LOGGER_ENABLE


#include <QObject>
#include <QEvent>


class MainWindow;

class DebugEventLogger : public QObject
{
	Q_OBJECT

		MainWindow* mainWindow;
public:
	DebugEventLogger(MainWindow* mainWindow) : mainWindow(mainWindow) {}

	bool eventFilter(QObject* object, QEvent* event) override;

	virtual ~DebugEventLogger() {}
};

#endif // DEBUG_EVENT_LOGGER_ENABLE
