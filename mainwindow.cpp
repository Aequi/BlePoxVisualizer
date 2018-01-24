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

    hidDevice = new HidDevice();
    ui->lableConnectionState->setStyleSheet("QLabel {color : red;}");
    ui->lableConnectionState->setText("Disconnected");

    ui->qPltHr->addGraph();
    ui->qPltHr->graph(0)->setName("Hr");
    ui->qPltHr->graph(0)->setPen(QPen(Qt::green));
    ui->qPltHr->yAxis->setRange(-0.1, 1.5);

    ui->qPltOx->addGraph();
    ui->qPltOx->graph(0)->setName("Ox");
    ui->qPltOx->graph(0)->setPen(QPen(Qt::cyan));
    ui->qPltOx->yAxis->setRange(-0.1, 1.5);

    ui->qPltTemp->addGraph();
    ui->qPltTemp->graph(0)->setName("T");
    ui->qPltTemp->graph(0)->setPen(QPen(Qt::red));
    ui->qPltTemp->yAxis->setRange(-10.0, 60.0);

    setPlotStyle(ui->qPltHr);
    setPlotStyle(ui->qPltOx);
    setPlotStyle(ui->qPltTemp);

    QObject::connect(hidDevice, &HidDevice::hidDataReady, this, &MainWindow::on_hidDataReady);
    hidDevice->start();

}

MainWindow::~MainWindow()
{
    hidDevice->exit();
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

void MainWindow::on_hidDataReady(quint8 *data, quint8 length)
{
    if (length != 20) {
        return;
    }
    static uint8_t cntr = 0;
    static uint32_t cntr2 = 0;
    static bool isFirstRun = false;
    if (!isFirstRun) {
        isFirstRun = true;
        cntr = data[19];
    }

    if (cntr != data[19]) {
        qDebug() << "Packet lost!" << cntr2++;
        cntr = data[19];
    }
    cntr++;
    static double timeR = 0.0;
    data++;
    uint32_t reData[4];
    reData[0] = ((uint32_t) data[0] << 8)  | ((uint32_t) (data[1]));
    reData[1] = ((uint32_t) data[4] << 8)  | ((uint32_t) (data[5]));
    reData[2] = ((uint32_t) data[8] << 8)  | ((uint32_t) (data[9]));
    reData[3] = ((uint32_t) data[12] << 8) | ((uint32_t) (data[13]));
    uint32_t irData[4];
    irData[0] = ((uint32_t) data[2] << 8)  | ((uint32_t) (data[3]));
    irData[1] = ((uint32_t) data[6] << 8)  | ((uint32_t) (data[7]));
    irData[2] = ((uint32_t) data[10] << 8) | ((uint32_t) (data[11]));
    irData[3] = ((uint32_t) data[14] << 8) | ((uint32_t) (data[15]));

    float temperature = ((float) (int8_t) data[16]) - ((data[17] & 0x0F) * 0.0625);
    static QList<double> hr;
    static QList<double> ox;
    static QList<double> temp;
    static QList<double> time;

    if (time.size() >= 500) {
        hr.pop_front();
        hr.pop_front();
        hr.pop_front();
        hr.pop_front();
        ox.pop_front();
        ox.pop_front();
        ox.pop_front();
        ox.pop_front();
        temp.pop_front();
        temp.pop_front();
        temp.pop_front();
        temp.pop_front();
        time.pop_front();
        time.pop_front();
        time.pop_front();
        time.pop_front();
    }

    hr.push_back((double) reData[0] / 65535.0);
    hr.push_back((double) reData[1] / 65535.0);
    hr.push_back((double) reData[2] / 65535.0);
    hr.push_back((double) reData[3] / 65535.0);

    ox.push_back((double) irData[0] / 65535.0);
    ox.push_back((double) irData[1] / 65535.0);
    ox.push_back((double) irData[2] / 65535.0);
    ox.push_back((double) irData[3] / 65535.0);

    temp.push_back((double) temperature);
    temp.push_back((double) temperature);
    temp.push_back((double) temperature);
    temp.push_back((double) temperature);

    time.push_back(timeR + 0.00);
    time.push_back(timeR + 0.01);
    time.push_back(timeR + 0.02);
    time.push_back(timeR + 0.03);
    timeR += 0.04;

    QVector<double> timeVector = QVector<double>::fromList(time);

    ui->qPltHr->graph(0)->setData(timeVector, QVector<double>::fromList(hr));
    ui->qPltHr->xAxis->setRange(time.front(), 5.0, Qt::AlignLeft);
    ui->qPltOx->graph(0)->setData(timeVector, QVector<double>::fromList(ox));
    ui->qPltOx->xAxis->setRange(time.front(), 5.0, Qt::AlignLeft);
    ui->qPltTemp->graph(0)->setData(timeVector, QVector<double>::fromList(temp));
    ui->qPltTemp->xAxis->setRange(time.front(), 5.0, Qt::AlignLeft);

    ui->qPltHr->rescaleAxes();
    ui->qPltOx->rescaleAxes();

    ui->qPltHr->replot();
    ui->qPltOx->replot();
    ui->qPltTemp->replot();
}
