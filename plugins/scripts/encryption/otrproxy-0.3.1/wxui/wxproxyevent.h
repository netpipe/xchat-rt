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

#ifndef __EVENTS_H__
#define __EVENTS_H__

#include <wx/event.h>
#include <wx/string.h>

extern "C" {
#include <gcrypt.h>
#include <dh.h>

#include "proxyevent.h"
}

extern const wxEventType wxEVT_PROXY_EVENT;

class wxProxyEvent : public wxEvent
{
public:
    wxProxyEvent(int id = 0): wxEvent(id, wxEVT_PROXY_EVENT) { }
    wxProxyEvent(const wxProxyEvent& event): wxEvent(event) {
	this->type = event.type;
	this->dialogID = event.dialogID;
	this->account = event.account;
	this->protocol = event.protocol;
	this->username = event.username;
	this->level = event.level;
	this->title = event.title;
	this->primary = event.primary;
	this->secondary = event.secondary;
	this->protocol_version = event.protocol_version;
	memmove(this->fingerprint, event.fingerprint, 20);
	memmove(this->sessionid, event.sessionid, 20);
	this->sessionid_len = event.sessionid_len;
	this->boldhalf = event.boldhalf;
    }

    virtual wxEvent *Clone() const { return new wxProxyEvent(*this); }

    ProxyEventType type;
    unsigned int dialogID;
    wxString account, protocol, username;
    OtrlNotifyLevel level;
    wxString title, primary, secondary;
    unsigned char fingerprint[20], sessionid[20];
    size_t sessionid_len;
    OtrlSessionIdHalf boldhalf;
    int protocol_version;

};

typedef void (wxEvtHandler::*wxProxyEventFunction)(wxProxyEvent&);

#define EVT_PROXY_EVENT(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY(  \
	wxEVT_PROXY_EVENT, id, -1,  \
	(wxObjectEventFunction)(wxEventFunction)(wxProxyEventFunction)&fn,  \
	(wxObject *) NULL  \
    ),

/* The proxy has sent us an event to handle.  Note that this function
 * will usually be called from the context of the proxy's thread, but
 * may occasionally be called by the ui thread (if the ui thread does
 * uisync_lock(), and then calls functions in the proxy, this function
 * may eventually get called back.  So this function should NEVER call
 * uisync_lock or attempt to call other functions in the proxy. */
void wxproxyevent_handle(ProxyEvent *ev, void *data);

#endif
