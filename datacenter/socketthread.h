#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H
#include <QObject>
#include <QRunnable>
#include <QHostAddress>
#include <QTcpSocket>
class SocketThread : public QObject
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = nullptr);
private:
    void TcplInit();
public slots:
    void Tcpconnect(QString ip,QString portin);
    void ReadData();
public:
    QTcpSocket *tcpsocket;
signals:
    void tcpstate(bool sta);
    void tcpdata(QByteArray data);
};
#endif // SOCKETTHREAD_H
