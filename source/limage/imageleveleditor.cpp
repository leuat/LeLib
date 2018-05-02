#include "imageleveleditor.h"
#include "source/util/util.h"

void ImageLevelEditor::SetLevel(QPoint f)
{
    // Clamp'
    f.setX(Util::clamp(f.x(), 0, m_meta.m_sizex-1));
    f.setY(Util::clamp(f.y(), 0, m_meta.m_sizey-1));

    m_currentLevelPos = f;
    if (f.x()<0 || f.x()>=m_meta.m_sizex || f.y()<0 || f.y()>=m_meta.m_sizey)
        return;
    m_currentLevel = m_levels[f.x() + f.y()*m_meta.m_sizex];


//    qDebug() << "Current colors:";

    for (int i=0;i<3;i++)
        m_extraCols[i] = m_currentLevel->m_ExtraData[i];
    m_background = m_currentLevel->m_ExtraData[0];
//        qDebug() << QString::number(m_currentLevel->m_ExtraData[i]);

    if (m_charset!=nullptr) {
        m_charset->SetColor(m_currentLevel->m_ExtraData[0], 0);
        m_charset->SetColor(m_currentLevel->m_ExtraData[1], 1);
        m_charset->SetColor(m_currentLevel->m_ExtraData[2], 2);
    }

/*    SetColor(m_currentLevel->m_ExtraData[0], 0);
    SetColor(m_currentLevel->m_ExtraData[1], 1);
    SetColor(m_currentLevel->m_ExtraData[2], 2);
*/
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

    m_supports.asmExport = false;
    m_supports.binaryLoad = false;
    m_supports.binarySave = false;
    m_supports.flfSave = true;
    m_supports.flfLoad = true;
    m_supports.asmExport = false;


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

//    qDebug() << "SetColor being set..." << QString::number(col) << " and " << QString::number(idx);

    m_currentLevel->m_ExtraData[idx] = col;
    m_extraCols[idx] = col;
    if (idx==0)
        m_background = col;
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
    CharmapLevel* ll = m_levels[0];

    qDebug() << "Chardata: " <<ll->m_CharData.size();
    qDebug() << "Colordata: " <<ll->m_CharData.size();
    qDebug() << "Extradata: " <<ll->m_ExtraData.size();

    for (CharmapLevel* l : m_levels) {
        file.write( l->m_CharData);
        file.write( l->m_ColorData);
        file.write( l->m_ExtraData);

        if (l->m_CharData.size()!= ll->m_CharData.size()) {
            qDebug() << "1";
            exit(1);
        }
        if (l->m_ColorData.size()!= ll->m_ColorData.size()) {
            qDebug() << "1";
            exit(1);
        }
        if (l->m_ExtraData.size()!= ll->m_ExtraData.size()) {
            qDebug() << "1";
            exit(1);
        }
    }

}

void ImageLevelEditor::LoadBin(QFile &file)
{
    QByteArray header = file.read(9);
    m_meta.fromHeader(header);
    m_meta.Calculate();

    Initialize(m_meta);
    qDebug() << "INChardata: " <<m_meta.dataSize();
    qDebug() << "INExtraData: " <<m_meta.m_extraDataSize;
    for (CharmapLevel* l : m_levels) {

        l->m_CharData = file.read(m_meta.dataSize());
        l->m_ColorData = file.read(m_meta.dataSize());
        l->m_ExtraData = file.read(m_meta.m_extraDataSize);
    }

    SetLevel(QPoint(0,0));
}

void ImageLevelEditor::BuildData(QTableWidget *tbl, QStringList header)
{
    int chunks = m_meta.m_dataChunks;
    int size = m_meta.m_dataChunkSize;

    tbl->setColumnCount(chunks);
    tbl->setRowCount(size);
    tbl->setVerticalHeaderLabels(header);
    for (int i=0;i<chunks;i++)
        tbl->setColumnWidth(i,55);
    int i=0;
    int j=0;
    for (int k=3;k<m_currentLevel->m_ExtraData.count();k++) {
        tbl->setItem(i,j,new QTableWidgetItem(QString::number(m_currentLevel->m_ExtraData[k])));
        if (++i>=size) {
            i=0;
            j++;
        }

    }

}

void ImageLevelEditor::StoreData(QTableWidget *tbl)
{
    int i=0;
    int j=0;

    for (int k=3;k<m_currentLevel->m_ExtraData.count();k++) {
        if (tbl->item(i,j)==nullptr)
                return;
        uchar val = tbl->item(i,j)->text().toInt();


/*        if (val!=0)
        {
            qDebug() << " data " << j << ", "<< i << "  : " << val;
        }*/
        m_currentLevel->m_ExtraData[k] = val;
        if (++i>=tbl->rowCount()) {
            i=0;
            j++;
        }

    }
}

bool ImageLevelEditor::KeyPress(QKeyEvent *e)
{
    QPoint dir(0,0);
    if (e->key()==Qt::Key_W)
        dir.setY(-1);
    if (e->key()==Qt::Key_A)
        dir.setX(-1);
    if (e->key()==Qt::Key_S)
        dir.setY(1);
    if (e->key()==Qt::Key_D)
        dir.setX(1);


    if (dir.x()!=0 || dir.y()!=0) {
        SetLevel(m_currentLevelPos + dir);
        return true;
    }
    return false;
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
    if (pos<0 || pos>=m_meta.dataSize())
        return false;

    return true;

}

void ImageLevelEditor::Fix()
{
    QByteArray data = m_currentLevel->m_CharData;
    QByteArray cols = m_currentLevel->m_ColorData;

    for (int i=0;i<m_meta.m_width;i++)
        for (int j=0;j<m_meta.m_height;j++) {
            int x = (i-1)%m_meta.m_width;
            int y = (j)%m_meta.m_height;
            m_currentLevel->m_CharData[i+j*m_meta.m_width] = data[x+y*m_meta.m_width];
            m_currentLevel->m_ColorData[i+j*m_meta.m_width] = cols[x+y*m_meta.m_width];

        }


}

void ImageLevelEditor::setPixel(int x, int y, unsigned int color)
{
    int pos;
    if (!PixelToPos(x,y, pos))
        return; // out of bounds

    if (m_writeType==Character)
        m_currentLevel->m_CharData[pos] = m_currencChar;
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
    uint val = m_charset->m_data[pos].get(ix, iy,m_charset->m_bitMask);

    if (val==m_charset->m_data[pos].c[3]) {
        val = col&0b00000111;
    }
    if ((col&0b00001000)==0b00001000)
            val+=1000;

    return val;

}

void ImageLevelEditor::CopyFrom(LImage *mc)
{
    ImageLevelEditor* c = dynamic_cast<ImageLevelEditor*>(mc);
    if (c!=nullptr) {
        if (c->m_charset==nullptr)
            return;


        CharmapGlobalData d = c->m_meta;
        //d.m_sizex = 1;
        //d.m_sizey = 1;
        Initialize(c->m_meta);

        m_currencChar = c->m_currencChar;

        for (int i=0;i<m_meta.m_sizex*m_meta.m_sizey;i++) {
            m_levels[i]->m_CharData = c->m_levels[i]->m_CharData;
            m_levels[i]->m_ColorData = c->m_levels[i]->m_ColorData;
            m_levels[i]->m_ExtraData = c->m_levels[i]->m_ExtraData;
        }
        /*for (int i=0;i<m_meta.dataSize();i++) {
            m_currentLevel->m_CharData[i]  = c->m_currentLevel->m_CharData[i];
            m_currentLevel->m_ColorData[i]  = c->m_currentLevel->m_ColorData[i];
        }
        for (int i=0;i<c->m_currentLevel->m_ExtraData.count();i++) {
            m_currentLevel->m_ExtraData[i] = c->m_currentLevel->m_ExtraData[i];
        }
        */
        m_charset = c->m_charset;
        m_writeType = c->m_writeType;
        for (int i=0;i<4;i++)
            m_extraCols[i] = c->m_extraCols[i];
        //SetLevel(QPoint(0,0));
        m_currentLevelPos = c->m_currentLevelPos;
        SetLevel(m_currentLevelPos);
        renderPathGrid = c->renderPathGrid;
    }
    else
    LImage::CopyFrom(mc);

}


void ImageLevelEditor::Resize(CharmapGlobalData newMeta)
{
    if (newMeta.m_dataChunks!=m_meta.m_dataChunks || newMeta.m_dataChunkSize!=m_meta.m_dataChunkSize) {
      //  qDebug() << "Resizing datachunks";
        m_meta.m_dataChunks = newMeta.m_dataChunks;
        m_meta.m_dataChunkSize = newMeta.m_dataChunkSize;

        m_meta.m_extraDataSize = 3 + m_meta.m_dataChunkSize*m_meta.m_dataChunks;
        m_meta.Calculate();
        for (int i=0;i<m_meta.m_sizex*m_meta.m_sizey;i++) {
            int k = m_levels[i]->m_ExtraData.count();
            m_levels[i]->m_ExtraData.resize(m_meta.m_extraDataSize);
            for (int j=k;j<m_meta.m_extraDataSize; j++)
                m_levels[i]->m_ExtraData[j]=0;
        }

    }
    if (newMeta.m_sizex!=m_meta.m_sizex || newMeta.m_sizey!=m_meta.m_sizey) {
      //  qDebug() << "Resizing datachunks";
        int osx = m_meta.m_sizex;
        int osy = m_meta.m_sizey;
        m_meta.m_dataChunks = newMeta.m_dataChunks;
        m_meta.m_dataChunkSize = newMeta.m_dataChunkSize;
        m_meta.m_sizex = newMeta.m_sizex;
        m_meta.m_sizey = newMeta.m_sizey;

        m_meta.Calculate();

        QVector<CharmapLevel*> nl;
        nl.resize(m_meta.m_sizex*m_meta.m_sizey);
        for (int i=0;i<nl.count();i++)
            nl[i] = nullptr;
        for (int i=0;i<osx;i++)
            for (int j=0;j<osy;j++)
                nl[ i + m_meta.m_sizex*j] = m_levels[i + osx*j];


        for (int i=0;i<nl.count();i++)
            if (nl[i] == nullptr)
                nl[i] = new CharmapLevel(m_meta.dataSize(), m_meta.m_extraDataSize);


        m_levels = nl;
    }

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
