#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMenu>
#include <QSystemTrayIcon>
#include "MusicList.h"
#include "LyricWidget.h"
#include <QCloseEvent>


namespace Ui {
class Widget;
}

class MainWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget() override;
    
private:
    Ui::Widget *ui;
    void paintEvent(QPaintEvent *event) override;
    
    // Additional UI setup
    void init_UI();
    
    // Current media player
    QMediaPlayer *player;
    // Current playlist
    QMediaPlaylist *playlist;
    // Initialize some member variables and connect signals and slots
    void init_play();
    // Update playback information (progress, info, etc.)
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void setPosition(int position);
    void updateInfo();
    void updatePlayBtn();
    
    // System tray icon
    QSystemTrayIcon *mySystemTray;
    QAction *action_systemTray_play;
    QAction *action_systemTray_playmode;
    // Handle system tray actions (e.g., double click)
    void systemTrayIcon_activated(QSystemTrayIcon::ActivationReason reason);
    // Quit the music player
    void quitMusicPlayer();
    // Initialize the system tray icon
    void init_systemTrayIcon();
    
    // Initialize the database
    void init_sqlite();
    // Initialize settings (read from configuration file)
    void init_settings();
    // Initialize playlists such as "Local Music" and "Favorites"
    void init_musicList();
    
    // Playlists
    vector<MusicList> musiclist;
    // Index to identify which playlist is currently being displayed
    int musiclist_index=-1;
    // Update the list widget displaying the playlist names
    void namelist_refresh();
    // Update the list widget displaying the playlist content
    void musicListWidget_refresh();   
    
     /* Context menus */
    // Initialize menu items
    void init_actions();
    // Context menu for the current playlist
    QMenu *menu_playlist;
    // Context menu for the local music list
    QMenu *menu_locallist;
    // Menu for changing the favorites list
    QMenu *menu_favorlist;
    // Menu for changing the playlist name list
    QMenu *menu_namelist;
    // Menu for changing  the playlist display list
    QMenu *menu_musiclist;
    // Menu for changing the skin
    QMenu *menu_changeSkin; 

protected:
    // The starting point recorded when dragging the window
    QPoint offset;
    /* Override some methods of QWidget */
    // Enable window dragging
    void mousePressEvent(QMouseEvent *event) override; 
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // Minimize to system tray instead of exiting when closing the window
    void closeEvent(QCloseEvent *event) override;
     // Handle file dragging into the window
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    /* Some operations corresponding to context menu items (i.e., slots connected to QAction) */
    void playlist_removeMusic();
    void play_to_favor();
    void local_to_favor();
    void local_to_playlist();
    void favor_to_playlist();
    void namelist_delete();
    void musiclist_removeMusic();
    void musiclist_to_favor();
    void musiclist_to_playlist();
    void background_to_default();
    void background_setting();
    
    /* Response to some click events (automatically generated via "Go to Slot" for .ui components) */
    void on_btnCurMusic_clicked();
    void on_btnLocalMusic_clicked();
    void on_btnFavorMusic_clicked();
    void on_btnQuit_clicked();    
    void on_btnMin_clicked();    
    void on_btnPlay_clicked();
    void on_btnNext_clicked();
    void on_btnPre_clicked();
    void on_btnPlayMode_clicked();
    void on_btnAdd_clicked();
    void on_btnVolume_clicked();
    void on_volumeSlider_valueChanged(int value);
    void on_btnAddMusicList_clicked();
    void on_playListWidget_doubleClicked(const QModelIndex &index);
    void on_localMusicWidget_doubleClicked(const QModelIndex &index);
    void on_favorMusicWidget_doubleClicked(const QModelIndex &index);
    void on_playListWidget_customContextMenuRequested(const QPoint &pos);
    void on_localMusicWidget_customContextMenuRequested(const QPoint &pos);
    void on_favorMusicWidget_customContextMenuRequested(const QPoint &pos);
    void on_nameListWidget_customContextMenuRequested(const QPoint &pos);
    void on_nameListWidget_doubleClicked(const QModelIndex &index);
    void on_btnSkin_clicked();
    void on_btnAddtoMusicList_clicked();
    void on_musicListWidget_doubleClicked(const QModelIndex &index);
    void on_musicListWidget_customContextMenuRequested(const QPoint &pos);
    void on_btnAddtoFavor_clicked();
    void on_btnNeaten_clicked();
    void on_btnNeaten_2_clicked();
    void on_btnNeaten_3_clicked();
    void on_btnTitle_clicked();
    void on_btnLyric_clicked();
    void on_btnClear_clicked();
    void on_btnClear_2_clicked();
    void on_btnClear_3_clicked();
    void on_btnClear_4_clicked();
    void on_btnAbout_clicked();
    void on_btnSortSinger_clicked();
    void on_btnSortTitle_clicked();
    void on_btnSortDuration_clicked();
    void on_btnSortSinger_2_clicked();
    void on_btnSortTitle_2_clicked();
    void on_btnSortDuration_2_clicked();
    void on_btnSortSinger_4_clicked();
    void on_btnSortTitle_4_clicked();
    void on_btnSortDuration_4_clicked();
};

#endif // MAINWIDGET_H
