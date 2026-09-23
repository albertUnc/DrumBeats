//MainWindow.cpp
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QDir>
#include <QCheckBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSlider>
#include <QVBoxLayout>

#include "SETTINGS.h"
#include "Utils.h"
#include "saveTools.h"

static const string local_signature = "[MainWindow.cpp] ";

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, selected("", "", "", "")
, newSong("", "", "", "") {
    logs.setSignature(local_signature);
    logs.write("Started main window constructor.\n");

    ui->setupUi(this);

    auto *centralLayout = new QVBoxLayout(ui->centralwidget);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->addWidget(ui->screens);

    auto addPageLayout = [](QWidget *page, QWidget *contents) {
        auto *pageLayout = new QVBoxLayout(page);
        pageLayout->setContentsMargins(0, 0, 0, 0);
        pageLayout->addWidget(contents);

        if (contents->objectName() == "mainAppContents") return;

        auto *contentsLayout = qobject_cast<QVBoxLayout *>(contents->layout());
        if (!contentsLayout) return;

        for (int index = 0; index < contentsLayout->count(); ++index) {
            QWidget *widget = contentsLayout->itemAt(index)->widget();
            if (!widget) continue;

            contentsLayout->setAlignment(widget, Qt::AlignHCenter);
            if (qobject_cast<QLineEdit *>(widget)) {
                widget->setSizePolicy(QSizePolicy::Maximum, widget->sizePolicy().verticalPolicy());
            }
        }
    };
    addPageLayout(ui->mainMenu, ui->mainMenuContents);
    addPageLayout(ui->settings, ui->settingsContents);
    addPageLayout(ui->mainAppScreen, ui->mainAppContents);
    addPageLayout(ui->musicLibrary, ui->muscLibraryContents);
    addPageLayout(ui->songAdder, ui->songAdderContents);

    ui->gridLayout->removeWidget(ui->playPush);
    ui->gridLayout->removeWidget(ui->pausePush);
    ui->gridLayout->removeWidget(ui->restartPush);
    auto *playbackControls = new QWidget(ui->mainAppContents);
    auto *playbackLayout = new QHBoxLayout(playbackControls);
    playbackLayout->setContentsMargins(0, 0, 0, 0);
    playbackLayout->setSpacing(8);
    playbackLayout->addWidget(ui->playPush);
    playbackLayout->addWidget(ui->pausePush);
    playbackLayout->addWidget(ui->restartPush);
    ui->gridLayout->addWidget(playbackControls, 2, 3, 1, 3, Qt::AlignCenter);
    ui->playPush->setEnabled(false);
    ui->pausePush->setEnabled(false);
    ui->restartPush->setEnabled(false);

    ui->screens->setCurrentWidget(ui->mainMenu);
    logs.write("Done setupUi(this) and set current page to mainMenu.\n");

    //CONNECTS
    logs.write("Connecting all the user inputs to their functions...\n");

    //mainMenu
    logs.write("Connecting main menu inputs\n");
    connect(ui->mainSettingsPush, &QPushButton::clicked, this, &MainWindow::onSettingsPressed);
    connect(ui->menuQuitPush, &QPushButton::clicked, this, &MainWindow::onQuitPressed);
    connect(ui->startAppPush, &QPushButton::clicked, this, &MainWindow::onStartAppPressed);
    //settings
    logs.write("Connecting settings inputs\n");
    connect(ui->adminCode, &QLineEdit::textChanged, this, &MainWindow::adminCodeEntered);
    connect(ui->backToMenuPush, &QPushButton::clicked, this, &MainWindow::backToMainMenu);
    //main app
    logs.write("Connecting main app inputs\n");
    connect(ui->clickCheck, &QCheckBox::toggled, this, &MainWindow::clickToggled);
    connect(ui->drumsCheck, &QCheckBox::toggled, this, &MainWindow::drumsToggled);
    connect(ui->songCheck, &QCheckBox::toggled, this, &MainWindow::songToggled);
    connect(ui->playPush, &QPushButton::clicked, this, &MainWindow::playClicked);
    connect(ui->pausePush, &QPushButton::clicked, this, &MainWindow::pauseClicked);
    connect(ui->restartPush, &QPushButton::clicked, this, &MainWindow::restartClicked);
    connect(ui->openLibraryPush, &QPushButton::clicked, this, &MainWindow::openLibraryClicked);
    connect(ui->quitAppPush, &QPushButton::clicked, this, &MainWindow::onQuitPressed);
    connect(ui->backToMainPush, &QPushButton::clicked, this, &MainWindow::backToMainMenu);
    //musicLibrary
    logs.write("Connecting music library inputs\n");
    connect(ui->selectSongPush, &QPushButton::clicked, this, &MainWindow::selectSongClicked);
    connect(ui->songAdderPush, &QPushButton::clicked, this, &MainWindow::addNewSongClicked);
    connect(ui->libraryQuitPush, &QPushButton::clicked, this, &MainWindow::onQuitPressed);
    connect(ui->libraryBackToMainPush, &QPushButton::clicked, this, &MainWindow::backToMainMenu);
    //songAdder
    logs.write("Connecting song adder inuts\n");
    connect(ui->addSongButton, &QPushButton::clicked, this, &MainWindow::addSongClicked);
    connect(ui->browseClickTrackPush, &QPushButton::clicked, this, &MainWindow::browseClickClicked);
    connect(ui->browseDrumsTrackPush, &QPushButton::clicked, this, &MainWindow::browseDrumsClicked);
    connect(ui->browseSongTrackPush, &QPushButton::clicked, this, &MainWindow::browseSongClicked);
    connect(ui->clickTrackLine, &QLineEdit::textChanged, this, &MainWindow::validateClickPath);
    connect(ui->songTrackLine, &QLineEdit::textChanged, this, &MainWindow::validateSongPath);
    connect(ui->drumsTrackLine, &QLineEdit::textChanged, this, &MainWindow::validateDrumsPath);
    connect(ui->ignoreClickTrackCheck, &QCheckBox::toggled, this, [this]() {
        validateClickPath(ui->clickTrackLine->text());
    });
    connect(ui->ignoreDrumlessTrackCheck, &QCheckBox::toggled, this, [this]() {
        validateSongPath(ui->songTrackLine->text());
    });
    connect(ui->ignoreDrumsTrackCheck, &QCheckBox::toggled, this, [this]() {
        validateDrumsPath(ui->drumsTrackLine->text());
    });
    logs.write("Connected input sources to dedicated handlers.\n");

    //BACKGROUND
    logs.write("Setting up background...\n");
    background = new QLabel(ui->centralwidget);
    QPixmap bgPixmap(":/prefix/textures/snare-top.jpg");
    background->setPixmap(bgPixmap);
    background->setScaledContents(true); //rest will be handled in resizeEvent
    background->lower();
    bgRatio = double(bgPixmap.size().width()) / bgPixmap.size().height();
    if (!bgPixmap.isNull()) {
        logs.write(format("Successfully loaded background with ratio {}\n", bgRatio));
        resizeBackground();
    } else {
        logs.write("bgPixmap.isNull() == true. This means the background's texture could not be loaded succesfully.\n", messageType::Error);
    }

    //PLAYERS
    logs.write("Creating all the players for the pointers...\n");
    clickPlayer = new QMediaPlayer;
    drumsPlayer = new QMediaPlayer;
    songPlayer = new QMediaPlayer;
    logs.write("Creating audio output...\n");
    clickOutput = new QAudioOutput;
    drumsOutput = new QAudioOutput;
    songOutput = new QAudioOutput;
    clickPlayer->setAudioOutput(clickOutput);
    drumsPlayer->setAudioOutput(drumsOutput);
    songPlayer->setAudioOutput(songOutput);
    clickOutput->setVolume(1.0);
    drumsOutput->setVolume(1.0);
    songOutput->setVolume(1.0);
    connect(ui->clickVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
        clickOutput->setVolume(value / 100.0);
    });
    connect(ui->drumsVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
        drumsOutput->setVolume(value / 100.0);
    });
    connect(ui->drumlessVolumeSlider, &QSlider::valueChanged, this, [this](int value) {
        songOutput->setVolume(value / 100.0);
    });
    connect(songPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::songDurationChanged);
    connect(songPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::songPositionChanged);
    connect(ui->songSlider, &QSlider::sliderMoved, this, &MainWindow::seekSong);
    connect(ui->leftSideLabel, &QLineEdit::textEdited, this, [this]() {
        editingPosition = true;
    });
    connect(ui->leftSideLabel, &QLineEdit::editingFinished, this, &MainWindow::seekSongFromInput);
    ui->songSlider->installEventFilter(this);
    
    //RANDOM LABELS
    logs.write("Setting invalid path labels' visible value to false\n");
    ui->invalidClickPathLabel->setVisible(false);
    ui->invalidDrumsPathLabel->setVisible(false);
    ui->invalidSongPathLabel->setVisible(false);
}

MainWindow::~MainWindow() {
    logs.write("Destructor of MainWindow ran...Deleting ui\n");
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    if (!background) return;

    resizeBackground();
}

void MainWindow::resizeBackground() {
    if (!background || bgRatio <= 0) return;

    const double newW = ui->centralwidget->width();
    const double newH = ui->centralwidget->height();
    if (newW <= 0 || newH <= 0) return;

    const double windowRatio = newW / newH;

    logs.write(format("resizeEvent triggered. Details: width={}, height={}, windowRatio={}\n", newW, newH, windowRatio));

    //Background:
    //logs.write("Resizing background...\n");
    if (windowRatio > bgRatio) {
        //window's w/h bigger than background's. More like a laptop screen (horizontal)
        background->resize(newW, (newW / bgRatio));
    } else if (bgRatio > windowRatio) {
        //bg's w/h is bigger than window's, more like a phone screen (vertical)
        background->resize((newH * bgRatio), newH);
    } else { //If equal
        background->resize(newW, newH);
    }
    //Center
    double cutoffW = 0, cutoffH = 0, finalX = 0, finalY=0;
    cutoffW = newW - background->size().width();
    cutoffH = newH - background->size().height();
    finalX = cutoffW / 2;
    finalY = cutoffH / 2;
    //logs.write(format("Centering background with details: width cut off: {}, height cut off: {}. Final position: x={}, y={}\n", cutoffW, cutoffH, finalX, finalY));
    background->move(finalX, finalY);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->songSlider && event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            const qint64 duration = songPlayer->duration();
            const int sliderWidth = ui->songSlider->width();
            if (duration > 0 && sliderWidth > 0) {
                const int clickPosition = qBound(
                    0,
                    static_cast<int>(mouseEvent->position().x()),
                    sliderWidth
                );
                seekSong((static_cast<qint64>(clickPosition) * duration) / sliderWidth);
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

//HANDLERS]
//shared
void MainWindow::onQuitPressed() {
    logs.write("Quit pressed. Quitting app...\n");
    close();
}
//mainMenu
void MainWindow::onSettingsPressed() {
    logs.write("Settings button pressed. Opening settings...\n");
    ui->screens->setCurrentWidget(ui->settings);
}
void MainWindow::onStartAppPressed() {
    logs.write("Start app clicked. Starting main app screen...\n");
    logs.write("Switching background pixmap to the main app's background picture.\n");
    QPixmap newPixmap(":/prefix/textures/mainBG.jpg");
    if (newPixmap.isNull()) {
        logs.write("newPixmap.isNull() evaluated to true. Main app background texture could not be loaded.\n", messageType::Error);
    }
    background->setPixmap(newPixmap);
    bgRatio = double(newPixmap.width()) / newPixmap.height();
    resizeBackground();

    logs.write("Switching current screen...\n");
    ui->screens->setCurrentWidget(ui->mainAppScreen);
}
//settings
void MainWindow::adminCodeEntered(QString input) {
    logs.write(format("Admin code input changed. New text: {}\n", input.toStdString()));
    if (input.trimmed() == "0000reset_data") {
        platform::setAppName(NAME);
        const fs::path dataPath = platform::appDataPath() / SONGS_APPDATA_RELATIVE_PATH;
        fs::remove(dataPath);
        close();
    }
    if (input.trimmed() == "0001clear_log") {
        platform::setAppName(NAME);
        const fs::path logPath = platform::appDataPath() / LOG_APPDATA_RELATIVE_PATH;
        fs::remove(logPath);
        close();
    }
    if (input.trimmed() == "0002move_data") {
        platform::setAppName(NAME);

        const fs::path oldAppDataRoot = getExeDir() / "AppData";
        const fs::path oldLogPath = oldAppDataRoot / LOG_APPDATA_RELATIVE_PATH;
        const fs::path oldDataPath = oldAppDataRoot / SONGS_APPDATA_RELATIVE_PATH;

        const fs::path newLogPath = platform::appDataPath() / LOG_APPDATA_RELATIVE_PATH;
        const fs::path newDataPath = platform::appDataPath() / SONGS_APPDATA_RELATIVE_PATH;

        if (fs::exists(oldLogPath) && !fs::exists(newLogPath)) {
            fs::create_directories(newLogPath.parent_path());
            fs::rename(oldLogPath, newLogPath);
        }
        if (fs::exists(oldDataPath) && !fs::exists(newDataPath)) {
            fs::create_directories(newDataPath.parent_path());
            fs::rename(oldDataPath, newDataPath);
        }

        close();
    }
    if (input.trimmed() == "0003delete_old") {
        const fs::path oldAppDataRoot = getExeDir() / "AppData";
        if (fs::exists(oldAppDataRoot)) {
            fs::remove_all(oldAppDataRoot);
        }
        close();
    }
}
void MainWindow::backToMainMenu() {
    logs.write("Going back to main menu\n");
    ui->screens->setCurrentWidget(ui->mainMenu);
}
//main app
void MainWindow::clickToggled(bool checked) {
    clickOutput->setMuted(!checked);
}
void MainWindow::drumsToggled(bool checked) {
    drumsOutput->setMuted(!checked);
}
void MainWindow::songToggled(bool checked) {
    songOutput->setMuted(!checked);
}
void MainWindow::playClicked() {
    if (playing) return;
    playing = true;
    ui->restartPush->setEnabled(true);
    ui->pausePush->setText("Pause");
    clickPlayer->play();
    songPlayer->play();
    drumsPlayer->play();
}
void MainWindow::pauseClicked() {
    if (playing) {
        playing = false;
        ui->pausePush->setText("Resume");
        clickPlayer->pause();
        songPlayer->pause();
        drumsPlayer->pause();
        return;
    }
    //ELSE: Trigger play
    playClicked();
}
void MainWindow::restartClicked() {
    clickPlayer->setPosition(0);
    songPlayer->setPosition(0);
    drumsPlayer->setPosition(0);
    if (!playing) ui->restartPush->setEnabled(false);
    ui->pausePush->setText("Pause");
    if (!playing) ui->pausePush->setEnabled(false);
}
QString formatTrackTime(qint64 milliseconds) {
    const qint64 totalSeconds = qMax<qint64>(0, milliseconds) / 1000;
    const qint64 hours = totalSeconds / 3600;
    const qint64 minutes = (totalSeconds % 3600) / 60;
    const qint64 seconds = totalSeconds % 60;

    if (hours > 0) {
        return QString("%1:%2:%3")
            .arg(hours)
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }
    return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
}
void MainWindow::songDurationChanged(qint64 duration) {
    ui->songSlider->setRange(0, qMax<qint64>(0, duration));
    ui->songSlider->setValue(songPlayer->position());
    ui->rightSideLabel->setText(QString("-%1 | %2")
        .arg(formatTrackTime(duration - songPlayer->position()))
        .arg(formatTrackTime(duration)));
}
void MainWindow::songPositionChanged(qint64 position) {
    const qint64 duration = songPlayer->duration();
    ui->songSlider->setValue(position);
    if (!editingPosition) {
        ui->leftSideLabel->setText(formatTrackTime(position));
    }
    ui->rightSideLabel->setText(QString("-%1 | %2")
        .arg(formatTrackTime(duration - position))
        .arg(formatTrackTime(duration)));
}
void MainWindow::seekSong(qint64 position) {
    clickPlayer->setPosition(position);
    songPlayer->setPosition(position);
    drumsPlayer->setPosition(position);
}
void MainWindow::seekSongFromInput() {
    const QString input = ui->leftSideLabel->text().trimmed();
    const QStringList parts = input.split(':', Qt::KeepEmptyParts);
    qint64 minutes = 0;
    qint64 seconds = 0;
    bool valid = false;

    if (parts.size() == 1) {
        minutes = parts[0].toLongLong(&valid);
        valid = valid && minutes >= 0;
    } else if (parts.size() == 2) {
        bool minutesValid = false;
        bool secondsValid = false;
        minutes = parts[0].toLongLong(&minutesValid);
        seconds = parts[1].toLongLong(&secondsValid);
        valid = minutesValid && secondsValid
            && minutes >= 0
            && seconds >= 0
            && seconds < 60;
    }

    const qint64 duration = songPlayer->duration();
    if (!valid || duration <= 0) {
        editingPosition = false;
        ui->leftSideLabel->setText(formatTrackTime(songPlayer->position()));
        return;
    }

    const qint64 requestedPosition = (minutes * 60 + seconds) * 1000;
    const qint64 position = qBound<qint64>(0, requestedPosition, duration);
    editingPosition = false;
    seekSong(position);
    ui->leftSideLabel->setText(formatTrackTime(position));
}
void MainWindow::openLibraryClicked() {
    logs.write("Library opened\n");
    ui->screens->setCurrentWidget(ui->musicLibrary);
    setupLibrary(true);
}
//Library:
void MainWindow::setupLibrary(const bool load) {
    logs.write("Setting library screen up...\n");
    ui->selectorCombo->clear();
    if (load) {
        songs.clear();
        loadSongs();
    }
    QStringList options;
    for (const Song &s : songs) {
        options.append(QString::fromStdString(s.name));
    }
    ui->selectorCombo->addItems(options);
}
void MainWindow::selectSongClicked() {
    logs.write("Select song pressed\n");
    int sIndex = ui->selectorCombo->currentIndex();
    if (sIndex == -1) {
        logs.write("Nothing selected in selecotrCombo. Returning handler\n", messageType::Warn);
        return;
    }
    selected = songs.at(sIndex);
    const bool hasClick = !selected.clickPath.empty();
    const bool hasDrums = !selected.drumsPath.empty();
    const bool hasSong = !selected.songPath.empty();

    playing = false;
    editingPosition = false;
    clickPlayer->stop();
    drumsPlayer->stop();
    songPlayer->stop();
    clickPlayer->setSource(QUrl());
    drumsPlayer->setSource(QUrl());
    songPlayer->setSource(QUrl());
    ui->pausePush->setText("Pause");
    ui->songSlider->setRange(0, 0);
    ui->songSlider->setValue(0);
    ui->leftSideLabel->setText(formatTrackTime(0));
    ui->rightSideLabel->setText("-0:00 | 0:00");

    ui->playPush->setEnabled(hasSong);
    ui->pausePush->setEnabled(hasSong);
    ui->restartPush->setEnabled(hasSong);

    ui->clickCheck->setVisible(hasClick);
    ui->drumsCheck->setVisible(hasDrums);
    ui->songCheck->setVisible(hasSong);
    ui->clickVolumeSlider->setVisible(hasClick);
    ui->drumsVolumeSlider->setVisible(hasDrums);
    ui->drumlessVolumeSlider->setVisible(hasSong);
    ui->clickCheck->setChecked(hasClick);
    ui->drumsCheck->setChecked(hasDrums);
    ui->songCheck->setChecked(hasSong);

    if (hasClick) {
        clickPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(selected.clickPath)));
    }
    if (hasDrums) {
        drumsPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(selected.drumsPath)));
    }
    if (hasSong) {
        songPlayer->setSource(QUrl::fromLocalFile(QString::fromStdString(selected.songPath)));
    }
    ui->currentlyPlayingUpdateLabel->setText(QString::fromStdString(selected.name));
    logs.write("Song selected. Going back to main app...\n");
    ui->screens->setCurrentWidget(ui->mainAppScreen);
}
void MainWindow::addNewSongClicked() {
    logs.write("Add new song clicked, opening song adder screen\n");
    ui->screens->setCurrentWidget(ui->songAdder);
}
//songAdder
QString makePattern() {
    string pattern = "Music files (";
    for (string temp : supportedFormats) {
        pattern += "*" + temp + " ";
    }
    pattern += ")";
    return QString::fromStdString(pattern);
}
QString selectPath(string which) {
    QString title = QString::fromStdString(format("Select {} track", which));
    QString filePath = QFileDialog::getOpenFileName (
        nullptr,
        title,
        QDir::homePath(),
        makePattern()
    );
    return filePath;
}
void MainWindow::browseClickClicked() {
    string which = "click";
    QString absPath = selectPath(which);
    ui->clickTrackLine->setText(absPath);
}
void MainWindow::browseDrumsClicked() {
    string which = "seperated drums";
    QString absPath = selectPath(which);
    ui->drumsTrackLine->setText(absPath);
}
void MainWindow::browseSongClicked() {
    string which = "drumless song";
    QString absPath = selectPath(which);
    ui->songTrackLine->setText(absPath);
}
void MainWindow::validateClickPath(const QString &pathText) {
    if (ignoringClick()) {
        ui->invalidClickPathLabel->setVisible(false);
        return;
    }

    fs::path path(pathText.toStdString());
    bool valid = fs::exists(path)
        && std::find(std::begin(supportedFormats), std::end(supportedFormats), path.extension().string())
            != std::end(supportedFormats);
    ui->invalidClickPathLabel->setVisible(!valid);
}
void MainWindow::validateSongPath(const QString &pathText) {
    if (ignoringSong()) {
        ui->invalidSongPathLabel->setVisible(false);
        return;
    }

    fs::path path(pathText.toStdString());
    bool valid = fs::exists(path)
        && std::find(std::begin(supportedFormats), std::end(supportedFormats), path.extension().string())
            != std::end(supportedFormats);
    ui->invalidSongPathLabel->setVisible(!valid);
}
void MainWindow::validateDrumsPath(const QString &pathText) {
    if (ignoringDrums()) {
        ui->invalidDrumsPathLabel->setVisible(false);
        return;
    }

    fs::path path(pathText.toStdString());
    bool valid = fs::exists(path)
        && std::find(std::begin(supportedFormats), std::end(supportedFormats), path.extension().string())
            != std::end(supportedFormats);
    ui->invalidDrumsPathLabel->setVisible(!valid);
}
QString MainWindow::getClickinput() {
    return ui->clickTrackLine->text();
}
QString MainWindow::getSongInput() {
    return ui->songTrackLine->text();
}
QString MainWindow::getDrumsInput() {
    return ui->drumsTrackLine->text();
}
QString MainWindow::getNameInput() {
    return ui->lineEdit->text();
}
bool MainWindow::ignoringClick() {
    return ui->ignoreClickTrackCheck->isChecked();
}
bool MainWindow::ignoringSong() {
    return ui->ignoreDrumlessTrackCheck->isChecked();
}
bool MainWindow::ignoringDrums() {
    return ui->ignoreDrumsTrackCheck->isChecked();
}
void MainWindow::addSongClicked() {
    const QString name = getNameInput().trimmed();
    const QString clickPath = getClickinput().trimmed();
    const QString songPath = getSongInput().trimmed();
    const QString drumsPath = getDrumsInput().trimmed();
    const auto validPath = [](const QString &pathText) {
        fs::path path(pathText.toStdString());
        return fs::exists(path)
            && std::find(std::begin(supportedFormats), std::end(supportedFormats), path.extension().string())
                != std::end(supportedFormats);
    };

    if (name.isEmpty()) {
        logs.write("Cannot add song without a name.\n", messageType::Warn);
        return;
    }

    if ((!ignoringClick() && !validPath(clickPath))
        || (!ignoringSong() && !validPath(songPath))
        || (!ignoringDrums() && !validPath(drumsPath))) {
        logs.write("Cannot add song because one or more track paths are invalid.\n", messageType::Warn);
        return;
    }

    newSong = Song(
        ignoringClick() ? "" : clickPath.toStdString(),
        ignoringSong() ? "" : songPath.toStdString(),
        ignoringDrums() ? "" : drumsPath.toStdString(),
        name.toStdString()
    );
    songs.push_back(newSong);
    saveSongs();
    logs.write(format("Added song {}.\n", newSong.name));
    ui->screens->setCurrentWidget(ui->musicLibrary);
    setupLibrary();
}