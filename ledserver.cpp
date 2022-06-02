#include "ledserver.h"
#include"gpio.h"
#include "config.h"

LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_server=new QTcpServer(this);
    m_gpio=new Gpio(this);
    connect(m_server, &QTcpServer::newConnection,this, &LedServer::myNewConnection);

}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, m_port);
    qDebug()<< "Server lauscht auf Port"<<m_port;
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{

    m_socket = m_server->nextPendingConnection();
    qDebug()<<"Verbindung zum Client hergestellt";
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
    connect(m_socket, &QTcpSocket::readyRead,this,&LedServer::myServerRead);

    qDebug()<<"Bitte die Zahl zwischen 0 und 15: ";
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    for (int b=0;b<4;b++)
        {
            m_gpio->set(LEDS[b], 0);
        }

        m_socket->close();
    m_socket->disconnect();
    qDebug()<<"Verbindung zum Client getrennt";
}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{

    while (m_socket->bytesAvailable()) {

       // qDebug()<<"Bitte die Zahl zwischen 0 und 15: ";
        QString message=m_socket->readAll();
        int x=message.toInt();
        if(x>0&&x<=15)
        {
            QByteArray answer=message.toUtf8();
            m_socket->write(answer);
            qDebug()<<message;
            qDebug()<<"OK";
            qDebug()<<"Bitte die Zahl zwischen 0 und 15: ";
            m_socket->write("Bitte die Zahl zwischen 0 und 15: ");
            m_gpio->set(x);
        }
        else {
            {
                if(x>15)
                {
                    QByteArray answer=message.toUtf8();
                    m_socket->write(answer);
                qDebug()<<message;
                 qDebug()<<"ZAHL ZU HOCH! Bitte die Zahl zwischen 0 und 15: ";
                 m_socket->write("ZAHL ZU HOCH! Bitte die Zahl zwischen 0 und 15: ");

                m_socket->readyRead();
                }
                if(x<=0)
                {
                    QByteArray answer=message.toUtf8();
                    m_socket->write(answer);

                    qDebug()<<message;
                     qDebug()<<"ZAHL ZU NIEDRIG! Bitte die Zahl zwischen 0 und 15: ";
                     m_socket->write("ZAHL ZU NIEDRIG! Bitte die Zahl zwischen 0 und 15: ");

                    m_socket->readyRead();

                }

            }
        }
    }
}
