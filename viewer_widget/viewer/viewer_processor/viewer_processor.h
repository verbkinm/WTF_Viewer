#ifndef VIEWER_PROCESSOR_H
#define VIEWER_PROCESSOR_H

#include <memory>
#include <QSettings>

class Viewer_Processor
{
public:
    Viewer_Processor();
    virtual ~Viewer_Processor();

    enum marker
    {
        NO_MARKERS = 0x0,
        BORDER = 0x1,
        MASKING = 0x2,
        GENERAL_CALIBRATION = 0x4
    };
    Q_DECLARE_FLAGS(Markers_Flags, marker)
    Viewer_Processor::Markers_Flags _markers;

    enum  class fileType {UNDEFINED, TXT, CLOG};

    fileType getFileType() const;
    QString getFileName() const;
    size_t getColumns() const;
    size_t getRows() const;
    double getDataInVec2D(size_t column, size_t row);
    const std::vector<std::vector<double> > &getVec2D() const;

    void setFileName(const QString &fileName);
    void setSettings(std::shared_ptr<const QSettings> spSettings);
    void setDataInVec2D(size_t column_number, size_t row_number, double value);

    std::vector<std::vector<double> > cutVec2D(size_t fromColumn, size_t fromRow, size_t width, size_t height);

    virtual QImage getImage() = 0;
    virtual QImage getRedrawnImage() = 0;

protected:
    fileType _fileType;
    std::shared_ptr<const QSettings> _spSettings;
    QString _fileName;

    std::vector<std::vector<double>> _vec2D;
    std::vector<std::vector<double>> _vec2DMask;

    size_t _rows;
    size_t _columns;

    double findMaxInVec2D();
    double findMinInVec2D();
    double rangeConverter(double value, double From1, double From2, double To1, double To2);
    void createFrameInVec2D();
    void createMaskInVec2D();
    void rebuildVec2DAccordingToSettings();
    void rebuildImageAccordingToSettings(QImage &image);

    //повороты
//    QImage rotatePlus();
//    QImage rotateMinus();

    void setFileType(const QString &fileName);
    void allocateEmptyVec2D(std::vector<std::vector<double> > &vec2D, size_t columns, size_t rows);
    QImage getImageFromVec2D();
    bool checkSettingsPtr();

    virtual void createVec2D() = 0;
    virtual void resetDataToDefault() = 0;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Viewer_Processor::Markers_Flags)

#endif // VIEWER_PROCESSOR_H
