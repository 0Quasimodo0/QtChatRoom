#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QMouseEvent>

namespace Ui {
class login;
}
class Widget;

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

protected:
    //连接设置
    void setLocalIP(Widget *w);
    void setUserName(Widget *w);
    void setUDP_BindPort(Widget *w);
    void setUDP_TargetPort(Widget *w);
    void setTCP_ListenPort(Widget *w);
    void setTCP_ConnectPort(Widget *w);

    //鼠标事件
    void setAreaMovable(const QRect rt);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    void on_loginButton_clicked();
    void on_closeBtn_clicked();

    void on_minBtn_clicked();

    void on_getlocalIPButton_clicked();

    void on_getUserNameButton_clicked();

    void on_downBtn_clicked();

    void on_upBtn_clicked();

private:
    Ui::login *ui;
    Widget *w;

    QRect m_areaMovable;//可移动窗口的区域，鼠标只有在该区域按下才能移动窗口
    bool m_bPressed;//鼠标按下标志（不分左右键）
    QPoint m_ptPress;//鼠标按下的初始位置

};

#endif // LOGIN_H
