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
#include <wx/listctrl.h>
#include <wx/clipbrd.h>
#include "wxprefframe.h"
#include "wxotrproxy.h"
#include "wxotrpolicy.h"
#include "wxotrpolicybox.h"


extern "C" {
#include "otrproxy.h"
}

wxPrefFrame* wxPrefFrame::prefframe = NULL;

wxPrefFrame::wxPrefFrame (const wxChar *title, int xpos, int ypos,
	                    int width, int height)
                    : wxFrame ((wxFrame *) NULL,
		    -1, title, wxPoint(xpos, ypos),
		    wxDefaultSize,
		    wxDEFAULT_FRAME_STYLE){

    // Constructor for Preferences window, which contains a
    // notebook with three tabs:
    //
    // 1.  Known Fingerprints -- fingerprints for all users with
    //     whom user has held private conversations.
    // 2.  OTR Preferences -- information for user's private keys,
    //     and global preferences for private conversations.
    // 3.  Proxy Settings  -- proxy protocol, host and port setting.
    //     NOT YET IMPLEMENTED.

    // defaults for sort in fingerprint tab
    sort_column = 0;
    order = ASCENDING;

    // Sizer for prefs window.  Contains notebook.
    wxBoxSizer *mainsizer = new wxBoxSizer(wxVERTICAL);
    wxNotebook *nb = new wxNotebook(this, wxID_ANY);

    // Known fingerprints tab.
    wxBoxSizer *fpsizer = new wxBoxSizer(wxVERTICAL);
    wxNotebookPage *kfps = new wxPanel(nb);

    fplist = new wxListCtrl(kfps, wxID_ANY, wxDefaultPosition,
	    wxDefaultSize, wxLC_REPORT);
    fplist->InsertColumn(SCREENNAME, wxT("Screenname"), wxLIST_FORMAT_LEFT,
	    110);
    fplist->InsertColumn(STATUS, wxT("Status"), wxLIST_FORMAT_LEFT, 90);
    fplist->InsertColumn(FINGERPRINT, wxT("Fingerprint"), wxLIST_FORMAT_LEFT,
	    400);
    fplist->InsertColumn(ACCOUNT, wxT("Account"), wxLIST_FORMAT_LEFT, 200);
    fpsizer->Add(fplist, 1, wxALL | wxEXPAND, 1);

    wxBoxSizer *fpbuttons = new wxBoxSizer(wxHORIZONTAL);
    start_priv = new wxButton(kfps, START_PRIV,
	    wxT("Start private connection"));
    fpbuttons->Add(start_priv, 0, wxALL | wxALIGN_LEFT, 5);
    start_priv->Disable();
    end_priv = new wxButton(kfps, END_PRIV, wxT("End private connection"));
    fpbuttons->Add(end_priv, 0, wxALL | wxALIGN_CENTER, 5);
    end_priv->Disable();
    forget_fp = new wxButton(kfps, FORGET_FP, wxT("Forget fingerprint"));
    fpbuttons->Add(forget_fp, 0, wxALL | wxALIGN_RIGHT, 5);
    forget_fp->Disable();

    fpsizer->Add(fpbuttons, 0, wxALL | wxALIGN_CENTER);
    kfps->SetSizer(fpsizer);

    nb->AddPage(kfps, wxT("Known fingerprints"), TRUE);
    mainsizer->Add(nb, 1, wxEXPAND | wxTOP | wxLEFT | wxRIGHT, 5);

    // OTR preferences tab
    otrprefs = new wxPanel(nb);
    nb->AddPage(otrprefs, wxT("OTR Preferences"));
    wxBoxSizer* otrprefs_sizer = new wxBoxSizer(wxVERTICAL);

    // private key info
    wxStaticBox* privkey_box = new wxStaticBox(otrprefs, wxID_ANY,
	    wxT("My private keys"), wxDefaultPosition);
    wxStaticBoxSizer* privkey_sizer = new wxStaticBoxSizer(
	    privkey_box, wxVERTICAL);
    wxBoxSizer* account_menu_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText* account_label = new wxStaticText(otrprefs,
	    wxID_ANY, wxT("Key for account:"));
    account_menu_sizer->Add(account_label, 0, wxALL | wxALIGN_CENTER, 5);
    account_menu = new wxChoice(otrprefs, wxID_ANY, wxDefaultPosition,
	    wxDefaultSize, 0, NULL);
    account_menu_sizer->Add(account_menu, 1, wxALL |
	    wxALIGN_CENTER, 5);
    account_list = NULL;
    privkey_sizer->Add(account_menu_sizer, 1, wxEXPAND | wxALIGN_TOP, 5);
    account_fp = new wxStaticText(otrprefs, wxID_ANY, wxT(""));
    privkey_sizer->Add(account_fp, 0, wxALIGN_CENTER, 0);

    wxBoxSizer* privkey_buttons_sizer = new wxBoxSizer(wxHORIZONTAL);
    copy_fp = new wxButton(otrprefs, COPY_FP, wxT("Copy fingerprint"));
    privkey_buttons_sizer->Add(copy_fp, 1, wxALL | wxALIGN_CENTER, 3);
    generate = new wxButton(otrprefs, GENERATE, wxT("Generate key"));
    privkey_buttons_sizer->Add(generate, 1, wxALL | wxALIGN_CENTER, 3);
    privkey_sizer->Add(privkey_buttons_sizer, 0, wxEXPAND | wxALL |
	    wxALIGN_BOTTOM);
    otrprefs_sizer->Add(privkey_sizer, 0, wxEXPAND | wxALL | wxALIGN_TOP);

    // OTR global policy settings
    wxOTRPolicyBox *global_policybox = new wxOTRPolicyBox(otrprefs,
	    wxT("Default OTR Settings"), NULL);
    otrprefs_sizer->Add(global_policybox, 1, wxEXPAND | wxALL
	    | wxALIGN_BOTTOM);
    otrprefs->SetSizer(otrprefs_sizer);

    // Proxy preferences tab goes here.

    //wxNotebookPage *proxyprefs = new wxPanel(nb);
    //nb->AddPage(proxyprefs, wxT("Proxy Preferences"));

    mainsizer->Add( new wxButton( this, wxID_CLOSE, wxT("Close") ),
	0, wxALL | wxALIGN_RIGHT, 10);
    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);
#ifdef __WXMAC__
    // I don't know why wxMac isn't setting the mainsizer's minimum
    // size.  For now, manually set the size of the window.
    SetSize(525, 315);
#endif
}

int wxCALLBACK wxListCompareScreenname(long item1, long item2, long order){

    // Comparison function for sort on screenname.

    Fingerprint* a = (Fingerprint*)item1;
    Fingerprint* b = (Fingerprint*)item2;
    int ret = (strcmp(a->context->username, b->context->username));
    if(order == ASCENDING) return ret;
    else return -ret;
}

int GetRank(Fingerprint* fp){

    // Return ranking of connection state so that we can
    // reasonably sort on this column.
    //
    // Order is private, finished, not private, unused.

    if((fp->context->msgstate == OTRL_MSGSTATE_ENCRYPTED) &&
	    (fp->context->active_fingerprint == fp)){
	return 0;
    } else if(fp->context->msgstate == OTRL_MSGSTATE_FINISHED){
	return 1;
    } else if(fp->context->msgstate == OTRL_MSGSTATE_PLAINTEXT){
	return 2;
    } else {
	return 3;
    }
}

int wxCALLBACK wxListCompareStatus(long item1, long item2, long order){

    // Comparison function for sort on connection state.

    Fingerprint* a = (Fingerprint*)item1;
    Fingerprint* b = (Fingerprint*)item2;
    int status_a = GetRank(a);
    int status_b = GetRank(b);
    int ret;
    if(status_a < status_b) ret = -1;
    else if(status_a > status_b) ret = 1;
    else ret =  0;
    if(order == ASCENDING) return ret;
    else return -ret;
}

int wxCALLBACK wxListCompareFingerprint(long item1, long item2, long order){

    // Comparison function for sort on fingerprint.

    Fingerprint* a = (Fingerprint*)item1;
    Fingerprint* b = (Fingerprint*)item2;
    int ret = (memcmp(a->fingerprint, b->fingerprint, 20));
	                        // 20 bytes per fingerprint
    if(order == ASCENDING) return ret;
    else return -ret;
}

int wxCALLBACK wxListCompareAccount(long item1, long item2, long order){

    // Comparison function for sort on accountname.

    Fingerprint* a = (Fingerprint*)item1;
    Fingerprint* b = (Fingerprint*)item2;
    int ret = (strcmp(a->context->accountname, b->context->accountname));
    if(order == ASCENDING) return ret;
    else return -ret;
}

void wxPrefFrame::SortItems(wxListEvent &event){

    // Sort fingerprint list based on column clicked.  If the immediately
    // previous sort was on the same column, reverse sort order, otherwise
    // sort in acsending order.

    if(event.m_col == SCREENNAME){
	if(sort_column == SCREENNAME){
	    // If we last sorted on this column, reverse order.
	    if(order == ASCENDING){
		order = DESCENDING;
	    } else {
		order = ASCENDING;
	    }
	} else {
	    order = ASCENDING;
	    sort_column = SCREENNAME;
	}
    } else if(event.m_col == STATUS){
	if(sort_column == STATUS){
	    // If we last sorted on this column, reverse order.
	    if(order == ASCENDING){
		order = DESCENDING;
	    } else {
		order = ASCENDING;
	    }
	} else {
	    order = ASCENDING;
	    sort_column = STATUS;
	}
    } else if(event.m_col == FINGERPRINT){
	if(sort_column == FINGERPRINT){
	    // If we last sorted on this column, reverse order.
	    if(order == ASCENDING){
		order = DESCENDING;
	    } else {
		order = ASCENDING;
	    }
	} else {
	    order = ASCENDING;
	    sort_column = FINGERPRINT;
	}
    } else {
	if(sort_column == ACCOUNT){
	    // If we last sorted on this column, reverse order.
	    if(order == ASCENDING){
		order = DESCENDING;
	    } else {
		order = ASCENDING;
	    }
	} else {
	    order = ASCENDING;
	    sort_column = ACCOUNT;
	}
    }
    DoSort();
}

void wxPrefFrame::DoSort(){

    // Sort fingerprint list based on column and order.
    // Use lock, as comparison routines access context state
    // list.

    uisync_lock(wxotrproxy_uisync);
    if(sort_column == SCREENNAME){
	fplist->SortItems(wxListCompareScreenname, order);
    } else if (sort_column == STATUS){
	fplist->SortItems(wxListCompareStatus, order);
    } else if(sort_column == FINGERPRINT){
	fplist->SortItems(wxListCompareFingerprint, order);
    } else {
	fplist->SortItems(wxListCompareAccount, order);
    }
    uisync_unlock(wxotrproxy_uisync);
}

void wxPrefFrame::OnClose(wxCommandEvent &event){

    // Called when close button is clicked.
    Close(TRUE);
}

void wxPrefFrame::OnListItemSelected(wxListEvent &event){

    // Called when line is selected from the fingerprint
    // list.
    //
    // Enables and disables buttons based on connection state.
    // Use lock, as we're accessing context state list.

    Fingerprint* fp = (Fingerprint*)event.GetData();
    selectedfp = fp;
    uisync_lock(wxotrproxy_uisync);
    if (fp->context->msgstate == OTRL_MSGSTATE_PLAINTEXT) {
	start_priv->Enable();
	end_priv->Disable();
	forget_fp->Enable();
    } else if (fp->context->msgstate == OTRL_MSGSTATE_FINISHED) {
	start_priv->Enable();
	end_priv->Enable();
	forget_fp->Enable();
    } else if (fp->context->msgstate == OTRL_MSGSTATE_ENCRYPTED) {
	if (fp == fp->context->active_fingerprint) {
	    start_priv->Disable();
	    end_priv->Enable();
	    forget_fp->Disable();
	} else {
	    start_priv->Disable();
	    end_priv->Disable();
	    forget_fp->Enable();
	}
    }
    uisync_unlock(wxotrproxy_uisync);
}

void wxPrefFrame::OnListItemDeselected(wxListEvent &event){

    // Called when line in the fingerprint list is deselected.
    //
    // Disables all buttons.

    start_priv->Disable();
    end_priv->Disable();
    forget_fp->Disable();
    selectedfp = NULL;
}

void wxPrefFrame::OnStartPrivate(wxCommandEvent &event){

    // Called when "Start private connection" button is
    // clicked.
    //
    // Starts private connection with selected user.  Use lock because
    // we're accessing the context list and changing state.

    if(selectedfp == NULL) return;
    uisync_lock(wxotrproxy_uisync);
    if (selectedfp->context != NULL) {
	if (selectedfp->context->msgstate != OTRL_MSGSTATE_ENCRYPTED) {
	    otrproxy_connect(selectedfp->context);
	}
    }
    uisync_unlock(wxotrproxy_uisync);
}

void  wxPrefFrame::OnEndPrivate(wxCommandEvent &event){

    // Called when "End private connection" button is
    // clicked.
    //
    // Ends private connection with selected user.  Use lock because
    // we're accessing the context list and changing state.

    if(selectedfp == NULL) return;
    uisync_lock(wxotrproxy_uisync);
    if(selectedfp->context != NULL){
	ConnContext* context = selectedfp->context;
	if( (context->msgstate == OTRL_MSGSTATE_FINISHED) ||
		((context->msgstate == OTRL_MSGSTATE_ENCRYPTED) &&
		    (context->active_fingerprint == selectedfp)) ){
	    otrproxy_disconnect(context);
	}
    }
    uisync_unlock(wxotrproxy_uisync);
}

void wxPrefFrame::OnForgetFingerprint(wxCommandEvent &event){

    // Called when "Forget fingerprint" button is clicked.
    //
    // If there's a fingerprint line selected, gets list
    // index of line, sets item state to "unselected", and
    // calls otrproxy_forget_fingerprint to delete the fingerprint
    // from the context list.
    //
    // Lock because we're accessing the context list.

    if(selectedfp == NULL) return;
    uisync_lock(wxotrproxy_uisync);
    long index = fplist->FindItem(-1, (long)selectedfp);
    Fingerprint* savedfp = selectedfp;
    if(index >= 0){
	fplist->SetItemState(index, 0, wxLIST_STATE_SELECTED);
    }
    otrproxy_forget_fingerprint(savedfp);
    uisync_unlock(wxotrproxy_uisync);
}

void wxPrefFrame::OnAccountSelected(wxCommandEvent &ev){

    // Called when account is selected from dropdown in
    // private key info box in OTR prefs tab.
    //
    // If selected account has a fingerprint, displays it.
    // Enables and disables buttons accordingly.

    AccountList account = (AccountList)account_menu->GetClientData(
			    account_menu->GetSelection());
    if(account->fingerprint_text != NULL){
	account_fp->SetLabel(wxT("Fingerprint:  ") +
		wxString(account->fingerprint_text, wxConvUTF8));
	otrprefs->Layout();
	generate->Disable();
	copy_fp->Enable();
    } else {
	account_fp->SetLabel(wxT("No fingerprint"));
	otrprefs->Layout();
	generate->Enable();
	copy_fp->Disable();
    }
}

void wxPrefFrame::OnCopyFP(wxCommandEvent &event){

    // Called when "Copy fingerprint" button is clicked.
    //
    // Opens clipboard, if possible, and copies key to it.

    AccountList account= (AccountList)account_menu->GetClientData(
			    account_menu->GetSelection());
////    if(wxTheClipboard->Open()) {
////	wxTheClipboard->UsePrimarySelection(TRUE);
////	wxTheClipboard->SetData(new wxTextDataObject(
////		    wxString(account->fingerprint_text, wxConvUTF8)));
////	wxTheClipboard->Close();
////    }
}

void wxPrefFrame::OnGenerate(wxCommandEvent &event){

    // Called when the "Generate" button is clicked.
    //
    // Creates insensitive dialog box indicating that key is being
    // generated, generates key, then changes dialog to resesitize it
    // and indicate that the task has been completed.  Use lock, as
    // context list is being changed.

    AccountList account = (AccountList)account_menu->GetClientData(
			    account_menu->GetSelection());
    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(0, OTRL_NOTIFY_INFO,
            wxT("Generating Private Key"),
            wxT("Please wait."),
            wxT("Generating private key for ") + wxString(account->accountname,
	    wxConvUTF8) + wxT(" ") +
            wxMainFrame::GetProtocolString(wxString(account->protocol,
	    wxConvUTF8)) + wxT(" ..."), 0);
    dialog->Show(TRUE);
    wxSafeYield();
    uisync_lock(wxotrproxy_uisync);
    otrproxy_generate_privkey(account->accountname, account->protocol);
    uisync_unlock(wxotrproxy_uisync);
    dialog->AppendSecondary(wxT(" Done."));
    dialog->EnableOK(TRUE);
}



wxPrefFrame::~wxPrefFrame(){

    // default destructor.
    prefframe = NULL;
}

BEGIN_EVENT_TABLE (wxPrefFrame, wxFrame)
    EVT_BUTTON (wxID_CLOSE, wxPrefFrame::OnClose)
    EVT_BUTTON (END_PRIV, wxPrefFrame::OnEndPrivate)
    EVT_BUTTON (START_PRIV, wxPrefFrame::OnStartPrivate)
    EVT_BUTTON (FORGET_FP, wxPrefFrame::OnForgetFingerprint)
    EVT_BUTTON (COPY_FP, wxPrefFrame::OnCopyFP)
    EVT_BUTTON (GENERATE, wxPrefFrame::OnGenerate)
    EVT_LIST_COL_CLICK(wxID_ANY, wxPrefFrame::SortItems)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, wxPrefFrame::OnListItemSelected)
    EVT_LIST_ITEM_DESELECTED(wxID_ANY, wxPrefFrame::OnListItemDeselected)
    EVT_CHOICE(wxID_ANY, wxPrefFrame::OnAccountSelected)
END_EVENT_TABLE()
