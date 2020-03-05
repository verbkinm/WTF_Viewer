#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <vector>
#include <QVBoxLayout>

namespace Ui {
class RangeWidget;
}

class RangeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RangeWidget(QWidget *parent = nullptr);
    ~RangeWidget();

    void setRange(const std::pair<float, float> &range);
    void setTitle(const QString &title);

    float getRangeBegin() const;
    float getRangeEnd() const;


private:
    Ui::RangeWidget *ui;

private slots:
    void slotBeginChanged(double);
    void slotEndChanged(double);
};

#endif // WIDGET_H
