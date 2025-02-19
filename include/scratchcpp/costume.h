// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "iimageformat.h"
#include "spimpl.h"

#include "asset.h"

namespace libscratchcpp
{

class Broadcast;
class IGraphicsEffect;
class CostumePrivate;

/*! \brief The Costume class represents a Scratch costume. */
class LIBSCRATCHCPP_EXPORT Costume : public Asset
{
    public:
        Costume(const std::string &name, const std::string &id, const std::string &format);
        Costume(const Costume &) = delete;

        double bitmapResolution() const;
        void setBitmapResolution(double newBitmapResolution);

        int rotationCenterX() const;
        void setRotationCenterX(int newRotationCenterX);

        int rotationCenterY() const;
        void setRotationCenterY(int newRotationCenterY);

        unsigned int width() const;
        unsigned int height() const;

        double scale() const;
        void setScale(double scale);

        bool mirrorHorizontally() const;
        void setMirrorHorizontally(bool mirror);

        Rgb **bitmap() const;

        double graphicsEffectValue(IGraphicsEffect *effect) const;
        void setGraphicsEffectValue(IGraphicsEffect *effect, double value);

        void clearGraphicsEffects();

        Broadcast *broadcast();

    protected:
        void processData(unsigned int size, void *data) override;

    private:
        spimpl::unique_impl_ptr<CostumePrivate> impl;
};

} // namespace libscratchcpp
