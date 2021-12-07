#pragma once

#include "menubase.h"

enum TrayIconSource
{
    App = 0,
    Os,
    User,
};

class TrayMenu : public MenuBase
{
    Q_OBJECT

    public:
        TrayMenu();
        ~TrayMenu() override;

        void createContents(QFileIconProvider *iconProvider) override;

    signals:
        void settingsApplied();

    private slots:
        void showSettingsDialog();

    protected:
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
};
