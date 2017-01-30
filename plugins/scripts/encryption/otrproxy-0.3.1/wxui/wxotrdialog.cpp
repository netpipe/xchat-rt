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

#include <wx/dialog.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/artprov.h>
#include <wx/bitmap.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
#include "wxotrproxy.h"
#include "wxotrdialog.h"

std::map<unsigned int, wxOTRDialog *> wxOTRDialog::idmap;

void wxOTRDialog::create_contents(unsigned int dialogID, int haslevel,
	OtrlNotifyLevel level, const wxString &title, const wxString &primary,
	const wxString &secondary, long style)
{
    topsizer = new wxBoxSizer(wxVERTICAL);

    // The first thing in the top-level sizer is a horizontal sizer that
    // will contain (if present) the icon, and the text.
    wxBoxSizer *icontext = new wxBoxSizer(wxHORIZONTAL);

    this->dialogID = dialogID;
    result = -1;

    // The icontext sizer contains the icon, and a vertical sizer
    // containing the text
    if (haslevel) {
	wxIcon icon;
	switch(level) {
	    case OTRL_NOTIFY_ERROR:
		icon = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
		break;
	    case OTRL_NOTIFY_WARNING:
		icon = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
		break;
	    case OTRL_NOTIFY_INFO:
		icon = wxArtProvider::GetIcon(wxART_INFORMATION,
			wxART_MESSAGE_BOX);
		break;
	}
	icontext->Add(new wxStaticBitmap(this, wxID_ANY, icon), 0,
		wxALIGN_LEFT);
    }

    textsizer = new wxBoxSizer(wxVERTICAL);

    // The text sizer contains two static text labels: one in bold, one
    // not.

    if (!primary.IsEmpty()) {
	wxFont curfont = GetFont();
	wxFont boldfont = wxFont(curfont.GetPointSize() * 5 / 4,
		curfont.GetFamily(), wxNORMAL, wxBOLD, FALSE);
	wxStaticWrappedText *primaryst =
	    new wxStaticWrappedText(this, wxID_ANY, primary);
	primaryst->SetFont(boldfont);
	primaryst->WrapText(wrapwidth);
	textsizer->Add(primaryst, 0, wxALIGN_TOP);
    }
    if (!primary.IsEmpty() && !secondary.IsEmpty()) {
	// Put a blank line between the primary and secondary
	textsizer->Add(new wxStaticText(this, wxID_ANY, wxEmptyString),
		0, wxALIGN_TOP);
    }
    if (!secondary.IsEmpty()) {
	secondaryst = new wxStaticWrappedText(this, wxID_ANY, secondary);
	secondaryst->WrapText(wrapwidth);
	textsizer->Add(secondaryst, 0, wxALIGN_TOP);
    } else {
	secondaryst = NULL;
    }

    icontext->Add(textsizer, 1, wxALIGN_RIGHT);

    topsizer->Add(icontext, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP
	    | wxALIGN_CENTER, 5);

    // The second thing in the top-level sizer is a horizontal sizer
    // that will contain the buttons
    wxBoxSizer *buttonsizer = new wxBoxSizer(wxHORIZONTAL);

    // Always create an OK button, but it's only enabled if wxOK is
    // specified.
    okbutton = new wxButton(this, wxID_OK, wxEmptyString,
	    wxDefaultPosition, wxDefaultSize, wxCLIP_SIBLINGS);
    if (! (style & wxOK)) {
	okbutton->Enable(FALSE);
    }
    buttonsizer->Add(okbutton, 0, wxLEFT | wxRIGHT, 5);
    okbutton->SetDefault();

    // Create a Cancel button if wxCANCEL is specified.
    if (style & wxCANCEL) {
	wxButton *cancelbutton = new wxButton(this, wxID_CANCEL,
		wxEmptyString, wxDefaultPosition, wxDefaultSize,
		wxCLIP_SIBLINGS);
	buttonsizer->Add(cancelbutton, 0, wxLEFT | wxRIGHT, 5);
    }

    topsizer->Add(buttonsizer, 0, wxTOP | wxBOTTOM | wxRIGHT | 
	    wxALIGN_RIGHT, 15);

    // Set the topsizer as the top-level sizer
    SetAutoLayout(TRUE);
    SetSizer(topsizer);
    topsizer->SetSizeHints(this);
    topsizer->Fit(this);

    Center(wxBOTH | wxCENTER_FRAME);

    if (dialogID) {
	idmap[dialogID] = this;
    }
    tracker = NULL;
}

wxOTRDialog::wxOTRDialog(unsigned int dialogID, OtrlNotifyLevel level,
	const wxString &title, const wxString &primary,
	const wxString &secondary, long style) :
    wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
	    wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP)
{
    // Make the box a little width than would be needed for the
    // fingerprint
    wxStaticText widthsizer(this, wxID_ANY,
	    wxT("AAAAAAAA AAAAAAAA AAAAAAAA AAAAAAAA AAAAAAAA"));
    wrapwidth = widthsizer.GetSize().GetWidth();
    create_contents(dialogID, 1, level, title, primary, secondary, style);
}

wxOTRDialog::wxOTRDialog(unsigned int dialogID, const wxString &title,
	const wxString &primary, const wxString &secondary,
	long style) :
    wxDialog(NULL, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
	    wxDEFAULT_DIALOG_STYLE)
{
    wrapwidth = 0;
    create_contents(dialogID, 0, OTRL_NOTIFY_INFO, title, primary,
	    secondary, style);
}

wxOTRDialog::~wxOTRDialog()
{
    if (tracker) {
	*tracker = NULL;
    }
    if (dialogID) {
	uisync_lock(wxotrproxy_uisync);
	proxyevent_ui_closed_dialog(dialogID, result);
	uisync_unlock(wxotrproxy_uisync);
	idmap.erase(dialogID);
    }
}

void wxOTRDialog::OnCancel(wxCommandEvent& ev)
{
    result = 0;
    Destroy();
}

void wxOTRDialog::OnOK(wxCommandEvent& ev)
{
    result = 1;
    Destroy();
}

void wxOTRDialog::Track(wxOTRDialog **tracker)
{
    this->tracker = tracker;
    *tracker = this;
}

void wxOTRDialog::EnableOK(bool enable)
{
    okbutton->Enable(enable);
}

void wxOTRDialog::AppendSecondary(wxString appendtext)
{
    if (secondaryst) {
	wxString oldlabel = secondaryst->GetLabel();
	wxString newlabel = oldlabel + appendtext;
	secondaryst->SetLabel(newlabel);
	topsizer->Fit(this);
    }
}

static wxString Sess2Str(unsigned char *sess, size_t len)
{
    char hex[2*len + 1];
    int i;
    for(i=0;i<len;++i) {
	sprintf(hex+(2*i), "%02x", sess[i]);
    }
    return wxString(hex, wxConvUTF8);
}

void wxOTRDialog::AddSessionID(OtrlSessionIdHalf boldhalf,
	unsigned char *sessionid, size_t sessionid_len)
{
    // Make a horizontal BoxSizer containing two StaticText labels,
    // separated by a little space.  One of the labels should be bold.
    wxBoxSizer *sessidsizer = new wxBoxSizer(wxHORIZONTAL);
    wxString sess1str = Sess2Str(sessionid, sessionid_len / 2);
    wxString sess2str = Sess2Str(sessionid+(sessionid_len / 2),
	    sessionid_len / 2);
    wxStaticText *sess1text = new wxStaticText(this, wxID_ANY, sess1str);
    wxStaticText *sess2text = new wxStaticText(this, wxID_ANY, sess2str);
    wxFont boldfont = GetFont();
    boldfont.SetWeight(wxBOLD);
    if (boldhalf == OTRL_SESSIONID_FIRST_HALF_BOLD) {
	sess1text->SetFont(boldfont);
    } else {
	sess2text->SetFont(boldfont);
    }
    sessidsizer->Add(sess1text, 0, 0, 0);
    sessidsizer->Add(sess2text, 0, wxLEFT, 10);
    
    // Attach the new sizer to the existing textsizer
    textsizer->Add(sessidsizer, 0, wxALIGN_LEFT);

    // Re-fit the window
    topsizer->Fit(this);
}

void wxOTRDialog::EnableOK(unsigned int dialogid, bool enable)
{
    wxOTRDialog *dialog = idmap[dialogid];
    if (dialog) {
	dialog->EnableOK(enable);
    }
}

void wxOTRDialog::AppendSecondary(unsigned int dialogid, wxString appendtext)
{
    wxOTRDialog *dialog = idmap[dialogid];
    if (dialog) {
	dialog->AppendSecondary(appendtext);
    }
}

BEGIN_EVENT_TABLE(wxOTRDialog, wxDialog)
    EVT_BUTTON(wxID_CANCEL, wxOTRDialog::OnCancel)
    EVT_BUTTON(wxID_OK, wxOTRDialog::OnOK)
END_EVENT_TABLE()
