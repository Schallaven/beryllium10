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

#include "BerylliumViewSafetySheetsProperties.h"

#include <wx/bookctrl.h>

IMPLEMENT_CLASS(CBerylliumViewSafetySheetsProperties, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumViewSafetySheetsProperties, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumViewSafetySheetsProperties::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumViewSafetySheetsProperties::OnOK )
END_EVENT_TABLE()

// Erstellt das Fenster...
CBerylliumViewSafetySheetsProperties::CBerylliumViewSafetySheetsProperties(wxWindow* parent, CBerylliumData newdata, int iPage = 0) : data(newdata)
{
	// Zeiger auf Null setzen...
	m_Columnslist = NULL;

	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Eigenschaften von Betriebsanweisung"), wxDefaultPosition, wxSize( 525, 470)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seiten hinzufügen
	notebook->AddPage( CreatePageColumns(notebook), _(L"Spalten"), (iPage == 0));
    notebook->AddPage( CreatePagePhrases(notebook), _(L"Sätze"), (iPage == 1));
    notebook->AddPage( CreatePageSignature(notebook), _(L"Signatur"), (iPage == 2));
}

CBerylliumViewSafetySheetsProperties::~CBerylliumViewSafetySheetsProperties(void)
{
}

wxPanel *CBerylliumViewSafetySheetsProperties::CreatePageColumns(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Markieren Sie die Spalten, die in der Betriebsanweisung angezeigt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	
	
	m_Columnslist = new wxCheckListBox( panel, wxID_ANY, wxDefaultPosition, wxSize( 450, 210), 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	
	bSizer1->Add( m_Columnslist, 0, wxALL|wxALIGN_RIGHT, 10 );	
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumViewSafetySheetsProperties::CreatePagePhrases(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Diese Optionen ermöglichen Ihnen das Aussehen der Risiko- und Sicherheitssätze zu ändern."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	

	// Checkbox
	m_checkshowhpstatements = new wxCheckBox( panel, wxID_ANY, _(L"H-/P-Sätze statt den R-/S-Sätzen anzeigen"), wxDefaultPosition, wxDefaultSize);
	bSizer1->Add( m_checkshowhpstatements, 0, wxLEFT, 30 );

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumViewSafetySheetsProperties::CreatePageSignature(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Stellungnahme und Signatur (links):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	

	m_textStatementStudent = new wxTextCtrl( panel, wxID_ANY, data.szStatementStudent , wxDefaultPosition, wxSize( 450, 60 ), wxTE_MULTILINE );
	bSizer1->Add( m_textStatementStudent, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	m_textSignaturStudent = new wxTextCtrl( panel, wxID_ANY, data.szSignaturStudent , wxDefaultPosition, wxSize( 450, -1 ) );
	bSizer1->Add( m_textSignaturStudent, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Stellungnahme und Signatur (rechts):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 450 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );	

	m_textStatementSupervisor = new wxTextCtrl( panel, wxID_ANY, data.szStatementSupervisor , wxDefaultPosition, wxSize( 450, 60 ), wxTE_MULTILINE );
	bSizer1->Add( m_textStatementSupervisor, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );
	
	m_textSignaturSupervisor = new wxTextCtrl( panel, wxID_ANY, data.szSignaturSupervisor , wxDefaultPosition, wxSize( 450, -1 ) );
	bSizer1->Add( m_textSignaturSupervisor, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumViewSafetySheetsProperties::OnInitDialog( wxInitDialogEvent &event )
{
	// Seite 1: Listbox mit Daten füllen
	// Dazu wird ein Feld von Strings angelegt, die einfach in die Listbox
	// eingefügt werden. Danach wird noch "gecheckt", welche Spalte momentan
	// angezeigt wird.
	if ( m_Columns.size() > 0 )
	{
		wxArrayString strColumns;

		// Namen holen (und alle Umbruchzeichen ersetzen!)
		for ( unsigned int i = 0; i < m_Columns.size(); i++ )
		{
			strColumns.Add( m_Columns[i]->GetColumnTitle() );
			strColumns.Last().Replace( "\n", " " );
		}

		// Liste einfügen
		m_Columnslist->InsertItems( strColumns, 0);

		// Liste durchgehen und markieren, welche Spalte momentan angezeigt wird
		for ( unsigned int i = 0; i < m_Columns.size(); i++ )
			m_Columnslist->Check( i, m_Columns[i]->IsShown() );
	}

	// Weiter im Programm
	event.Skip();
}

void CBerylliumViewSafetySheetsProperties::OnOK( wxCommandEvent &event )
{
	// Seite 1: Spaltendaten abfragen und die Spalten je
	// nach Häkchen zeigen bzw. verstecken
	for ( unsigned int i = 0; i < m_Columns.size(); i++ )
		m_Columns[i]->Show( m_Columnslist->IsChecked(i) );

	// Signaturdaten übernehmen
	data.szStatementStudent		= m_textStatementStudent->GetValue();
	data.szStatementSupervisor	= m_textStatementSupervisor->GetValue();
	data.szSignaturStudent		= m_textSignaturStudent->GetValue();
	data.szSignaturSupervisor	= m_textSignaturSupervisor->GetValue();

	// Weiter im Programm
	event.Skip();
}