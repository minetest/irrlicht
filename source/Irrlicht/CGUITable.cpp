// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// 07.10.2005 - Multicolor-Listbox added by A. Buschhueter (Acki)
//                                          A_Buschhueter@gmx.de

#include "CGUITable.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUISkin.h"
#include "IGUIEnvironment.h"
#include "IVideoDriver.h"
#include "IGUIFont.h"
#include "CGUIScrollBar.h"
#include "os.h"

#define ARROW_PAD 15

namespace irr
{
namespace gui
{

//! constructor
CGUITable::CGUITable(IGUIEnvironment* environment, IGUIElement* parent,
						s32 id, const core::rect<s32>& rectangle, bool clip,
						bool drawBack, bool moveOverSelect)
: IGUITable(environment, parent, id, rectangle),
	VerticalScrollBar(0), HorizontalScrollBar(0),
	Clip(clip), DrawBack(drawBack), MoveOverSelect(moveOverSelect),
	Selecting(false), CurrentResizedColumn(-1), ResizeStart(0), ResizableColumns(true),
	ItemHeight(0), TotalItemHeight(0), TotalItemWidth(0), Selected(-1),
	CellHeightPadding(2), CellWidthPadding(5), ActiveTab(-1),
	CurrentOrdering(EGOM_NONE), DrawFlags(EGTDF_ROWS | EGTDF_COLUMNS | EGTDF_ACTIVE_ROW ),
	ScrollBarSize(0),
	OverrideFont(0)
{
	#ifdef _DEBUG
	setDebugName("CGUITable");
	#endif

	VerticalScrollBar = Environment->addScrollBar(false, core::rect<s32>(0, 0, 100, 100), this, -1);
	if (VerticalScrollBar)
	{
		VerticalScrollBar->grab();
		VerticalScrollBar->setNotClipped(false);
		VerticalScrollBar->setSubElement(true);
	}

	HorizontalScrollBar = Environment->addScrollBar(true, core::rect<s32>(0, 0, 100, 100), this, -1);
	if ( HorizontalScrollBar )
	{
		HorizontalScrollBar->grab();
		HorizontalScrollBar->setNotClipped(false);
		HorizontalScrollBar->setSubElement(true);
	}

	refreshControls();
}


//! destructor
CGUITable::~CGUITable()
{
	if (VerticalScrollBar)
		VerticalScrollBar->drop();
	if ( HorizontalScrollBar )
		HorizontalScrollBar->drop();

	if (OverrideFont)
		OverrideFont->drop();
}


void CGUITable::addColumn(const wchar_t* caption, s32 columnIndex)
{
	Column tabHeader;
	tabHeader.Name = caption;
	tabHeader.Width = getActiveFont()->getDimension(caption).Width + (CellWidthPadding * 2) + ARROW_PAD;
	tabHeader.OrderingMode = EGCO_NONE;

	if ( columnIndex < 0 || columnIndex >= (s32)Columns.size() )
	{
		Columns.push_back(tabHeader);
		for ( u32 i=0; i < Rows.size(); ++i )
		{
			Cell cell;
			Rows[i].Items.push_back(cell);
		}
	}
	else
	{
		Columns.insert(tabHeader, columnIndex);
		for ( u32 i=0; i < Rows.size(); ++i )
		{
			Cell cell;
			Rows[i].Items.insert(cell, columnIndex);
		}
	}

	if (ActiveTab == -1 && Columns.size() == 1)	// first column added - make it active automatically
		ActiveTab = 0;

	recalculateWidths();
}


//! remove a column from the table
void CGUITable::removeColumn(u32 columnIndex)
{
	if ( columnIndex < Columns.size() )
	{
		Columns.erase(columnIndex);
		for ( u32 i=0; i < Rows.size(); ++i )
		{
			Rows[i].Items.erase(columnIndex);
		}
	}
	if ( (s32)columnIndex <= ActiveTab )
		ActiveTab = Columns.size() ? 0 : -1;

	recalculateWidths();
}


s32 CGUITable::getColumnCount() const
{
	return Columns.size();
}


s32 CGUITable::getRowCount() const
{
	return Rows.size();
}


bool CGUITable::setActiveColumn(s32 idx, bool doOrder )
{
	if ( idx >= (s32)Columns.size() )
		idx = -1;

	bool changed = (ActiveTab != idx);
	ActiveTab = idx;
	if ( ActiveTab < 0 )
		return false;

	if ( doOrder )
	{
		switch ( Columns[idx].OrderingMode )
		{
			case EGCO_NONE:
				CurrentOrdering = EGOM_NONE;
				break;

			case EGCO_CUSTOM:
				CurrentOrdering = EGOM_NONE;
				if (Parent)
				{
					SEvent event;
					event.EventType = EET_GUI_EVENT;
					event.GUIEvent.Caller = this;
					event.GUIEvent.Element = 0;
					event.GUIEvent.EventType = EGET_TABLE_HEADER_CHANGED;
					Parent->OnEvent(event);
				}

				break;

			case EGCO_ASCENDING:
				CurrentOrdering = EGOM_ASCENDING;
				break;

			case EGCO_DESCENDING:
				CurrentOrdering = EGOM_DESCENDING;
				break;

			case EGCO_FLIP_ASCENDING_DESCENDING:
				CurrentOrdering = EGOM_ASCENDING == CurrentOrdering ? EGOM_DESCENDING : EGOM_ASCENDING;
				break;
			default:
				CurrentOrdering = EGOM_NONE;
		}

		orderRows(getActiveColumn(), CurrentOrdering);
	}

	if (changed)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.Element = 0;
		event.GUIEvent.EventType = EGET_TABLE_HEADER_CHANGED;
		Parent->OnEvent(event);
	}

	return true;
}


s32 CGUITable::getActiveColumn() const
{
	return ActiveTab;
}


EGUI_ORDERING_MODE CGUITable::getActiveColumnOrdering() const
{
	return CurrentOrdering;
}


void CGUITable::setColumnWidth(u32 columnIndex, u32 width)
{
	if ( columnIndex < Columns.size() )
	{
		const u32 MIN_WIDTH = getActiveFont()->getDimension(Columns[columnIndex].Name.c_str() ).Width + (CellWidthPadding * 2);
		if ( width < MIN_WIDTH )
			width = MIN_WIDTH;

		Columns[columnIndex].Width = width;

		for ( u32 i=0; i < Rows.size(); ++i )
		{
			breakText( Rows[i].Items[columnIndex].Text, Rows[i].Items[columnIndex].BrokenText, Columns[columnIndex].Width );
		}
	}
	recalculateWidths();
}

//! Get the width of a column
u32 CGUITable::getColumnWidth(u32 columnIndex) const
{
	if ( columnIndex >= Columns.size() )
		return 0;

	return Columns[columnIndex].Width;
}

void CGUITable::setResizableColumns(bool resizable)
{
	ResizableColumns = resizable;
}


bool CGUITable::hasResizableColumns() const
{
	return ResizableColumns;
}


u32 CGUITable::addRow(u32 rowIndex)
{
	if ( rowIndex > Rows.size() )
	{
		rowIndex = Rows.size();
	}

	Row row;

	if ( rowIndex == Rows.size() )
		Rows.push_back(row);
	else
		Rows.insert(row, rowIndex);

	Rows[rowIndex].Items.reallocate(Columns.size());
	for ( u32 i = 0 ; i < Columns.size() ; ++i )
	{
		Rows[rowIndex].Items.push_back(Cell());
	}

	recalculateHeights();
	return rowIndex;
}


void CGUITable::removeRow(u32 rowIndex)
{
	if ( rowIndex > Rows.size() )
		return;

	Rows.erase( rowIndex );

	if ( !(Selected < s32(Rows.size())) )
		Selected = Rows.size() - 1;

	recalculateHeights();
}


//! adds an list item, returns id of item
void CGUITable::setCellText(u32 rowIndex, u32 columnIndex, const core::stringw& text)
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].Text = text;
		breakText( Rows[rowIndex].Items[columnIndex].Text, Rows[rowIndex].Items[columnIndex].BrokenText, Columns[columnIndex].Width );

		IGUISkin* skin = Environment->getSkin();
		if ( skin )
			Rows[rowIndex].Items[columnIndex].Color = skin->getColor(EGDC_BUTTON_TEXT);
	}
}

void CGUITable::setCellText(u32 rowIndex, u32 columnIndex, const core::stringw& text, video::SColor color)
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].Text = text;
		breakText( Rows[rowIndex].Items[columnIndex].Text, Rows[rowIndex].Items[columnIndex].BrokenText, Columns[columnIndex].Width );
		Rows[rowIndex].Items[columnIndex].Color = color;
		Rows[rowIndex].Items[columnIndex].IsOverrideColor = true;
	}
}


void CGUITable::setCellColor(u32 rowIndex, u32 columnIndex, video::SColor color)
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].Color = color;
		Rows[rowIndex].Items[columnIndex].IsOverrideColor = true;
	}
}


void CGUITable::setCellData(u32 rowIndex, u32 columnIndex, void *data)
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		Rows[rowIndex].Items[columnIndex].Data = data;
	}
}


const wchar_t* CGUITable::getCellText(u32 rowIndex, u32 columnIndex ) const
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		return Rows[rowIndex].Items[columnIndex].Text.c_str();
	}

	return 0;
}


void* CGUITable::getCellData(u32 rowIndex, u32 columnIndex ) const
{
	if ( rowIndex < Rows.size() && columnIndex < Columns.size() )
	{
		return Rows[rowIndex].Items[columnIndex].Data;
	}

	return 0;
}


//! clears the list
void CGUITable::clear()
{
    Selected = -1;
	Rows.clear();
	Columns.clear();

	if (VerticalScrollBar)
		VerticalScrollBar->setPos(0);
	if ( HorizontalScrollBar )
		HorizontalScrollBar->setPos(0);

	recalculateHeights();
	recalculateWidths();
}


void CGUITable::clearRows()
{
    Selected = -1;
	Rows.clear();

	if (VerticalScrollBar)
		VerticalScrollBar->setPos(0);

	recalculateHeights();
}


/*!
*/
s32 CGUITable::getSelected() const
{
	return Selected;
}

//! set which row is currently selected
void CGUITable::setSelected( s32 index )
{
	Selected = -1;
	if ( index >= 0 && index < (s32) Rows.size() )
		Selected = index;
}


void CGUITable::recalculateWidths()
{
	TotalItemWidth=0;
	for ( u32 i=0; i < Columns.size(); ++i )
	{
		TotalItemWidth += Columns[i].Width;
	}
	checkScrollbars();
}


void CGUITable::recalculateHeights()
{
	IGUIFont* activeFont = getActiveFont();
	if(activeFont)
	{
		ItemHeight = activeFont->getDimension(L"A").Height + (CellHeightPadding * 2);
		TotalItemHeight = ItemHeight * Rows.size();		//  header is not counted, because we only want items
	}
	else
	{
		ItemHeight = 0;
		TotalItemHeight = 0;
	}
	checkScrollbars();
}


// automatic enabled/disabling and resizing of scrollbars
void CGUITable::checkScrollbars()
{
	IGUISkin* skin = Environment->getSkin();
	if ( !HorizontalScrollBar || !VerticalScrollBar || !skin)
		return;

	ScrollBarSize = skin->getSize(EGDS_SCROLLBAR_SIZE);

	bool wasHorizontalScrollBarVisible = HorizontalScrollBar->isVisible();
	bool wasVerticalScrollBarVisible = VerticalScrollBar->isVisible();
	HorizontalScrollBar->setVisible(false);
	VerticalScrollBar->setVisible(false);

	// CAREFUL: near identical calculations for tableRect and clientClip are also done in draw
	// area of table used for drawing without scrollbars
	core::rect<s32> tableRect(AbsoluteRect);
	tableRect.UpperLeftCorner.X += 1;
	tableRect.UpperLeftCorner.Y += 1;
	s32 headerBottom = tableRect.UpperLeftCorner.Y + ItemHeight;

	// area of for the items (without header and without scrollbars)
	core::rect<s32> clientClip(tableRect);
	clientClip.UpperLeftCorner.Y = headerBottom + 1;

	// needs horizontal scroll be visible?
	if( TotalItemWidth > clientClip.getWidth() )
	{
		clientClip.LowerRightCorner.Y -= ScrollBarSize;
		HorizontalScrollBar->setVisible(true);
		HorizontalScrollBar->setMax(core::max_(0,TotalItemWidth - clientClip.getWidth()));
	}

	// needs vertical scroll be visible?
	if( TotalItemHeight > clientClip.getHeight() )
	{
		clientClip.LowerRightCorner.X -= ScrollBarSize;
		VerticalScrollBar->setVisible(true);
		VerticalScrollBar->setMax(core::max_(0,TotalItemHeight - clientClip.getHeight()));

		// check horizontal again because we have now smaller clientClip
		if ( !HorizontalScrollBar->isVisible() )
		{
			if( TotalItemWidth > clientClip.getWidth() )
			{
				clientClip.LowerRightCorner.Y -= ScrollBarSize;
				HorizontalScrollBar->setVisible(true);
				HorizontalScrollBar->setMax(core::max_(0,TotalItemWidth - clientClip.getWidth()));
			}
		}
	}

	// find the correct size for the vertical scrollbar
	if ( VerticalScrollBar->isVisible() )
	{
		if  (!wasVerticalScrollBarVisible )
			VerticalScrollBar->setPos(0);

		if ( HorizontalScrollBar->isVisible() )
		{
			VerticalScrollBar->setRelativePosition(
				core::rect<s32>(RelativeRect.getWidth() - ScrollBarSize, 1,
				RelativeRect.getWidth()-1, RelativeRect.getHeight()-(1+ScrollBarSize) ) );
		}
		else
		{
			VerticalScrollBar->setRelativePosition(
				core::rect<s32>(RelativeRect.getWidth() - ScrollBarSize, 1,
				RelativeRect.getWidth()-1, RelativeRect.getHeight()-1) );
		}
	}

	// find the correct size for the horizontal scrollbar
	if ( HorizontalScrollBar->isVisible() )
	{
		if ( !wasHorizontalScrollBarVisible )
			HorizontalScrollBar->setPos(0);

		if ( VerticalScrollBar->isVisible() )
		{
			HorizontalScrollBar->setRelativePosition( core::rect<s32>(1, RelativeRect.getHeight() - ScrollBarSize, RelativeRect.getWidth()-(1+ScrollBarSize), RelativeRect.getHeight()-1) );
		}
		else
		{
			HorizontalScrollBar->setRelativePosition( core::rect<s32>(1, RelativeRect.getHeight() - ScrollBarSize, RelativeRect.getWidth()-1, RelativeRect.getHeight()-1) );
		}
	}
}


void CGUITable::refreshControls()
{
	updateAbsolutePosition();

	if ( VerticalScrollBar )
		VerticalScrollBar->setVisible(false);

	if ( HorizontalScrollBar )
		HorizontalScrollBar->setVisible(false);

	recalculateHeights();
	recalculateWidths();
}


//! called if an event happened.
bool CGUITable::OnEvent(const SEvent &event)
{
	if (isEnabled())
	{

		switch(event.EventType)
		{
		case EET_GUI_EVENT:
			switch(event.GUIEvent.EventType)
			{
			case gui::EGET_SCROLL_BAR_CHANGED:
				if (event.GUIEvent.Caller == VerticalScrollBar)
				{
					// current position will get read out in draw
					return true;
				}
				if (event.GUIEvent.Caller == HorizontalScrollBar)
				{
					// current position will get read out in draw
					return true;
				}
				break;
			case gui::EGET_ELEMENT_FOCUS_LOST:
				{
					CurrentResizedColumn = -1;
					Selecting = false;
				}
				break;
			default:
				break;
			}
			break;
		case EET_MOUSE_INPUT_EVENT:
			{
				if ( !isEnabled() )
					return false;

				core::position2d<s32> p(event.MouseInput.X, event.MouseInput.Y);

				switch(event.MouseInput.Event)
				{
				case EMIE_MOUSE_WHEEL:
					VerticalScrollBar->setPos(VerticalScrollBar->getPos() + (event.MouseInput.Wheel < 0 ? -1 : 1)*-10);
					return true;

				case EMIE_LMOUSE_PRESSED_DOWN:

					if (VerticalScrollBar->isVisible() &&
						VerticalScrollBar->getAbsolutePosition().isPointInside(p) &&
						VerticalScrollBar->OnEvent(event))
						return true;

					if (HorizontalScrollBar->isVisible() &&
						HorizontalScrollBar->getAbsolutePosition().isPointInside(p) &&
						HorizontalScrollBar->OnEvent(event))
						return true;

					if ( dragColumnStart( event.MouseInput.X, event.MouseInput.Y ) )
					{
						return true;
					}

					if ( selectColumnHeader( event.MouseInput.X, event.MouseInput.Y ) )
						return true;

					Selecting = true;
					return true;

				case EMIE_LMOUSE_LEFT_UP:

					CurrentResizedColumn = -1;
					Selecting = false;

					if (VerticalScrollBar->isVisible() &&
						VerticalScrollBar->getAbsolutePosition().isPointInside(p) &&
						VerticalScrollBar->OnEvent(event))
					{
						return true;
					}

					if (HorizontalScrollBar->isVisible() &&
						HorizontalScrollBar->getAbsolutePosition().isPointInside(p) &&
						HorizontalScrollBar->OnEvent(event))
					{
						return true;
					}

					selectNew(event.MouseInput.Y);
					return true;

				case EMIE_MOUSE_MOVED:
					if ( CurrentResizedColumn >= 0 )
					{
						if ( dragColumnUpdate(event.MouseInput.X) )
						{
							return true;
						}
					}
					if (Selecting || MoveOverSelect)
					{
						if (getAbsolutePosition().isPointInside(p))
						{
							selectNew(event.MouseInput.Y);
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

	return IGUIElement::OnEvent(event);
}


void CGUITable::setColumnOrdering(u32 columnIndex, EGUI_COLUMN_ORDERING mode)
{
	if ( columnIndex < Columns.size() )
		Columns[columnIndex].OrderingMode = mode;
}


void CGUITable::swapRows(u32 rowIndexA, u32 rowIndexB)
{
	if ( rowIndexA >= Rows.size() )
		return;

	if ( rowIndexB >= Rows.size() )
		return;

	Row swap = Rows[rowIndexA];
	Rows[rowIndexA] = Rows[rowIndexB];
	Rows[rowIndexB] = swap;

	if ( Selected == s32(rowIndexA) )
		Selected = rowIndexB;
	else if( Selected == s32(rowIndexB) )
		Selected = rowIndexA;

}


bool CGUITable::dragColumnStart(s32 xpos, s32 ypos)
{
	if ( !ResizableColumns )
		return false;

	if ( ypos > ( AbsoluteRect.UpperLeftCorner.Y + ItemHeight ) )
		return false;

	const s32 CLICK_AREA = 12;	// to left and right of line which can be dragged
	s32 pos = AbsoluteRect.UpperLeftCorner.X+1;

	if ( HorizontalScrollBar && HorizontalScrollBar->isVisible() )
		pos -= HorizontalScrollBar->getPos();

	pos += TotalItemWidth;

	// have to search from the right as otherwise lines could no longer be resized when a column width is 0
	for ( s32 i = (s32)Columns.size()-1; i >= 0 ; --i )
	{
		u32 colWidth = Columns[i].Width;

		if ( xpos >= (pos - CLICK_AREA) && xpos < ( pos + CLICK_AREA ) )
		{
			CurrentResizedColumn = i;
			ResizeStart = xpos;
			return true;
		}

		pos -= colWidth;
	}

	return false;
}


bool CGUITable::dragColumnUpdate(s32 xpos)
{
	if ( !ResizableColumns || CurrentResizedColumn < 0 || CurrentResizedColumn >= s32(Columns.size()) )
	{
		CurrentResizedColumn = -1;
		return false;
	}

	s32 width = s32(Columns[CurrentResizedColumn].Width) + (xpos-ResizeStart);
	if ( width < 0 )
		width = 0;
	setColumnWidth(CurrentResizedColumn, u32(width));
	ResizeStart = xpos;

	return false;
}


bool CGUITable::selectColumnHeader(s32 xpos, s32 ypos)
{
	if ( ypos > ( AbsoluteRect.UpperLeftCorner.Y + ItemHeight ) )
		return false;

	s32 pos = AbsoluteRect.UpperLeftCorner.X+1;

	if ( HorizontalScrollBar && HorizontalScrollBar->isVisible() )
		pos -= HorizontalScrollBar->getPos();

	for ( u32 i = 0 ; i < Columns.size() ; ++i )
	{
		u32 colWidth = Columns[i].Width;

		if ( xpos >= pos && xpos < ( pos + s32(colWidth) ) )
		{
			setActiveColumn( i, true );

			return true;
		}

		pos += colWidth;
	}

	return false;
}


void CGUITable::orderRows(s32 columnIndex, EGUI_ORDERING_MODE mode)
{
	Row swap;

	if ( columnIndex == -1 )
		columnIndex = getActiveColumn();
	if ( columnIndex < 0 )
		return;

	if ( mode == EGOM_ASCENDING )
	{
		for ( s32 i = 0 ; i < s32(Rows.size()) - 1 ; ++i )
		{
			for ( s32 j = 0 ; j < s32(Rows.size()) - i - 1 ; ++j )
			{
				if ( Rows[j+1].Items[columnIndex].Text < Rows[j].Items[columnIndex].Text )
				{
					swap = Rows[j];
					Rows[j] = Rows[j+1];
					Rows[j+1] = swap;

					if ( Selected == j )
						Selected = j+1;
					else if( Selected == j+1 )
						Selected = j;
				}
			}
		}
	}
	else if ( mode == EGOM_DESCENDING )
	{
		for ( s32 i = 0 ; i < s32(Rows.size()) - 1 ; ++i )
		{
			for ( s32 j = 0 ; j < s32(Rows.size()) - i - 1 ; ++j )
			{
				if ( Rows[j].Items[columnIndex].Text < Rows[j+1].Items[columnIndex].Text)
				{
					swap = Rows[j];
					Rows[j] = Rows[j+1];
					Rows[j+1] = swap;

					if ( Selected == j )
						Selected = j+1;
					else if( Selected == j+1 )
						Selected = j;
				}
			}
		}
	}
}


void CGUITable::selectNew(s32 ypos, bool onlyHover)
{
	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	s32 oldSelected = Selected;

	if ( ypos < ( AbsoluteRect.UpperLeftCorner.Y + ItemHeight ) )
		return;

	// find new selected item.
	if (ItemHeight!=0)
		Selected = ((ypos - AbsoluteRect.UpperLeftCorner.Y - ItemHeight - 1) + VerticalScrollBar->getPos()) / ItemHeight;

	if (Selected >= (s32)Rows.size())
		Selected = Rows.size() - 1;
	else if (Selected<0)
		Selected = 0;

	// post the news
	if (Parent && !onlyHover)
	{
		SEvent event;
		event.EventType = EET_GUI_EVENT;
		event.GUIEvent.Caller = this;
		event.GUIEvent.Element = 0;
		event.GUIEvent.EventType = (Selected != oldSelected) ? EGET_TABLE_CHANGED : EGET_TABLE_SELECTED_AGAIN;
		Parent->OnEvent(event);
	}
}


//! draws the element and its children
void CGUITable::draw()
{
	if (!IsVisible)
		return;

	irr::video::IVideoDriver* driver = Environment->getVideoDriver();

	IGUISkin* skin = Environment->getSkin();
	if (!skin)
		return;

	IGUIFont* font = getActiveFont();
	if (!font)
		return;

	if ( ScrollBarSize != skin->getSize(EGDS_SCROLLBAR_SIZE) )
		checkScrollbars();

	// CAREFUL: near identical calculations for tableRect and clientClip are also done in checkScrollbars and selectColumnHeader
	// Area of table used for drawing without scrollbars
	core::rect<s32> tableRect(AbsoluteRect);
	tableRect.UpperLeftCorner.X += 1;
	tableRect.UpperLeftCorner.Y += 1;
	if ( VerticalScrollBar && VerticalScrollBar->isVisible() )
		tableRect.LowerRightCorner.X -= ScrollBarSize;
	if ( HorizontalScrollBar && HorizontalScrollBar->isVisible() )
		tableRect.LowerRightCorner.Y -= ScrollBarSize;

	s32 headerBottom = tableRect.UpperLeftCorner.Y + ItemHeight;

	// area of for the items (without header and without scrollbars)
	core::rect<s32> clientClip(tableRect);
	clientClip.UpperLeftCorner.Y = headerBottom + 1;
	clientClip.clipAgainst(AbsoluteClippingRect);

	// draw background for whole element
	skin->draw3DSunkenPane(this, skin->getColor(EGDC_3D_HIGH_LIGHT), true, DrawBack, AbsoluteRect, &AbsoluteClippingRect);

	// scrolledTableClient is the area where the table items would be if it could be drawn completely
	core::rect<s32> scrolledTableClient(tableRect);
	scrolledTableClient.UpperLeftCorner.Y = headerBottom + 1;
	scrolledTableClient.LowerRightCorner.Y = scrolledTableClient.UpperLeftCorner.Y + TotalItemHeight;
	scrolledTableClient.LowerRightCorner.X = scrolledTableClient.UpperLeftCorner.X + TotalItemWidth;
	if ( VerticalScrollBar && VerticalScrollBar->isVisible() )
	{
		scrolledTableClient.UpperLeftCorner.Y -= VerticalScrollBar->getPos();
		scrolledTableClient.LowerRightCorner.Y -= VerticalScrollBar->getPos();
	}
	if ( HorizontalScrollBar && HorizontalScrollBar->isVisible() )
	{
		scrolledTableClient.UpperLeftCorner.X -= HorizontalScrollBar->getPos();
		scrolledTableClient.LowerRightCorner.X -= HorizontalScrollBar->getPos();
	}

	// rowRect is around the scrolled row
	core::rect<s32> rowRect(scrolledTableClient);
	rowRect.LowerRightCorner.Y = rowRect.UpperLeftCorner.Y + ItemHeight;

	u32 pos;
	for ( u32 i = 0 ; i < Rows.size() ; ++i )
	{
		if (rowRect.LowerRightCorner.Y >= AbsoluteRect.UpperLeftCorner.Y &&
			rowRect.UpperLeftCorner.Y <= AbsoluteRect.LowerRightCorner.Y)
		{
			// draw row separator
			if ( DrawFlags & EGTDF_ROWS )
			{
				core::rect<s32> lineRect(rowRect);
				lineRect.UpperLeftCorner.Y = lineRect.LowerRightCorner.Y - 1;
				driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), lineRect, &clientClip);
			}

			core::rect<s32> textRect(rowRect);
			pos = rowRect.UpperLeftCorner.X;

			// draw selected row background highlighted
			if ((s32)i == Selected && DrawFlags & EGTDF_ACTIVE_ROW )
				driver->draw2DRectangle(skin->getColor(EGDC_HIGH_LIGHT), rowRect, &clientClip);

			for ( u32 j = 0 ; j < Columns.size() ; ++j )
			{
				textRect.UpperLeftCorner.X = pos + CellWidthPadding;
				textRect.LowerRightCorner.X = pos + Columns[j].Width - CellWidthPadding;

				// draw item text
				if ((s32)i == Selected)
				{
					font->draw(Rows[i].Items[j].BrokenText.c_str(), textRect, skin->getColor(isEnabled() ? EGDC_HIGH_LIGHT_TEXT : EGDC_GRAY_TEXT), false, true, &clientClip);
				}
				else
				{
					if ( !Rows[i].Items[j].IsOverrideColor )	// skin-colors can change
						Rows[i].Items[j].Color = skin->getColor(EGDC_BUTTON_TEXT);
					font->draw(Rows[i].Items[j].BrokenText.c_str(), textRect, isEnabled() ? Rows[i].Items[j].Color : skin->getColor(EGDC_GRAY_TEXT), false, true, &clientClip);
				}

				pos += Columns[j].Width;
			}
		}

		rowRect.UpperLeftCorner.Y += ItemHeight;
		rowRect.LowerRightCorner.Y += ItemHeight;
	}

	core::rect<s32> columnSeparator(clientClip);
	pos = scrolledTableClient.UpperLeftCorner.X;

	core::rect<s32> tableClip(tableRect);
	tableClip.clipAgainst(AbsoluteClippingRect);

	for (u32 i = 0 ; i < Columns.size() ; ++i )
	{
		const wchar_t* text = Columns[i].Name.c_str();
		u32 colWidth = Columns[i].Width;

		core::rect<s32> columnrect(pos, tableRect.UpperLeftCorner.Y, pos + colWidth, headerBottom);

		// draw column background
		skin->draw3DButtonPaneStandard(this, columnrect, &tableClip);

		// draw column separator
		if ( DrawFlags & EGTDF_COLUMNS )
		{
			columnSeparator.UpperLeftCorner.X = pos;
			columnSeparator.LowerRightCorner.X = pos + 1;
			driver->draw2DRectangle(skin->getColor(EGDC_3D_SHADOW), columnSeparator, &tableClip);
		}

		// draw header column text
		columnrect.UpperLeftCorner.X += CellWidthPadding;
		font->draw(text, columnrect, skin->getColor( isEnabled() ? EGDC_BUTTON_TEXT : EGDC_GRAY_TEXT), false, true, &tableClip);

		// draw icon for active column tab
		if ( (s32)i == ActiveTab )
		{
			if ( CurrentOrdering == EGOM_ASCENDING )
			{
				columnrect.UpperLeftCorner.X = columnrect.LowerRightCorner.X - CellWidthPadding - ARROW_PAD / 2 + 2;
				columnrect.UpperLeftCorner.Y += 7;
				skin->drawIcon(this,EGDI_CURSOR_UP,columnrect.UpperLeftCorner,0,0,false,&tableClip);
			}
			else
			{
				columnrect.UpperLeftCorner.X = columnrect.LowerRightCorner.X - CellWidthPadding - ARROW_PAD / 2 + 2;
				columnrect.UpperLeftCorner.Y += 7;
				skin->drawIcon(this,EGDI_CURSOR_DOWN,columnrect.UpperLeftCorner,0,0,false,&tableClip);
			}
		}

		pos += colWidth;
	}

	// fill up header background up to the right side
	core::rect<s32> columnrect(pos, tableRect.UpperLeftCorner.Y, tableRect.LowerRightCorner.X , headerBottom);
	skin->draw3DButtonPaneStandard(this, columnrect, &tableClip);

	IGUIElement::draw();
}


void CGUITable::breakText(const core::stringw& text, core::stringw& brokenText, u32 cellWidth)
{
	IGUISkin* skin = Environment->getSkin();

	if (!skin)
		return;

	IGUIFont* font = getActiveFont();
	if (!font)
		return;

	core::stringw line, lineDots;
	wchar_t c[2];
	c[1] = L'\0';

	const u32 maxLength = cellWidth - (CellWidthPadding * 2);
	const u32 maxLengthDots = cellWidth - (CellWidthPadding * 2) - font->getDimension(L"...").Width;
	const u32 size = text.size();
	u32 pos = 0;

	u32 i;

	for (i=0; i<size; ++i)
	{
		c[0] = text[i];

		if (c[0] == L'\n')
			break;

		pos += font->getDimension(c).Width;
		if ( pos > maxLength )
			break;

		if ( font->getDimension( (line + c).c_str() ).Width > maxLengthDots )
			lineDots = line;

		line += c[0];
	}

	if ( i < size )
		brokenText = lineDots + L"...";
	else
		brokenText = line;
}


//! Set some flags influencing the layout of the table
void CGUITable::setDrawFlags(s32 flags)
{
	DrawFlags = flags;
}


//! Get the flags which influence the layout of the table
s32 CGUITable::getDrawFlags() const
{
	return DrawFlags;
}

//! Sets another skin independent font.
void CGUITable::setOverrideFont(IGUIFont* font)
{
	if (OverrideFont == font)
		return;

	if (OverrideFont)
		OverrideFont->drop();

	OverrideFont = font;

	if (OverrideFont)
		OverrideFont->grab();

	refreshControls();
}

//! Gets the override font (if any)
IGUIFont * CGUITable::getOverrideFont() const
{
	return OverrideFont;
}

//! Get the font which is used right now for drawing
IGUIFont* CGUITable::getActiveFont() const
{
	if ( OverrideFont )
		return OverrideFont;
	IGUISkin* skin = Environment->getSkin();
	if (skin)
		return skin->getFont();
	return 0;
}

//! Get the height of items/rows
s32 CGUITable::getItemHeight() const
{
	return ItemHeight;
}

//! Access the vertical scrollbar
IGUIScrollBar* CGUITable::getVerticalScrollBar() const
{
	return VerticalScrollBar;
}

//! Access the horizontal scrollbar
IGUIScrollBar* CGUITable::getHorizontalScrollBar() const
{
	return HorizontalScrollBar;
}

//! Sets whether to draw the background.
void CGUITable::setDrawBackground(bool draw)
{
	DrawBack = draw;
}

//! Checks if background drawing is enabled
/** \return true if background drawing is enabled, false otherwise */
bool CGUITable::isDrawBackgroundEnabled() const
{
	return DrawBack;
}

} // end namespace gui
} // end namespace irr

#endif

