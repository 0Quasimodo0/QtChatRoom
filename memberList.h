#ifndef MEMBERLIST_H_
#define MEMBERLIST_H_
#include "cus-listWidget.h"

class memberList: public customListWidget
{
private:

protected:

public:
    memberList();
    ~memberList();

    bool addItem(QListWidget *listWidget, QString ipAddress="", QString username="");
    bool deleteItem(QListWidget *listWidget, QString ipAddress);
};

#endif
