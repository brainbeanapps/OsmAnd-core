#include "MapMarker.h"
#include "MapMarker_P.h"

OsmAnd::MapMarker::MapMarker(
    const int markerId_,
    const int baseOrder_,
    const std::shared_ptr<const SkBitmap>& pinIcon_,
    const PinIconVerticalAlignment pinIconVerticalAlignment_,
    const PinIconHorisontalAlignment pinIconHorisontalAlignment_,
    const QHash< OnSurfaceIconKey, std::shared_ptr<const SkBitmap> >& onMapSurfaceIcons_,
    const bool isAccuracyCircleSupported_,
    const FColorRGB accuracyCircleBaseColor_)
    : _p(new MapMarker_P(this))
    , markerId(markerId_)
    , baseOrder(baseOrder_)
    , pinIcon(pinIcon_)
    , pinIconVerticalAlignment(pinIconVerticalAlignment_)
    , pinIconHorisontalAlignment(pinIconHorisontalAlignment_)
    , onMapSurfaceIcons(onMapSurfaceIcons_)
    , isAccuracyCircleSupported(isAccuracyCircleSupported_)
    , accuracyCircleBaseColor(accuracyCircleBaseColor_)
{
}

OsmAnd::MapMarker::~MapMarker()
{
}

bool OsmAnd::MapMarker::isHidden() const
{
    return _p->isHidden();
}

void OsmAnd::MapMarker::setIsHidden(const bool hidden)
{
    _p->setIsHidden(hidden);
}

bool OsmAnd::MapMarker::isAccuracyCircleVisible() const
{
    return _p->isAccuracyCircleVisible();
}

void OsmAnd::MapMarker::setIsAccuracyCircleVisible(const bool visible)
{
    _p->setIsAccuracyCircleVisible(visible);
}

double OsmAnd::MapMarker::getAccuracyCircleRadius() const
{
    return _p->getAccuracyCircleRadius();
}

void OsmAnd::MapMarker::setAccuracyCircleRadius(const double radius)
{
    _p->setAccuracyCircleRadius(radius);
}

OsmAnd::PointI OsmAnd::MapMarker::getPosition() const
{
    return _p->getPosition();
}

void OsmAnd::MapMarker::setPosition(const PointI position)
{
    _p->setPosition(position);
}

float OsmAnd::MapMarker::getOnMapSurfaceIconDirection(const OnSurfaceIconKey key) const
{
    return _p->getOnMapSurfaceIconDirection(key);
}

void OsmAnd::MapMarker::setOnMapSurfaceIconDirection(const OnSurfaceIconKey key, const float direction)
{
    _p->setOnMapSurfaceIconDirection(key, direction);
}

OsmAnd::ColorARGB OsmAnd::MapMarker::getPinIconModulationColor() const
{
    return _p->getPinIconModulationColor();
}

void OsmAnd::MapMarker::setPinIconModulationColor(const ColorARGB colorValue)
{
    _p->setPinIconModulationColor(colorValue);
}

bool OsmAnd::MapMarker::hasUnappliedChanges() const
{
    return _p->hasUnappliedChanges();
}

bool OsmAnd::MapMarker::applyChanges()
{
    return _p->applyChanges();
}

std::shared_ptr<OsmAnd::MapMarker::SymbolsGroup> OsmAnd::MapMarker::createSymbolsGroup() const
{
    return _p->createSymbolsGroup();
}

OsmAnd::MapMarker::SymbolsGroup::SymbolsGroup(const std::shared_ptr<MapMarker_P>& mapMarkerP_)
    : _mapMarkerP(mapMarkerP_)
{
}

OsmAnd::MapMarker::SymbolsGroup::~SymbolsGroup()
{
    if (const auto mapMarkerP = _mapMarkerP.lock())
        mapMarkerP->unregisterSymbolsGroup(this);
}

const OsmAnd::MapMarker* OsmAnd::MapMarker::SymbolsGroup::getMapMarker() const
{
    if (const auto mapMarkerP = _mapMarkerP.lock())
        return mapMarkerP->owner;
    return nullptr;
}

bool OsmAnd::MapMarker::SymbolsGroup::updatesPresent()
{
    if (const auto mapMarkerP = _mapMarkerP.lock())
        return mapMarkerP->hasUnappliedChanges();

    return false;
}

bool OsmAnd::MapMarker::SymbolsGroup::update()
{
    if (const auto mapMarkerP = _mapMarkerP.lock())
        return mapMarkerP->applyChanges();

    return false;
}
