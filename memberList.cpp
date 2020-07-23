#include "memberList.h"
#include "ui_widget.h"

memberList::memberList()
{

}

bool memberList::addItem(QListWidget *listWidget, QString ipAddress, QString username)
{
    if(ipAddress!="" && username!=""){
        QWidget *widget = customMemberListItem(ipAddress, username);
        QListWidgetItem *item = new QListWidgetItem;
        listWidget->addItem(item);
        item->setSizeHint(QSize(0,60));
        listWidget->setItemWidget(item, widget);

        return true;
    }
    else{
        return false;
    }
}

bool memberList::deleteItem(QListWidget *listWidget, QString ipAddress)
{
    int row = searchItemRow(listWidget, ipAddress, getID_memberItem());

    if(row!=NOITEM) {
        QListWidgetItem *delItem=listWidget->item(row);
        listWidget->removeItemWidget(delItem);
        delItem=listWidget->takeItem(row);
        delete delItem;

        return true;
    }
    else{
        return false;
    }
}

memberList::~memberList()
{

}
