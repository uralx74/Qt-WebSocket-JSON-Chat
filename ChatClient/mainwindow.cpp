#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Client");

    hostEdt = new QLineEdit(this);
    hostEdt->setGeometry(10,360,480,30);
    hostEdt->setText("127.0.0.1");

    connectBtn = new QPushButton(this);
    connectBtn->setText("Connect");
    connectBtn->setGeometry(510,360,100,30);
    connect(connectBtn, SIGNAL(clicked()), this, SLOT(onConnectBtnClick()));


    messageEdt = new QLineEdit(this);
    messageEdt->setGeometry(10,320,480,30);

    sendMessageBtn = new QPushButton(this);
    sendMessageBtn->setGeometry(510,320,100,30);
    sendMessageBtn->setText("Send");
    connect(sendMessageBtn, SIGNAL(clicked()), this, SLOT(onSendMessageBtnClick()));

    userList = new QListWidget(this);
    userList->setGeometry(510,10,100,300);


    messageBoard = new QTextEdit(this);
    messageBoard->setGeometry(10,10,480,300);

    client = new Client(this);
    connect(client, SIGNAL(receiveMessage(QString, QString)), this, SLOT(onReceiveMessage(QString, QString)));
    connect(client, SIGNAL(requestUsername()), this, SLOT(onRequestUsername()));
    connect(client, SIGNAL(receiveUserlist(QJsonArray)), this, SLOT(onReceiveUserlist(QJsonArray)));
    connect(client, SIGNAL(receiveServiceMessage(QString)), this, SLOT(onReceiveServiceMessage(QString)));


}

MainWindow::~MainWindow()
{
    delete ui;
}

/* Слот для кнопки соединения с сервером
 */
void MainWindow::onConnectBtnClick()
{
    client->connectSocket(hostEdt->text());
}

/* Слот для кнопки отправки сообщения
 */
void MainWindow::onSendMessageBtnClick()
{
    if (client != NULL && messageEdt->text() != "") {
        client->sendMessage(messageEdt->text());
        messageEdt->clear();
    }
}

/* Слот для получения сообщения
 */
void MainWindow::onReceiveMessage(QString username, QString message)
{
   QString fullMessage = QString("[%1] %2> %3")
           .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
           .arg(username)
           .arg(message);

   messageBoard->append(fullMessage);
}

/* Слот для обработки запроса имени пользователя
 */
void MainWindow::onRequestUsername()
{
    bool ok;
    QInputDialog *renameDialog = new QInputDialog();

    renameDialog->setTextValue("Test"); // has no effect

    QString result = renameDialog->getText(0, "Rename Label", "Username:", QLineEdit::Normal, "", &ok);

    if( result.length() > 0 && ok ) {
        client->sendUsername(result);
    }



   /* QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                            tr("User name:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);*/
       /* if (ok && !text.isEmpty())
            textLabel->setText(text);*/


}

/* Слот для получения сервисного сообщения
 */
void  MainWindow::onReceiveServiceMessage(QString message)
{
    messageBoard->append(message);
}

/* Слот для получения списка пользователей
 */
void MainWindow::onReceiveUserlist(QJsonArray userlist)
{
    userList->clear();
    foreach (const QJsonValue &username, userlist) {
        userList->addItem(username.toString());
    }
}

