#include "trayicon.h"
#include "qmutex.h"
#include "qmenu.h"
#include "qapplication.h"

// 对静态成员变量self进行定义与初始化
// QScopedPointer为智能指针，能自动管理内存，避免内存泄漏
QScopedPointer<TrayIcon> TrayIcon::self;
// 定义懒汉单例，确保只能有一个托盘
TrayIcon* TrayIcon::Instance()
{
    if(self.isNull())
    {
        static QMutex mutex;
        QMutexLocker locker(&mutex);
        // 双重检查锁定，保证线程安全
        if(self.isNull())
        {
            self.reset(new TrayIcon);
        }
    }
    return self.data();
}

TrayIcon::TrayIcon(QObject *parent) : QObject(parent)
{
    mainWidget = 0;
    // 实例化托盘对象
    trayIcon = new QSystemTrayIcon(this);
    // 设置托盘图标
    trayIcon->setIcon(QIcon(":/img/resources/controller.png"));
    // 绑定托盘被激活
    // activated信号在托盘图标被用户操作时会触发，QSystemTrayIcon::ActivationReason是表示激活原因的枚举值
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));
    // 实例化出来托盘菜单
    menu = new QMenu;
}

// 设置悬浮提示文本
void TrayIcon::setToolTip(const QString &tip)
{
    trayIcon->setToolTip(tip);
}

// 定义设置所属的主窗体
void TrayIcon::setMainWidget(QWidget *mainWidget)
{
    this->mainWidget = mainWidget;
    // 给托盘加上菜单功能
    menu->addAction("主界面", this, SLOT(showMainWidget()));
    // 直接退出
    menu->addAction("退出", this, SLOT(closeAll()));
    // 菜单绑定到右键
    trayIcon->setContextMenu(menu);
}

// 定义气泡显示信息
void TrayIcon::showMessage(const QString &title, const QString &msg, QSystemTrayIcon::MessageIcon icon, int msec)
{
    trayIcon->showMessage(title, msg, icon, msec);
}

// 定义get和set visible
bool TrayIcon::getVisible() const
{
    return trayIcon->isVisible();
}
void TrayIcon::setVisible(bool visible)
{
    trayIcon->setVisible(visible);
}

// 定义槽函数
// 定义关闭的槽函数
void TrayIcon::closeAll()
{
    // 隐藏，并删掉，退出程序
    trayIcon->hide();
    trayIcon->deleteLater();
    qApp->exit();
}

// 定义显示主界面的槽函数
void TrayIcon::showMainWidget()
{
    // 有主窗体
    if(mainWidget)
    {
        // 显示出来主窗体
        mainWidget->showNormal();
        // 重新聚焦主窗体
        mainWidget->activateWindow();
    }
}

// 处理托盘激活事件
void TrayIcon::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    // 双击和单击显示主窗口
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick: {
        this->showMainWidget();
        break;
    }

    default:
        break;
    }
}
