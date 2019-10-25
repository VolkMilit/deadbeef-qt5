#ifndef SEEKSLIDER_H
#define SEEKSLIDER_H

//#include "ProgressBar.h"

#include <QSlider>
#include "QtGui.h"
#include <QMouseEvent>


#define SEEK_SCALE 10

class SeekSlider : public QSlider {
    Q_OBJECT

public:
    SeekSlider(QWidget *parent = nullptr);
    ~SeekSlider();

protected slots:
    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *ev);

private slots:
    void onFrameUpdate();

private:
    int pos(QMouseEvent *ev) const;
    bool activateNow;
};

#endif // SEEKSLIDER_H
