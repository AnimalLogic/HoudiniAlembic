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
 * NAME:	GABC_Util.h ( GA Library, C++)
 *
 * COMMENTS:
 */

#ifndef __GABC_Util__
#define __GABC_Util__

#include "GABC_API.h"
#include "GABC_Include.h"

// Houdini includes
#include <SYS/SYS_Types.h>
#include <UT/UT_Matrix4.h>

// We need the definition of IObject
#include <Alembic/Abc/IObject.h>

class UT_StringArray;

class GABC_API GABC_Util
{
public:
    typedef Alembic::Abc::IObject	IObject;
    typedef std::vector<std::string>	PathList;

    /// A simple enum for the different Alembic node types supported
    enum NodeType
    {
	GABC_UNKNOWN=-1,
	GABC_XFORM,
	GABC_POLYMESH,
	GABC_SUBD,
	GABC_CAMERA,
	GABC_FACESET,
	GABC_CURVES,
	GABC_POINTS,
	GABC_NUPATCH,
    };

    /// Determine the node type given an input object.  If GABC_UNKNOWN is
    /// returned, you can still check the ObjectHeader to see if it matches
    /// other object types.
    static NodeType	getNodeType(const IObject &obj);

    /// Get name associated with the enum
    static const char	*nodeType(NodeType token);
    static NodeType	 nodeType(const char *name);

    /// Classify a transform node
    static bool		isMayaLocator(const IObject &obj);

    /// Class used in traversal of Alembic trees
    class GABC_API Walker
    {
    public:
	Walker()
	    : myFilename()
	{}
	virtual ~Walker();

	/// @c preProcess() is called on the "root" of the walk.  The root may
	/// @b not be the root of the Alembic file (i.e. when walking lists of
	/// objects).  The @c preProcess() method will be called one time only.
	virtual bool	preProcess(const IObject &node);

	/// Return true to continue traveral and process the children of the
	/// given node.  Returning false will process the next sibling.  Use
	/// interrupted() to perform an early termination.
	virtual bool	process(const IObject &node) = 0;

	/// Allow for interruption of walk
	virtual bool	interrupted() const	{ return false; }

	/// Manually walk children of the given node.  Returns false if the
	/// walk was interrupted, true if the walk was completed.
	bool		walkChildren(const IObject &node);

	/// @{
	///  Access the current filename
	const std::string	filename() const	{ return myFilename; }
	void			setFilename(const std::string &f)
					{ myFilename = f; }
	/// @}

    private:
	std::string	myFilename;
	friend class GABC_Util;
    };

    /// Clear the cache.  If no filename is given, the entire cache will be
    /// cleared.
    static void		clearCache(const char *filename=NULL);

    /// Set the cache size
    static void		setFileCacheSize(int nfiles);

    /// Get the file cache size
    static int		fileCacheSize();

    /// Find a given IObject in an Alembic file.
    static IObject	findObject(const std::string &filename,
				const std::string &objectpath);

    /// Walk the tree in an alembic file.  Returns false if traversal was
    /// interrupted, otherwise returns true.
    static bool		walk(const std::string &filename, Walker &walker);

    /// Process a list of unique objects in an Alembic file (including their
    /// children)
    static bool		walk(const std::string &filename, Walker &walker,
			    const UT_StringArray &objects);

    /// Get the local transform for a given node in an Alembic file.  The @c
    /// isConstant flag will be true if the local transform is constant (even
    /// if parent transforms are non-constant).
    static bool		getLocalTransform(
				const std::string &filename,
				const std::string &objectpath,
				fpreal sample_time,
				UT_Matrix4D &xform,
				bool &isConstant,
				bool &inheritsXform);

    /// Get the world transform for a given node in an Alembic file.  If the
    /// given node is a shape node, the transform up to its parent will be
    /// computed.  If the transform is constant (including all parents), the @c
    /// isConstant flag will be set.
    ///
    /// The method returns false if there was an error computing the transform.
    static bool		 getWorldTransform(
				const std::string &filename,
				const std::string &objectpath,
				fpreal sample_time,
				UT_Matrix4D &xform,
				bool &isConstant);

    /// Get the world transform for an IObject in an Alembic file.  If the
    /// given node is a shape node, the transform up to its parent will be
    /// returned.
    static bool		 getWorldTransform(
				const std::string &filename,
				const IObject &object,
				fpreal sample_time,
				UT_Matrix4D &xform,
				bool &isConstant);

    /// Return a list of all the objects in an Alembic file
    static const PathList	&getObjectList(const std::string &filename);
};

#endif
