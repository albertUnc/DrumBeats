//main.cpp
#include <QApplication>
#include <QStyleFactory>
#include <filesystem>
#include <fstream>
#include "MainWindow.h"
#include "Utils.h"
#include "SETTINGS.h"
namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    logs.setSignature("[main.cpp] ");
    logs.welcome();
    logs.write("Main function just started, setting up and running QApplication.\n");
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    MainWindow window;
    window.show();
    return app.exec();
}