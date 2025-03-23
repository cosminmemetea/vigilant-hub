#include "MLProcessor.h"
#include <QDebug>
#include <QDataStream>
#include <QBuffer>

MLProcessor::MLProcessor(QObject *parent)
    : QObject(parent), shouldStop(false), yawn_percentage(0.0f), head_pose_percentage(0.0f), eye_closure_percentage(0.0f), phone_percentage(0.0f) {
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &MLProcessor::readSocketData);
    connect(socket, &QTcpSocket::errorOccurred, this, &MLProcessor::handleSocketError);
    connect(socket, &QTcpSocket::connected, this, &MLProcessor::handleConnected);

    reconnectTimer = new QTimer(this);
    reconnectTimer->setInterval(5000);
    connect(reconnectTimer, &QTimer::timeout, this, [this]() {
        if (socket->state() != QAbstractSocket::ConnectedState) {
            qDebug() << "Attempting to connect to Python server...";
            socket->connectToHost("localhost", 5001);
        }
    });
    reconnectTimer->start();

    qDebug() << "Attempting initial connection to Python server...";
    socket->connectToHost("localhost", 5001);
}

MLProcessor::~MLProcessor() {
    stop();
}

void MLProcessor::stop() {
    shouldStop = true;
    reconnectTimer->stop();
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->disconnectFromHost();
    }
}

void MLProcessor::processFrame(const QImage &frame) {
    if (socket->state() != QAbstractSocket::ConnectedState) {
        qDebug() << "Socket not connected, cannot send frame.";
        return;
    }

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);
    frame.save(&buffer, "JPG");
    QByteArray frameData = buffer.data();

    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_5_15);
    quint32 frameSize = frameData.size();
    out << frameSize;
    socket->write(frameData);
    socket->flush();
    qDebug() << "Frame sent to server, size:" << frameSize;
}

void MLProcessor::readSocketData() {
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);

    while (socket->bytesAvailable() >= sizeof(quint32)) {
        quint32 blockSize;
        in >> blockSize;
        qDebug() << "Received block size:" << blockSize;

        // Validate block size (should be 16 for four floats)
        if (blockSize != 16) {
            qWarning() << "Invalid block size received:" << blockSize << ". Expected 16 bytes for four floats.";
            socket->readAll();
            continue;
        }

        if (socket->bytesAvailable() < blockSize) {
            qDebug() << "Waiting for more data... Bytes available:" << socket->bytesAvailable() << ", Needed:" << blockSize;
            return;
        }

        QByteArray data = socket->read(blockSize);
        QDataStream dataStream(data);
        dataStream.setVersion(QDataStream::Qt_5_15);

        float yawn, head_pose, eye_closure, phone;
        dataStream >> yawn >> head_pose >> eye_closure >> phone;

        qDebug() << "Raw values from server - Yawn:" << yawn << "%, Head Pose:" << head_pose << "%, Eye Closure:" << eye_closure << "%, Phone:" << phone << "%";

        yawn_percentage = qBound(0.0f, yawn, 100.0f);
        head_pose_percentage = qBound(0.0f, head_pose, 100.0f);
        eye_closure_percentage = qBound(0.0f, eye_closure, 100.0f);
        phone_percentage = qBound(0.0f, phone, 100.0f);

        qDebug() << "Updated properties - Yawn:" << yawn_percentage << "%, Head Pose:" << head_pose_percentage << "%, Eye Closure:" << eye_closure_percentage << "%, Phone:" << phone_percentage << "%";

        emit resultChanged();
        qDebug() << "Emitted resultChanged signal";
    }
}

void MLProcessor::handleSocketError(QAbstractSocket::SocketError socketError) {
    qWarning() << "Socket error:" << socket->errorString();
    socket->disconnectFromHost();
    if (!shouldStop) {
        reconnectTimer->start();
    }
}

void MLProcessor::handleConnected() {
    qDebug() << "Successfully connected to Python server!";
}
