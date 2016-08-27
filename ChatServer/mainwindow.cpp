#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Server");

    userList = new QListWidget(this);
    userList->setGeometry(10,10, 500, 400);

    server = new Server(this);
    connect(server, SIGNAL(userConnected(QString)), this, SLOT(onUserConnected(QString)));
    connect(server, SIGNAL(userDisconnected(QString)), this, SLOT(onUserDisconnected(QString)));
    connect(server, SIGNAL(errorOccured(QString)), this, SLOT(onErrorOccured(QString)));

    server->Start();
}

MainWindow::~MainWindow()
{
    releaseConnections(m_pSocket);
    server->deleteLater();
    delete ui;
}

/* Слот для соедениния с пользователем
 * Добавляет в список пользователей имя нового пользователя
 */
void MainWindow::onUserConnected(QString username)
{
    userList->addItem(username);
}

/* Слот для разьединения с пользователем
 * Удаляет из списка имя отсоединившегося пользователя
 */
void MainWindow::onUserDisconnected(QString username)
{
    QList<QListWidgetItem *> items = userList->findItems(username, Qt::MatchExactly);
    if (items.size() > 0) {
        foreach(QListWidgetItem* item, items) {
            delete item;
        }
    }
}

/* Слот для обработки ошибок
 * Выводит сообщение об ошибке
 */
void MainWindow::onErrorOccured(QString errorMessage)
{
    QMessageBox::information(this, "Error occured", errorMessage);
}

