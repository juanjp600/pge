/**

Blitz3D is released under the zlib/libpng license.

Altered to utilize pge's math classes

The zlib/libpng License

Copyright (c) 2013 Blitz Research Ltd

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source distribution.

**/

#ifndef COLLISION_H
#define COLLISION_H

#include <Math/AABBox.h>
#include <Math/Line.h>
#include <Math/Matrix.h>
#include <Math/Rectangle.h>
#include <Math/Vector.h>

extern const float COLLISION_EPSILON;

struct Collision {
    bool hit;
    PGE::Line3f line;
    PGE::Vector3f normal;
    float coveredAmount;

	Collision(){}

	static Collision edgeTest(const PGE::Vector3f& v0,const PGE::Vector3f& v1,const PGE::Vector3f& pn,const PGE::Vector3f& en,const PGE::Line3f& line,float radius);

	static Collision update(const PGE::Line3f& line,float t,const PGE::Vector3f& n);

	static Collision sphereCollide(const PGE::Line3f& src_line,float src_radius,const PGE::Vector3f& otherPos,float other_radius);
	//void sphereCollide(const PGE::Line3f& line,float radius,const PGE::Vector3f& otherPos,const PGE::Vector3f& radii);

	static Collision triangleCollide(const PGE::Line3f& src_line,float src_radius,const PGE::Vector3f& v0,const PGE::Vector3f& v1,const PGE::Vector3f& v2);

	//void boxCollide(const PGE::Line3f& src_line,float src_radius,const PGE::AABBox& box);
};

#endif
