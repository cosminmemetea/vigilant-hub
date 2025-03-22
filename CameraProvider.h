#ifndef CAMERAPROVIDER_H
#define CAMERAPROVIDER_H
#include <QQuickImageProvider>
#include <QImage>
#include <QMutex>

class CameraProvider : public QQuickImageProvider {
public:
    CameraProvider() : QQuickImageProvider(QQuickImageProvider::Image), frameCount(0) {}
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override {
        Q_UNUSED(id);
        Q_UNUSED(requestedSize);
        QMutexLocker locker(&mutex);
        if (size) *size = m_image.size();
        if (m_image.isNull()) {
            qWarning() << "CameraProvider: Requested image is null. Frame count:" << frameCount;
        } else {
            qDebug() << "CameraProvider: Providing image:" << m_image.size() << "Frame count:" << frameCount;
        }
        return m_image;
    }
    void updateImage(const QImage &image) {
        QMutexLocker locker(&mutex);
        if (image.isNull()) {
            qWarning() << "CameraProvider: Received null image. Frame count:" << frameCount;
        } else {
            qDebug() << "CameraProvider: Updated image:" << image.size() << "Frame count:" << frameCount++;
        }
        m_image = image;
    }
private:
    QImage m_image;
    QMutex mutex;
    int frameCount;
};
#endif
