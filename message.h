#ifndef MESSAGE_H_
#define MESSAGE_H_
#include "cus-listWidget.h"

class customMessage: public customListWidget
{
private:
    //气泡聊天
    QRect m_iconLeftRect;
    QRect m_iconRightRect;
    QRect m_triangleLeftRect;
    QRect m_triangleRightRect;
    QRect m_kuangLeftRect;
    QRect m_kuangRightRect;
    QRect m_textLeftRect;
    QRect m_textRightRect;
    QPixmap m_leftPixmap;
    QPixmap m_rightPixmap;

protected:
    //QSize fontRect(QString msg);
    //void customMessageItem(messageType type);
    //void setText(QString text, QString time, QSize allSize, messageType type);
    QWidget *customMessageItem(const QString message, const QString userName, messageType type);

public:
    customMessage();
    ~customMessage();

    enum messageType{
        System,//系统
        Me,    //自己
        Other,   //用户
        Time,  //时间
        NewParticipant, //新用户
        ParticipantLeft //用户离开
    };

    void addMessage(const QString msg, messageType type);
};

#endif
