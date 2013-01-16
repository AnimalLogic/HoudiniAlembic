/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * Produced by:
 *	Side Effects Software Inc
 *	123 Front Street West, Suite 1401
 *	Toronto, Ontario
 *	Canada   M5J 2M2
 *	416-504-9876
 *
 * NAME:	GABC_OGTGeometry.h
 *
 * COMMENTS:
 */

#ifndef __GABC_OGTGeometry__
#define __GABC_OGTGeometry__

#include "GABC_API.h"
#include "GABC_Include.h"
#include <GT/GT_Primitive.h>
#include <Alembic/AbcGeom/All.h>

class GABC_OError;
class GABC_OOptions;
class GABC_OProperty;

/// Take a GT_Primitive and translate it to Alembic.
class GABC_API GABC_OGTGeometry
{
public:
    typedef Alembic::Abc::OObject		OObject;
    typedef UT_SymbolMap<GABC_OProperty *>	PropertyMap;
    typedef Alembic::AbcGeom::OPolyMesh		OPolyMesh;
    typedef Alembic::AbcGeom::OSubD		OSubD;
    typedef Alembic::AbcGeom::OCurves		OCurves;
    typedef Alembic::AbcGeom::OPoints		OPoints;
    typedef Alembic::AbcGeom::ONuPatch		ONuPatch;

     GABC_OGTGeometry(const std::string &name);
    ~GABC_OGTGeometry();

    /// Return true if the primitive can be processed
    static bool		isPrimitiveSupported(const GT_PrimitiveHandle &prim);

    bool	start(const GT_PrimitiveHandle &prim,
			const OObject &parent,
			GABC_OError &err,
			const GABC_OOptions &ctx);
    bool	update(const GT_PrimitiveHandle &prim,
			GABC_OError &err,
			const GABC_OOptions &ctx);

    /// The intrinsic cache is used to cache array values frame to frame when
    /// optimizing the .abc file for space.  Only arrays which change will be
    /// written to the file.  The cache has storage for most primitive types.
    class GABC_API IntrinsicCache
    {
    public:
	 IntrinsicCache() {}
	~IntrinsicCache() { clear(); }

	void	clear();

	/// @{
	/// Test whether topology arrays need to be written
	bool	needVertex(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &vertex_list)
		    { return needWrite(ctx, vertex_list, myVertexList); }
	bool	needCounts(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &counts)
		    { return needWrite(ctx, counts, myCounts); }
	/// @}

	/// Test to see if the attribute needs to be written (true) or whether
	/// we can use the sample from the previous frame (false)
	bool	needWrite(const GABC_OOptions &ctx,
			const char *name, const GT_DataArrayHandle &data);

	GT_DataArrayHandle	&vertexList()	{ return myVertexList; }
	GT_DataArrayHandle	&counts()	{ return myCounts; }
	GT_DataArrayHandle	&P()	{ return myP; }
	GT_DataArrayHandle	&Pw()	{ return myPw; }
	GT_DataArrayHandle	&N()	{ return myN; }
	GT_DataArrayHandle	&uv()	{ return myUV; }
	GT_DataArrayHandle	&v()	{ return myVel; }
	GT_DataArrayHandle	&id()	{ return myId; }
	GT_DataArrayHandle	&width()	{ return myWidth; }
	GT_DataArrayHandle	&uknots()	{ return myUKnots; }
	GT_DataArrayHandle	&vknots()	{ return myVKnots; }

    private:
	bool	needWrite(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data,
			GT_DataArrayHandle &cache);
	GT_DataArrayHandle	myVertexList;
	GT_DataArrayHandle	myCounts;
	GT_DataArrayHandle	myP;
	GT_DataArrayHandle	myPw;
	GT_DataArrayHandle	myN;
	GT_DataArrayHandle	myUV;
	GT_DataArrayHandle	myVel;
	GT_DataArrayHandle	myId;
	GT_DataArrayHandle	myWidth;
	GT_DataArrayHandle	myUKnots;
	GT_DataArrayHandle	myVKnots;
    };
    /// Secondary cache is used to cache values for subdivision tags and trim
    /// curves.  This is an optional cache and is only created for some
    /// primitive types.
    class GABC_API SecondaryCache
    {
    public:
	 SecondaryCache() {}
	~SecondaryCache() { clear(); }

	/// @{
	/// Access trim curve information
	GT_DataArrayHandle	&trimNCurves()	{ return myData[0]; }
	GT_DataArrayHandle	&trimN()	{ return myData[1]; }
	GT_DataArrayHandle	&trimOrder()	{ return myData[2]; }
	GT_DataArrayHandle	&trimKnot()	{ return myData[3]; }
	GT_DataArrayHandle	&trimMin()	{ return myData[4]; }
	GT_DataArrayHandle	&trimMax()	{ return myData[5]; }
	GT_DataArrayHandle	&trimU()	{ return myData[6]; }
	GT_DataArrayHandle	&trimV()	{ return myData[7]; }
	GT_DataArrayHandle	&trimW()	{ return myData[8]; }
	/// @}

	/// Updates the cache with the current values and returns @c true if
	/// the values have changed or @c false if the previous values can be
	/// used.  There's no way in the Alembic API to set trim components
	/// individually.
	bool	needTrimNCurves(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimNCurves()); }
		}
	bool	needTrimN(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimN()); }
		}
	bool	needTrimOrder(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimOrder()); }
		}
	bool	needTrimKnot(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimKnot()); }
		}
	bool	needTrimMin(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimMin()); }
		}
	bool	needTrimMax(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimMax()); }
		}
	bool	needTrimU(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimU()); }
		}
	bool	needTrimV(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimV()); }
		}
	bool	needTrimW(const GABC_OOptions &ctx,
				const GT_DataArrayHandle &data)
		{
		    { return needWrite(ctx, data, trimW()); }
		}

	/// @{
	/// Access to fixed subdivision tags
	GT_DataArrayHandle	&creaseIndices()	{ return myData[0]; }
	GT_DataArrayHandle	&creaseLengths()	{ return myData[1]; }
	GT_DataArrayHandle	&creaseSharpnesses()	{ return myData[2]; }
	GT_DataArrayHandle	&cornerIndices()	{ return myData[3]; }
	GT_DataArrayHandle	&cornerSharpnesses()	{ return myData[4]; }
	GT_DataArrayHandle	&holeIndices()		{ return myData[5]; }
	/// @}

	/// @{
	/// Check whether subdivision tags need to be written
	bool	needCreaseIndices(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, creaseIndices()); }
	bool	needCreaseLengths(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, creaseLengths()); }
	bool	needCreaseSharpnesses(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, creaseSharpnesses()); }
	bool	needCornerIndices(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, cornerIndices()); }
	bool	needCornerSharpnesses(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, cornerSharpnesses()); }
	bool	needHoleIndices(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data)
		    { return needWrite(ctx, data, holeIndices()); }
	/// @}

    private:
	void	clear();
	bool	needWrite(const GABC_OOptions &ctx,
			const GT_DataArrayHandle &data,
			GT_DataArrayHandle &cache);
	GT_DataArrayHandle	myData[9];
    };
    SecondaryCache	&getSecondaryCache();
protected:
    /// Return the secondary cache (allocating if needed)
    void	makeProperties(const GT_PrimitiveHandle &prim,
			const GABC_OOptions &ctx);
    void	writeProperties(const GT_PrimitiveHandle &prim,
			const GABC_OOptions &ctx);
    void	clearProperties();
    void	clearShape();
    void	clearCache();

private:
    enum
    {
	VERTEX_PROPERTIES,
	POINT_PROPERTIES,
	UNIFORM_PROPERTIES,
	DETAIL_PROPERTIES,
	MAX_PROPERTIES
    };
    union {
	OPolyMesh	*myPolyMesh;
	OSubD		*mySubD;
	OCurves		*myCurves;
	OPoints		*myPoints;
	ONuPatch	*myNuPatch;
	void		*myVoidPtr;
    } myShape;
    std::string		 myName;
    PropertyMap		 myProperties[MAX_PROPERTIES];
    IntrinsicCache	 myCache;	// Cache for space optimization
    SecondaryCache	*mySecondaryCache;
    int			 myType;
};

#endif
