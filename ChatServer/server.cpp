#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    server = new QWebSocketServer("CharServer", QWebSocketServer::NonSecureMode);
    connect(server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

Server::~Server()
{


    if (server->isListening()) {
        server->close();
    }
    server->deleteLater();
    server = NULL;
}

void Server::freeConnections()
{
    foreach ( QWebSocket* client, clients.keys() ) {
        client->close();
        client->deleteLater();
        client = NULL;
    }
    clients.clear(); // is it necessary?
}

/* Запускает сервер
 * Подает сигнал в случае ошибки
 */
void Server::Start(unsigned int port)
{
    if (!server->listen(QHostAddress::Any, port) )
    {
        emit errorOccured(server->errorString());
    }
}

/* Удаляет клиентское соединение из списка соединений
 */
void Server::onDisconnect()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client)
    {
        QString username = clients[client];
        clients.remove(client);
        client->deleteLater();

        // Отправляем сигнал
        emit userDisconnected(username);

        // Рассылаем измененный список клиентов
        sendServiceMessage("Пользователь " + username + " покинул чат.");
        sendUserList();
    }
}

/* Слот при установлении нового соединения
 * Создает клиентский сокет
 * Регистрирует слоты для клиентского сокета
 * Добавляет клинтский сокет в общий список сокетов подключенных пользователей,
 * имя пользователя пока пустое
 * Отправляет запрос имени пользователя
 */
void Server::onNewConnection()
{
    QWebSocket* client = server->nextPendingConnection();
    connect(client, SIGNAL(textMessageReceived(QString)), this, SLOT(onReceiveMessage(QString)));
    connect(client, SIGNAL(disconnected()), this, SLOT(onDisconnect()));

    clients.insert(client, "");

    sendRequestUsername(client);
}

/* Отправляет сервисное сообщение всем пользователям
 */
void Server::sendServiceMessage(const QString& message)
{
    QJsonObject ansewerJson;

    ansewerJson["service_message"] = message;

    QJsonDocument ansewerDocument;
    ansewerDocument.setObject(ansewerJson);

    QString messageAnswer = QString::fromUtf8(ansewerDocument.toJson(QJsonDocument::Compact));

    sendToAll(messageAnswer);
}

/* Слот для получения сообщения от клиента
 */
void Server::onReceiveMessage(QString message)
{
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    if (client == NULL)
    {
        return;
    }

    // Парсинг сообщения
    QJsonDocument messageDocument = QJsonDocument::fromJson(message.toUtf8());
    //qWarning() << doc.isNull();
    QJsonObject messageJson;
    messageJson = messageDocument.object();

    // Пользователь логинится
    if ( messageJson.contains("username") ) {
        QString username = messageJson.take("username").toString().toUpper();
        clients[client] = username;
        sendServiceMessage("Пользователь " + username + " вошел в чат.");
        sendUserList();
        emit userConnected(username);
    }
    // Пользователь отправил текстовое сообщение
    else if ( messageJson.contains("message") ) {
        QJsonObject ansewerJson;

        ansewerJson["username"] = clients[client];
        ansewerJson["message"] = messageJson.take("message").toString();

        QJsonDocument ansewerDocument;
        ansewerDocument.setObject(ansewerJson);

        QString answerMessage = QString::fromUtf8(ansewerDocument.toJson(QJsonDocument::Compact));
        sendToAll(answerMessage);
    }


    /*
    QVariantMap map;
    map["message"] = "test message";

    QByteArray data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Compact);
    socket->send(data);
    */


    /* QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(message, &json_error);
        if(json_error.error == QJsonParseError::NoError)
        {
            if(parse_doucment.isObject())
            {
                QJsonObject obj = parse_doucment.object();
                if(obj.contains("name"))
                {
                    QJsonValue name_value = obj.take("name");
                    if(name_value.isString())
                    {
                        name = name_value.toString();
                    }
                }
                if(obj.contains("msg"))
                {
                    QJsonValue msg_value = obj.take("msg");
                    if(msg_value.isString())
                    {
                        msg = msg_value.toString();
                    }
                }
            }
        }

    */
}

/* Отправляет сообщение всем пользователям
 */
void Server::sendToAll(const QString& message)
{
    foreach ( QWebSocket* client, clients.keys() ) {
        client->sendTextMessage(message);
    }
}

/* Отправляет всем обновленный список пользователей
 */
void Server::sendUserList()
{
    QJsonObject ansewerJson;
    QJsonArray userlist;

    foreach(QString username, clients) {
        userlist <<  username;
    }

    ansewerJson["userlist"] = userlist;

    QJsonDocument ansewerDocument;
    ansewerDocument.setObject(ansewerJson);

    QString message = QString::fromUtf8(ansewerDocument.toJson(QJsonDocument::Compact));

    sendToAll(message);
}

/* Отправляет клиенту запрос имени пользователя
 */
void Server::sendRequestUsername(QWebSocket* client)
{
    if (client->isValid()) {
        QJsonObject ansewerJson;
        ansewerJson["request_username"] = true;

        QJsonDocument ansewerDocument;
        ansewerDocument.setObject(ansewerJson);

        QString answerMessage = QString::fromUtf8(ansewerDocument.toJson(QJsonDocument::Compact));

        client->sendTextMessage(answerMessage);
    }

}

