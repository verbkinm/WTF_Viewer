#ifndef MASKSETTINGS_H
#define MASKSETTINGS_H

#include <QWidget>
#include <vector>

namespace Ui {
class MaskSettings;
}

class MaskSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MaskSettings(QWidget *parent = nullptr);
    ~MaskSettings();

    int getRank() const;
    int getTransparency() const;

private:
    Ui::MaskSettings *ui;

    size_t _BINNING_MAX_SIZE;
    std::vector<std::vector<double>> _vecBin;
    double minus_one_or_zero;
    size_t _binning;	//ранг бингования

    enum TAB_NAME{MASK, CONVOLUTION, BACKPROJECTION};

    int square_vychet(int i, int p) const;
    void mask_construction(int N);

private slots:
    void slotGenerate();
    void slotOpenTXT();
    void slotSaveTXT();
    void slotBinningChanged(int);
    void slotTableShow();

signals:
    void signalGenerated(QString);
    void signalOpenTXT(QString);
    void signalSaveTXT();
};
#endif // MASKSETTINGS_H
