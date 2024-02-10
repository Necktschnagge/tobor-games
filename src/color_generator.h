#pragma once

#include <vector>
#include <QColor>
#include <QMenuBar>
#include <QAction>
#include <QEvent>



class QActionExtend : public QAction {

public:
	bool eventFilter(QObject* object, QEvent* event) override;

protected:
	bool event(QEvent* e) override;
};


class Counter : public QObject
{
	Q_OBJECT

public:
	Counter() { m_value = 0; }

	int value() const { return m_value; }

public slots:
	void gotClicked(bool a) {
		(void)a;
		qDebug() << "clicked";
	}

public:
signals:
	void valueChanged(int newValue);

private:
	int m_value;
};


namespace tobor {

	namespace v1_0 {

		class color_vector {

		public:

			struct color_item {

				int r{ 0 };
				int g{ 0 };
				int b{ 0 };

				std::string display_string;
				std::string display_string_with_underscore;
				std::string shortcut_letter;


				color_item(int r, int g, int b, std::string display_string, std::string display_string_with_underscore, std::string shortcut_letter) :
					r(r),
					g(g),
					b(b),
					display_string(display_string),
					display_string_with_underscore(display_string_with_underscore),
					shortcut_letter(shortcut_letter)
				{}
			};

		private:

			std::vector<color_item> colors;

			color_vector() {}

		public:

			static color_vector get_standard_coloring() {
				color_vector v;

				v.colors.emplace_back(0xFF, 0x00, 0x00, "red", "&red", "r");
				v.colors.emplace_back(0x00, 0xFF, 0x00, "green", "&green", "g");
				v.colors.emplace_back(0x00, 0x00, 0xFF, "blue", "&blue", "b");
				v.colors.emplace_back(0x00, 0xFF, 0xFF, "yellow", "&yellow", "y");

				return v;
			}

			static void test(QMenuBar* mm) {

				static QActionExtend qae;

				static Counter ccc;

				for (QAction* item : mm->actions()) {
					qDebug() << item->text();
					if (item->isSeparator()) {

					}
					else if (item->menu()) {
						color_vector x = get_standard_coloring();

						QMenu* sub = item->menu();
						if (sub->title().replace('&', "").toLower() == "developer") {

							//auto a2 = new QActionExtend();


							auto action = sub->addAction(QString::fromStdString(x.colors[0].display_string_with_underscore));

							//action->installEventFilter(&qae);
							//action->activate(QAction::Trigger);

							QObject::connect(action, &QAction::triggered, &ccc, &Counter::gotClicked, Qt::AutoConnection);
							//QObject::connect(action, SIGNAL(triggered()), &ccc, SLOT(gotClicked()));
						}
					}
					else {

					}
				}
			}

		};

	}
}
