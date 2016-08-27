#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"
#include <QPushButton>
#include <QListWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QListWidget* userList;
    //static const unsigned int _port = 8087;
    Server* server;

public slots:
    void onUserConnected(QString username);
    void onUserDisconnected(QString username);
    void onErrorOccured(QString errorMessage);
};

#endif // MAINWINDOW_H
