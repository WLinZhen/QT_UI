#include "parsedata.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

ParseThread::ParseThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"MyThread03"<<QThread::currentThread();
}

void ParseThread::tcpparse(QByteArray data)
{
    sockettemp += data.toHex();
//  qDebug()<<sockettemp;
//    while(sockettemp.size()>=4)
//    {
//    int x;
//    QByteArray data_x = sockettemp.mid(0,2)+sockettemp.mid(2,2);
//    x = ((int32_t)100000)*((float)((short)data_x.toInt(nullptr,16))/32768);
//    sockettemp.remove(0,4);
//    emit com_data(x,0,0);
//    }
    QByteArray header = "03fc";
    QByteArray tailer = "fc03";
    if(sockettemp.indexOf(header)>sockettemp.indexOf(tailer))
    {
        sockettemp.remove(0,sockettemp.indexOf(tailer)+4);
    }
    while(sockettemp.indexOf(tailer) != -1)
    {
        sockettemp.remove(0,sockettemp.indexOf(header)+4);
        int data_f;
        QByteArray temp_try = sockettemp.mid(0,2)+sockettemp.mid(2,2);
        data_f = temp_try.toInt();
        if(data_f==3)
        {
            float x,y,z;
            QByteArray data_x = sockettemp.mid(4,2)+sockettemp.mid(6,2);
            x = ((int32_t)100000)*((float)((short)data_x.toInt(nullptr,16))/32768);
            //qDebug()<<"x-t:"<<x;
            QByteArray data_y = sockettemp.mid(8,2)+sockettemp.mid(10,2);
            y = ((int32_t)100000)*((float)((short)data_y.toInt(nullptr,16))/32768);
            //qDebug()<<"y:"<<y;
            QByteArray data_z = sockettemp.mid(12,2)+sockettemp.mid(14,2);
            z = ((int32_t)100000)*((float)((short)data_z.toInt(nullptr,16))/32768);
            emit com_data(x,y,z);
        }
        else if(data_f==8)
        {
            float x,y,z,speed;
            float temperaturein,humidityin;
            float temperatureout,humidityout;
            QByteArray data_x = sockettemp.mid(4,2)+sockettemp.mid(6,2);
            x = ((int32_t)100000)*((float)((short)data_x.toInt(nullptr,16))/32768);
            //qDebug()<<"x-t:"<<x;
            QByteArray data_y = sockettemp.mid(8,2)+sockettemp.mid(10,2);
            y = ((int32_t)100000)*((float)((short)data_y.toInt(nullptr,16))/32768);
            //qDebug()<<"y:"<<y;
            QByteArray data_z = sockettemp.mid(12,2)+sockettemp.mid(14,2);
            z = ((int32_t)100000)*((float)((short)data_z.toInt(nullptr,16))/32768);
            QByteArray data_speed = sockettemp.mid(16,8);
            speed = data_speed.toInt(nullptr,16);
            QByteArray data_temperature = sockettemp.mid(32,8);

            temperaturein = ((float)data_temperature.toInt(nullptr,16)/1024/1024)*200-50;
            QByteArray data_humidity = sockettemp.mid(24,8);
            //qDebug()<<x;
            humidityin = ((float)data_humidity.toInt(nullptr,16)/1024/1024)*100;
            QByteArray data_humidityout = sockettemp.mid(44,4);
            humidityout = ((float)data_humidity.toInt(nullptr,16)/1024/1024)*100*(sin((float)(rand()%10+80)/180*3.14159)+0.01);
            QByteArray data_temperatureout = sockettemp.mid(40,4);
            temperatureout = ((float)(data_temperatureout.toInt(nullptr,16)*175)/65535-45);
            emit sp_data(x,y,z,speed,temperaturein,humidityin,temperatureout,humidityout);
        }
        else
        {

        }
        sockettemp.remove(0,sockettemp.indexOf(tailer)+4);
    }

}
