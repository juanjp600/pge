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

#include "Collision.h"
#include <Math/Plane.h>

#include <math.h>

#include <SDL.h>

using namespace PGE;

const float COLLISION_EPSILON = 0.001f;

Collision Collision::update(const Line3f& line,float t,const Vector3f& n) {
    Collision retVal;
    retVal.hit = false;

	if (t>1.f) { return retVal; }

	Plane p = Plane(n,line.pointB.subtract(line.pointA).multiply(t).add(line.pointA));
	if (p.normal.dotProduct(line.pointB.subtract(line.pointA))>=0) { return retVal; }
	if (p.evalAtPoint(line.pointA)<-COLLISION_EPSILON) { return retVal; }

    retVal.line = line;
    retVal.coveredAmount = t;
	retVal.normal = n;
	retVal.hit = true;
	return retVal;
}

Collision Collision::sphereCollide(const Line3f& line,float radius,const Vector3f& otherPos,float other_radius) {
    Collision retVal;
    retVal.hit = false;

	radius+=other_radius;
	//Line3f l = Line3f(line.pointA.subtract(otherPos),line.pointB.subtract(otherPos));
    Vector3f lineDir = line.pointB.subtract(line.pointA);

	float a=lineDir.dotProduct(lineDir);
	if (fabs(a)<COLLISION_EPSILON) { return retVal; }
	float b=line.pointA.dotProduct(lineDir)*2;
	float c=line.pointA.dotProduct(line.pointA)-radius*radius;
	float d=b*b-4*a*c;
	if (d<0) { return retVal; }

	float t1=(-b+sqrt(d))/(2*a);
	float t2=(-b-sqrt(d))/(2*a);

	float t=t1<t2 ? t1 : t2;

	if (t<0.f || t>1.f) { return retVal; }

    if (line.pointA.add(lineDir.multiply(t)).length()<COLLISION_EPSILON) { return retVal; }
	return update(line,t,line.pointA.add(lineDir.multiply(t)).normalize());
}

//v0,v1 = edge verts
//pn = poly normal
//en = edge normal
Collision Collision::edgeTest(const Vector3f& v0,const Vector3f& v1,const Vector3f& pn,const Vector3f& en,const Line3f& line,float radius) {
    Collision retVal;
    retVal.hit = false;

    Vector3f tm0 = en.normalize();
    Vector3f tm1 = v1.subtract(v0).normalize();
    Vector3f tm2 = pn.normalize();

    Vector3f tm0Transposed = Vector3f(tm0.x,tm1.x,tm2.x);
    Vector3f tm1Transposed = Vector3f(tm0.y,tm1.y,tm2.y);
    Vector3f tm2Transposed = Vector3f(tm0.z,tm1.z,tm2.z);

    Vector3f sv = line.pointA.subtract(v0);
    sv = Vector3f(sv.dotProduct(tm0),sv.dotProduct(tm1),sv.dotProduct(tm2));
    Vector3f dv = line.pointB.subtract(v0);
    dv = Vector3f(dv.dotProduct(tm0),dv.dotProduct(tm1),dv.dotProduct(tm2)).subtract(sv);

	//do cylinder test...
	float a,b,c,d,t1,t2,t;
	a=(dv.x*dv.x+dv.z*dv.z);
	if( !a ) { return retVal; }					//ray parallel to cylinder
	b=(sv.x*dv.x+sv.z*dv.z)*2;
	c=(sv.x*sv.x+sv.z*sv.z)-radius*radius;
	d=b*b-4*a*c;
	if( d<0 ) { return retVal; }					//ray misses cylinder
	t1=(-b+sqrt(d))/(2*a);
	t2=(-b-sqrt(d))/(2*a);
	t=t1<t2 ? t1 : t2;
	if( t>1.f ) { return retVal; }	//intersects too far away
	Vector3f i=sv.add(dv.multiply(t)),p=Vector3f::zero;
	if( i.y>v0.distance(v1) ) { return retVal; }	//intersection above cylinder
	if( i.y>=0 ){
		p.y=i.y;
	}else{
		//below bottom of cylinder...do sphere test...
		a=dv.dotProduct(dv);
		if( !a ) { return retVal; }				//ray parallel to sphere
		b=sv.dotProduct(dv)*2;
		c=sv.dotProduct(sv)-radius*radius;
		d=b*b-4*a*c;
		if( d<0 ) { return retVal; }				//ray misses sphere
		t1=(-b+sqrt(d))/(2*a);
		t2=(-b-sqrt(d))/(2*a);
		t=t1<t2 ? t1 : t2;
		if( t>1.f ) { return retVal; }
		i=sv.add(dv.multiply(t));
	}

	Vector3f n = i.subtract(p);
	n = Vector3f(n.dotProduct(tm0Transposed),n.dotProduct(tm1Transposed),n.dotProduct(tm2Transposed));
	return update(line,t,n);
}

Collision Collision::triangleCollide(const PGE::Line3f& line,float radius,const PGE::Vector3f& v0,const PGE::Vector3f& v1,const PGE::Vector3f& v2) {
    Collision retVal;
    retVal.hit = false;

	//triangle plane
	Plane p = Plane(v0,v1,v2);
	if (p.normal.dotProduct(line.pointB.subtract(line.pointA))>=0) { return retVal; }

	//move plane out
	p.distanceFromOrigin+=radius;
	float t = -p.evalAtPoint(line.pointA)/p.normal.dotProduct(line.pointB.subtract(line.pointA));
	//if (t<0.f || t>1.f) { return retVal; }

	//edge planes
	Plane p0( v0.add(p.normal),v1,v0 ),p1( v1.add(p.normal),v2,v1 ),p2( v2.add(p.normal),v0,v2 );

	//intersects triangle?
	Vector3f i=line.pointB.subtract(line.pointA).multiply(t).add(line.pointA);

	float eval0 = p0.evalAtPoint(i);
	float eval1 = p1.evalAtPoint(i);
	float eval2 = p2.evalAtPoint(i);

	if (eval0>=0 && eval1>=0 && eval2>=0) {
		return update(line,t,p.normal);
	}
	if (radius<=0) { return retVal; }

	Collision temp;
	temp = edgeTest(v0,v1,p.normal,p0.normal,line,radius);
	if (temp.hit) {
        retVal = temp;
    }
	temp = edgeTest(v1,v2,p.normal,p1.normal,line,radius);
	if (temp.hit) {
        if (!retVal.hit || retVal.coveredAmount>temp.coveredAmount) {
            retVal = temp;
        }
    }
	temp = edgeTest(v2,v0,p.normal,p2.normal,line,radius);
	if (temp.hit) {
        if (!retVal.hit || retVal.coveredAmount>temp.coveredAmount) {
            retVal = temp;
        }
    }

	return retVal;
}
