#ifndef PARSEDATA_H
#define PARSEDATA_H
#include <QObject>
#include <QRunnable>
class ParseThread : public QObject
{
    Q_OBJECT
public:
    explicit ParseThread(QObject *parent = nullptr);
private:
    QByteArray serialtemp;
    QByteArray sockettemp;
public slots:
    void tcpparse(QByteArray data);
public:

signals:
    void com_data(float x,float y,float z);
    void sp_data(float x,float y,float z,float speed,float temperaturein,float humidityin,float temperatureout,float humidityout);
};
#endif // PARSEDATA_H
