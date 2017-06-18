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

#ifndef __WXOTRPOLICYBOX_H
#define __WXOTRPOLICYBOX_H

#include <set>
#include <wx/frame.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/checkbox.h>
#include <wx/statbox.h>

#include "wxotrpolicy.h"

class wxOTRPolicyBox : public wxPanel {

    public:
	wxOTRPolicyBox(wxWindow* parent, const wxString &title, 
		Buddy* buddy);
	void SetFrameToClose(wxFrame* frame);
	static void UpdateStartEndButtons();
	~wxOTRPolicyBox();

    private:
	wxFrame* parentframe;
	Buddy b;
	wxStaticBoxSizer* policybox_sizer;
	wxCheckBox* use_defaults;
	wxCheckBox* enable_priv;
	wxCheckBox* auto_priv;
	wxCheckBox* require_priv;
	wxButton* start_priv;
	wxButton* end_priv;
	bool isglobal;
	static std::set <wxOTRPolicyBox*> buddyboxes;

	void OnPolicyChange(wxCommandEvent &event);
	void CreateCheckBoxes();
	void InitCheckBoxes(OtrlPolicy policy, bool useglobal);
	void CreateButtons(wxBoxSizer* mainsizer);
	void InitButtons();
	ConnContext* GetBuddyContext();
	void OnStartPrivate(wxCommandEvent &event);
	void OnEndPrivate(wxCommandEvent &event);
	void OnClose(wxCommandEvent &event);

	DECLARE_EVENT_TABLE()
};

#endif
