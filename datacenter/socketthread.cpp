#include "socketthread.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>
SocketThread::SocketThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"MyThread01"<<QThread::currentThread();
    TcplInit();
}

void SocketThread::TcplInit()
{
    tcpsocket = new QTcpSocket(this);
    connect(tcpsocket,SIGNAL(readyRead()),this,SLOT(ReadData()));
}

void SocketThread::Tcpconnect(QString ip,QString portin)
{
    if (tcpsocket->state() == QAbstractSocket::ConnectedState) {
        //如果使用disconnectFromHost()不会重置套接字，isValid还是会为true
        tcpsocket->abort();
        emit tcpstate(false);
    }
    else if (tcpsocket->state() == QAbstractSocket::UnconnectedState) {
        //从界面上读取ip和端口
        const QHostAddress address = QHostAddress(ip);
        const unsigned short port = portin.toUShort();
        //连接服务器
        tcpsocket->connectToHost(address, port);
        if(!tcpsocket->waitForConnected(500))
        {
            emit tcpstate(false);
        }
        else
        {
            emit tcpstate(true);
        }
    }
}

void SocketThread::ReadData()
{
    QByteArray data = tcpsocket->readAll();
    emit tcpdata(data);
}
