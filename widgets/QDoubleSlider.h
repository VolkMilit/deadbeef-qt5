#ifndef Q_DOUBLE_SLIDER_H
#define Q_DOUBLE_SLIDER_H
#include <QtGui/QtGui>
#include <QSlider>
class QDoubleSlider : public QSlider
{
    Q_OBJECT

public:
    QDoubleSlider(Qt::Orientation orientation, double multiplier, QWidget *pParent = nullptr);

    void setRange(double Min, double Max);
    void setMinimum(double Min);
    double minimum() const;
    void setMaximum(double Max);
    double maximum() const;
    double value() const;

    public slots:
        void setValue(int value);
        void setValue(double Value, bool BlockSignals = false);

    signals:
        void valueChanged(double Value);
        void rangeChanged(double Min, double Max);

    private:
        double	m_Multiplier;
    
    protected:
        virtual void sliderChange(SliderChange change);
};
#endif
