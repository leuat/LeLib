#include "c64fullscreenchar.h"

C64FullScreenChar::C64FullScreenChar(LColorList::Type t) : MultiColorImage(t)
{
    m_charset = new CharsetImage(t);
    QFile f("C:/Users/leuat/OneDrive/Documents/GitHub/pmm/pmm/charsets/test_mc.bin");
    f.open(QIODevice::ReadOnly);
    m_charset->ImportBin(f);
    m_width = 160;
    m_height = 200;
    m_scaleX = 1.0f;
    m_bitMask = 0b11;
    m_noColors = 4;
    m_scale = 2;
    m_minCol = 0;
    Clear();
    m_type = LImage::Type::FullScreenChar;

    m_rawData.resize(m_width*m_height);

}

void C64FullScreenChar::SetColor(uchar col, uchar idx)
{
    m_charset->SetColor(col, idx);
}

void C64FullScreenChar::ImportBin(QFile &f)
{
    m_rawData = f.readAll();
//    FromRaw(m_rawData);

}

void C64FullScreenChar::ExportBin(QFile &f)
{
 //   ToRaw(m_rawData);
    f.write(m_rawData);
}

void C64FullScreenChar::BuildImage()
{
    for (int i=0;i<m_charWidth;i++)
        for (int j=0;j<m_charHeight;j++) {
            uchar val = m_rawData[i*m_charHeight + j];
            m_data[i] = m_charset->m_data[val*8];
        }

}

void C64FullScreenChar::setPixel(int x, int y, unsigned int color)
{
    m_rawData[x/4 + (y/8)*m_charHeight] = color;
    BuildImage();
}

unsigned int C64FullScreenChar::getPixel(int x, int y)
{
    uchar v = m_rawData[(x/4) + (y/8)*m_height];
    int pos = v*8;
    return m_charset->m_data[pos].get(x%8, y%8,m_bitMask);

}

void C64FullScreenChar::CopyFrom(LImage *mc)
{
    LImage::CopyFrom(mc);

}
