#include "pathmenu.h"
#include "traymenu.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMouseEvent>
#include <QStorageInfo>
#include <QUrl>

TrayMenu::TrayMenu()
{

}

TrayMenu::~TrayMenu()
{

}

void TrayMenu::createContents(QFileIconProvider *iconProvider)
{
    for (const auto &info : QStorageInfo::mountedVolumes()) {
        const QIcon icon = iconProvider->icon(QFileInfo(info.rootPath()));
        QString label = info.displayName();

#ifdef Q_OS_WIN
        if ((label != info.rootPath()) && !info.rootPath().isEmpty()) {
            label += QString(" (%1)").arg(info.rootPath().chopped(1));
        }
#endif

        PathMenu *menu = new PathMenu(info.rootPath(), icon, label);
        connect(this, &QMenu::aboutToHide, menu, &QObject::deleteLater);
        addMenu(menu);

    }

    addSeparator();
    addAction(tr("&Settings...")); // TODO
    addAction(tr("&Quit"), qApp, &QApplication::quit);
}

void TrayMenu::mousePressEvent(QMouseEvent *ev)
{
    // process Settings and Quit normally
    const int actionIdx = actions().indexOf(activeAction());
    if (actionIdx >= actions().size() - 2) {
        QMenu::mousePressEvent(ev);
        return;
    }

    MenuBase::mousePressEvent(ev);
}

void TrayMenu::mouseReleaseEvent(QMouseEvent *ev)
{
    // process Settings and Quit normally
    const int actionIdx = actions().indexOf(activeAction());
    if (actionIdx >= actions().size() - 2) {
        QMenu::mouseReleaseEvent(ev);
        return;
    }

    MenuBase::mouseReleaseEvent(ev);
}
