#include "connection.h"
#include <QMessageBox>
#include <QScriptEngine>
#include "../Common/Tags.h"

Connection::Connection(QObject *parent) :
    QObject(parent)
{
    conection = new QLocalSocket(this);

    connect(conection,SIGNAL(readyRead()),this,SLOT(readSocket()));
    connect(conection,SIGNAL(disconnected()),this,SLOT(discardSocket()));
    conection->connectToServer(Packets::server_name_tag);
}

void Connection::readSocket()
{
    QByteArray block = conection->readAll();
    QDataStream in(&block, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_0);
    while (!in.atEnd())
    {
        QByteArray p;
        in >> p;
        QJsonDocument j_doc = QJsonDocument::fromJson(p);
        qDebug() << j_doc.toJson(QJsonDocument::Compact);
        emit ReadReady(j_doc.object());
    }
}

void Connection::discardSocket()
{
    conection->deleteLater();
    conection = 0;
    emit ServerGoAway();
}

void Connection::send(QJsonDocument p)
{
    if(conection)
    {
        if(conection->isOpen())
        {
            QByteArray block;
            QDataStream out(&block, QIODevice::WriteOnly);

            out.setVersion(QDataStream::Qt_5_0);
            out << p.toJson();
            conection->write(block);
        }
        else
        {
            QMessageBox::critical(0,tr("Error"),tr("Connection isn't open!"));
        }
    }
    else
    {
        QMessageBox::critical(0,tr("Error"),tr("Not connected!"));
    }
}

void Connection::send(QString message, QString extension)
{    
    QJsonDocument doc;
    QJsonObject obj;
    obj[Packets::header_tag] = (int)Packets::Headers::EXTENSION;
    obj[Packets::extension_tag] = extension;
    obj[Packets::content_tag] = message;
    doc.setObject(obj);
    this->send(doc);
}
