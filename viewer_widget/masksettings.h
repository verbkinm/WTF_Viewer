#ifndef MASKSETTINGS_H
#define MASKSETTINGS_H

#include <QWidget>

#define BINNING_MAX_SIZE 15

namespace Ui {
class MaskSettings;
}

class MaskSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MaskSettings(QWidget *parent = nullptr);
    ~MaskSettings();

    int     getRank() const;
    int     getTransparency() const;


private:
    Ui::MaskSettings *ui;

    double** arrayBin = nullptr;

    enum TAB_NAME{MASK, CONVOLUTION, BACKPROJECTION};

    //
    double minus_one_or_zero;
    int binning;	//ранг бингования

    int** dynamicArrayInt(int N, int M);
    double** dynamicArrayDouble(int N, int M);
    int square_vychet(int i, int p);
    void mask_construction(int N);

    //

private slots:
    void    slotGenerate();
    void    slotOpenTXT();
    void    slotSaveTXT();
    void    slotBinningChanged(int);

    void    slotTableShow();

signals:
    void    signalGenerated(QString);
    void    signalOpenTXT(QString);
    void    signalSaveTXT();
};

#endif // MASKSETTINGS_H
