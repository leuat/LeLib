#ifndef MULTICOLORIMAGE_H
#define MULTICOLORIMAGE_H

#include <QVector>
#include "source/limage/lcolorlist.h"
#include "source/limage/limage.h"
#include <QImage>
#include <QByteArray>

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

class PixelChar {
public:
    PixelChar();
    unsigned char p[8];
    unsigned char c[4];
    unsigned char get(int x, int y, unsigned char bitMask);
    void set(int x, int y, unsigned char color, unsigned char bitMask, unsigned char maxCol, unsigned char minCol);
    void set(int x, int y, unsigned char color, unsigned char bitMask);
    void Clear(unsigned char background);
    QString bitmapToAssembler();
    QString colorMapToAssembler(int i, int j);
    uchar colorMapToNumber(int i, int j);
    QByteArray data();
    QString colorToAssembler();
    bool isEmpty();
    bool isEqualBytes(PixelChar& o);
    void Reorganize(unsigned char bitMask, unsigned char Scale,unsigned char minCol, unsigned char maxCol);
    int Count(unsigned int col, unsigned char bitMask, unsigned char Scale);
    static unsigned char reverse(unsigned char b) {
       b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
       b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
       b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
       return b;
    }
};


/*/class CharPos {
public:
    char m_charIndex, m_colorIndex;
    uint m_x;
    uint m_y;

    CharPos() {}

    CharPos(uint x,uint y, char charIndex, char colorIndex) {
        m_charIndex = charIndex;
        m_x = x;
        m_y = y;
        m_colorIndex = colorIndex;
    }

};
*/







class MultiColorImage  : public LImage
{
public:
  //  (x shift, x count, data, x shift, x count, data)

    QVector<PixelChar> m_organized;
//    QVector<CharPos> m_charPos;
    QByteArray m_outputData;
    int m_Index = 0;

    MultiColorImage(LColorList::Type t);
    PixelChar m_data[40*25];
    PixelChar& getPixelChar(int x, int y);
    void Clear() override;

    int LookUp(PixelChar pc);

    void CalculateCharIndices();
    int Eat(int start, int add);
    void SaveCharRascal(QString file, QString name);

//    unsigned char m_border=0, m_background=0;

    void setPixel(int x, int y, unsigned int color) override;
    unsigned int getPixel(int x, int y) override;

    void Reorganize();

    void SaveBin(QFile& f) override;
    void LoadBin(QFile& f) override;

    void Initialize(int width, int height) override {}

    void setForeground(unsigned int col) override;
    void setBackground(unsigned int col) override;

    void ToQImage(LColorList& lst, QImage* img, float zoom, QPointF center) override;

    void Release() override {}

    void ApplyToLabel(QLabel* l) override {}

/*    void drawLine(float x0, float y0, float x1, float y1, unsigned int col, int size);

    void Box(int x, int y, unsigned char col, int size);
*/
//    void ToQImage(LColorList& lst, QImage* img, float zoom = 1, QPoint center = QPoint(160,100));
    void fromQImage(QImage* img, LColorList& lst) override;

    void CopyFrom(LImage* mc) override;


    void ExportAsm(QString filename) override;
    void ExportRasBin(QString filename, QString name);

};

#endif // MULTICOLORIMAGE_H
