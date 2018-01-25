#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include "HidDevice.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    HidDevice *hidDevice;
    void setPlotStyle(QCustomPlot *plot);

private slots:

    void on_hidDataReady(quint8 *data, quint8 length);

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
