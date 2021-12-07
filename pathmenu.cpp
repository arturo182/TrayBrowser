#include "pathmenu.h"

#include <QDebug>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMouseEvent>
#include <QSettings>
#include <QUrl>


PathMenu::PathMenu(const QString &fullPath, const QIcon &icon, const QString &label)
    : MenuBase()
    , m_fullPath(fullPath)
{
    setIcon(icon);

    menuAction()->setProperty("path", fullPath);

    if (label.isEmpty()) {
        const QDir dir(fullPath);
        setTitle(dir.dirName());
    } else {
        setTitle(label);
    }
}

PathMenu::~PathMenu()
{

}

void PathMenu::createContents(QFileIconProvider *iconProvider)
{
    const QSettings set;
    const QDir dir(m_fullPath);
    QDir::SortFlags sortFlags = QDir::Name;

    if (set.value("dirsFirst", true).toBool())
        sortFlags |= QDir::DirsFirst;

    if (set.value("ignoreCase", true).toBool())
        sortFlags |= QDir::IgnoreCase;

    const QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, sortFlags);

    if (entries.isEmpty()) {
        addAction(tr("(Empty)"));

        return;
    }

    for (const auto &info : entries) {
        if (info.isDir()) {
            PathMenu *menu = new PathMenu(info.absoluteFilePath(), iconProvider->icon(info));
            connect(this, &QMenu::aboutToHide, menu, &QObject::deleteLater);
            addMenu(menu);
        } else {
            QAction *action = addAction(iconProvider->icon(info), info.fileName());
            action->setProperty("path", info.absoluteFilePath());
        }
    }
}
