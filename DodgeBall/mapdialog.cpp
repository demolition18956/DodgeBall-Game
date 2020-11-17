#include "mapdialog.h"
#include "ui_mapdialog.h"

mapDialog::mapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mapDialog)
{

    ui->setupUi(this);
    scene = new QGraphicsScene(-1.25, -1.25, 2.5, 2.5, this);
    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();
    QRectF bounds = scene->itemsBoundingRect();
    bounds.setWidth(bounds.width()*0.9);
    bounds.setHeight(bounds.height()*0.9);
    ui->graphicsView->fitInView(bounds, Qt::KeepAspectRatio);
    ui->graphicsView->centerOn(0, 0);
}

mapDialog::~mapDialog()
{
    delete ui;
}
