#pragma once

#include <QMessageBox>
#include <QString>


inline void showErrorDialog(const QString& message) {
	QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), message);
	msgBox.exec();
}