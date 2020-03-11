#ifndef VIEWER_PROCESSOR_H
#define VIEWER_PROCESSOR_H

#include <memory>
#include <QSettings>
#include <QFileInfo>

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
        GENERAL_CALIBRATION = 0x4,
        TOT_MODE = 0x8
    };
    Q_DECLARE_FLAGS(Markers_Flags, marker)
    Viewer_Processor::Markers_Flags _markers;

    enum  class fileType {UNDEFINED, TXT, CLOG};

    fileType getFileType() const;
    QString getFileName() const;
    const QFile *getFile() const;
    size_t getColumns() const;
    size_t getRows() const;
    float getDataInVec2D(size_t column, size_t row);
    const std::vector<std::vector<float> > &getVec2D() const;

    bool setFileName(const QString &fileName);
    void setSettings(std::shared_ptr<const QSettings> spSettings);
    void setDataInVec2D(size_t column_number, size_t row_number, float value);

    std::vector<std::vector<float> > cutVec2D(size_t fromColumn, size_t fromRow, size_t width, size_t height);

    virtual QImage getImage() = 0;
    virtual QImage getRedrawnImage() = 0;

    virtual void clear() = 0;

protected:
    fileType _fileType;
    std::shared_ptr<const QSettings> _spSettings;
    QFile _file;

    std::vector<std::vector<float>> _vec2D;
    std::vector<std::vector<float>> _vec2DMask;

    size_t _rows;
    size_t _columns;

    float findMaxInVec2D();
    float findMinInVec2D();
    float rangeConverter(float value, float From1, float From2, float To1, float To2);
    void createFrameInVec2D();
    void createMaskInVec2D();
    void rebuildVec2DAccordingToSettings();
    void rebuildImageAccordingToSettings(QImage &image);

    //повороты
//    QImage rotatePlus();
//    QImage rotateMinus();

    void setFileType(const QString &fileName);
    void allocateEmptyVec2D(std::vector<std::vector<float> > &vec2D, size_t columns, size_t rows);
    QImage getImageFromVec2D();
    bool checkSettingsPtr();

    virtual bool createVec2D() = 0;
    virtual void resetDataToDefault() = 0;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(Viewer_Processor::Markers_Flags)

#endif // VIEWER_PROCESSOR_H
