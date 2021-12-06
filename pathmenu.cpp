#include "pathmenu.h"

#include <QFileIconProvider>
#include <QDebug>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

PathMenu::PathMenu(const QString &fullPath, const QString &label)
    : QMenu()
    , m_fullPath(fullPath)
{
    connect(this, &QMenu::aboutToShow, this, &PathMenu::fill);

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

void PathMenu::fill()
{
    clear();

    QFileIconProvider iconProvider;

    const QDir dir(m_fullPath);
    for (const auto &info : dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name)) {
        const bool isEmpty = QDir(info.filePath()).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty();

        if (info.isDir() && !isEmpty) {
            PathMenu *subMenu = new PathMenu(info.absoluteFilePath());
            subMenu->setIcon(iconProvider.icon(info));
            addMenu(subMenu);

            QObject::connect(this, &QMenu::aboutToHide, subMenu, &QObject::deleteLater);
        } else {
            QAction *action = addAction(iconProvider.icon(info), info.fileName());

            if (!info.isDir())
                action->setProperty("path", info.absoluteFilePath());
        }
    }
}

void PathMenu::mousePressEvent(QMouseEvent *ev)
{
    QMenu::mousePressEvent(ev);

    switch (ev->button()) {
    case Qt::LeftButton:
    {
//        qDebug() << m_fullPath << activeAction() << "left click!";
        if (activeAction()) {
            const QString path = activeAction()->property("path").toString();
            if (!path.isEmpty())
                QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
        break;
    }

    case Qt::RightButton:
        qDebug() << m_fullPath << activeAction() << "right click!";
        break;

    default:
        break;
    }
}

void PathMenu::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QMenu::mouseDoubleClickEvent(ev);

    qDebug() << m_fullPath << activeAction() <<  "double click!";
}
