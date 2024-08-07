#ifndef SQLTHREAD_H
#define SQLTHREAD_H
#include <QObject>
#include <QRunnable>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <deque>
#include <cmath>
using namespace std;
class SqlThread : public QObject
{
    Q_OBJECT
public:
    explicit SqlThread(QObject *parent = nullptr);
private:
    void SqlInit();
    float aver(deque<float> data);
    float averz(deque<float> data);
    float variance(deque<float> data);
    float standard_deviation(deque<float> data,float aver);
    float Square_magnitude(deque<float> data);
    float xp(deque<float> data);
    float xrms(deque<float> data);
    float xI(float xp,float aver);
    float xL(float xp,float sm);
    float xQ(deque<float> data,float sd,float aver);
public slots:
    void SqlTo01(float x,float y,float z);
    void SqlTo02(float speed,float temperaturein,float humidityin,float temperatureout,float humidityout);
    void infocol(int number,int jump);
    void copedata(deque<float> datax,deque<float> datay,deque<float> dataz,float speed,int id);
public:
    int id;
    deque<float> datax;
    deque<float> datay;
    deque<float> dataz;
    int col_number=1024;
    int col_jump=0;
    QSqlDatabase db;
    QSqlQuery query;
signals:
    void finishcol();
    void cope(deque<float> datax,deque<float> datay,deque<float> dataz,float speed,int id);

};
#endif // SQLTHREAD_H
