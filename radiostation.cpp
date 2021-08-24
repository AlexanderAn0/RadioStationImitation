#include "radiostation.h"
#include <math.h>

/**
 * @brief RadioStation::RadioStation Создает объект радиостанц с радиовидимостью
 * @param radius Радиус радиовидимости станции
 * @param num Номер станции
 * @param parent --
 */
RadioStation::RadioStation(int radius, int num, QObject *parent) :
 QObject(parent),QGraphicsItem(), numStation(num),radiusStationVis(radius)
{
    myRadV = new RadioVisibility(radiusStationVis);
    actDel = new QAction("Удалить", this);
    connect(actDel, &QAction::triggered, this, &RadioStation::deleteRS);
    menuRadioStation  = new QMenu;
    menuRadioStation->addAction(actDel);

    this->setZValue(1);
}

/**
 * @brief RadioStation::~RadioStation Удаляет созданные объекты
 */
RadioStation::~RadioStation(){
    actDel->deleteLater();
    menuRadioStation->deleteLater();
}

/**
 * @brief RadioStation::updateLinks Проверяет, какие объекты пересекаются с контуром радиовидимости
 *                                  и добавляет/убирает их из вектора отрисовки линий связи
 */
void RadioStation::updateLinks(){
    /// Объекты с которые пересекаются с радиовидимостью
    QList <QGraphicsItem*> collItems;
    collItems = myRadV->collidingItems(Qt::IntersectsItemShape);

    /// Если пересекается не только с собственной станцией, проверяет станция ли это и добавляет его в
    /// вектор линий связи
    if(collItems.count() > 2)
    {
        for(int i =0; i<collItems.count(); ++i){
            if(collItems.at(i)->type() == RS)
            {
                addLink(static_cast<RadioStation*>(collItems.at(i)));

            }
        }
    }

    /// Проверяет, есть ли в векторе связей лишние станции, с которыми уже нет контакта и удаляет их
    for(int i=0; i<links.count(); ++i)
    {
        if(!collItems.contains(static_cast<QGraphicsItem*>(links.at(i))))
        {
            links.removeAll(links.at(i));
        }
    }

    /// Перевод вектора радиостанций в вектор графических объектов и отправка его радиовидимости для отрисовки линии связи
    QVector <QGraphicsItem*> vecForRadioVis;

    for(int i=0; i<links.count(); ++i){
        vecForRadioVis.append((QGraphicsItem*)links.at(i));
    }

    myRadV->drawlines(vecForRadioVis);
}
/**
 * @brief RadioStation::linked Показывает, есть ли станция в векторе отрисовки связи
 * @param RS Станция, которую проверяю на наличие связи
 * @return
 */
bool RadioStation::linked(RadioStation *RS){
    return links.contains(RS);
}

/**
 * @brief RadioStation::removeLink Убирает станцию из вектора отрисовки связи
 * @param RS убираемая станция
 */
void RadioStation::removeLink(RadioStation *RS){
    if(links.contains(RS))
    links.removeAll(RS);
}

/**
 * @brief RadioStation::getCoverageRadius показывает радиус радиопокрытия
 * @return радиус радиопокрытия/радиовидимости
 */
int RadioStation::getCoverageRadius(){
   return radiusStationVis;
}

/**
 * @brief RadioStation::drawLink Передает станции иформацию, что её радиовидимость рисует связь к объекту
 * @param RS объект к которому необходимо нарисовать связь
 */
void RadioStation::drawLink(RadioStation *RS){
    links.append(RS);
}

/**
 * @brief RadioStation::getRadioVisibility возвращает радиовидимость этой станции
 * @return указатель на радиовидимость
 */
RadioVisibility * RadioStation::getRadioVisibility(){
     return myRadV;
}

/**
 * @brief RadioStation::type виртуальная функция QGraphicsItem, показывает тип элемента
 * @return тип элемента в виде числа
 */
int RadioStation::type() const {
    return RS;
}

/**
 * @brief RadioStation::boundingRect виртуальная функция QGraphicsItem, область, в которой объект отрисовывается на сцене
 * @return прямоугольник в котором на сцене будет отрисовываться объект
 */
QRectF RadioStation::boundingRect() const
{
    return QRectF (-STATION_CIRCLE_RADIUS,-STATION_CIRCLE_RADIUS,
                   STATION_CIRCLE_RADIUS*2,STATION_CIRCLE_RADIUS*2);
}

/**
 * @brief RadioStation::paint Рисует круг станции и ее номер с заданными параметрами
 * @param painter --
 * @param option --
 * @param widget --
 */
void RadioStation::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);

    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawEllipse(QPointF(0,0), STATION_CIRCLE_RADIUS,STATION_CIRCLE_RADIUS);
    painter->drawText(QPointF(-SHIFT_FOR_CENTERING,SHIFT_FOR_CENTERING), QString::number(numStation));

}

/**
 * @brief RadioStation::mouseMoveEvent Действие при движении мышью с выбранным объектом,
 *                                     если зажата ЛКМ перемещает станцию и  радиовидимость
 * @param event Что и где на сцене делают мышью
 */
void RadioStation::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton )
    {
        this->setPos(mapToScene(event->pos()));
        myRadV->setPos(mapToScene(event->pos()));

    }

}

/**
 * @brief RadioStation::mousePressEvent Действия, при нажатии мышью на объект ЛКМ смена вида курсора
 *                                      ПКМ - открытие контекстного меню
 * @param event Что и где на сцене делают мышью
 */
void RadioStation::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(event->button()== Qt::LeftButton )
    {
        ///!!! при выборе контекстного меню, а после, если необходимо меню закрыть, при нажатии ЛКМ на сцене
        /// вызывается mouseMoveEvent и перемещает станцию и видимость несинхронно в место нажатия
        /// добавил собственный вызов mouseMoveEvent, чтобы перемещалось хотябы синхронно
        mouseMoveEvent(event);
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    else
    {
        ///Если вместо этого условия поставить event->ignore(), то бага как коменте сверху не возникает,
        /// но тогда открывается много контекстных меню и каждое нужно закрывать отдельным щелчком мимо
        /// не знаю как решить
      if(event->isAccepted())
       {
         menuRadioStation->popup(QCursor::pos());
       }
    }

}
/**
 * @brief RadioStation::mouseReleaseEvent Действие при отпускании мыши, смена курсора на стандартный
 * @param event Что и где на сцене делают мышью
 */
void RadioStation::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->setCursor(QCursor(Qt::ArrowCursor));
    Q_UNUSED(event);
}
/**
 * @brief RadioStation::stationsAtBoundingRectAngles Проверяет, вошла станция в радиус, или только в
 *                                                   углу BoundingRect, не находясь в радиусе
 * @param RS вошедший объект
 * @return true, если объект не в радиусе покрытия, а по краям BoundingRect
 */
bool RadioStation::stationsAtBoundingRectAngles(RadioStation *RS){
    /// расчет гипотенузы
    static double deltaX, deltaY;
    deltaX = this->pos().x() - RS->pos().x();
    deltaY = this->pos().y() - RS->pos().y();
    static double dist;
    dist = sqrt(deltaX*deltaX+deltaY*deltaY);
    /// если ни один радиус не больше расстояния между объектами, убирает их из вектора отрисовки связей
    if(!((dist<= radiusStationVis)||(dist<=RS->getCoverageRadius())))
    {
        this->removeLink(RS);
        RS->removeLink(this);
        return true;
    }
    return false;
}
/**
 * @brief RadioStation::addLink Добавляет объект к вектору отрисовки связей
 * @param RS добавляемый объект
 */
void RadioStation::addLink(RadioStation *RS){

    if(RS == this) return;
    if(stationsAtBoundingRectAngles(RS)) return;

    /// Если станции не связаны, станция с большей зоной радиовидимости будет рисовать связь
    if((!RS->linked(this))&&(!linked(RS)))
    {
        if(radiusStationVis > RS->getCoverageRadius())
        {
            links.append(RS);
        }
        else
        {
            RS->drawLink(this);
        }
    }
}

/**
 * @brief RadioStation::deleteRS Удаляет станцию и радиовидимость
 */
void RadioStation::deleteRS(){
    delete  myRadV;
    delete this;
}


