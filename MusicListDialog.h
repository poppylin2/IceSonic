#ifndef MUSICLISTDIALOG_H
#define MUSICLISTDIALOG_H

#include <QDialog>
#include "MusicList.h"
#include "MusicListWidget.h"

namespace Ui {
class MusicListDialog;
}

class MusicListDialog : public QDialog
{
    Q_OBJECT
    // Number of songs
    int num; 
    // Stores whether each song item is selected (used to return the selection result)
    bool *selected_flag;
public:
    explicit MusicListDialog(QWidget *parent = nullptr);
    ~MusicListDialog();
    
    // Set the playlist to be displayed in the dialog and the place to store the selection results
    void setMusicList(MusicList& ilist, bool *results);
    
private slots:
    
    void on_buttonBox_accepted();
    
private:
    Ui::MusicListDialog *ui;
};

#endif // MUSICLISTDIALOG_H
