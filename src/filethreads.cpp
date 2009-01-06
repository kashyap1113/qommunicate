#include "filethreads.h"

#include <QMessageBox>
#include <QProgressDialog>

#include "constants.h"

void FileSendThread::socketError(QTcpSocket::SocketError error)
{
    qWarning() << "Socket Error:" << m_socket->errorString();
    m_socket->disconnectFromHost();
    emit connectionError(m_socket->errorString());
    exit(1);
}

void FileSendThread::run()
{
}

void FileSendThread::nextFileRequested()
{
    qDebug() << "nextFileRequested";
    m_offset = 0;
    m_totalSent = 0;
    
//     if(m_file != NULL)
//     {
//         m_file->close();
//         delete m_file;
//         m_file = NULL;
//     }
    
    qint64 offset;
    QList<QByteArray> metadata = m_socket->readAll().split(':');
    qDebug() << metadata;
    // this 8 thing is because the last token in metadata is blank
    if(metadata.size() <= 8)
    {
        qWarning() << "Error: Bad request from receiver" ;
        return;
    }
    
    int command = metadata[4].toInt();
    switch(command)
    {
        case QOM_GETFILEDATA:
            qDebug() << "Command was GETFILEDATA" ;
            emit requestFilePath(metadata[6].toInt());
            m_offset = ( metadata[7].isEmpty() ? 0 : metadata[7].toInt() );
            break;
    }
}

void FileSendThread::acceptFilePath(QString fileName)
{
    qDebug() << " acceptFilePath: preparing to send" << fileName;
    emit sendingNextFile(fileName);
    m_file = new QFile(fileName);
    if(!m_file->open(QIODevice::ReadOnly))
    {
        qWarning() << "Could not open file for reading" ;
        exit(1);
    }
    m_file->seek(m_offset);
    m_totalSent = m_offset;
    qDebug() << "Preparing to write";
    m_socket->write(m_file->readAll());
}

void FileSendThread::updateProgress(qint64 bytes)
{
    m_totalSent += bytes;
    qDebug() << "Wrote" << m_totalSent/m_file->size() * 100 << "%";
    emit notifyProgress(m_totalSent/m_file->size() * 100);
    
}

void FileSendThread::done()
{
    qDebug() << "Socket disconnected, transfer done" ;
    terminate();
}

void FileSendThread::sendFiles()
{
    
}