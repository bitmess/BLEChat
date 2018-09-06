#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore/qtimer.h>
#include <QDebug>
#include <QtBluetooth/qlowenergyadvertisingdata.h>
#include <QtBluetooth/qlowenergyadvertisingparameters.h>
#include <QtBluetooth/qlowenergycharacteristic.h>
#include <QtBluetooth/qlowenergycharacteristicdata.h>
#include <QtBluetooth/qlowenergydescriptordata.h>
#include <QtBluetooth/qlowenergycontroller.h>
#include <QtBluetooth/qlowenergyservice.h>
#include <QtBluetooth/qlowenergyservicedata.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qlist.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qscopedpointer.h>
#include <QBluetoothLocalDevice>
#include <QStandardItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



private slots:
    void onDeviceConnected();
    void onDeviceDisconnected();
    void onError(QLowEnergyController::Error error);
    void characteristicRead(const QLowEnergyCharacteristic &info,
                            const QByteArray &value);
    void characteristicWritten(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);
    void characteristicChanged(const QLowEnergyCharacteristic &info,
                               const QByteArray &value);

    void on_sendButton_clicked();

private:
    void setup();
    void sendText(QString text);
    void refreshDevices();
    void acceptMineText(QString text);
    void acceptOtherText(QString text);

private:
    Ui::MainWindow *ui;
    QLowEnergyService *service;
    QLowEnergyController *mController;
    QLowEnergyAdvertisingData mAdvertisingData;
    QStandardItemModel deviceModel;
    QStandardItemModel messageModel;
};

#endif // MAINWINDOW_H
