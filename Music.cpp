#include "Music.h"
#include <QMediaPlayer>
#include <QCoreApplication>
#include <QMessageBox>
#include <QtSql>

Music::Music(QUrl iurl)
{
    url=iurl;
    refreshInfo();
}

extern QString formatTime(qint64 timeMilliSeconds);
void Music::refreshInfo()
{
    QMediaPlayer tempPlayer;
    tempPlayer.setMedia(url);

    while(!tempPlayer.isMetaDataAvailable()){
        QCoreApplication::processEvents();
    }
    QStringList list=tempPlayer.availableMetaData();
    if(tempPlayer.isMetaDataAvailable()){

        author = tempPlayer.metaData(QStringLiteral("Author")).toStringList().join(",");
        //author = tempPlayer.metaData(QStringLiteral("Author")).toString();
        //author=tempPlayer.metaData(QStringLiteral("ContributingArtist")).toStringList().join(",");
        title = tempPlayer.metaData(QStringLiteral("Title")).toString();
        albumTitle = tempPlayer.metaData(QStringLiteral("AlbumTitle")).toString();
        audioBitRate = tempPlayer.metaData(QStringLiteral("AudioBitRate")).toInt();
        duration=tempPlayer.duration();
    }
}

QString Music::getLyricFile()
{
    QString mp3FilePath=url.toLocalFile();
    mp3FilePath.replace(".mp3",".lrc");
    mp3FilePath.replace(".flac",".lrc");
    mp3FilePath.replace(".mpga",".lrc");
    return mp3FilePath;
}

QString Music::getInfo() const
{
    return author+" - "+title+" ["+formatTime(duration)+"]";
}

void Music::detail()
{
    QString info("Title: %1\nArtist: %2\nDuration: %3\nAlbum: %4\nBitrate: %5\nFile Path: %6");
    info = info.arg(title, author, formatTime(duration), albumTitle, QString::number(audioBitRate) + "bps", url.toString());
    QMessageBox::about(Q_NULLPTR, "Song Information", info);
}

void Music::insertSQL(const QString &name)
{
    QSqlQuery sql_query;
    QString insert_sql = "insert into MusicInfo values (?, ?, ?, ?, ?, ?, ?)";
    sql_query.prepare(insert_sql);
    sql_query.addBindValue(name);
    sql_query.addBindValue(url.toString());
    sql_query.addBindValue(author);
    sql_query.addBindValue(title);
    sql_query.addBindValue(duration);
    sql_query.addBindValue(albumTitle);
    sql_query.addBindValue(audioBitRate);
    sql_query.exec();
}

bool MusicCompare::operator()(const Music &A, const Music &B)
{
    switch (key) {
    case TITLE:
        return A.title<B.title;
    case AUTHOR:
        return A.author<B.author;
    case DURATION:
        return A.duration<B.duration;
    case EQUALITY:
        return A.getUrl()==B.getUrl();
    default:
        return A.getInfo()<B.getInfo();
    }
}
