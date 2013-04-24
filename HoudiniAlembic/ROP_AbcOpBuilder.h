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

#ifndef __ROP_AbcOpBuilder__
#define __ROP_AbcOpBuilder__

#include <UT/UT_Assert.h>
#include <UT/UT_Set.h>
#include <UT/UT_SharedPtr.h>
#include <OP/OP_Node.h>
#include <GABC/GABC_OError.h>

class ROP_AbcObject;
class ROP_AbcArchive;
class ROP_AbcContext;

class ROP_AbcOpBuilder
{
public:
    typedef GABC_NAMESPACE::GABC_OError	GABC_OError;
    class	InternalNode;

    ROP_AbcOpBuilder(OP_Node *root)
	: myRootNode(root)
	, myTree(root->getUniqueId())
    {
    }
    ~ROP_AbcOpBuilder()
    {
    }

    /// Add a child
    bool	addChild(GABC_OError &err, OP_Node *child);

    /// Create the ROP_AbcArchive
    void	buildTree(ROP_AbcArchive &arch, const ROP_AbcContext &ctx) const;

    /// @{
    /// Function callback for walking tree
    class WalkFunc
    {
    public:
	virtual ~WalkFunc() {}
	virtual void	process(const InternalNode &node) = 0;
    };
    void	walk(WalkFunc &func) const	{ myTree.walk(func); }
    /// @}

    /// List contents
    void	ls(bool just_leaves=true) const;

    // Tree of operator nodes
    class InternalNode
    {
    public:
	typedef UT_Map<int, InternalNode>		ChildSetType;
	typedef UT_SharedPtr<ChildSetType>	ChildSetPtr;
	typedef ChildSetType::const_iterator	const_iterator;
	typedef ChildSetType::iterator		iterator;
	InternalNode(int op_id=-1)
	    : myOpId(op_id)
	    , myKids(new ChildSetType())
	{
	}
	InternalNode(const InternalNode &src)
	    : myOpId(src.myOpId)
	    , myKids(src.myKids)
	{
	}
	~InternalNode() {}

	void	walk(WalkFunc &func) const
		{
		    func.process(*this);
		    for (const_iterator it = myKids->begin();
			    it != myKids->end(); ++it)
		    {
			it->second.walk(func);
		    }
		}

	bool	operator==(const InternalNode &n) const	{ return isEqual(n); }
	size_t	hash_value() const	{ return boost::hash_value(myOpId); }
	bool	isEqual(const InternalNode &n) const	{ return myOpId == n.myOpId; }

	bool	 valid() const		{ return myOpId >= 0; }
	int	 opid() const	{ return myOpId; }
	OP_Node	*node() const	{ return OP_Node::lookupNode(myOpId); }
	InternalNode	*child(const int op_id) const
		{
		    ChildSetType::iterator it = myKids->find(op_id);
		    if (it == myKids->end())
			return NULL;
		    return &(it->second);
		}
	InternalNode	*addChild(int op_id)
		{
		    InternalNode	&kid = myKids->operator[](op_id);
		    if (!kid.valid())
			kid = InternalNode(op_id);
		    return &kid;
		}

	size_t		childCount() const	{ return myKids->size(); }
	const_iterator	begin() const	{ return myKids->begin(); }
	const_iterator	end() const	{ return myKids->end(); }
	iterator	begin()		{ return myKids->begin(); }
	iterator	end()		{ return myKids->end(); }

    private:
	ChildSetPtr	myKids;
	int		myOpId;
    };

private:
    OP_Node		*myRootNode;
    InternalNode	 myTree;
};

#endif

