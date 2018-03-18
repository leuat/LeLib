#ifndef LIMAGECHARSETREGULAR_H
#define LIMAGECHARSETREGULAR_H

#include "charsetimage.h"
#include <QPixmap>


class LImageCharsetRegular: public CharsetImage
{
public:
    LImageCharsetRegular(LColorList::Type t);

    //void setPixel(int x, int y, unsigned int color) override;
};

#endif // LIMAGECHARSETREGULAR_H
