#ifndef LIMAGEFACTORY_H
#define LIMAGEFACTORY_H

#include "source/limage/limageqimage.h"
#include "source/limage/multicolorimage.h"
#include "source/limage/standardcolorimage.h"
#include "source/limage/limagetiff.h"
#include "source/limage/charsetimage.h"
#include "source/limage/c64fullscreenchar.h"
#include "source/limage/imageleveleditor.h"
#include "source/limage/limagecharsetregular.h"
#include "source/limage/limagesprites.h"

class LImageFactory {
public:

    static LImage* Create(LImage::Type t, LColorList::Type colorType) {
        if (t == LImage::Type::QImageBitmap)
            return new LImageQImage(colorType);
        if (t == LImage::Type::MultiColorBitmap)
            return new MultiColorImage(colorType);
        if (t == LImage::Type::HiresBitmap)
            return new StandardColorImage(colorType);
        if (t == LImage::Type::Tiff)
            return new LImageTiff(colorType);
        if (t == LImage::Type::CharMapMulticolor)
            return new CharsetImage(colorType);
        if (t == LImage::Type::CharmapRegular)
            return new LImageCharsetRegular(colorType);
        if (t == LImage::Type::FullScreenChar)
            return new C64FullScreenChar(colorType);
        if (t == LImage::Type::LevelEditor)
            return new ImageLevelEditor(colorType);
        if (t == LImage::Type::Sprites)
            return new LImageSprites(colorType);

        qDebug() << "ERROR: LImageFactory could not find type " << t;
        qDebug() << "Charmapfactory: " << LImage::Type::CharmapRegular;
        return nullptr;
    }



};




#endif // LIMAGEFACTORY_H
