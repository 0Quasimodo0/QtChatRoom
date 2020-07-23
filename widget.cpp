#include <QtNetwork>
#include <QTcpSocket>
#include <QTcpServer>
#include <QHostAddress>
#include <QHostInfo>

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QColorDialog>
#include <QDateTime>
#include <QProcess>

#include "widget.h"
#include "ui_widget.h"
#include "customItem.h"

//构造函数
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    //界面初始化
    ui->setupUi(this);

    //Widget窗体
    setWindowFlags(Qt::FramelessWindowHint);//无边框
    ui->stackedWidget->setCurrentIndex(0);//设置默认页面

    //成员列表模块
    ui->listWidget->setFocusPolicy(Qt::NoFocus);       // 去除item选中时的虚线边框
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   //水平滚动条关闭

    //文件传输模块
    ui->updownProgressBar->hide();
    ui->statusLabel->setText(tr("无文件"));
    ui->listWidget_updown->setFocusPolicy(Qt::NoFocus); //去除item选中时的虚线边框
    ui->listWidget_updown->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);    //水平滚动条关闭

    //连接设置模块
    ui->localIPLineEdit->setText(localIP);
    ui->userNameLineEdit->setText(userName);
    ui->udp_bindPortSpinBox->setValue(udp_bindPort);
    ui->udp_targetPortSpinBox->setValue(udp_targetPort);
    ui->tcp_ListenPortSpinBox->setValue(tcp_listenPort);
    ui->tcp_connectPortSpinBox->setValue(tcp_connectPort);
    ui->localIPLineEdit->setEnabled(false);
    ui->userNameLineEdit->setEnabled(false);
    ui->udp_bindPortSpinBox->setEnabled(false);
    ui->udp_bindPortBtn->setEnabled(false);
    ui->udp_targetPortSpinBox->setEnabled(false);
    ui->udp_targetPortBtn->setEnabled(false);
    ui->tcp_ListenPortSpinBox->setEnabled(false);
    ui->tcp_listenBtn->setEnabled(false);
    ui->tcp_connectPortSpinBox->setEnabled(false);
    ui->tcp_connectBtn->setEnabled(false);

    //鼠标事件
    m_areaMovable = geometry();
    m_bPressed = false;

    //信号槽连接
    connect(ui->messageTextEdit, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
                this, SLOT(currentFormatChanged(const QTextCharFormat)));
}

//通信模块初始化
void Widget::iniConnectModule()
{
    /*********************************************UDP通信模块******************************************************/
    //初始化
    udpSocket=new QUdpSocket(this);

    //绑定端口
    if(udpSocket->bind(udp_bindPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)){
        ui->connectStatusBrowser->append(tr("**UDP端口绑定成功"));
        connect(udpSocket,SIGNAL(readyRead()), this,SLOT(readDatagrams()));
        sendDatagrams(NewParticipant);
    }

    /*********************************************TCP通信模块****************************************************/
    //初始化
    tcpServer=new QTcpServer(this);

    //开始监听
    tcpServer->listen(QHostAddress::Any, tcp_listenPort);//
    ui->connectStatusBrowser->append(tr("**开始监听..."));
    ui->connectStatusBrowser->append(tr("**监听状态：正在监听"));

    //信号槽连接
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(tcpserver_NewConnection()));

    /**********************************************数据初始化*********************************************/
    //文件传输数据
    perDataSize = 64*1024;
    totalBytes = 0;
    bytestoWrite = 0;
    bytesWritten = 0;
    bytesReceived = 0;
    filenameSize = 0;

    /****************************************************************************************************/
}

//析构函数
Widget::~Widget()
{
    udpSocket->abort();
    delete udpSocket;
    delete ui;
}

/*UDP通信*****************************************/

// 使用UDP广播发送信息
void Widget::sendDatagrams(MessageType type)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString hostIP = localIP;

    if(type==Message){
        QString message = ui->messageTextEdit->toPlainText();
        ui->messageTextEdit->clear();
        ui->messageTextEdit->setFocus();
        ui->messageBrowser->append(message);
        out << type << hostIP << userName <<message;
    }

    if(type==NewParticipant){
        out << type << hostIP << userName;
    }

    if(type==ParticipantLeft){
        out << type << hostIP << userName;
    }

    if(type==SendFile){
        QString FileName = filename.right(filename.size() - filename.lastIndexOf('/')-1);
        out << type << hostIP << serverIP << FileName;
    }

    if(type==Accept){
        out << type << hostIP << clientIP;
    }

    if(type==Refuse){
        out << type << hostIP << clientIP;
    }

    udpSocket->writeDatagram(data, QHostAddress::Broadcast, udp_targetPort);
}

//读取收到的数据报
void Widget::readDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QDataStream in(&datagram, QIODevice::ReadOnly);
        int messageType;
        QString sendIP;
        in >> messageType >> sendIP;

        if(messageType==Message){
            QString username;
            in >> username;
            QString message;
            in>>message;
            QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            ui->messageBrowser->setTextColor(Qt::black);
            ui->messageBrowser->setCurrentFont(QFont("幼圆",12));
            ui->messageBrowser->append(username + "["+sendIP+"] "+time);
            ui->messageBrowser->append(message);
        }

        if(messageType==NewParticipant){
            QString username;
            in >> username;
            newParticipant(sendIP, username);
        }

        if(messageType==ParticipantLeft){
            QString username;
            in >> username;
            participantLeft(sendIP, username);
        }

        if(messageType==SendFile){
            QString ipAddress;
            in >> ipAddress;
            QString FileName;
            in >> FileName;

            clientIP = sendIP;

            if(ipAddress==localIP){
                int choose = QMessageBox::information(this,tr("接受文件"), tr("来自%1的文件%2是否接收？").arg(sendIP).arg(FileName),
                                                      QMessageBox::Yes,QMessageBox::No);

                if (choose == QMessageBox::Yes){
                    filename = QFileDialog::getSaveFileName(0,tr("保存文件"),FileName);
                    if(!filename.isEmpty()){
                        ui->messageBrowser->append(tr("文件保存成功！"));
                        sendDatagrams(Accept);
                    }
                }
                else
                    sendDatagrams(Refuse);
            }
        }

        if(messageType==Accept){
            QString ipAddress;
            in >> ipAddress;

            if(ipAddress==localIP){
                //初始化
                tcpClient=new QTcpSocket(this); //创建socket变量                
                connect(tcpClient,SIGNAL(connected()),this,SLOT(tcpclient_Connected()));
                connect(tcpClient,SIGNAL(disconnected()),this,SLOT(tcpclient_Disconnected()));

                //建立连接
                tcpClient->connectToHost(serverIP, tcp_connectPort);

                //发送文件
                tcpclient_sendFile();
            }
        }

        if(messageType==Refuse){
            QString ipAddress;
            in >> ipAddress;

            if(ipAddress==localIP){

            }
        }
    }
}

//新用户登录
void Widget::newParticipant(QString ipAddress, QString username)
{
    customListWidgetItem *customItem = new customListWidgetItem;
    if(customItem->addItem(this, MemberList, ipAddress, 0, username)){
        ui->messageBrowser->append(tr("[%1]在线！").arg(ipAddress));

        sendDatagrams(NewParticipant);
    }

}

//用户离开
void Widget::participantLeft(QString ipAddress, QString username)
{
    customListWidgetItem *customItem = new customListWidgetItem;
    if(customItem->deleteItem(this, MemberList, ipAddress)){
        ui->messageBrowser->append(username + "下线！");
    }

}
/******************************************TCP通信--服务器端文件接收*********************************************************/
//建立新连接
void Widget::tcpserver_NewConnection()
{    
    ui->connectStatusBrowser->append(tr("tcpserver连接成功"));
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(tcpSocket, SIGNAL(connected()), this, SLOT(tcpserver_ClientConnected()));
    tcpserver_ClientConnected();//
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(tcpserver_ClientDisconnected()));
    /*connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    onSocketStateChange(tcpSocket->state());*/
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(tcpserver_receiveFile()));
}

//接收文件
void Widget::tcpserver_receiveFile()
{
    qApp->processEvents();
    ui->stackedWidget->setCurrentIndex(1);
    ui->updownProgressBar->show();
    ui->statusLabel->setText(tr("正在接收："));

    QDataStream inFile(tcpSocket);
    inFile.setVersion(QDataStream::Qt_5_9);

    //如果接收到的数据小于16个字节，保存到来的文件头结构
    if(bytesReceived <= sizeof(qint64)*2)
    {
        if((tcpSocket->bytesAvailable()>=(sizeof(qint64))*2) && (filenameSize==0))
        {
            inFile>>totalBytes>>filenameSize;
            bytesReceived += sizeof(qint64)*2;
        }
        if((tcpSocket->bytesAvailable()>=filenameSize) && (filenameSize != 0))
        {
            inFile>>filename;
            bytesReceived += filenameSize;          
            localFile = new QFile(filename);
            if(!localFile->open(QFile::WriteOnly))
            {
                QMessageBox::warning(this,tr("应用程序"),tr("无法读取文件 %1:\n%2.")
                                     .arg(filename).arg(localFile->errorString()));
                ui->updownStatusBrowser->append(tr("**文件\"%1\"读取失败！").arg(filename));
                return;
            }
        }
        else
            return;
    }

    customListWidgetItem *customItem = new customListWidgetItem;
    QString currentFile = filename.right(filename.size()- filename.lastIndexOf('/')-1);
    if(customItem->addItem(this, DownloadList, currentFile, totalBytes)){
        ui->updownStatusBrowser->append(tr("**正在接收文件：%1...").arg(currentFile));
        time.start();
    }

    //如果接收的数据小于总数据，则写入文件
    if(bytesReceived < totalBytes)
    {
        bytesReceived += tcpSocket->bytesAvailable();
        inBlock = tcpSocket->readAll();
        localFile->write(inBlock);
        inBlock.resize(0);
    }

    //更新进度条显示
    ui->updownProgressBar->setMaximum(totalBytes);
    ui->updownProgressBar->setValue(bytesReceived);

    float useTime = time.elapsed();
    double speed = bytesReceived / useTime;
    ui->speedLabel->setText(tr("%1MB/s  已用时%2s, 还剩%3s").arg(speed*1000/(1024*1024),0,'f',2)
                            .arg(useTime/1000,0,'f',0)
                            .arg(totalBytes/speed/1000 - useTime/1000,0,'f',0));

    //数据接收完成时
    if(bytesReceived == totalBytes)
    {
        if(customItem->deleteItem(this, DownloadList, currentFile)){
            bytesReceived = 0;
            totalBytes = 0;
            filenameSize = 0;
            ui->statusLabel->setText(tr("无文件"));
            ui->updownStatusBrowser->append(tr("**文件：%1 已接收完成，共用时%2秒").arg(currentFile).arg(useTime/1000,0,'f',0));
            ui->speedLabel->clear();
            ui->updownProgressBar->reset();
            ui->updownProgressBar->hide();
            localFile->close();
        }
    }
}

//客户端接入时
void Widget::tcpserver_ClientConnected()
{
    ui->updownStatusBrowser->append(tr("**已与[%1]建立连接").arg(clientIP));
}

//客户端断开连接时
void Widget::tcpserver_ClientDisconnected()
{
    ui->updownStatusBrowser->append(tr("**已与[%1]断开连接").arg(clientIP));
}

/**************************************TCP通信--客户端文件发送******************************************************/
//发送文件
void Widget::tcpclient_sendFile()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->updownProgressBar->show();    
    ui->statusLabel->setText(tr("正在传输："));
    connect(tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(tcpclient_updateProgress(qint64)));

    localFile = new QFile(filename);
    if(!localFile->open((QFile::ReadOnly))){
        QMessageBox::warning(this, tr("应用程序"), tr("无法打开文件 %1:\n%2")
                             .arg(filename).arg(localFile->errorString()));
        ui->updownStatusBrowser->append(tr("**文件\"%1\"打开失败！").arg(filename));
        return;
    }

    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock, QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_9);

    customListWidgetItem *customItem = new customListWidgetItem;
    QString currentFile = filename.right(filename.size()- filename.lastIndexOf('/')-1);
    if(customItem->addItem(this, UploadList, currentFile, totalBytes)){
        ui->updownStatusBrowser->append(tr("**正在发送文件：%1...").arg(currentFile));
        // 开始计时
        time.start();

        sendOut << qint64(0) << qint64(0) << currentFile;
        totalBytes += outBlock.size();
        sendOut.device()->seek(0);
        sendOut << totalBytes << qint64((outBlock.size() - sizeof(qint64)*2));
        bytestoWrite = totalBytes - tcpClient->write(outBlock);
        outBlock.resize(0);
    }
}

//数据更新
void Widget::tcpclient_updateProgress(qint64 numBytes)
{
    qApp->processEvents();
    bytesWritten += (int)numBytes;
    if (bytestoWrite > 0) {
        outBlock = localFile->read(qMin(bytestoWrite, perDataSize));
        bytestoWrite -= (int)tcpClient->write(outBlock);
        outBlock.resize(0);
    } else {
        localFile->close();
    }

    ui->updownProgressBar->setMaximum(totalBytes);
    ui->updownProgressBar->setValue(bytesWritten);

    float useTime = time.elapsed();
    double speed = bytesWritten / useTime;

    ui->speedLabel->setText(tr("%1MB/s  已用时%2s, 还剩%3s").arg(speed*1000/(1024*1024),0,'f',2)
                            .arg(useTime/1000,0,'f',0)
                            .arg(totalBytes/speed/1000 - useTime/1000,0,'f',0));

    if(bytesWritten == totalBytes) {
        customListWidgetItem *customItem = new customListWidgetItem;
        QString currentFileName = filename.right(filename.size()- filename.lastIndexOf('/')-1);
        if(customItem->deleteItem(this, UploadList, currentFileName)){
            ui->statusLabel->setText(tr("无文件"));
            ui->updownStatusBrowser->append(tr("** 文件：%1 已发送完成，共用时%2秒").arg(currentFileName).arg(useTime/1000,0,'f',0));
            ui->speedLabel->clear();
            ui->updownProgressBar->reset();
            ui->updownProgressBar->hide();
            bytesWritten = 0;
            bytestoWrite = 0;
            totalBytes = 0;
            filenameSize = 0;
            localFile->close();
        }
    }
}

//连接服务器
void Widget::tcpclient_Connected()
{
    ui->updownStatusBrowser->append(tr("**已与[%1]建立连接").arg(serverIP));
}

//与服务器断开
void Widget::tcpclient_Disconnected()
{
    ui->updownStatusBrowser->append(tr("**已与[%1]断开连接").arg(serverIP));
}

/********************************************功能按钮*******************************************************/
//发送消息
void Widget::on_sendButton_clicked()
{
    sendDatagrams(Message);

    /****************************************************************/
    QString msg = ui->messageTextEdit->toPlainText();
    ui->messageTextEdit->setText("");
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳

    bool isSending = true; // 发送中

    qDebug()<<"addMessage" << msg << time << ui->messageListWidget->count();
    if(ui->messageListWidget->count()%2) {
        if(isSending) {
            dealMessageTime(time);

            QNChatMessage* messageW = new QNChatMessage(ui->messageListWidget->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->messageListWidget);
            dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);
        } else {
            bool isOver = true;
            for(int i = ui->messageListWidget->count() - 1; i > 0; i--) {
                QNChatMessage* messageW = (QNChatMessage*)ui->messageListWidget->itemWidget(ui->messageListWidget->item(i));
                if(messageW->text() == msg) {
                    isOver = false;
                    messageW->setTextSuccess();
                }
            }
            if(isOver) {
                dealMessageTime(time);

                QNChatMessage* messageW = new QNChatMessage(ui->messageListWidget->parentWidget());
                QListWidgetItem* item = new QListWidgetItem(ui->messageListWidget);
                dealMessage(messageW, item, msg, time, QNChatMessage::User_Me);
                messageW->setTextSuccess();
            }
        }
    } else {
        if(msg != "") {
            dealMessageTime(time);

            QNChatMessage* messageW = new QNChatMessage(ui->messageListWidget->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->messageListWidget);
            dealMessage(messageW, item, msg, time, QNChatMessage::User_She);
        }
    }
    ui->messageListWidget->setCurrentRow(ui->messageListWidget->count()-1);
}

//传输文件
void Widget::on_fileSendButton_clicked()
{
    if(ui->listWidget->selectedItems().isEmpty()){
        QMessageBox::warning(0, tr("选择用户"),tr("请先从用户列表选择要传送的用户！"), QMessageBox::Ok);
        return;
    }
    else{        
        int row = ui->listWidget->currentRow();
        QListWidgetItem *currentItem=ui->listWidget->item(row);
        QWidget *currentWidget = ui->listWidget->itemWidget(currentItem);
        QLabel * ipLabel = currentWidget->findChild<QLabel *>("ip");
        serverIP = ipLabel->text();

        filename = QFileDialog::getOpenFileName(this);
        if(!filename.isEmpty()){
            sendDatagrams(SendFile);
        }
    }
}

// 更改字体族
void Widget::on_fontComboBox_currentFontChanged(QFont f)
{
    ui->messageTextEdit->setCurrentFont(f);
    ui->messageTextEdit->setFocus();
}


// 更改字体大小
void Widget::on_sizeComboBox_currentIndexChanged(QString size)
{
    ui->messageTextEdit->setFontPointSize(size.toDouble());
    ui->messageTextEdit->setFocus();
}

// 加粗
void Widget::on_boldToolBtn_clicked(bool checked)
{
    if(checked)
        ui->messageTextEdit->setFontWeight(QFont::Bold);
    else
        ui->messageTextEdit->setFontWeight(QFont::Normal);
    ui->messageTextEdit->setFocus();
}

// 倾斜
void Widget::on_italicToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontItalic(checked);
    ui->messageTextEdit->setFocus();
}

// 下划线
void Widget::on_underlineToolBtn_clicked(bool checked)
{
    ui->messageTextEdit->setFontUnderline(checked);
    ui->messageTextEdit->setFocus();
}

// 颜色
void Widget::on_colorToolBtn_clicked()
{
    color = QColorDialog::getColor(color, this);
    if (color.isValid()) {
        ui->messageTextEdit->setTextColor(color);
        ui->messageTextEdit->setFocus();
    }
}

//字体格式
void Widget::currentFormatChanged(const QTextCharFormat &format)
{
    ui->fontComboBox->setCurrentFont(format.font());

    // 如果字体大小出错(因为我们最小的字体为9)，使用12
    if (format.fontPointSize() < 9) {
        ui->sizeComboBox->setCurrentIndex(3);
    } else {
        ui->sizeComboBox->setCurrentIndex( ui->sizeComboBox
                                          ->findText(QString::number(format.fontPointSize())));
    }
    ui->boldToolBtn->setChecked(format.font().bold());
    ui->italicToolBtn->setChecked(format.font().italic());
    ui->underlineToolBtn->setChecked(format.font().underline());
    color = format.foreground().color();
}


// 保存聊天记录
void Widget::on_saveToolBtn_clicked()
{
    if (ui->messageBrowser->document()->isEmpty()) {
        QMessageBox::warning(0, tr("警告"), tr("聊天记录为空，无法保存！"), QMessageBox::Ok);
    }
    else {
        QString recordFileName = QFileDialog::getSaveFileName(this,tr("保存聊天记录"), tr("聊天记录"), tr("文本(*.txt);;All File(*.*)"));

        if(!recordFileName.isEmpty())
            saveFile(recordFileName);
    }
}

// 保存聊天记录
bool Widget::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("保存文件"),tr("无法保存文件 %1:\n %2").arg(fileName).arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out << ui->messageBrowser->toPlainText();

    return true;
}

// 清空聊天记录
void Widget::on_clearToolBtn_clicked()
{
    ui->messageBrowser->clear();
}

/***********************************************窗口控制*************************************************/
//窗口关闭事件
void Widget::closeEvent(QCloseEvent *event)
{
    //发送用户离开消息
    sendDatagrams(ParticipantLeft);

    QWidget::closeEvent(event);
}

//鼠标左键
void Widget::mousePressEvent(QMouseEvent *e)
{
  if(e->button() == Qt::LeftButton)
  {
    m_ptPress = e->pos();
    qDebug() << pos() << e->pos() << m_ptPress;
    m_bPressed = m_areaMovable.contains(m_ptPress);
  }
}

void Widget::mouseMoveEvent(QMouseEvent *e)
{
  if(m_bPressed)
  {
    qDebug() << pos() << e->pos() << m_ptPress;
    move(pos() + e->pos() - m_ptPress);
  }
}

void Widget::mouseReleaseEvent(QMouseEvent *)
{
    m_bPressed = false;
}

//设置鼠标按下的区域
void Widget::setAreaMovable(const QRect rt)
{
  if(m_areaMovable != rt)
  {
    m_areaMovable = rt;
  }
}

void Widget::on_closeButton_clicked()
{
    close();
}

void Widget::on_minButton_clicked()
{
    showMinimized();
}

/******************************************设置模块***********************************************/

void Widget::on_udp_bindPortBtn_clicked()
{
    udp_bindPort = ui->udp_bindPortSpinBox->value(); //本机UDP端口
    if (udpSocket->bind(udp_bindPort))//绑定端口成功
    {
        ui->connectStatusBrowser->append("**已成功绑定端口："
               +QString::number(udpSocket->localPort()));

        ui->udp_bindPortBtn->setEnabled(false);
        ui->udp_abortPortBtn->setEnabled(true);
        ui->udp_bindPortSpinBox->setEnabled(false);
    }
    else
        ui->connectStatusBrowser->append("**绑定端口失败");
}

void Widget::on_udp_abortPortBtn_clicked()
{
    udpSocket->abort(); //不能解除绑定
    ui->udp_bindPortBtn->setEnabled(true);
    ui->udp_abortPortBtn->setEnabled(false);
    ui->udp_bindPortSpinBox->setEnabled(true);
    ui->connectStatusBrowser->append("**已解除绑定");
}

void Widget::on_tcp_listenBtn_clicked()
{
    tcp_listenPort = ui->tcp_ListenPortSpinBox->value();
    QHostAddress serverip(localIP);
    tcpServer->listen(serverip, tcp_listenPort);
    ui->connectStatusBrowser->append("**正在监听...");
    ui->tcp_listenBtn->setEnabled(false);
    ui->tcp_disListenBtn->setEnabled(true);
    ui->tcp_ListenPortSpinBox->setEnabled(false);
}

void Widget::on_tcp_disListenBtn_clicked()
{
    if (tcpServer->isListening()) //tcpServer正在监听
    {
        tcpServer->close();//停止监听
        ui->tcp_listenBtn->setEnabled(true);
        ui->tcp_disListenBtn->setEnabled(false);
        ui->tcp_ListenPortSpinBox->setEnabled(true);
        ui->connectStatusBrowser->append("**已停止监听");
    }
}

void Widget::on_tcp_connectBtn_clicked()
{
    tcp_connectPort = ui->tcp_connectPortSpinBox->value();
    ui->connectStatusBrowser->append("**已更改连接端口");
    ui->tcp_disConnectBtn->setEnabled(true);
    ui->tcp_connectBtn->setEnabled(false);
    ui->tcp_connectPortSpinBox->setEnabled(false);
}

void Widget::on_tcp_disConnectBtn_clicked()
{
    ui->tcp_connectBtn->setEnabled(true);
    ui->tcp_disConnectBtn->setEnabled(false);
    ui->tcp_connectPortSpinBox->setEnabled(true);
}

void Widget::on_udp_changeTargetPortBtn_clicked()
{
    ui->udp_targetPortBtn->setEnabled(true);
    ui->udp_changeTargetPortBtn->setEnabled(false);
    ui->udp_targetPortSpinBox->setEnabled(true);
}

void Widget::on_udp_targetPortBtn_clicked()
{
    udp_targetPort = ui->udp_targetPortSpinBox->value();

    ui->connectStatusBrowser->append("**已更改目标端口");
    ui->udp_targetPortSpinBox->setEnabled(false);
    ui->udp_changeTargetPortBtn->setEnabled(true);
    ui->udp_targetPortBtn->setEnabled(false);
}

void Widget::on_savesettingBtn_clicked()
{
    if((localIP != ui->localIPLineEdit->text())||(userName != ui->userNameLineEdit->text())){
        sendDatagrams(ParticipantLeft);
        localIP = ui->localIPLineEdit->text();
        userName = ui->userNameLineEdit->text();
        sendDatagrams(NewParticipant);
    }

    localIP = ui->localIPLineEdit->text();
    userName = ui->userNameLineEdit->text();

    ui->localIPLineEdit->setEnabled(false);
    ui->userNameLineEdit->setEnabled(false);
    ui->connectStatusBrowser->append("**保存成功");
}

void Widget::on_resetSettingBtn_clicked()
{
    ui->localIPLineEdit->setText(localIP);
    ui->userNameLineEdit->setText(userName);

    ui->localIPLineEdit->setEnabled(false);
    ui->userNameLineEdit->setEnabled(false);
    ui->connectStatusBrowser->append("**重置成功");
}

void Widget::on_cleanLocalIPBtn_clicked()
{
    ui->localIPLineEdit->setEnabled(true);
    ui->localIPLineEdit->clear();
}

void Widget::on_cleanUserNameBtn_clicked()
{
    ui->userNameLineEdit->setEnabled(true);
    ui->userNameLineEdit->clear();
}

/*************************************导航键******************************************/

void Widget::on_chatPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Widget::on_updownPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_settingPageButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_backPageButton_1_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_backPageButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_backPageButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_settingPage_1Button_clicked()
{
    ui->settingStackedWidget->setCurrentIndex(0);
}

void Widget::on_settingPage_2Button_clicked()
{
    ui->settingStackedWidget->setCurrentIndex(1);
}

/******************************************************************************/

void Widget::dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time,  QNChatMessage::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->messageListWidget->setItemWidget(item, messageW);
}

void Widget::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->messageListWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->messageListWidget->item(ui->messageListWidget->count() - 1);
        QNChatMessage* messageW = (QNChatMessage*)ui->messageListWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        QNChatMessage* messageTime = new QNChatMessage(ui->messageListWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->messageListWidget);

        /****************************/
        QSize size = QSize(100, 40);

        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, QNChatMessage::User_Time);
        ui->messageListWidget->setItemWidget(itemTime, messageTime);
    }
}

/*void Widget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);


    //ui->messageTextEdit->resize(this->width() - 20, ui->widget->height() - 20);
    //ui->messageTextEdit->move(10, 10);

    //ui->pushButton->move(ui->messageTextEdit->width()+ui->messageTextEdit->x() - ui->pushButton->width() - 10,
                         //ui->messageTextEdit->height()+ui->messageTextEdit->y() - ui->pushButton->height() - 10);


    for(int i = 0; i < ui->messageListWidget->count(); i++) {
        QNChatMessage* messageW = (QNChatMessage*)ui->messageListWidget->itemWidget(ui->messageListWidget->item(i));
        QListWidgetItem* item = ui->messageListWidget->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}*/
