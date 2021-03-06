#include "OnlineTileSources_P.h"
#include "OnlineTileSources.h"

#include "ICoreResourcesProvider.h"
#include "Logging.h"

OsmAnd::OnlineTileSources_P::OnlineTileSources_P(OnlineTileSources* owner_)
    : owner(owner_)
{
}

OsmAnd::OnlineTileSources_P::~OnlineTileSources_P()
{
}

bool OsmAnd::OnlineTileSources_P::deserializeFrom(QXmlStreamReader& xmlReader)
{
    QHash< QString, std::shared_ptr<const Source> > collection;

    while(!xmlReader.atEnd() && !xmlReader.hasError())
    {
        xmlReader.readNext();
        if (!xmlReader.isStartElement())
            continue;
        const auto tagName = xmlReader.name();
        if (tagName == QLatin1String("tile_source"))
        {
            const auto name = xmlReader.attributes().value(QLatin1String("name")).toString();
            if (collection.contains(name))
            {
                LogPrintf(LogSeverityLevel::Warning,
                    "Ignored duplicate online tile source with name '%s'",
                    qPrintable(name));
                continue;
            }

            // Original format of the tile sources, used in the Android application
            if (xmlReader.attributes().hasAttribute("rule"))
            {
                LogPrintf(LogSeverityLevel::Warning,
                    "'%s' online tile source is not supported since it uses special rule, possibly quadkey",
                    qPrintable(name));
                continue;
            }

            const auto originalUrlTemplate = xmlReader.attributes().value(QLatin1String("url_template")).toString();
            const auto urlPattern = QString(originalUrlTemplate)
                .replace(QLatin1String("{0}"), QLatin1String("${osm_zoom}"))
                .replace(QLatin1String("{1}"), QLatin1String("${osm_x}"))
                .replace(QLatin1String("{2}"), QLatin1String("${osm_y}"));
            const auto minZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("min_zoom")).toUInt());
            const auto maxZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("max_zoom")).toUInt());
            const auto tileSize = xmlReader.attributes().value(QLatin1String("tile_size")).toUInt();
            
            std::shared_ptr<Source> newSource(new Source(name));
            newSource->urlPattern = urlPattern;
            newSource->minZoom = minZoom;
            newSource->maxZoom = maxZoom;
            newSource->maxConcurrentDownloads = 1;
            newSource->tileSize = tileSize;
            newSource->alphaChannelPresence = AlphaChannelPresence::Unknown;
            newSource->tileDensityFactor = 1.0f;
            collection.insert(name, newSource);
        }
        else if (tagName == QLatin1String("onlineTileSource"))
        {
            const auto name = xmlReader.attributes().value(QLatin1String("name")).toString();
            if (collection.contains(name))
            {
                LogPrintf(LogSeverityLevel::Warning,
                    "Ignored duplicate online tile source with name '%s'",
                    qPrintable(name));
                continue;
            }

            const auto title = xmlReader.attributes().value(QLatin1String("title")).toString();
            const auto urlPattern = xmlReader.attributes().value(QLatin1String("urlPattern")).toString();
            const auto minZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("minZoom")).toUInt());
            const auto maxZoom = static_cast<ZoomLevel>(xmlReader.attributes().value(QLatin1String("maxZoom")).toUInt());
            const auto maxConcurrentDownloads = xmlReader.attributes().value(QLatin1String("maxConcurrentDownloads")).toUInt();
            const auto tileSize = xmlReader.attributes().value(QLatin1String("tileSize")).toUInt();
            const auto alphaChannelPresenceValue =
                xmlReader.attributes().value(QLatin1String("alphaChannelPresence")).toString();
            auto alphaChannelPresence = AlphaChannelPresence::Unknown;
            if (QString::compare(alphaChannelPresenceValue, QLatin1String("notPresent"), Qt::CaseInsensitive) == 0)
                alphaChannelPresence = AlphaChannelPresence::NotPresent;
            else if (QString::compare(alphaChannelPresenceValue, QLatin1String("present"), Qt::CaseInsensitive) == 0)
                alphaChannelPresence = AlphaChannelPresence::Present;
            else if (QString::compare(alphaChannelPresenceValue, QLatin1String("unknown"), Qt::CaseInsensitive) != 0)
            {
                LogPrintf(LogSeverityLevel::Warning,
                    "Unknown alphaChannelPresence value '%s'",
                    qPrintable(alphaChannelPresenceValue));
            }
            const auto tileDensityFactor = xmlReader.attributes().value(QLatin1String("tileDensityFactor")).toFloat();

            std::shared_ptr<Source> newSource(new Source(name, title));
            newSource->urlPattern = urlPattern;
            newSource->minZoom = minZoom;
            newSource->maxZoom = maxZoom;
            newSource->maxConcurrentDownloads = maxConcurrentDownloads;
            newSource->tileSize = tileSize;
            newSource->alphaChannelPresence = alphaChannelPresence;
            newSource->tileDensityFactor = tileDensityFactor;
            collection.insert(name, newSource);
        }
    }
    if (xmlReader.hasError())
    {
        LogPrintf(
            LogSeverityLevel::Warning,
            "XML error: %s (%" PRIi64 ", %" PRIi64 ")",
            qPrintable(xmlReader.errorString()),
            xmlReader.lineNumber(),
            xmlReader.columnNumber());
        return false;
    }

    _collection = collection;

    return true;
}

bool OsmAnd::OnlineTileSources_P::serializeTo(QXmlStreamWriter& xmlWriter) const
{
    assert(false);
    return false;
}

bool OsmAnd::OnlineTileSources_P::loadFrom(const QByteArray& content)
{
    QXmlStreamReader xmlReader(content);
    return deserializeFrom(xmlReader);
}

bool OsmAnd::OnlineTileSources_P::loadFrom(QIODevice& ioDevice)
{
    QXmlStreamReader xmlReader(&ioDevice);
    return deserializeFrom(xmlReader);
}

bool OsmAnd::OnlineTileSources_P::saveTo(QIODevice& ioDevice) const
{
    QXmlStreamWriter xmlWriter(&ioDevice);
    return serializeTo(xmlWriter);
}

QHash< QString, std::shared_ptr<const OsmAnd::OnlineTileSources_P::Source> > OsmAnd::OnlineTileSources_P::getCollection() const
{
    return _collection;
}

std::shared_ptr<const OsmAnd::OnlineTileSources_P::Source> OsmAnd::OnlineTileSources_P::getSourceByName(const QString& sourceName) const
{
    const auto citSource = _collection.constFind(sourceName);
    if (citSource == _collection.cend())
        return nullptr;
    return *citSource;
}

bool OsmAnd::OnlineTileSources_P::addSource(const std::shared_ptr<Source>& source)
{
    if (_collection.constFind(source->name) != _collection.cend())
        return false;

    _collection.insert(source->name, source);

    return true;
}

bool OsmAnd::OnlineTileSources_P::removeSource(const QString& sourceName)
{
    return (_collection.remove(sourceName) > 0);
}

std::shared_ptr<OsmAnd::OnlineTileSources> OsmAnd::OnlineTileSources_P::_builtIn;
std::shared_ptr<const OsmAnd::OnlineTileSources> OsmAnd::OnlineTileSources_P::getBuiltIn()
{
    static QMutex mutex;
    QMutexLocker scopedLocker(&mutex);

    if (!_builtIn)
    {
        bool ok = true;
        _builtIn.reset(new OnlineTileSources());
        _builtIn->loadFrom(getCoreResourcesProvider()->getResource(
            QLatin1String("misc/default.online_tile_sources.xml"),
            &ok));
        assert(ok);
    }

    return _builtIn;
}
