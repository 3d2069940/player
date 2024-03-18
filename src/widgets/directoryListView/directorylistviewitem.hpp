
#ifndef _DIRECTORYLISTVIEWITEM_HPP_
#define _DIRECTORYLISTVIEWITEM_HPP_


#include <QEvent>
#include <QDebug>
#include <QCheckBox>
#include <qsize.h>
#include <qsizepolicy.h>

#include "directorylistviewitem.h"


class DirectoryListView;

DirectoryListViewItem::DirectoryListViewItem (const QString &folderName, Qt::CheckState state) 
    : label(folderName) {
    setupWidgets(state);
    updateIcons();
}

DirectoryListViewItem::~DirectoryListViewItem () {

}

void DirectoryListViewItem::setCheckState() {
    QSignalBlocker blocker (&checkbox);
    Qt::CheckState state = checkbox.checkState();
    if (state == Qt::Unchecked || state == Qt::PartiallyChecked)
        checkbox.setCheckState(Qt::Checked);
    else 
        checkbox.setCheckState(Qt::Unchecked);
}

QString DirectoryListViewItem::getFolderName () const {
    return label.text();
}

QCheckBox *DirectoryListViewItem::getCheckBox () {
    return &checkbox;
}

void DirectoryListViewItem::setupWidgets (Qt::CheckState state) {
    checkbox.setTristate(true);
    checkbox.setCheckState(state);
    checkbox.setFocusPolicy(Qt::NoFocus);
    QSizePolicy policy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    checkbox.setSizePolicy(policy);
    
    layout.addWidget(&checkbox);
    layout.addWidget(&label);

    setLayout(&layout);
}

void DirectoryListViewItem::updateIcons () {
    QColor   backgroundColor = palette().color(QPalette::Window);
    QString iconPath = backgroundColor.value() < 128 ? QStringLiteral(":icons/white/") :
                                                       QStringLiteral(":icons/black/");

    checkbox.setIcon(QIcon(iconPath+"folder.svg"));
}

void DirectoryListViewItem::changeEvent (QEvent *event) {
    if (event->type() == QEvent::PaletteChange)
        updateIcons();
    QWidget::changeEvent(event);
}

#endif // _DIRECTORYLISTVIEWITEM_HPP_
