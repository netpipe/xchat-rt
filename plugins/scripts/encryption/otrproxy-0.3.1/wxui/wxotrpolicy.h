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

#ifndef __WXOTRPOLICY_H__
#define __WXOTRPOLICY_H__

#include <wx/string.h>
#include <wx/config.h>
#include <wx/treectrl.h>
#include <map>

extern "C" {
#include <context.h>
#include <proto.h>
#include <message.h>
}

class Buddy : public wxTreeItemData {
    public:
	wxString accountname;
	wxString protocol;
	wxString username;
	int Cmp(const Buddy &b) const;
};

inline bool operator< (const Buddy &a, const Buddy &b){
    return (a.Cmp(b) < 0);
}

class wxOTRPolicy {
public:
    static wxOTRPolicy *GetManager();

    void SetGlobal(OtrlPolicy policy);
    void GetGlobal(OtrlPolicy &policy);

    void SetBuddy(const Buddy &b, bool useglobal, OtrlPolicy policy);
    void GetBuddy(const Buddy &b, bool &useglobal, OtrlPolicy &policy);
    void LoadProxySettings(unsigned short &socksport,
	unsigned short &httpport, wxString &username, wxString &password);
    void ApplyProxySettings(unsigned short socksport,
	unsigned short httpport, wxString username, wxString password);
    void SaveProxySettings(unsigned short socksport,
	unsigned short httpport, wxString username, wxString password);

private:
    wxOTRPolicy();

    ~wxOTRPolicy();

    long policy_to_long(OtrlPolicy policy);
    OtrlPolicy long_to_policy(long longval);

    static wxOTRPolicy *otrpolicy;
    static OtrlPolicy GetPolicy(ConnContext *context);

    wxConfig *config;
};

#endif
