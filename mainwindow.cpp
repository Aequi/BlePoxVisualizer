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

    ui->qPltTemp->addGraph();
    ui->qPltTemp->graph(0)->setName("T");
    ui->qPltTemp->graph(0)->setPen(QPen(Qt::red));
    ui->qPltTemp->yAxis->setRange(-1.0, 1.0);

    setPlotStyle(ui->qPltHr);
    setPlotStyle(ui->qPltOx);
    setPlotStyle(ui->qPltTemp);

    QThread *poxThread = new QThread;
    hidDevice->moveToThread(poxThread);
    QObject::connect(hidDevice, &HidDevice::hidDataReady, this, &MainWindow::on_hidDataReady);
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

void MainWindow::on_hidDataReady(uint8_t data[], uint8_t length)
{
    if (length != 20) {
        return;
    }

    uint16_t reData[4] = {data[0] | data[1] << 8, data[4] | data[5] << 8, data[8] | data[9] << 8, data[12] | data[13] << 8};
    uint16_t irData[4] = {data[2] | data[3] << 8, data[6] | data[7] << 8, data[10] | data[11] << 8, data[14] | data[15] << 8};
    float temperature = ((float) (int8_t) data[16]) - ((data[17] & 0x0F) * 0.0625);
    static QList<double> hr;
    static QList<double> ox;
    static QList<double> temp;
    static QList<double> time;

    QVector<double> timeVector = QVector<double>::fromList(time);

    ui->qPltHr->graph(0)->setData(timeVector, QVector<double>::fromList(hr));
    ui->qPltOx->graph(0)->setData(timeVector, QVector<double>::fromList(ox));
    ui->qPltTemp->graph(0)->setData(timeVector, QVector<double>::fromList(temp));

    ui->qPltHr->replot();
    ui->qPltOx->replot();
    ui->qPltTemp->replot();
}
