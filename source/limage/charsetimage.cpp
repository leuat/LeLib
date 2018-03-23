#include "charsetimage.h"

CharsetImage::CharsetImage(LColorList::Type t) : MultiColorImage(t)
{
    m_width = 320;
    m_height = 200;
    m_scaleX = 1.0f;
    m_bitMask = 0b11;
    m_noColors = 4;
    m_scale = 2;
    m_width = 160;
    m_minCol = 0;
    Clear();
    m_type = LImage::Type::CharMapMulticolor;
    SetColor(1,1);
    SetColor(2,2);

    m_supports.asmExport = false;
    m_supports.binaryLoad = true;
    m_supports.binarySave = true;
    m_supports.flfSave = false;
    m_supports.flfLoad = true;
    m_supports.asmExport = false;


}

void CharsetImage::SetColor(uchar col, uchar idx)
{
    m_color.c[idx] = col;

    if (idx==0)
        m_background = col;
    for (int i=0;i<40*25;i++)
        m_data[i].c[idx] = col;

    m_extraCols[idx] = col;
}

void CharsetImage::ImportBin(QFile &file)
{
    m_rawData = file.readAll();
    FromRaw(m_rawData);
    SetColor(0, m_background); //MULTICOLOR_CHAR_COL +0
    SetColor(1, 2);// MULTICOLOR_CHAR_COL +2
    SetColor(2, 1); // MULTICOLOR_CHAR_COL +1
    SetColor(5, 3); // Color that is changeable
}

void CharsetImage::ExportBin(QFile &f)
{
    ToRaw(m_rawData);
    f.write(m_rawData);

}

void CharsetImage::FromRaw(QByteArray &arr)
{
    Clear();
    for (int i=0;i<256;i++) {
        PixelChar& pc = m_data[i];
        int idx=i*8;
        for (int j=0;j<8;j++) {
            uchar v = PixelChar::reverse(arr[i*8+j]);
            pc.p[j] = v;
        }
    }
}

void CharsetImage::ToRaw(QByteArray &arr)
{
    arr.resize(256*8);
    for (int i=0;i<256;i++) {
        PixelChar& pc = m_data[i];
        int idx=i*8;
        for (int j=0;j<8;j++)
            arr[i*8+j] = PixelChar::reverse(pc.p[j]);
    }
}

void CharsetImage::ToQPixMaps(QVector<QPixmap> &map)
{
    map.clear();
    for (int i=0;i<255;i++) {
        QImage img = m_data[i].toQImage(64, m_bitMask, m_colorList);
        QPixmap p = QPixmap::fromImage(img);
        map.append(p);
    }
    /*
    for (int i=0;i<63;i++) {

        int j=i*4;
        QImage img1 = m_data[i].toQImage(j, m_bitMask, m_colorList);
        QImage img2 = m_data[i].toQImage(j+1, m_bitMask, m_colorList);
        QImage img3 = m_data[i].toQImage(j+40, m_bitMask, m_colorList);
        QImage img4 = m_data[i].toQImage(j+41, m_bitMask, m_colorList);

        QImage img = QImage(img1.size()*2, QImage::Format_RGB32);
        int w= img1.size().width();
        int h= img1.size().height();

        for (int i=0;i<w;i++)
            for (int j=0;j<h;j++) {
                img.setPixel(i,j,img1.pixel(i,j));
                img.setPixel(i+w,j,img2.pixel(i,j));
                img.setPixel(i,j+h,img3.pixel(i,j));
                img.setPixel(i+w,j+h,img4.pixel(i,j));
            }


        QPixmap p = QPixmap::fromImage(img);
        map.append(p);
    }
*/
}

void CharsetImage::setPixel(int x, int y, unsigned int color)
{
    {

        if (x>=m_width || x<0 || y>=m_height || y<0)
            return;
        PixelChar& pc = getPixelChar(x,y);


        int ix = x % (8/m_scale);//- (dx*40);
        int iy = y % 8;//- (dy*25);

        pc.set(m_scale*ix, iy, color, m_bitMask);

    }

}
