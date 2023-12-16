
#ifndef PLAYLIST_WIDGET_ITEM
#define PLAYLIST_WIDGET_ITEM

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
    
    QString filePath () const;
    
signals:
    void buttonClicked ();
    
private:
    QHBoxLayout *layout;
    QPushButton *button;
    QLabel      *label;
    
    QVariant m_data;
};

#endif
