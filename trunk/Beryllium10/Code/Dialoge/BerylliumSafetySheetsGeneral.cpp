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

#include "stdafx.h"
#include "BerylliumSafetySheetsGeneral.h"

#include <wx/bookctrl.h>

IMPLEMENT_CLASS(CBerylliumSafetySheetsGeneral, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumSafetySheetsGeneral, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumSafetySheetsGeneral::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumSafetySheetsGeneral::OnOK )
END_EVENT_TABLE()

CBerylliumSafetySheetsGeneral::CBerylliumSafetySheetsGeneral(wxWindow* parent, const CBerylliumData &data)
{
	// Zeiger auf NULL setzen
	m_textTitle			= NULL;
	m_textLocation		= NULL;
	m_textEvent			= NULL;
	m_textName			= NULL;
	m_textPosition		= NULL;
	m_textSupervisor	= NULL;

	// Kopie der Daten erstellen
	m_data = data;

	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Eigenschaften von Betriebsanweisung"), wxDefaultPosition, wxSize( 525, 450)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seiten hinzufügen
	notebook->AddPage( CreatePageGeneral(notebook), _(L"Allgemeines"), true);
	notebook->AddPage( CreatePagePersonal(notebook), _(L"Persönliches"), false);
	notebook->AddPage( CreatePagePreparation(notebook), _(L"Präparat"), false);
}

CBerylliumSafetySheetsGeneral::~CBerylliumSafetySheetsGeneral(void)
{
}

wxPanel *CBerylliumSafetySheetsGeneral::CreatePageGeneral(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Betriebsanweisungstitel:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 460 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	

	m_textTitle = new wxTextCtrl( panel, wxID_ANY, m_data.szTitle, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textTitle, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( panel, wxID_ANY, _(L"Betriebsanweisungstitel (GHS):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 460 );
	bSizer1->Add( m_staticText4, 0, wxALL, 10 );	

	m_textTitleGHS = new wxTextCtrl( panel, wxID_ANY, m_data.szGHSTitle, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textTitleGHS, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Aufenthalts-/Bestimmungsort:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 460 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );	

	m_textLocation = new wxTextCtrl( panel, wxID_ANY, m_data.szLocation, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textLocation, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Veranstaltung:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 460 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );	

	m_textEvent = new wxTextCtrl( panel, wxID_ANY, m_data.szEvent, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textEvent, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSafetySheetsGeneral::CreatePagePersonal(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Name, Vorname:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 460 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	

	m_textName = new wxTextCtrl( panel, wxID_ANY, m_data.GetName(), wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textName, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );	

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Platznummer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 460 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );		

	m_textPosition = new wxTextCtrl( panel, wxID_ANY, m_data.szPosition, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textPosition, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Assistent/Betreuer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 460 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );	

	m_textSupervisor = new wxTextCtrl( panel, wxID_ANY, m_data.szSupervisor, wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textSupervisor, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSafetySheetsGeneral::CreatePagePreparation(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Name der Verbindung:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 460 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	

	m_textPrepName = new wxTextCtrl( panel, wxID_ANY, m_data.sdPreparation.GetName(), wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textPrepName, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );	

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Summenformel der Verbindung:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 460 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );	

	m_textPrepFormula = new wxTextCtrl( panel, wxID_ANY, m_data.sdPreparation.GetFormula(), wxDefaultPosition, wxSize( 450,-1 ), 0 );
	bSizer1->Add( m_textPrepFormula, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );	

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumSafetySheetsGeneral::OnInitDialog( wxInitDialogEvent &event )
{
	// Weiter im Programm
	event.Skip();
}

void CBerylliumSafetySheetsGeneral::OnOK( wxCommandEvent &event )
{
	// Steuerelemente auslesen und in der Kopie des Datenobjektes speichern
	m_data.szTitle		= m_textTitle->GetValue(); 
	m_data.szGHSTitle	= m_textTitleGHS->GetValue();
	m_data.szLocation	= m_textLocation->GetValue();
	m_data.szEvent		= m_textEvent->GetValue();

	wxString szName = m_textName->GetValue();
	m_data.SetName( szName );
	m_data.szPosition	= m_textPosition->GetValue();
	m_data.szSupervisor	= m_textSupervisor->GetValue();

	wxString szPrepName		= m_textPrepName->GetValue();
	wxString szPrepFormula	= m_textPrepFormula->GetValue();
	m_data.sdPreparation.ReplaceNamesBy( szPrepName  );
	m_data.sdPreparation.SetFormula( szPrepFormula );

	// Weiter im Programm
	event.Skip();
}

void CBerylliumSafetySheetsGeneral::SetPage( int iPage )
{ 
	if ( (iPage >= 0) && (iPage < 3) ) 
		GetBookCtrl()->ChangeSelection( iPage ); 
}
