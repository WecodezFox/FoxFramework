#include "server.h"

#include <QTime>
#include <QDebug>

Server::Server(QString name, QObject *parent) :
    QLocalServer(parent)
{
    m_name = name;
}

Server::~Server()
{
    //Delete stored sockets
    foreach (QLocalSocket* socket, m_connections)
    {
        socket->close();
        socket->deleteLater();
    }
}

bool Server::init()
{
    // start the socket server
    if(this->listen(m_name))
    {
        connect(this, SIGNAL(newConnection()), this, SLOT(newConectionAvaliable()));
        return true;
    }
    return false;
}

void Server::closeSocket(int sockDesc)
{
    if(m_connections.contains(sockDesc))
    {
        QLocalSocket * socket = m_connections.value(sockDesc);
        socket->close();
        m_connections.remove(sockDesc);
        socket->deleteLater();
    }
}

void Server::newConectionAvaliable()
{
    //store all Pending Connections
    while (this->hasPendingConnections())
    {
        addToList(this->nextPendingConnection());
    }
}

void Server::discardSocket()
{
    //Pointer to the disconnecting socket
    QLocalSocket * ptrSender = qobject_cast<QLocalSocket*>(sender());
    if(ptrSender)
    {
        //Notify socket deleted
        emit SocketDeleted(ptrSender->socketDescriptor());

        //Remove the socket from the list
        m_connections.remove(ptrSender->socketDescriptor());

        //Delete the socket a soon as possible
        ptrSender->deleteLater();
    }
}

void Server::readSocket()
{
    //Pointer to the signal sender
    QLocalSocket * socket = qobject_cast<QLocalSocket*>(sender());

    if(socket)
    {
        //Read all data on the socket & store it on a QByteArray
        QByteArray block = socket->readAll();

        //Data stream to easy read all data
        QDataStream in(&block, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_0);

        while (!in.atEnd()) //loop needed cause some messages can come on a single packet
        {
            QByteArray p;
            in >> p;
            QJsonDocument j_doc = QJsonDocument::fromJson(p);
            emit ReadReady(socket->socketDescriptor(),j_doc.object());
            qDebug() << "Data received :: " << socket->socketDescriptor()
                                  << " :: " << j_doc.toJson(QJsonDocument::Compact);
        }
    }
}

void Server::addToList(QLocalSocket *socket)
{
    //Store sockets & connect them for read & discard them on disconnect
    qDebug() << "Client connected ::" << socket->socketDescriptor();
    m_connections.insert(socket->socketDescriptor(),socket);
    connect(socket, SIGNAL(readyRead()), this , SLOT(readSocket()));
    connect(socket, SIGNAL(disconnected()), this , SLOT(discardSocket()));
    emit SocketConnected(socket->socketDescriptor());    
}

void Server::send(int  socketDescriptor , QJsonDocument sms)
{
    if(m_connections.contains(socketDescriptor))
    {
        QLocalSocket * socket = m_connections.value(socketDescriptor);
        if(socket)
        {
            if(socket->isOpen())
            {
                QByteArray block;
                QDataStream out(&block, QIODevice::WriteOnly);

                out.setVersion(QDataStream::Qt_5_0);
                out << sms.toJson();

                socket->write(block);
                emit messageSent(socketDescriptor, sms.object());
                qDebug() << "Data sent :: " << socket->socketDescriptor()
                                   <<" :: " << sms.toJson(QJsonDocument::Compact);
            }
        }
    }
}
