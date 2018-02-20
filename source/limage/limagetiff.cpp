#include "limagetiff.h"
#include <QImage>

LImageTiff::LImageTiff()
{
    m_type = LImage::Tiff;
    m_width = 0;
    m_height = 0;
}

LImageTiff::LImageTiff(LColorList::Type t) : LImageQImage(t)
{
    m_type = LImage::Tiff;
}

void LImageTiff::Initialize(int width, int height)
{
    Release();
    m_width = width;
    m_height = height;
    m_qImage = new QImage(width, height, QImage::Format_ARGB32);
}

void LImageTiff::LoadTiff(QString filename)
{
#ifdef USE_LIBTIFF

    if (!m_tif.Open(filename)) {
        //LMessage::lMessage.Error("Could not open tiff file + '"+i + "' ! Please check the filename and try again.");
        qDebug() << "Could not open tiff " << filename;
        return;
    }
    m_tif.SetupBuffers();
    for (int i=0;i<m_width;i++) {
        for (int j=0;j<m_height;j++) {
            float x  = i/(float)m_width*m_tif.m_width;
            float y  = j/(float)m_height*m_tif.m_height;
            QColor col = m_tif.GetTiledRGB(x,y,0);
            QColor rCol = QColor(col.blue(), col.green(), col.red());
            //col = QColor(255,0,255);
            m_qImage->setPixel(i,j,rCol.rgba());
            if (j%20 == 0)
                m_tif.bufferStack.UpdateBuffer();

        }
        }
#else
    qDebug() << "LibTIFF not compiled in this version";
#endif

}

void LImageTiff::ToQImage(LColorList &lst, QImage *img, float zoom, QPoint center)
{
#pragma omp parallel for
    for (int i=0;i<m_width;i++)
        for (int j=0;j<m_height;j++) {

            float xp = ((i-center.x())*zoom)+ center.x();
            float yp = ((j-center.y())*zoom) + center.y();

            QRgb c;
            if (xp>=0 && xp<m_width && yp>=0 && yp<m_height)
                c = m_qImage->pixel(xp,yp);

            //            img->setPixel(i,j,QRgb(col));
            img->setPixel(i,j,c);
        }
    //return img;

}
