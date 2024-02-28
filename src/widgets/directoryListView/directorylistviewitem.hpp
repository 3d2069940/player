
#ifndef _DIRECTORYLISTVIEWITEM_HPP_
#define _DIRECTORYLISTVIEWITEM_HPP_


#include "directorylistviewitem.h"
#include <QDebug>
#include <qcheckbox.h>
#include <qnamespace.h>


class DirectoryListView;

DirectoryListViewItem::DirectoryListViewItem (const QString &folderName, Qt::CheckState state) 
    : label(folderName) {
    checkbox.setTristate(true);
    checkbox.setCheckState(state);
    checkbox.setFocusPolicy(Qt::NoFocus);

    layout.addWidget(&checkbox);
    layout.addWidget(&label);

    setLayout(&layout);

    if (parent() != nullptr) {
        qDebug() << "bruh";
    }
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

#endif // _DIRECTORYLISTVIEWITEM_HPP_
