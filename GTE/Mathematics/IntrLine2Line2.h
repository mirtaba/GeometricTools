// David Eberly, Geometric Tools, Redmond WA 98052
// Copyright (c) 1998-2022
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt
// https://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
// Version: 6.0.2022.01.06

#pragma once

#include <Mathematics/Vector2.h>
#include <Mathematics/Line.h>
#include <Mathematics/FIQuery.h>
#include <Mathematics/TIQuery.h>
#include <limits>

namespace gte
{
    template <typename T>
    class TIQuery<T, Line2<T>, Line2<T>>
    {
    public:
        struct Result
        {
            Result()
                :
                intersect(false),
                numIntersections(0)
            {
            }

            // If the lines do not intersect,
            //   intersect = false
            //   numIntersections = 0
            //
            // If the lines intersect in a single point,
            //   intersect = true
            //   numIntersections = 1
            //
            // If the lines are the same,
            //   intersect = true
            //   numIntersections = std::numeric_limits<int32_t>::max()
            bool intersect;
            int32_t numIntersections;
        };

        Result operator()(Line2<T> const& line0, Line2<T> const& line1)
        {
            Result result{};

            // The intersection of two lines is a solution to P0 + s0 * D0 =
            // P1 + s1 * D1. Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q. If
            // DotPerp(D0, D1)) = 0, the lines are parallel. Additionally, if
            // DotPerp(Q, D1)) = 0, the lines are the same. If
            // DotPerp(D0, D1)) is not zero, then the lines intersect in a
            // single point where
            //   s0 = DotPerp(Q, D1))/DotPerp(D0, D1))
            //   s1 = DotPerp(Q, D0))/DotPerp(D0, D1))
            Vector2<T> diff = line1.origin - line0.origin;
            T D0DotPerpD1 = DotPerp(line0.direction, line1.direction);
            if (D0DotPerpD1 != (T)0)
            {
                // The lines are not parallel.
                result.intersect = true;
                result.numIntersections = 1;
            }
            else
            {
                // The lines are parallel.
                Normalize(diff);
                T diffNDotPerpD1 = DotPerp(diff, line1.direction);
                if (diffNDotPerpD1 != (T)0)
                {
                    // The lines are parallel but distinct.
                    result.intersect = false;
                    result.numIntersections = 0;
                }
                else
                {
                    // The lines are the same.
                    result.intersect = true;
                    result.numIntersections = std::numeric_limits<int32_t>::max();
                }
            }

            return result;
        }
    };

    template <typename T>
    class FIQuery<T, Line2<T>, Line2<T>>
    {
    public:
        struct Result
        {
            Result()
                :
                intersect(false),
                numIntersections(0),
                line0Parameter{ (T)0, (T)0 },
                line1Parameter{ (T)0, (T)0 },
                point(Vector2<T>::Zero())
            {
            }

            // If the lines do not intersect,
            //   intersect = false
            //   numIntersections = 0
            //   line0Parameter[] = { 0, 0 }  // invalid
            //   line1Parameter[] = { 0, 0 }  // invalid
            //   point = { 0, 0 }  // invalid
            //
            // If the lines intersect in a single point, the parameter for
            // line0 is s0 and the parameter for line1 is s1,
            //   intersect = true
            //   numIntersections = 1
            //   line0Parameter = { s0, s0 }
            //   line1Parameter = { s1, s1 }
            //   point = line0.origin + s0 * line0.direction
            //         = line1.origin + s1 * line1.direction
            //
            // If the lines are the same,
            // let maxT = std::numeric_limits<T>::max(),
            //   intersect = true
            //   numIntersections = std::numeric_limits<int32_t>::max()
            //   line0Parameter = { -maxT, +maxT }
            //   line1Parameter = { -maxT, +maxT }
            //   point = { 0, 0 }  // invalid
            bool intersect;
            int32_t numIntersections;
            std::array<T, 2> line0Parameter, line1Parameter;
            Vector2<T> point;
        };

        Result operator()(Line2<T> const& line0, Line2<T> const& line1)
        {
            Result result{};

            // The intersection of two lines is a solution to P0 + s0 * D0 =
            // P1 + s1 * D1. Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q. If
            // DotPerp(D0, D1)) = 0, the lines are parallel. Additionally, if
            // DotPerp(Q, D1)) = 0, the lines are the same. If
            // DotPerp(D0, D1)) is not zero, then the lines intersect in a
            // single point where
            //   s0 = DotPerp(Q, D1))/DotPerp(D0, D1))
            //   s1 = DotPerp(Q, D0))/DotPerp(D0, D1))

            Vector2<T> Q = line1.origin - line0.origin;
            T D0DotPerpD1 = DotPerp(line0.direction, line1.direction);
            if (D0DotPerpD1 != (T)0)
            {
                // The lines are not parallel.
                result.intersect = true;
                result.numIntersections = 1;
                T QDotPerpD0 = DotPerp(Q, line0.direction);
                T QDotPerpD1 = DotPerp(Q, line1.direction);
                T s0 = QDotPerpD1 / D0DotPerpD1;
                T s1 = QDotPerpD0 / D0DotPerpD1;
                result.line0Parameter = { s0, s0 };
                result.line1Parameter = { s1, s1 };
                result.point = line0.origin + s0 * line0.direction;
            }
            else
            {
                // The lines are parallel.
                T QDotPerpD1 = DotPerp(Q, line1.direction);
                if (std::fabs(QDotPerpD1) != (T)0)
                {
                    // The lines are parallel but distinct.
                    result.intersect = false;
                    result.numIntersections = 0;
                }
                else
                {
                    // The lines are the same.
                    result.intersect = true;
                    result.numIntersections = std::numeric_limits<int32_t>::max();
                    T const maxT = std::numeric_limits<T>::max();
                    result.line0Parameter = { -maxT, maxT };
                    result.line1Parameter = { -maxT, maxT };
                }
            }

            return result;
        }
    };
}
