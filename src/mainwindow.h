
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
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionshowSVG_triggered();

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;

    std::unique_ptr<QSvgRenderer> q_svg_renderer;
    std::unique_ptr<QGraphicsSvgItem> q_graphics_svg_item;
    std::unique_ptr<QGraphicsScene> q_graphics_scene;

};

#endif // MAINWINDOW_H
