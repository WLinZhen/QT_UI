#include "sqlthread.h"
SqlThread::SqlThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"MyThread01"<<QThread::currentThread();
    SqlInit();
}

void SqlThread::infocol(int number,int jump)
{
    col_number = number;
    col_jump = jump;
    qDebug() << col_number << col_jump;
    qDebug() <<((float)col_number)/1000*col_jump;
}

void SqlThread::SqlInit()
{
    connect(this,&SqlThread::cope,this,&SqlThread::copedata);
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("WLZ123qe");
    db.setDatabaseName("userinfo");
    db.open();
    query = QSqlQuery ("",db);
    QString str1 = QString("SELECT * FROM datalists ORDER BY id DESC LIMIT 1;");
    query.exec(str1);
    query.next();
    id = query.value("id").toInt();
    if(db.open())
    {
        qDebug() <<"连接成功998"<<id;
    }
    else
    {
        qDebug() <<"数据库连接失败";
    }
    db.close();
}

void SqlThread::SqlTo01(float x,float y,float z)
{
    datax.push_back(x);
    datay.push_back(y);
    dataz.push_back(z);
    //qDebug() <<datax.size();
    if(datax.size()>(((float)col_number)/1000*col_jump))
    {
        datax.pop_front();
        datay.pop_front();
        dataz.pop_front();
    }
}

void SqlThread::SqlTo02(float speed,float temperaturein,float humidityin,float temperatureout,float humidityiout)
{
    qDebug() <<datax.size();
    if(datax.size()==((float)col_number)/1000*col_jump)
    {
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("localhost");
        db.setUserName("root");
        db.setPort(3306);
        db.setPassword("WLZ123qe");
        db.setDatabaseName("userinfo");
        db.open();
        query = QSqlQuery ("",db);
        id++;
        QString save_id = QString::number(id);
        QString save_speed = QString::number(speed);
        QString save_temperaturein = QString::number(temperaturein);
        QString save_humidityin = QString::number(humidityin);
        QString save_humidityout = QString::number(humidityiout);
        QString save_temperatureout = QString::number(temperatureout);
        QString save_time = QString::number(col_number);
        QString save_fre = QString::number(col_jump);
        QString str1 = QString("insert into datalists(id,speed,temperaturein,humidityin,temperatureout,humidityout,sampled_time,sampled_fre,vir) values("+save_id+","+save_speed+","+save_temperaturein+","+save_humidityin+","+save_temperatureout+","+save_humidityout+","+save_time+","+save_fre+",'"+save_speed+"_"+save_id+"');");
        if(query.exec(str1)==false)
        {
            qDebug() <<"数据库添加数据失败";
        }
        else
        {
            qDebug() <<"已采集";
        }
        QString str2 = QString("create table "+save_speed+"_"+save_id+"(`id` int,`virx` float,`viry` float,`virz` float);");
        if(query.exec(str2)==false)
        {
            qDebug() <<"数据失败";
        }
        else
        {
            qDebug() <<"已采集";
        }
        for(int i=0;i<datax.size();i+=1+(1024/col_jump-1))
        {
            QString save_iid = QString::number(i+1);
            QString save_x = QString::number(datax[i]);
            QString save_y = QString::number(datay[i]);
            QString save_z = QString::number(dataz[i]);
            QString str3 = QString("insert into "+save_speed+"_"+save_id+"(id,virx,viry,virz) values("+save_iid+","+save_x+","+save_y+","+save_z+");");
            if(query.exec(str3)==false)
            {
                qDebug() <<"数据库添加数据失败";
            }
            else
            {
                qDebug() <<"已采集";
            }
        }
        emit cope(datax,datay,dataz,speed,id);
        datax.clear();
        datay.clear();
        dataz.clear();
        if(col_number!=0)
        {
            emit finishcol();
        }
    }    
}

void SqlThread::copedata(deque<float> datax,deque<float> datay,deque<float> dataz,float speed,int id)
{
    float qx,lx,ix;
    ix = xI(xp(datax),averz(datax));
    lx = xL(xp(datax),Square_magnitude(datax));
    qx = xQ(datax,standard_deviation(datax,aver(datax)),aver(datax));
    float qy,ly,iy;
    iy = xI(xp(datay),averz(datay));
    ly = xL(xp(datay),Square_magnitude(datay));
    qy = xQ(datay,standard_deviation(datay,aver(datay)),aver(datay));
    float qz,lz,iz;
    iz = xI(xp(dataz),averz(dataz));
    lz = xL(xp(dataz),Square_magnitude(dataz));
    qz = xQ(dataz,standard_deviation(dataz,aver(dataz)),aver(dataz));
    QString save_speed = QString::number(speed);
    QString save_qx = QString::number(qx);
    QString save_lx = QString::number(lx);
    QString save_ix = QString::number(ix);
    QString save_qy = QString::number(qy);
    QString save_ly = QString::number(ly);
    QString save_iy = QString::number(iy);
    QString save_qz = QString::number(qz);
    QString save_lz = QString::number(lz);
    QString save_iz = QString::number(iz);
    QString str1 = QString("insert into copedat(speed,qx,lx,ix,qy,ly,iy,qz,lz,iz) values("+save_speed+","+save_qx+","+save_lx+","+save_ix+","+save_qy+","+save_ly+","+save_iy+","+save_qz+","+save_lz+","+save_iz+");");
    if(query.exec(str1)==false)
    {
        qDebug() <<"数据库添加数据失败";
    }
    else
    {
        qDebug() <<"已采集";
    }
}

float SqlThread::aver(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]/data.size();
    }
    return ans;
}

float SqlThread::averz(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        if(data[i]<=0)
        {
            data[i]*=-1;
        }
        ans += data[i]/data.size();
    }
    return ans;
}

float SqlThread::variance(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]*data[i]/data.size();
    }
    return ans;
}

float SqlThread::standard_deviation(deque<float> data,float aver)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += (data[i]-aver)*(data[i]-aver)/(data.size()-1);
    }
    return sqrt(ans);
}

float SqlThread::Square_magnitude(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        if(data[i]<=0)
        {
            data[i]*=-1;
        }
        ans += sqrt(data[i])/data.size();
    }
    return ans*ans;
}

float SqlThread::xp(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        if(data[i]<=0)
        {
            data[i]*=-1;
        }
        ans = max(ans,data[i]);
    }
    return ans;
}

float SqlThread::xrms(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]*data[i]/data.size();
    }
    return sqrt(ans);
}

float SqlThread::xI(float xp,float aver)
{
    return xp/aver;
}
float SqlThread::xL(float xp,float sm)
{
    return xp/sm;
}

float SqlThread::xQ(deque<float> data,float sd,float aver)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += (data[i]-aver)*(data[i]-aver)*(data[i]-aver)*(data[i]-aver)/((data.size()-1)*sd*sd*sd*sd);
    }
    return ans;
}
