
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QXmlStreamReader>
#include <QSvgRenderer>
 #include <QGraphicsSvgItem>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    struct SvgViewToolchain {
        std::unique_ptr<QSvgRenderer> q_svg_renderer;
        std::unique_ptr<QGraphicsScene> q_graphics_scene;
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionshowSVG_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    SvgViewToolchain svgViewToolchain;

    void viewSvgInMainView(const QString& svg_string);
};

#endif // MAINWINDOW_H
