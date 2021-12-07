#pragma once

#include <QMenu>

class QFileIconProvider;

class MenuBase : public QMenu
{
    Q_OBJECT

    public:
        MenuBase();

        virtual void createContents(QFileIconProvider *iconProvider) = 0;

    protected:
        void mousePressEvent(QMouseEvent *ev) override;
        void mouseMoveEvent(QMouseEvent *ev) override;
        void mouseReleaseEvent(QMouseEvent *ev) override;
        void mouseDoubleClickEvent(QMouseEvent *ev) override;

    private:
        QString currentPath() const;

    private:
        QPointF m_dragStartPos;
//        bool m_buttonDown;
//        bool m_dragging;
};
