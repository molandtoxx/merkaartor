#include "ExportOSM.h"
#include "Utils.h"

#include "Features.h"

#include "MerkaartorPreferences.h"

static QString stripToOSMId(const IFeature::FId& id)
{
    return QString::number(id.numId);
}

static QString tagOSM(const Feature& F)
{
    QString S;
    for (int i=0; i<F.tagSize(); ++i)
    {
        if (F.tagKey(i).startsWith('_') && (F.tagKey(i).endsWith('_')))
            continue;

        const QString & tagKey = Utils::encodeAttributes( F.tagKey(i) );
        const QString & tagValue = Utils::encodeAttributes( F.tagValue(i) );

        S += QString("<tag k=\"%1\" v=\"%2\"/>").arg( tagKey ).arg( tagValue );
    }
    return S;
}

QString versionAttribute(const Feature& F)
{
#ifndef FRISIUS_BUILD
    return QString(" version=\"%1\"").arg(F.versionNumber());
#endif
}

QString exportOSM(const Node& Pt, const QString& ChangesetId)
{
    if (Pt.isVirtual())
        return QString();

    QString S;
    if (ChangesetId.isEmpty())
        S += QString("<node id=\"%1\" lat=\"%2\" lon=\"%3\"%4>")
            .arg(stripToOSMId(Pt.id())).arg(Pt.position().y(),0,'f',8).arg(Pt.position().x(),0,'f',8).arg(versionAttribute(Pt));
    else
        S += QString("<node id=\"%1\" lat=\"%2\" lon=\"%3\"%4 changeset=\"%5\">")
            .arg(stripToOSMId(Pt.id())).arg(Pt.position().y(),0,'f',8).arg(Pt.position().x(),0,'f',8).arg(versionAttribute(Pt))
            .arg(ChangesetId);
    S+=tagOSM(Pt);
    S+="</node>";
    return S;
}

QString exportOSM(const Way& R, const QString& ChangesetId)
{
    QString S;
    if (ChangesetId.isEmpty())
        S += QString("<way id=\"%1\"%2>").arg(stripToOSMId(R.id())).arg(versionAttribute(R));
    else
        S += QString("<way id=\"%1\"%2 changeset=\"%3\">").arg(stripToOSMId(R.id())).arg(versionAttribute(R)).arg(ChangesetId);
    if (R.size()) {
        S+=QString("<nd ref=\"%1\"/>").arg(stripToOSMId(R.get(0)->id()));
        for (int i=1; i<R.size(); ++i)
            if (!R.getNode(i)->isVirtual())
                if (R.get(i)->id().numId != R.get(i-1)->id().numId)
                    S+=QString("<nd ref=\"%1\"/>").arg(stripToOSMId(R.get(i)->id()));
    }
    S += tagOSM(R);
    S += "</way>";
    return S;
}

QString exportOSM(const Relation& R, const QString& ChangesetId)
{
    QString S;
    if (ChangesetId.isEmpty())
        S += QString("<relation id=\"%1\"%2>").arg(stripToOSMId(R.id())).arg(versionAttribute(R));
    else
        S += QString("<relation id=\"%1\"%2 changeset=\"%3\">").arg(stripToOSMId(R.id())).arg(versionAttribute(R)).arg(ChangesetId);
    for (int i=0; i<R.size(); ++i)
    {
        QString Type("node");
        if (dynamic_cast<const Way*>(R.get(i)))
            Type="way";
        else if (dynamic_cast<const Relation*>(R.get(i)))
            Type="relation";
        S+=QString("<member type=\"%1\" ref=\"%2\" role=\"%3\"/>").arg(Type).arg(stripToOSMId(R.get(i)->id())).arg(R.getRole(i));
    }
    S += tagOSM(R);
    S += "</relation>";
    return S;
}


QString wrapOSM(const QString& S, const QString& ChangeSetId)
{
    if (ChangeSetId.isEmpty())
        return "<osm version=\"0.5\">"+S+"</osm>"+QChar(0);
    else
        return "<osm>"+S+"</osm>"+QChar(0);
        //return "<osm version=\"0.6\" changeset=\""+ChangeSetId+"\">"+S+"</osm>"+QChar(0);
}


