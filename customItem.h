#ifndef CUSTOMTITEM_H_
#define CUSTOMTITEM_H_
#include <QWidget>
#include <QLabel>
#include <QProgressBar>

class Widget;

/****************************自定义QListWidgetItem, 实现自定义Listwidget列表显示***************************************/

enum listWidgetType{MemberList, UploadList, DownloadList};

class customListWidgetItem : public QWidget
{
private:
    const int NOITEM=-1;

protected:
    QWidget *customMemberListItem(QString ipAddress, QString userName);
    QWidget *customUpDownItem(QString fileName, qint64 totalBytes);

    bool searchIsRepeat(Widget *UI, listWidgetType type, QString keyWord);
    int searchItemRow(Widget *UI, listWidgetType type, QString keyWord);
public:
    customListWidgetItem();

    bool addItem(Widget *UI, listWidgetType type, QString keyWord, qint64 totalBytes, QString username = "");
    bool deleteItem(Widget *UI, listWidgetType type, QString keyWord);
};

/****************************************************************************************************************/


/*********************************************************************************************************************/

#endif //CUSTOMITEM_H_
