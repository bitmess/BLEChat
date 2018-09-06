#include "mainwindow.h"
#include "ui_mainwindow.h"

const static QString kServiceId = "77777777-7777-7777-7777-7777777777777";
const static QString kNotifyId = "99999999-9999-9999-9999-999999999999";
const static QString kWriteId = "88888888-8888-8888-8888-888888888888";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setup();
}

void MainWindow::setup(){

    ui->listView->setModel(&this->deviceModel);
    ui->listView_2->setModel(&this->messageModel);

    mController = QLowEnergyController::createPeripheral(this);

    QLowEnergyServiceData serviceData;
    serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);
    serviceData.setUuid(QBluetoothUuid(kServiceId));

    QLowEnergyCharacteristicData notifyChar;//notify
    notifyChar.setUuid(QBluetoothUuid(kNotifyId));
    notifyChar.setProperties(QLowEnergyCharacteristic::Notify);
    serviceData.addCharacteristic(notifyChar);

    QLowEnergyCharacteristicData writeNotify;//write
    writeNotify.setUuid(QBluetoothUuid(kWriteId));
    writeNotify.setProperties(QLowEnergyCharacteristic::Write);
    serviceData.addCharacteristic(writeNotify);

    this->service = mController->addService(serviceData, mController);

    mAdvertisingData.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    mAdvertisingData.setIncludePowerLevel(false);
    mAdvertisingData.setLocalName("Chart Server");
    mAdvertisingData.setServices(QList<QBluetoothUuid>() << serviceData.uuid());

    mController->startAdvertising(QLowEnergyAdvertisingParameters(), mAdvertisingData,mAdvertisingData);

    connect(mController, SIGNAL(connected()), this, SLOT(onDeviceConnected()));
    connect(mController, SIGNAL(disconnected()), this, SLOT(onDeviceDisconnected()));
    connect(mController, SIGNAL(error(QLowEnergyController::Error)), this, SLOT(onError(QLowEnergyController::Error)));


    connect(this->service, SIGNAL(characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(characteristicChanged(const QLowEnergyCharacteristic &, const QByteArray &)));
    connect(this->service, SIGNAL(characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(characteristicWritten(const QLowEnergyCharacteristic &, const QByteArray &)));
    connect(this->service, SIGNAL(characteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)), this, SLOT(characteristicRead(const QLowEnergyCharacteristic &, const QByteArray &)));


    this->refreshDevices();
}

void MainWindow::sendText(const QString text){

    if(text.isEmpty()){
        return;
    }

    const QLowEnergyCharacteristic cha = this->service->characteristic(QBluetoothUuid(kNotifyId));

    const QByteArray arr = text.toUtf8();

    this->service->writeCharacteristic(cha,arr);

    this->acceptMineText(text);
    ui->lineEdit->setText("");
}

void MainWindow::refreshDevices()
{

    this->deviceModel.clear();

    QBluetoothLocalDevice device;

    QList<QBluetoothAddress> adrs = device.connectedDevices();

    for (QBluetoothAddress adr : adrs) {
        QStandardItem *item = new QStandardItem(adr.toString());
        this->deviceModel.appendRow(item);
    }


}

void MainWindow::acceptMineText(QString text)
{
    QStandardItem *item = new QStandardItem(text);

    this->messageModel.appendRow(item);
    ui->listView_2->scrollToBottom();
}

void MainWindow::acceptOtherText(QString text)
{
    QStandardItem *item = new QStandardItem(text);

    this->messageModel.appendRow(item);
    ui->listView_2->scrollToBottom();
}

void MainWindow::characteristicRead(const QLowEnergyCharacteristic &info,
                                    const QByteArray &value){
    Q_UNUSED(info);

    qDebug() << "read : " << value;

}
void MainWindow::characteristicWritten(const QLowEnergyCharacteristic &info,
                                       const QByteArray &value){
    Q_UNUSED(info);

    qDebug() << "write : " << value << " length : " << value.count();
}
void MainWindow::characteristicChanged(const QLowEnergyCharacteristic &info,
                                       const QByteArray &value){
    Q_UNUSED(info);

    qDebug() << "接受到了 " << QString(value);

    this->acceptOtherText(QString(value));

    const QLowEnergyCharacteristic cha = this->service->characteristic(QBluetoothUuid(kNotifyId));
    this->service->writeCharacteristic(cha,value);
}
void MainWindow::onDeviceConnected(){

    qDebug() << "device connected : ";


    this->refreshDevices();
}

void MainWindow::onDeviceDisconnected(){

    qDebug() << "device disconnected";

    this->refreshDevices();

}

void MainWindow::onError(QLowEnergyController::Error error){

    qDebug() << "onErro : " << error;

}


MainWindow::~MainWindow()
{
    delete ui;
    delete service;
    delete mController;
}

void MainWindow::on_sendButton_clicked()
{
    QString text = ui->lineEdit->text();

    if(text.isEmpty()){
        ui->statusBar->showMessage("发送文本不能为空",2000);
        return;
    }

    if(text.length() > 182){
        ui->statusBar->showMessage("文本长度超过182,当前长度 " + QString::number(text.length()),2000);
        return;
    }

    this->sendText(text);

}
