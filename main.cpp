#include <QApplication>
#include <QFileIconProvider>
#include <QSettings>
#include <QSystemTrayIcon>

#include "pathmenu.h"
#include "traymenu.h"

int main(int argc, char *argv[])
{
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QApplication::setOrganizationName("arturo182");
    QApplication::setApplicationName("TrayBrowser");

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QFileIconProvider iconProvider;

    QSystemTrayIcon trayIcon(&app);
    trayIcon.setIcon(iconProvider.icon(QFileIconProvider::Folder));

    TrayMenu trayMenu;
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, &trayIcon, [&](QSystemTrayIcon::ActivationReason reason)
    {
        (void)reason;

        trayMenu.popup(QCursor::pos());
    });

    trayIcon.show();

    return app.exec();
}
