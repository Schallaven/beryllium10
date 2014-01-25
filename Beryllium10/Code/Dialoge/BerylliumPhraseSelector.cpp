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
#include "BerylliumPhraseSelector.h"

#include <wx/bookctrl.h>

IMPLEMENT_CLASS(CBerylliumPhraseSelector, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumPhraseSelector, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumPhraseSelector::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumPhraseSelector::OnOK )
END_EVENT_TABLE()

// Erstellt das Fenster...
CBerylliumPhraseSelector::CBerylliumPhraseSelector(wxWindow* parent, int iPage = 0)
{
	// Listen NULL setzen
	m_listRisks = NULL;
	m_listSafeties = NULL;
	m_listHarzards = NULL;
	m_listPrecautionaries = NULL;

	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Sätze verstecken"), wxDefaultPosition, wxSize( 525, 450)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seiten hinzufügen
	notebook->AddPage( CreatePageRisks(notebook), _(L"R-Sätze"), (iPage == 0));
    notebook->AddPage( CreatePageSafeties(notebook), _(L"S-Sätze"), (iPage == 1));
    notebook->AddPage( CreatePageHarzards(notebook), _(L"H-Sätze"), (iPage == 2));
	notebook->AddPage( CreatePagePrecautionaries(notebook), _(L"P-Sätze"), (iPage == 3));
}

CBerylliumPhraseSelector::~CBerylliumPhraseSelector(void)
{
}

wxPanel *CBerylliumPhraseSelector::CreatePageRisks(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Markieren Sie die R-Sätze, die in der Betriebsanweisung angezeigt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	
	
	m_listRisks = new wxCheckListBox( panel, wxID_ANY, wxDefaultPosition, wxSize( 450, 290), 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	
	bSizer1->Add( m_listRisks, 1, wxALL|wxALIGN_RIGHT, 10 );
	bSizer1->AddSpacer(10);
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumPhraseSelector::CreatePageSafeties(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Markieren Sie die S-Sätze, die in der Betriebsanweisung angezeigt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	
	
	m_listSafeties = new wxCheckListBox( panel, wxID_ANY, wxDefaultPosition, wxSize( 450, 290), 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	
	bSizer1->Add( m_listSafeties, 1, wxALL|wxALIGN_RIGHT, 10 );	
	bSizer1->AddSpacer(10);
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumPhraseSelector::CreatePageHarzards(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Markieren Sie die H-Sätze, die in der Betriebsanweisung angezeigt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	
	
	m_listHarzards = new wxCheckListBox( panel, wxID_ANY, wxDefaultPosition, wxSize( 450, 290), 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	
	bSizer1->Add( m_listHarzards, 1, wxALL|wxALIGN_RIGHT, 10 );	
	bSizer1->AddSpacer(10);
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumPhraseSelector::CreatePagePrecautionaries(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Markieren Sie die P-Sätze, die in der Betriebsanweisung angezeigt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 450 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );	
	
	m_listPrecautionaries = new wxCheckListBox( panel, wxID_ANY, wxDefaultPosition, wxSize( 450, 290), 0, NULL, wxLB_NEEDED_SB|wxLB_SINGLE );
	
	bSizer1->Add( m_listPrecautionaries, 1, wxALL|wxALIGN_RIGHT, 10 );	
	bSizer1->AddSpacer(10);
	
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumPhraseSelector::OnInitDialog( wxInitDialogEvent &event )
{
	// Seite 1: R-Sätze
	InsertListInto( m_listRisks, m_allphrases_r, "R" );

	// Seite 2: S-Sätze
	InsertListInto( m_listSafeties, m_allphrases_s, "S" );

	// Seite 3: H-Sätze
	InsertListInto( m_listHarzards, m_allphrases_h, "H" );

	// Seite 4: P-Sätze
	InsertListInto( m_listPrecautionaries, m_allphrases_p, "P" );

	// Weiter im Programm
	event.Skip();
}

void CBerylliumPhraseSelector::OnOK( wxCommandEvent &event )
{
	// HidePhrases löschen
	m_hidephrases.clear();

	// Seite 1: R-Sätze
	HidePhrasesOfList( m_listRisks, "R" );

	// Seite 2: S-Sätze
	HidePhrasesOfList( m_listSafeties, "S" );

	// Seite 3: H-Sätze
	HidePhrasesOfList( m_listHarzards, "H" );

	// Seite 4: P-Sätze
	HidePhrasesOfList( m_listPrecautionaries, "P" );


	// Weiter im Programm
	event.Skip();
}

// Hilfsfunktion: Fügt die Liste in das entsprechende Steuerelement ein
void CBerylliumPhraseSelector::InsertListInto( wxCheckListBox *m_listcontrol, std::vector< wxString > &phrases, wxString prefix )
{
	// Array vorbereiten
	wxArrayString stringlist;

	// Sätze einfügen
	for ( unsigned int i = 0; i < phrases.size(); ++i )
	{
		// Schon vorhanden?
		if ( std::find( stringlist.begin(), stringlist.end(), phrases[i] ) != stringlist.end() )
			continue;

		// Einfügen
		stringlist.Add( phrases[i] );
	}

	// Sortieren
	stringlist.Sort();

	// Einfügen
	if ( !stringlist.IsEmpty() )
	{
		// In das Steuerelement einfügen
		m_listcontrol->InsertItems( stringlist, 0);

		// Jetzt alle Elemente nochmals durchgehen
		for ( unsigned int i = 0; i < stringlist.size(); ++i )
		{
			// Soll versteckt werden?
			if ( std::find( m_hidephrases.begin(), m_hidephrases.end(), prefix + stringlist[i]) != m_hidephrases.end() )
				continue;
			
			// Ansonsten checken
			m_listcontrol->Check(i, true);
		}
	}
}

// Hilfsfunktion: Fügt die Sätze aus dem Steuerelement in hidephrases ein
void CBerylliumPhraseSelector::HidePhrasesOfList( wxCheckListBox *m_listcontrol, wxString prefix )
{
	// Anzahl der Elemente
	unsigned int count = m_listcontrol->GetCount();

	// Elemente durchgehen
	for ( unsigned int i = 0; i < count; ++i )
	{
		// Ist das Element NICHT mit einem Häkchen versehen? -> Verstecken!
		if ( !m_listcontrol->IsChecked( i ) )
			m_hidephrases.push_back( wxString(prefix + m_listcontrol->GetString(i)).ToStdString() );
	}
}