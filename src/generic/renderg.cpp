///////////////////////////////////////////////////////////////////////////////
// Name:        generic/renderg.cpp
// Purpose:     generic implementation of wxRendererNative (for any platform)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.07.2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Vadim Zeitlin <vadim@wxwindows.org>
// License:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/gdicmn.h"
#include "wx/dc.h"

#include "wx/settings.h"
#include "wx/splitter.h"
#include "wx/dcmirror.h"
#include "wx/module.h"
#include "wx/renderer.h"

// ----------------------------------------------------------------------------
// wxRendererGeneric: our wxRendererNative implementation
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRendererGeneric : public wxRendererNative
{
public:
    wxRendererGeneric();

    virtual void DrawHeaderButton(wxWindow *win,
                                  wxDC& dc,
                                  const wxRect& rect,
                                  int flags = 0);

    virtual void DrawTreeItemButton(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);

    virtual void DrawSplitterBorder(wxWindow *win,
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int flags = 0);

    virtual void DrawSplitterSash(wxWindow *win,
                                  wxDC& dc,
                                  const wxSize& size,
                                  wxCoord position,
                                  wxOrientation orient,
                                  int flags = 0);


    virtual wxSplitterRenderParams GetSplitterParams(const wxWindow *win);

    virtual wxRendererVersion GetVersion() const
    {
        return wxRendererVersion(wxRendererVersion::Current_Version,
                                 wxRendererVersion::Current_Age);
    }


    // Cleanup by deleting standard renderer
    static void Cleanup();

    // Get the generic object
    static wxRendererGeneric* DoGetGeneric();

protected:
    // draw the rectange using the first pen for the left and top sides and
    // the second one for the bottom and right ones
    void DrawShadedRect(wxDC& dc, wxRect *rect,
                        const wxPen& pen1, const wxPen& pen2);

    // the standard pens
    wxPen m_penBlack,
          m_penDarkGrey,
          m_penLightGrey,
          m_penHighlight;

    static wxRendererGeneric* sm_rendererGeneric;
};

// ============================================================================
// wxRendererGeneric implementation
// ============================================================================

// Get the generic object
wxRendererGeneric* wxRendererGeneric::DoGetGeneric()
{
    if (!sm_rendererGeneric)
        sm_rendererGeneric = new wxRendererGeneric;
    return sm_rendererGeneric;
}

// ----------------------------------------------------------------------------
// wxRendererGeneric creation
// ----------------------------------------------------------------------------

/* static */
wxRendererNative& wxRendererNative::GetGeneric()
{
    return * wxRendererGeneric::DoGetGeneric();
}

void wxRendererGeneric::Cleanup()
{
    if (sm_rendererGeneric)
        delete sm_rendererGeneric;
    
    sm_rendererGeneric = NULL;
}

wxRendererGeneric* wxRendererGeneric::sm_rendererGeneric = NULL;

wxRendererGeneric::wxRendererGeneric()
    : m_penBlack(wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW)),
      m_penDarkGrey(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)),
      m_penLightGrey(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)),
      m_penHighlight(wxSystemSettings::GetColour(wxSYS_COLOUR_3DHIGHLIGHT))
{
}

// ----------------------------------------------------------------------------
// wxRendererGeneric helpers
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawShadedRect(wxDC& dc,
                                  wxRect *rect,
                                  const wxPen& pen1,
                                  const wxPen& pen2)
{
    // draw the rectangle
    dc.SetPen(pen1);
    dc.DrawLine(rect->GetLeft(), rect->GetTop(),
                rect->GetLeft(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft() + 1, rect->GetTop(),
                rect->GetRight(), rect->GetTop());
    dc.SetPen(pen2);
    dc.DrawLine(rect->GetRight(), rect->GetTop(),
                rect->GetRight(), rect->GetBottom());
    dc.DrawLine(rect->GetLeft(), rect->GetBottom(),
                rect->GetRight() + 1, rect->GetBottom());

    // adjust the rect
    rect->Inflate(-1);
}

// ----------------------------------------------------------------------------
// tree/list ctrl drawing
// ----------------------------------------------------------------------------

void
wxRendererGeneric::DrawHeaderButton(wxWindow * WXUNUSED(win),
                                    wxDC& dc,
                                    const wxRect& rect,
                                    int WXUNUSED(flags))
{
    const int CORNER = 1;

    const wxCoord x = rect.x,
                  y = rect.y,
                  w = rect.width,
                  h = rect.height;

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    dc.SetPen(m_penBlack);
    dc.DrawLine( x+w-CORNER+1, y, x+w, y+h );  // right (outer)
    dc.DrawRectangle( x, y+h, w+1, 1 );        // bottom (outer)

    dc.SetPen(m_penDarkGrey);
    dc.DrawLine( x+w-CORNER, y, x+w-1, y+h );  // right (inner)
    dc.DrawRectangle( x+1, y+h-1, w-2, 1 );    // bottom (inner)

    dc.SetPen(m_penHighlight);
    dc.DrawRectangle( x, y, w-CORNER+1, 1 );   // top (outer)
    dc.DrawRectangle( x, y, 1, h );            // left (outer)
    dc.DrawLine( x, y+h-1, x+1, y+h-1 );
    dc.DrawLine( x+w-1, y, x+w-1, y+1 );
}

// draw the plus or minus sign
void
wxRendererGeneric::DrawTreeItemButton(wxWindow * WXUNUSED(win),
                                      wxDC& dc,
                                      const wxRect& rect,
                                      int flags)
{
    // white background
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.DrawRectangle(rect.Deflate(1, 2));

    // black lines
    const wxCoord xMiddle = rect.x + rect.width/2;
    const wxCoord yMiddle = rect.y + rect.height/2;

    dc.SetPen(*wxBLACK_PEN);
    dc.DrawLine(xMiddle - 2, yMiddle, xMiddle + 3, yMiddle);
    if ( !(flags & wxCONTROL_EXPANDED) )
    {
        // turn "-" into "+"
        dc.DrawLine(xMiddle, yMiddle - 2, xMiddle, yMiddle + 3);
    }
}

// ----------------------------------------------------------------------------
// sash drawing
// ----------------------------------------------------------------------------

wxSplitterRenderParams
wxRendererGeneric::GetSplitterParams(const wxWindow *win)
{
    // see below
    wxCoord sashWidth,
            border;

    if ( win->HasFlag(wxSP_3D) )
    {
        sashWidth = 7;
        border = 2;
    }
    else // no 3D effect
    {
        sashWidth = 3;
        border = 0;
    }

    return wxSplitterRenderParams(sashWidth, border, false);
}

void
wxRendererGeneric::DrawSplitterBorder(wxWindow *win,
                                      wxDC& dc,
                                      const wxRect& rectOrig,
                                      int WXUNUSED(falgs))
{
    if ( win->HasFlag(wxSP_3D) )
    {
        wxRect rect = rectOrig;
        DrawShadedRect(dc, &rect, m_penDarkGrey, m_penHighlight);
        DrawShadedRect(dc, &rect, m_penBlack, m_penLightGrey);
    }
}

void
wxRendererGeneric::DrawSplitterSash(wxWindow *win,
                                    wxDC& dcReal,
                                    const wxSize& sizeReal,
                                    wxCoord position,
                                    wxOrientation orient,
                                    int WXUNUSED(flags))
{
    // to avoid duplicating the same code for horizontal and vertical sashes,
    // simply mirror the DC instead if needed (i.e. if horz splitter)
    wxMirrorDC dc(dcReal, orient != wxVERTICAL);
    wxSize size = dc.Reflect(sizeReal);


    // we draw a Win32-like grey sash with possible 3D border here:
    //
    //   ---- this is position
    //  /
    // v
    // dWGGGDd
    // GWGGGDB
    // GWGGGDB  where G is light grey (face)
    // GWGGGDB        W    white      (light)
    // GWGGGDB        D    dark grey  (shadow)
    // GWGGGDB        B    black      (dark shadow)
    // GWGGGDB
    // GWGGGDB  and lower letters are our border (already drawn)
    // GWGGGDB
    // wWGGGDd
    //
    // only the middle 3 columns are drawn unless wxSP_3D is specified

    const wxCoord h = size.y;
    wxCoord offset = 0;
    
    // If we're not drawing the border, droppings will
    // be left unless we make the sash shorter
    if ( !win->HasFlag(wxSP_3DBORDER) )
    {
        offset = 3;
    }

    // from left to right
    if ( win->HasFlag(wxSP_3D) )
    {
        dc.SetPen(m_penLightGrey);
        dc.DrawLine(position, 1 + offset, position, h - 1 - offset);

        dc.SetPen(m_penHighlight);
        dc.DrawLine(position + 1, offset, position + 1, h - offset);
    }

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetBrush(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    dc.DrawRectangle(position + 2, offset, 3, h - 2*offset);

    if ( win->HasFlag(wxSP_3D) )
    {
        dc.SetPen(m_penDarkGrey);
        dc.DrawLine(position + 5, offset, position + 5, h - offset);

        dc.SetPen(m_penBlack);
        dc.DrawLine(position + 6, offset, position + 6, h - 1 - offset);
    }
}

// A module to allow cleanup of generic renderer.
class wxGenericRendererModule: public wxModule
{
DECLARE_DYNAMIC_CLASS(wxGenericRendererModule)
public:
    wxGenericRendererModule() {}
    bool OnInit() { return true; };
    void OnExit() { wxRendererGeneric::Cleanup(); };
};

IMPLEMENT_DYNAMIC_CLASS(wxGenericRendererModule, wxModule)

