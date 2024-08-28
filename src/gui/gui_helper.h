#pragma once

#include <QMessageBox>
#include <QString>


inline void showErrorDialog(const QString& message) {
	QMessageBox msgBox(QMessageBox::Icon::Critical, QString("GUI ERROR"), message, QMessageBox::StandardButton::Ok);
	msgBox.exec();
}

inline void showErrorActionAvailable() {
	showErrorDialog("This action should not be available.");
}

