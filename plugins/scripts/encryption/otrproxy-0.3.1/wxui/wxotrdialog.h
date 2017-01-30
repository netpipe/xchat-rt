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

#ifndef __WXOTRDIALOG_H__
#define __WXOTRDIALOG_H__

#include <map>
extern "C" {
#include <context.h>
#include <proto.h>
#include <userstate.h>
#include <message.h>
#include "uisync.h"
}
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include "wxstatwrtext.h"

class wxOTRDialog : public wxDialog {
public:
    wxOTRDialog(unsigned int dialogID, OtrlNotifyLevel level,
	    const wxString &title, const wxString &primary,
	    const wxString &secondary, long style = wxOK);

    wxOTRDialog(unsigned int dialogID, const wxString &title,
	    const wxString &primary, const wxString &secondary,
	    long style = wxOK);

    ~wxOTRDialog();

    void Track(wxOTRDialog **tracker);
    void EnableOK(bool enable);
    void AppendSecondary(wxString appendtext);
    void AddSessionID(OtrlSessionIdHalf boldhalf, unsigned char *sessionid,
	    size_t sessionid_len);

    static void EnableOK(unsigned int dialogid, bool enable);
    static void AppendSecondary(unsigned int dialogid, wxString appendtext);

private:
    void create_contents(unsigned int dialogID, int haslevel,
	    OtrlNotifyLevel level, const wxString &title,
	    const wxString &primary, const wxString &secondary,
	    long style);

    void OnCancel(wxCommandEvent& ev);
    void OnOK(wxCommandEvent& ev);

    int wrapwidth;
    wxBoxSizer *topsizer;
    wxBoxSizer *textsizer;
    wxStaticWrappedText *secondaryst;
    wxButton *okbutton;
    unsigned int dialogID;
    int result;
    wxOTRDialog **tracker;

    static std::map<unsigned int, wxOTRDialog *> idmap;

    DECLARE_EVENT_TABLE()
};

#endif
