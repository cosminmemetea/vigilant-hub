#include "CameraThread.h"
#include <QDebug>
#include <QElapsedTimer>

CameraThread::CameraThread(int cameraIndex, QObject *parent)
    : QThread(parent), cameraOpened(false), cameraIndex(cameraIndex), targetWidth(640), targetHeight(480), targetFrameRate(15), shouldStop(false) {
    tryOpenCamera();
}

void CameraThread::setTargetResolution(int width, int height) {
    targetWidth = width;
    targetHeight = height;
    if (cameraOpened) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, targetWidth);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, targetHeight);
        qDebug() << "Set camera resolution to" << targetWidth << "x" << targetHeight;
    }
}

void CameraThread::setTargetFrameRate(int fps) {
    targetFrameRate = fps;
    if (cameraOpened) {
        cap.set(cv::CAP_PROP_FPS, targetFrameRate);
        qDebug() << "Set camera frame rate to" << targetFrameRate << "FPS";
    }
}

void CameraThread::stop() {
    shouldStop = true;
    wait();
    qDebug() << "CameraThread stopped.";
}

bool CameraThread::tryOpenCamera() {
    if (cap.isOpened()) {
        cap.release();
        qDebug() << "Previous camera instance released.";
    }
    cap.open(cameraIndex);
    if (!cap.isOpened()) {
        qWarning() << "Failed to open camera at index" << cameraIndex << "! Check permissions or camera availability.";
        return false;
    }
    cameraOpened = true;
    qDebug() << "Camera at index" << cameraIndex << "opened successfully.";
    cap.set(cv::CAP_PROP_FRAME_WIDTH, targetWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, targetHeight);
    cap.set(cv::CAP_PROP_FPS, targetFrameRate);
    qDebug() << "Set camera resolution to" << targetWidth << "x" << targetHeight << "and frame rate to" << targetFrameRate << "FPS";
    return true;
}

void CameraThread::run() {
    QElapsedTimer timer;
    timer.start();
    qint64 frameInterval = 1000 / targetFrameRate;
    int frameCount = 0;

    while (!shouldStop) {
        if (!cameraOpened) {
            qWarning() << "Camera not opened. Retrying...";
            if (!tryOpenCamera()) {
                QImage fallbackImage(targetWidth, targetHeight, QImage::Format_RGB888);
                fallbackImage.fill(Qt::red);
                qDebug() << "Emitting fallback image (red square). Frame count:" << frameCount;
                emit frameReady(fallbackImage);
                msleep(1000);
                continue;
            }
        }

        cv::Mat frame;
        try {
            if (!cap.grab()) {
                qWarning() << "Failed to grab frame. Camera may be disconnected.";
                cameraOpened = false;
                msleep(100);
                continue;
            }
            if (!cap.retrieve(frame)) {
                qWarning() << "Failed to retrieve frame. Camera may be disconnected.";
                cameraOpened = false;
                msleep(100);
                continue;
            }
            if (frame.empty()) {
                qWarning() << "Captured an empty frame. Camera may be disconnected.";
                cameraOpened = false;
                continue;
            }
            QImage qimg(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            QImage frameCopy = qimg.rgbSwapped().copy();
            qDebug() << "Captured frame:" << frame.cols << "x" << frame.rows << "Frame count:" << frameCount++;
            emit frameReady(frameCopy);
        } catch (const cv::Exception &e) {
            qCritical() << "OpenCV exception caught:" << e.what();
            cameraOpened = false;
            msleep(100);
            continue;
        } catch (...) {
            qCritical() << "Unknown exception caught in CameraThread.";
            cameraOpened = false;
            msleep(100);
            continue;
        }

        qint64 elapsed = timer.elapsed();
        qint64 sleepTime = frameInterval - elapsed;
        if (sleepTime > 0) {
            msleep(sleepTime);
        } else {
            qWarning() << "Frame processing took too long:" << elapsed << "ms. Target interval:" << frameInterval << "ms";
        }
        timer.restart();
    }
    if (cap.isOpened()) {
        cap.release();
        qDebug() << "Camera released.";
    }
}
