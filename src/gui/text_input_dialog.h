#pragma once

#include <QDialog>
#include <QString>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class TextInputDialog : public QDialog {

	QVBoxLayout* layout;
	QTextEdit* text;
	QPushButton* ok_button;

public:
	TextInputDialog(QWidget* parent = nullptr, const QString& initialText = QString());

	QString getText() const;
};
