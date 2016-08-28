#include "client.h"
#include <QInputDialog>

Client::Client(QObject *parent) : QObject(parent)
{
    clientSocket = new QWebSocket();
    connect(clientSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onReceiveMessage(QString)));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

/* Сокет для обработки разьединения с сервером
 */
void Client::onDisconnected()
{
    clientSocket->deleteLater();
}

/* Соединяется с сервером
 */
void Client::connectSocket(const QString& host, unsigned int port)
{
    if (clientSocket != NULL && clientSocket->isValid()) {
        return; // Necessary to check and close (or return) connection if it is open
    }

    _port = port;
    _host = host;

    QUrl qUrl;
    qUrl.setPort(_port);
    qUrl.setHost(_host);
    qUrl.setScheme("ws");

    clientSocket->open(qUrl);
}

/* Отправляет имя пользователя
 */
void Client::sendUsername(const QString& username)
{
    QJsonObject json;
    json.insert("username", username);

    QJsonDocument doc;
    doc.setObject(json);
    clientSocket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

/* Отправляет текстовое сообщение
 */
void Client::sendMessage(const QString& message)
{
    QJsonObject json;
    json.insert("message", message);

    QJsonDocument doc;
    doc.setObject(json);
    clientSocket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

/* Слот для приема сообщений
 */
void Client::onReceiveMessage(QString message)
{
    // Парсинг сообщения
    QJsonDocument messageDocument = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject messageJson;
    messageJson = messageDocument.object();


    // Сервер запрашивает имя пользователя
    if ( messageJson.contains("request_username") ) {
        emit requestUsername();
    }
    // Сервер отправил текстовое сообщение
    else if ( messageJson.contains("message") ) {
        QString messageText = messageJson["message"].toString();
        QString username = messageJson["username"].toString();
        emit receiveMessage(username, messageText);
    }
    else if ( messageJson.contains("userlist") ) {
        QJsonArray userlist = messageJson["userlist"].toArray();
        emit receiveUserlist(userlist);
    }
    else if ( messageJson.contains("service_message") ) {
        QString messageText = messageJson["service_message"].toString();
        emit receiveServiceMessage(messageText);
    }

}
