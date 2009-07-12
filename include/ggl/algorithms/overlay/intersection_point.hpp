// Generic Geometry Library
//
// Copyright Barend Gehrels 1995-2009, Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef GGL_ALGORITHMS_INTERSECTION_POINT_HPP
#define GGL_ALGORITHMS_INTERSECTION_POINT_HPP

#include <vector>


#include <ggl/core/access.hpp>
#include <ggl/core/coordinate_dimension.hpp>

#include <ggl/strategies/distance_result.hpp>
#include <ggl/strategies/strategy_traits.hpp>

#include <ggl/algorithms/overlay/segment_identifier.hpp>


namespace ggl
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace intersection {


template<typename P>
struct intersection_info
{
    typedef typename distance_result<P, P>::type distance_type;

    inline intersection_info()
        : travels_to_vertex_index(-1)
        , travels_to_ip_index(-1)
        , next_ip_index(-1)
        , distance(ggl::make_distance_result<distance_type>(0))
        , direction(0)
        , how(0)
        , arrival(0)
    {}

    // Identifier of this segment (source,segment,ring,multi)
    segment_identifier seg_id;


    // vertex to which is free travel after this IP,
    // so from "segment_index+1" to "travels_to_vertex_index", without IP-s,
    // can be -1
    int travels_to_vertex_index;

    // same but now IP index, so "next IP index" but not on THIS segment
    int travels_to_ip_index;

    // index of next IP on this segment, -1 if there is no one
    int next_ip_index;

    distance_type distance; // distance-measurement from segment.first to IP

    // 1: left, -1: right, 0: collinear
    int direction;

    // Information about how intersection is done
    char how;
    int arrival;
};


template<typename P>
struct intersection_point
{
    public :
        inline intersection_point()
            : visit_code(0) // VISIT_NONE
            , shared_code(0)
        {
            // Init intersection point with zero
            ggl::assign_zero(point);
        }


#ifdef GGL_DEBUG_INTERSECTION
        static inline std::string dir(int d)
        {
            return d == 0 ? "-" : (d == 1 ? "L" : "R");
        }
        static inline std::string how(int h)
        {
            return h == 0 ? "-" : (h == 1 ? "A" : "D");
        }

        friend std::ostream& operator<<(std::ostream &os, intersection_point<P> const& p)
        {
            os << "IP (" << ggl::get<0>(p.point) << "," << ggl::get<1>(p.point) << ")"
                << " visited: " << int(p.visit_code)
                << " shared: " << p.shared_code
                << std::endl;

            for (unsigned int i = 0; i < p.info.size(); i++)
            {
                os  << "\t"
                    << " src: " << p.info[i].seg_id.source_index
                    << " how: " << p.info[i].how << "[" << how(p.info[i].arrival) << "]"
                    << " dir: " << dir(p.info[i].direction)
                    << " seg: " << p.info[i].seg_id.segment_index
                    << " - " << p.info[i].travels_to_vertex_index
                    << " next ip: " << p.info[i].travels_to_ip_index
                    << " , " << p.info[i].next_ip_index
                    << " dist: " << double(p.info[i].distance)
                    << std::endl;
            }
            return os;
        }
#endif
        typedef intersection_info<P> traversal_type;
        typedef std::vector<traversal_type> traversal_vector;

        P point;

        int visit_code;
        short int shared_code; // 0 for nothing, 1 for is-shared, 2 for to-be-deleted

        // info about the two intersecting segments
        // usually two, but can be more if IP's are merged
        traversal_vector info;


        inline void clone_except_info(intersection_point& other) const
        {
            other.point = point;
            other.shared_code = shared_code;
            other.visit_code = visit_code;
        }
};




}} // namespace detail::intersection
#endif //DOXYGEN_NO_DETAIL


// Register the intersection point as being a point fulfilling the ggl Point Concept
namespace traits
{

    template <typename P>
    struct coordinate_type<ggl::detail::intersection::intersection_point<P> >
    {
        typedef typename ggl::coordinate_type<P>::type type;
    };

    template <typename P>
    struct coordinate_system<ggl::detail::intersection::intersection_point<P> >
    {
        typedef typename ggl::coordinate_system<P>::type type;
    };

    template <typename P>
    struct dimension<ggl::detail::intersection::intersection_point<P> >
        : ggl::dimension<P>
    {};

    template <typename P>
    struct tag<ggl::detail::intersection::intersection_point<P> >
    {
        typedef point_tag type;
    };

    template <typename P>
    struct access<ggl::detail::intersection::intersection_point<P> >
    {
        template <int Index>
        static inline typename coordinate_type<P>::type get(
                ggl::detail::intersection::intersection_point<P> const& p)
        {
            return ggl::get<Index>(p.point);
        }

        template <int Index>
        static inline void set(ggl::detail::intersection::intersection_point<P>& p,
                typename coordinate_type<P>::type const& value)
        {
            ggl::set<Index>(p.point, value);
        }
    };

}


} // namespace ggl

#endif // GGL_ALGORITHMS_INTERSECTION_POINT_HPP