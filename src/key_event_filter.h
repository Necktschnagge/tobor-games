#pragma once

#include <QEvent>

class ControlKeyEventAgent : public QObject
{
	Q_OBJECT
public:
	ControlKeyEventAgent(){}

	bool eventFilter(QObject* object, QEvent* event) override;

	virtual ~ControlKeyEventAgent(){}
};