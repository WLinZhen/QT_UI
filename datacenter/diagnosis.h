#ifndef DIAGNOSIS_H
#define DIAGNOSIS_H
#include <QObject>
#include <QRunnable>
#include <deque>
#include <cmath>
using namespace std;
class DiagnosisThread : public QObject
{
    Q_OBJECT
public:
    explicit DiagnosisThread(QObject *parent = nullptr);
private:
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
private:
    deque<float> datadia;

public slots:
    void diap(float data);
signals:
    void diac(int a);
    void diacs(int b);
};

#endif // DIAGNOSIS_H
