/*
 *  Off-the-Record Messaging Proxy
 *  Copyright (C) 2004-2005  Nikita Borisov, Ian Goldberg, Katrina Hanna
 *                           <otr@cypherpunks.ca>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <wx/string.h>
#include "wxstatwrtext.h"

// Find the next extents of whitespace and non-whitespace starting at
// position pos of the given label.  Don't mix spaces and newlines in a
// single whitespace extent; set whiteisnl to 1 if the whitespace extent
// contains newlines.
void wxStaticWrappedText::NextPiece(wxString const &label, size_t pos,
	size_t &whitelen, int &whiteisnl, size_t &nonwhitelen)
{
    size_t labellen = label.Length();
    whitelen = 0;
    whiteisnl = -1;
    nonwhitelen = 0;

    // First find the next bit of homogeneous whitespace
    while (pos < labellen) {
	if (!label.compare(pos, 1, wxT(" ")) && whiteisnl != 1) {
	    whiteisnl = 0;
	    ++pos;
	    ++whitelen;
	    continue;
	}
	if (!label.compare(pos, 1, wxT("\n")) && whiteisnl != 0) {
	    whiteisnl = 1;
	    ++pos;
	    ++whitelen;
	    continue;
	}
	break;
    }

    // Now find the next bit of non-whitespace
    while (pos < labellen) {
	if (!label.compare(pos, 1, wxT(" ")) ||
		!label.compare(pos, 1, wxT("\n"))) {
	    break;
	}
	++pos;
	++nonwhitelen;
    }

    if (whiteisnl == -1) whiteisnl = 0;
}

void wxStaticWrappedText::WrapText(int width)
{
    int curwidth, curheight;

    // We weren't actually asked to wrap anything
    if (width == 0) return;

    // If the text already fits in the requested width, don't change
    // anything.
    GetSize(&curwidth, &curheight);
    if (curwidth <= width) return;

    wxString origlabel = GetLabel();
    wxString newlabel = wxEmptyString;
    size_t curpos = 0;
    size_t origlen = origlabel.Length();
    while(curpos < origlen) {
	size_t whitelen, nonwhitelen;
	int test1width, test1height;
	int test2width, test2height;
	int whiteisnl;

	// Get the next stretch of whitespace and non-whitespace
	NextPiece(origlabel, curpos, whitelen, whiteisnl, nonwhitelen);

	// First see if we can safely just use it as-is.
	wxString test1label = newlabel +
	    origlabel.Mid(curpos, whitelen + nonwhitelen);
	SetLabel(test1label);
	GetSize(&test1width, &test1height);

	// If so, great.  Also, if the whitespace is already newlines,
	// there's nothing more we'll be able to do
	if (test1width <= width || whiteisnl) {
	    newlabel = test1label;
	    curpos += whitelen + nonwhitelen;
	    if (test1width > width) {
		// We got a single word wider than the requested width,
		// so up the width.
		width = test1width;
	    }
	    continue;
	}

	// If not, see if it helps any to change the whitespace (if not
	// already newlines) to a single newline (or, if we're still at
	// the beginning of the string, nothing)
	wxString test2label = newlabel +
	    (newlabel.IsEmpty() ? wxT("") : wxT("\n")) +
	    origlabel.Mid(curpos + whitelen, nonwhitelen);
	SetLabel(test2label);
	GetSize(&test2width, &test2height);

	if (test2width < test1width) {
	    // It did help; go with that.
	    newlabel = test2label;
	    curpos += whitelen + nonwhitelen;
	    if (test2width > width) {
		// We got a single word wider than the requested width,
		// so up the width.
		width = test2width;
	    }
	    continue;
	}

	// Just stick with the original
	newlabel = test1label;
	curpos += whitelen + nonwhitelen;
	width = test1width;
    }
    SetLabel(newlabel);
}
