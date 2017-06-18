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

#ifndef __WXOTRPROXY_H__
#define __WXOTRPROXY_H__

#include <wx/app.h>

extern "C" {
#include "uisync.h"
}
#include "wxproxyevent.h"
#include "wxmainframe.h"
#include "wxotrdialog.h"
#include "wxprefframe.h"

class wxOTRProxy : public wxApp
{
    public:
	void HandleKnownFPs();
	void UpdatePrivKeyBox();

    private:
	virtual bool OnInit();
	void HandleProxyEvent(wxProxyEvent &ev);
	void HandleContextState(void);
	void HandleGoneSecure(wxProxyEvent &ev);
	void HandleStillSecure(wxProxyEvent &ev);
	void HandleGoneInsecure(wxProxyEvent &ev);
	void HandleGenericDialog(wxProxyEvent &ev);
	void HandleConfirmFingerprint(wxProxyEvent &ev);
	void HandleGeneratingPrivkey(wxProxyEvent &ev);
	void HandleDoneGeneratingPrivkey(wxProxyEvent &ev);
	void HandleConnectionState();
	wxMainFrame* frame;

private : 
    DECLARE_EVENT_TABLE()
};

extern UiSyncHandle wxotrproxy_uisync;

#endif
