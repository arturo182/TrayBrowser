#pragma once

#include "traymenu.h"

#include <QDialog>

namespace Ui
{
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QWidget *parent = nullptr);
        ~SettingsDialog() override;

        QStringList customItems() const;
        void setCustomItems(const QStringList &items);

        QStringList hiddenVolumes() const;
        void setHiddenVolumes(const QStringList &volumes);

        TrayIconSource trayIconSource() const;
        void setTrayIconSource(const TrayIconSource &source);

        QString trayIconFile() const;
        void setTrayIconFile(const QString &fileName);

        bool dirsFirst() const;
        void setDirsFirst(const bool dirsFirst);

        bool ignoreCase() const;
        void setIgnoreCase(const bool ignoreCase);

    signals:
        void applyClicked();

    private:
        void addCustomItem(const QString &dir);

    private slots:
        void browseUserTrayIcon();
        void updateCustomItemButtons();
        void browseAndAddCustomItem();
        void deleteSelectedCustomItem();
        void moveSelectedCustomItemUp();
        void moveSelectedCustomItemDown();
        void openCurrentItemsDir();

    private:
        Ui::SettingsDialog *m_ui;
        QString m_lastBrowseDir;
};
