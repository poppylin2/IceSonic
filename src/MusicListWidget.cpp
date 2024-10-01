#include "MusicListWidget.h"

MusicListWidget::MusicListWidget(QWidget *parent)
    :QListWidget (parent)
{
    
}

void MusicListWidget::refresh()
{
    clear();
    musicList.addToListWidget(this);    
}

void MusicListWidget::setMusicList(const MusicList &music)
{
    musicList=music;
    refresh();
}

void MusicListWidget::setMusicList_playing(const MusicList &music)
{
    musicList=music;
    musicList.setSQL(false);
    refresh();
}

void MusicListWidget::removeMusic()
{
    int pos=currentRow();    

    musicList.removeMusic(pos);

    QListWidgetItem *tempItem=takeItem(pos);
    removeItemWidget(tempItem);
    delete tempItem;
}

void MusicListWidget::showInExplorer()
{
    int pos=currentRow();
    musicList.showInExplorer(pos);
}

void MusicListWidget::detail()
{
    int pos=currentRow();
    musicList.detail(pos);
}
