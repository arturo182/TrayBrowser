#pragma once

#include <QDir>
#include <QMenu>

class PathMenu : public QMenu
{
    Q_OBJECT

    public:
        PathMenu(const QString &fullPath, const QString &label = QString());
        ~PathMenu() override;

        void fill();

    protected:
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseDoubleClickEvent(QMouseEvent *ev) override;

    private:
        QString m_fullPath;
};
