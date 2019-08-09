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

/*	создание массива рангом N	*/
int** MaskSettings::dynamicArrayInt(int N, int M)
{
    int** arrayPtr = nullptr;

    arrayPtr = new int *[2 * N + 1];

    for (int i = 0; i < 2 * N + 1; ++i)
        arrayPtr[i] = new int[2 * M + 1];

    return arrayPtr;
}

double** MaskSettings::dynamicArrayDouble(int N, int M)
{
    double** arrayPtr = nullptr;

    arrayPtr = new double *[N];

    for (int i = 0; i < N; ++i)
        arrayPtr[i] = new double[M];

    return arrayPtr;
}


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
    int** a = nullptr;
    a = dynamicArrayInt(2 * N + 1, 2 * N + 1);	//создание массива стандартной маски

    double** bincoded = dynamicArrayDouble((2 * N + 1)*binning, (2 * N + 1)*binning);	//создание массива бининговой маски

    double** decoded = nullptr;
    decoded = dynamicArrayDouble(2 * N + 1, 2 * N + 1);	//создание массива декодирующей функции

    double** mass_bp = nullptr;
    mass_bp = dynamicArrayDouble(binning, binning);		//массив бинов


    for (int i=0;i<binning;i++)						//заполнение массива бинов
        for (int j = 0; j < binning; j++) {
            mass_bp[i][j] = double(arrayBin[i][j]);
        }

    /*Построение массива стандартной маски*/
    for (int i = -N; i < 1; ++i)
        for (int j = -N; j < 1; ++j) {
            if (i == 0) {
                a[i + N][j + N] = 0;
                a[i + 2 * N + 1][j + 2 * N + 1] = 0;
                a[i + N][j + 2 * N + 1] = 0;
                a[i + 2 * N + 1][j + N] = 0;
            }
            else {
                if (j == 0) {
                    a[i + N][j + N] = 1;
                    a[i + 2 * N + 1][j + 2 * N + 1] = 1;
                    a[i + N][j + 2 * N + 1] = 1;
                    a[i + 2 * N + 1][j + N] = 1;
                }
                else {
                    if (square_vychet(i, N + 1)*square_vychet(j, N + 1) == 1) {
                        a[i + N][j + N] = 1;
                        a[i + 2 * N + 1][j + 2 * N + 1] = 1;
                        a[i + N][j + 2 * N + 1] = 1;
                        a[i + 2 * N + 1][j + N] = 1;
                    }
                    else {
                        a[i + N][j + N] = 0;
                        a[i + 2 * N + 1][j + 2 * N + 1] = 0;
                        a[i + N][j + 2 * N + 1] = 0;
                        a[i + 2 * N + 1][j + N] = 0;
                    }
                }
            }
        }

//    for (int i = 0; i < 2 * N + 1; ++i) {
//        for (int j = 0; j < 2 * N + 1; ++j) {
//            cout << a[i][j] << " ";	//вывод массива стандартной маски
//        }
//        cout << endl;
//    }
//    cout << endl << endl;

//    cout << endl << endl;

    for (int i = 0; i < 2 * N + 1; ++i) {
        for (int j = 0; j < 2 * N + 1; ++j) {
            if (i == j) decoded[i][j] = 1;
            else if (a[i][j] == 1) decoded[i][j] = 1;
            else decoded[i][j] = minus_one_or_zero;			//ПРОЗРАЧНОСТЬ МАСКИ по умолчанию "-1"
//            cout << decoded[i][j] << " "; //вывод массива декодирующей функции
        }
//        cout << endl;
    }


    for (int i = 0; i < 2 * N + 1; i++) {

        for (int j = 0; j < 2 * N + 1; j++) {

            {
                if (decoded[i][j] == 1.00) {

                    for (int k = 0; k < binning; k++)
                        for (int l = 0; l < binning; l++) {

                            bincoded[binning * i + k][binning * j + l] = mass_bp[k][l];
                        }
                }
                else
                {
                    for (int k = 0; k < binning; k++)
                        for (int l = 0; l < binning; l++) {

                            bincoded[binning * i + k][binning * j + l] = ui->transparency_of_mask->value();
                        }
                }
            }
        }
    }

    ofstream decoded_f("output.txt", ofstream::out);	//запись в файл, он же (ВЫВОД НА ФОРМУ) binningcoded
    for (int i = 0; i < (N * 2 + 1)*binning; i++){
        for (int j = 0; j < (N * 2 + 1)*binning; j++){
            if(j != 0)
                decoded_f << " ";
            decoded_f << bincoded[i][j];
        }
        decoded_f << endl;
    }
    emit signalGenerated("output.txt");
    decoded_f.close();

    for (int i = 0; i < 2 * N + 1; ++i)	//очистка памяти
        delete[] a[i];
    delete a;

    for (int i = 0; i < (2 * N + 1)*binning; ++i)	//очистка памяти
        delete[] bincoded[i];
    delete bincoded;

    for (int i = 0; i < 2 * N + 1; ++i)	//очистка памяти
        delete[] decoded[i];
    delete decoded;

    for (int i = 0; i < binning; ++i)	//очистка памяти
        delete[] mass_bp[i];
    delete mass_bp;
}

//---------------------------------------------
//код Розы
// <-

MaskSettings::MaskSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskSettings)
{
    //выделение памяти
    arrayBin = new double* [BINNING_MAX_SIZE];
    for (int i = 0; i < BINNING_MAX_SIZE ; ++i)
        arrayBin[i] = new double[BINNING_MAX_SIZE ];

    for (int row = 0; row < BINNING_MAX_SIZE; ++row)
        for (int column = 0; column < BINNING_MAX_SIZE; ++column)
            arrayBin[row][column]= 1.00;

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

    for (int i = 0; i < BINNING_MAX_SIZE ; ++i)
        delete [] arrayBin[i];
    delete [] arrayBin;
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
    binning = ui->binning->value();

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
    Table* table = new Table(ui->binning->value(), arrayBin);

    if(table->exec() == QDialog::Accepted)
        table->saveData();

    delete table;
}
