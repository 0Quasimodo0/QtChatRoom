#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUdpSocket>
#include <QTcpServer>
#include <QFile>
#include <QTime>
#include <QMouseEvent>
#include <QTextCharFormat>

#include <QListWidgetItem>
#include "customChatMessage.h"

namespace Ui {
class Widget;
}

//定义数据报类型
enum MessageType{Message, NewParticipant, ParticipantLeft, SendFile, Accept, Refuse};

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    /***************************************************************************************************************************/
    void dealMessage(QNChatMessage *messageW, QListWidgetItem *item, QString text, QString time, QNChatMessage::User_Type type);
    void dealMessageTime(QString curMsgTime);

protected:
    //设置友元类
    friend class customListWidgetItem;
    friend class login;

    //UDP通信
    void sendDatagrams(MessageType type);
    void newParticipant(QString ipAddress, QString username);
    void participantLeft(QString ipAddress, QString username);

    //TCP通信
    void tcpclient_sendFile();
    void iniConnectModule();

    //保存聊天记录
    bool saveFile(const QString& fileName);

    //鼠标事件
    void setAreaMovable(const QRect rt);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

    //关闭事件
    void closeEvent(QCloseEvent *event);

    /***************************************/
    //void resizeEvent(QResizeEvent *event);

private:
    /**************************************************************/
    //界面
    Ui::Widget *ui;

    QString userName;   //用户名
    QColor color;   //字体颜色

    /**************************************************************/
    //通信模块
    QUdpSocket *udpSocket;//UDP通讯的Socket
    QTcpSocket *tcpSocket;//TCP服务器端Socket
    QTcpSocket *tcpClient;//TCP客户端Socket
    QTcpServer *tcpServer;//TCP服务器

    quint16 udp_bindPort;
    quint16 udp_targetPort;
    quint16 tcp_listenPort;
    quint16 tcp_connectPort;
    QString localIP;
    QString serverIP;
    QString clientIP;

    QTime time;

    QString filename;
    QFile *localFile;

    qint64 totalBytes;      //文件总字节数
    qint64 bytesWritten;    //已发送的字节数
    qint64 bytestoWrite;    //尚未发送的字节数
    qint64 filenameSize;    //文件名字的字节数
    qint64 bytesReceived;   //接收的字节数

    qint64 perDataSize; //每次发送数据大小

    QByteArray inBlock;     //数据缓冲区
    QByteArray outBlock;    //数据缓冲区

    /****************************************************************/
    //鼠标事件
    QRect m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
    bool m_bPressed;//鼠标按下标志（不分左右键）
    QPoint m_ptPress;//鼠标按下的初始位置

    /****************************************************************/

private slots:
    //UDP通信
    void readDatagrams();//读取socket传入的数据

    //TCP服务端
    void tcpserver_NewConnection();//QTcpServer的newConnection()信号
    void tcpserver_ClientConnected(); //Client Socket connected
    void tcpserver_ClientDisconnected();//Client Socket disconnected
    void tcpserver_receiveFile();//读取socket传入的数据

    //TCP客户端
    void tcpclient_Connected();
    void tcpclient_Disconnected();
    void tcpclient_updateProgress(qint64 numBytes);

    //功能按键
    void currentFormatChanged(const QTextCharFormat &format);
    void on_fontComboBox_currentFontChanged(QFont f);
    void on_sizeComboBox_currentIndexChanged(QString );
    void on_boldToolBtn_clicked(bool checked);
    void on_italicToolBtn_clicked(bool checked);
    void on_underlineToolBtn_clicked(bool checked);
    void on_colorToolBtn_clicked();
    void on_saveToolBtn_clicked();
    void on_clearToolBtn_clicked();
    void on_sendButton_clicked();
    void on_fileSendButton_clicked();

    //设置按键
    void on_udp_bindPortBtn_clicked();
    void on_udp_abortPortBtn_clicked();
    void on_tcp_listenBtn_clicked();
    void on_tcp_disListenBtn_clicked();
    void on_tcp_connectBtn_clicked();
    void on_tcp_disConnectBtn_clicked();
    void on_udp_changeTargetPortBtn_clicked();
    void on_udp_targetPortBtn_clicked();
    void on_savesettingBtn_clicked();
    void on_resetSettingBtn_clicked();
    void on_cleanLocalIPBtn_clicked();
    void on_cleanUserNameBtn_clicked();

    //导航键
    void on_chatPageButton_clicked();
    void on_updownPageButton_clicked();
    void on_settingPageButton_clicked();
    void on_backPageButton_1_clicked();
    void on_backPageButton_2_clicked();
    void on_backPageButton_3_clicked();
    void on_settingPage_1Button_clicked();
    void on_settingPage_2Button_clicked();

    //窗口控制按键
    void on_closeButton_clicked();
    void on_minButton_clicked();
};

#endif // WIDGET_H
