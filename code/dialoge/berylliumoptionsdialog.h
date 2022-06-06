/***********************************************************************************
   Copyright (C) 2022 by Sven Kochmann

   This file is part of Beryllium¹º.

   Beryllium¹º is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Beryllium¹º is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Beryllium¹º.  If not, see <http://www.gnu.org/licenses/>.
***********************************************************************************/

#pragma once

#include <wx/propdlg.h>
#include <wx/statline.h>
#include <wx/listctrl.h>

// Dialog: Einstellungen
class CBerylliumOptionsDialog : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumOptionsDialog)

public:
	CBerylliumOptionsDialog(wxWindow* parent);
	~CBerylliumOptionsDialog(void);

	// Set: Sprache
	void SetLanguage( long newlang );

	// Get: Sprache
	long GetLanguage() { return m_iLanguage; };

	// Set: Proxy
	void SetProxy( wxString host, wxString port );

	// Get: Proxy
	wxString GetProxyHost() { return proxyhost; };
	wxString GetProxyPort() { return proxyport; };

private:
	// Sprache
	long m_iLanguage;

	// Proxy
	wxString proxyhost;
	wxString proxyport;

	// Elemente
	wxChoice	*m_choiceLanguage;
	wxTextCtrl	*m_textProxyServer;
	wxTextCtrl	*m_textProxyPort;

	// Initialisiert die Seite "Sprache"
	wxPanel *CreatePageLanguage(wxWindow* parent);

	// Initialisiert die Seite "Proxy"
	wxPanel *CreatePageProxy(wxWindow* parent);

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

DECLARE_EVENT_TABLE()
};
