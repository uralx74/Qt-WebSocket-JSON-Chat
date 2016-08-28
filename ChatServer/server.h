#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>

class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    ~Server();
    void Start(unsigned int port = 8089);


private:
    void sendToAll(const QString& message);
    void sendUserList();
    void sendRequestUsername(QWebSocket* client);
    void sendServiceMessage(const QString& message);
    void freeConnections();

    //unsigned int _port;
    QWebSocketServer *server;
    QMap<QWebSocket*, QString> clients;

signals:
    void userConnected(QString username);
    void userDisconnected(QString username);
    void errorOccured(QString errorMessage);
    //void messageReceived(QString message, QString username);

private slots:
    void onNewConnection();
    void onReceiveMessage(QString message);
    void onDisconnect();


public slots:
};

#endif // SERVER_H
