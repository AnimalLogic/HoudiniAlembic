/*
 * Copyright (c) 2013
 *	Side Effects Software Inc.  All rights reserved.
 *
 * Redistribution and use of Houdini Development Kit samples in source and
 * binary forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. The name of Side Effects Software may not be used to endorse or
 *    promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SIDE EFFECTS SOFTWARE `AS IS' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL SIDE EFFECTS SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *----------------------------------------------------------------------------
 */

#ifndef __GABC_IObject__
#define __GABC_IObject__

#include "GABC_API.h"
#include "GABC_Include.h"
#include "GABC_Types.h"
#include "GABC_IItem.h"
#include "GABC_NameMap.h"
#include "GABC_Visibility.h"
#include <UT/UT_Matrix4.h>
#include <UT/UT_BoundingBox.h>
#include <GT/GT_Handles.h>
#include <Alembic/Abc/IObject.h>
#include <Alembic/AbcGeom/GeometryScope.h>

class GEO_Primitive;
class UT_StringArray;

namespace GABC_NAMESPACE
{
/// This class wraps an Alembic IObject and provides convenience methods that
/// allow thread-safe access to its data.
///
/// Do not grab and hold onto the contained IObject as this may cause
/// referencing issues on archives.
class GABC_API GABC_IObject : public GABC_IItem
{
public:
    typedef Alembic::Abc::M44d			M44d;
    typedef Alembic::Abc::IObject		IObject;
    typedef Alembic::Abc::ObjectHeader		ObjectHeader;
    typedef Alembic::Abc::ICompoundProperty	ICompoundProperty;
    typedef Alembic::Abc::TimeSamplingPtr	TimeSamplingPtr;
    typedef Alembic::AbcGeom::GeometryScope	GeometryScope;

    GABC_IObject();
    GABC_IObject(const GABC_IObject &obj);
    GABC_IObject(const GABC_IArchivePtr &arch, const std::string &objectpath);
    virtual ~GABC_IObject();

    /// Initialize (called by GABC_IArchive)
    static void		init();

    /// Return the number of children
    exint		getNumChildren() const;

    /// @{
    /// Get the given child
    GABC_IObject	getChild(exint index) const;
    GABC_IObject	getChild(const std::string &name) const;
    /// @}

    /// Get the parent
    GABC_IObject	getParent() const
			{
			    if (!valid())
				return GABC_IObject();
			    return GABC_IObject(archive(),object().getParent());
			}

    /// Get my header
    ObjectHeader	getHeader() const
			    { return object().getHeader(); }
    /// Get the child header
    ObjectHeader	getChildHeader(exint i) const
			    { return object().getChildHeader(i); }

    /// Get the name of the object
    std::string		getName() const	{ return myObject.getName(); }

    /// Get the full name of the object
    std::string		getFullName() const { return myObject.getFullName(); }

    /// @{
    /// Interface from GABC_IItem
    virtual void	purge();
    /// @}

    /// Test validity
    bool		valid() const	{ return myObject.valid(); }

    /// Assignment operator
    GABC_IObject	&operator=(const GABC_IObject &src);

    /// Query the type of this node
    GABC_NodeType	nodeType() const;

    /// Query whether the node is a Maya locator
    bool		isMayaLocator() const;

    /// Query visibility
    GABC_VisibilityType	visibility(bool &animated, fpreal t,
				bool check_parent=false) const;

    /// Query the visibilty by acquiring a visibility cache.  Returns a new
    /// cache.
    GABC_VisibilityCache	*visibilityCache() const;

    /// Get animation type for this node.
    /// @note This only checks animation types of intrinsic properties
    GABC_AnimationType	getAnimationType(bool include_transform) const;

    /// Get the bounding box.  Returns false if there are no bounds defined
    bool		getBoundingBox(UT_BoundingBox &box, fpreal t,
				bool &isConstant) const;

    /// Get the bounding box for rendering (includes the "width" attribute for
    /// curves and points).
    bool		getRenderingBoundingBox(UT_BoundingBox &box,
				fpreal t) const;

    /// Get a representation of the shape.  If the @c GEO_Primitive pointer is
    /// non-NULL, the primitive attributes will be added as GT arrays.
    GT_PrimitiveHandle	getPrimitive(const GEO_Primitive *prim,
				fpreal t,
				GABC_AnimationType &atype,
				const GABC_NameMapPtr &namemap) const;

    /// Update primitive time.  Given a primitive created by @c getPrimitive(),
    /// update the transform/attribute arrays with values from the new time.
    GT_PrimitiveHandle	updatePrimitive(const GT_PrimitiveHandle &src,
				const GEO_Primitive *prim,
				fpreal new_time,
				const GABC_NameMapPtr &namemap) const;

    /// Get a representation of the point cloud.  This doesn't include any
    /// attributes.
    GT_PrimitiveHandle	getPointCloud(fpreal t, GABC_AnimationType &a) const;

    /// Get a representation of the bounding box.  This doesn't include any
    /// attributes.
    GT_PrimitiveHandle	getBoxGeometry(fpreal t, GABC_AnimationType &a) const;

    /// Get a point for the centroid of the bounding box.  This doesn't include
    /// any attributes.
    GT_PrimitiveHandle	getCentroidGeometry(fpreal t, GABC_AnimationType &a) const;

    /// Get local transform for the object.  For shape objects, this will
    /// return an identity transform.  The animation type only considers the
    /// local transform (not all the parent transforms).
    ///
    /// The transform will be blended between samples if the time doesn't align
    /// perfectly with a sample point.
    bool		getLocalTransform(UT_Matrix4D &xform, fpreal t,
				GABC_AnimationType &atype,
				bool &inherits) const;

    /// Get world transform for the object.  The animation type includes all
    /// parent transforms.
    ///
    /// The transform will be blended between samples if the time doesn't align
    /// perfectly with a sample point.
    bool		getWorldTransform(UT_Matrix4D &xform, fpreal t,
				GABC_AnimationType &atype) const;

    /// Query the number of geometry properties
    exint		 getNumGeometryProperties() const;
    /// Lookup the data array for the Nth geometry property at the given time.
    ///  - The @c name parameter is filled out with the property name
    ///  - The @c scope parameter is filled out with the property scope
    ///  - The @c atype parameter is filled out with the animation type
    GT_DataArrayHandle	 getGeometryProperty(exint index, fpreal t,
				std::string &name,
				GeometryScope &scope,
				GABC_AnimationType &atype) const;

    /// Get position property from shape node
    GT_DataArrayHandle	getPosition(fpreal t, GABC_AnimationType &atype) const;
    /// Get velocity property from shape node
    GT_DataArrayHandle	getVelocity(fpreal t, GABC_AnimationType &atype) const;

    /// Lookup the data array for the named geometry property at the given time.
    ///  - The @c scope parameter is filled out with the property scope
    ///  - The @c atype parameter is filled out with the animation type
    GT_DataArrayHandle	 getGeometryProperty(const std::string &name, fpreal t,
				GeometryScope &scope,
				GABC_AnimationType &atype) const;

    /// Get number of user properties for this node
    exint		getNumUserProperties() const;
    /// Lookup the data array for the Nth geometry property at the given time.
    ///  - The @c name parameter is filled out with the property name
    ///  - The @c atype parameter is filled out with the animation type
    GT_DataArrayHandle	getUserProperty(exint index, fpreal t,
				std::string &name,
				GABC_AnimationType &atype) const;
    /// Lookup the data array for the Nth geometry property at the given time.
    ///  - The @c name parameter is filled out with the property name
    ///  - The @c atype parameter is filled out with the animation type
    GT_DataArrayHandle	getUserProperty(const std::string &name, fpreal t,
				GABC_AnimationType &atype) const;

    /// Access the time sampling pointer
    TimeSamplingPtr	timeSampling();

    /// Clamp the time to the animated time range
    fpreal		clampTime(fpreal input_time);

    /// @{
    /// Get the world transform for the node.  This includes all it's parent
    /// transforms.
    bool		worldTransform(fpreal t,
				    UT_Matrix4D &xform,
				    bool &isConstant,
				    bool &inheritsXform) const;
    bool		worldTransform(fpreal t,
				    M44d &xform,
				    bool &isConstant,
				    bool &inheritsXform) const;
    /// @}

    /// @{
    /// Get the local transform for the node.  This is an identity matrix 
    bool		localTransform(fpreal t,
				    UT_Matrix4D &xform,
				    bool &isConstant,
				    bool &inheritsXform) const;
    bool		localTransform(fpreal t,
				    M44d &xform,
				    bool &isConstant,
				    bool &inheritsXform) const;
    /// @}

    /// @{
    /// Member data access
    const std::string	&objectPath() const		{ return myObjectPath; }
    const IObject	&object() const			{ return myObject; }
    /// @}

private:
    GABC_IObject(const GABC_IArchivePtr &arch, const IObject &obj);

    void		 setObject(const IObject &o)	{ myObject = o; }
    ICompoundProperty	 getArbGeomParams() const;
    ICompoundProperty	 getUserProperties() const;

    std::string		 myObjectPath;
    IObject		 myObject;

    friend class	 GABC_IArchive;
};
}

#endif
