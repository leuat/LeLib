#ifndef C64FullScreenChar_H
#define C64FullScreenChar_H

#include "multicolorimage.h"
#include "charsetimage.h"

class C64FullScreenChar: public MultiColorImage
{
public:
    QByteArray m_rawData;
    bool m_isMultiColor = false;

    int m_charWidth = 40;
    int m_charHeight = 25;

    CharsetImage* m_charset;

    PixelChar m_color;
    C64FullScreenChar(LColorList::Type t);


    void SetColor(uchar col, uchar idx);

    void ImportBin(QFile& f) override;
    void ExportBin(QFile& f) override;
//    void FromRaw(QByteArray& arr);
//    void ToRaw(QByteArray& arr);
    void BuildImage();

    void setPixel(int x, int y, unsigned int color) override;
    unsigned int getPixel(int x, int y) override;
    void CopyFrom(LImage* mc) override;
};

#endif // CHARSETIMAGE_H
