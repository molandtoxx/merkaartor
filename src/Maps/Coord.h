#ifndef MERKATOR_COORD_H_
#define MERKATOR_COORD_H_

#include <math.h>
#ifndef M_PI
#define M_PI        3.14159265358979323846
#endif
#include <QRectF>
#include <QtDebug>
#include <QtXml>

#define COORD_MAX 180.0
#define COORD_ENLARGE 0.00015

#define COORD2STRING(c) QString::number(c, 'f', 7)
inline QString Coord2Sexa(double c)
{
    int deg = int(c);
    double min = fabs((c - deg)*60);
    double sec = (min - int(min)) *60;

    return QString("%1� %2' %3\"").arg(deg).arg(int(min)).arg(sec, 0, 'f', 2);
}


inline double angToRad(double a)
{
    return a*M_PI/180.;
}

inline double radToAng(double a)
{
    return a*180/M_PI;
}

class Coord : public QPointF
{
    public:
        Coord()
            : QPointF() {}
        Coord(const Coord& c)
            : QPointF(c.x(), c.y()) {}
        Coord(const QPoint& P)
            : QPointF(P.x(), P.y()) {}
        Coord(const QPointF& P)
            : QPointF(P.x(), P.y()) {}
        Coord(double aLon, double aLat)
            : QPointF(aLon, aLat) {}

        double length() const
        {
            return sqrt((y()*y()+x()*x()));
        }

        double distanceFrom(const Coord& other) const;

        bool toXML(QString elName, QXmlStreamWriter& stream) const;
        bool toXML(QString elName, QDomElement& xParent) const;
        static Coord fromXML(QDomElement e);
        static Coord fromXML(QXmlStreamReader& stream);
};

uint qHash(const Coord &c);


#ifndef _MOBILE
#if QT_VERSION < 0x040700
#include <ggl/ggl.hpp>
#include <ggl/geometries/register/point.hpp>

GEOMETRY_REGISTER_POINT_2D_GET_SET(Coord, double, cs::cartesian, x, y, setX, setY)

#endif
#endif

inline Coord operator-(const Coord& A, const Coord& B)
{
    return Coord(A.x()-B.x(), A.y()-B.y());
}

inline Coord operator-(const Coord& A, const double B)
{
    return Coord(A.x()-B, A.y()-B);
}

inline Coord operator+(const Coord& A, const Coord& B)
{
    return Coord(A.x()+B.x(), A.y()+B.y());
}

inline Coord operator+(const Coord& A, const double B)
{
    return Coord(A.x()+B, A.y()+B);
}

inline Coord operator*(const Coord& A, double d)
{
    return Coord(A.x()*d, A.y()*d);
}

inline Coord operator/(const Coord& A, double d)
{
    if(d==0)
    {
        qDebug()<<"Error: divide by 0"<<endl;
        return A;
    }
    return Coord(A.x()/d, A.y()/d);
}

inline bool operator==(const Coord& A,const Coord& B)
{
    return A.y()==B.y() && A.x()==B.x();
}

double angle(Coord p1);
void rotate(Coord & p1,double angle);

class CoordBox : public QRectF
{
    public:
        CoordBox()
            : QRectF() {}
        CoordBox(const CoordBox& cb)
            : QRectF(cb) {}
        CoordBox(const QRectF& r)
            : QRectF(r) {}
        CoordBox(const Coord& C1, const Coord& C2);

        bool isNull() const
        {
            return (bottomLeft().isNull() && topRight().isNull());
        }
        bool isEmpty() const
        {
            return (lonDiff() == 0 || latDiff() == 0);
        }

        void merge(const Coord& C)
        {
            if (C.y() < bottom())
                setBottom(C.y());
            if (C.x() < left())
                setLeft(C.x());
            if (C.y() > top())
                setTop(C.y());
            if (C.x() > right())
                setRight(C.x());
        }

        void merge(const CoordBox& B)
        {
            merge(B.bottomLeft());
            merge(B.topRight());
        }

        Coord center() const
        {
            return Coord(QRectF::center());
        }

        double lonDiff() const
        {
            return width();
        }
        double latDiff() const
        {
            return -height();
        }
        CoordBox zoomed(double f) const;

        bool contains(const Coord& C) const
        {
            return (bottomLeft().y() <= C.y()) && (bottomLeft().x() <= C.x()) &&
                (C.y() < topRight().y()) && (C.x() <= topRight().x());
        }

        bool contains(const CoordBox& B) const
        {
            return contains(B.bottomLeft()) && contains(B.topRight());
        }

        bool intersects(const CoordBox& B) const
        {
            if ((B.latDiff() == 0) && (B.lonDiff() == 0)) {
                return contains(B.bottomLeft());
            }
            return QRectF::intersects(B);
        }

        bool disjunctFrom(const CoordBox& B) const
        {
            return !intersects(B);
        }

        void resize(double f);

        static bool visibleLine(const CoordBox & viewport, Coord & last, Coord & here);

        bool toXML(QString elName, QXmlStreamWriter& stream) const;
        bool toXML(QString elName, QDomElement& xParent) const;
        static CoordBox fromXML(QDomElement e);
        static CoordBox fromXML(QXmlStreamReader& stream);
};

Q_DECLARE_METATYPE( CoordBox );

#ifndef _MOBILE
#if QT_VERSION < 0x040700
#include <ggl/geometries/register/box.hpp>

GEOMETRY_REGISTER_BOX(CoordBox, Coord, bottomLeft, topRight)
#endif
#endif

#endif


