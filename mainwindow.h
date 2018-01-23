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


private slots:
    void on_pbConnect_clicked();

    void on_pbDisconnect_clicked();

    void on_pbApply_clicked();

    void on_pbStart_clicked();

    void on_pbStop_clicked();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
