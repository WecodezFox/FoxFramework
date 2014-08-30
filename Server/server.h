#ifndef SERVER_H
#define SERVER_H

#include <QLocalServer>
#include <QLocalSocket>
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonObject>

class Server : public QLocalServer
{
    Q_OBJECT
public:
    /*!
     * \brief Local Server.
     * \param name Listening name for the server.
     * \param parent Qt parent object
     */
    explicit Server(QString name , QObject *parent = 0);
    ~Server();

    /*!
     * \brief Start the server listening at this name.
     * \return success.
     */
    bool init();

    /*!
     * \brief Close the connection with one client.
     * \param sockDesc Client socket descriptor.
     */
    void closeSocket(int sockDesc);

signals:
    /*!
     * \brief Emitted when new client connect to the server.
     * \param socketDescriptor Socket Descriptor of the new client.
     */
    void SocketConnected(int socketDescriptor);

    /*!
     * \brief Emitted when a client disconnect from the server.
     * \param socketDescriptor Socket Descriptor of the disconnected client.
     */
    void SocketDeleted(int socketDescriptor);

    /*!
     * \brief Server have some incomming data that can be read.
     * \param socketDescriptor Socket Descriptor of the data client.
     * \param message Just arrived data.
     */
    void ReadReady(int socketDescriptor , QJsonObject message);

    /*!
     * \brief Server have sent some data to a client
     * \param socketDescriptor Socket Descriptor of the client.
     * \param sms Data sent
     */
    void messageSent(int socketDescriptor, QJsonObject sms);

public slots:    
    /*!
     * \brief Send data to a connected client.
     * \param socketDescriptor Socket Descriptor of the client.
     * \param sms Data to send.
     */
    void send(int socketDescriptor, QJsonDocument sms);

protected slots:
    void newConectionAvaliable();
    void discardSocket();
    void readSocket();

private:
    QString m_name;
    QMap<int , QLocalSocket *> m_connections;

    void addToList(QLocalSocket *socket);
};
#endif // SERVER_H
