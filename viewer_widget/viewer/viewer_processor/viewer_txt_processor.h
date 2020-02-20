#ifndef VIEWER_TXT_PROCESSOR_H
#define VIEWER_TXT_PROCESSOR_H

#include "viewer_processor.h"

class Viewer_Txt_Processor : public Viewer_Processor
{
public:
    Viewer_Txt_Processor();
    virtual QImage getImage() override;
    virtual QImage getRedrawnImage() override;

    virtual void clear() override;

private:
    virtual void createVec2D() override;
    virtual void resetDataToDefault() override;
};

#endif // VIEWER_TXT_PROCESSOR_H
