
#ifndef PLAYLIST_WIDGET_ITEM
#define PLAYLIST_WIDGET_ITEM


//***********************************************************//
// Qt5
//***********************************************************//
#include <QLabel>
#include <QWidget>
#include <QString>
#include <QVariant>
#include <QHBoxLayout>
#include <QPushButton>


class PlaylistWidgetItem : public QWidget {
    Q_OBJECT
public:
    explicit PlaylistWidgetItem (const QString&, const QVariant &data, QWidget *parent = nullptr);
    ~PlaylistWidgetItem ();
    
    std::string filePath () const;
    
private:
    QVariant m_data;
    
    QLabel      *label;
    QHBoxLayout *layout;
    QPushButton *button;
    
};

#endif
