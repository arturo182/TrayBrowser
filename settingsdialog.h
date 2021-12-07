#pragma once

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

    signals:
        void applyClicked();

    private:
        void addCustomItem(const QString &dir);

    private slots:
        void updateCustomItemButtons();
        void on_addCustomItemButton_clicked();
        void on_deleteCustomItemButton_clicked();
        void on_upCustomItemButton_clicked();
        void on_downCustomItemButton_clicked();

    private:
        Ui::SettingsDialog *m_ui;
        QString m_lastBrowseDir;
};
