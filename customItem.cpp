#include <QPainter>
#include <QLabel>
#include <QLayout>
#include <QPalette>
#include <QFont>
#include <QToolButton>
#include <QIcon>
#include <QMessageBox>
#include <QScrollBar>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QListWidgetItem>

#include "customItem.h"
#include "widget.h"
#include "ui_widget.h"

/********************************************************************************************************/
customListWidgetItem::customListWidgetItem()
{

}

//添加item
bool customListWidgetItem::addItem(Widget *UI, listWidgetType type, QString keyWord, qint64 totalBytes, QString username)
{
    if(!searchIsRepeat(UI, type, keyWord)) {
        if(type==MemberList) {
            QWidget *widget = customMemberListItem(keyWord, username);
            QListWidgetItem *item = new QListWidgetItem;
            UI->ui->listWidget->addItem(item);
            item->setSizeHint(QSize(0,60));
            UI->ui->listWidget->setItemWidget(item, widget);

            return true;
        }

        if(type==UploadList) {
            QWidget *widget = customUpDownItem(keyWord, totalBytes);
            QListWidgetItem *item = new QListWidgetItem;
            UI->ui->listWidget_updown->addItem(item);
            item->setSizeHint(QSize(0,60));
            UI->ui->listWidget_updown->setItemWidget(item, widget);

            return true;
        }

        if(type==DownloadList) {
            QWidget *widget = customUpDownItem(keyWord, totalBytes);
            QListWidgetItem *item = new QListWidgetItem;
            UI->ui->listWidget_updown->addItem(item);
            item->setSizeHint(QSize(0,60));
            UI->ui->listWidget_updown->setItemWidget(item, widget);

            return true;
        }
    }

    return false;
}

//删除item
bool customListWidgetItem::deleteItem(Widget *UI, listWidgetType type, QString keyWord)
{
    if(type==MemberList) {
        int row = searchItemRow(UI, MemberList, keyWord);

        if(row!=NOITEM) {
            QListWidgetItem *delItem=UI->ui->listWidget->item(row);
            UI->ui->listWidget->removeItemWidget(delItem);
            delItem=UI->ui->listWidget->takeItem(row);
            delete delItem;

            return true;
        }
    }
    if(type==UploadList){
        int row = searchItemRow(UI, UploadList, keyWord);

        if(row!=NOITEM) {
            QListWidgetItem *delItem=UI->ui->listWidget_updown->item(row);
            UI->ui->listWidget_updown->removeItemWidget(delItem);
            delItem=UI->ui->listWidget_updown->takeItem(row);
            delete delItem;

            return true;
        }
    }
    if(type==DownloadList){
        int row = searchItemRow(UI, DownloadList, keyWord);

        if(row!=NOITEM) {
            QListWidgetItem *delItem=UI->ui->listWidget_updown->item(row);
            UI->ui->listWidget_updown->removeItemWidget(delItem);
            delItem=UI->ui->listWidget_updown->takeItem(row);
            delete delItem;

            return true;
        }
    }

    return false;
}

//查找item是否重复
bool customListWidgetItem::searchIsRepeat(Widget *UI, listWidgetType type, QString keyWord)
{
    int Row = searchItemRow(UI, type, keyWord);

    if(Row==NOITEM)
        return false;
    else
        return true;
}

int customListWidgetItem::searchItemRow(Widget *UI, listWidgetType type, QString keyWord)
{
    if(type==MemberList) {
        for(int i=0; i<UI->ui->listWidget->count(); i++) {
            QListWidgetItem *currentItem=UI->ui->listWidget->item(i);
            QWidget *currentWidget = UI->ui->listWidget->itemWidget(currentItem);
            QLabel * idLabel = currentWidget->findChild<QLabel *>("ip");

            QString id=idLabel->text();
            if(id==keyWord)
                return i;           
        }      
    }

    if(type==UploadList) {
        for(int i=0; i<UI->ui->listWidget_updown->count(); i++) {
            QListWidgetItem *currentItem=UI->ui->listWidget_updown->item(i);
            QWidget *currentWidget = UI->ui->listWidget_updown->itemWidget(currentItem);
            QLabel * idLabel = currentWidget->findChild<QLabel *>("FileNameLabel");

            QString id=idLabel->text();
            if(id==keyWord)
                return i;           
        }
    }

    if(type==DownloadList) {
        for(int i=0; i<UI->ui->listWidget_updown->count(); i++) {
            QListWidgetItem *currentItem=UI->ui->listWidget_updown->item(i);
            QWidget *currentWidget = UI->ui->listWidget_updown->itemWidget(currentItem);
            QLabel * idLabel = currentWidget->findChild<QLabel *>("FileNameLabel");

            QString id=idLabel->text();
            if(id==keyWord)
                return i;            
        }
    }

    return NOITEM;
}

//自定义用户列表
QWidget *customListWidgetItem::customMemberListItem(QString ipAddress, QString userName)
{
    QWidget *widget = new QWidget;

    QHBoxLayout *horLayout = new QHBoxLayout();
    QVBoxLayout *verLayout = new QVBoxLayout();

    QPushButton *head = new QPushButton(widget);
    head->setFixedSize(48,48);
    head->setChecked(false);
    head->setStyleSheet("background-color: rgb(255, 255, 255);border-color: rgb(85, 170, 255);border-radius:24px;");
    QIcon head_icon;
    head_icon.addFile(QStringLiteral(":/images/head.png"), QSize(), QIcon::Normal, QIcon::Off);
    head->setIcon(head_icon);
    head->setIconSize(QSize(40, 40));


    QLabel *user=new QLabel(widget);
    user->setObjectName(QStringLiteral("user"));
    QFont ft_user;
    ft_user.setPixelSize(20);
    user->setMinimumHeight(32);
    user->setText(userName);
    user->setFont(ft_user);

    QLabel *ip=new QLabel(widget);
    ip->setObjectName(QStringLiteral("ip"));
    QFont ft_ip;
    ft_ip.setPixelSize(15);
    ip->setMinimumHeight(16);
    ip->setText(ipAddress);
    user->setFont(ft_ip);

    QPalette color;
    color.setColor(QPalette::Text,Qt::gray);
    ip->setPalette(color);

    verLayout->setContentsMargins(0, 0, 0, 0);
    verLayout->setMargin(0);
    verLayout->setSpacing(10);
    verLayout->addWidget(user);
    verLayout->addWidget(ip);

    horLayout->setContentsMargins(0, 0, 0, 0);
    horLayout->setMargin(6);
    horLayout->setSpacing(10);
    horLayout->addWidget(head);
    horLayout->addLayout(verLayout);
    widget->setLayout(horLayout);

    return widget;
}

//自定义传输列表
QWidget *customListWidgetItem::customUpDownItem(QString fileName, qint64 totalBytes)
{
    QWidget *widget=new QWidget;

    //显示区域的布局
    QHBoxLayout *horLayout = new QHBoxLayout();

    //主要控件
    QPushButton *head = new QPushButton(widget);
    QLabel *fileNameLabel = new QLabel(widget);
    QLabel *totalByteLabel = new QLabel(widget);

    //设置不同控件的样式
    head->setFixedSize(30,30);
    head->setChecked(false);
    head->setStyleSheet("background-color: rgb(255, 255, 255);border-color: rgb(85, 170, 255);border-radius:5px;");
    QIcon head_icon;
    head_icon.addFile(QStringLiteral(":/images/file_2.png"), QSize(), QIcon::Normal, QIcon::Off);
    head->setIcon(head_icon);
    head->setIconSize(QSize(30, 30));

    QFont tempFont(QStringLiteral("幼圆"), 10, 0);
    fileNameLabel->setText(fileName);
    fileNameLabel->setFont(tempFont);
    fileNameLabel->setAlignment(Qt::AlignCenter);
    fileNameLabel->setObjectName(QStringLiteral("FileNameLabel"));

    totalByteLabel->setText(tr("共%1MB").arg(totalBytes / (1024*1024)));
    totalByteLabel->setFont(tempFont);
    totalByteLabel->setFixedWidth(120);
    totalByteLabel->setObjectName(QStringLiteral("TotalByteLabel"));
    totalByteLabel->setAlignment(Qt::AlignCenter);

    horLayout->setContentsMargins(0, 0, 0, 0);
    horLayout->setMargin(8);
    horLayout->setSpacing(8);
    horLayout->addWidget(head);
    horLayout->addWidget(fileNameLabel);
    horLayout->addWidget(totalByteLabel);

    widget->setLayout(horLayout);

    return widget;
}

/*****************************************************************************************************************************/


/***************************************************************************************************************************/

