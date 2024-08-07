#include "diagnosis.h"
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

DiagnosisThread::DiagnosisThread(QObject *parent) : QObject(parent)
{
    qDebug()<<"MyThread03"<<QThread::currentThread();
}

float DiagnosisThread::aver(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]/data.size();
    }
    return ans;
}

float DiagnosisThread::averz(deque<float> data)
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

float DiagnosisThread::variance(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]*data[i]/data.size();
    }
    return ans;
}

float DiagnosisThread::standard_deviation(deque<float> data,float aver)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += (data[i]-aver)*(data[i]-aver)/(data.size()-1);
    }
    return sqrt(ans);
}

float DiagnosisThread::Square_magnitude(deque<float> data)
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

float DiagnosisThread::xp(deque<float> data)
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

float DiagnosisThread::xrms(deque<float> data)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += data[i]*data[i]/data.size();
    }
    return sqrt(ans);
}

float DiagnosisThread::xI(float xp,float aver)
{
    return xp/aver;
}
float DiagnosisThread::xL(float xp,float sm)
{
    return xp/sm;
}

float DiagnosisThread::xQ(deque<float> data,float sd,float aver)
{
    float ans = 0;
    for(int i=0;i<data.size();i++)
    {
        ans += (data[i]-aver)*(data[i]-aver)*(data[i]-aver)*(data[i]-aver)/((data.size()-1)*sd*sd*sd*sd);
    }
    return ans;
}

void DiagnosisThread::diap(float data)
{
    datadia.push_back(data);
    if(datadia.size()>2048)
    {
        datadia.pop_front();
        float qx,lx,ix;
        ix = xI(xp(datadia),averz(datadia));
        lx = xL(xp(datadia),Square_magnitude(datadia));
        qx = xQ(datadia,standard_deviation(datadia,aver(datadia)),aver(datadia));

        if(qx<=5&&lx<=5&&ix<=5)
        {
            emit diac(1);
            emit diacs(1);
        }
        else if(qx>5&&qx<=9&&lx>5&&lx<=9&&ix>5&&ix<=9)
        {
            emit diac(2);
            if(qx>lx)
            {
                emit diacs(2);
            }
            else
            {
                emit diacs(3);
            }
        }
        else if(qx>9&&lx>=9&&ix>=9)
        {
            emit diac(3);
            if(qx>lx)
            {
                emit diacs(4);
            }
            else
            {
                emit diacs(5);
            }
        }
        datadia.clear();
    }
}
