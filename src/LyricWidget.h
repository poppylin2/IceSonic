#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include<QWidget>
#include<QLabel>
#include<QFile>
#include<vector>
using namespace std;

// Represents a line of lyrics (a timestamp + corresponding lyric text)
class LyricLine
{
public:
    qint64 time;
    QString text;
    LyricLine(qint64 time, QString text):time(time), text(text){}    
};

bool operator <(const LyricLine& A, const LyricLine& B);

namespace Ui {
class LyricWidget;
}

class LyricWidget : public QWidget
{
    Q_OBJECT
    
    // Stores all the lyrics
    vector<LyricLine> lines;    
public:
    explicit LyricWidget(QWidget *parent = nullptr);
    ~LyricWidget();

    // Process the content of the lyric file into a QList of lyric structures
    bool process(QString filePath); 
    // Find the corresponding lyric line based on the current time
    int getIndex(qint64 position);
    // Display the lyrics corresponding to the current playback progress
    void show(qint64 position);
    // Get the lyric content based on the index
    QString getLyricText(int index);
    // Clear the lyric label
    void clear();
    
private:
    Ui::LyricWidget *ui;
};

#endif // LYRICWIDGET_H
