/*
 * программа сетевого взаимодействия между радиостанциями, станции перемещаются,создаются и удаляются мышью
 * при передвежении и совпадении радиовидимости станции с центром другой станции рисуется между ними линия связи
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsScene>
#include <QMenu>
#include <QAction>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QContextMenuEvent>

#include <radiostation.h>
#include <radiovisibility.h>

#define MIN_RAD_ROW 3
#define MAX_RAD_ROW MIN_RAD_ROW
#define MIN_RAD_COLUMN 1
#define MAX_RAD_COLUMN 2
#define SCENE_RECT_WIDTH 500
#define SCENE_RECT_HEIGHT 500

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
private slots:

    void updateGraphicsItemLinks();
    void on_btnAddStation_clicked();
    void on_cbxRandomRadius_stateChanged(int arg1);
    void on_btnDeleteAllStations_clicked();

private:
    Ui::MainWindow *ui;
    /// Сцена на которой размещаюстся все объекты
    QGraphicsScene *mainScene;
    /// Действие для создания объекта и контекстное меню для этого действия
    QAction * actNew;

    QMenu *menuMW;
    /// точка, куда в последний раз нажималась ПКМ
    QPoint lastRightclickPos;
    /// виджет для зоны дополнительной информации
    QLabel *lblInfo;

    void tuneUI();
    void showAdditionalInfo(QString info, bool rewrite = false );
    bool ledDataCheck(QLineEdit *Min, QLineEdit *Max);
    int randomBetween(int low, int high);

    /// последние верно введенные минимальный и максимальный радиусы радиовидимости
    int minRad, maxRad;

};
#endif // MAINWINDOW_H
