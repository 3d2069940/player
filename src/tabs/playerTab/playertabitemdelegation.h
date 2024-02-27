
#ifndef _PLAYERTABITEMDELEGATE_H_
#define _PLAYERTABITEMDELEGATE_H_

#include <QPainter>
#include <QStyledItemDelegate>

class PlayerTabItemDelegate : public QStyledItemDelegate {

public:
    void paint (QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        if ((index.row()&1) == 0)
            painter->fillRect(option.rect, QPalette().color(QPalette::Window));
        else
            painter->fillRect(option.rect, QPalette().color(QPalette::AlternateBase));
        QStyledItemDelegate::paint(painter, option, index);
    }
};

#endif // _PLAYERTABITEMDELEGATE_H_
