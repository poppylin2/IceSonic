#ifndef MUSIC_H
#define MUSIC_H
#include <QString>
#include <QUrl>

//Music class
class Music
{
private:
    //Song resource URL
    QUrl url;
    //Artist
    QString author;
    //Song title
    QString title;
    // Duration
    qint64 duration; 
    // Album title
    QString albumTitle;
    // BItrate
    int audioBitRate;

    // Retrieve song information based on URL
    void refreshInfo();
    
    friend class MusicCompare;
public:
    Music(){}
    Music(QUrl iurl);
    
    // Return the song URL
    QUrl getUrl() const {return url;}
    // Return the song information
    QString getInfo() const;
    // Show a popup with song details
    void detail();
    // Insert into the database
    void insertSQL(const QString& name);
    // Get the lyrics file path based on the file name
    QString getLyricFile();
    
    friend class MusicList;
};

// Sortable attributes
enum COMPARE{DEFAULT,TITLE,AUTHOR,DURATION,EQUALITY};

// Functor for song sorting
class MusicCompare
{
    COMPARE key;
public:
    MusicCompare(){key=DEFAULT;}
    MusicCompare(COMPARE ikey){key=ikey;}
    bool operator()(const Music& A, const Music& B);
};

#endif // MUSIC_H
