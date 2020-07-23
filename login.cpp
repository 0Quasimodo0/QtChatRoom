#include "login.h"
#include "widget.h"
#include "ui_login.h"
#include <QHostInfo>
#include <QProcess>

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);    //无边框
    this->setFixedSize(580, 375);
    ui->upBtn->setEnabled(false);
    w =new Widget;

    //鼠标事件
    m_areaMovable = geometry();
    m_bPressed = false;

}

login::~login()
{
    delete ui;
}

void login::setLocalIP(Widget *w)
{
    w->localIP=ui->localIPLineEdit->text();
}

void login::setUserName(Widget *w)
{
    w->userName=ui->userNameLineEdit->text();
}

void login::setUDP_BindPort(Widget *w)
{
    w->udp_bindPort=ui->upd_bindPortSpinBox->value();
}

void login::setUDP_TargetPort(Widget *w)
{
    w->udp_targetPort=ui->upd_targetPortSpinBox->value();
}

void login::setTCP_ListenPort(Widget *w)
{
    w->tcp_listenPort=ui->tcp_listenPortSpinBox->value();
}

void login::setTCP_ConnectPort(Widget *w)
{
    w->tcp_connectPort=ui->tcp_connectPortSpinBox->value();
}
/***********************************************************************/

//鼠标左键
void login::mousePressEvent(QMouseEvent *e)
{
  if(e->button() == Qt::LeftButton)
  {
    m_ptPress = e->pos();
    qDebug() << pos() << e->pos() << m_ptPress;
    m_bPressed = m_areaMovable.contains(m_ptPress);
  }
}

void login::mouseMoveEvent(QMouseEvent *e)
{
  if(m_bPressed)
  {
    qDebug() << pos() << e->pos() << m_ptPress;
    move(pos() + e->pos() - m_ptPress);
  }
}

void login::mouseReleaseEvent(QMouseEvent *)
{
    m_bPressed = false;
}

//设置鼠标按下的区域
void login::setAreaMovable(const QRect rt)
{
  if(m_areaMovable != rt)
  {
    m_areaMovable = rt;
  }
}

/********************************************************************************/
void login::on_loginButton_clicked()
{
    setLocalIP(w);
    setUserName(w);
    setUDP_BindPort(w);
    setUDP_TargetPort(w);
    setTCP_ListenPort(w);
    setTCP_ConnectPort(w);

    w->iniConnectModule();
    w->show();
    this->hide();
}

void login::on_getlocalIPButton_clicked()
{
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();//

    if (!addList.isEmpty())
    for (int i=0;i<addList.count();i++)
    {
        QHostAddress aHost=addList.at(i);
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    }

    ui->localIPLineEdit->setText(localIP);
}


void login::on_getUserNameButton_clicked()
{
    QStringList envVariables;
    envVariables << "USERNAME.*" << "USER.*" << "USERDOMAIN.*"
                 << "HOSTNAME.*" << "DOMAINNAME.*";
    QStringList environment = QProcess::systemEnvironment();
    foreach (QString string, envVariables) {
        int index = environment.indexOf(QRegExp(string));
        if (index != -1) {
            QStringList stringList = environment.at(index).split('=');
            if (stringList.size() == 2) {
                ui->userNameLineEdit->setText(stringList.at(1));
                break;
            }
            else{
                ui->userNameLineEdit->setText("unknown");
            }
        }
    }
}

void login::on_minBtn_clicked()
{
    showMinimized();
}

void login::on_closeBtn_clicked()
{
    close();
}

void login::on_downBtn_clicked()
{
    this->setFixedSize(580,430);
    ui->downBtn->setEnabled(false);
    ui->upBtn->setEnabled(true);
}

void login::on_upBtn_clicked()
{
    this->setFixedSize(580, 375);
    ui->downBtn->setEnabled(true);
    ui->upBtn->setEnabled(false);
}
