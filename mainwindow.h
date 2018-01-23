#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include "HidDevice.h"
#include "stdint.h"

namespace Ui {
class MainWindow;
}

class QCustomPlot;

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    HidDevice *hidDevice;
    void setPlotStyle(QCustomPlot *plot);

private slots:
    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

    void on_pbStartStop_clicked();

    void on_hidDataReady(uint8_t data[], uint8_t length);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
