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

#ifndef __WXPREFFRAME_H
#define __WXPREFFRAME_H

#include <wx/fontdlg.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/choice.h>
#include <wx/button.h>
#include <wx/notebook.h>
#include <wx/checkbox.h>
#include "wxmainframe.h"
#include "wxotrdialog.h"

extern "C" {
#include "accountlist.h"
}


enum {
    START_PRIV = wxID_HIGHEST + 1,
    END_PRIV,
    FORGET_FP,
    GENERATE,
    COPY_FP
};

enum {
    SCREENNAME,
    STATUS,
    FINGERPRINT,
    ACCOUNT
};

enum {
    ASCENDING,
    DESCENDING
};

enum {
    GLOBAL_POLICY = wxID_HIGHEST + 1,
};

class wxPrefFrame : public wxFrame{

    public:
        wxPrefFrame(const wxChar *title,
                int xpos, int ypos,
                int width, int height);
	
	void OnClose(wxCommandEvent &event);
	void SortItems(wxListEvent &event);
	void DoSort();
	void OnListItemSelected(wxListEvent &event);
	void OnListItemDeselected(wxListEvent &event);
	void OnStartPrivate(wxCommandEvent &event);
	void OnForgetFingerprint(wxCommandEvent &event);
	void OnEndPrivate(wxCommandEvent &event);
	void OnAccountSelected(wxCommandEvent &ev);
	void OnCopyFP(wxCommandEvent &event);
	void OnGenerate(wxCommandEvent &event);
	void OnPolicyChange(wxCommandEvent &event);
	void InitGlobalPolicyCheckboxes();
	~wxPrefFrame();

	static wxPrefFrame *prefframe;
	wxListCtrl* fplist;
	Fingerprint* selectedfp;
	wxNotebookPage* otrprefs;
	wxButton* start_priv;
	wxButton* end_priv;
	wxButton* forget_fp;
	int sort_column;
	int order;
	wxChoice* account_menu;
	wxStaticText* account_fp;
	wxButton* copy_fp;
	wxButton* generate;
	AccountList account_list;
	wxCheckBox* enable_priv;
	wxCheckBox* auto_priv;
	wxCheckBox* require_priv;

	DECLARE_EVENT_TABLE()
};
#endif
