#ifndef CUS-LISTWIDGET_H_
#define CUS-LISTWIDGET_H_

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>

class customListWidget: public QWidget
{
private:
    const int NOITEM=-1;

    const QString label_ip_memberListItem = "ip";



protected:
    QWidget *customMemberListItem(const QString ipAddress, const QString userName);   

    //QWidget *customUpDownItem(const QString fileName, const qint64 totalBytes);

    bool searchIsRepeat(QListWidget *listWidget, const QString keyWord, const QString id);
    int searchItemRow(QListWidget *listWidget, const QString keyWord, const QString id);

    inline QString getID_memberItem() const{return label_ip_memberListItem;}

public:
    /*enum listWidgetType{
        MemberList,     //成员列表
        UploadList,     //上传文件
        DownloadList    //下载文件
    };*/

    //bool addItem(Widget *UI, const listWidgetType type, const QString keyWord, const qint64 totalBytes, QString username = "");
    //bool deleteItem(Widget *UI, const listWidgetType type, const QString keyWord);

    //virtual bool addItem(Widget *UI, const QString keyWord, const qint64 totalBytes, QString username = "");
    //virtual bool deleteItem(Widget *UI, const QString keyWord);

    customListWidget();
    virtual ~customListWidget(){}
};

#endif
