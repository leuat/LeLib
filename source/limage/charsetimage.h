#ifndef CHARSETIMAGE_H
#define CHARSETIMAGE_H

#include "multicolorimage.h"


class CharsetImage : public MultiColorImage
{
public:
    QByteArray m_rawData;
    bool m_isMultiColor = false;
    PixelChar m_color;
    CharsetImage(LColorList::Type t);


    void SetColor(uchar col, uchar idx);

    void ImportBin(QFile& f) override;
    void ExportBin(QFile& f) override;
    void FromRaw(QByteArray& arr);
    void ToRaw(QByteArray& arr);

    void setPixel(int x, int y, unsigned int color) override;
};

#endif // CHARSETIMAGE_H
