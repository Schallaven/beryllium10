// **********************************************************************************
//   This file is part of Beryllium¹º.
//
//   Beryllium¹º is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   Beryllium¹º is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with Beryllium¹º.  If not, see <http://www.gnu.org/licenses/>.
//
//   Diese Datei ist Teil von Beryllium¹º.
//
//   Beryllium¹º ist Freie Software: Sie können es unter den Bedingungen
//   der GNU General Public License, wie von der Free Software Foundation,
//   Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren
//   veröffentlichten Version, weiterverbreiten und/oder modifizieren.
//
//   Beryllium¹º wird in der Hoffnung, dass es nützlich sein wird, aber
//   OHNE JEDE GEWÄHELEISTUNG, bereitgestellt; sogar ohne die implizite
//   Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
//   Siehe die GNU General Public License für weitere Details.
//
//   Sie sollten eine Kopie der GNU General Public License zusammen mit
//   Beryllium¹º erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.
// **********************************************************************************

#include "../stdafx.h"
#include "berylliumoptionsdialog.h"

#include <wx/bookctrl.h>
#include <wx/filesys.h>
#include <wx/wfstream.h>

IMPLEMENT_CLASS(CBerylliumOptionsDialog, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumOptionsDialog, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumOptionsDialog::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumOptionsDialog::OnOK )
END_EVENT_TABLE()

// Erstellt das Fenster...
CBerylliumOptionsDialog::CBerylliumOptionsDialog(wxWindow* parent)
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Einstellungen"), wxDefaultPosition, wxSize( 480, 500)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// Sprache setzen
	m_iLanguage = 0;

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seiten hinzufügen
	notebook->AddPage( CreatePageLanguage(notebook), _(L"Sprache"));
	notebook->AddPage( CreatePageProxy(notebook), _(L"Proxy"));

	// Seite auswählen
	notebook->SetSelection( 0 );
}

CBerylliumOptionsDialog::~CBerylliumOptionsDialog(void)
{
}

wxPanel *CBerylliumOptionsDialog::CreatePageLanguage(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Benutzersprache von Beryllium:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	// Sprache
	m_choiceLanguage = new wxChoice( panel, wxID_ANY, wxDefaultPosition, wxSize( 410,-1 ) );

	// Sprachen einfügen
	m_choiceLanguage->Append( _(L"Deutsch"), (void*)wxLANGUAGE_GERMAN);
	m_choiceLanguage->Append( _(L"Englisch"), (void*)wxLANGUAGE_ENGLISH);

	bSizer1->Add( m_choiceLanguage, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Die Sprache wird erst nach einem Neustart von Beryllium geändert.\nYou have to restart Beryllium to switch to the selected language."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 410 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

// Initialisiert die Seite "Proxy"
wxPanel *CBerylliumOptionsDialog::CreatePageProxy(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Sie können hier einen Proxy-Server eintragen, über den die HTTP-Anfragen für die Substanzsuche geleitet werden. Lassen Sie die Eingabefelder frei, um keinen Proxy-Server zu benutzen."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Server:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 410 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );

	wxBoxSizer* bSizerSub1;	bSizerSub1 = new wxBoxSizer( wxHORIZONTAL );
	bSizerSub1->SetMinSize( wxSize( 410,-1 ) );

	m_textProxyServer = new wxTextCtrl( panel, wxID_ANY, "", wxDefaultPosition, wxSize( 210,-1 ), 0 );
	bSizerSub1->Add( m_textProxyServer, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	bSizerSub1->Add( 200, 0, 1, wxALIGN_RIGHT|wxEXPAND, 0 );
	bSizer1->Add( bSizerSub1, 0, wxALIGN_RIGHT, 0 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 410 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );

	wxBoxSizer* bSizerSub2;	bSizerSub2 = new wxBoxSizer( wxHORIZONTAL );
	bSizerSub2->SetMinSize( wxSize( 410,-1 ) );

	m_textProxyPort = new wxTextCtrl( panel, wxID_ANY, "", wxDefaultPosition, wxSize( 120,-1 ), 0 );
	bSizerSub2->Add( m_textProxyPort, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	bSizerSub2->Add( 290, 0, 1, wxALIGN_RIGHT|wxEXPAND, 0 );
	bSizer1->Add( bSizerSub2, 0, wxALIGN_RIGHT, 0 );

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumOptionsDialog::OnInitDialog( wxInitDialogEvent &event )
{
	// Weiter im Programm
	event.Skip();
}

void CBerylliumOptionsDialog::OnOK( wxCommandEvent &event )
{
	// Sprache zurückgeben
	m_iLanguage = (long)m_choiceLanguage->GetClientData( m_choiceLanguage->GetSelection() );

	// Proxy zurückgeben
	proxyhost = m_textProxyServer->GetValue();
	proxyport = m_textProxyPort->GetValue();

	// Ende: Weiter im Programm
	// ------------------------
	event.Skip();
}

void CBerylliumOptionsDialog::SetLanguage( long newlang )
{
	// Ausgewählte Sprache auswählen
	int n = m_choiceLanguage->GetCount();

	for ( int i = 0; i < n; ++i )
	{
		if ( (long)m_choiceLanguage->GetClientData(i) == newlang )
		{
			// Auswählen
			m_choiceLanguage->SetSelection( i );

			// Setzen
			m_iLanguage = newlang;

			// Raus hier
			break;
		}
	}

}

// Set: Proxy
void CBerylliumOptionsDialog::SetProxy( wxString host, wxString port )
{
	// Eigenschaften setzen
	proxyport = port; m_textProxyPort->SetValue( port );
	proxyhost = host; m_textProxyServer->SetValue( host );
}

