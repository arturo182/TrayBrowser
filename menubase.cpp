#include "menubase.h"

#include <QApplication>
#include <QDesktopServices>
#include <QDrag>
#include <QFileIconProvider>
#include <QMimeData>
#include <QMouseEvent>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <shellapi.h>
#include <Shobjidl.h>
#include <ShlObj.h>
#include <windowsx.h>

class ItemIdListReleaser
{
    public:
        explicit ItemIdListReleaser(ITEMIDLIST *list) : m_list(list) { }
        ~ItemIdListReleaser() { if (m_list) CoTaskMemFree(m_list); }

    private:
        ITEMIDLIST *m_list = nullptr;
};

class ComInterfaceReleaser
{
    public:
        explicit ComInterfaceReleaser(IUnknown *i) : m_i(i) { }
        ~ComInterfaceReleaser() { if (m_i) m_i->Release(); }

    private:
        IUnknown *m_i = nullptr;
};

#define DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
DEFINE_GUID(IID_IShellFolder, 0x000214e6, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);
DEFINE_GUID(IID_IContextMenu, 0x000214e4, 0x0000, 0x0000, 0xc0,0x00, 0x00,0x00,0x00,0x00,0x00,0x46);

void showContextMenu(const QString &path, const QPoint &pos, void *parentWindow)
{
    assert(parentWindow);

    ITEMIDLIST *id = nullptr;
    std::wstring windowsPath = path.toStdWString();
    HRESULT result;

    std::replace(windowsPath.begin(), windowsPath.end(), '/', '\\');

    result = SHParseDisplayName(windowsPath.c_str(), 0, &id, 0, 0);
    if (!SUCCEEDED(result) || !id)
        return;

    ItemIdListReleaser idReleaser(id);

    IShellFolder *ifolder = nullptr;

    LPCITEMIDLIST idChild = nullptr;
    result = SHBindToParent(id, IID_IShellFolder, (void**)&ifolder, &idChild);
    if (!SUCCEEDED(result) || !ifolder)
        return;

    ComInterfaceReleaser ifolderReleaser(ifolder);

    IContextMenu *imenu = nullptr;
    result = ifolder->GetUIObjectOf((HWND)parentWindow, 1, (const ITEMIDLIST **)&idChild, IID_IContextMenu, 0, (void**)&imenu);
    if (!SUCCEEDED(result))
        return;

    ComInterfaceReleaser menuReleaser(imenu);

    HMENU hMenu = CreatePopupMenu();
    if (!hMenu)
        return;

    if (!SUCCEEDED(imenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
        DestroyMenu(hMenu);
        return;
    }

    const int iCmd = TrackPopupMenuEx(hMenu, TPM_RETURNCMD, pos.x(), pos.y(), (HWND)parentWindow, NULL);
    if (iCmd <= 0) {
        DestroyMenu(hMenu);
        return;
    }

    CMINVOKECOMMANDINFOEX info =
    {
        .cbSize = sizeof(info),
        .fMask = CMIC_MASK_UNICODE,
        .hwnd = (HWND)parentWindow,
        .lpVerb  = MAKEINTRESOURCEA(iCmd - 1),
        .nShow = SW_SHOWNORMAL,
        .lpVerbW = MAKEINTRESOURCEW(iCmd - 1),
    };
    imenu->InvokeCommand((LPCMINVOKECOMMANDINFO)&info);

    DestroyMenu(hMenu);
}
#endif

MenuBase::MenuBase()
{
    connect(this, &QMenu::aboutToShow, [&]()
    {
        QFileIconProvider iconProvider;

        clear();

        createContents(&iconProvider);
    });
}

MenuBase::~MenuBase()
{

}

void MenuBase::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton)
        return;

    if (!activeAction())
        return;

    m_dragStartPos = ev->globalPosition();
}

void MenuBase::mouseMoveEvent(QMouseEvent *ev)
{
    QMenu::mouseMoveEvent(ev);

    if (m_dragStartPos.isNull())
        return;

    if ((ev->globalPosition() - m_dragStartPos).manhattanLength() >= qApp->startDragDistance())
        return;

    QFileIconProvider iconProvider;

    QMimeData *mimeData = new QMimeData;
    mimeData->setUrls({ QUrl::fromLocalFile(currentPath()) });

    QDrag *drag = new QDrag(this);
    drag->setPixmap(iconProvider.icon(QFileInfo(currentPath())).pixmap(256));
    drag->setMimeData(mimeData);

    const auto dropAction = drag->exec(Qt::CopyAction | Qt::MoveAction | Qt::LinkAction);
    qDebug() << dropAction;

    drag->deleteLater();
    m_dragStartPos = QPointF();
}

void MenuBase::mouseReleaseEvent(QMouseEvent *ev)
{
    m_dragStartPos = QPointF();

    const QString path = currentPath();
    if (path.isEmpty())
        return;

    switch (ev->button()) {
    case Qt::LeftButton:
    {
        if (QFileInfo(path).isFile())
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        break;
    }

    case Qt::RightButton:
    {
#ifdef Q_OS_WIN
        showContextMenu(path, ev->globalPosition().toPoint(), reinterpret_cast<void*>(winId()));
#endif
        break;
    }

    default:
        break;
    }
}

void MenuBase::mouseDoubleClickEvent(QMouseEvent *ev)
{
    (void)ev;

    const QString path = currentPath();
    if (path.isEmpty())
        return;

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QString MenuBase::currentPath() const
{
    // QMenu is a bit weird, see README for details
    if (!activeAction())
        return menuAction()->property("path").toString();

    if (activeAction()->property("path").isValid())
        return activeAction()->property("path").toString();

    // "(Empty)" action
    return QString();
}
