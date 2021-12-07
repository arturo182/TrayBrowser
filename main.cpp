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

    QSystemTrayIcon trayIcon(&app);

    TrayMenu trayMenu;
    QObject::connect(&trayMenu, &TrayMenu::settingsApplied, &trayMenu, [&]()
    {
        QSettings sett;

        QFileIconProvider iconProvider;

        switch (sett.value("iconSource", TrayIconSource::App).toInt()) {
        case TrayIconSource::App:
            trayIcon.setIcon(QIcon(":/res/icon.png"));
            break;

        case TrayIconSource::Os:
            trayIcon.setIcon(iconProvider.icon(QFileIconProvider::Folder));
            break;

        case TrayIconSource::User:
            trayIcon.setIcon(QIcon(sett.value("iconFile").toString()));
            break;

        default:
            break;
        }
    });
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated, &trayIcon, [&](QSystemTrayIcon::ActivationReason reason)
    {
        (void)reason;

        trayMenu.popup(QCursor::pos());
    });

    trayIcon.show();

    return app.exec();
}
