#include "MainWidget.h"
#include "ui_mainWidget.h"
#include <QPainter>
#include "myQSS.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QScrollBar>
#include "Music.h"
#include <QMouseEvent>
#include <QtSql>
#include <QMessageBox>
#include <QInputDialog>
#include "MusicListDialog.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    
    init_UI();

    init_play();

    init_actions();

    init_sqlite();

    init_musicList();

    init_settings();
    

    init_systemTrayIcon();
}

MainWidget::~MainWidget()
{
    delete ui;
}
void MainWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void MainWidget::init_UI()
{
    setAttribute(Qt::WA_TranslucentBackground, true);

    setWindowFlags(Qt::FramelessWindowHint);
    
    ui->volumeSlider->hide();
    ui->playListWidget->verticalScrollBar()->setStyleSheet(ListWidgetStyle());
    ui->localMusicWidget->verticalScrollBar()->setStyleSheet(ListWidgetStyle());
    ui->favorMusicWidget->verticalScrollBar()->setStyleSheet(ListWidgetStyle());
    ui->nameListWidget->verticalScrollBar()->setStyleSheet(ListWidgetStyle());
    ui->musicListWidget->verticalScrollBar()->setStyleSheet(ListWidgetStyle());
    ui->playListWidget->setIcon(QIcon(":/image/image/image/music.png"));
    ui->localMusicWidget->setIcon(QIcon(":/image/image/image/music-file.png"));
    ui->favorMusicWidget->setIcon(QIcon(":/image/image/image/like.png"));
    ui->musicListWidget->setIcon(QIcon(":/image/image/image/MusicListItem.png"));
}

void MainWidget::init_play()
{
    player= new QMediaPlayer(this);
    playlist=new QMediaPlaylist;    
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playlist);

    connect(ui->positionSlider, &QAbstractSlider::valueChanged, this, &MainWidget::setPosition);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWidget::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWidget::updateDuration);
    connect(player, &QMediaPlayer::metaDataAvailableChanged, this, &MainWidget::updateInfo);
    connect(player, &QMediaPlayer::stateChanged, this, &MainWidget::updatePlayBtn);
}

void MainWidget::init_actions()
{
    // Initialize context menu for "Now Playing" list
    ui->playListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *action_playlist_delete=new QAction(QIcon(":/image/image/image/remove.png"),"Remove");
    connect(action_playlist_delete,&QAction::triggered,this,&MainWidget::playlist_removeMusic);
    QAction *action_playlist_showfile=new QAction(QIcon(":/image/image/image/music-dir.png"),"Open Containing Folder");
    connect(action_playlist_showfile,&QAction::triggered,ui->playListWidget,&MusicListWidget::showInExplorer);
    QAction *action_playlist_detail=new QAction(QIcon(":/image/image/image/detail.png"),"Song Details");
    connect(action_playlist_detail,&QAction::triggered,ui->playListWidget,&MusicListWidget::detail);
    QAction *action_play_to_favor=new QAction(QIcon(":/image/image/image/To-like.png"),"Add to Favorites");
    connect(action_play_to_favor,&QAction::triggered,this,&MainWidget::play_to_favor);
    menu_playlist=new QMenu(this);
    menu_playlist->addAction(action_playlist_delete);
    menu_playlist->addAction(action_playlist_showfile);
    menu_playlist->addAction(action_playlist_detail);
    menu_playlist->addAction(action_play_to_favor);
    
    // Initialize context menu for "Local Music" list
    ui->localMusicWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *action_locallist_delete=new QAction(QIcon(":/image/image/image/remove.png"),"Remove");
    connect(action_locallist_delete,&QAction::triggered,ui->localMusicWidget,&MusicListWidget::removeMusic);
    QAction *action_locallist_showfile=new QAction(QIcon(":/image/image/image/music-dir.png"),"Open Containing Folder");
    connect(action_locallist_showfile,&QAction::triggered,ui->localMusicWidget,&MusicListWidget::showInExplorer);
    QAction *action_locallist_detail=new QAction(QIcon(":/image/image/image/detail.png"),"Song Details");
    connect(action_locallist_detail,&QAction::triggered,ui->localMusicWidget,&MusicListWidget::detail);
    QAction *action_local_to_favor=new QAction(QIcon(":/image/image/image/To-like.png"),"Add to Favorites");
    connect(action_local_to_favor,&QAction::triggered,this,&MainWidget::local_to_favor);
    QAction *action_local_to_playlist=new QAction(QIcon(":/image/image/image/To-playlist.png"),"Add to Current Playlist");
    connect(action_local_to_playlist,&QAction::triggered,this,&MainWidget::local_to_playlist);
    menu_locallist=new QMenu(this);
    menu_locallist->addAction(action_locallist_delete);
    menu_locallist->addAction(action_locallist_showfile);
    menu_locallist->addAction(action_locallist_detail);
    menu_locallist->addAction(action_local_to_favor);
    menu_locallist->addAction(action_local_to_playlist);
    
    // Initialize context menu for "Favorites" list
    ui->favorMusicWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *action_favorlist_delete = new QAction(QIcon(":/image/image/image/remove.png"),"Remove");
	connect(action_favorlist_delete, &QAction::triggered, ui->favorMusicWidget, &MusicListWidget::removeMusic);
    QAction *action_favorlist_showfile = new QAction(QIcon(":/image/image/image/music-dir.png"),"Open Containing Folder");
	connect(action_favorlist_showfile, &QAction::triggered, ui->favorMusicWidget, &MusicListWidget::showInExplorer);
    QAction *action_favorlist_detail = new QAction(QIcon(":/image/image/image/detail.png"),"Song Details");
	connect(action_favorlist_detail, &QAction::triggered, ui->favorMusicWidget, &MusicListWidget::detail);
    QAction *action_favor_to_playlist=new QAction(QIcon(":/image/image/image/To-playlist.png"),"Add to Current Playlist");
    connect(action_favor_to_playlist,&QAction::triggered,this,&MainWidget::favor_to_playlist);
	menu_favorlist = new QMenu(this);
	menu_favorlist->addAction(action_favorlist_delete);
	menu_favorlist->addAction(action_favorlist_showfile);
	menu_favorlist->addAction(action_favorlist_detail);
    menu_favorlist->addAction(action_favor_to_playlist);
    
    // Initialize context menu for "My Playlists" list
    ui->nameListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *action_namelist_delete=new QAction(QIcon(":/image/image/image/remove.png"),"Remove");
    connect(action_namelist_delete,&QAction::triggered,this,&MainWidget::namelist_delete);
    menu_namelist=new QMenu(this);
    menu_namelist->addAction(action_namelist_delete);
    
    // Initialize context menu for playlist display list
    ui->musicListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *action_musiclist_delete = new QAction(QIcon(":/image/image/image/remove.png"), "Remove");
    connect(action_musiclist_delete, &QAction::triggered, this, &MainWidget::musiclist_removeMusic);
    QAction *action_musiclist_showfile = new QAction(QIcon(":/image/image/image/music-dir.png"), "Open Containing Folder");
    connect(action_musiclist_showfile, &QAction::triggered, ui->musicListWidget, &MusicListWidget::showInExplorer);
    QAction *action_musiclist_detail = new QAction(QIcon(":/image/image/image/detail.png"), "Song Details");
    connect(action_musiclist_detail, &QAction::triggered, ui->musicListWidget, &MusicListWidget::detail);
    QAction *action_musiclist_to_favor = new QAction(QIcon(":/image/image/image/To-like.png"), "Add to Favorites");
    connect(action_musiclist_to_favor, &QAction::triggered, this, &MainWidget::musiclist_to_favor);
    QAction *action_musiclist_to_playlist = new QAction(QIcon(":/image/image/image/To-playlist.png"), "Add to Current Playlist");
    connect(action_musiclist_to_playlist, &QAction::triggered, this, &MainWidget::musiclist_to_playlist);
    menu_musiclist = new QMenu(this);
    menu_musiclist->addAction(action_musiclist_delete);
    menu_musiclist->addAction(action_musiclist_showfile);
    menu_musiclist->addAction(action_musiclist_detail);
    menu_musiclist->addAction(action_musiclist_to_favor);
    menu_musiclist->addAction(action_musiclist_to_playlist);

    // "Change Skin" menu items
    QAction *action_backgroud_to_default = new QAction(QIcon(":/image/image/image/default.png"),"Switch to Default Background");
	connect(action_backgroud_to_default,&QAction::triggered,this,&MainWidget::background_to_default);
    QAction *action_backgroud_setting=new QAction(QIcon(":/image/image/image/setting.png"),"Custom Background");
    connect(action_backgroud_setting,&QAction::triggered,this,&MainWidget::background_setting);
    menu_changeSkin=new QMenu(this);
    menu_changeSkin->addAction(action_backgroud_to_default);
    menu_changeSkin->addAction(action_backgroud_setting);
}

void MainWidget::init_sqlite()
{
    QSqlDatabase database;
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("Music.db");
        database.setUserName("NJUTang");
        database.setPassword("123456");
        if (!database.open())
        {
            QMessageBox::critical(this, "Unable to open database file: Music.db", database.lastError().databaseText());
            exit(-1);
        }
    }

    QSqlQuery query;
    query.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("MusicInfo"));
    if(query.next()){
        if(query.value(0).toInt()==0){
            QSqlQuery sql_query;
            //QString create_sql = "create table MusicInfo (id int primary key, name varchar(30), url varchar(200), author varchar(50), title varchar(50), duration bigint, albumTitle varchar(50), audioBitRate int)";
            QString create_sql = "create table MusicInfo (name varchar(30), url varchar(200), author varchar(50), title varchar(50), duration bigint, albumTitle varchar(50), audioBitRate int)";
            sql_query.prepare(create_sql);
            sql_query.exec();
        }        
    }
    QSqlQuery query2;
    query2.exec(QString("select count(*) from sqlite_master where type='table' and name='%1'").arg("MusicLists"));
    if(query2.next()){
        if(query2.value(0).toInt()==0){
            QSqlQuery sql_query;
            QString create_sql = "create table MusicLists (name varchar(30))";
            sql_query.prepare(create_sql);
            sql_query.exec();
        }        
    }
}

void MainWidget::init_musicList()
{

    ui->localMusicWidget->musicList.setName("LocalMusic");
    ui->localMusicWidget->musicList.read_fromSQL();
    ui->localMusicWidget->refresh();

    ui->favorMusicWidget->musicList.setName("FavorMusic");
    ui->favorMusicWidget->musicList.read_fromSQL();
    ui->favorMusicWidget->refresh();
    

    QSqlQuery sql_query;
    QString select_sql = "select name from MusicLists";
    sql_query.prepare(select_sql);
    if(sql_query.exec())
    {
        while(sql_query.next())
        {    
            QString musicListName=sql_query.value(0).toString();
            MusicList tempList;
            tempList.setName(musicListName);
            tempList.read_fromSQL();
            musiclist.push_back(tempList);
        }  
    }
    namelist_refresh();
}

void MainWidget::namelist_refresh()
{

    QSqlQuery sql_query;
    QString delete_sql = "delete from MusicLists";
    sql_query.prepare(delete_sql);
    sql_query.exec();
    for(size_t i=0;i<musiclist.size();i++){
        QSqlQuery sql_query2;
        QString insert_sql = "insert into MusicLists values (?)";
        sql_query2.prepare(insert_sql);
        sql_query2.addBindValue(musiclist[i].getName());
        sql_query2.exec();    
    }

    ui->nameListWidget->clear();
    for(size_t i=0;i<musiclist.size();i++){
        QListWidgetItem *item = new QListWidgetItem;
        item->setIcon(QIcon(":/image/image/image/music_list.png"));
        item->setText(musiclist[i].getName());
        ui->nameListWidget->addItem(item);  
    }
}

void MainWidget::init_settings()
{
    QSettings mysettings("./LightMusicPlayer.ini",QSettings::IniFormat);
    mysettings.setIniCodec("UTF8");
    QString fileName = mysettings.value("background/image-url").toString();
    QImage testImage(fileName);
    if(!fileName.isEmpty()&&!testImage.isNull())
    {
        setStyleSheet(QString("QWidget#Widget{"
                              "border-radius:10px;"
                              "border-image: url(%1);}").arg(fileName));
    }else{
        fileName=":/image/image/background/default.jpg";
        QSettings mysettings("./LightMusicPlayer.ini",QSettings::IniFormat);
        mysettings.setIniCodec("UTF8");
        mysettings.setValue("background/image-url",fileName);
        setStyleSheet(QString("QWidget#Widget{"
                              "border-radius:10px;"
                              "border-image: url(%1);}").arg(fileName));
    }
}

void MainWidget::musicListWidget_refresh()
{
    if(musiclist_index != -1){
        ui->musicListNameLabel->setText("Musiclist - "+musiclist[musiclist_index].getName());
        ui->musicListWidget->setMusicList_playing(musiclist[musiclist_index]);
    }
}

void MainWidget::on_playListWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->playListWidget->itemAt(pos)==Q_NULLPTR)
    {
        return;
    }
    menu_playlist->exec(QCursor::pos());
}

void MainWidget::playlist_removeMusic()
{
    int pos=ui->playListWidget->currentRow();
    int playing_pos=playlist->currentIndex();
    ui->playListWidget->removeMusic();
    if(pos<playing_pos){
        QMediaPlayer::State state=player->state();
        qint64 position_backup=player->position();
        playing_pos=playing_pos-1;
        playlist->removeMedia(pos);
        playlist->setCurrentIndex(playing_pos);
        player->setPosition(position_backup);
        ui->positionSlider->setValue(position_backup);
        if(state==QMediaPlayer::PlayingState){
            player->play();
        }else if(state==QMediaPlayer::PlayingState){
            player->pause();
        }
    }else if(pos<playing_pos){
        playlist->removeMedia(pos);
        playlist->setCurrentIndex(pos);
    }else{
        playlist->removeMedia(pos);
    }
}

void MainWidget::play_to_favor()
{
    int pos=ui->playListWidget->currentRow();
    ui->favorMusicWidget->musicList.addMusic(ui->playListWidget->musicList.getMusic(pos));
    ui->favorMusicWidget->refresh();
}

void MainWidget::local_to_favor()
{
    int pos=ui->localMusicWidget->currentRow();
    ui->favorMusicWidget->musicList.addMusic(ui->localMusicWidget->musicList.getMusic(pos));
    ui->favorMusicWidget->refresh();
}

void MainWidget::local_to_playlist()
{
    int pos=ui->localMusicWidget->currentRow();
    Music tempMusic=ui->localMusicWidget->musicList.getMusic(pos);
    ui->playListWidget->musicList.addMusic(tempMusic);
    ui->playListWidget->refresh();

    playlist->addMedia(tempMusic.getUrl());
}

void MainWidget::favor_to_playlist()
{
    int pos=ui->favorMusicWidget->currentRow();
    Music tempMusic=ui->favorMusicWidget->musicList.getMusic(pos);
    ui->playListWidget->musicList.addMusic(tempMusic);
    ui->playListWidget->refresh();

    playlist->addMedia(tempMusic.getUrl());
}

void MainWidget::namelist_delete()
{
    int pos=ui->nameListWidget->currentRow();
    musiclist[pos].remove_SQL_all();

    int i=0;
    for(auto it=musiclist.begin();it!=musiclist.end();){
        if(i==pos){
            it= musiclist.erase(it);
            break;
        }
        else{
            it++;
        }
        i++;
    }
    namelist_refresh();
}

void MainWidget::musiclist_removeMusic()
{
    int pos=ui->musicListWidget->currentRow();
    musiclist[musiclist_index].removeMusic(pos);
    musicListWidget_refresh();
}

void MainWidget::musiclist_to_favor()
{
    int pos=ui->musicListWidget->currentRow();
    ui->favorMusicWidget->musicList.addMusic(musiclist[musiclist_index].getMusic(pos));
    ui->favorMusicWidget->refresh();
}

void MainWidget::musiclist_to_playlist()
{
    int pos=ui->musicListWidget->currentRow();
    Music tempMusic=ui->musicListWidget->musicList.getMusic(pos);
    ui->playListWidget->musicList.addMusic(tempMusic);
    ui->playListWidget->refresh();

    playlist->addMedia(tempMusic.getUrl());
}

void MainWidget::background_to_default()
{
    QString fileName=":/image/image/background/default.jpg";
    QSettings mysettings("./LightMusicPlayer.ini",QSettings::IniFormat);
    mysettings.setIniCodec("UTF8");
    mysettings.setValue("background/image-url",fileName);
    setStyleSheet(QString("QWidget#Widget{"
                          "border-radius:10px;"
                          "border-image: url(%1);}").arg(fileName));
}

void MainWidget::background_setting()
{

    QString fileName=QFileDialog::getOpenFileName(this,("Select Custom Background Image"),QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first(),u8"图片文件(*jpg *png)");
    if(!fileName.isEmpty())
    {
        QImage testImage(fileName);
        if(!testImage.isNull()){
            QSettings mysettings("./LightMusicPlayer.ini",QSettings::IniFormat);
            mysettings.setIniCodec("UTF8");
            mysettings.setValue("background/image-url",fileName);
            setStyleSheet(QString("QWidget#Widget{"
                                  "border-radius:10px;"
                                  "border-image: url(%1);}").arg(fileName));    
        }
    }
}



void MainWidget::on_localMusicWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->localMusicWidget->itemAt(pos)==Q_NULLPTR)
    {
        return;
    }
    menu_locallist->exec(QCursor::pos());
}

void MainWidget::on_favorMusicWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->favorMusicWidget->itemAt(pos)==Q_NULLPTR)
    {
        return;
    }
    menu_favorlist->exec(QCursor::pos());
}

void MainWidget::on_nameListWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->nameListWidget->itemAt(pos)==Q_NULLPTR)
    {
        return;
    }
    menu_namelist->exec(QCursor::pos());
}


QString formatTime(qint64 timeMilliSeconds)
{
    qint64 seconds = timeMilliSeconds / 1000;
    const qint64 minutes = seconds / 60;
    seconds -= minutes * 60;
    return QStringLiteral("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

void MainWidget::updatePosition(qint64 position)
{
    ui->positionSlider->setValue(static_cast<int>(position));
    ui->positionLabel->setText(formatTime(position)+"/"+formatTime(player->duration()));
    if(playlist->currentIndex()>=0)ui->lyricWidget->show(position);
}

void MainWidget::updateDuration(qint64 duration)
{
    ui->positionSlider->setRange(0, static_cast<int>(duration));
    ui->positionSlider->setEnabled(static_cast<int>(duration) > 0);
    if(!(static_cast<int>(duration) > 0)) {

        ui->infoLabel->setText("KEEP CALM AND CARRY ON ...");
        mySystemTray->setToolTip("IceSonic - Music Player By Poppy Aniya");
        QImage image(":/image/image/image/non-music.png");
        ui->coverLabel->setPixmap(QPixmap::fromImage(image));
        ui->musicTitleLabel->setText("");
        ui->musicAlbumLabel->setText("");
        ui->musicAuthorLabel->setText("");
        ui->lyricWidget->clear();
    }
    ui->positionSlider->setPageStep(static_cast<int>(duration) / 10);
}

void MainWidget::setPosition(int position)
{
    // avoid seeking when the slider value change is triggered from updatePosition()
    if (qAbs(player->position() - position) > 99)
        player->setPosition(position);
}

void MainWidget::updateInfo()
{
    if (player->isMetaDataAvailable()) {
        // Return the available MP3 metadata list (can be viewed during debugging)
        QStringList listInfo_debug=player->availableMetaData();
        // Music Info
        QString info="";
        QString author = player->metaData(QStringLiteral("Author")).toStringList().join(",");
        info.append(author);
        QString title = player->metaData(QStringLiteral("Title")).toString();
        QString albumTitle = player->metaData(QStringLiteral("AlbumTitle")).toString();
        info.append(" - "+title);
        info.append(" ["+formatTime(player->duration())+"]");
        ui->infoLabel->setText(info);
        mySystemTray->setToolTip("Now playing:"+info);
        // ThumbnailImage
        QImage picImage= player->metaData(QStringLiteral("ThumbnailImage")).value<QImage>();
        if(picImage.isNull()) picImage=QImage(":/image/image/image/non-music.png");
        ui->coverLabel->setPixmap(QPixmap::fromImage(picImage));
        ui->coverLabel->setScaledContents(true);
        // Change the icon of the currently playing song
        for(int i=0;i<playlist->mediaCount();i++){
            QListWidgetItem *p=ui->playListWidget->item(i);
            p->setIcon(ui->playListWidget->getIcon());
        }
        int index=playlist->currentIndex();
        QListWidgetItem *p=ui->playListWidget->item(index);
        p->setIcon(QIcon(":/image/image/image/music-playing.png"));
        

        ui->musicTitleLabel->setText(title);
        ui->musicAlbumLabel->setText("Album:"+albumTitle);
        ui->musicAuthorLabel->setText("Artist:"+author);
        // Parse Lyrics
        ui->lyricWidget->process(ui->playListWidget->musicList.music[index].getLyricFile());
    }
}

void MainWidget::updatePlayBtn()
{
    if(player->state()==QMediaPlayer::PlayingState)
    {
        ui->btnPlay->setStyleSheet(PlayStyle());
        action_systemTray_play->setIcon(QIcon(":/image/image/image/pause2.png"));
        action_systemTray_play->setText(u8"Pause");
    }else{
        ui->btnPlay->setStyleSheet(PaseStyle());
        action_systemTray_play->setIcon(QIcon(":/image/image/image/play2.png"));
        action_systemTray_play->setText("Play");
    }
}

void MainWidget::systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::DoubleClick:
        if(isHidden()){
            show();
        }else{
            hide();
        }
        break;
    default:
        break;
    }
}

void MainWidget::quitMusicPlayer()
{
    QCoreApplication::quit();
}

void MainWidget::init_systemTrayIcon()
{
    mySystemTray=new QSystemTrayIcon(this);
    mySystemTray->setIcon(QIcon(":/image/image/image/systemTrayIcon.png"));
    mySystemTray->setToolTip(u8"LightMusicPlayer · By Poppy Aniya");
    connect(mySystemTray,&QSystemTrayIcon::activated,this,&MainWidget::systemTrayIcon_activated);

    QAction *action_systemTray_pre = new QAction(QIcon(":/image/image/image/pre2.png"), "Previous Track");
    connect(action_systemTray_pre, &QAction::triggered, this, &MainWidget::on_btnPre_clicked);
    action_systemTray_play = new QAction(QIcon(":/image/image/image/play2.png"), "Play");
    connect(action_systemTray_play, &QAction::triggered, this, &MainWidget::on_btnPlay_clicked);
    QAction *action_systemTray_next = new QAction(QIcon(":/image/image/image/next2.png"), "Next Track");
    connect(action_systemTray_next, &QAction::triggered, this, &MainWidget::on_btnNext_clicked);
    action_systemTray_playmode = new QAction(QIcon(":/image/image/image/loop2.png"), "Repeat All");
    connect(action_systemTray_playmode, &QAction::triggered, this, &MainWidget::on_btnPlayMode_clicked);
    QAction *action_systemTray_quit = new QAction(QIcon(":/image/image/image/exit.png"), "Exit");
    connect(action_systemTray_quit, &QAction::triggered, this, &MainWidget::quitMusicPlayer);
    
    QMenu *pContextMenu = new QMenu(this);
    pContextMenu->addAction(action_systemTray_pre);
    pContextMenu->addAction(action_systemTray_play);
    pContextMenu->addAction(action_systemTray_next);
    pContextMenu->addAction(action_systemTray_playmode);
    pContextMenu->addAction(action_systemTray_quit);
    mySystemTray->setContextMenu(pContextMenu);
    mySystemTray->show();
}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    int x=event->pos().x();
    int y=event->pos().y();
    int x_widget=ui->volumeSlider->geometry().x();
    int y_widget=ui->volumeSlider->geometry().y();
    int w=ui->volumeSlider->geometry().width();
    int h=ui->volumeSlider->geometry().height();
    if(!(x>=x_widget&&x<=x_widget+w && y>=y_widget&&y<=y_widget+h)){
        ui->volumeSlider->hide();
    }
    
    offset = event->globalPos() - pos();
    event->accept();
}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{
    int x=event->pos().x();
    int y=event->pos().y();
    // Note: The name "layoutWidget1" can be found in the ui_xxx.h file generated after compiling the UI file (it cannot be seen directly in the UI layout)
    if((y<ui->titleLabel->geometry().height())&&(x<ui->layoutWidget1->geometry().x())){
        move(event->globalPos() - offset);
        event->accept();    
        setCursor(Qt::ClosedHandCursor);
    }
}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{
    offset = QPoint();
    event->accept();
    setCursor(Qt::ArrowCursor);
}

void MainWidget::closeEvent(QCloseEvent *event)
{

    if(!mySystemTray->isVisible()){
        mySystemTray->show();
    }
    hide();
    event->ignore();
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWidget::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    ui->localMusicWidget->musicList.addMusic(urls);
    ui->localMusicWidget->refresh();
    ui->stackedWidget->setCurrentIndex(1);
    
}
void MainWidget::on_btnQuit_clicked()
{
    close();
}

void MainWidget::on_btnPlay_clicked()
{
    if(player->state()==QMediaPlayer::PlayingState)
     {
        player->pause();

     }
    else if(player->state()==QMediaPlayer::PausedState){

        player->play();
    }
    else if(!playlist->isEmpty() && (player->state()==QMediaPlayer::StoppedState))
    {
        playlist->setCurrentIndex(0);
        player->play();
    }
}

void MainWidget::on_btnNext_clicked()
{
    playlist->next();
}

void MainWidget::on_btnPre_clicked()
{
    playlist->previous();
}

void MainWidget::on_btnPlayMode_clicked()
{
    if(playlist->playbackMode()==QMediaPlaylist::Loop){
        ui->btnPlayMode->setStyleSheet(RandomStyle());
        ui->btnPlayMode->setToolTip("Shuffle Play");
        action_systemTray_playmode->setIcon(QIcon(":/image/image/image/random2.png"));
        action_systemTray_playmode->setText("Shuffle Play");
        playlist->setPlaybackMode(QMediaPlaylist::Random);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::Random){
        ui->btnPlayMode->setStyleSheet(LoopOneStyle());
        ui->btnPlayMode->setToolTip("Repeat One");
        action_systemTray_playmode->setIcon(QIcon(":/image/image/image/loop-one2.png"));
        action_systemTray_playmode->setText("Repeat One");
        playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
    }
    else if(playlist->playbackMode()==QMediaPlaylist::CurrentItemInLoop){
        ui->btnPlayMode->setStyleSheet(LoopStyle());
        ui->btnPlayMode->setToolTip("Repeat All");
        action_systemTray_playmode->setIcon(QIcon(":/image/image/image/loop2.png"));
        action_systemTray_playmode->setText("Repeat All");
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    }
}

void MainWidget::on_btnMin_clicked()
{
    showMinimized();
}

void MainWidget::on_btnAdd_clicked()
{    
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFiles);
    fileDialog.setWindowTitle(tr("Add Local Music (Note: Automatic filtering; press \"Ctrl+A\" to select all and add; folders are not supported)"));
    QStringList list;list<<"application/octet-stream";
    fileDialog.setMimeTypeFilters(list);
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted){
       QList<QUrl> urls=fileDialog.selectedUrls();
       ui->localMusicWidget->musicList.addMusic(urls);
       ui->localMusicWidget->refresh();
       ui->stackedWidget->setCurrentIndex(1);
    }
}


void MainWidget::on_btnVolume_clicked()
{
    if(ui->volumeSlider->isHidden()){
        ui->volumeSlider->show();
    }else{
        ui->volumeSlider->hide();
    }
}

void MainWidget::on_volumeSlider_valueChanged(int value)
{
    player->setVolume(value);
}

void MainWidget::on_btnAddMusicList_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "New Playlist", "Please enter the name of the new playlist:", QLineEdit::Normal, "", &ok);
    if(ok && !text.isEmpty()){
        MusicList tempMusic;
        tempMusic.setName(text);
        musiclist.push_back(tempMusic);
        namelist_refresh();
    }
}


void MainWidget::on_btnCurMusic_clicked()
{

    ui->stackedWidget->setCurrentIndex(0);
}
void MainWidget::on_btnLocalMusic_clicked()
{

    ui->stackedWidget->setCurrentIndex(1);
}
void MainWidget::on_btnFavorMusic_clicked()
{

    ui->stackedWidget->setCurrentIndex(2);
}

void MainWidget::on_playListWidget_doubleClicked(const QModelIndex &index)
{
    int i=index.row();
    playlist->setCurrentIndex(i);
    player->play();
}
void MainWidget::on_localMusicWidget_doubleClicked(const QModelIndex &index)
{
    playlist->clear();
    ui->localMusicWidget->musicList.addToPlayList(playlist);
    ui->playListWidget->setMusicList_playing(ui->localMusicWidget->musicList);
    int i=index.row();
    playlist->setCurrentIndex(i);
    player->play();
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWidget::on_favorMusicWidget_doubleClicked(const QModelIndex &index)
{
    playlist->clear();
    ui->favorMusicWidget->musicList.addToPlayList(playlist);
    ui->playListWidget->setMusicList_playing(ui->favorMusicWidget->musicList);
    int i=index.row();
    playlist->setCurrentIndex(i);
    player->play();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::on_nameListWidget_doubleClicked(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(3);
    musiclist_index=index.row();
    musicListWidget_refresh();
}

void MainWidget::on_btnSkin_clicked()
{
    menu_changeSkin->exec(QCursor::pos());
}

void MainWidget::on_btnAddtoMusicList_clicked()
{
    MusicListDialog *dialog=new MusicListDialog(this);
    int num=ui->localMusicWidget->count();
    bool *results=new bool[num];
    dialog->setMusicList(ui->localMusicWidget->musicList,results);
    if(dialog->exec()==QDialog::Accepted){
        for(int i=0;i<num;i++){
            if(results[i]){
                musiclist[musiclist_index].addMusic(ui->localMusicWidget->musicList.getMusic(i));
            }
        }
        musicListWidget_refresh();
    }
    delete []results;
}

void MainWidget::on_musicListWidget_doubleClicked(const QModelIndex &index)
{
    playlist->clear();
    musiclist[musiclist_index].addToPlayList(playlist);
    ui->playListWidget->setMusicList_playing(musiclist[musiclist_index]);
    int i=index.row();
    playlist->setCurrentIndex(i);
    player->play();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWidget::on_musicListWidget_customContextMenuRequested(const QPoint &pos)
{
    if(ui->musicListWidget->itemAt(pos)==Q_NULLPTR)
    {
        return;
    }
    menu_musiclist->exec(QCursor::pos());
}

void MainWidget::on_btnAddtoFavor_clicked()
{
    MusicListDialog *dialog=new MusicListDialog(this);
    int num=ui->localMusicWidget->count();
    bool *results=new bool[num];
    dialog->setMusicList(ui->localMusicWidget->musicList,results);
    if(dialog->exec()==QDialog::Accepted){
        for(int i=0;i<num;i++){
            if(results[i]){
                ui->favorMusicWidget->musicList.addMusic(ui->localMusicWidget->musicList.getMusic(i));
            }
        }
        ui->favorMusicWidget->refresh();
    }
    delete []results;
}

void MainWidget::on_btnSortSinger_clicked()
{
    ui->localMusicWidget->musicList.sort_by(AUTHOR);
    ui->localMusicWidget->refresh();
}
void MainWidget::on_btnSortTitle_clicked()
{
    ui->localMusicWidget->musicList.sort_by(TITLE);
    ui->localMusicWidget->refresh();
}
void MainWidget::on_btnSortDuration_clicked()
{
    ui->localMusicWidget->musicList.sort_by(DURATION);
    ui->localMusicWidget->refresh();
}

void MainWidget::on_btnSortSinger_2_clicked()
{
    ui->favorMusicWidget->musicList.sort_by(AUTHOR);
    ui->favorMusicWidget->refresh();
}
void MainWidget::on_btnSortTitle_2_clicked()
{
    ui->favorMusicWidget->musicList.sort_by(TITLE);
    ui->favorMusicWidget->refresh();
}
void MainWidget::on_btnSortDuration_2_clicked()
{
    ui->favorMusicWidget->musicList.sort_by(DURATION);
    ui->favorMusicWidget->refresh();
}

void MainWidget::on_btnSortSinger_4_clicked()
{
    musiclist[musiclist_index].sort_by(AUTHOR);
    musicListWidget_refresh();
}
void MainWidget::on_btnSortTitle_4_clicked()
{
    musiclist[musiclist_index].sort_by(TITLE);
    musicListWidget_refresh();
}
void MainWidget::on_btnSortDuration_4_clicked()
{
    musiclist[musiclist_index].sort_by(DURATION);
    musicListWidget_refresh();
}

void MainWidget::on_btnNeaten_clicked()
{
    ui->localMusicWidget->musicList.neaten();
    ui->localMusicWidget->refresh();
}
void MainWidget::on_btnNeaten_2_clicked()
{
    ui->favorMusicWidget->musicList.neaten();
    ui->favorMusicWidget->refresh();
}
void MainWidget::on_btnNeaten_3_clicked()
{
    musiclist[musiclist_index].neaten();
    musicListWidget_refresh();
}

void MainWidget::on_btnTitle_clicked()
{
    on_btnAbout_clicked();
}

void MainWidget::on_btnLyric_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void MainWidget::on_btnClear_clicked()
{
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Notice", "This action is irreversible!\nAre you sure you want to clear it?", QMessageBox::Yes|QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        ui->playListWidget->musicList.clear();
        ui->playListWidget->refresh();
        playlist->clear();
    }
}

void MainWidget::on_btnClear_2_clicked()
{
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Notice", "This action is irreversible!\nAre you sure you want to clear it?", QMessageBox::Yes|QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        ui->localMusicWidget->musicList.clear();
        ui->localMusicWidget->refresh();
    }
}

void MainWidget::on_btnClear_3_clicked()
{
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Notice", "This action is irreversible!\nAre you sure you want to clear it?", QMessageBox::Yes|QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        ui->favorMusicWidget->musicList.clear();
        ui->favorMusicWidget->refresh();
    }
}

void MainWidget::on_btnClear_4_clicked()
{
    QMessageBox::StandardButton btn;
    btn = QMessageBox::question(this, "Notice", "This action is irreversible!\nAre you sure you want to clear it?", QMessageBox::Yes|QMessageBox::No);
    if (btn == QMessageBox::Yes) {
        musiclist[musiclist_index].clear();
        musicListWidget_refresh();
    }
}

void MainWidget::on_btnAbout_clicked()
{
    QMessageBox::about(this, "About", "La La Land | A delicate and compact music player\n"
                                       "Author: Poppy Lin\n"
                                       "\n[Lyric File Instructions] The lyric file should be in the same directory and have the same name\n"
                                       "[Play/Pause]  -  Spacebar\n"
                                       "[Previous/Next Track]  -  Alt + Arrow Keys ←/→\n"
                                       "[Add Local Music]  - You can either drag and drop music files into the software interface or click the add button(Ctrl + O)\n"
                                       "[Supported Music File Types]  - During the addition process, playable file types (.mp3/.flac/.mpga) will be automatically filtered\n"
                                       "\nNote: Hover your mouse over unfamiliar buttons to see descriptions!\n");
}
