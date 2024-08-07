#include "mainface.h"
#include "ui_mainface.h"

#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>
#include <QPainter>
#include <QProxyStyle>

class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
        }

        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x134f8b);
                    painter->setBrush(QBrush(0x134f8b));
                    painter->drawRect(allRect.adjusted(0, 0, -0, -0));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xffffff);
                }
                else {
                    painter->setPen(0xeaeaea);
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};


mainface::mainface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mainface)
{
    ui->setupUi(this);
    initface();
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

    SerialInit();
    serialthread_init();
    //qDebug()<<ui->text_re->size();
    socketthread_init();
    parsethread_init();
    sqlthread_init();
    graph_init();
    gif_init();
    graph_init_cope();
    dia_init();
}

mainface::~mainface()
{
    QThreadPool::globalInstance()->waitForDone();
    delete ui;
}
void mainface::iduser(int id)
{
    m_id = id;
    qDebug()<<m_id;
}

void mainface::initface()
{
    QDateTime current = QDateTime::currentDateTime();
    QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
    ui->work_state->append("\n"+timeApares+"\n"+"Welcome back");
    setWindowFlags(Qt::FramelessWindowHint);    //隐藏标题栏（无边框）
    setAttribute(Qt::WA_StyledBackground);      //启用样式背景绘制
    setAttribute(Qt::WA_TranslucentBackground); //背景透明
    setAttribute(Qt::WA_Hover);
//    ui->text_re->document()->setMaximumBlockCount(4096);
//    ui->VibrationData->document()->setMaximumBlockCount(4096);
//    ui->OtherData->document()->setMaximumBlockCount(4096);
}

void mainface::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();       // 获得部件当前位置
    this->mousePos = event->globalPos(); // 获得鼠标位置
    this->dPos = mousePos - windowPos;   // 移动后部件所在的位置
}

void mainface::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void mainface::on_close_clicked()
{
    close();
}

void mainface::on_minswitch_clicked()
{
    showMinimized();
}

void mainface::SerialInit()
{
    QSerialPortInfo serialinfo;
    foreach (serialinfo, QSerialPortInfo::availablePorts())
    {
        //qDebug()<<serialinfo.portName();
        ui->choose_com->addItem(serialinfo.portName());
    }
    //波特率
    ui->choose_port->addItem("9600");
    ui->choose_port->addItem("38400");
    ui->choose_port->addItem("57600");
    ui->choose_port->addItem("115200");
    ui->choose_port->addItem("460800");

    //停止位
    ui->choose_stopbits->addItem("1");
    ui->choose_stopbits->addItem("1.5");
    ui->choose_stopbits->addItem("2");

    //数据位
    ui->choose_databits->addItem("5");
    ui->choose_databits->addItem("6");
    ui->choose_databits->addItem("7");
    ui->choose_databits->addItem("8");
    ui->choose_databits->setCurrentIndex(3);

    //校验位
    ui->choose_checkbits->addItem("无校验");
    ui->choose_checkbits->addItem("奇校验");
    ui->choose_checkbits->addItem("偶校验");
}

void mainface::on_serial_connect_clicked()
{
    QString str1 = "连接串口";
    QString str2 = ui->serial_connect->text();
    if(str1.compare(str2) == 0)
    {
        emit connectserial(ui->choose_com->currentText(),ui->choose_port->currentIndex(),ui->choose_stopbits->currentIndex(),ui->choose_databits->currentIndex(),ui->choose_checkbits->currentIndex());
    }
    else
    {
        emit closeserial();
    }
}

void mainface::serialthread_init()
{
    Thread01 = new QThread();
    connect(Thread01,&QThread::finished,Thread01,&QThread::deleteLater);
    serialthread = new SerialThread();
    serialthread->moveToThread(Thread01);
    connect(this,&mainface::connectserial, serialthread,&SerialThread::serialconnect);
    connect(this,&mainface::closeserial, serialthread,&SerialThread::serialclose);
    connect(serialthread,&SerialThread::serialready, this,&mainface::readyserial);
    connect(serialthread,&SerialThread::ser_close, this,&mainface::close_ser);
    connect(serialthread,&SerialThread::serial_data, this,&mainface::data_serial);
    Thread01->start();
}

void mainface::readyserial()
{
    QDateTime current = QDateTime::currentDateTime();
    QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
    ui->work_state->append("\n"+timeApares+"\n"+"串口连接成功");
    ui->serial_connect->setText("关闭串口");
}

void mainface::close_ser()
{
    QDateTime current = QDateTime::currentDateTime();
    QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
    ui->work_state->append("\n"+timeApares+"\n"+"串口已关闭");
    ui->serial_connect->setText("连接串口");
}

void mainface::data_serial(QByteArray date)
{
    //QElapsedTimer time;
    //time.start();
    //i++;
    QByteArray temp = date;
    if(ui->hex_re->isChecked())
    {
        temp = temp.toHex();
    }
    if(ui->time_postion->isChecked())
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->text_re->append(timeApares+"\n"+temp);//+"\t"+QString::number(i)
    }
    else
    {
        ui->text_re->append(temp);//+"\t"+QString::number(i)

    }
    //qDebug() <<"线程，共耗时： "<<time.elapsed() <<" ms。";
}

void mainface::on_se_clear_clicked()
{
    ui->text_se->clear();
}


void mainface::on_re_clear_clicked()
{
    ui->text_re->clear();
}

void mainface::SocketState(bool sta)
{
    if(sta)
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->work_state->append("\n"+timeApares+"\n"+"服务器连接成功");
        ui->tcp_connect->setText("断开服务器");
        socketsta = sta;
    }
    else
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->work_state->append("\n"+timeApares+"\n"+"服务器已断开");
        ui->tcp_connect->setText("连接服务器");
        socketsta = sta;
        if(ui->colloct_switch->text() == "停止采样")
        {
            ui->colloct_switch->setText("开始采样");
            disconnect(this,&mainface::ToSql01,sqlthread,&SqlThread::SqlTo01);
            disconnect(this,&mainface::ToSql02,sqlthread,&SqlThread::SqlTo02);
        }
    }
}

void mainface::socketthread_init()
{
    Thread02 = new QThread();
    connect(Thread02,&QThread::finished,Thread02,&QThread::deleteLater);
    socketthread = new SocketThread();
    socketthread->moveToThread(Thread02);
    connect(this,&mainface::tcpconnect, socketthread,&SocketThread::Tcpconnect);
    connect(socketthread,&SocketThread::tcpstate, this,&mainface::SocketState);
    connect(socketthread,&SocketThread::tcpdata, this,&mainface::datatcp);
    Thread02->start();
}

void mainface::on_tcp_connect_clicked()
{
    emit tcpconnect(ui->tcp_ip->text(),ui->tcp_port->text());
}

void mainface::datatcp(QByteArray data)
{
    //i++;
    QByteArray temp = data;
    chouyangre++;
    if(chouyangre>=100)
    {
        chouyangre = 0;
        if(ui->hex_re->isChecked())
        {
            temp = temp.toHex();
        }
        if(ui->time_postion->isChecked())
        {
            QDateTime current = QDateTime::currentDateTime();
            QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
            ui->text_re->append(timeApares+"\n"+temp);//+"\t"+QString::number(i)
        }
        else
        {
            ui->text_re->append(temp);//+"\t"+QString::number(i)
        }
    }

    emit parsetcp(data);
}

void mainface::parsethread_init()
{
    Thread03 = new QThread();
    connect(Thread03,&QThread::finished,Thread03,&QThread::deleteLater);
    parsethread = new ParseThread();
    parsethread->moveToThread(Thread03);
    connect(this,&mainface::parsetcp,parsethread,&ParseThread::tcpparse);
    connect(parsethread,&ParseThread::com_data,this,&mainface::data_com);
    connect(parsethread,&ParseThread::sp_data,this,&mainface::data_sp);
    Thread03->start();
}

void mainface::data_com(float x,float y,float z)
{
    emit pdia(x);
    chouyang++;
    if(chouyang>=100)
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        chouyang = 0;
        virgra->xAxis->setRange(xAx,8, Qt::AlignRight);
        virgra->replot(QCustomPlot::rpQueuedReplot);
        humgra->xAxis->setRange(xAxh,8, Qt::AlignRight);
        humgra->replot(QCustomPlot::rpQueuedReplot);
        temgra->xAxis->setRange(xAxt,8, Qt::AlignRight);
        temgra->replot(QCustomPlot::rpQueuedReplot);
        speedgra->xAxis->setRange(xAxs,8, Qt::AlignRight);
        speedgra->replot(QCustomPlot::rpQueuedReplot);
        virx->xAxis->setRange(xAx,8, Qt::AlignRight);
        virx->replot(QCustomPlot::rpQueuedReplot);
        viry->xAxis->setRange(xAx,8, Qt::AlignRight);
        viry->replot(QCustomPlot::rpQueuedReplot);
        virz->xAxis->setRange(xAx,8, Qt::AlignRight);
        virz->replot(QCustomPlot::rpQueuedReplot);
        ui->VibrationData->append(timeApares+"\n"+"vibration_X:"+QString::number(x)+"   vibration_Y:"+QString::number(y)+"   vibration_Z:"+QString::number(z));
    }
    virgra->graph(0)->addData(xAx,x);
    virgra->graph(1)->addData(xAx,y);
    virgra->graph(2)->addData(xAx,z);
    virx->graph(0)->addData(xAx,x);
    viry->graph(0)->addData(xAx,y);
    virz->graph(0)->addData(xAx,z);
    xAx += 0.0005;

    //绘图
    //virgra->replot();
    if(ui->postionx->isChecked()==false)
    {
        x = 0;
    }
    if(ui->postiony->isChecked()==false)
    {
        y = 0;
    }
    if(ui->postionz->isChecked()==false)
    {
        z = 0;
    }
    emit ToSql01(x,y,z);
}

void mainface::data_sp(float x,float y,float z,float speed,float temperaturein,float humidityin,float temperatureout,float humidityout)
{
    emit pdia(x);
    m->setSpeed((int)speed);
    ui->speedc->setText(QString::number(speed));
    ui->teminc->setText(QString::number(temperatureout));
    ui->temoutc->setText(QString::number(temperaturein));
    ui->huminc->setText(QString::number(humidityout));
    ui->humoutc->setText(QString::number(humidityin));
    QDateTime current = QDateTime::currentDateTime();
    QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
    chouyang++;
    if(chouyang>=100)
    {
        ui->VibrationData->append(timeApares+"\n"+"vibration_X:"+QString::number(x)+"   vibration_Y:"+QString::number(y)+"   vibration_Z:"+QString::number(z));
        chouyang = 0;
        virgra->xAxis->setRange(xAx,8, Qt::AlignRight);
        virgra->replot(QCustomPlot::rpQueuedReplot);
        virx->xAxis->setRange(xAx,8, Qt::AlignRight);
        virx->replot(QCustomPlot::rpQueuedReplot);
        viry->xAxis->setRange(xAx,8, Qt::AlignRight);
        viry->replot(QCustomPlot::rpQueuedReplot);
        virz->xAxis->setRange(xAx,8, Qt::AlignRight);
        virz->replot(QCustomPlot::rpQueuedReplot);
        humgra->xAxis->setRange(xAxh,8, Qt::AlignRight);
        humgra->replot(QCustomPlot::rpQueuedReplot);
        temgra->xAxis->setRange(xAxt,8, Qt::AlignRight);
        temgra->replot(QCustomPlot::rpQueuedReplot);
        speedgra->xAxis->setRange(xAxs,8, Qt::AlignRight);
        speedgra->replot(QCustomPlot::rpQueuedReplot);
    }
    ui->OtherData->append(timeApares+"\n"+"Speed:"+QString::number(speed)+"   Temperature:"+QString::number(temperaturein)+"   Humidity:"+QString::number(humidityin));

    virgra->graph(0)->addData(xAx,x);
    virgra->graph(1)->addData(xAx,y);
    virgra->graph(2)->addData(xAx,z);
    virx->graph(0)->addData(xAx,x);
    viry->graph(0)->addData(xAx,y);
    virz->graph(0)->addData(xAx,z);
    xAx += 0.0005;
    humgra->graph(0)->addData(xAxh,humidityin);
    humgra->graph(1)->addData(xAxh,humidityout);
    xAxh += 0.5;
    temgra->graph(0)->addData(xAxt,temperaturein);
    temgra->graph(1)->addData(xAxt,temperatureout);
    xAxt += 0.5;
    speedgra->graph(0)->addData(xAxs,speed);
    xAxs += 0.5;

    //绘图
    //virgra->replot();
//    if(ui->VibrationData->toPlainText().count()>4096)
//    {
//        ui->VibrationData->clear();
//    }
//    if(ui->OtherData->toPlainText().count()>4096)
//    {
//        ui->OtherData->clear();
//    }

    if(ui->postionx->isChecked()==false)
    {
        x = 0;
    }
    if(ui->postiony->isChecked()==false)
    {
        y = 0;
    }
    if(ui->postionz->isChecked()==false)
    {
        z = 0;
    }
    emit ToSql01(x,y,z);
    if(ui->postionspeed->isChecked()==false)
    {
        speed = 0;
    }
    if(ui->postiontem->isChecked()==false)
    {
        temperaturein = 0;
        temperatureout = 0;
    }
    if(ui->postionhum->isChecked()==false)
    {
        humidityin = 0;
        humidityout = 0;
    }
    emit ToSql02(speed,temperaturein,humidityin,temperatureout,humidityout);
}

void mainface::on_colloct_switch_clicked()
{
    if(socketsta)
    {
    QString str1 = "开始采样";
    QString str2 = ui->colloct_switch->text();
    if(str1.compare(str2) == 0)
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->work_state->append("\n"+timeApares+"\n"+"开始采样");
        ui->colloct_switch->setText("停止采样");
        int col_number = ui->time_col->text().toInt();
        int col_jump = ui->jump_col->text().toInt();
        if(ui->col_time->isChecked()==false)
        {
            col_number=0;
        }
        if(ui->col_jump->isChecked()==false)
        {
            col_jump=1024;
        }
        emit colinfo(col_number,col_jump);
        connect(this,&mainface::ToSql01,sqlthread,&SqlThread::SqlTo01);
        connect(this,&mainface::ToSql02,sqlthread,&SqlThread::SqlTo02);
    }
    else
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->work_state->append("\n"+timeApares+"\n"+"停止采样");
        ui->colloct_switch->setText("开始采样");
        disconnect(this,&mainface::ToSql01,sqlthread,&SqlThread::SqlTo01);
        disconnect(this,&mainface::ToSql02,sqlthread,&SqlThread::SqlTo02);
    }
    }
    else
    {
        QDateTime current = QDateTime::currentDateTime();
        QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
        ui->work_state->append("\n"+timeApares+"\n"+"请连接服务器");
    }
}

void mainface::sqlthread_init()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("WLZ123qe");
    db.setDatabaseName("userinfo");
    db.open();
    query = QSqlQuery ("",db);
    query.exec("select * from datalists");
    ui->sqltable->setRowCount(query.size());
    ui->sqltable->setColumnCount(query.record().count());
    int w = 0;
    while (query.next())
    {
        for (int j = 0; j < query.record().count(); j++)
        {
        QString str = query.value(j).toString();
        QTableWidgetItem *item = new QTableWidgetItem(str);
        ui->sqltable->setItem(w, j, item);
        }
        w++;
    }
    ui->sqltable->horizontalHeader()->setStretchLastSection(true);
    ui->sqltable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->time_col->setText("1000");
    ui->jump_col->setText("1024");
    QStringList header;
    header.append("ID");
    header.append("转速");
    header.append("环境温度");
    header.append("环境湿度");
    header.append("轴承温度");
    header.append("轴承湿度");
    header.append("采样时间");
    header.append("采样频率");
    header.append("振动信号");
    ui->sqltable->setHorizontalHeaderLabels(header);
    ui->sqltable->verticalHeader()->hide();
    Thread04 = new QThread();
    connect(Thread04,&QThread::finished,Thread04,&QThread::deleteLater);
    sqlthread = new SqlThread();
    sqlthread->moveToThread(Thread04);
    connect(this,&mainface::colinfo,sqlthread,&SqlThread::infocol);
    connect(sqlthread,&SqlThread::finishcol,this,&mainface::colfinish);
    Thread04->start();
}

void mainface::on_Toleft_clicked()
{
    int nCount = ui->stackedWidget->count();
    int nIndex = ui->stackedWidget->currentIndex();

    // 获取下一个需要显示的页面索引
    --nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex < 0)
        nIndex = nCount-1;

    ui->stackedWidget->setCurrentIndex(nIndex);
}


void mainface::on_Toright_clicked()
{
    int nCount = ui->stackedWidget->count();
    int nIndex = ui->stackedWidget->currentIndex();

    // 获取下一个需要显示的页面索引
    ++nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;

    ui->stackedWidget->setCurrentIndex(nIndex);
}


void mainface::on_Toleftface_clicked()
{
    int nCount = ui->dataview->count();
    int nIndex = ui->dataview->currentIndex();

    // 获取下一个需要显示的页面索引
    --nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex < 0)
        nIndex = nCount-1;

    ui->dataview->setCurrentIndex(nIndex);
}


void mainface::on_Torightface_clicked()
{
    int nCount = ui->dataview->count();
    int nIndex = ui->dataview->currentIndex();

    // 获取下一个需要显示的页面索引
    ++nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;

    ui->dataview->setCurrentIndex(nIndex);
}

void mainface::colfinish()
{
    QDateTime current = QDateTime::currentDateTime();
    QString timeApares = "["+current.toString(("yyyy-MM-dd hh:mm:ss.zzz")) + "] ";
    ui->work_state->append("\n"+timeApares+"\n"+"停止采样");
    ui->colloct_switch->setText("开始采样");
    disconnect(this,&mainface::ToSql01,sqlthread,&SqlThread::SqlTo01);
    disconnect(this,&mainface::ToSql02,sqlthread,&SqlThread::SqlTo02);
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("WLZ123qe");
    db.setDatabaseName("userinfo");
    db.open();
    query = QSqlQuery ("",db);
    query.exec("select * from datalists");
    ui->sqltable->setRowCount(query.size());
    ui->sqltable->setColumnCount(query.record().count());
    int w = 0;
    while (query.next())
    {
        for (int j = 0; j < query.record().count(); j++)
        {
        QString str = query.value(j).toString();
        QTableWidgetItem *item = new QTableWidgetItem(str);
        ui->sqltable->setItem(w, j, item);
        }
        w++;
    }
    db.close();
}

void mainface::on_clearvir_clicked()
{
    ui->VibrationData->clear();
    ui->OtherData->clear();
}

void mainface::copedata()
{
    float pi= 3.1415927;
    for(int i=0;i<=2500;i++)
    {
        float x,y,z;
        x = (rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i)+ 0.6*sin(2*pi*6*i)*2.557*cos(2*pi*31*i))*exp(((rand()%8+1)+i-(rand()%7+1))/2500)+exp(((rand()%8+1)+i-(rand()%7+1))/2500)*(rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i));
        y = (rand()%4+1)*((rand()%13+1)+160.56-(rand()%7+1))*abs(0.63*sin(2*pi*6*i)+ 0.6*sin(2*pi*15*i)*2.123*cos(2*pi*54*i))*exp(((rand()%9+1)+i-(rand()%8+1))/2500)+exp(((rand()%8+1)+i-(rand()%7+1))/2500)*(rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i));
        z = (rand()%4+1)*((rand()%12+1)+180.25-(rand()%6+1))*abs(0.71*sin(2*pi*3*i)+ 0.6*sin(2*pi*26*i)*2.535*cos(2*pi*20*i))*exp(((rand()%5+1)+i-(rand()%6+1))/2500)+exp(((rand()%8+1)+i-(rand()%7+1))/2500)*(rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i));
        if(x<(rand()%2+1)*((rand()%15+1)+130.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i)+ 0.6*sin(2*pi*6*i)*2.557*cos(2*pi*31*i)+0.5*cos(2*pi*8*i))*exp(((rand()%8+1)+i-(rand()%7+1))/2500)+exp(((rand()%8+1)+i-(rand()%7+1))/2500)*(rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i)))
        {
        x = 1.5*(rand()%2+1)*((rand()%15+1)+130.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i)+ 0.6*sin(2*pi*6*i)*2.557*cos(2*pi*31*i)+0.5*cos(2*pi*8*i))*exp(((rand()%8+1)+i-(rand()%7+1))/2500)+exp(((rand()%8+1)+i-(rand()%7+1))/2500)*(rand()%4+1)*((rand()%15+1)+150.45-(rand()%8+1))*abs(0.78*sin(2*pi*8*i));
        }
        sq->graph(0)->addData(i,x);
        sq->graph(1)->addData(i,y);
        sq->graph(2)->addData(i,z);
    }
}
void mainface::graph_init_cope()
{
    sq = ui->SQ;
    sq->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush sqBrush(QColor(209,225,233));
    sq->setBackground(sqBrush);
    sq->legend->setVisible(true);
    sq->xAxis->setLabel("speed");//设置ｘ轴
    sq->yAxis->setLabel("峭度因子");//设置ｙ轴
    sq->yAxis->setRange(0,20);
    sq->xAxis->setRange(0,2500);
    // 使上下轴、左右轴范围同步
    connect(sq->xAxis, SIGNAL(rangeChanged(QCPRange)), sq->xAxis2, SLOT(setRange(QCPRange)));
    connect(sq->yAxis, SIGNAL(rangeChanged(QCPRange)), sq->yAxis2, SLOT(setRange(QCPRange)));
    sq->addGraph();//增加一条曲线图层
    sq->addGraph();//增加一条曲线图层
    sq->addGraph();//增加一条曲线图层
    QPen xsqPen(QColor(68,87,102));
    sq->graph(0)->setPen(xsqPen);//设置画笔颜色
    sq->graph(0)->setName(QString("峭度x"));
    QPen ysqPen(QColor(122,150,171));
    sq->graph(1)->setPen(ysqPen);//设置画笔颜色
    sq->graph(1)->setName(QString("峭度y"));
    QPen zsqPen(QColor(82,111,132));
    sq->graph(2)->setPen(zsqPen);//设置画笔颜色
    sq->graph(2)->setName(QString("峭度z"));
 //   copedata();
//
    si = ui->SI;
    si->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush siBruish(QColor(209,225,233));
    si->setBackground(siBruish);
    si->legend->setVisible(true);
    si->xAxis->setLabel("speed");//设置ｘ轴
    si->yAxis->setLabel("脉冲因子");//设置ｙ轴
    si->yAxis->setRange(0,20);
    si->xAxis->setRange(0,2500);
    // 使上下轴、左右轴范围同步
    connect(si->xAxis, SIGNAL(rangeChanged(QCPRange)), si->xAxis2, SLOT(setRange(QCPRange)));
    connect(si->yAxis, SIGNAL(rangeChanged(QCPRange)), si->yAxis2, SLOT(setRange(QCPRange)));
    si->addGraph();//增加一条曲线图层
    si->addGraph();//增加一条曲线图层
    si->addGraph();//增加一条曲线图层
    QPen xsiPen(QColor(68,87,102));
    si->graph(0)->setPen(xsiPen);//设置画笔颜色
    si->graph(0)->setName(QString("脉冲x"));
    QPen ysiPen(QColor(122,150,171));
    si->graph(1)->setPen(ysiPen);//设置画笔颜色
    si->graph(1)->setName(QString("脉冲y"));
    QPen zsiPen(QColor(82,111,132));
    si->graph(2)->setPen(zsiPen);//设置画笔颜色
    si->graph(2)->setName(QString("脉冲z"));
//
    sl = ui->SL;
    sl->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush slBrush(QColor(209,225,233));
    sl->setBackground(slBrush);
    sl->legend->setVisible(true);
    sl->xAxis->setLabel("speed");//设置ｘ轴
    sl->yAxis->setLabel("裕度因子");//设置ｙ轴
    sl->yAxis->setRange(0,20);
    sl->xAxis->setRange(0,2500);
    // 使上下轴、左右轴范围同步
    connect(sl->xAxis, SIGNAL(rangeChanged(QCPRange)), sl->xAxis2, SLOT(setRange(QCPRange)));
    connect(sl->yAxis, SIGNAL(rangeChanged(QCPRange)), sl->yAxis2, SLOT(setRange(QCPRange)));
    sl->addGraph();//增加一条曲线图层
    sl->addGraph();//增加一条曲线图层
    sl->addGraph();//增加一条曲线图层
    QPen xslPen(QColor(68,87,102));
    sl->graph(0)->setPen(xslPen);//设置画笔颜色
    sl->graph(0)->setName(QString("裕度x"));
    QPen yslPen(QColor(122,150,171));
    sl->graph(1)->setPen(yslPen);//设置画笔颜色
    sl->graph(1)->setName(QString("裕度y"));
    QPen zslPen(QColor(82,111,132));
    sl->graph(2)->setPen(zslPen);//设置画笔颜色
    sl->graph(2)->setName(QString("裕度z"));

    sl->replot();
    sq->replot();
    si->replot();
}

void mainface::graph_init()
{
    virgra = ui->virgra;
    virgra->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush virBrush(QColor(209,225,233));
    virgra->setBackground(virBrush);
    virgra->legend->setVisible(true);
    virgra->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    virgra->yAxis->setLabel("VIR Channel");//设置ｙ轴
    virgra->yAxis->setRange(-30000,30000);
    virgra->xAxis->setRange(0,8);
    // 使上下轴、左右轴范围同步
    connect(virgra->xAxis, SIGNAL(rangeChanged(QCPRange)), virgra->xAxis2, SLOT(setRange(QCPRange)));
    connect(virgra->yAxis, SIGNAL(rangeChanged(QCPRange)), virgra->yAxis2, SLOT(setRange(QCPRange)));
    virgra->addGraph();//增加一条曲线图层
    virgra->addGraph();//增加一条曲线图层
    virgra->addGraph();//增加一条曲线图层
    QPen xPen(QColor(68,87,102));
    virgra->graph(0)->setPen(xPen);//设置画笔颜色
    virgra->graph(0)->setName(QString("vir_x"));
    QPen yPen(QColor(122,150,171));
    virgra->graph(1)->setPen(yPen);//设置画笔颜色
    virgra->graph(1)->setName(QString("vir_y"));
    QPen zPen(QColor(82,111,132));
    virgra->graph(2)->setPen(zPen);//设置画笔颜色
    virgra->graph(2)->setName(QString("vir_z"));
    //
    humgra = ui->humgra;
    humgra->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush humBrush(QColor(242,212,201));
    humgra->setBackground(humBrush);
    humgra->legend->setVisible(true);
    humgra->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    humgra->yAxis->setLabel("hum Channel");//设置ｙ轴
    humgra->yAxis->setRange(32,50);
    humgra->xAxis->setRange(0,15);
    // 使上下轴、左右轴范围同步
    connect(humgra->xAxis, SIGNAL(rangeChanged(QCPRange)), humgra->xAxis2, SLOT(setRange(QCPRange)));
    connect(humgra->yAxis, SIGNAL(rangeChanged(QCPRange)), humgra->yAxis2, SLOT(setRange(QCPRange)));
    humgra->addGraph();//增加一条曲线图层
    QPen humPen1(QColor(155,85,74));
    humgra->graph(0)->setPen(humPen1);//设置画笔颜色
    humgra->graph(0)->setName(QString("humidity_envir"));
    humgra->addGraph();//增加一条曲线图层
    QPen humPen2(QColor(68,87,102));
    humgra->graph(1)->setPen(humPen2);//设置画笔颜色
    humgra->graph(1)->setName(QString("humidity_bearing"));

    //
    temgra = ui->temgra;
    temgra->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush temBrush(QColor(209,219,189));
    temgra->setBackground(temBrush);
    temgra->legend->setVisible(true);
    temgra->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    temgra->yAxis->setLabel("tem Channel");//设置ｙ轴
    temgra->yAxis->setRange(25,100);
    temgra->xAxis->setRange(0,15);
    // 使上下轴、左右轴范围同步
    connect(temgra->xAxis, SIGNAL(rangeChanged(QCPRange)), temgra->xAxis2, SLOT(setRange(QCPRange)));
    connect(temgra->yAxis, SIGNAL(rangeChanged(QCPRange)), temgra->yAxis2, SLOT(setRange(QCPRange)));
    temgra->addGraph();//增加一条曲线图层
    QPen temPen1(QColor(62,96,111));
    temgra->graph(0)->setPen(temPen1);//设置画笔颜色
    temgra->graph(0)->setName(QString("temperature_envir"));
    temgra->addGraph();//增加一条曲线图层
    QPen temPen2(QColor(82,111,132));
    temgra->graph(1)->setPen(temPen2);//设置画笔颜色
    temgra->graph(1)->setName(QString("temperature_bearing"));
    //
    speedgra = ui->speedgra;
    speedgra->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush speedBrush(QColor(139,166,147));
    speedgra->setBackground(speedBrush);
    speedgra->legend->setVisible(true);
    speedgra->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    speedgra->yAxis->setLabel("speed Channel");//设置ｙ轴
    speedgra->yAxis->setRange(0,5000);
    speedgra->xAxis->setRange(0,15);
    // 使上下轴、左右轴范围同步
    connect(speedgra->xAxis, SIGNAL(rangeChanged(QCPRange)), speedgra->xAxis2, SLOT(setRange(QCPRange)));
    connect(speedgra->yAxis, SIGNAL(rangeChanged(QCPRange)), speedgra->yAxis2, SLOT(setRange(QCPRange)));
    speedgra->addGraph();//增加一条曲线图层
    QPen speedPen(QColor(40,71,92));
    speedgra->graph(0)->setPen(speedPen);//设置画笔颜色
    speedgra->graph(0)->setName(QString("speed"));
    //    ui->qcustomplot->axisRect()->setupFullAxesBox();
    //
    virx = ui->vir_x_2;
    virx->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush virxBrush(QColor(177,86,70));
    virx->setBackground(virxBrush);
    virx->legend->setVisible(true);
    virx->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    virx->yAxis->setLabel("virx Channel");//设置ｙ轴
    virx->yAxis->setRange(-20000,20000);
    virx->xAxis->setRange(0,8);
    // 使上下轴、左右轴范围同步
    connect(virx->xAxis, SIGNAL(rangeChanged(QCPRange)), virx->xAxis2, SLOT(setRange(QCPRange)));
    connect(virx->yAxis, SIGNAL(rangeChanged(QCPRange)), virx->yAxis2, SLOT(setRange(QCPRange)));
    virx->addGraph();//增加一条曲线图层
    QPen virxPen(QColor(68,87,102));
    virx->graph(0)->setPen(virxPen);//设置画笔颜色
    virx->graph(0)->setName(QString("vir_x"));
    //
    viry = ui->vir_y;
    viry->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush viryBrush(QColor(242,212,201));
    viry->setBackground(viryBrush);
    viry->legend->setVisible(true);
    viry->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    viry->yAxis->setLabel("viry Channel");//设置ｙ轴
    viry->yAxis->setRange(-20000,20000);
    viry->xAxis->setRange(0,8);
    // 使上下轴、左右轴范围同步
    connect(viry->xAxis, SIGNAL(rangeChanged(QCPRange)), viry->xAxis2, SLOT(setRange(QCPRange)));
    connect(viry->yAxis, SIGNAL(rangeChanged(QCPRange)), viry->yAxis2, SLOT(setRange(QCPRange)));
    viry->addGraph();//增加一条曲线图层
    QPen viryPen(QColor(122,150,171));
    viry->graph(0)->setPen(viryPen);//设置画笔颜色
    viry->graph(0)->setName(QString("vir_y"));
    //
    virz = ui->vir_z;
    virz->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |	QCP::iSelectLegend | QCP::iSelectPlottables);//设置交互方式;
    QBrush virzBrush(QColor(166,119,114));
    virz->setBackground(virzBrush);
    virz->legend->setVisible(true);
    virz->xAxis->setLabel("Time Axis (t/s)");//设置ｘ轴
    virz->yAxis->setLabel("virz Channel");//设置ｙ轴
    virz->yAxis->setRange(-20000,20000);
    virz->xAxis->setRange(0,8);
    // 使上下轴、左右轴范围同步
    connect(virz->xAxis, SIGNAL(rangeChanged(QCPRange)), virz->xAxis2, SLOT(setRange(QCPRange)));
    connect(virz->yAxis, SIGNAL(rangeChanged(QCPRange)), virz->yAxis2, SLOT(setRange(QCPRange)));
    virz->addGraph();//增加一条曲线图层
    QPen virzPen(QColor(2,80,89));
    virz->graph(0)->setPen(virzPen);//设置画笔颜色
    virz->graph(0)->setName(QString("vir_z"));
}

void mainface::on_Process_clicked()
{

    ui->sqltable->currentItem()->text();
}

void mainface::gif_init()
{
    m = new QMovie;
    m->setFileName(":/new/prefix1/image/goto.gif");
    m->setSpeed(100);
    ui->movier->setMovie(m);
    m->start();
}

void mainface::on_Toleftface_2_clicked()
{
    int nCount = ui->dataview_3->count();
    int nIndex = ui->dataview_3->currentIndex();

    // 获取下一个需要显示的页面索引
    --nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex < 0)
        nIndex = nCount-1;

    ui->dataview_3->setCurrentIndex(nIndex);
}


void mainface::on_Torightface_2_clicked()
{
    int nCount = ui->dataview_3->count();
    int nIndex = ui->dataview_3->currentIndex();

    // 获取下一个需要显示的页面索引
    ++nIndex;

    // 当需要显示的页面索引大于等于总页面时，切换至首页
    if (nIndex >= nCount)
        nIndex = 0;

    ui->dataview_3->setCurrentIndex(nIndex);
}


void mainface::on_Refresh_clicked()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("WLZ123qe");
    db.setDatabaseName("userinfo");
    db.open();
    query = QSqlQuery ("",db);
    query.exec("select * from datalists");
    ui->sqltable->setRowCount(query.size());
    ui->sqltable->setColumnCount(query.record().count());
    int w = 0;
    while (query.next())
    {
        for (int j = 0; j < query.record().count(); j++)
        {
        QString str = query.value(j).toString();
        QTableWidgetItem *item = new QTableWidgetItem(str);
        ui->sqltable->setItem(w, j, item);
        }
        w++;
    }
}

void mainface::dia_init()
{
    Thread05 = new QThread();
    connect(Thread05,&QThread::finished,Thread05,&QThread::deleteLater);
    diagnosisthread = new DiagnosisThread();
    diagnosisthread->moveToThread(Thread05);
    connect(this,&mainface::pdia,diagnosisthread,&DiagnosisThread::diap);
    connect(diagnosisthread,&DiagnosisThread::diac,this,&mainface::cdia);
    connect(diagnosisthread,&DiagnosisThread::diacs,this,&mainface::csdia);
    Thread05->start();
}

void mainface::cdia(int a)
{
    switch (a) {
    case 1:
        ui->diac->setText("低");
        break;
    case 2:
        ui->diac->setText("中");
        break;
    case 3:
        ui->diac->setText("高");
        break;
    default:
        break;
    }
}

void mainface::csdia(int b)
{
    switch (b) {
    case 1:
        ui->diacs->setText("整体正常");
        break;
    case 2:
        ui->diacs->setText("轻微磨损");
        break;
    case 3:
        ui->diacs->setText("局部冲击");
        break;
    case 4:
        ui->diacs->setText("轴承故障");
        break;
    case 5:
        ui->diacs->setText("轴承偏心");
        break;
    default:
        break;
    }
}
