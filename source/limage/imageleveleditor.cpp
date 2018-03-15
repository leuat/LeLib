#include "imageleveleditor.h"


void ImageLevelEditor::SetLevel(QPoint f)
{
    m_currentLevelPos = f;
    if (f.x()<0 || f.x()>=m_meta.m_sizex || f.y()<0 || f.y()>=m_meta.m_sizey)
        return;
    m_currentLevel = m_levels[f.x() + f.y()*m_meta.m_sizex];

  //  qDebug() << QString::number(m_currentLevel->m_ExtraData[1]);

    SetColor(m_currentLevel->m_ExtraData[0], 0);
    SetColor(m_currentLevel->m_ExtraData[1], 1);
    SetColor(m_currentLevel->m_ExtraData[2], 2);
//    SetColor(m_currentLevel->m_ExtraData[3], 3);

}

ImageLevelEditor::ImageLevelEditor(LColorList::Type t)  : MultiColorImage(t)
{
    m_width = 320;
    m_height = 200;
    m_scaleX = 1.0f;
    m_bitMask = 0b11;
    m_noColors = 4;
    m_scale = 1;
    m_minCol = 0;
    m_type = LImage::Type::LevelEditor;


}

void ImageLevelEditor::Initialize(CharmapGlobalData meta)
{
    m_meta = meta;
    m_meta.Calculate();
    m_levels.resize(m_meta.m_sizex*m_meta.m_sizey);
    for (int i=0;i<m_meta.m_sizex;i++)
        for (int j=0;j<m_meta.m_sizey;j++)
            m_levels[i + j*m_meta.m_sizex] = new
                    CharmapLevel(m_meta.dataSize(), m_meta.m_extraDataSize);

    //SetLevel(QPoint(0,0));

    m_currentLevel = m_levels[0];

}

void ImageLevelEditor::SetColor(uchar col, uchar idx)
{
    if (m_charset==nullptr)
        return;
    m_currentLevel->m_ExtraData[idx] = col;
    m_charset->SetColor(col, idx);
}

void ImageLevelEditor::Clear()
{
    if (m_currentLevel!=nullptr)
        m_currentLevel->Clear();
}

void ImageLevelEditor::SaveBin(QFile &file)
{
    file.write(m_meta.toHeader());
    int i=0;
    for (CharmapLevel* l : m_levels) {
        file.write( l->m_CharData);
        file.write( l->m_ColorData);
        file.write( l->m_ExtraData);
    }

}

void ImageLevelEditor::LoadBin(QFile &file)
{
    QByteArray header = file.read(9);
    m_meta.fromHeader(header);
    Initialize(m_meta);
    m_meta.Calculate();
    for (CharmapLevel* l : m_levels) {
        l->m_CharData = file.read(m_meta.dataSize());
        l->m_ColorData = file.read(m_meta.dataSize());
        l->m_ExtraData = file.read(m_meta.m_extraDataSize);
    }

    qDebug() << "Loadbin Setlevel";
    SetLevel(QPoint(0,0));
    qDebug() << QString::number(m_currentLevel->m_ExtraData[1]);
}



CharmapLevel *ImageLevelEditor::getLevel(int i, int j)
{
    return m_levels[i+j*m_meta.m_sizex];
}


QVector<QPixmap> ImageLevelEditor::CreateIcons()
{
    QVector<QPixmap> lst;
    for (int i=0;i<m_meta.m_sizex;i++)
        for (int j=0;j<m_meta.m_sizey;j++) {
            QImage img = getLevel(i,j)->createImage(64,m_colorList,m_meta.m_width, m_meta.m_height);
            QPixmap pixmap = QPixmap::fromImage(img);
            //QIcon icon(pixmap);
            lst.append(pixmap);
        }
    return lst;
}


bool ImageLevelEditor::PixelToPos(int x, int y, int& pos)
{
    if (x>=320 || x<0 || y>=200 || y<0)
        return false;

    x/=16;
    y/=16;
    x=x-m_meta.m_startx;
    y=y-m_meta.m_starty*0.5;

    pos = x + y*m_meta.m_width;
    if (pos<0 || pos>m_meta.dataSize())
        return false;

    return true;

}

void ImageLevelEditor::setPixel(int x, int y, unsigned int color)
{
    int pos;
    if (!PixelToPos(x,y, pos))
        return; // out of bounds

    if (m_writeType==Character)
        m_currentLevel->m_CharData[pos] = color;
    if (m_writeType==Color)
        m_currentLevel->m_ColorData[pos] = color;

    //BuildImage();
}

unsigned int ImageLevelEditor::getPixel(int x, int y)
{
    if (m_charset==nullptr)
        return 0;
    int pos;
    if (!PixelToPos(x,y, pos))
        return 0; // out of bounds


    int shift=0;
    if (x%16>=8) shift+=1;
    if (y%16>=8) shift+=40;


    uchar v = m_currentLevel->m_CharData[pos];
    uchar col = m_currentLevel->m_ColorData[pos];
    int ix = (x % (8)/2)*2;//- (dx*40);
    int iy = y % 8;//- (dy*25);

 //   return pc.get(m_scale*ix, iy, m_bitMask);



    pos = v + shift;

//    return m_charset->m_data[pos].get(v + (2*x)&7, v+ y&7,m_bitMask);
    uchar val = m_charset->m_data[pos].get(ix, iy,m_charset->m_bitMask);

    if (val==m_charset->m_data[pos].c[3])
        val = col;


    return val;

}

void ImageLevelEditor::CopyFrom(LImage *mc)
{
    ImageLevelEditor* c = dynamic_cast<ImageLevelEditor*>(mc);
    if (c!=nullptr) {
        if (c->m_charset==nullptr)
            return;


        CharmapGlobalData d = c->m_meta;
        d.m_sizex = 1;
        d.m_sizey = 1;
        Initialize(c->m_meta);

        for (int i=0;i<m_meta.dataSize();i++) {
            m_currentLevel->m_CharData[i]  = c->m_currentLevel->m_CharData[i];
            m_currentLevel->m_ColorData[i]  = c->m_currentLevel->m_ColorData[i];
        }
        for (int i=0;i<c->m_currentLevel->m_ExtraData.count();i++) {
            m_currentLevel->m_ExtraData[i] = c->m_currentLevel->m_ExtraData[i];
        }
        m_charset = c->m_charset;
        m_writeType = c->m_writeType;
        //SetLevel(QPoint(0,0));

    }
    else
    LImage::CopyFrom(mc);

}




int CharmapGlobalData::levelSize() const
{
    return m_levelSize;
}

int CharmapGlobalData::totalSize() const
{
    return m_totalSize;
}

int CharmapGlobalData::dataSize() const
{
    return m_dataSize;
}

QImage CharmapLevel::createImage(int size, LColorList& lst, int width, int height)
{
    QImage img = QImage(size, size, QImage::Format_ARGB32);
    for (int i=0;i<size;i++)
        for (int j=0;j<size;j++) {
            QColor c = QColor(0,0,0);
            int x = (i/(float)size)*(float)width;
            int y = (j/(float)size)*(float)height;
            int pos = x+ width*y;
            uchar val = 32;
            uchar colval = 0;
            if (pos>=0 && pos<m_CharData.count()) {
                val = m_CharData[pos];
                colval = m_ColorData[pos];
            }
            if (val==32)
                colval = 0;
            img.setPixel(i,j, lst.m_list[colval].color.rgba());
        }

    return img;
}
