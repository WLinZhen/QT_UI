#include "serialthread.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
SerialThread::SerialThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"MyThread02"<<QThread::currentThread();
    serialport = new QSerialPort(this);
    connect(serialport,SIGNAL(readyRead()),this,SLOT(serial_read_date()));
}

void SerialThread::serialconnect(QString com,int bund,int stop,int data,int check)
{
    serialport->setPortName(com);
    switch(bund)
    {
    case 0:m_bundrates = 9600;break;
    case 1:m_bundrates = 38400;break;
    case 2:m_bundrates = 57600;break;
    case 3:m_bundrates = 115200;break;
    case 4:m_bundrates = 460800;break;
    }
    serialport->setBaudRate(m_bundrates);
    switch(data)
    {
    case 0:m_datebits = QSerialPort::Data5;break;
    case 1:m_datebits = QSerialPort::Data6;break;
    case 2:m_datebits = QSerialPort::Data7;break;
    case 3:m_datebits = QSerialPort::Data8;break;
    }
    serialport->setDataBits(m_datebits);
    switch(stop)
    {
    case 0:m_stopbits = QSerialPort::OneStop;break;
    case 1:m_stopbits = QSerialPort::OneAndHalfStop;break;
    case 2:m_stopbits = QSerialPort::TwoStop;break;
    }
    serialport->setStopBits(m_stopbits);
    switch(check)
    {
    case 0:m_checkbits = QSerialPort::NoParity;break;
    case 1:m_checkbits = QSerialPort::OddParity;break;
    case 2:m_checkbits = QSerialPort::EvenParity;break;
    }
    serialport->setParity(m_checkbits);

    serialport->setFlowControl(QSerialPort::NoFlowControl);
    if(serialport->open(QIODeviceBase::ReadWrite) == false)
    {
        qDebug()<<"  串口连接失败\n  1.检查串口是否连接\n  2.设置正确的串口信息";
    }
    else
    {
        emit serialready();
        qDebug()<<"  串口连接成功\n";
    }
}

void SerialThread::serialclose()
{
    serialport->close();

    if(serialport->isOpen()==false)
    {
        emit ser_close();
    }
}

void SerialThread::serial_read_date()
{
    //QElapsedTimer time;
    //time.start();
    QByteArray date = serialport->readAll();
    emit serial_data(date);
    //qDebug() <<"线程，共耗时： "<<time.elapsed() <<" ms。";
}
