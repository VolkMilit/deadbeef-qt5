#include "SeekSlider.h"

#include "QtGuiSettings.h"
#include "GuiUpdater.h"

SeekSlider::SeekSlider(QWidget *parent) :
    QSlider(parent)
{
    activateNow = false;
    setRange(0, 100 * SEEK_SCALE);
    setOrientation(Qt::Horizontal);
    connect(GuiUpdater::Instance(), &GuiUpdater::frameUpdate, this, &SeekSlider::onFrameUpdate);
    connect(GuiUpdater::Instance(), &GuiUpdater::isPlaying, this, &SeekSlider::setEnabled);
}

SeekSlider::~SeekSlider() {}

void SeekSlider::mouseReleaseEvent(QMouseEvent *)
{
    DBAPI->playback_set_pos(value() / SEEK_SCALE);
    activateNow = false;
}

void SeekSlider::mousePressEvent(QMouseEvent *ev)
{
    activateNow = true;
    setValue(pos(ev));
}

void SeekSlider::mouseMoveEvent(QMouseEvent *ev)
{
    setValue(pos(ev));
}

void SeekSlider::wheelEvent(QWheelEvent *ev)
{
    ev->ignore();
}

void SeekSlider::onFrameUpdate()
{
    if (activateNow)
        return;

    if (isHidden() || parentWidget()->isHidden())
        return;

    if (!DBAPI->get_output())
        return;

    if (DBAPI->get_output()->state() == OUTPUT_STATE_PLAYING || DBAPI->get_output()->state() == OUTPUT_STATE_PAUSED)
        setValue(DBAPI->playback_get_pos() * SEEK_SCALE);
}

int SeekSlider::pos(QMouseEvent *ev) const
{
    int val = (static_cast<float>(ev->x()) / this->width()) * maximum();
    if(val >= maximum()) return maximum();
    if(val <= minimum()) return minimum();
    return val;
}
