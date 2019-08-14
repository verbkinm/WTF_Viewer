#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include <QWidget>


#include <QColorDialog>
#include <QIcon>

namespace Ui {
class ToolsPanel;
}

class ToolsPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ToolsPanel(QWidget *parent = nullptr);
    ~ToolsPanel();

    void buttonSelectionDisable (bool value = true);
    void buttonEditDisable      (bool value = true);
    void buttonCutDisable       (bool value = true);

    void setToggleButtons       (bool value);

    void finishSelection        ();

    QColor  getPenColor         () const;
    double getPenValue();

private:
    Ui::ToolsPanel *ui;

    QColor _color;
    double _value;

private slots:
    void slotSetColor           ();

    void slotSelectionToggle    (bool value);
    void slotPenToggle          (bool value);

    void slotPenValueChanged    (double value);

signals:
    void signalPenToggle        (bool);
    void signalSelectionToggle  (bool);
    void signalCutClicked       (bool);
    void signalRepaint          ();
};

#endif // TOOLSPANEL_H
