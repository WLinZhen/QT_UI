#ifndef MAINFACE_H
#define MAINFACE_H

#include "qcustomplot.h"
#include <QWidget>
#include <QMouseEvent>
#include "QDateTime"
#include <QSerialPort>
#include <QSerialPortInfo>
#include "QSqlDatabase"
#include "serialthread.h"
#include "socketthread.h"
#include "sqlthread.h"
#include "parsedata.h"
#include <QThread>
#include <QThreadPool>
#include <QElapsedTimer>
#include <QSqlTableModel> // 数据库显示模型
#include <QSqlQuery>   // 操作数据库接口
#include <QSqlRecord>
#include <QMovie>
#include "diagnosis.h"
namespace Ui {
class mainface;
}

class mainface : public QWidget
{
    Q_OBJECT

public:
    explicit mainface(QWidget *parent = nullptr);
    ~mainface();

private:
    Ui::mainface *ui;
    int m_id;
public slots:
    void iduser(int id);
//ui
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private slots:
    void on_close_clicked();
    void on_minswitch_clicked();
    void on_serial_connect_clicked();
    void on_se_clear_clicked();
    void on_re_clear_clicked();
    void on_tcp_connect_clicked();
    void on_colloct_switch_clicked();
    void on_Toleft_clicked();
    void on_Toright_clicked();
    void on_Toleftface_clicked();
    void on_Torightface_clicked();
private:
    //记录鼠标，窗口位置
    QPoint windowPos;
    QPoint mousePos;
    QPoint dPos;
    void initface();
    int chouyang = 0;
    int chouyangre = 0;
//uart
private:
    void SerialInit();
    void serialthread_init();
signals:
    void connectserial(QString com,int bund,int stop,int data,int check);
    void closeserial();
private:
    QThread *Thread01;
    SerialThread *serialthread;
public slots:
    void readyserial();
    void close_ser();
    void data_serial(QByteArray date);
//tcp
private:
    void socketthread_init();
    bool socketsta = false;
signals:
    void tcpconnect(QString ip,QString portin);
private:
    QThread *Thread02;
    SocketThread *socketthread;
public slots:
    void SocketState(bool sta);
    void datatcp(QByteArray data);
private:
    void parsethread_init();
private:
    QThread *Thread03;
    ParseThread *parsethread;
signals:
    void parsetcp(QByteArray data);
public slots:
    void data_com(float x,float y,float z);
    void data_sp(float x,float y,float z,float speed,float temperature,float humidity,float temperatureout,float humidityout);
    //
private:
    void sqlthread_init();
private:
    QThread *Thread04;
    SqlThread *sqlthread;
    QSqlDatabase db;
    QSqlQuery query;
    //QSqlTableModel *model;
signals:
    void ToSql01(float x,float y,float z);
    void ToSql02(float speed,float temperaturein,float humidityin,float temperatureout,float humidityout);
    void colinfo(int number,int jump);
private slots:
    void colfinish();
//test
//public:
    //int i = 0;
    void copedata();
    void on_clearvir_clicked();
//折线图
    void on_Process_clicked();

    void on_Toleftface_2_clicked();

    void on_Torightface_2_clicked();

    void on_Refresh_clicked();

private:
    void graph_init();
    QCustomPlot *virgra;
    float xAx=0;
    QCustomPlot *humgra;
    float xAxh=0;
    QCustomPlot *temgra;
    float xAxt=0;
    QCustomPlot *speedgra;
    float xAxs=0;
    QCustomPlot *virx;
    QCustomPlot *viry;
    QCustomPlot *virz;
    void graph_init_cope();
    QCustomPlot *sq;
    QCustomPlot *sl;
    QCustomPlot *si;


private:
    QMovie *m;
    void gif_init();

private:
    void dia_init();
    QThread *Thread05;
    DiagnosisThread *diagnosisthread;
private slots:
    void cdia(int a);
    void csdia(int b);
signals:
    void pdia(float data);
};

#endif // MAINFACE_H
