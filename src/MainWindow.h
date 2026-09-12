//MainWindow.h
#pragma once
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include <QUrl>
#include <QLabel>
#include <QResizeEvent>
#include <QFileDialog>

#include "Utils.h"

namespace Ui {class MainWindow;}

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    protected:
        void resizeEvent(QResizeEvent *event) override;
        bool eventFilter(QObject *watched, QEvent *event) override;
    
    private slots:
        //shared:
        void onQuitPressed();

        //mainMenu:
        void onSettingsPressed();
        void onStartAppPressed();
        
        //settings:
        void adminCodeEntered(QString input);
        void backToMainMenu();

        //Main app:
        void clickToggled(bool checked);
        void drumsToggled(bool checked);
        void songToggled(bool checked);
        void playClicked();
        void pauseClicked();
        void restartClicked();
        void openLibraryClicked();
        void songDurationChanged(qint64 duration);
        void songPositionChanged(qint64 position);
        void seekSong(qint64 position);
        void seekSongFromInput();

        //Music library:
        void selectSongClicked();
        void addNewSongClicked();

        //Song Adder:
        QString getClickinput();
        QString getSongInput();
        QString getDrumsInput();
        QString getNameInput();
        void browseClickClicked();
        void browseDrumsClicked();
        void browseSongClicked();
        bool ignoringClick();
        bool ignoringSong();
        bool ignoringDrums();
        void addSongClicked();
        void validateClickPath(const QString &path);
        void validateSongPath(const QString &path);
        void validateDrumsPath(const QString &path);

    private:
        Ui::MainWindow *ui;

        QLabel *background;
        double bgRatio;

        QMediaPlayer *clickPlayer;
        QMediaPlayer *drumsPlayer;
        QMediaPlayer *songPlayer;
        QAudioOutput *clickOutput;
        QAudioOutput *drumsOutput;
        QAudioOutput *songOutput;

        bool playing = false;
        bool editingPosition = false;

        void resizeBackground();
        void setupLibrary(const bool load = false);
        Song selected;

        Song newSong;
};