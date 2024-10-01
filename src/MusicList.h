#ifndef MUSICLIST_H
#define MUSICLIST_H
#include<vector>
using namespace std; 

#include"Music.h"
#include <QMediaPlaylist>
#include <QListWidget> 

class MusicListWidget;
// Music Playlist
class MusicList
{    
    // Playlist name
    QString name;
    // Playlist name
    vector<Music> music;
    
    // Control whether database interaction is needed (e.g., when deleting songs from the current playlist, there is no need to update the database as it is temporary)
    bool sql_flag=true;
    
    friend class MainWidget;
public:
    MusicList(){}
    MusicList(const QList<QUrl>& urls,QString iname="");
    // Set playlist name
    void setName(const QString& iname){name=iname;}
    QString getName(){ return name; }
    // Set the database flag
    void setSQL(bool on){ sql_flag=on; }
    // Add songs from URLs
    void addMusic(const QList<QUrl>& urls);
    // Add a single song
    void addMusic(const Music& iMusic);
    // Get the song at a specific position
    Music getMusic(int pos);
    // Add this playlist to the media playlist
    void addToPlayList(QMediaPlaylist *playlist);
    // Visualize the playlist
    void addToListWidget(MusicListWidget *listWidget);
    // Remove the specified song
    void removeMusic(int pos);
    // Open the folder containing the song
    void showInExplorer(int pos);
    // Show detailed information of the specified song
    void detail(int pos);
    // Restore this playlist from the database
    void remove_SQL_all();
    // Insert all songs from the playlist into the database
    void insert_SQL_all();
    // Restore this playlist from the database
    void read_fromSQL();
    // Sort the songs in this playlist by the specified key
    void sort_by(COMPARE key);
    // Organize the playlist: remove duplicate songs and sort the playlist
    void neaten();
    // Clear the playlist
    void clear();
};

#endif // MUSICLIST_H
