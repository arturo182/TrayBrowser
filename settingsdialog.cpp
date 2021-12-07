#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QFileDialog>
#include <QFileIconProvider>
#include <QStorageInfo>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    connect(m_ui->customItemTree, &QTreeWidget::currentItemChanged, this, &SettingsDialog::updateCustomItemButtons);
    connect(m_ui->buttonBox, &QDialogButtonBox::clicked, this, [&](QAbstractButton *button)
    {
        const auto stdButton = m_ui->buttonBox->standardButton(button);
        if (stdButton != QDialogButtonBox::Apply)
            return;

        emit applyClicked();
    });


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
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
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

void SettingsDialog::addCustomItem(const QString &dir)
{
    QFileIconProvider iconProvider;
    const QFileInfo info(dir);

    QTreeWidgetItem *treeItem = new QTreeWidgetItem(m_ui->customItemTree);
    treeItem->setText(0, info.completeBaseName());
    treeItem->setIcon(0, iconProvider.icon(info));
    treeItem->setData(0, Qt::UserRole, dir);
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

void SettingsDialog::on_addCustomItemButton_clicked()
{
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Add custom item"), m_lastBrowseDir);
    if (dir.isEmpty())
        return;

    addCustomItem(dir);

    m_lastBrowseDir = dir;

    updateCustomItemButtons();
}

void SettingsDialog::on_deleteCustomItemButton_clicked()
{
    QTreeWidgetItem *current = m_ui->customItemTree->currentItem();
    if (!current)
        return;

    delete current;

    updateCustomItemButtons();
}

void SettingsDialog::on_upCustomItemButton_clicked()
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

    updateCustomItemButtons();
}

void SettingsDialog::on_downCustomItemButton_clicked()
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

    updateCustomItemButtons();
}
