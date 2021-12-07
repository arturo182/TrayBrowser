#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "version.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileIconProvider>
#include <QImageReader>
#include <QLibraryInfo>
#include <QSettings>
#include <QStorageInfo>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    const auto updateTrayIconWidgets = [&]()
    {
        const bool isUser = m_ui->trayIconUserRadio->isChecked();

        m_ui->trayIconPathEdit->setEnabled(isUser);
        m_ui->trayIconBrowseButton->setEnabled(isUser);
    };

    connect(m_ui->trayIconAppRadio,  &QRadioButton::toggled, this, updateTrayIconWidgets);
    connect(m_ui->trayIconOsRadio,   &QRadioButton::toggled, this, updateTrayIconWidgets);
    connect(m_ui->trayIconUserRadio, &QRadioButton::toggled, this, updateTrayIconWidgets);
    connect(m_ui->trayIconBrowseButton, &QPushButton::clicked, this, &SettingsDialog::browseUserTrayIcon);
    connect(m_ui->trayIconPathEdit, &QLineEdit::textChanged, this, [&](const QString &text)
    {
        const bool fileExists = QFileInfo::exists(text);
        m_ui->trayIconPathEdit->setStyleSheet(QString("color: %1;").arg(fileExists ? "black" : "red"));
    });

    connect(m_ui->addCustomItemButton,    &QToolButton::clicked, this, &SettingsDialog::browseAndAddCustomItem);
    connect(m_ui->deleteCustomItemButton, &QToolButton::clicked, this, &SettingsDialog::deleteSelectedCustomItem);
    connect(m_ui->upCustomItemButton,     &QToolButton::clicked, this, &SettingsDialog::moveSelectedCustomItemUp);
    connect(m_ui->downCustomItemButton,   &QToolButton::clicked, this, &SettingsDialog::moveSelectedCustomItemDown);

    connect(m_ui->customItemTree,   &QTreeWidget::currentItemChanged, this, &SettingsDialog::updateCustomItemButtons);
    connect(m_ui->customItemTree,   &QTreeWidget::doubleClicked,      this, &SettingsDialog::openCurrentItemsDir);
    connect(m_ui->hiddenVolumeTree, &QTreeWidget::doubleClicked,      this, &SettingsDialog::openCurrentItemsDir);

    connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, [&](QAbstractButton *button)
    {
        const auto stdButton = m_ui->buttonBox->standardButton(button);
        if (stdButton != QDialogButtonBox::Apply)
            return;

        emit applyClicked();
    });

    // fill volume list
    for (const auto &info : QStorageInfo::mountedVolumes()) {
        QString label = info.displayName();

#ifdef Q_OS_WIN
        if ((label != info.rootPath()) && !info.rootPath().isEmpty()) {
            label += QString(" (%1)").arg(info.rootPath().chopped(1));
        }
#endif

        QTreeWidgetItem *treeItem = new QTreeWidgetItem(m_ui->hiddenVolumeTree);
        treeItem->setText(0, label);
        treeItem->setData(0, Qt::UserRole, info.rootPath());
        treeItem->setCheckState(0, Qt::Unchecked);
    }

    m_ui->aboutTextLabel->setText(tr("<h3>TrayBrowser %1.%2</h3>"
                                     "Based on Qt %3<br><br>"
                                     "Built on %4 %5<br><br>"
                                     "Copyright %6 © arturo182<br><br>"
                                     "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, "
                                     "EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF "
                                     "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.")
                                  .arg(VER_MAJOR)
                                  .arg(VER_MINOR)
                                  .arg(QLibraryInfo::version().toString())
                                  .arg(__DATE__).arg(__TIME__)
                                  .arg(QDateTime::currentDateTime().date().year()));

    QSettings sett;
    m_lastBrowseDir = sett.value("lastBrowseDir").toString();
}

SettingsDialog::~SettingsDialog()
{
    QSettings sett;
    sett.setValue("lastBrowseDir", m_lastBrowseDir);

    delete m_ui;
    m_ui = nullptr;
}

QStringList SettingsDialog::customItems() const
{
    QStringList customItems;

    QTreeWidget *tree = m_ui->customItemTree;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);

        customItems << item->data(0, Qt::UserRole).toString();
    }

    return customItems;
}

void SettingsDialog::setCustomItems(const QStringList &items)
{
    for (const QString &dir : items)
        addCustomItem(dir);

    updateCustomItemButtons();
}

QStringList SettingsDialog::hiddenVolumes() const
{
    QStringList volumes;

    QTreeWidget *tree = m_ui->hiddenVolumeTree;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);

        if (item->checkState(0) == Qt::Checked)
            volumes << item->data(0, Qt::UserRole).toString();
    }

    return volumes;
}

void SettingsDialog::setHiddenVolumes(const QStringList &volumes)
{
    QTreeWidget *tree = m_ui->hiddenVolumeTree;

    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        const QString volume = item->data(0, Qt::UserRole).toString();

        item->setCheckState(0, volumes.contains(volume) ? Qt::Checked : Qt::Unchecked);
    }
}

TrayIconSource SettingsDialog::trayIconSource() const
{
    if (m_ui->trayIconAppRadio->isChecked())
        return TrayIconSource::App;

    if (m_ui->trayIconOsRadio->isChecked())
        return TrayIconSource::Os;

    return TrayIconSource::User;
}

void SettingsDialog::setTrayIconSource(const TrayIconSource &source)
{
    switch (source) {
    case TrayIconSource::App:
        m_ui->trayIconAppRadio->setChecked(true);
        break;

    case TrayIconSource::Os:
        m_ui->trayIconOsRadio->setChecked(true);
        break;

    default:
        m_ui->trayIconUserRadio->setChecked(true);
        break;
    }
}

QString SettingsDialog::trayIconFile() const
{
    return m_ui->trayIconPathEdit->text();
}

void SettingsDialog::setTrayIconFile(const QString &fileName)
{
    m_ui->trayIconPathEdit->setText(fileName);
}

bool SettingsDialog::dirsFirst() const
{
    return m_ui->dirsFirstCheckBox->isChecked();
}

void SettingsDialog::setDirsFirst(const bool dirsFirst)
{
    m_ui->dirsFirstCheckBox->setChecked(dirsFirst);
}

bool SettingsDialog::ignoreCase() const
{
    return m_ui->ignoreCaseCheckBox->isChecked();
}

void SettingsDialog::setIgnoreCase(const bool ignoreCase)
{
    m_ui->ignoreCaseCheckBox->setChecked(ignoreCase);
}

void SettingsDialog::addCustomItem(const QString &dir)
{
    QFileIconProvider iconProvider;
    const QFileInfo info(dir);

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(m_ui->customItemTree);
    treeItem->setText(0, info.completeBaseName());
    treeItem->setIcon(0, iconProvider.icon(info));
    treeItem->setData(0, Qt::UserRole, dir);
}

void SettingsDialog::browseUserTrayIcon()
{
    QString filters = "Images (";
    for (const QByteArray &type : QImageReader::supportedImageFormats())
        filters += QString(" *.%1").arg(type);
    filters += ")";

    const QString fileName = QFileDialog::getOpenFileName(this, tr("Select icon"), m_lastBrowseDir, filters);
    if (fileName.isEmpty())
        return;

    m_ui->trayIconPathEdit->setText(fileName);

    m_lastBrowseDir = QFileInfo(fileName).absolutePath();
}

void SettingsDialog::updateCustomItemButtons()
{
    QTreeWidget *tree = m_ui->customItemTree;

    const QTreeWidgetItem *current = tree->currentItem();
    const bool hasSelection = (current != nullptr);
    const bool canMoveUp    = (tree->itemAbove(current) != nullptr);
    const bool canMoveDown  = (tree->itemBelow(current) != nullptr);

    m_ui->deleteCustomItemButton->setEnabled(hasSelection);
    m_ui->upCustomItemButton->setEnabled(canMoveUp);
    m_ui->downCustomItemButton->setEnabled(canMoveDown);
}

void SettingsDialog::browseAndAddCustomItem()
{
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Add custom item"), m_lastBrowseDir);
    if (dir.isEmpty())
        return;

    addCustomItem(dir);

    m_lastBrowseDir = dir;

    updateCustomItemButtons();
}

void SettingsDialog::deleteSelectedCustomItem()
{
    QTreeWidgetItem *current = m_ui->customItemTree->currentItem();
    if (!current)
        return;

    delete current;

    updateCustomItemButtons();
}

void SettingsDialog::moveSelectedCustomItemUp()
{
    QTreeWidget *tree = m_ui->customItemTree;

    QTreeWidgetItem *current = tree->currentItem();
    if (!current)
        return;

    const int idx = tree->indexOfTopLevelItem(current);
    if (idx == 0)
        return;

    tree->takeTopLevelItem(idx);
    tree->insertTopLevelItem(idx - 1, current);
    tree->setCurrentItem(current);

    updateCustomItemButtons();
}

void SettingsDialog::moveSelectedCustomItemDown()
{
    QTreeWidget *tree = m_ui->customItemTree;

    QTreeWidgetItem *current = tree->currentItem();
    if (!current)
        return;

    const int idx = tree->indexOfTopLevelItem(current);
    if (idx == tree->topLevelItemCount() - 1)
        return;

    tree->takeTopLevelItem(idx);
    tree->insertTopLevelItem(idx + 1, current);
    tree->setCurrentItem(current);

    updateCustomItemButtons();
}

void SettingsDialog::openCurrentItemsDir()
{
    QTreeWidget *tree = qobject_cast<QTreeWidget*>(sender());
    if (!tree)
        return;

    QTreeWidgetItem *current = tree->currentItem();
    if (!current)
        return;

    const QString dir = current->data(0, Qt::UserRole).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}
