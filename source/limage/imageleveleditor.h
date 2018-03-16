#ifndef IMAGELEVELEDITOR_H
#define IMAGELEVELEDITOR_H

#include "multicolorimage.h"
#include "charsetimage.h"
#include <QByteArray>
#include <QVector>


class CharmapLevel {
public:
//    uchar m_col0, m_col1, m_col2;
    QByteArray m_CharData;
    QByteArray m_ColorData;
    QByteArray m_ExtraData;


    void Clear() {
        for (int i=0;i<m_CharData.count();i++) {
            m_CharData[i] = 0x20;
            m_ColorData[i] = 0x5;
        }
    }


    CharmapLevel(int sizeChar, int sizeExtraData) {
        m_CharData.resize(sizeChar);
        m_ColorData.resize(sizeChar);
        m_ExtraData.resize(sizeExtraData);
        m_ExtraData.fill(0);


        m_ExtraData[0] = 0;
        m_ExtraData[1] = 1; // Colors
        m_ExtraData[2] = 2;

        Clear();
    }

    QImage createImage(int size, LColorList& lst, int width, int height);

};


class CharmapGlobalData {
private:
    int m_dataSize;
    int m_levelSize;
    int m_totalSize;
    int m_headerSize;

public:
    int m_width, m_height;
    int m_sizex, m_sizey;
    int m_startx, m_starty;
    int m_extraDataSize;

    int scale = 1;
    bool m_multiChar = true;

    int dataSize() const;
    int levelSize() const;
    int totalSize() const;

    void Calculate()
    {
        m_dataSize = m_width*m_height;
        m_levelSize = 2*m_dataSize + m_extraDataSize;
        m_headerSize = 1 + 1 + 1 + 1 + 1 + 1 + 2 + 1;
        // w/h sx/sy  stx/sty   levelSize
        m_totalSize = m_levelSize*m_sizex*m_sizey + m_headerSize;
    }

    QByteArray toHeader() {
        QByteArray ba;
        ba.resize(9);
        ba[0] = m_sizex;
        ba[1] = m_sizey;
        ba[2] = m_width;
        ba[3] = m_height;
        ba[4] = m_startx;
        ba[5] = m_starty;
        ba[6] = (uchar)(m_levelSize >>8)&0xFF;
        ba[7] = (uchar)(m_levelSize&0xFF);
        ba[8] = m_extraDataSize;
        return ba;
    }

    void fromHeader(QByteArray ba) {
        m_sizex = ba[0];
        m_sizey = ba[1];
        m_width = ba[2];
        m_height = ba[3];
        m_startx = ba[4];
        m_starty = ba[5];

        m_levelSize = ba[7] + ba[6]<<8;
        m_extraDataSize = ba[8];

    }



};







class ImageLevelEditor : public MultiColorImage
{
public:
    QPoint m_currentLevelPos = QPoint(0,0);
    CharmapLevel* m_currentLevel = nullptr;
    QVector<CharmapLevel*> m_levels;

    CharmapLevel* getLevel(int i, int j);
    CharmapGlobalData m_meta;

    CharsetImage* m_charset = nullptr;

    void SetLevel(QPoint f);
    ImageLevelEditor(LColorList::Type t);
    void Initialize(CharmapGlobalData meta);


    void SetColor(uchar col, uchar idx) override;
    void Clear() override;
    void SaveBin(QFile& f) override;
    void LoadBin(QFile& f) override;
//    void FromRaw(QByteArray& arr);
//    void ToRaw(QByteArray& arr);



    QVector<QPixmap> CreateIcons();

    void setPixel(int x, int y, unsigned int color) override;
    unsigned int getPixel(int x, int y) override;
    void CopyFrom(LImage* mc) override;


    bool PixelToPos(int x, int y, int& pos);

};

#endif // IMAGELEVELEDITOR_H