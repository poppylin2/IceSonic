#ifndef MYLISTWIDGET_H
#define MYLISTWIDGET_H

#include<QListWidget>
#include<MusicList.h>

class MusicListWidget:public QListWidget
{
    Q_OBJECT
    // Current song list (stores song information)
    MusicList musicList;
    // The icon used for the current displayed list items
    QIcon icon=QIcon(":/image/image/image/music.png");
public:
    MusicListWidget(QWidget *parent = Q_NULLPTR);
    // Refresh the display (needs to be called when musicList changes)
    void refresh();

    void setMusicList(const MusicList& music);

    void setMusicList_playing(const MusicList& music);

    void removeMusic();

    void showInExplorer();

    void detail();

    void setIcon(QIcon iicon){ icon=iicon; }
    QIcon getIcon(){ return icon; }
    
    friend class MainWidget;
private:
    
};

#endif // MYLISTWIDGET_H
