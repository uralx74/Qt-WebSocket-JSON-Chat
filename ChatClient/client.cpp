#include "client.h"
#include <QInputDialog>

Client::Client(QObject *parent) : QObject(parent)
{
    clientSocket = new QWebSocket();
    connect(clientSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onReceiveMessage(QString)));
    connect(clientSocket, SIGNAL(connected), this, SLOT(onConnected));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    //QObject::connect(wsSocket, SIGNAL(sslErrors(const QList<QSslError>&)), this, SLOT(displaySslErrors(const QList<QSslError>&)));
    //connect(&m_webSocket, &QWebSocket::connected, this, &EchoClient::onConnected);

}

void Client::onConnected()
{
    // doesn work
    // why?
    //sendUsername("testUsername");
}

/* Сокет для обработки разьединения с сервером
 */
void Client::onDisconnected()
{
    clientSocket->deleteLater();
}

/* Соединяется с сервером
 */
void Client::connectSocket(unsigned int port, QString host)
{
    _port = port;
    _host = host;

    QUrl qUrl;
    qUrl.setPort(_port);
    qUrl.setHost(_host);
    qUrl.setScheme("ws");
    clientSocket->open(qUrl);




    /* bool ok;
   // QString ipAddress = QInputDialog::getText(this, tr("Client"), tr("Server IP:"), QLineEdit::Normal, "ws://localhost:80", &ok);
    QString ipAddress = "";


    ipAddress = ipAddress.trimmed();
    if (ok && !ipAddress.isEmpty())
    {
        QString ip;
        quint16 port;
        if (ipAddress.contains(QRegExp(QLatin1String(":([0-9]|[1-9][0-9]{1,3}|[1-5][0-9]{1,4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$"))))
        {
            QStringList splitted = ipAddress.split(':');
            port = splitted.takeLast().toUInt();
            ip = splitted.join(':');
        }
        else
        {
            ip = ipAddress;
            port = 80;
        }
        wsSocket->connect(ip.toUtf8(), port);
        wsSocket->open();
        //wsSocket->connectToHost(ip.toUtf8(), port);
    }*/
}

/* Отправляет имя пользователя
 */
void Client::sendUsername(QString username)
{
    QJsonObject json;
    json.insert("username", username);
    //json["username"]    = QJsonValue() <<  username;

    QJsonDocument doc;
    doc.setObject(json);
    clientSocket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
}

/* Отправляет текстовое сообщение
 */
void Client::sendMessage(QString message)
{
    QJsonObject json;
    json.insert("message", message);
    //json["booleanAndInt"]    = QJsonArray() <<  false  <<  1;
    //json["message"]    = QJsonValue() <<  message;

    QJsonDocument doc;
    doc.setObject(json);
    clientSocket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));

    //json.insert("test",123);
    //json.toVariantMap()
    //wsSocket->sendTextMessage( QString::fromUtf8( ) ) );
    //QJsonDocument d = QJsonDocument::fromJson("reply->readAll()");
    //QJsonDocument d = QJsonDocument::toJson(obj);
    //QJsonDocument doc(message);
    //wsSocket->sendTextMessage(QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    //wsSocket->sendTextMessage(message);

}

/* Слот для приема сообщений
 */
void Client::onReceiveMessage(QString message)
{
    // Парсинг сообщения
    QJsonDocument messageDocument = QJsonDocument::fromJson(message.toUtf8());
    //qWarning() << doc.isNull();
    QJsonObject messageJson;
    messageJson = messageDocument.object();


    // Сервер запрашивает имя пользователя
    if ( messageJson.contains("request_username") ) {
        emit requestUsername();
    }
    // Сервер отправил текстовое сообщение
    else if ( messageJson.contains("message") ) {
        //QString messageText  = messageJson.take("message").toString();
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

/*
QJsonObject ObjectFromString(const QString& in)
{
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(in.toUtf8());

    // check validity of the document
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
        }
        else
        {
            qDebug() << "Document is not an object" << endl;
        }
    }
    else
    {
        qDebug() << "Invalid JSON...\n" << in << endl;
    }

    return obj;
}*/
