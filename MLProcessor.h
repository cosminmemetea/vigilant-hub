#ifndef MLPROCESSOR_H
#define MLPROCESSOR_H
#include <QObject>
#include <QImage>
#include <QTcpSocket>
#include <QTimer>

class MLProcessor : public QObject {
    Q_OBJECT
    Q_PROPERTY(float yawnPercentage READ yawnPercentage NOTIFY resultChanged)
    Q_PROPERTY(float headPosePercentage READ headPosePercentage NOTIFY resultChanged)
    Q_PROPERTY(float eyeClosurePercentage READ eyeClosurePercentage NOTIFY resultChanged)
    Q_PROPERTY(float phonePercentage READ phonePercentage NOTIFY resultChanged)

public:
    MLProcessor(QObject *parent = nullptr);
    ~MLProcessor();
    void processFrame(const QImage &frame);
    void stop();

    float yawnPercentage() const { return yawn_percentage; }
    float headPosePercentage() const { return head_pose_percentage; }
    float eyeClosurePercentage() const { return eye_closure_percentage; }
    float phonePercentage() const { return phone_percentage; }

signals:
    void resultChanged();

private slots:
    void readSocketData();
    void handleSocketError(QAbstractSocket::SocketError socketError);
    void handleConnected();

private:
    float yawn_percentage;
    float head_pose_percentage;
    float eye_closure_percentage;
    float phone_percentage;
    QTcpSocket *socket;
    QTimer *reconnectTimer;
    bool shouldStop;
};

#endif
