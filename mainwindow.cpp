#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "windows.h"
#include <Winuser.h>
#include <QDebug>
#include <QThread>
#include <QVector>
#include "stdint.h"
#include "HidDevice.h"


void MainWindow::setPlotStyle(QCustomPlot *plot)
{
    QMargins chartsMargin(40,0,0,0);

    plot->axisRect()->setAutoMargins(QCP::msRight|QCP::msTop|QCP::msBottom);
    plot->axisRect()->setMargins(chartsMargin);

    plot->legend->setVisible(true);
    plot->legend->setBrush(Qt::NoBrush);
    plot->legend->setBorderPen(Qt::NoPen);
    plot->legend->setTextColor(Qt::white);

    plot->xAxis->setAutoTickStep(false);
    plot->xAxis->setTickStep(1.0);
    plot->xAxis->setTicks(false);

    plot->xAxis->setTickLabelColor(Qt::white);
    plot->xAxis->setTickPen(QPen(Qt::white));
    plot->xAxis->setSubTickPen(QPen(Qt::gray));
    plot->xAxis->setLabelColor(Qt::white);
    plot->xAxis->setLabelFont(QFont( "lucida", 10));

    plot->yAxis->setTickLabelColor(Qt::white);
    plot->yAxis->setTickPen(QPen(Qt::white));
    plot->yAxis->setSubTickPen(QPen(Qt::gray));
    plot->yAxis->setLabelColor(Qt::white);
    plot->yAxis->setLabelFont(QFont( "lucida", 10));

    plot->xAxis2->setVisible(true);
    plot->xAxis2->setLabelColor(Qt::white);
    plot->xAxis2->setLabelFont(QFont( "lucida", 10, QFont::Bold));

    plot->xAxis2->setTickLabelColor(Qt::white);
    plot->xAxis2->setTickPen(QPen(Qt::white));
    plot->xAxis2->setSubTickPen(QPen(Qt::gray));
    plot->xAxis2->setLabelColor(Qt::white);
    plot->xAxis2->setLabelFont(QFont( "lucida", 10));

    plot->xAxis2->setAutoTickStep(false);
    plot->xAxis2->setTickStep(1.0);
    plot->xAxis2->setTicks(true);
    plot->xAxis2->setRange(0, 5, Qt::AlignRight);

    plot->setBackground(QBrush(Qt::black));
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("BlePoxVisualizer");
    QPalette pal = palette();

    pal.setColor(QPalette::Background, Qt::lightGray);
    ui->centralWidget->setAutoFillBackground(true);
    ui->centralWidget->setPalette(pal);

    ui->lblDevice->setStyleSheet("QLabel {color : green;}");
    ui->lblVid->setStyleSheet("QLabel {color : green;}");
    ui->lblPid->setStyleSheet("QLabel {color : green;}");

    hidDevice = new HidDevice;
    ui->lableConnectionState->setStyleSheet("QLabel {color : red;}");
    ui->lableConnectionState->setText("Disconnected");

    ui->qPltHr->addGraph();
    ui->qPltHr->graph(0)->setName("Hr");
    ui->qPltHr->graph(0)->setPen(QPen(Qt::green));
    ui->qPltHr->yAxis->setRange(-1.0, 1.0);

    ui->qPltOx->addGraph();
    ui->qPltOx->graph(0)->setName("Ox");
    ui->qPltOx->graph(0)->setPen(QPen(Qt::cyan));
    ui->qPltOx->yAxis->setRange(-1.0, 1.0);


    setPlotStyle(ui->qPltHr);
    setPlotStyle(ui->qPltOx);
}

MainWindow::~MainWindow()
{
    hidDevice->close();
    delete ui;
}

void MainWindow::on_pbConnect_clicked()
{
    bool isOk;

    if (hidDevice->open(ui->leVid->text().toUInt(&isOk, 16), ui->lePid->text().toUInt(&isOk, 16)) != true) {
        QMessageBox msgBox;
        msgBox.setText("Failed to open device!");
        msgBox.exec();
        hidDevice->close();
        ui->lableConnectionState->setText("Disconnected");
        ui->lableConnectionState->setStyleSheet("QLabel {color : red;}");
        return;
    }
    ui->lableConnectionState->setText("Connected");
    ui->lableConnectionState->setStyleSheet("QLabel {color : green;}");
}

void MainWindow::on_pbDisconnect_clicked()
{
    hidDevice->close();
    ui->lableConnectionState->setStyleSheet("QLabel {color : red;}");
    ui->lableConnectionState->setText("Disconnected");
}

void MainWindow::on_pbStartStop_clicked()
{

}
