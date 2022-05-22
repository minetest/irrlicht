// This file is part of the "Irrlicht Engine".
// Written by Reinhard Ostermeier, reinhard@nospam.r-ostermeier.de
// Expanded by burningwater
// Bugfixes by Michael Zeilfelder
// Bugfixes by Andreas Reichl

#include "CGUITreeView.h"

#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "CGUIScrollBar.h"
#include "os.h"

namespace irr
{
namespace gui
{

CGUITreeViewNode::CGUITreeViewNode( CGUITreeView* owner, CGUITreeViewNode* parent )
	: Owner(owner), Parent(parent), ImageIndex(-1), SelectedImageIndex(-1),
	Data(0), Data2(0), Expanded(false)
{
#ifdef _DEBUG
	setDebugName( "CGUITreeView" );
#endif
}

CGUITreeViewNode::~CGUITreeViewNode()
{
	if( Owner && this == Owner->getSelected() )
	{
		setSelected( false );
	}

	clearChildren();

	if( Data2 )
	{
		Data2->drop();
	}
}

IGUITreeView* CGUITreeViewNode::getOwner() const
{
	return Owner;
}

IGUITreeViewNode* CGUITreeViewNode::getParent() const
{
	return Parent;
}

void CGUITreeViewNode::setText( const wchar_t* text )
{
	Text = text;
}

void CGUITreeViewNode::setIcon( const wchar_t* icon )
{
	Icon = icon;
}

void CGUITreeViewNode::clearChildren()
{
	for (auto child : Children)
	{
		child->Parent = nullptr;
		child->drop();
	}
	Children.clear();
}

IGUITreeViewNode* CGUITreeViewNode::addChildBack(
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2 /*= 0*/ )
{
	auto newChild = new CGUITreeViewNode( Owner, this );
	newChild->ParentPos = Children.insert(Children.end(), newChild);
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
	{
		data2->grab();
	}
	return newChild;
}

IGUITreeViewNode* CGUITreeViewNode::addChildFront(
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2 /*= 0*/ )
{
	auto newChild = new CGUITreeViewNode( Owner, this );
	newChild->ParentPos = Children.insert(Children.begin(), newChild);
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
	{
		data2->grab();
	}
	return newChild;
}

IGUITreeViewNode* CGUITreeViewNode::insertChildAfter(
	IGUITreeViewNode*	iother,
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2/* = 0*/ )
{
	// This cast is needed to access the ParentPos member of `other`.
	// The abstraction was already broken, because Children is a list of
	// CGUITreeViewNode, not IGUITreeViewNode. The existing code was
	// implicitly casting through pointer comparison.
	auto other = static_cast<CGUITreeViewNode*>(iother);
	assert(other->Parent == this);
	auto newChild = new CGUITreeViewNode( Owner, this );
	newChild->ParentPos = Children.insert(std::next(other->ParentPos), newChild);
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
		data2->grab();
	return newChild;
}

IGUITreeViewNode* CGUITreeViewNode::insertChildBefore(
	IGUITreeViewNode*	iother,
	const wchar_t*		text,
	const wchar_t*		icon /*= 0*/,
	s32					imageIndex /*= -1*/,
	s32					selectedImageIndex /*= -1*/,
	void*					data /*= 0*/,
	IReferenceCounted*			data2/* = 0*/ )
{
	auto other = static_cast<CGUITreeViewNode*>(iother);
	assert(other->Parent == this);
	auto newChild = new CGUITreeViewNode( Owner, this );
	newChild->ParentPos = Children.insert(other->ParentPos, newChild);
	newChild->Text = text;
	newChild->Icon = icon;
	newChild->ImageIndex = imageIndex;
	newChild->SelectedImageIndex = selectedImageIndex;
	newChild->Data = data;
	newChild->Data2 = data2;
	if( data2 )
		data2->grab();
	return newChild;
}

IGUITreeViewNode* CGUITreeViewNode::getFirstChild() const
{
	if( Children.empty() )
	{
		return 0;
	}
	else
	{
		return Children.front();
	}
}

IGUITreeViewNode* CGUITreeViewNode::getLastChild() const
{
	if( Children.empty() )
	{
		return 0;
	}
	else
	{
		return Children.back();
	}
}

IGUITreeViewNode* CGUITreeViewNode::getPrevSibling() const
{
	if (!Parent || ParentPos == Parent->Children.begin())
		return nullptr;
	return *std::prev(ParentPos);
}

IGUITreeViewNode* CGUITreeViewNode::getNextSibling() const
{
	if (!Parent)
		return nullptr;
	auto nextIt = std::next(ParentPos);
	if (nextIt == Parent->Children.end())
		return nullptr;
	return *nextIt;
}

IGUITreeViewNode* CGUITreeViewNode::getNextVisible() const
{
	IGUITreeViewNode*	next = 0;
	const IGUITreeViewNode*	node = this;

	if( node->getExpanded() && node->hasChildren() )
	{
		next = node->getFirstChild();
	}
	else
	{
		next = node->getNextSibling();
	}
	while( !next && node->getParent() )
	{
		next = node->getParent()->getNextSibling();
		if( !next )
		{
			node = node->getParent();
		}
	}

	return next;
}

bool CGUITreeViewNode::deleteChild( IGUITreeViewNode* ichild )
{
	auto child = static_cast<CGUITreeViewNode*>(ichild);
	assert(child->Parent == this);
	Children.erase(child->ParentPos);
	child->Parent = nullptr;
	child->drop();
	return true;
}

bool CGUITreeViewNode::moveChildUp( IGUITreeViewNode* ichild )
{
	auto child = static_cast<CGUITreeViewNode*>(ichild);
	assert(child->Parent == this);
	if (child->ParentPos == Children.begin())
		return false;
	auto curPos = child->ParentPos;
	auto prevPos = std::prev(child->ParentPos);
	std::swap(*curPos, *prevPos);
	std::swap((*curPos)->ParentPos, (*prevPos)->ParentPos);
	return true;
}

bool CGUITreeViewNode::moveChildDown( IGUITreeViewNode* ichild )
{
	auto child = static_cast<CGUITreeViewNode*>(ichild);
	assert(child->Parent == this);
	auto nextPos = std::next(child->ParentPos);
	if (nextPos == Children.end())
		return false;
	auto curPos = child->ParentPos;
	std::swap(*curPos, *nextPos);
	std::swap((*curPos)->ParentPos, (*nextPos)->ParentPos);
	return true;
}

void CGUITreeViewNode::setExpanded( bool expanded )
{
	Expanded = expanded;
}

void CGUITreeViewNode::setSelected( bool selected )
{
	if( Owner )
	{
		if( selected )
		{
			Owner->Selected = this;
		}
		else
		{
			if( Owner->Selected == this )
			{
				Owner->Selected = 0;
			}
		}
	}
}

bool CGUITreeViewNode::getSelected() const
{
	if( Owner )
	{
		return Owner->Selected == (IGUITreeViewNode*)this;
	}
	else
	{
		return false;
	}
}

bool CGUITreeViewNode::isRoot() const
{
	return ( Owner && ( this == Owner->Root ) );
}

s32 CGUITreeViewNode::getLevel() const
{
	if( Parent )
	{
		return Parent->getLevel() + 1;
	}
	else
	{
		return 0;
	}
}

bool CGUITreeViewNode::isVisible() const
{
	if( Parent )
	{
		return Parent->getExpanded() && Parent->isVisible();
	}
	else
	{
		return true;
	}
}


//! constructor
CGUITreeView::CGUITreeView(IGUIEnvironment* environment, IGUIElement* parent,
	s32 id, core::rect<s32> rectangle, bool clip,
	bool drawBack,bool scrollBarVertical, bool scrollBarHorizontal)
	: IGUITreeView( environment, parent, id, rectangle ),
	Root(0), Selected(0),
	ItemHeight( 0 ),
	IndentWidth( 0 ),
	TotalItemHeight( 0 ),
	TotalItemWidth ( 0 ),
	ScrollBarSize( 0 ),
	Font( 0 ),
	OverrideFont( 0 ),
	IconFont( 0 ),
	ScrollBarH( 0 ),
	ScrollBarV( 0 ),
	ImageList( 0 ),
	LastEventNode( 0 ),
	LinesVisible( true ),
	Selecting( false ),
	Clip( clip ),
	DrawBack( drawBack ),
	ImageLeftOfIcon( true )
{
#ifdef _DEBUG
	setDebugName( "CGUITreeView" );
#endif

	IGUISkin* skin = Environment->getSkin();
	ScrollBarSize = skin->getSize( EGDS_SCROLLBAR_SIZE );

	if ( scrollBarVertical )
	{
		ScrollBarV = new CGUIScrollBar( false, Environment, this, -1,
			core::rect<s32>(	RelativeRect.getWidth() - ScrollBarSize,
								0,
								RelativeRect.getWidth(),
								RelativeRect.getHeight() - ScrollBarSize
			), !clip );
		ScrollBarV->drop();

		ScrollBarV->setSubElement(true);
		ScrollBarV->setPos( 0 );
		ScrollBarV->grab();
	}

	if ( scrollBarHorizontal )
	{
		ScrollBarH = new CGUIScrollBar( true, Environment, this, -1,
			core::rect<s32>(	0,
								RelativeRect.getHeight() - ScrollBarSize,
								RelativeRect.getWidth() - ScrollBarSize,
								RelativeRect.getHeight()
			), !clip );
		ScrollBarH->drop();

		ScrollBarH->setSubElement(true);
		ScrollBarH->setPos( 0 );
		ScrollBarH->grab();
	}

	Root = new CGUITreeViewNode( this, 0 );
	Root->Expanded = true;

	recalculateItemHeight();
}


//! destructor
CGUITreeView::~CGUITreeView()
{
	if( ScrollBarV )
	{
		ScrollBarV->drop();
	}

	if( ScrollBarH )
	{
		ScrollBarH->drop();
	}

	if( Font )
	{
		Font->drop();
	}

	if( IconFont )
	{
		IconFont->drop();
	}

	if( ImageList )
	{
		ImageList->drop();
	}

	if( Root )
	{
		Root->drop();
	}
}

//! Sets another skin independent font.
void CGUITreeView::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont == font)
		return;

	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();

	recalculateItemHeight();
}

//! Gets the override font (if any)
IGUIFont * CGUITreeView::getOverrideFont() const
{
	return OverrideFont;
}

//! Get the font which is used right now for drawing
IGUIFont* CGUITreeView::getActiveFont() const
{
	if ( OverrideFont )
		return OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (skin)
		return skin->getFont();
	return 0;
}

void CGUITreeView::recalculateItemHeight()
{
	if( Font != getActiveFont() )
	{
		if( Font )
		{
			Font->drop();
		}

		Font = getActiveFont();
		ItemHeight = 0;

		if( Font )
		{
			ItemHeight = Font->getDimension( L"A" ).Height + 4;
			Font->grab();
		}

		if( IconFont )
		{
			s32 height = IconFont->getDimension( L" " ).Height;
			if( height > ItemHeight )
			{
				ItemHeight = height;
			}
		}
		if( ImageList )
		{
			if( ImageList->getImageSize().Height + 1 > ItemHeight )
			{
				ItemHeight = ImageList->getImageSize().Height + 1;
			}
		}
	}

	IndentWidth = ItemHeight;
	if( IndentWidth < 9 )
	{
		IndentWidth = 9;
	}
	else if( IndentWidth > 15 )
	{
		IndentWidth = 15;
	}
	else
	{
		if( ( ( IndentWidth >> 1 ) << 1 ) - IndentWidth == 0 )
		{
			--IndentWidth;
		}
	}

	TotalItemHeight = 0;
	TotalItemWidth = AbsoluteRect.getWidth() * 2;
	IGUITreeViewNode* node = Root->getFirstChild();
	while( node )
	{
		TotalItemHeight += ItemHeight;
		node = node->getNextVisible();
	}

	if ( ScrollBarV )
	{
		s32 diffHor = TotalItemHeight - AbsoluteRect.getHeight();
		if ( ScrollBarH )
		{
			diffHor += ScrollBarH->getAbsolutePosition().getHeight();
		}
		ScrollBarV->setMax( core::max_( 0, diffHor) );
	}

	if ( ScrollBarH )
	{
		s32 diffVert = TotalItemWidth - AbsoluteRect.getWidth();
		if ( ScrollBarV )
		{
			// TODO: not sure yet if it needs handling
		}
		ScrollBarH->setMax( core::max_( 0, diffVert ) );
	}

}

void CGUITreeView::updateScrollBarSize(s32 size)
{
	if ( size != ScrollBarSize )
	{
		ScrollBarSize = size;

		if ( ScrollBarV )
		{
			core::recti r(RelativeRect.getWidth() - ScrollBarSize, 0,
			              RelativeRect.getWidth(), RelativeRect.getHeight() - ScrollBarSize);
			ScrollBarV->setRelativePosition(r);
		}

		if ( ScrollBarH ) 
		{
			core::recti r(0, RelativeRect.getHeight() - ScrollBarSize,
			              RelativeRect.getWidth() - ScrollBarSize, RelativeRect.getHeight());
			ScrollBarH->setRelativePosition(r);
		}
	}
}

//! called if an event happened.
bool CGUITreeView::OnEvent( const SEvent &event )
{
	if ( isEnabled() )
	{
		switch( event.EventType )
		{
		case EET_GUI_EVENT:
			switch( event.GUIEvent.EventType )
			{
			case gui::EGET_SCROLL_BAR_CHANGED:
				if( event.GUIEvent.Caller == ScrollBarV || event.GUIEvent.Caller == ScrollBarH )
				{
					//s32 pos = ( ( gui::IGUIScrollBar* )event.GUIEvent.Caller )->getPos();
					return true;
				}
				break;
			case gui::EGET_ELEMENT_FOCUS_LOST:
				{
					Selecting = false;
					return false;
				}
				break;
			default:
				break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			{
				core::position2d<s32> p( event.MouseInput.X, event.MouseInput.Y );

				switch( event.MouseInput.Event )
				{
				case EMIE_MOUSE_WHEEL:
					if ( ScrollBarV )
						ScrollBarV->setPos( ScrollBarV->getPos() + (event.MouseInput.Wheel < 0 ? -1 : 1) * -10 );
					return true;
					break;

				case EMIE_LMOUSE_PRESSED_DOWN:
					if( ( ScrollBarV && ScrollBarV->getAbsolutePosition().isPointInside( p ) && ScrollBarV->OnEvent( event ) ) ||
						( ScrollBarH && ScrollBarH->getAbsolutePosition().isPointInside( p ) &&	ScrollBarH->OnEvent( event ) )
						)
					{
						return true;
					}

					Selecting = true;
					return true;
					break;

				case EMIE_LMOUSE_LEFT_UP:
					if( ( ScrollBarV && ScrollBarV->getAbsolutePosition().isPointInside( p ) && ScrollBarV->OnEvent( event ) ) ||
						( ScrollBarH && ScrollBarH->getAbsolutePosition().isPointInside( p ) &&	ScrollBarH->OnEvent( event ) )
						)
					{
						return true;
					}

					Selecting = false;
					mouseAction( event.MouseInput.X, event.MouseInput.Y );
					return true;
					break;

				case EMIE_MOUSE_MOVED:
					if( Selecting )
					{
						if( getAbsolutePosition().isPointInside( p ) )
						{
							mouseAction( event.MouseInput.X, event.MouseInput.Y, true );
							return true;
						}
					}
					break;

				default:
					break;
				}
			}
			break;
		default:
			break;
		}
	}

	return Parent ? Parent->OnEvent( event ) : false;
}

/*!
*/
void CGUITreeView::mouseAction( s32 xpos, s32 ypos, bool onlyHover /*= false*/ )
{
	IGUITreeViewNode*		oldSelected = Selected;
	s32						selIdx=-1;
	SEvent					event;

	event.EventType			= EET_GUI_EVENT;
	event.GUIEvent.Caller	= this;
	event.GUIEvent.Element = 0;

	xpos -= AbsoluteRect.UpperLeftCorner.X;
	ypos -= AbsoluteRect.UpperLeftCorner.Y;

	// find new selected item.
	s32 scrollBarVPos = ScrollBarV ? ScrollBarV->getPos() : 0;
	if( ItemHeight != 0 )
	{
		selIdx = ( ( ypos - 1 ) + scrollBarVPos ) / ItemHeight;
	}

	IGUITreeViewNode* hitNode = 0;
	IGUITreeViewNode* node = Root->getFirstChild();
	s32	n = 0;
	while( node )
	{
		if( selIdx == n )
		{
			hitNode = node;
			break;
		}
		node = node->getNextVisible();
		++n;
	}

	s32 scrollBarHPos = ScrollBarH ? ScrollBarH->getPos() : 0;
	xpos += scrollBarHPos; // correction for shift
	if( hitNode && xpos > hitNode->getLevel() * IndentWidth )
	{
		Selected = hitNode;
	}

	if( hitNode && !onlyHover
		&& xpos < hitNode->getLevel() * IndentWidth
		&& xpos > ( hitNode->getLevel() - 1 ) * IndentWidth
		&& hitNode->hasChildren() )
	{
		hitNode->setExpanded( !hitNode->getExpanded() );

		// post expand/collaps news
		if( hitNode->getExpanded() )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_EXPAND;
		}
		else
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_COLLAPS;
		}
		LastEventNode = hitNode;
		Parent->OnEvent( event );
		LastEventNode = 0;
	}

	if( Selected && !Selected->isVisible() )
	{
		Selected = 0;
	}

	// post selection news

	if( Parent && !onlyHover && Selected != oldSelected )
	{
		if( oldSelected )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_DESELECT;
			LastEventNode = oldSelected;
			Parent->OnEvent( event );
			LastEventNode = 0;
		}
		if( Selected )
		{
			event.GUIEvent.EventType = EGET_TREEVIEW_NODE_SELECT;
			LastEventNode = Selected;
			Parent->OnEvent( event );
			LastEventNode = 0;
		}
	}
}

//! draws the element and its children
void CGUITreeView::draw()
{
	if( !IsVisible )
	{
		return;
	}

	IGUISkin* skin = Environment->getSkin();

	updateScrollBarSize(skin->getSize(EGDS_SCROLLBAR_SIZE));
	recalculateItemHeight(); // if the font changed

	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	core::rect<s32>* clipRect = 0;
	if( Clip )
	{
		clipRect = &AbsoluteClippingRect;
	}

	// draw background

	core::rect<s32> frameRect( AbsoluteRect );

	if( DrawBack )
	{
		driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect, clipRect );
	}

	// draw the border

	frameRect.LowerRightCorner.Y = frameRect.UpperLeftCorner.Y + 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), frameRect, clipRect );

	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	frameRect.LowerRightCorner.X = frameRect.UpperLeftCorner.X + 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), frameRect, clipRect );

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.X  = frameRect.LowerRightCorner.X - 1;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect, clipRect );

	frameRect = AbsoluteRect;
	frameRect.UpperLeftCorner.Y  = AbsoluteRect.LowerRightCorner.Y - 1;
	frameRect.LowerRightCorner.Y = AbsoluteRect.LowerRightCorner.Y;
	driver->draw2DRectangle( skin->getColor( EGDC_3D_HIGH_LIGHT ), frameRect, clipRect );

	// draw items

	core::rect<s32> clientClip( AbsoluteRect );
	clientClip.UpperLeftCorner.X  += 1;
	clientClip.UpperLeftCorner.Y  += 1;
	clientClip.LowerRightCorner.X  = AbsoluteRect.LowerRightCorner.X;
	clientClip.LowerRightCorner.Y -= 1;

	if ( ScrollBarV )
		clientClip.LowerRightCorner.X -= ScrollBarSize;
	if ( ScrollBarH )
		clientClip.LowerRightCorner.Y -= ScrollBarSize;

	if( clipRect )
	{
		clientClip.clipAgainst( *clipRect );
	}

	frameRect = AbsoluteRect;
	frameRect.LowerRightCorner.X = AbsoluteRect.LowerRightCorner.X - ScrollBarSize;
	frameRect.LowerRightCorner.Y = AbsoluteRect.UpperLeftCorner.Y + ItemHeight;

	if ( ScrollBarV )
	{
		frameRect.UpperLeftCorner.Y  -= ScrollBarV->getPos();
		frameRect.LowerRightCorner.Y -= ScrollBarV->getPos();
	}

	IGUITreeViewNode* node = Root->getFirstChild();
	while( node )
	{
		frameRect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X + 1 + node->getLevel() * IndentWidth;
		if ( ScrollBarH )
		{
			frameRect.UpperLeftCorner.X  -= ScrollBarH->getPos();
		}

		if( frameRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y
			&& frameRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y )
		{
			if( node == Selected )
			{
				// selection box beginning from far left
				core::rect<s32> copyFrameRect( frameRect ); // local copy to keep original untouched
				copyFrameRect.UpperLeftCorner.X = AbsoluteRect.UpperLeftCorner.X + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_HIGH_LIGHT ), copyFrameRect, &clientClip );
			}

			if( node->hasChildren() )
			{
				core::rect<s32> rc;
				core::rect<s32> expanderRect;

				expanderRect.UpperLeftCorner.X = frameRect.UpperLeftCorner.X - IndentWidth + 2;
				expanderRect.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y + ( ( frameRect.getHeight() - ( IndentWidth - 4 ) ) >> 1 );
				expanderRect.LowerRightCorner.X = expanderRect.UpperLeftCorner.X + IndentWidth - 4;
				expanderRect.LowerRightCorner.Y = expanderRect.UpperLeftCorner.Y + IndentWidth - 4;

				// box upper line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = expanderRect.LowerRightCorner.X;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_DARK_SHADOW ), rc, clipRect );

				// box left line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
				rc.LowerRightCorner.Y = expanderRect.LowerRightCorner.Y;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_DARK_SHADOW ), rc, clipRect );

				// box right line
				rc.UpperLeftCorner.X = expanderRect.LowerRightCorner.X - 1;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y;
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
				rc.LowerRightCorner.Y = expanderRect.LowerRightCorner.Y;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_DARK_SHADOW ), rc, clipRect );

				// box bottom line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X;
				rc.UpperLeftCorner.Y = expanderRect.LowerRightCorner.Y - 1;
				rc.LowerRightCorner.X = expanderRect.LowerRightCorner.X;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_DARK_SHADOW ), rc, clipRect );

				// horizontal '-' line
				rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X + 2;
				rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y + ( expanderRect.getHeight() >> 1 );
				rc.LowerRightCorner.X = rc.UpperLeftCorner.X + expanderRect.getWidth() - 4;
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_BUTTON_TEXT ), rc, clipRect );

				if( !node->getExpanded() )
				{
					// vertical '+' line
					rc.UpperLeftCorner.X = expanderRect.UpperLeftCorner.X + ( expanderRect.getWidth() >> 1 );
					rc.UpperLeftCorner.Y = expanderRect.UpperLeftCorner.Y + 2;
					rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
					rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + expanderRect.getHeight() - 4;
					driver->draw2DRectangle( skin->getColor( EGDC_BUTTON_TEXT ), rc, clipRect );
				}
			}

			core::rect<s32> textRect = frameRect;

			if( Font )
			{
				EGUI_DEFAULT_COLOR textCol = EGDC_GRAY_TEXT;
				if ( isEnabled() )
					textCol = ( node == Selected ) ? EGDC_HIGH_LIGHT_TEXT : EGDC_BUTTON_TEXT;

				s32 iconWidth = 0;
				for( s32 n = 0; n < 2; ++n )
				{
					s32 index = node->getImageIndex();
					if( ( ImageList && index >= 0 )
						&& ( ( ImageLeftOfIcon && n == 0 )
						|| ( !ImageLeftOfIcon && n == 1 ) ) )
					{
						index = node->getSelectedImageIndex();
						if( node != Selected || index < 0 )
						{
							index = node->getImageIndex();
						}
						ImageList->draw(
							index,
							core::position2d<s32>(
							textRect.UpperLeftCorner.X,
							textRect.UpperLeftCorner.Y + ( ( textRect.getHeight() - ImageList->getImageSize().Height ) >> 1 ) ),
							&clientClip );
						iconWidth += ImageList->getImageSize().Width + 3;
						textRect.UpperLeftCorner.X += ImageList->getImageSize().Width + 3;
					}
					else if( ( IconFont && static_cast<CGUITreeViewNode*>( node )->Icon.size() )
						&& ( ( ImageLeftOfIcon && n == 1 )
						|| ( !ImageLeftOfIcon && n == 0 ) ) )
					{
						IconFont->draw( node->getIcon(), textRect, skin->getColor(textCol), false, true, &clientClip );
						iconWidth += IconFont->getDimension( node->getIcon() ).Width + 3;
						textRect.UpperLeftCorner.X += IconFont->getDimension( node->getIcon() ).Width + 3;
					}
				}

				Font->draw( node->getText(), textRect, skin->getColor(textCol), false, true, &clientClip );

				textRect.UpperLeftCorner.X -= iconWidth;
			}

			// draw the lines if neccessary
			if( LinesVisible )
			{
				core::rect<s32> rc;

				// horizontal line
				rc.UpperLeftCorner.X = frameRect.UpperLeftCorner.X - IndentWidth - ( IndentWidth >> 1 ) - 1;
				rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y + ( ( frameRect.getHeight() ) >> 1 );
				if( node->hasChildren() )
				{
					rc.LowerRightCorner.X = frameRect.UpperLeftCorner.X - IndentWidth;
				}
				else
				{
					rc.LowerRightCorner.X = frameRect.UpperLeftCorner.X - 2;
				}
				rc.LowerRightCorner.Y = rc.UpperLeftCorner.Y + 1;
				driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc, clipRect );

				if( node->getParent() != Root )
				{
					// vertical line
					if( node == node->getParent()->getFirstChild() )
					{
						rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( ( frameRect.getHeight() - IndentWidth ) >> 1 );
					}
					else
					{
						rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( frameRect.getHeight() >> 1 );
					}
					rc.LowerRightCorner.X = rc.UpperLeftCorner.X + 1;
					driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc, clipRect );

					// the vertical lines of all parents
					IGUITreeViewNode* nodeTmp = node->getParent();
					rc.UpperLeftCorner.Y = frameRect.UpperLeftCorner.Y - ( frameRect.getHeight() >> 1 );
					for( s32 n = 0; n < node->getLevel() - 2; ++n )
					{
						rc.UpperLeftCorner.X -= IndentWidth;
						rc.LowerRightCorner.X -= IndentWidth;
						if( nodeTmp != nodeTmp->getParent()->getLastChild() )
						{
							driver->draw2DRectangle( skin->getColor( EGDC_3D_SHADOW ), rc, clipRect );
						}
						nodeTmp = nodeTmp->getParent();
					}
				}
			}
		}

		frameRect.UpperLeftCorner.Y += ItemHeight;
		frameRect.LowerRightCorner.Y += ItemHeight;

		node = node->getNextVisible();
	}

	IGUIElement::draw();
}

//! Sets the font which should be used as icon font. This font is set to the Irrlicht engine
//! built-in-font by default. Icons can be displayed in front of every list item.
//! An icon is a string, displayed with the icon font. When using the build-in-font of the
//! Irrlicht engine as icon font, the icon strings defined in GUIIcons.h can be used.
void CGUITreeView::setIconFont( IGUIFont* font )
{
	s32	height;

	if ( font )
		font->grab();
	if ( IconFont )
	{
		IconFont->drop();
	}

	IconFont = font;
	if( IconFont )
	{
		height = IconFont->getDimension( L" " ).Height;
		if( height > ItemHeight )
		{
			ItemHeight = height;
		}
	}
}

//! Sets the image list which should be used for the image and selected image of every node.
//! The default is 0 (no images).
void CGUITreeView::setImageList( IGUIImageList* imageList )
{
	if (imageList )
		imageList->grab();
	if( ImageList )
	{
		ImageList->drop();
	}

	ImageList = imageList;
	if( ImageList )
	{
		if( ImageList->getImageSize().Height + 1 > ItemHeight )
		{
			ItemHeight = ImageList->getImageSize().Height + 1;
		}
	}
}

//! Access the vertical scrollbar
IGUIScrollBar* CGUITreeView::getVerticalScrollBar() const
{
	return ScrollBarV;
}

//! Access the horizontal scrollbar
IGUIScrollBar* CGUITreeView::getHorizontalScrollBar() const
{
	return ScrollBarH;
}

} // end namespace gui
} // end namespace irr


#endif // _IRR_COMPILE_WITH_GUI_
