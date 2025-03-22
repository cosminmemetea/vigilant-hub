#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H
#include <QThread>
#include <QImage>
#include <opencv2/opencv.hpp>

class CameraThread : public QThread {
    Q_OBJECT
public:
    CameraThread(int cameraIndex = 0, QObject *parent = nullptr);
    void setTargetResolution(int width, int height);
    void setTargetFrameRate(int fps);
    void stop();
signals:
    void frameReady(const QImage &frame);
protected:
    void run() override;
private:
    cv::VideoCapture cap;
    bool cameraOpened;
    int cameraIndex;
    int targetWidth;
    int targetHeight;
    int targetFrameRate;
    bool shouldStop;
    bool tryOpenCamera();
};
#endif
