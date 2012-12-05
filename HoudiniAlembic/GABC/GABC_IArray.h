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
 * NAME:	GABC_IArray.h ( GT Library, C++)
 *
 * COMMENTS:
 */

#ifndef __GABC_IArray__
#define __GABC_IArray__

#include "GABC_API.h"
#include "GABC_Include.h"
#include "GABC_IItem.h"
#include "GABC_IArchive.h"
#include "GABC_GTUtil.h"
#include <UT/UT_Assert.h>
#include <GT/GT_Types.h>
#include <Alembic/Abc/IArchive.h>

/// This class wraps an Alembic data array and provides convenience methods
/// that allow thread-safe access to its data.
///
/// Do not grab and hold onto the contained data array as this may cause
/// referencing issues on archives.
class GABC_API GABC_IArray : public GABC_IItem
{
public:
    typedef Alembic::Abc::ArraySamplePtr	ArraySamplePtr;
    typedef Alembic::Abc::IArrayProperty	IArrayProperty;
    typedef Alembic::Abc::ISampleSelector	ISampleSelector;
    typedef Alembic::Abc::index_t		index_t;
    typedef Alembic::Abc::chrono_t		chrono_t;
    typedef Alembic::Abc::PlainOldDataType	PlainOldDataType;

    /// Create an array wrapper for the given array sample.
    /// The @c interp is the "intepretation", which is one of
    ///	 - "point"
    ///	 - "vector"
    ///	 - "matrix"
    ///	 - "normal"
    ///	 - "quat"
    /// (see Abc/TypedPropertyTraits.h)
    static GABC_IArray getSample(GABC_IArchive &arch,
		const ArraySamplePtr &sample, const char *interp);
    /// Create an array wrapper for the given array sample with the specified
    /// GT_Type as the interpretation.
    static GABC_IArray getSample(GABC_IArchive &arch,
		const ArraySamplePtr &sample, GT_Type type);

    static GABC_IArray getSample(GABC_IArchive &arch,
		const IArrayProperty &prop, index_t index,
		GT_Type override_type=GT_TYPE_NONE)
    {
	return getSample(arch, prop, ISampleSelector(index), override_type);
    }
    static GABC_IArray getSample(GABC_IArchive &arch,
		const IArrayProperty &prop, chrono_t time,
		GT_Type override_type=GT_TYPE_NONE)
    {
	return getSample(arch, prop, ISampleSelector(time), override_type);
    }
    // If a type is specified, the implicit array type will be overridden
    static GABC_IArray getSample(GABC_IArchive &arch,
		const IArrayProperty &prop, const ISampleSelector &iss,
		GT_Type override_type);

    GABC_IArray()
	: GABC_IItem()
	, myContainer()
	, mySize(0)
	, myTupleSize(0)
	, myType(GT_TYPE_NONE)
    {
    }
    GABC_IArray(const GABC_IArray &src)
	: GABC_IItem(src)
	, myContainer(src.myContainer)
	, mySize(src.mySize)
	, myTupleSize(src.myTupleSize)
	, myType(src.myType)
    {
    }
    GABC_IArray(GABC_IArchive &archive,
		const ArraySamplePtr &array,
		GT_Size size,
		int tuple_size,
		GT_Storage storage,
		GT_Type tinfo=GT_TYPE_NONE)
	: GABC_IItem(&archive)
	, myContainer(array)
	, mySize(size)
	, myTupleSize(tuple_size)
	, myType(tinfo)
    {
    }
    virtual ~GABC_IArray();

    GABC_IArray	&operator=(const GABC_IArray &src)
		{
		    GABC_IItem::operator=(src);
		    myContainer = src.myContainer;
		    mySize = src.mySize;
		    myTupleSize = src.myTupleSize;
		    myType = src.myType;
		    return *this;
		}

    bool	valid() const	{ return myContainer.valid(); }

    class Container
    {
    public:
	Container()
	    : myArray()
	{
	}
	Container(const ArraySamplePtr &array)
	    : myArray(array)
	{
	}
	bool		valid() const
			{
			    return myArray && myArray->valid();
			}
	Container	&operator=(const Container &src)
			{
			    myArray = src.myArray;
			    return *this;
			}
	const void	*data() const
			{
			    UT_ASSERT(myArray && myArray->getData());
			    return myArray->getData();
			}
	PlainOldDataType	abcType() const
			{
			    UT_ASSERT(myArray && myArray->getData());
			    return myArray->getDataType().getPod();
			}
	int		extent() const
			{
			    UT_ASSERT(myArray && myArray->getData());
			    return myArray->getDataType().getExtent();
			}
	exint		size() const
			{
			    return myArray->size();
			}
    private:
	ArraySamplePtr	 myArray;
    };

    const void		*data() const		{ return myContainer.data(); }
    PlainOldDataType	 abcType() const	{ return myContainer.abcType(); }
    int			 tupleSize() const	{ return myContainer.extent(); }
    GT_Size		 entries() const	{ return myContainer.size(); }
    GT_Type		 gtType() const		{ return myType; }

    /// {
    virtual void	purge();
    /// @}
private:
    Container		myContainer;
    GT_Size		mySize;
    int			myTupleSize;
    GT_Type		myType;
    PlainOldDataType	myAbcType;
};

#endif
