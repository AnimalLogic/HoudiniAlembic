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
 * NAME:	ROP_AbcContext.h ( ROP Library, C++)
 *
 * COMMENTS:
 */

#ifndef __ROP_AbcContext__
#define __ROP_AbcContext__

#include <GABC/GABC_Include.h>
#include <GABC/GABC_OOptions.h>
#include <Alembic/AbcGeom/All.h>
#include <OP/OP_Context.h>
#include <UT/UT_Array.h>
#include <UT/UT_String.h>

class UT_WorkBuffer;

/// Alembic evaluation context
class ROP_AbcContext : public GABC_OOptions
{
public:
    typedef Alembic::Abc::TimeSamplingPtr	TimeSamplingPtr;

     ROP_AbcContext();
    ~ROP_AbcContext();

    /// @{
    /// Time sampling
    virtual const TimeSamplingPtr	&timeSampling() const
					    { return myTimeSampling; }
    const UT_Array<fpreal>	&blurTimes() const { return myBlurTimes; }
    void			setTimeSampling(fpreal tstart,
						fpreal tstep,
						int mb_samples = 1,
						fpreal shutter_open = 0,
						fpreal shutter_close = 0);
    /// @}

    /// @{
    /// Manage sub-frame time sampling.  To write a single frame, you'll want
    /// to have code like:
    /// @code
    ///   void writeFrame(fpreal time)
    ///   {
    ///       for (exint i = 0; i < ctx.samplesPerFrame(); ++i)
    ///       {
    ///           ctx.setTime(time, i);
    ///           ...
    ///       }
    ///   }
    /// @endcode
    exint		samplesPerFrame() const
			{
			    return myBlurTimes.entries();
			}
    void		setTime(fpreal base_time, exint samp);
    /// @}

    /// @{
    /// Access to the cook context
    fpreal	  cookTime() const
		    { return myCookContext.getTime(); }
    OP_Context	 &cookContext() const
		    { return const_cast<ROP_AbcContext *>(this)->myCookContext; }
    /// @}

    /// @{
    /// Whether to collapse leaf node identity transforms
    bool	 collapseIdentity() const	{ return myCollapseIdentity; }
    void	 setCollapseIdentity(bool v)	{ myCollapseIdentity = v; }
    /// @}

    /// @{
    /// Set partition attribute for splitting geometry into different shapes
    enum
    {
	// When partitioning using a string attribute that represents the full
	// path for shape nodes, there are different ways we can name the
	// resulting shape node.  All of these will have invalid characters
	// (i.e. '/') replaced with '_'.
	PATHMODE_FULLPATH,	// Full string (default mode)
	PATHMODE_SHAPE,		// The tail of the path (shape node)
	PATHMODE_XFORM,		// The second last path component
	PATHMODE_XFORM_SHAPE,	// The last two components of the path
    };
    int		 partitionMode() const		{ return myPartitionMode; }
    void	 setPartitionMode(int m)	{ myPartitionMode = m; }
    const char	*partitionAttribute() const	{ return myPartitionAttribute; }
    void	 setPartitionAttribute(const char *s)
		    { myPartitionAttribute.harden(s); }
    void	 clearPartitionAttribute()
		 {
		     myPartitionMode = PATHMODE_FULLPATH;
		     myPartitionAttribute.clear();
		 }
    /// @}

    /// @{
    /// Utility method to take the value of the partition attribute and
    /// generate the name according to the partition mode.  Any slashes ('/')
    /// will be replaced with the replace_char.
    const char 		*partitionModeValue(const char *value,
				UT_WorkBuffer &storage,
				char replace_char=':') const
			{
			    return partitionModeValue(myPartitionMode,
				    value, storage, replace_char);
			}
    static const char	*partitionModeValue(int mode, const char *value,
				UT_WorkBuffer &storage,
				char replace_char=':');
    /// @}

private:
    TimeSamplingPtr	myTimeSampling;
    UT_Array<fpreal>	myBlurTimes;	// Sub-frame offsets for motion blur
    OP_Context		myCookContext;
    UT_String		myPartitionAttribute;
    fpreal		myWriteTime;
    int			myPartitionMode;
    bool		myCollapseIdentity;
};

#endif
