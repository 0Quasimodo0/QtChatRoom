#include "cus-listWidget.h"
#include "ui_widget.h"

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

customListWidget::customListWidget()
{
    m_leftPixmap = QPixmap(":/images/Customer Copy.png");
    m_rightPixmap = QPixmap(":/images/CustomerService.png");
}

bool customListWidget::searchIsRepeat(QListWidget *listWidget, const QString keyWord, const QString id)
{
    int Row = searchItemRow(listWidget, keyWord, id);

    if(Row==NOITEM)
        return false;
    else
        return true;
}

int customListWidget::searchItemRow(QListWidget *listWidget, const QString keyWord, const QString id)
{
    for(int i=0; i<listWidget->count(); i++) {
        QListWidgetItem *currentItem=listWidget->item(i);
        QWidget *currentWidget = listWidget->itemWidget(currentItem);
        QLabel *idLabel = currentWidget->findChild<QLabel *>(id);
        QString id=idLabel->text();

        if(id==keyWord)
            return i;
    }

    return NOITEM;
}

//自定义用户列表
QWidget *customListWidget::customMemberListItem(QString ipAddress, QString userName)
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
    ip->setObjectName(QStringLiteral(label_ip_memberListItem));
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
/*QWidget *customListWidget::customUpDownItem(QString fileName, qint64 totalBytes)
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
}*/
