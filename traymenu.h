#pragma once

#include "menubase.h"

class TrayMenu : public MenuBase
{
    Q_OBJECT

    public:
        TrayMenu();
        ~TrayMenu() override;

        void createContents(QFileIconProvider *iconProvider) override;

    private slots:
        void showSettingsDialog();

    protected:
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
};
