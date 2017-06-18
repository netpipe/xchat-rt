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

#include <vector>
#include <wx/fontdlg.h>
#include "wxmainframe.h"
#include "wxprefframe.h"
#include "wxotrdialog.h"
#include "wxotrproxy.h"
#include "wxotrpolicybox.h"

extern "C"{
    #include "otrproxy.h"
}

wxMainFrame::wxMainFrame (wxOTRProxy* proxy, const wxChar *title, 
		    int xpos, int ypos,
		    int width, int height)
		    : wxFrame ((wxFrame *) NULL,
			    -1, title, wxPoint(xpos, ypos),
			    wxSize(width, height),
			    wxDEFAULT_FRAME_STYLE){
			
     // Constructor for the main window, containing file, edit and help
     // menus, as well as tree for current private connections.

    this->proxy = proxy;
    menuBar = (wxMenuBar *) NULL;
    fileMenu = (wxMenu *) NULL;
    editMenu = (wxMenu *) NULL;
    helpMenu = (wxMenu *) NULL;

    fileMenu = new wxMenu;
    fileMenu->Append(MAIN_EXIT, wxT("E&xit"), wxT("Exit proxy"));

    editMenu = new wxMenu;
    editMenu->Append(MAIN_PREFS, wxT("&Preferences..."), 
	    wxT("Edit Preferences"));

    helpMenu = new wxMenu;
    helpMenu->Append(MAIN_ABOUT, wxT("&About"), wxT("Authors"));

    menuBar = new wxMenuBar;
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(helpMenu, wxT("&Help"));
    SetMenuBar(menuBar);
    CreateStatusBar(2);
    
    tree = new wxTreeCtrl(this, -1, wxDefaultPosition, wxDefaultSize,
	    wxTR_HIDE_ROOT | wxTR_NO_BUTTONS | wxVSCROLL | wxHSCROLL |
	    wxTR_NO_LINES);
}

wxOTRDialog *wxMainFrame::aboutbox = NULL;

void wxMainFrame::OnAbout (wxCommandEvent & event){ 

    // Called when "About" is chosen from the "Help" Menu.
    //
    // Construct dialog box containing program and developer
    // info.

    // Don't show more than one About box
    if (aboutbox) {
	return;
    }

    wxString title = wxT("About OTR Proxy");
    wxString primary = wxT("OTR Proxy " OTRPROXY_VERSION);
    wxString secondary = wxT("Nikita Borisov, Ian Goldberg, Katrina Hanna " 
	    "\n<otr@cypherpunks.ca>\n\n"
	    "See http://www.cypherpunks.ca/otr/ for more "
	    "information.");
    wxOTRDialog *aboutDialog = new wxOTRDialog(0, title, primary,
	    secondary);
    aboutDialog->Track(&aboutbox);
    aboutDialog->Show(TRUE);
}

void wxMainFrame::OnExit (wxCommandEvent & event){

    // Called when "Exit" is chosen from the "File" menu.

      Close(TRUE);
}

void wxMainFrame::OnPrefs(wxCommandEvent &event){

    // Called when "Preferences" is chosen from the "Edit" menu.
    //
    // If preferences window does not exist, create, if it does,
    // raise it.  Invoke methods to display (or refresh) current 
    // fingerprint and key state.

    if(wxPrefFrame::prefframe == NULL){
	wxPrefFrame::prefframe = new wxPrefFrame(wxT("OTR Proxy Preferences"),
		50, 50, 500, 300);
	wxPrefFrame::prefframe->Show(TRUE);
    } else {
	wxPrefFrame::prefframe->Raise();
    }
    proxy->HandleKnownFPs();
    proxy->UpdatePrivKeyBox();
}

void wxMainFrame::AddTreeItems(wxTreeItemId &branch, 
	std::vector<Buddy> &v) {

    // Add tree items contained in v to tree root rid.

    std::vector<Buddy>::iterator a; 
    a = v.begin();
    while(a != v.end()){
	wxString curracct = a->accountname;
	wxTreeItemId currid = tree->AppendItem(branch, curracct + wxT(" ") 
		+ GetProtocolString(a->protocol));
	tree->Expand(branch);
	while(a != v.end() && curracct == a->accountname){
	    wxTreeItemId buddyid = tree->AppendItem(currid, a->username);
	    tree->Expand(currid);
	    tree->SetItemData(buddyid, new Buddy(*a));
	    a++;
	}
    }
}

void wxMainFrame::DisplayState(std::vector<Buddy> &convect,
	std::vector<Buddy> &nonconvect){

    // Called when PROXYEVENT_CONTEXT_STATE event occurs, indicating
    // that context state has changed.
    //
    // Delete Private and Non-Private tree items if any exist, reconstruct 
    // tree from items in vectors that are passed in.
    
    tree->DeleteAllItems();
    wxTreeItemId rid = tree->AddRoot(wxT("Tree Root"));
    wxTreeItemId priv_branch = tree->AppendItem(rid, 
	    wxT("Private Connections"));
    tree->SetItemBold(priv_branch);
    wxTreeItemId spacer = tree->AppendItem(rid, wxT(""));
    wxTreeItemId non_priv_branch = tree->AppendItem(rid, 
	    wxT("Other Buddies"));
    tree->SetItemBold(non_priv_branch);
    tree->Expand(rid);
    AddTreeItems(priv_branch, convect);
    AddTreeItems(non_priv_branch, nonconvect);
}

void wxMainFrame::OnTreeItemSelect(wxTreeEvent & event) {

    // When an item from one of the main frame's tree is selected,
    // create a new panel containing:
    //
    // 1. Checkboxes for setting OTR policy for the corresponding buddy;
    // 2. A button to:
    //   start a private connection with this buddy if you don't have one
    //   - or -
    //   end the private connection if you have one.
    
    // Get policy info for this buddy.
    Buddy *a = (Buddy*)tree->GetItemData(tree->GetSelection());
    if(a == NULL)  // we've clicked on something that isn't a buddy
	return;

    wxBoxSizer *policysizer = new wxBoxSizer(wxVERTICAL);
    wxFrame *buddyframe = new wxFrame(this, wxID_ANY, 
	    wxT("OTR Buddy Settings"));
    wxString title = wxT("OTR Settings for ") + wxString(a->username,
	    wxConvUTF8);
    wxOTRPolicyBox* policybox = new wxOTRPolicyBox(buddyframe, title, a);
    policybox->SetFrameToClose(buddyframe);
    policysizer->Add(policybox, 0, wxEXPAND | wxALIGN_TOP | wxALL, 7);
    buddyframe->SetSizer(policysizer);
    policysizer->SetSizeHints(buddyframe);
    buddyframe->Show();
}

wxString wxMainFrame::GetProtocolString(wxString proto){

    // Return meaningful string that corresponds to
    // protocol passed in.

    if(proto.Cmp(wxT("prpl-oscar")) == 0){
	return wxT("(AIM/ICQ)");
    } else {
	return wxT("(Unknown Protocol)");
    }
}

void wxMainFrame::OnClose(wxCloseEvent &event){

    // When the main window is closed, terminate the 
    // program.
    wxExit();
}

void wxMainFrame::OnCollapseAttempt(wxTreeEvent & event) {

    // Don't allow branches to be collapsed.
    event.Veto();
}

wxMainFrame::~wxMainFrame(){
    //Default destructor.
}

BEGIN_EVENT_TABLE (wxMainFrame, wxFrame)
    EVT_MENU ( MAIN_EXIT,  wxMainFrame::OnExit)
    EVT_MENU ( MAIN_ABOUT, wxMainFrame::OnAbout)
    EVT_MENU ( MAIN_PREFS,  wxMainFrame::OnPrefs)
    EVT_CLOSE (wxMainFrame::OnClose)
    EVT_TREE_ITEM_COLLAPSING(wxID_ANY, wxMainFrame::OnCollapseAttempt)
    EVT_TREE_ITEM_ACTIVATED(wxID_ANY, wxMainFrame::OnTreeItemSelect)
END_EVENT_TABLE()
