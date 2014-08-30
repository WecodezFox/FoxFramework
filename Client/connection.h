#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QScriptable>
#include <QLocalSocket>
#include <QJsonDocument>
#include <QJsonObject>

class Connection : public QObject, protected QScriptable
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
    void send(QJsonDocument p);

signals:
    void ReadReady(QJsonObject);
    void ServerGoAway();

public slots:
    void send(QString message, QString extension = QStringLiteral("global"));
    
private slots:
    void readSocket();
    void discardSocket();

private:
    QLocalSocket* conection;
};

#endif // CONNECTION_H
