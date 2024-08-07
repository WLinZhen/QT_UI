#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include <QObject>
#include <QRunnable>
#include <QSerialPort>
#include <QSerialPortInfo>
class SerialThread : public QObject
{
    Q_OBJECT
public:
    explicit SerialThread(QObject *parent = nullptr);
signals:
    void serialready();
    void ser_close();
    void serial_data(QByteArray date);
public slots:
    void serialconnect(QString com,int bund,int stop,int data,int check);
    void serialclose();
private slots:
    void serial_read_date();
public:
    QSerialPort *serialport;
    QSerialPort::Parity m_checkbits;
    QSerialPort::StopBits m_stopbits;
    QSerialPort::DataBits m_datebits;
    int m_bundrates;
};
#endif // SERIALTHREAD_H
