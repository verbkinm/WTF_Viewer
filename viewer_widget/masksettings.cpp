#include "masksettings.h"
#include "ui_masksettings.h"
#include "table.h"

#include <QFileDialog>
#include <QDebug>
#include <iostream>
#include <fstream>

using namespace std;

//---------------------------------------------
//код Розы
// ->


/*	вычисление квадратного вычета по модулю p	*/
int MaskSettings::square_vychet(int i, int p)
{
    int sign = -1;
    int vychet;
    int mas[100];

    for (int v_count = 0; v_count < 100; v_count++)
    {
        mas[v_count] = 0;
    }
    //массив обнуленный

    for (int v_count = 0; v_count < 100; v_count++)
    {
        mas[v_count] = 0;

    }

    //конец вывода обнуленного массива


    //вывод вычетов по заданному модулу

    int count = 0;

    for (int i = 1; i < p; i++)
    {

        int k = i * i;

        vychet = k % p;

        if (vychet != 0)
        {
            mas[count] = vychet;
            count = count + 1;

        }

    }

    int i2 = i * i;
    vychet = i2 % p;

    for (int count = 0; count < p;)
    {
        if (qAbs(i) == mas[count])
        {
            sign = 1; goto finish;
        }
        else
        {
            count = count + 1;
        }
    }

finish:

    return(sign);
}

/*	 построение маски заданного ранга N	*/
void MaskSettings::mask_construction(int N)
{
    size_t N_to_size_t = static_cast<size_t>(N);
    size_t N_to_size_t_for_a = 2 * (2 * N_to_size_t + 1) + 1;

    std::vector<std::vector<int>> a(N_to_size_t_for_a, std::vector<int>(N_to_size_t_for_a, 0)); //создание массива стандартной маски
    std::vector<std::vector<double>> bincoded((2 * N_to_size_t + 1) *_binning, std::vector<double>((2 * N_to_size_t + 1) * _binning, 0));
    std::vector<std::vector<double>> decoded(2 * N_to_size_t + 1, std::vector<double>(2 * N_to_size_t + 1, 0));
    std::vector<std::vector<double>> mass_bp(_binning, std::vector<double>(_binning, 0));

    for (size_t i = 0; i < _binning; i++)						//заполнение массива бинов
        for (size_t j = 0; j < _binning; j++)
            mass_bp[i][j] = _vecBin[i][j];

    /*Построение массива стандартной маски*/
    for (int i = -N; i < 1; ++i)
    {
        for (int j = -N; j < 1; ++j)
        {
            if (i == 0)
            {
                a[static_cast<size_t>(i + N)][static_cast<size_t>(j + N)] = 0;
                a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + 2 * N + 1)] = 0;
                a[static_cast<size_t>(i + N)][static_cast<size_t>(j + 2 * N + 1)] = 0;
                a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + N)] = 0;
            }
            else
            {
                if (j == 0)
                {
                    a[static_cast<size_t>(i + N)][static_cast<size_t>(j + N)] = 1;
                    a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + 2 * N + 1)] = 1;
                    a[static_cast<size_t>(i + N)][static_cast<size_t>(j + 2 * N + 1)] = 1;
                    a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + N)] = 1;
                }
                else
                {
                    if (square_vychet(i, N + 1)*square_vychet(j, N + 1) == 1)
                    {
                        a[static_cast<size_t>(i + N)][static_cast<size_t>(j + N)] = 1;
                        a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + 2 * N + 1)] = 1;
                        a[static_cast<size_t>(i + N)][static_cast<size_t>(j + 2 * N + 1)] = 1;
                        a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + N)] = 1;
                    }
                    else
                    {
                        a[static_cast<size_t>(i + N)][static_cast<size_t>(j + N)] = 0;
                        a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + 2 * N + 1)] = 0;
                        a[static_cast<size_t>(i + N)][static_cast<size_t>(j + 2 * N + 1)] = 0;
                        a[static_cast<size_t>(i + 2 * N + 1)][static_cast<size_t>(j + N)] = 0;
                    }
                }
            }
        }
    }

    for (size_t i = 0; i < 2 * N_to_size_t + 1; ++i)
    {
        for (size_t j = 0; j < 2 * N_to_size_t + 1; ++j)
        {
            if (i == j) decoded[i][j] = 1;
            else if (a[i][j] == 1) decoded[i][j] = 1;
            else decoded[i][j] = minus_one_or_zero;			//ПРОЗРАЧНОСТЬ МАСКИ по умолчанию "-1"
        }
    }


    for (size_t i = 0; i < 2 * N_to_size_t + 1; i++)
    {
        for (size_t j = 0; j < 2 * N_to_size_t + 1; j++)
        {
            if (decoded[i][j] == 1.00)
            {
                for (size_t k = 0; k < _binning; k++)
                    for (size_t l = 0; l < _binning; l++)
                        bincoded[_binning * i + k][_binning * j + l] = mass_bp[k][l];
            }
            else
            {
                for (size_t k = 0; k < _binning; k++)
                    for (size_t l = 0; l < _binning; l++)
                        bincoded[_binning * i + k][_binning * j + l] = ui->transparency_of_mask->value();
            }
        }
    }

    ofstream decoded_f("output.txt", ofstream::out);	//запись в файл, он же (ВЫВОД НА ФОРМУ) binningcoded
    for (size_t i = 0; i < (N_to_size_t * 2 + 1)*_binning; i++)
    {
        for (size_t j = 0; j < (N_to_size_t * 2 + 1)*_binning; j++)
        {
            if(j != 0)
                decoded_f << " ";
            decoded_f << bincoded[i][j];
        }
        decoded_f << endl;
    }
    emit signalGenerated("output.txt");
    decoded_f.close();
}

//---------------------------------------------
//код Розы
// <-

MaskSettings::MaskSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskSettings),
    _BINNING_MAX_SIZE(15)
{
    _vecBin.resize(_BINNING_MAX_SIZE, std::vector<double>(_BINNING_MAX_SIZE, 1));
    _vecBin.shrink_to_fit();

    ui->setupUi(this);

    ui->tabWidget->setTabEnabled(CONVOLUTION, false);
    ui->tabWidget->setTabEnabled(BACKPROJECTION, false);

    connect(ui->open_txt, SIGNAL(clicked(bool)), this, SLOT(slotOpenTXT()));
    connect(ui->save_txt, SIGNAL(clicked(bool)), this, SLOT(slotSaveTXT()));

    connect(ui->binning, SIGNAL(valueChanged(int)), this, SLOT(slotBinningChanged(int)));

    connect(ui->binning_button, SIGNAL(clicked()), this, SLOT(slotTableShow()));

    connect(ui->generated, SIGNAL(clicked()), this, SLOT(slotGenerate()));
}
MaskSettings::~MaskSettings()
{
    delete ui;
}
int MaskSettings::getRank() const
{
    return ui->rank_of_mask->value();
}
int MaskSettings::getTransparency() const
{
    return ui->transparency_of_mask->value();
}

void MaskSettings::slotGenerate()
{
    ui->tabWidget->setTabEnabled(CONVOLUTION, true);
    ui->tabWidget->setTabEnabled(BACKPROJECTION, true);

    int rang = ui->rank_of_mask->value();
    minus_one_or_zero = ui->transparency_of_mask->value();
    _binning = static_cast<size_t>(ui->binning->value());

    /*----Функция для создания файла с базовым узором ранга N-------*/
    //    cout << "Rang maski = ";
    //    cin >> rang;
    mask_construction(rang - 1);

}
void MaskSettings::slotOpenTXT()
{
    QString file = QFileDialog::getOpenFileName(this,
                                                "Open",
                                                QDir::homePath(),
                                                "TXT (*.txt)");
    if(!file.isNull()){
        //ловится в классе viewer_widget
        emit signalOpenTXT(file);
        ui->save_txt->setEnabled(true);
    }
}
void MaskSettings::slotSaveTXT()
{
    emit signalSaveTXT();
}
void MaskSettings::slotBinningChanged(int value)
{
    if(value > 1)
        ui->binning_button->setEnabled(true);
    else
        ui->binning_button->setEnabled(false);
}
void MaskSettings::slotTableShow()
{
    Table table(ui->binning->value(), &_vecBin);

    if(table.exec() == QDialog::Accepted)
        table.saveData();
}
