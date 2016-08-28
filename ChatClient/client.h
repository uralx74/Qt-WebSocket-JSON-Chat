#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QtWebSockets/QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = 0);
    void connectSocket(const QString& host = "localhost", unsigned int port = 8089);
    void sendUsername(const QString& username);
    void sendMessage(const QString& message);

private:
    unsigned int _port;
    QString _host;
    QWebSocket *clientSocket;

signals:
    void receiveMessage(QString username, QString message);
    void requestUsername();
    void receiveUserlist(QJsonArray array);
    void receiveServiceMessage(QString message);


private slots:
    void onReceiveMessage(QString message);
    void onDisconnected();

public slots:
};

#endif // CLIENT_H
