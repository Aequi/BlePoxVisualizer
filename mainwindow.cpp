#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "windows.h"
#include <Winuser.h>
#include <QDebug>
#include <QThread>
#include <QVector>
#include "stdint.h"
#include "HidDevice.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MysterioTestTool");
    hidDevice = new HidDevice;
    ui->lableConnectionState->setStyleSheet("QLabel {color : red;}");
    ui->lableConnectionState->setText("Disconnected");
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

void MainWindow::on_pbApply_clicked()
{
    const QVector<int8_t> powers = hidDevice->getTxPowers();
    hidDevice->setTxPower(powers.at(ui->cmbPowerLevel->currentIndex()));
}

void MainWindow::on_pbStart_clicked()
{
    hidDevice->setBatteryTestMode(true);
}

void MainWindow::on_pbStop_clicked()
{
    hidDevice->setBatteryTestMode(false);
}
