#include "pathmenu.h"
#include "settingsdialog.h"
#include "traymenu.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileIconProvider>
#include <QMouseEvent>
#include <QSettings>
#include <QStorageInfo>
#include <QTimer>
#include <QUrl>

TrayMenu::TrayMenu()
{
    QTimer::singleShot(100, this, [&]() { emit settingsApplied(); });
}

TrayMenu::~TrayMenu()
{

}

void TrayMenu::createContents(QFileIconProvider *iconProvider)
{
    QSettings set;
    const QStringList customItems = set.value("customItems").toStringList();
    const QStringList hiddenVolumes = set.value("hiddenVolumes").toStringList();

    if (customItems.size()) {
        for (const QString &dir : customItems) {
            const QFileInfo info(dir);
            const QIcon icon = iconProvider->icon(info);

            PathMenu *menu = new PathMenu(dir, icon, info.completeBaseName());
            connect(this, &QMenu::aboutToHide, menu, &QObject::deleteLater);
            addMenu(menu);
        }

        addSeparator();
    }

    bool needsSeparator = false;
    for (const auto &info : QStorageInfo::mountedVolumes()) {
        if (hiddenVolumes.contains(info.rootPath()))
            continue;

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

        needsSeparator = true;
    }

    if (needsSeparator)
        addSeparator();

    addAction(tr("&Settings..."), this, &TrayMenu::showSettingsDialog);
    addAction(tr("&Quit"), qApp, &QApplication::quit);
}

void TrayMenu::showSettingsDialog()
{
    QSettings set;
    SettingsDialog dlg;

    const auto loadSettings = [&]()
    {
        dlg.setCustomItems(set.value("customItems").toStringList());
        dlg.setHiddenVolumes(set.value("hiddenVolumes").toStringList());
        dlg.setTrayIconSource(static_cast<TrayIconSource>(set.value("iconSource").toInt()));
        dlg.setTrayIconFile(set.value("iconFile").toString());
        dlg.setDirsFirst(set.value("dirsFirst", true).toBool());
        dlg.setIgnoreCase(set.value("ignoreCase", true).toBool());
    };

    const auto applySettings = [&]()
    {
        set.setValue("customItems", dlg.customItems());
        set.setValue("hiddenVolumes", dlg.hiddenVolumes());
        set.setValue("iconSource", dlg.trayIconSource());
        set.setValue("iconFile", dlg.trayIconFile());
        set.setValue("dirsFirst", dlg.dirsFirst());
        set.setValue("ignoreCase", dlg.ignoreCase());

        emit settingsApplied();
    };
    connect(&dlg, &SettingsDialog::applyClicked, &dlg, applySettings);

    loadSettings();

    if (dlg.exec() == QDialog::Accepted)
        applySettings();
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
