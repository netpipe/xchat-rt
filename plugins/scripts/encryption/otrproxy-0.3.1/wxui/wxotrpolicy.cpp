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

#ifndef WIN32
#include <errno.h>
#endif

extern "C" {
#include "otrproxy.h"
#include "servsock.h"
}
#include "wxotrproxy.h"
#include "wxotrpolicy.h"

int Buddy::Cmp(const Buddy &b) const
{
    int acmp = accountname.Cmp(b.accountname);
    if (acmp) return acmp;
    int pcmp = protocol.Cmp(b.protocol);
    if (pcmp) return pcmp;
    return username.Cmp(b.username);
}

wxOTRPolicy *wxOTRPolicy::otrpolicy = NULL;

wxOTRPolicy *wxOTRPolicy::GetManager()
{
    if (wxOTRPolicy::otrpolicy == NULL) {
	wxOTRPolicy::otrpolicy = new wxOTRPolicy();
    }
    return wxOTRPolicy::otrpolicy;
}

long wxOTRPolicy::policy_to_long(OtrlPolicy policy)
{
    long val = 3;
    switch (policy) {
	case OTRL_POLICY_NEVER:
	    val = 1;
	    break;
	case OTRL_POLICY_MANUAL:
	    val = 2;
	    break;
	case OTRL_POLICY_OPPORTUNISTIC:
	    val = 3;
	    break;
	case OTRL_POLICY_ALWAYS:
	    val = 4;
	    break;
    }
    return val;
}

OtrlPolicy wxOTRPolicy::long_to_policy(long longval)
{
    OtrlPolicy p;
    switch (longval) {
	case 1:
	    p = OTRL_POLICY_NEVER;
	    break;
	case 2:
	    p = OTRL_POLICY_MANUAL;
	    break;
	case 3:
	    p = OTRL_POLICY_OPPORTUNISTIC;
	    break;
	case 4:
	    p = OTRL_POLICY_ALWAYS;
	    break;
	default:
	    p = OTRL_POLICY_DEFAULT;
	    break;
    }
    return p;
}

void wxOTRPolicy::SetGlobal(OtrlPolicy policy)
{
    uisync_lock(wxotrproxy_uisync);
    config->Write(wxT("/Global/Policy"), policy_to_long(policy));
    config->Flush();
    uisync_unlock(wxotrproxy_uisync);
}

void wxOTRPolicy::GetGlobal(OtrlPolicy &policy)
{
    policy = long_to_policy(config->Read(wxT("/Global/Policy"),
		    policy_to_long(OTRL_POLICY_DEFAULT)));
}

void wxOTRPolicy::SetBuddy(const Buddy &b, bool useglobal, OtrlPolicy policy)
{
    long storeval = useglobal ? 0 : policy_to_long(policy);
    wxString key = wxT("/Buddy/") + b.protocol + wxT("/") + b.accountname
	+ wxT("/") + b.username + wxT("/Policy");
    uisync_lock(wxotrproxy_uisync);
    config->Write(key, storeval);
    config->Flush();
    uisync_unlock(wxotrproxy_uisync);
}

void wxOTRPolicy::GetBuddy(const Buddy &b, bool &useglobal, OtrlPolicy &policy)
{
    long readval;
    wxString key = wxT("/Buddy/") + b.protocol + wxT("/") + b.accountname
	+ wxT("/") + b.username + wxT("/Policy");
    if (config->Read(key, &readval) && readval) {
	useglobal = false;
	policy = long_to_policy(readval);
    } else {
	// There was no stored policy for this buddy, or the policy was
	// "use global".
	useglobal = true;
	GetGlobal(policy);
    }
}

void wxOTRPolicy::LoadProxySettings(unsigned short &socksport,
	unsigned short &httpport, wxString &username, wxString &password)
{
    long style;
    config->Read(wxT("/Settings/ConfigStyle"), &style, 0);
    if (style == 0) {
	/* There are no settings yet; write the defaults. */
	SaveProxySettings(1080, 8080, wxEmptyString, wxEmptyString);
    }

    long socksportl, httpportl;
    config->Read(wxT("/Settings/SOCKS5_port"), &socksportl, 0);
    if (socksportl < 0 || socksportl > 65535) socksportl = 0;
    socksport = socksportl;
    config->Read(wxT("/Settings/HTTP_port"), &httpportl, 0);
    if (httpportl < 0 || httpportl > 65535) httpportl = 0;
    httpport = httpportl;
    username = config->Read(wxT("/Settings/Username"), wxEmptyString);
    password = config->Read(wxT("/Settings/Password"), wxEmptyString);
}

static wxString reason(int errn)
{
    switch(errn) {
#ifdef WIN32
	case WSAEADDRINUSE:
#else
	case EADDRINUSE:
#endif
	    return wxT("someone else is already using this port number.  "
		    "Please choose a different one.");
#ifdef WIN32
	case WSAEACCES:
#else
	case EACCES:
#endif
	    return wxT("you do not have permission to use this port.  "
		    "Please try one larger than 1024.");
	default:
#ifdef WIN32
	    return wxT("Unknown error ") + errn;
#else
	    return wxString(strerror(errn), wxConvUTF8);
#endif
    }
}

void wxOTRPolicy::ApplyProxySettings(unsigned short socksport,
	unsigned short httpport, wxString username, wxString password)
{
#ifdef DEBUG
    const in_addr_t bindaddr = INADDR_ANY;
#else
    const in_addr_t bindaddr = INADDR_LOOPBACK;
#endif

    int sockserr, httperr;

    uisync_lock(wxotrproxy_uisync);
    servsock_start_simple(bindaddr, socksport, httpport,
	    username.mb_str(wxConvUTF8), password.mb_str(wxConvUTF8),
	    &sockserr, &httperr);
    uisync_unlock(wxotrproxy_uisync);

    // Check for errors
    wxString errmsg = wxEmptyString;
    if (sockserr) {
	errmsg = wxString::Format(wxT("Unable to start SOCKS5 proxy on "
		    "port %d: "), socksport) + reason(sockserr);
    }
    if (httperr) {
	if (sockserr) {
	    errmsg += wxT("\n\n");
	}
	errmsg += wxString::Format(wxT("Unable to start HTTP proxy on "
		    "port %d: "), httpport) + reason(httperr);
    }

    if (sockserr || httperr) {
	wxOTRDialog *errdialog = new wxOTRDialog(0, OTRL_NOTIFY_ERROR,
		wxT("Error starting proxy"), wxEmptyString, errmsg);
	errdialog->Show(TRUE);
    }
}

void wxOTRPolicy::SaveProxySettings(unsigned short socksport,
	unsigned short httpport, wxString username, wxString password)
{
    uisync_lock(wxotrproxy_uisync);
    config->Write(wxT("/Settings/ConfigStyle"), (long) 1);
    config->Write(wxT("/Settings/SOCKS5_port"), (long)socksport);
    config->Write(wxT("/Settings/HTTP_port"), (long)httpport);
    config->Write(wxT("/Settings/Username"), username);
    config->Write(wxT("/Settings/Password"), password);
    config->Flush();
    uisync_unlock(wxotrproxy_uisync);
}

wxOTRPolicy::wxOTRPolicy()
{
    config = new wxConfig(wxT("OTRProxy"));
    otrproxy_set_policy_cb(GetPolicy);
}

wxOTRPolicy::~wxOTRPolicy()
{
    otrproxy_set_policy_cb(NULL);
}

OtrlPolicy wxOTRPolicy::GetPolicy(ConnContext *context)
{
    Buddy b;
    bool useglobal;
    OtrlPolicy p;

    // Make sure there is one
    wxOTRPolicy *policy = wxOTRPolicy::GetManager();

    b.accountname = wxString(context->accountname, wxConvUTF8);
    b.protocol = wxString(context->protocol, wxConvUTF8);
    b.username = wxString(context->username, wxConvUTF8);
    policy->GetBuddy(b, useglobal, p);
    return p;
}
