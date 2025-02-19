// SPDX-License-Identifier: Apache-2.0

#include <scratchcpp/costume.h>
#include <scratchcpp/scratchconfiguration.h>

#include "costume_p.h"

using namespace libscratchcpp;

/*! Constructs Costume. */
Costume::Costume(const std::string &name, const std::string &id, const std::string &format) :
    Asset(name, id, format),
    impl(spimpl::make_unique_impl<CostumePrivate>())
{
    // NOTE: image is always initialized, so there's no need for null checks
    impl->image = ScratchConfiguration::createImageFormat(format);
    impl->updateImage();
}

/*! Returns the reciprocal of the costume scaling factor for bitmap costumes. */
double Costume::bitmapResolution() const
{
    return impl->bitmapResolution;
}

/*! Sets the reciprocal of the costume scaling factor for bitmap costumes. */
void Costume::setBitmapResolution(double newBitmapResolution)
{
    if (impl->bitmapResolution == newBitmapResolution)
        return;

    impl->bitmapResolution = newBitmapResolution;
    impl->updateImage();
}

/*! Returns the x-coordinate of the rotation center. */
int Costume::rotationCenterX() const
{
    return impl->rotationCenterX;
}

/*! Sets the x-coordinate of the rotation center. */
void Costume::setRotationCenterX(int newRotationCenterX)
{
    impl->rotationCenterX = newRotationCenterX;
}

/*! Returns the y-coordinate of the rotation center. */
int Costume::rotationCenterY() const
{
    return impl->rotationCenterY;
}

/*! Returns the y-coordinate of the rotation center. */
void Costume::setRotationCenterY(int newRotationCenterY)
{
    impl->rotationCenterY = newRotationCenterY;
}

/*! Returns the costume width. */
unsigned int Costume::width() const
{
    return impl->image->width() * impl->scale / impl->bitmapResolution;
}

/*! Returns the costume height. */
unsigned int Costume::height() const
{
    return impl->image->height() * impl->scale / impl->bitmapResolution;
}

/*! Returns the image scale. */
double Costume::scale() const
{
    return impl->scale;
}

/*! Sets the image scale (this is automatically set by the sprite). */
void Costume::setScale(double scale)
{
    if (impl->scale == scale)
        return;

    impl->scale = scale;
    impl->updateImage();
}

/*! Returns true if the costume image is mirrored horizontally. */
bool Costume::mirrorHorizontally() const
{
    return impl->mirrorHorizontally;
}

/*! Sets whether the costume image is mirrored horizontally (this is automatically set by the sprite). */
void Costume::setMirrorHorizontally(bool mirror)
{
    if (impl->mirrorHorizontally == mirror)
        return;

    impl->mirrorHorizontally = mirror;
    impl->updateImage();
}

/*! Returns the bitmap of the costume (an array of pixel rows). */
Rgb **Costume::bitmap() const
{
    return impl->bitmap;
}

/*! Returns the value of the given graphics effect. */
double Costume::graphicsEffectValue(IGraphicsEffect *effect) const
{
    auto it = impl->graphicsEffects.find(effect);

    if (it == impl->graphicsEffects.cend())
        return 0;
    else
        return it->second;
}

/*! Sets the value of the given graphics effect (this is automatically set by the sprite). */
void Costume::setGraphicsEffectValue(IGraphicsEffect *effect, double value)
{
    auto it = impl->graphicsEffects.find(effect);
    bool update = ((it == impl->graphicsEffects.cend()) || (it->second != value));

    if (value == 0)
        impl->graphicsEffects.erase(effect);
    else
        impl->graphicsEffects[effect] = value;

    if (update)
        impl->updateImage();
}

/*! Clears all graphics effects (this is automatically called by the sprite). */
void Costume::clearGraphicsEffects()
{
    bool update = !impl->graphicsEffects.empty();
    impl->graphicsEffects.clear();

    if (update)
        impl->updateImage();
}

/*!
 * Returns the Broadcast linked with this costume.
 * \note This is used by the "switch backdrop to and wait" block.
 */
Broadcast *Costume::broadcast()
{
    return &impl->broadcast;
}

void Costume::processData(unsigned int size, void *data)
{
    impl->image->setData(size, data);
    impl->freeImage();
    impl->updateImage();
}
