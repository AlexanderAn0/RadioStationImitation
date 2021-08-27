#include "radiovisibility.h"
/**
 * @brief RadioVisibility::RadioVisibility Конструктор
 * @param radius Радиус радиовидимости
 * @param parent
 */
RadioVisibility::RadioVisibility(int radius, QObject *parent) :
    QObject(parent),QGraphicsItem(), radioVisRadius(radius)
{

}


/**
 * @brief RadioVisibility::drawlines Обновляет вектор объектов, к которым необходимо нарисовать линии связи
 * @param links вектор объектов, к которым рисуются связи
 */
void RadioVisibility::drawlines(QVector<QGraphicsItem*> links){

    if(links.empty())
        vecLinks.clear();

    vecLinks = links;

}
/**
 * @brief RadioVisibility::type виртуальная функция QGraphicsItem, показывает тип элемента
 * @return тип элемента в виде числа
 */
int RadioVisibility::type() const {
    return RV;
}

/**
 * @brief RadioVisibility::boundingRect виртуальная функция QGraphicsItem, область, в которой объект отрисовывается на сцене
 * @return прямоугольник в котором на сцене будет отрисовываться объект
 */
QRectF RadioVisibility::boundingRect() const
{
    return QRectF (-radioVisRadius,-radioVisRadius,
                   radioVisRadius*2, radioVisRadius*2);
}

/**
 * @brief RadioVisibility::paint Рисует круг радиовидимости и линии связи, которые получил от станции
 * @param painter --
 * @param option --
 * @param widget --
 */
void RadioVisibility::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    /// настройка пера и отрисовка зоны покрытия
    QPen dashLinePen;
    dashLinePen.setStyle(Qt::DashLine);
    dashLinePen.setColor(Qt::gray);
    painter->setPen(dashLinePen);
    painter->drawEllipse(QPointF(0,0), radioVisRadius, radioVisRadius);

    /// перенастройка пера и отрисовка линий связи к объектам, которые передала станция
    dashLinePen.setColor(Qt::red);
    painter->setPen(dashLinePen);
    for(int i=0; i<vecLinks.count();++i){
        painter->drawLine(mapFromScene(this->pos()),
                          mapFromScene(vecLinks.at(i)->pos()));


    }

    Q_UNUSED(option);
    Q_UNUSED(widget);

}



