/*
 *  Радиовидимость станции, рисует связи между станциями и показывает с какмим объектами столкнулась
*/
#ifndef RADIOVISIBILITY_H
#define RADIOVISIBILITY_H
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QGraphicsScene>


class RadioVisibility : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    RadioVisibility(int radius, QObject *parent = nullptr);


    void drawlines(QVector<QGraphicsItem*> links);
    int type() const;

private:

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    /// Радиовидимость станции
    int radioVisRadius;
    /// объекты к которым необходимо рисовать линии
    QVector <QGraphicsItem *> vecLinks;

    enum {
        RS = 65537,
        RV = 65538
    };
};

#endif // RADIOVISIBILITY_H
