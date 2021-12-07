#pragma once

#include "menubase.h"

#include <QDir>

class PathMenu : public MenuBase
{
    Q_OBJECT

    public:
        PathMenu(const QString &fullPath, const QIcon &icon = QIcon(), const QString &label = QString());
        ~PathMenu() override;

        void createContents(QFileIconProvider *iconProvider) override;

    private:
        QString m_fullPath;
};
