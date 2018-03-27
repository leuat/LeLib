#ifndef CHARSETIMAGE_H
#define CHARSETIMAGE_H

#include "multicolorimage.h"
#include <QPixmap>


class CharsetImage : public MultiColorImage
{
public:
    int m_charCount = 255;
    QByteArray m_rawData;
    bool m_isMultiColor = false;
    PixelChar m_color;
    CharsetImage(LColorList::Type t);

    PixelChar m_copy[16];

    enum Mode{ FULL_IMAGE, CHARSET1x1, CHARSET2x2, CHARSET2x2_REPEAT};


    Mode m_currentMode = FULL_IMAGE;


    void SetColor(uchar col, uchar idx) override;

    void SaveBin(QFile& file);
    void LoadBin(QFile& file);

    CharsetImage* getCharset() override { return this; }
    unsigned int getPixel(int x, int y) override;

    void ImportBin(QFile& f) override;
    void ExportBin(QFile& f) override;
    virtual void FromRaw(QByteArray& arr);
    virtual void ToRaw(QByteArray& arr);
    void ToQPixMaps(QVector<QPixmap>& map);
    void setPixel(int x, int y, unsigned int color) override;

    void CopyFrom(LImage* mc);
    bool KeyPress(QKeyEvent *e) override;

    void setLimitedPixel(int x, int y, unsigned int color);



    void CopyChar() override;
    void PasteChar() override;

};

#endif // CHARSETIMAGE_H
