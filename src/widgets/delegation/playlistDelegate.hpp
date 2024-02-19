
#pragma once

#include <QPainter>
#include <QStyledItemDelegate>

class PlaylistDelegate : public QStyledItemDelegate {

public:
    void paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if (index.data(Qt::DisplayRole).toBool()) {
            return;
        }
        if ((index.row()&1) == 0)
            painter->fillRect(option.rect, QPalette().color(QPalette::Window));
        else
            painter->fillRect(option.rect, QPalette().color(QPalette::AlternateBase));
        QStyledItemDelegate::paint(painter, option, index);
    }
};
