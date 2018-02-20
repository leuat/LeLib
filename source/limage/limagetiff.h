
#ifndef LIMAGETIFF_H
#define LIMAGETIFF_H

#include "source/limage/limageqimage.h"
#include "source/ltiff/ltiff.h"

class LImageTiff : public LImageQImage {
public:
    LImageTiff();
    LImageTiff(LColorList::Type t);

#ifdef USE_LIBTIFF
    LTiff m_tif;
#endif
    void Initialize(int width, int height) override;

//    void setPixel(int x, int y, unsigned int color) override;
  //  unsigned int getPixel(int x, int y) override;

    void LoadTiff(QString filename);

    void ToQImage(LColorList& lst, QImage* img, float zoom, QPoint center) override;


};
#endif // LIMAGETIFF_H
