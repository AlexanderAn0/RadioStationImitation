/*
 *Радиостанция - хранит в себе ссылкии на станции для которых  рисует связь, радиовидимость и функцианал взаимодействия мышью
*/
#ifndef RADIOSTATION_H
#define RADIOSTATION_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>
#include <QMenu>
#include <QAction>

#include "radiovisibility.h"
#define SHIFT_FOR_CENTERING 4
#define STATION_CIRCLE_RADIUS 20
class RadioStation : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    RadioStation (int Radius, int num,  QObject *parent = nullptr);
    ~RadioStation();

    void updateLinks();
    bool linked(RadioStation *RS);
    void removeLink(RadioStation *RS);
    int getCoverageRadius();
    void drawLink(RadioStation *RS);
    RadioVisibility * getRadioVisibility();
    int type() const;
    enum {
        RS = 65537,
        RV = 65538
     };

protected:
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
private:

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    bool stationsAtBoundingRectAngles(RadioStation *RS);
    void addLink(RadioStation *RS);
    /// номер станции и радиус видимости
    int numStation;
    int radiusStationVis;

    /// вектор связей для отрисовки
    QVector <RadioStation * > links;

    /// контекстное меню и действие для удаления
    QAction *actDel;
    QMenu *menuRadioStation;

    /// радиовидимость этой станции
    RadioVisibility * myRadV;


public slots:
    void deleteRS();


};

#endif // RADIOSTATION_H
