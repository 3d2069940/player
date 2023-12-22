
#ifndef PLAYLIST_WIDGET_ITEM
#define PLAYLIST_WIDGET_ITEM


//***********************************************************//
// Qt5
//***********************************************************//
#include <QMenu>
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QResizeEvent>


class PlaylistWidgetItem : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistWidgetItem (const QString&, const QVariant &data);
    ~PlaylistWidgetItem ();
    
    std::string filePath () const;
    
    QVariant     m_data;
    
    QLabel      *label;
    
    QMenu       *contextMenu;
    QHBoxLayout *layout;
    QPushButton *button;

public slots:
    void contextMenuShow ();
    
};

#endif
