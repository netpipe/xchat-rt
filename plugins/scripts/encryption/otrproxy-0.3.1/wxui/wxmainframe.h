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

#ifndef __WXMAINFRAME_H
#define __WXMAINFRAME_H

#include <vector>
#include <wx/treectrl.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/string.h>

#include "wxotrdialog.h"
#include "wxotrpolicy.h"

class wxOTRProxy;

class wxMainFrame : public wxFrame{

    public:
	wxMainFrame(wxOTRProxy* proxy, const wxChar *title,
		int xpos, int ypos,
		int width, int height);
	~wxMainFrame();

	wxMenuBar *menuBar;
	wxMenu *fileMenu;
	wxMenu *editMenu;
	wxMenu *helpMenu;

	wxTreeCtrl *tree;

	void OnExit (wxCommandEvent & event);
	void OnAbout (wxCommandEvent & event);
	void AddTreeItems(wxTreeItemId &branch, std::vector<Buddy> &v);
	void DisplayState(std::vector<Buddy> &convect,
		std::vector<Buddy> &nonconvect);
	void OnPrefs(wxCommandEvent &event);
	void OnClose(wxCloseEvent &event);
	void OnTreeItemSelect(wxTreeEvent & event);
	void OnCollapseAttempt(wxTreeEvent & event);
	static wxString GetProtocolString(wxString proto);

    private:
	static wxOTRDialog *aboutbox;
	wxOTRProxy* proxy;

	DECLARE_EVENT_TABLE()
};

enum{
    MAIN_EXIT,
    MAIN_PREFS,
    MAIN_ABOUT
};

#endif
