#include "MusicList.h"
#include <QCoreApplication>
#include <QProgressDialog>
#include <QDesktopServices>
#include <QtSql>
#include <algorithm>
#include "MusicListWidget.h"

MusicList::MusicList(const QList<QUrl> &urls, QString iname)
{
    addMusic(urls);
    setName(iname);
}

void MusicList::addMusic(const QList<QUrl> &urls)
{

    QProgressDialog proDialog("Adding Progress", "Cancel", 0, urls.size());
    proDialog.setMinimumSize(350,150);
    proDialog.setWindowModality(Qt::WindowModal);
    proDialog.setWindowTitle("Adding... Please wait");
    proDialog.show();
    int x=0;
    foreach (QUrl i, urls) {
        x++;
        proDialog.setValue(x);
        // Filter the type of URL
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(i.toLocalFile());
        if(mime.name()!="audio/mpeg"&&mime.name()!="audio/flac"){
            continue;
        }
        // Remaining files match the type
        music.push_back(Music(i));
        if(sql_flag){
            music[music.size()-1].insertSQL(name);
        }
        if(proDialog.wasCanceled()) break;
    }
}

void MusicList::addMusic(const Music &iMusic)
{
    music.push_back(iMusic);
    if(sql_flag){
        music[music.size()-1].insertSQL(name);
    }
}

Music MusicList::getMusic(int pos)
{
    return music[pos];
}

void MusicList::addToPlayList(QMediaPlaylist *playlist)
{
    for(auto i=music.begin();i!=music.end();i++){
        playlist->addMedia(i->getUrl());
    }
}

void MusicList::addToListWidget(MusicListWidget *listWidget)
{
    foreach(const Music &i,music){
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(listWidget->getIcon());
        item->setText(i.getInfo());
        listWidget->addItem(item);
    }
}

void MusicList::removeMusic(int pos)
{
    if(sql_flag){
        remove_SQL_all();
        int i=0;
        for(auto it=music.begin();it!=music.end();){
            if(i==pos){
                it= music.erase(it);
                break;
            }
            else{
                it++;
            }
            i++;
            
        }
        insert_SQL_all();    
    }else{
        int i=0;
        for(auto it=music.begin();it!=music.end();){
            if(i==pos){
                it= music.erase(it);
                break;
            }
            else{
                it++;
            }
            i++;
            
        }
    }
    
}

void MusicList::showInExplorer(int pos)
{
    QString str=music[pos].getUrl().toString();
    str.remove(str.split("/").last());
    QDesktopServices::openUrl(str);
}

void MusicList::detail(int pos)
{
    music[pos].detail();
}

void MusicList::remove_SQL_all()
{
    QSqlQuery sql_query;
    QString delete_sql = "delete from MusicInfo where name = ?";
    sql_query.prepare(delete_sql);
    sql_query.addBindValue(name);
    sql_query.exec();
}

void MusicList::insert_SQL_all()
{
    for(auto i=music.begin();i!=music.end();i++){
        i->insertSQL(name);
    }
}

void MusicList::read_fromSQL()
{
    QSqlQuery sql_query;
    QString select_sql = "select url, author, title, duration, albumTitle, audioBitRate from MusicInfo where name = ?";
    sql_query.prepare(select_sql);
    sql_query.addBindValue(name);
    if(sql_query.exec())
    {
        while(sql_query.next())
        {
            Music tempMusic;
            tempMusic.url=QUrl(sql_query.value(0).toString());
            tempMusic.author=sql_query.value(1).toString();
            tempMusic.title=sql_query.value(2).toString();
            tempMusic.duration=sql_query.value(3).toLongLong();
            tempMusic.albumTitle=sql_query.value(4).toString();
            tempMusic.audioBitRate=sql_query.value(5).toInt();
            music.push_back(tempMusic);
        }  
    }
}

void MusicList::sort_by(COMPARE key)
{
    sort(music.begin(),music.end(),MusicCompare(key));
    if(sql_flag){
        remove_SQL_all();
        insert_SQL_all();
    }
}

void MusicList::neaten()
{
    sort(music.begin(),music.end(),MusicCompare(DEFAULT));
    music.erase(unique(music.begin(),music.end(),MusicCompare(EQUALITY)), music.end());
    if(sql_flag){
        remove_SQL_all();
        insert_SQL_all();
    }
}

void MusicList::clear()
{
    music.clear();
    if(sql_flag){
        remove_SQL_all();    
    }
}
