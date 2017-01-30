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

#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/statline.h>
#include <wx/button.h>
#include "wxotrproxy.h"
#include "wxprefframe.h"
#include "wxotrpolicy.h"
#include "wxotrpolicybox.h"

extern "C" {
#include "uisync.h"
#include "otrproxy.h"
}

std::set <wxOTRPolicyBox*> wxOTRPolicyBox::buddyboxes;

wxOTRPolicyBox::wxOTRPolicyBox(wxWindow* parent, const wxString &title,
	Buddy* buddy) : wxPanel(parent, wxID_ANY) {

    // Default constructor for frame containing a static box
    // which contains checkboxes for either the global OTR policy
    // or the policy settings for a particular buddy.
    //
    // In the case of the per-buddy static box, an extra checkbox
    // indicates whether the policy is inherited from the global
    // settings.

    wxBoxSizer* mainsizer = new wxBoxSizer(wxVERTICAL);
    wxStaticBox* box = new wxStaticBox(this, wxID_ANY, title);
    policybox_sizer = new wxStaticBoxSizer(box, wxVERTICAL);
    wxOTRPolicy* manager = wxOTRPolicy::GetManager();
    OtrlPolicy policy;
    bool useglobal = FALSE;
    parentframe = NULL;
    if(buddy != NULL) {
	isglobal = FALSE;
	b = *buddy;
	buddyboxes.insert(this);
	use_defaults = new wxCheckBox(this, wxID_ANY,
		wxT("Use default OTR settings for this buddy"));
	policybox_sizer->Add(use_defaults, 0, wxALIGN_LEFT);
	wxStaticLine *hr = new wxStaticLine(this, wxID_ANY);
	policybox_sizer->Add(hr, 0, wxEXPAND | wxALIGN_CENTER);
	manager->GetBuddy(*buddy, useglobal, policy);
	use_defaults->SetValue(useglobal);
    } else {
	isglobal = TRUE;
	manager->GetGlobal(policy);
    }
    CreateCheckBoxes();
    InitCheckBoxes(policy, useglobal);
    mainsizer->Add(policybox_sizer, 0, wxEXPAND);

    // If we're a per-buddy panel, create buttons to
    // start and end private conversations, and to close
    // the panel.
    if(isglobal == FALSE){
	CreateButtons(mainsizer);
	InitButtons();
    }
    SetSizer(mainsizer);
}

void wxOTRPolicyBox::CreateCheckBoxes() {

    // Create the policy checkboxes.
    enable_priv = new wxCheckBox(this, wxID_ANY,
	    wxT("Enable private messaging"));
    policybox_sizer->Add(enable_priv, 0, wxALIGN_LEFT);
    auto_priv = new wxCheckBox(this, wxID_ANY,
	    wxT("Automatically initiate private messaging"));
    policybox_sizer->Add(auto_priv, 0, wxLEFT | wxALIGN_LEFT, 7);
    require_priv = new wxCheckBox(this, wxID_ANY,
	    wxT("Require private messaging"));
    policybox_sizer->Add(require_priv, 0, wxLEFT | wxALIGN_LEFT, 14);

}

void wxOTRPolicyBox::InitCheckBoxes(OtrlPolicy policy, bool useglobal) {

    // Set the values of the checkboxes according to policy,
    // then enable/disable them accordingly.

    enable_priv->Enable();
    if(policy == OTRL_POLICY_NEVER) {
	enable_priv->SetValue(FALSE);
	auto_priv->SetValue(FALSE);
	auto_priv->Disable();
	require_priv->SetValue(FALSE);
	require_priv->Disable();
    } else if(policy == OTRL_POLICY_MANUAL) {
	enable_priv->SetValue(TRUE);
	auto_priv->SetValue(FALSE);
	auto_priv->Enable();
	require_priv->SetValue(FALSE);
	require_priv->Disable();
    } else if(policy == OTRL_POLICY_OPPORTUNISTIC) {
	enable_priv->SetValue(TRUE);
	auto_priv->SetValue(TRUE);
	auto_priv->Enable();
	require_priv->SetValue(FALSE);
	require_priv->Enable();
    } else {  // policy is OTRL_POLICY_ALWAYS
	enable_priv->SetValue(TRUE);
	auto_priv->SetValue(TRUE);
	auto_priv->Enable();
	require_priv->SetValue(TRUE);
	require_priv->Enable();
    }
    // isglobal is only possibly TRUE if we're dealing with
    // the per-buddy policy box.  If it's TRUE, we're using
    // the global settings, so disable the checkboxes.
    if(useglobal == TRUE) {
	enable_priv->Disable();
	auto_priv->Disable();
	require_priv->Disable();
    }
}

void wxOTRPolicyBox::CreateButtons(wxBoxSizer* mainsizer) {

	// This is is only called when we're in per-buddy mode.
	//
	// Create buttons to start and end a private conversation.

	wxBoxSizer* buttonsizer = new wxBoxSizer(wxHORIZONTAL);
	start_priv = new wxButton(this, START_PRIV,
		wxT("Start private connection"));
	buttonsizer->Add(start_priv, 0, wxALL | wxALIGN_LEFT, 5);
	end_priv = new wxButton(this, END_PRIV,
		wxT("End private connection"));
	buttonsizer->Add(end_priv, 0, wxALL | wxALIGN_RIGHT, 5);
	mainsizer->Add(buttonsizer, 0, wxALL | wxALIGN_CENTER, 0);
	// Create button to close panel.
	wxButton* close_panel = new wxButton(this, wxID_CLOSE);
	mainsizer->Add(close_panel, 0, wxALL | wxALIGN_RIGHT, 10);
}

void wxOTRPolicyBox::InitButtons() {

    // Only called in per-buddy mode.
    //
    // Enable/disable buttons based on current connection
    // state with this buddy.  (If we're not in a private conversation
    // with her, enable start button; if we are, enable end button; if
    // we're finished our private conversation, enable both.)

    uisync_lock(wxotrproxy_uisync);
    ConnContext* con = GetBuddyContext();
    if (con) {
	switch(con->msgstate) {
	    case OTRL_MSGSTATE_PLAINTEXT:
		start_priv->Enable();
		end_priv->Disable();
		break;
	    case OTRL_MSGSTATE_ENCRYPTED:
		start_priv->Disable();
		end_priv->Enable();
		break;
	    case OTRL_MSGSTATE_FINISHED:
		start_priv->Enable();
		end_priv->Enable();
		break;
	}
    }
    uisync_unlock(wxotrproxy_uisync);
}

void wxOTRPolicyBox::UpdateStartEndButtons() {

    // Iterate through list of open buddy policy boxes and
    // enable/disable buttons depending on connection state.

    std::set<wxOTRPolicyBox*>::iterator a;
    a = buddyboxes.begin();
    while(a != buddyboxes.end()) {
	(*a)->InitButtons();
	a++;
    }
}

ConnContext* wxOTRPolicyBox::GetBuddyContext() {

    // Returns a pointer to the context asociated with the member b,
    // which is a Buddy.
    //
    // The caller MUST use uisync_lock() / uisync_unlcok() around
    // calls to this method.

    const wxCharBuffer accountname = b.accountname.mb_str(wxConvUTF8);
    const wxCharBuffer protocol = b.protocol.mb_str(wxConvUTF8);
    const wxCharBuffer username = b.username.mb_str(wxConvUTF8);
    return otrl_context_find(otrproxy_userstate, username, accountname,
	    protocol, 1, NULL, NULL, NULL); //tecan-addifmissing
}

void wxOTRPolicyBox::OnStartPrivate(wxCommandEvent &event) {

    // Called when "Start private connection" button is
    // clicked.
    //
    // Starts private connection with selected user.

    const wxCharBuffer accountname = b.accountname.mb_str(wxConvUTF8);
    const wxCharBuffer protocol = b.protocol.mb_str(wxConvUTF8);
    const wxCharBuffer username = b.username.mb_str(wxConvUTF8);
    uisync_lock(wxotrproxy_uisync);
    ConnContext* con = GetBuddyContext();
    if (con) {
	otrproxy_connect(con);
    }
    uisync_unlock(wxotrproxy_uisync);
}

void wxOTRPolicyBox::OnEndPrivate(wxCommandEvent &event) {

    // Called when "End private connection" button is
    // clicked.
    //
    // Ends private connection with selected user.

    uisync_lock(wxotrproxy_uisync);
    ConnContext* con = GetBuddyContext();
    if(con && con->msgstate != OTRL_MSGSTATE_PLAINTEXT) {
	otrproxy_disconnect(con);
    }
    uisync_unlock(wxotrproxy_uisync);

}

void wxOTRPolicyBox::SetFrameToClose(wxFrame* frame) {

    // Called by creator of wxOTRPolicyBox if the policy box
    // has a close button.  (Currently this is only the case for
    // the per-buddy policy box.)

    parentframe = frame;
}

void wxOTRPolicyBox::OnClose(wxCommandEvent &event){

    if(parentframe != NULL) {
	parentframe->Close(TRUE);
    }
}

void wxOTRPolicyBox::OnPolicyChange(wxCommandEvent &event){

    // Called when any of the checkboxes in the global or per-buddy
    // policy preferences is changed.
    //
    // Enable and disable other checkboxes as appropriate, change policy.

    printf("In OnPolicyChange...\n\n");
    wxOTRPolicy* manager = wxOTRPolicy::GetManager();
    OtrlPolicy policy;
    if((isglobal) || (use_defaults->IsChecked() == FALSE)) {
	enable_priv->Enable();
	if(enable_priv->IsChecked()){
	    auto_priv->Enable();
	    if(auto_priv->IsChecked()){
		policy = OTRL_POLICY_OPPORTUNISTIC;
		require_priv->Enable();
		if(require_priv->IsChecked()){
		    policy = OTRL_POLICY_ALWAYS;
		}
	    } else {
		policy = OTRL_POLICY_MANUAL;
		require_priv->Disable();
	    }
	} else {
	    policy = OTRL_POLICY_NEVER;
	    auto_priv->Disable();
	    require_priv->Disable();
	}
	if(isglobal) {
	    manager->SetGlobal(policy);
	} else {
	    manager->SetBuddy(b, FALSE, policy);
	}
    } else {
	enable_priv->Disable();
	auto_priv->Disable();
	require_priv->Disable();
	manager->SetBuddy(b, TRUE, OTRL_POLICY_DEFAULT);
    }
}


wxOTRPolicyBox::~wxOTRPolicyBox(){

    // Destructor.

    if(isglobal == FALSE)
	buddyboxes.erase(this);
}


BEGIN_EVENT_TABLE (wxOTRPolicyBox, wxPanel)
    EVT_CHECKBOX (wxID_ANY, wxOTRPolicyBox::OnPolicyChange)
    EVT_BUTTON (START_PRIV, wxOTRPolicyBox::OnStartPrivate)
    EVT_BUTTON (END_PRIV, wxOTRPolicyBox::OnEndPrivate)
    EVT_BUTTON (wxID_CLOSE, wxOTRPolicyBox::OnClose)
END_EVENT_TABLE()
