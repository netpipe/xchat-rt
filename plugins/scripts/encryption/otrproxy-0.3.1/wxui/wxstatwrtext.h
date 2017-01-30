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

#ifndef __WXSTATWRTEXT_H__
#define __WXSTATWRTEXT_H__

#include <wx/stattext.h>

class wxStaticWrappedText : public wxStaticText {
public:
    wxStaticWrappedText(wxWindow* parent, wxWindowID id,
	    const wxString& label, const wxPoint& pos = wxDefaultPosition,
	    const wxSize& size = wxDefaultSize, long style = 0,
	    const wxString& name = wxT("staticText")) :
	wxStaticText(parent, id, label, pos, size, style, name) {}

    wxStaticWrappedText(unsigned int width, wxWindow* parent, wxWindowID id,
	    const wxString& label, const wxPoint& pos = wxDefaultPosition,
	    const wxSize& size = wxDefaultSize, long style = 0,
	    const wxString& name = wxT("staticText")) :
	wxStaticText(parent, id, label, pos, size, style, name) {
	    WrapText(width);
	}

    void WrapText(int width);

private:
    void NextPiece(wxString const &label, size_t pos,
	size_t &whitelen, int &whiteisnl, size_t &nonwhitelen);
};

#endif
