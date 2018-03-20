#ifndef LIMAGE_H
#define LIMAGE_H

#include <QString>
#include <QImage>
#include "source/limage/lcolorlist.h"
#include <QVector3D>
#include <QGraphicsEffect>
#include <QGraphicsBlurEffect>
#include <QPoint>
#include <QFile>
#include <QDebug>
#include <QLabel>
#include <QTableWidget>

class LImageSupports {
public:
    bool binarySave = false;
    bool binaryLoad = false;
    bool flfSave = false;
    bool flfLoad = false;
    bool asmExport = false;
};


class LImage
{
public:
    enum Type { QImageBitmap, MultiColorBitmap, HiresBitmap,
                NotSupported, Tiff, CharMapMulticolor, FullScreenChar, LevelEditor, CharmapRegular};


    enum WriteType { Color, Character };
    LImage() {}
    LImage(LColorList::Type t);
    ~LImage() {
        Release();
    }
    LImageSupports m_supports;

    static unsigned char TypeToChar(Type t);
    static Type CharToType(unsigned char c);


    int m_width;
    int m_height;
    float m_scaleX = 0.6f;
    unsigned int m_border=0, m_background=0;
    Type m_type = Type::QImageBitmap;
    WriteType m_writeType = WriteType::Color;
    unsigned char m_bitMask = 0b11;
    unsigned char m_scale = 2;
    unsigned char m_noColors = 4;
    unsigned char m_minCol = 1;

    bool renderPathGrid = true;

    unsigned char m_extraCols[4];

    LColorList m_colorList;


    virtual void Initialize(int width, int height) = 0;

    virtual void setPixel(int x, int y, unsigned int color) = 0;
    virtual unsigned int getPixel(int x, int y) = 0;
    virtual void SetColor(uchar col, uchar idx) {}

    virtual void LoadCharset(QString file) {}

    virtual void KeyPress(QKeyEvent *e) {}
    virtual void SaveBin(QFile &file) = 0;
    virtual void LoadBin(QFile &file) = 0;


    virtual void BuildData(QTableWidget* tbl, QStringList header) {}
    virtual void StoreData(QTableWidget* tbl) {}

    virtual QPoint GetCurrentPosInImage(float x, float y) {
        return QPoint(x,y);
    }

    virtual void ExportBin(QFile &file) {}
    virtual void ImportBin(QFile &file) {}

    virtual void Release() {}

    virtual void ApplyToLabel(QLabel* l) = 0;

    virtual void setForeground(unsigned int col) { m_border = col; }
    virtual void setBackground(unsigned int col) {m_background = col; }

    void drawLine(float x0, float y0, float x1, float y1, unsigned int col, int size);

    void Box(int x, int y, unsigned char col, int size);

    virtual void ToQImage(LColorList& lst, QImage* img, float zoom = 1, QPointF center = QPointF(160,100)) = 0;

    virtual void CopyFrom(LImage* img) {
        #pragma omp parallel for

        for (int i=0;i<m_width;i++)
            for (int j=0;j<m_height;j++)
                setPixel(i,j,img->getPixel(i,j));
    }

    virtual void SetCurrentType(WriteType wt) {
        m_writeType = wt;
    }

    void CopyTo(LImage* img);

    virtual void Clear() = 0;
    virtual void fromQImage(QImage* img, LColorList& lst) = 0;

    virtual void ExportAsm(QString filename)  { qDebug() << "ASM Write not supported"; }

};


#endif // LIMAGE_H
