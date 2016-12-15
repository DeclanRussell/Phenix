#include <QApplication>
#include <QFile>
#include "ui/mainwindow.h"
#include <QSplashScreen>

int main(int argc, char **argv)
{
    QApplication app(argc,argv);

    //create our loading screen to give the user something to look at while everything loads
    QPixmap loadingPicture("./images/phenix_red.png");
    QSplashScreen loadingScreen(loadingPicture.scaled(400,400),Qt::WindowStaysOnTopHint);
    loadingScreen.show();
    loadingScreen.setMaximumSize(QSize(400,400));

    // Create our mainwindow
    MainWindow w;
    QFile file("styleSheet/darkOrange");
    file.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(file.readAll());
    w.setStyleSheet(stylesheet);
    w.setWindowTitle(QString("Phenix"));
    w.show();
    loadingScreen.hide();
    app.exec();
}
