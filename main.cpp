#include <QApplication>
#include <QSystemTrayIcon>
#include <QStorageInfo>
#include <QFileIconProvider>
#include <QMenu>

#include "pathmenu.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFileIconProvider iconProvider;

    QSystemTrayIcon trayIcon(&app);
    trayIcon.setIcon(iconProvider.icon(QFileIconProvider::Folder));

    QMenu trayMenu;
    QObject::connect(&trayMenu, &QMenu::aboutToShow, [&]()
    {
        trayMenu.clear();

        for (const auto &info : QStorageInfo::mountedVolumes()) {
            PathMenu *pathMenu = new PathMenu(info.rootPath(), info.displayName());
            pathMenu->setIcon(iconProvider.icon(QFileInfo(info.rootPath())));
            trayMenu.addMenu(pathMenu);

            QObject::connect(&trayMenu, &QMenu::aboutToHide, pathMenu, &QObject::deleteLater);
        }

        trayMenu.addSeparator();
        trayMenu.addAction("&Settings..."); // TODO
        trayMenu.addAction("&Quit", &app, &QApplication::quit);
    });

    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, [&](QSystemTrayIcon::ActivationReason reason)
    {
        (void)reason;

        trayMenu.popup(QCursor::pos());
    });

    trayIcon.show();

    return app.exec();
}
