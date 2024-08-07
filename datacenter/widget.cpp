#include "widget.h"
#include "ui_widget.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    initface();
    sqlinit();
}

void Widget::initface()
{
    setWindowFlags(Qt::FramelessWindowHint);    //隐藏标题栏（无边框）
    setAttribute(Qt::WA_StyledBackground);      //启用样式背景绘制
    setAttribute(Qt::WA_TranslucentBackground); //背景透明
    setAttribute(Qt::WA_Hover);
    mainface *face = new mainface();
    connect(this,&Widget::userid, face,&mainface::iduser);
    connect(this,&Widget::userid, face,&mainface::show);
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();       // 获得部件当前位置
    this->mousePos = event->globalPos(); // 获得鼠标位置
    this->dPos = mousePos - windowPos;   // 移动后部件所在的位置
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_minimized_clicked()
{
    showMinimized();
}

void Widget::on_close_clicked()
{
    close();
}

void Widget::on_login_clicked()
{
    // ui->errortip->setText("");
    // ui->errortip_2->setText("");
    // query = QSqlQuery ("",db);
    // QString acc = ui->account->text();
    // if(acc=="")
    // {
    //     ui->errortip->setText("请输入账号");
    //     return;
    // }
    // QString pass = ui->password->text();
    // QString str1 = QString("select id,account,passwords,isroot from information where account='"+acc+"'");
    // query.exec(str1);
    // if(query.last()==false)
    // {
    //     ui->errortip->setText("账号不存在");
    // }
    // else
    // {
    //     qDebug()<<query.last();
    //     query.last();
    //     if(query.value("passwords")==pass)
    //     {
            emit userid(query.value("id").toInt());
            // db.close();
            this->close();
    //     }
    //     else
    //     {
    //         ui->errortip_2->setText("密码错误");
    //     }
    // }
//    emit userid(1);
//    this->close();
}

void Widget::sqlinit()
{
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPort(3306);
    db.setPassword("WLZ123qe");
    db.setDatabaseName("userinfo");
    db.open();
    if(db.open())
    {
        qDebug() <<"连接成功";
    }
    else
    {
        qDebug() <<"数据库连接失败";
        this->close();
    }
}
