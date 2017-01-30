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

#include <wx/app.h>
#include <wx/thread.h>
#include <wx/filename.h>
#include <vector>
#include <algorithm>

extern "C" {
#ifdef WIN32
#include <winsock2.h>
typedef unsigned int in_addr_t;
#else
#include <netinet/in.h>
#endif

#include <proto.h>
#include <privkey.h>

#include "otrproxy.h"
#include "uisync.h"
#include "accountlist.h"
}

#include "wxotrpolicy.h"
#include "wxotrproxy.h"
#include "wxproxyevent.h"
#include "wxmainframe.h"
#include "wxotrpolicybox.h"

class ProxyThread : public wxThread
{
    virtual ExitCode Entry();
};

ProxyThread::ExitCode ProxyThread::Entry()
{
    otrproxy_mainloop();
    return NULL;
}

BEGIN_EVENT_TABLE(wxOTRProxy, wxApp)
    EVT_PROXY_EVENT(wxID_ANY, wxOTRProxy::HandleProxyEvent)
END_EVENT_TABLE()

IMPLEMENT_APP(wxOTRProxy)

UiSyncHandle wxotrproxy_uisync;

bool wxOTRProxy::OnInit()
{
#ifdef WIN32
    WSADATA wsaData;
#endif
    wxThread *proxythread = new ProxyThread();

    printf("Off-the-Record Messaging Proxy\n");
    printf("Copyright (C) 2004-2005  Nikita Borisov, Ian Goldberg, "
	    "Katrina Hanna\n");
    printf("                         <otr@cypherpunks.ca>\n");
    printf("Proxy version " OTRPROXY_VERSION ", using OTR library version ");
    printf("%s\n\n", otrl_version());
    printf("This program is free software.  See the file COPYING for "
	    "details.\n\n");

#ifdef WIN32
    /* Initialize Winsock2 */
    if (WSAStartup(0x0002, &wsaData)) {
	printf("Error initializing Winsock2.\n");
	exit(1);
    }
    if (wsaData.wVersion != 0x0002) {
	WSACleanup();
	printf("Bad version number initializing Winsock2: %04x\n",
		wsaData.wVersion);
	exit(1);
    }
#endif

    wxFileName userdir(wxGetHomeDir(), wxT(".otrproxy"));

    const wxCharBuffer userdirpath = userdir.GetFullPath().mb_str(wxConvUTF8);
    otrproxy_init(userdirpath);

    printf("OTR Proxy starting.\n");
#ifdef DEBUG
    printf("DEBUG is ON\n");
#endif
#ifdef WIN32
    printf("WIN32 is ON\n");
#endif

    wxotrproxy_uisync = uisync_setup(wxproxyevent_handle, this);

    wxOTRPolicy *manager = wxOTRPolicy::GetManager();
    proxythread->Create();
    proxythread->Run();


    /* Start listening on the sockets */

    unsigned short socksport, httpport;
    wxString username, password;

    manager->LoadProxySettings(socksport, httpport, username, password);
    manager->ApplyProxySettings(socksport, httpport, username, password);

    printf("\n");

    frame = new wxMainFrame(this, wxT("OTR Proxy"), 50, 50, 200, 300);
    frame->Show(TRUE);
    SetTopWindow(frame);

    fflush(stdout);

    return TRUE;
}

void wxOTRProxy::HandleProxyEvent(wxProxyEvent &ev)
{
    switch (ev.type){
    case PROXYEVENT_CONTEXT_STATE:
	HandleContextState();
	break;
    case PROXYEVENT_GONE_SECURE:
	HandleGoneSecure(ev);
	break;
    case PROXYEVENT_STILL_SECURE:
	HandleStillSecure(ev);
	break;
    case PROXYEVENT_GONE_INSECURE:
	HandleGoneInsecure(ev);
	break;
    case PROXYEVENT_GENERIC_DIALOG:
	HandleGenericDialog(ev);
	break;
    case PROXYEVENT_CONFIRM_FINGERPRINT:
	HandleConfirmFingerprint(ev);
	break;
    case PROXYEVENT_GENERATING_PRIVKEY:
	HandleGeneratingPrivkey(ev);
	break;
    case PROXYEVENT_DONE_GENERATING_PRIVKEY:
	HandleDoneGeneratingPrivkey(ev);
	break;
    case PROXYEVENT_PRIVKEY_STATE:
	UpdatePrivKeyBox();
	break;
    case PROXYEVENT_SOCKET_STATE:
	UpdatePrivKeyBox();
	break;
    default:
	break;
    }

}

void wxOTRProxy::HandleContextState(void){

    HandleConnectionState();
    HandleKnownFPs();
    wxOTRPolicyBox::UpdateStartEndButtons();
}

static const char *statestr(ConnContext *context)
{
    switch(context->msgstate) {
	case OTRL_MSGSTATE_PLAINTEXT:
	    return "Not private";
	case OTRL_MSGSTATE_ENCRYPTED:
	    if (context->active_fingerprint->trust &&
		    context->active_fingerprint->trust[0] != '\0') {
		return "Private";
	    } else {
		// Eventually, this should say "Unverified", but right
		// now there's no way to do the verification.
		return "Private";
	    }
	case OTRL_MSGSTATE_FINISHED:
	    return "Finished";
    }
}

void wxOTRProxy::HandleKnownFPs(){

    wxPrefFrame *prefframe = wxPrefFrame::prefframe;
    if(prefframe == NULL) return;
    prefframe->fplist->DeleteAllItems();
    uisync_lock(wxotrproxy_uisync);
    ConnContext* cur = otrproxy_userstate->context_root;
    while(cur != NULL){
	long row = 0;
	wxString cur_username;
	wxString acct_proto_str;
	Fingerprint* fp = cur->fingerprint_root.next;
	while(fp != NULL){
	    cur_username = wxString(cur->username, wxConvUTF7);
	    row = prefframe->fplist->InsertItem(row, cur_username);
	    if(row != -1){
		prefframe->fplist->SetItemData(row, (long)fp);
		if((cur->msgstate == OTRL_MSGSTATE_ENCRYPTED) &&
		    (fp != cur->active_fingerprint)){
		    prefframe->fplist->SetItem(row, 1, wxT("Unused"));
		} else {
		    prefframe->fplist->SetItem(row, 1,
			wxString(statestr(cur), wxConvUTF8));
		}
		char fpstr[45];
		otrl_privkey_hash_to_human(fpstr, fp->fingerprint);
		prefframe->fplist->SetItem(row, 2, wxString(fpstr,
			wxConvUTF8));
		acct_proto_str = wxString(cur->accountname, wxConvUTF8) +
		    wxT(" ") +
		    wxMainFrame::GetProtocolString(wxString(cur->protocol,
			    wxConvUTF8));
		prefframe->fplist->SetItem(row, 3, acct_proto_str);
		if(prefframe->selectedfp == fp){
		    prefframe->fplist->SetItemState(row, wxLIST_STATE_SELECTED,
			    wxLIST_STATE_SELECTED);
		}
	    }
	    fp = fp->next;
	}
    cur = cur->next;
    }
    uisync_unlock(wxotrproxy_uisync);
    prefframe->DoSort();
}

void wxOTRProxy::HandleConnectionState(){

    std::vector<Buddy> convect;
    std::vector<Buddy> nonconvect;
    uisync_lock(wxotrproxy_uisync);
    ConnContext* cur = otrproxy_userstate->context_root;
    while(cur != NULL){
	Buddy curcon;
	curcon.accountname = wxString(cur->accountname, wxConvUTF8);
	curcon.protocol = wxString(cur->protocol, wxConvUTF8);
	curcon.username = wxString(cur->username, wxConvUTF8);
	if(cur->msgstate == OTRL_MSGSTATE_PLAINTEXT){
	    nonconvect.push_back(curcon);
	} else {
	    convect.push_back(curcon);
	}
	cur = cur->next;
    }
    uisync_unlock(wxotrproxy_uisync);
    std::sort(convect.begin(), convect.end());
    std::sort(nonconvect.begin(), nonconvect.end());
    frame->DisplayState(convect, nonconvect);
}

void wxOTRProxy::HandleGoneSecure(wxProxyEvent &ev){

    char fp[45];

    // Make a human-readable version of the fingerprint.
    otrl_privkey_hash_to_human(fp, ev.fingerprint);

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, OTRL_NOTIFY_INFO,
	    wxT("Private Connection Established"), wxT("Private connection"
	    " with ") + ev.username + wxT(" established."),
	    (ev.protocol_version == 1 ?
		wxT("*Warning*: using old protocol version 1.\n\n"
		    "Fingerprint for ") : wxT("Fingerprint for ")) +
	    ev.username + wxT(":\n") +
	    wxString(fp, wxConvUTF8) +
	    wxT("\n\nSecure id for this session:"));
    dialog->AddSessionID(ev.boldhalf, ev.sessionid, ev.sessionid_len);
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleStillSecure(wxProxyEvent &ev){

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, OTRL_NOTIFY_INFO,
	    wxT("Refreshed Private Connection"),
	    wxT("Successfully refreshed private connection with ") +
	    ev.username + wxT("."),
	    ev.protocol_version == 1 ?
		wxT("*Warning*: using old protocol version 1.") : wxT(""));
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleGoneInsecure(wxProxyEvent &ev){

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, OTRL_NOTIFY_WARNING,
	    wxT("Private Connection Lost"),
	    wxT("Private connection with ") +
	    ev.username + wxT(" lost."), wxT(""));
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleGenericDialog(wxProxyEvent &ev){

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, ev.level,
	    ev.title, ev.primary, ev.secondary);
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleConfirmFingerprint(wxProxyEvent &ev){

    char fp[45];
    otrl_privkey_hash_to_human(fp, ev.fingerprint);

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, OTRL_NOTIFY_WARNING,
	    wxT("Unknown Fingerprint"),
	    ev.account + wxT(" ") + frame->GetProtocolString(ev.protocol) +
	    wxT(" has received an unknown fingerprint from ") +
	    ev.username + wxT(":"), wxString(fp, wxConvUTF8));
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleGeneratingPrivkey(wxProxyEvent &ev){

    wxOTRDialog* dialog;
    dialog = new wxOTRDialog(ev.dialogID, OTRL_NOTIFY_INFO,
	    wxT("Generating Private Key"),
	    wxT("Please wait."),
	    wxT("Generating private key for ") + ev.account + wxT(" ") +
	    frame->GetProtocolString(ev.protocol) + wxT(" ..."),
	    0);
    dialog->Show(TRUE);
}

void wxOTRProxy::HandleDoneGeneratingPrivkey(wxProxyEvent &ev){

    wxOTRDialog::AppendSecondary(ev.dialogID, wxT(" Done."));
    wxOTRDialog::EnableOK(ev.dialogID, TRUE);
}

void wxOTRProxy::UpdatePrivKeyBox(){

    wxPrefFrame* prefframe = wxPrefFrame::prefframe;
    if(prefframe == NULL) return;
    wxString selected_item = prefframe->account_menu->GetStringSelection();
    prefframe->account_menu->Clear();
    accountlist_free(prefframe->account_list);
    uisync_lock(wxotrproxy_uisync);
    prefframe->account_list = accountlist_generate();
    uisync_unlock(wxotrproxy_uisync);
    if(prefframe->account_list != NULL){
	AccountList cur = prefframe->account_list;
	int selected_row = 0;
	// populate the drop down menu
	while(cur != NULL){
	    wxString account_proto = wxString(cur->accountname, wxConvUTF8) +
		wxT(" ") +
		frame->GetProtocolString(wxString(cur->protocol, wxConvUTF8));
	    prefframe->account_menu->Append(account_proto, cur);
	    if(account_proto == selected_item){
		selected_row = (prefframe->account_menu->GetCount() - 1);
	    }
	    cur = cur->next;
	}
	// select currently slected item
	prefframe->account_menu->SetSelection(selected_row);
	AccountList account = (AccountList)prefframe->account_menu->GetClientData(selected_row);
	if(account->fingerprint_text != NULL){
	    prefframe->account_fp->SetLabel(wxT("Fingerprint:  ") +
		    wxString(account->fingerprint_text, wxConvUTF8));
	    prefframe->otrprefs->Layout();
	    prefframe->generate->Disable();
	    prefframe->copy_fp->Enable();
	} else {
	    prefframe->account_fp->SetLabel(wxT("No fingerprint"));
	    prefframe->otrprefs->Layout();
	    prefframe->generate->Enable();
	    prefframe->copy_fp->Disable();
	}
    } else {
	prefframe->generate->Disable();
	prefframe->copy_fp->Disable();
    }
}

