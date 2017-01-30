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

#include <wx/event.h>

extern "C" {
#include "uisync.h"
#include "proxyevent.h"
}

#include "wxotrproxy.h"
#include "wxproxyevent.h"

const wxEventType wxEVT_PROXY_EVENT = wxNewEventType();

/* The proxy has sent us an event to handle.  Note that this function
 * will usually be called from the context of the proxy's thread, but
 * may occasionally be called by the ui thread (if the ui thread does
 * uisync_lock(), and then calls functions in the proxy, this function
 * may eventually get called back.  So this function should NEVER call
 * uisync_lock or attempt to call other functions in the proxy. */
void wxproxyevent_handle(ProxyEvent *ev, void *data)
{
    wxOTRProxy *app = (wxOTRProxy *)data;
    wxProxyEvent wev;

    wev.type = ev->type;
    wev.dialogID = ev->dialogid;
    wev.account = wxString(ev->account, wxConvUTF8);
    wev.protocol = wxString(ev->protocol, wxConvUTF8);
    wev.username = wxString(ev->username, wxConvUTF8);
    wev.level = ev->level;
    wev.title = wxString(ev->title, wxConvUTF8);
    wev.primary = wxString(ev->primary, wxConvUTF8);
    wev.secondary = wxString(ev->secondary, wxConvUTF8);
    wev.boldhalf = ev->boldhalf;
    wev.protocol_version = ev->protocol_version;
    if (ev->fingerprint) {
	memmove(wev.fingerprint, ev->fingerprint, 20);
    } else {
	memset(wev.fingerprint, 0, 20);
    }
    if (ev->sessionid) {
	memmove(wev.sessionid, ev->sessionid, 20);
    } else {
	memset(wev.sessionid, 0, 20);
    }
    wev.sessionid_len = ev->sessionid_len;

    app->AddPendingEvent(wev);
}
