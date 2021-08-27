#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QRandomGenerator>

/**
 * @brief MainWindow::MainWindow Настраивается сцена и сигналы, слоты
 * @param parent --
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tuneUI();

    connect(actNew, &QAction::triggered, this, &MainWindow::on_btnAddStation_clicked);
    /// при изменении на сцене, обновлять связи между станциями
    connect(mainScene, &QGraphicsScene::changed, this, &MainWindow::updateGraphicsItemLinks );


}

/**
 * @brief MainWindow::~MainWindow освобождение памяти
 */
MainWindow::~MainWindow()
{
    delete lblInfo;
    delete actNew;
    delete menuMW;
    delete mainScene;
    delete ui;

}

/**
 * @brief MainWindow::updateGraphicsItemLinks Обновляет вектор связей элементов при каких-либо изменениях на сцене
 */
void MainWindow::updateGraphicsItemLinks()
{
    QList<QGraphicsItem*> sceneItems = mainScene->items();

    for (int i=0; i < sceneItems.count(); ++i ) {
        if(sceneItems.at(i)->type()== RadioStation::RS){
            static_cast<RadioStation*>(sceneItems.at(i))->updateLinks();
        }
    }

}

/**
 * @brief MainWindow::contextMenuEvent Вызывается при необходимости контекстного меню
 * @param event событие вызова контекстного меню
 */
void MainWindow::contextMenuEvent(QContextMenuEvent *event){
    /// Запоминается точка последнего нажатия ПКМ
    lastRightclickPos = event->pos();
    /// Проверяется, если под мышью нет RadioStation, то вызывает собственное меню
    /// иначе, ивент не используется и передается объекту под мышью и вызывается контекстное меню объекта
    QGraphicsItem* itemUnderMouse = ui->interactiveGraphicsWindow->itemAt(event->pos());
    if(itemUnderMouse == nullptr || itemUnderMouse->type() != RadioStation::RS)
    {
        menuMW->popup(ui->interactiveGraphicsWindow->mapToGlobal(event->pos()));
    }

}

/**
 * @brief MainWindow::on_btnAddStation_clicked добавляет новую станцию с её видимостью на сцену, увеличивает счетчик станций
 */
void MainWindow::on_btnAddStation_clicked()
{
    showAdditionalInfo("", true);
    /// Если данные радиуса корректны, создается новый объект
    if(ledDataCheck(ui->ledMinRadius, ui->ledMaxRadius))
    {
        RadioStation *newStation = new RadioStation(randomBetween(minRad,maxRad),
                                             ui->sbxNumStation->value());

        ui->sbxNumStation->setValue(ui->sbxNumStation->value()+1);

        /// устанавливается в позицию последнеко правого щелка мышью и добавляется на сцену
        newStation->setPos(ui->interactiveGraphicsWindow->mapToScene(lastRightclickPos));
        newStation->getRadioVisibility()->setPos(ui->interactiveGraphicsWindow->mapToScene(lastRightclickPos));

        mainScene->addItem(newStation);
        mainScene->addItem(newStation->getRadioVisibility());
    }

}

/**
 * @brief MainWindow::on_cbxRandomRadius_stateChanged при изменении чекбокса на сцене скрывает или передвигает виджеты
 * @param presset нажата или нет
 */
void MainWindow::on_cbxRandomRadius_stateChanged(int presset)
{
    ui->ledMaxRadius->setVisible(presset);
    ui->lblMinRadius->setVisible(presset);
    ui->lblMaxRadius->setVisible(presset);
    /// если радиус случайный, добавляется оба окна ввода рядом
    if(presset){
        ui->glautSettings->addWidget(ui->ledMinRadius,MIN_RAD_ROW,MIN_RAD_COLUMN);
        ui->glautSettings->addWidget(ui->ledMaxRadius,MAX_RAD_ROW,MAX_RAD_COLUMN);

    }
    else{
        /// иначе - одно большое окно ввода на месте двух
        ui->glautSettings->addWidget(ui->ledMinRadius, MIN_RAD_ROW,
                                     MIN_RAD_COLUMN,MIN_RAD_COLUMN,
                                     MAX_RAD_COLUMN);
    }

}

/**
 * @brief MainWindow::on_btnDeleteAllStations_clicked Удаляет все радиостанции и радиовидимости
 */
void MainWindow::on_btnDeleteAllStations_clicked()
{
    ///!!! при удалении разом 65+ объектов ошибка сегментации, т.к. у еще не удаленных идет обращение к
    /// удаленным объектам в RadioVisibility::vecLinks.at(i).pos() или в RadioStation::Links.at(i)

    QList<QGraphicsItem*> sceneItems = mainScene->items();

    /// находит среди объектов радиостанции и удаляет их, отдельно убирая их радиовидимость из себя
    for (int i=0;i<sceneItems.count() ;++i ) {
        {
            mainScene->removeItem(sceneItems.at(i));
            if(sceneItems.at(i)->type()== RadioStation::RS){
                sceneItems.removeAll(static_cast<RadioStation*>(sceneItems.at(i))->getRadioVisibility());
                static_cast<RadioStation*>(sceneItems.at(i))->deleteRS();
            }
        }
    }

}


/**
 * @brief MainWindow::tuneUI Настраивает окно
 */
void MainWindow::tuneUI()
{

   this->resize(1000,800);
   /// настраивается основной слой, строка вывода
   ui->centralwidget->setLayout(ui->glautMainLayout);
   lblInfo = new QLabel();
   lblInfo->setFont(ui->btnAddStation->font());
   ui->interactiveGraphicsWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding );
   ui->glautMainLayout->addWidget(lblInfo,1,0,1,2);

   mainScene = new QGraphicsScene(this);
   mainScene->setItemIndexMethod(QGraphicsScene::NoIndex);

   /// настраивается graphicsView
   ui->interactiveGraphicsWindow->setScene(mainScene);
   ui->interactiveGraphicsWindow->setRenderHint(QPainter::Antialiasing);
   ui->interactiveGraphicsWindow->setCacheMode(QGraphicsView::CacheBackground);
   ui->interactiveGraphicsWindow->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
   mainScene->setSceneRect(0,0,SCENE_RECT_WIDTH,SCENE_RECT_HEIGHT);

   /// создается контекстное меню
   menuMW = new QMenu(ui->interactiveGraphicsWindow);
   actNew = new QAction("Добавить станцию", ui->interactiveGraphicsWindow);

   menuMW->addAction(actNew);

}
/**
 * @brief MainWindow::showAdditionalInfo добавляет в область доп. данных текст
 * @param info добавляемый текст
 * @param rewrite true, для удаления старого текста
 */
void MainWindow::showAdditionalInfo(QString info, bool rewrite){
    if(rewrite){
        lblInfo->setText(info);
    }
    else
    {
        lblInfo->setText(lblInfo->text() + " " + info);
    }
}

/**
 * @brief MainWindow::ledDataCheck Проверяет вводимые данные в окна мин/макс радиуса
 * @param Min окно мин. радиуса
 * @param Max окно макс радиуса
 * @return  true если данные корректны
 */
bool MainWindow::ledDataCheck(QLineEdit *Min, QLineEdit * Max){
    bool minValueCorrect = true;
    bool maxValueCorrect = true;
    int minRadiusTemp, maxRadiusTemp;
    minRadiusTemp = Min->text().toInt(&minValueCorrect);
    /// если мин. граница корректна, то обе границы становятся равны минимальной
    if(minValueCorrect){
        minRad = minRadiusTemp;
        maxRad = minRadiusTemp;
    }
    else
    {
        showAdditionalInfo("Мин. Радиус введен неверно");
    }
    /// если окно макс границы не скрыто, то проверяем на корректность и его, иначе - игнорируем
    if(Max->isVisible())
    {
        maxRadiusTemp = Max->text().toInt(&maxValueCorrect);

        if(maxValueCorrect){
            maxRad = maxRadiusTemp;
        }
        else
        {
            showAdditionalInfo("Макс. Радиус введен неверно");
        }
    }

    return (minValueCorrect && maxValueCorrect);
}
/**
 * @brief MainWindow::randomBetween возвращает случайное число в введенных границах
 * @param low нижняя граница
 * @param high верхняя граница
 * @return случайное ограниченное число, если границы равны, саму границу
 */
int MainWindow::randomBetween(int low, int high)
{
   if(low==high) return low;
   if(low>high){
       int temp;
       temp = low;
       low = high;
       high = temp;
       showAdditionalInfo("Нижняя и верхняя граница поменялись местами", true);
   }
   QRandomGenerator *rg =QRandomGenerator::global();
  return rg->bounded(low,high);
}
