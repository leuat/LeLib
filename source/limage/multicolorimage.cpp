#include "source/limage/multicolorimage.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include "source/limage/standardcolorimage.h"
#include "source/limage/charsetimage.h"
#include <typeinfo>
#include "source/util/util.h"

MultiColorImage::MultiColorImage(LColorList::Type t) : LImage(t)
{
    m_width = 160;
    m_height = 200;
    m_scaleX = 2.5f;
    Clear();
    m_type = LImage::Type::MultiColorBitmap;
    m_supports.asmExport = true;
    m_supports.binaryLoad = false;
    m_supports.binarySave = false;
    m_supports.flfSave = true;
    m_supports.flfLoad = true;
    m_supports.asmExport = true;


}

void MultiColorImage::setPixel(int x, int y, unsigned int color)
{

    if (x>=m_width || x<0 || y>=m_height || y<0)
        return;
    PixelChar& pc = getPixelChar(x,y);

    pc.Reorganize(m_bitMask, m_scale,m_minCol, m_noColors);

    int ix = x % (8/m_scale);//- (dx*40);
    int iy = y % 8;//- (dy*25);

    //if (ix==0 || ix == 2 || ix == 4 || ix == 6)
    pc.set(m_scale*ix, iy, color, m_bitMask, m_noColors, m_minCol);

}

unsigned int MultiColorImage::getPixel(int x, int y)
{

    if (x>=m_width || x<0 || y>=m_height || y<0)
        return 0;
    PixelChar& pc = getPixelChar(x,y);

    int ix = x % (8/m_scale);//- (dx*40);
    int iy = y % 8;//- (dy*25);

    return pc.get(m_scale*ix, iy, m_bitMask);

}

void MultiColorImage::setForeground(unsigned int col)
{
    m_border = col;
}

void MultiColorImage::setBackground(unsigned int col)
{
    m_background = col;
    for (int i=0;i<40*25;i++) {
        m_data[i].c[0] = col;
    }
}

void MultiColorImage::Reorganize()
{
#pragma omp parallel for
    for (int i=0;i<40*25;i++)
        m_data[i].Reorganize(m_bitMask, m_scale, m_minCol, m_noColors);
}

void MultiColorImage::SaveBin(QFile& file)
{
    file.write( ( char * )( &m_background ),  1 );
    file.write( ( char * )( &m_border ), 1 );
    file.write( ( char * )( &m_data ),  25*40*12 );

}

void MultiColorImage::LoadBin(QFile& file)
{
    file.read( ( char * )( &m_background ),1 );
    file.read( ( char * )( &m_border ), 1);
    file.read( ( char * )( &m_data ),  25*40*12 );

}

void MultiColorImage::fromQImage(QImage *img, LColorList &lst)
{
#pragma omp parallel for
    for (int i=0;i<m_width;i++)
        for (int j=0;j<m_height;j++) {
            unsigned char col = lst.getIndex(QColor(img->pixel(i, j)));
            setPixel(i,j,col);
        }
 //   Reorganize();

}

void MultiColorImage::CopyFrom(LImage* img)
{
    MultiColorImage* mc = dynamic_cast<MultiColorImage*>(img);
    //if ((typeid(*img) == typeid(MultiColorImage)) || (typeid(*img) == typeid(StandardColorImage))
    //        || (typeid(*img) == typeid(CharsetImage)))
    if (mc!=nullptr)
    {
        MultiColorImage* mc = (MultiColorImage*)img;
         m_background = mc->m_background;
         m_border = mc->m_border;
        // qDebug() << "COPY FROM";
#pragma omp parallel for
         for(int i=0;i<25*40;i++) {
             for (int j=0;j<8;j++)
                 m_data[i].p[j] = mc->m_data[i].p[j];
             for (int j=0;j<4;j++)
                 m_data[i].c[j] = mc->m_data[i].c[j];
         }
    }
    else
    {
        LImage::CopyFrom(img);
        return;
    }

}

void MultiColorImage::ExportAsm(QString filename)
{
    QString filen = filename.split(".")[0];
    ExportRasBin(filen,"");

    return;
    // Fuck this
    if (QFile::exists(filename))
         QFile::remove(filename);
    QFile file(filename);
    if (!file.open(QFile::Append))
        return;
    QTextStream s(&file);


    s << "*=$0801\n";
    s << "  BYTE    $0E, $08, $0A, $00, $9E, $20, $28,  $32, $33\n";
    s << "  BYTE    $30, $34, $29, $00, $00, $00\n";
    s << "*=$0900\n";


    s << "IMG_CHARSET = $2000\n";
    s << "IMG_SCREENDATA = $4000\n";
    s << "IMG_COLORDATA = IMG_SCREENDATA + 1002\n\n";

    s << "display_image\n";

    s << "  lda IMG_SCREENDATA\n";
    s << "  sta $d020 ; background \n";
    s << "  lda IMG_SCREENDATA+1 \n";
    s << "  sta $d021; foreground \n";
    s << "  ldx #$00 \n";
    s << "loaddccimage\n";
    s << "  lda IMG_SCREENDATA+2,x\n";
    s << "  sta $0400,x \n";
    s << "  lda IMG_SCREENDATA + #$102,x \n";
    s << "  sta $0500,x \n";
    s << "  lda IMG_SCREENDATA + #$202,x \n";
    s << "  sta $0600,x \n";
    s << "  lda IMG_SCREENDATA + #$302,x \n";
    s << "  sta $0700,x \n";
    s << "\n";
    s << "; Color \n";
    s << "  lda IMG_COLORDATA,x \n";
    s << "  sta $d800,x \n";
    s << "  lda IMG_COLORDATA+ #$100,x \n";
    s << "  sta $d900,x \n";
    s << "  lda IMG_COLORDATA+ #$200,x \n";
    s << "  sta $da00,x \n";
    s << "  lda IMG_COLORDATA+ #$300,x \n";
    s << "  sta $db00,x \n\n";
    s << "  inx \n";
    s << "  bne loaddccimage \n";

    s << " ; set bitmap mode \n";
    s << "  lda #$3b \n";
    s << "  sta $d011 \n";
    s << "; Set multicolor mode \n";
    s << "  lda #$18 \n";
    s << "  sta $d016 \n";
    s << "; Set bitmap position ($400 bytes) \n";
    s << "  lda #$18 \n";
    s << "  sta $d018 \n";
    s << "loop \n";
    s << "  jmp loop \n";
    s << "  rts \n \n";
    s << "*=IMG_CHARSET \n";

    for (int y=0;y<25;y++)
        for (int x=0;x<40;x++)
        {
        s << m_data[y*40 + x].bitmapToAssembler();
    }
    s << "\n*=IMG_SCREENDATA \n";
    s << "  byte " << m_border << ", " << m_background << "\n";
    for (int y=0;y<25;y++)
    {
        QString str = "   byte ";
        for (int x=0;x<40;x++) {
           str = str +  m_data[x + y*40].colorMapToAssembler(1,2);
            if (x!=39)
                str = str + ", ";
        }
        str = str + "\n";
        s << str;
    }

    s << "\n*=IMG_COLORDATA \n";
    for (int y=0;y<25;y++)
    {
        QString str = "   byte ";
        for (int x=0;x<40;x++) {
           str = str +  m_data[x + y*40].colorToAssembler();
            if (x!=39)
                str = str + ", ";
        }
        str = str + "\n";
        s << str;
    }



    file.close();

}

void MultiColorImage::ExportRasBin(QString filenameBase, QString name)
{

    QString fData = filenameBase + "_data.bin";
    QString fColor = filenameBase + "_color.bin";

    if (QFile::exists(fData))
        QFile::remove(fData);

    if (QFile::exists(fColor))
        QFile::remove(fColor);

    QByteArray data;
    for (int i=0;i<40*25;i++) {
        data.append(m_data[i].data());
    }
    QFile file(fData);
    file.open(QIODevice::WriteOnly);
    file.write( data );
    file.close();

    QByteArray colorData;
    colorData.append(m_background);
    colorData.append(m_border);
    data.clear();
    for (int i=0;i<40*25;i++) {
        colorData.append((uchar)m_data[i].colorMapToNumber(1,2));
      //  qDebug () << QString::number((uchar)colorData[colorData.count()-1]);
    }
    for (int i=0;i<40*25;i++) {
        uchar c = (uchar)m_data[i].c[3];
        if (c==255)
            c=0;
        if (c!=0)
            qDebug() << c;
        data.append((char)c);
    }
    QFile file2(fColor);
    file2.open(QIODevice::WriteOnly);
    file2.write( colorData );
    file2.write( data );
    qDebug() << "Length: " << colorData.count();
    file2.close();


    // Take care of color data!




}

void MultiColorImage::Clear()
{
    for (int i=0;i<40*25;i++) {
        m_data[i].Clear(m_background);
    }
}

int MultiColorImage::LookUp(PixelChar pc)
{
    for (int i=0;i<m_organized.count();i++) {
        if (pc.isEqualBytes(m_organized[i]))
            return i;
    }
    // Not found, add
    m_organized.append(pc);
    return m_organized.count()-1;

}

void MultiColorImage::CalculateCharIndices()
{
    m_organized.clear();
    m_outputData.clear();
    int add=0;
    for (int x=0;x<40*25;x++) {
        PixelChar pc= m_data[x];
        if (pc.isEmpty()) {
            add++;
            continue;
        }
        x+=Eat(x, add);
        add=0;
    }
    m_outputData.append((char)0);
    m_outputData.append((char)0);
}

int MultiColorImage::Eat(int start, int add)
{
    int length=0;
    int cur = start;
    QVector<uchar> arr;



    while(!m_data[cur].isEmpty()) {
        arr.append(LookUp(m_data[cur]));

        qDebug() << "Colors : " << m_data[cur].c[0] << " and " <<  m_data[cur].c[1];

        arr.append(m_data[cur].c[1]);
        cur++;
        length++;
    }
    qDebug() << "Start: " << add << " , " << length;
    m_outputData.append(add);
    m_outputData.append(length);
    for (char i: arr)
        m_outputData.append(i);

    return length;
}

void MultiColorImage::SaveCharRascal(QString fileName, QString name)
{
    if (QFile::exists(fileName))
        QFile::remove(fileName);

    QFile file(fileName);
    file.open(QIODevice::Text | QIODevice::WriteOnly);
    QTextStream s(&file);

    s<< " /* Auto generated image file */\n\n";
    s<< " /* Charset */\n";
    s<< " char_"+name+"_set : array[" + QString::number(m_organized.count()*8) +"] of byte = (\n";
    bool isEnd = false;
    for (int i=0;i<m_organized.count();i++) {
        if (i==m_organized.count()-1)
            isEnd=true;
        for (int j=0;j<8;j++) {
            s<<QString::number(PixelChar::reverse(m_organized[i].p[j]));
            if (!(isEnd && j==7))
                s<<",";
        }
        if (!isEnd)
            s<<"\n";

    }
    s<<");\n";

    s<< " /* Char data */ \n";
    s<< " char_"+name+"_data : array[" + QString::number(m_outputData.count()) +"] of byte = (\n";
    isEnd = false;
    for (int i=0;i<m_outputData.count();i++) {
           if (i==m_outputData.count()-1)
               isEnd=true;

            s<< QString::number((uchar)m_outputData[i]);
            if (!isEnd)
                   s<<",";
            if (i%8==0)
               s<<"\n";
       }
       s<<");\n";


     // Then generate copying routine
       int incPos = 0;
       int shift = 8*64;
       int size = m_organized.count()*8;
       int cur = 0;
       s<< "procedure CopyChar"+name+"Data(); \n";
       s<< "begin \n";

       while (cur<size) {
           int count = size-cur;
           if (count>=255)
               count=0;
           s<< "\tmemcpy(char_"+name+"_set, #"+QString::number(incPos)+
               ", $2800+ "+QString::number(shift) +", #"+QString::number(count)+");"
            << "\n";
            incPos+=256;
            shift+=256;
            cur+=256;
       }


       s<< "end;\n";
       file.close();
}


PixelChar &MultiColorImage::getPixelChar(int x, int y)
{
    int dx = x/(8/m_scale);
    int dy = y/8;
    return m_data[dx + 40*dy];

}

PixelChar::PixelChar()
{
    Clear(0);
}

unsigned char PixelChar::get(int x, int y, unsigned char bitMask)
{
    if (x<0 || x>=8 || y<0 || y>=8)
        return 0;

    unsigned char pp = (p[y]>>x) & bitMask;

    return c[pp];

}

void PixelChar::set(int x, int y, unsigned char color, unsigned char bitMask, unsigned char maxCol, unsigned char minCol)
{
    if (x<0 || x>=8 || y<0 || y>=8) {
        qDebug() << "Trying to set " << x << ", " << y << " out of bounds";
        return;
    }


     unsigned char winner = 254;
    // Does color exist in map?
    for (int i=0;i<maxCol;i++) {
        if (c[i] == color) {
            winner = i;
            break;
        }
    }

    if (winner==254) {// && color!=c[0]) {

        for (int j=minCol;j<maxCol;j++)
            if (c[j]==255) {
                winner = j;
                break;
            }

        // not available slots found
        if (winner==254)
        {
            //winner = 3;
            winner = (p[y]>>x) & bitMask;
            if (winner==0 && minCol!=0)
                winner = maxCol-1;

        }

        if (winner>=minCol)
            c[winner] = color;

    }

    // Clear
    unsigned int f = ~(bitMask << x);
    p[y] &= f;
    // Add
    p[y] |= winner<<x;

}

void PixelChar::set(int x, int y, unsigned char color, unsigned char bitMask)
{
    if (x<0 || x>=8 || y<0 || y>=8) {
        qDebug() << "Trying to set " << x << ", " << y << " out of bounds";
        return;
    }

    // find index
    uchar index = 10;
    if (c[0]==color) index=0;
    if (c[1]==color) index=1;
    if (c[2]==color) index=2;
    if (index==10) {
        index=3;
        c[index] = color;

    }



    // Clear
    unsigned int f = ~(bitMask << x);
    p[y] &= f;
    // Add
    p[y] |= index<<x;

}

void PixelChar::Clear(unsigned char bg)
{
    for (int i=0;i<8;i++)
        p[i] = 0;
    for (int i=1;i<4;i++)
        c[i] = 255;
    c[0] = bg;

}

QString PixelChar::bitmapToAssembler()
{
    QString s = "   byte ";
    for (int i=0;i<8;i++) {
        s = s + QString::number(reverse(p[i]));
        if (i!=7)
            s = s+", ";
    }
    s=s+"\n";
    return s;
}

QString PixelChar::colorMapToAssembler(int i, int j)
{
    if (c[i]==255) c[i] = 0;
    if (c[j]==255) c[j] = 0;
    return QString(QString::number(c[i] | c[j]<<4));
}

uchar PixelChar::colorMapToNumber(int i, int j)
{
    if (c[i]==255) c[i] = 0;
    if (c[j]==255) c[j] = 0;
    return (c[i] | c[j]<<4);
}

QByteArray PixelChar::data()
{
    QByteArray qb;
    for (int i=0;i<8;i++)
        qb.append(reverse(p[i]));

    return qb;
}



QString PixelChar::colorToAssembler()
{
    if (c[3]==255) c[3] = 0;
    return QString(QString::number(c[3]));

}

QImage PixelChar::toQImage(int size, uchar bmask, LColorList& lst)
{
    QImage img= QImage(size,size,QImage::Format_RGB32);
    for (int i=0;i<size;i++)
        for (int j=0;j<size;j++) {
            int x = i/(float)(size)*8;
            int ix = (x % (8)/2)*2;
            int y = j/(float)(size)*8;
            uchar c = get(ix,y, bmask);

            img.setPixel(i,j,lst.m_list[c].color.rgba());
        }

    return img;
}

bool PixelChar::isEmpty()
{
    for (int i=0;i<8;i++)
        if (p[i]!=0)
            return false;

    return true;
}

bool PixelChar::isEqualBytes(PixelChar &o)
{
    for (int i=0;i<8;i++)
        if (p[i]!=o.p[i])
            return false;

    return true;
}

void PixelChar::Reorganize(unsigned char bitMask, unsigned char scale, unsigned char minCol, unsigned char maxCol)
{
    for (int i=minCol;i<maxCol;i++) {
        unsigned int cnt = Count(i, bitMask, scale);
        if ((cnt == 0)) {
            c[i] = 255;
           // qDebug() << "REMOVING COLOR";
        }
    }
}

int PixelChar::Count(unsigned int col, unsigned char bitMask, unsigned char scale)
{
    int cnt=0;
    for (int i=0;i<8/scale;i++)
        for (int j=0;j<8;j++)
            if ( ((p[j]>>scale*i) & bitMask)==col)
                cnt++;
    return cnt;
}

void MultiColorImage::ToQImage(LColorList& lst, QImage* img, float zoom, QPointF center)
{
//    return;
#pragma omp parallel for
    for (int i=0;i<m_width;i++)
        for (int j=0;j<m_height;j++) {

            float xp = ((i-center.x())*zoom)+ center.x();
            float yp = ((j-center.y())*zoom) + center.y();

            unsigned int col = getPixel(xp,yp);
            // Has transparency?
            QColor c=QColor(0,0,0);
            if (col>=1000) {
                col-=1000;
                c = QColor(255, 128, 128);
            }
            QColor scol = lst.m_list[col].color;
            if (c.red()>0 && renderPathGrid) {
                if ((int)(xp) %4==0 || (int)(yp+1)%4==0)
                    scol = c;
            }
            QRgb rgbCol = (scol).rgb();
            //for (int k=0;k<m_scale;k++)
 //               img->setPixel(m_scale*i + k,j,rgbCol);
                img->setPixel(i,j,rgbCol);
        }
    //return img;
}
