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

#include "../beryllium.h"
#include "berylliumsubstanceproperties.h"

#include <wx/bookctrl.h>
#include <wx/filesys.h>
#include <wx/wfstream.h>

// Ist nur für Windows interessant. Wird für einen kleinen BugFix (siehe weiter unten) genutzt.
#ifdef WIN32
	#include <commctrl.h>
#endif

#include "berylliumstatementeditor.h"
#include "berylliumlethaldialog.h"

IMPLEMENT_CLASS(CBerylliumSubstanceProperties, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumSubstanceProperties, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumSubstanceProperties::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumSubstanceProperties::OnOK )

	EVT_CHECKBOX(beID_DECOMPOSITION, CBerylliumSubstanceProperties::OnDecomposition)
	EVT_CHECKBOX(beID_MAKPPM, CBerylliumSubstanceProperties::OnMAKppm)

	EVT_RADIOBUTTON(beID_BATCHMASS, CBerylliumSubstanceProperties::OnBatch)
	EVT_RADIOBUTTON(beID_BATCHVOLUME, CBerylliumSubstanceProperties::OnBatch)
	EVT_RADIOBUTTON(beID_BATCHSOLUTION, CBerylliumSubstanceProperties::OnBatch)

	EVT_BUTTON( beID_CUSTOMDANGERS, CBerylliumSubstanceProperties::OnStatements )
	EVT_BUTTON( beID_CUSTOMPRECAUTIONS, CBerylliumSubstanceProperties::OnStatements )
	EVT_BUTTON( beID_CUSTOMFIRSTAID, CBerylliumSubstanceProperties::OnStatements )
	EVT_BUTTON( beID_CUSTOMDISPOSAL, CBerylliumSubstanceProperties::OnStatements )

	EVT_TEXT(beID_BATCHTEXTMASS, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_TEXT(beID_BATCHTEXTVOL, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_TEXT(beID_BATCHTEXTCONC, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_TEXT(beID_BATCHTEXTSVOL, CBerylliumSubstanceProperties::OnBatchTextChange)

	EVT_CHOICE(beID_BATCHUNITMASS, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_CHOICE(beID_BATCHUNITVOL, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_CHOICE(beID_BATCHUNITCONC, CBerylliumSubstanceProperties::OnBatchTextChange)
	EVT_CHOICE(beID_BATCHUNITSVOL, CBerylliumSubstanceProperties::OnBatchTextChange)

	EVT_LIST_ITEM_ACTIVATED(beID_TOXY_LD50, CBerylliumSubstanceProperties::OnLD50Activate)
	EVT_LIST_KEY_DOWN(beID_TOXY_LD50, CBerylliumSubstanceProperties::OnLD50KeyDown)
END_EVENT_TABLE()

// Erstellt das Fenster...
CBerylliumSubstanceProperties::CBerylliumSubstanceProperties(wxWindow* parent, CBerylliumSubstanceData newdata, int page = 0)
{
	// Daten übernehmen
	data = newdata;

	// LD50-Daten holen
	data.GetLD50Data( m_ld50values );

	// Fenstertitel
	wxString szTitle = _(L"Eigenschaften von ") + data.GetName();

	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, szTitle, wxDefaultPosition, wxSize( 480, 500)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seite?
	if ( (page > 6) || (page < 0) )
		page = 0;

	// Seiten hinzufügen
	notebook->AddPage( CreatePageGeneral(notebook), _(L"Allgemein"));
	notebook->AddPage( CreatePagePhysical(notebook), _(L"Eigenschaften"));
	notebook->AddPage( CreatePageSafety(notebook), _(L"Sicherheit"));
	notebook->AddPage( CreatePageGHS(notebook), _(L"GHS"));
	notebook->AddPage( CreatePageToxicology(notebook), _(L"Toxikologie"));
	notebook->AddPage( CreatePageBatch(notebook), _(L"Ansatz"));
	notebook->AddPage( CreatePageAdvices(notebook), _(L"Hinweise"));

	// Diese Seite wird nur angezeigt, wenn zumindest Provider und Url angegeben wurden
	if ( (data.source.provider.length() > 0) && (data.source.url.length() > 0) )
		notebook->AddPage( CreatePageSource(notebook), _(L"Quelle"));

	// Seite auswählen
	notebook->SetSelection( page );

#ifdef WIN32
	// Bugfix Windows: Tabcontrol "scrollen"
	::SendMessage( notebook->GetHWND(), WM_HSCROLL, MAKEWPARAM(SB_THUMBPOSITION, ( page > 3 ? 1 : 0) ), (LPARAM)::FindWindowEx( notebook->GetHWND(), 0, _T("msctls_updown32"), _T("") ));
#endif

	// Berechnen der Ansatzseite
	CalculateBatchPage();
}

CBerylliumSubstanceProperties::~CBerylliumSubstanceProperties(void)
{
}

wxPanel *CBerylliumSubstanceProperties::CreatePageGeneral(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Name(n) der Substanz:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	m_textNames = new wxTextCtrl( panel, wxID_ANY, data.GetNames(), wxDefaultPosition, wxSize( 410,-1 ), 0 );
	bSizer1->Add( m_textNames, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"Summenformel:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 410 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );

	m_textFormula = new wxTextCtrl( panel, wxID_ANY, data.GetFormula(), wxDefaultPosition, wxSize( 410,-1 ), 0 );
	bSizer1->Add( m_textFormula, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Molekulargewicht in g/mol:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 410 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );

	wxBoxSizer* bSizerSub1;	bSizerSub1 = new wxBoxSizer( wxHORIZONTAL );
	bSizerSub1->SetMinSize( wxSize( 410,-1 ) );

	m_textMolWeight = new wxTextCtrl( panel, wxID_ANY, wxString::Format("%.2f", data.dbMolWeight), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	bSizerSub1->Add( m_textMolWeight, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	bSizerSub1->Add( 290, 0, 1, wxALIGN_RIGHT|wxEXPAND, 0 );

	bSizer1->Add( bSizerSub1, 0, wxALIGN_RIGHT, 0 );

	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( panel, wxID_ANY, _(L"CAS-Nummer:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 410 );
	bSizer1->Add( m_staticText4, 0, wxALL, 10 );

	wxBoxSizer* bSizerSub2;	bSizerSub2 = new wxBoxSizer( wxHORIZONTAL );
	bSizerSub2->SetMinSize( wxSize( 410,-1 ) );

	m_textCAS = new wxTextCtrl( panel, wxID_ANY, data.szCAS, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	bSizerSub2->Add( m_textCAS, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	bSizerSub2->Add( 290, 0, 1, wxALIGN_RIGHT|wxEXPAND, 0 );

	bSizer1->Add( bSizerSub2, 0, wxALIGN_RIGHT, 0 );


	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePagePhysical(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 0, 2, 10, 20 );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Schmelzpunkt:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	gSizer1->Add( m_staticText1, 0, wxLEFT|wxTOP, 10 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	m_textMeltingPoint = new wxTextCtrl( panel, wxID_ANY, data.szMeltingPoint, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer1->Add( m_textMeltingPoint, 0, wxLEFT|wxALIGN_LEFT, 30 );

	m_checkDecomposition = new wxCheckBox( panel, beID_DECOMPOSITION, _(L"unter Zersetzung"), wxDefaultPosition, wxSize( 150, -1));
	gSizer1->Add( m_checkDecomposition, 0, wxLEFT|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );

	m_staticBoilingPoint = new wxStaticText( panel, wxID_ANY, _(L"Siedepunkt:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticBoilingPoint->Wrap( 205 );
	gSizer1->Add( m_staticBoilingPoint, 0, wxLEFT|wxTOP, 10 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	m_textBoilingPoint = new wxTextCtrl( panel, wxID_ANY,  data.szBoilingPoint, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer1->Add( m_textBoilingPoint, 0, wxLEFT|wxALIGN_LEFT, 30 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Flammpunkt:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 205 );
	gSizer1->Add( m_staticText3, 0, wxLEFT|wxTOP, 10 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	m_textFlashPoint = new wxTextCtrl( panel, wxID_ANY,  data.szFlashPoint, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer1->Add( m_textFlashPoint, 0, wxLEFT|wxALIGN_LEFT, 30 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	// Unter Zersetzung markieren
	if ( data.bDecomposition )
	{
		// Checkbox aktivieren
		m_checkDecomposition->SetValue( true );

		// Siedepunkt disablen
		m_staticBoilingPoint->Disable();
		m_textBoilingPoint->Disable();
	}

	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( panel, wxID_ANY, _(L"Dichte in g/cm³:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 410 );
	gSizer1->Add( m_staticText4, 0, wxLEFT|wxTOP, 10 );

	gSizer1->Add( 0, 0, 1, wxEXPAND, 10 );

	m_textDensity = new wxTextCtrl( panel, wxID_ANY,  wxString::Format("%.3f", data.dbDensity), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer1->Add( m_textDensity, 0, wxLEFT|wxALIGN_LEFT, 30 );

	panel->SetSizer( gSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePageSafety(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Gefahrensymbole:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	m_textHarzards = new wxTextCtrl( panel, wxID_ANY, data.GetHarzards() , wxDefaultPosition, wxSize( 410,-1 ), 0 );
	bSizer1->Add( m_textHarzards, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"R-Sätze:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 410 );
	bSizer1->Add( m_staticText2, 0, wxALL, 10 );

	m_textRisks = new wxTextCtrl( panel, wxID_ANY, data.szRisks, wxDefaultPosition, wxSize( 410,-1 ), 0 );
	bSizer1->Add( m_textRisks, 0,wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"S-Sätze:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 410 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );

	m_textSafeties = new wxTextCtrl( panel, wxID_ANY, data.szSafeties, wxDefaultPosition, wxSize( 410,-1 ), 0 );
	bSizer1->Add( m_textSafeties, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	// Abschließen
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePageGHS(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1 = new wxBoxSizer( wxVERTICAL );

	// GHS-Gefahrensymbole
	// -------------------
	wxPanel* panelSymbols = new wxPanel( panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER );
	panelSymbols->SetBackgroundColour( wxColour(255,255,255) );

	// Box-Sizer erstellen
	wxBoxSizer* bSizerSymbols = new wxBoxSizer( wxVERTICAL );

	// Flexibles Grid erstellen
	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 0, 6, 5, 10 );

	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Erstelle die 9 Symbole, plus die entsprechenden Checkboxen
	// 3 Zeilen á 3 Symbole
	for ( unsigned int line = 0; line < 3; ++line )
	{
		// 3 Symbole pro Zeile
		for ( unsigned int column = 0; column < 3; ++column )
		{
			// Index
			const int index = line * 3 + column;

			// Datei-Stream laden
			wxFSFile *file = filesystem->OpenFile("memory:org_ghs" + wxString::Format("%02d", index + 1 ) + ".png");

			// Datei konnte nicht geladen werden
			if ( file == NULL )
				continue;

			// ein Image-Objekt anlegen
			wxImage img( *file->GetStream(), wxBITMAP_TYPE_PNG  );

			// Skalieren
			img.Rescale( 50, 50, wxIMAGE_QUALITY_HIGH );

			// Bild nicht ok?
			if ( !img.IsOk() )
				continue;

			// daraus ein Bitmap machen
			wxBitmap bmp( img );

			// Checkbox einfügen
			m_GHSSymbols[index] = new wxCheckBox( panelSymbols, wxID_ANY, "", wxDefaultPosition, wxSize( -1, -1));
			gSizer1->Add(m_GHSSymbols[index], 0, wxLEFT | wxALIGN_CENTER, 5);

			// Symbol
			wxString sSymbol = wxString::Format("%02d", index + 1 );

			// Symbol vorhanden? Dann Checkbox aktivieren!
			if ( data.HasGHSSymbol( sSymbol ) )
				m_GHSSymbols[index]->SetValue(true);

			// Bild als StaticBitmap einfügen
			wxStaticBitmap *m_statbitmap = new wxStaticBitmap( panelSymbols, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, 0 );
			gSizer1->Add( m_statbitmap, 0, wxRIGHT, 15 );

			// Datei löschen
			delete file;
		}
	}

	// Pointer zum Dateisystem löschen
	delete filesystem;

	// Grid hinzufügen
	bSizerSymbols->Add( gSizer1, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 10 );

	// SubPanel setzen
	panelSymbols->SetSizer( bSizerSymbols );
	panelSymbols->Layout();

	// SubPanel hinzufügen
	bSizerSymbols->Fit( panelSymbols );
	//bSizer1->Add( panelSymbols, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 5 );

	// "Hauptgitter" anlegen
	// --------------------------
	wxFlexGridSizer* gSizer2;
	gSizer2 = new wxFlexGridSizer( 0, 2, 0, 5 );

	// Leerraum
	gSizer2->Add( 0, 0, 1, wxEXPAND, 10 );

	// GHS-Symbole hinzufügen
	gSizer2->Add( panelSymbols, 0, wxEXPAND | wxALL | wxALIGN_CENTER, 10 );

	// Signalwort, H- und P-Sätze
	// --------------------------
	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( panel, wxID_ANY, _(L"Signalwort:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( 290 );
	gSizer2->Add( m_staticText5, 0, wxLEFT|wxTOP|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10 );

	m_textGHSSignalWord = new wxTextCtrl( panel, wxID_ANY, data.szSignalWord, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gSizer2->Add( m_textGHSSignalWord, 0, wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 10 );

	wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( panel, wxID_ANY, _(L"H-Sätze:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( 290 );
	gSizer2->Add( m_staticText6, 0, wxLEFT|wxTOP|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10 );

	m_textGHSHarzards = new wxTextCtrl( panel, wxID_ANY, data.szHarzardSatements, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gSizer2->Add( m_textGHSHarzards, 0, wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 10 );

	wxStaticText* m_staticText8;
	m_staticText8 = new wxStaticText( panel, wxID_ANY, _(L"EuH-Sätze:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( 290 );
	gSizer2->Add( m_staticText8, 0, wxLEFT|wxTOP|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10 );

	m_textGHSEuHarzards = new wxTextCtrl( panel, wxID_ANY, data.szEuropeanHarzardSatements, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gSizer2->Add( m_textGHSEuHarzards, 0, wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 10 );

	wxStaticText* m_staticText7;
	m_staticText7 = new wxStaticText( panel, wxID_ANY, _(L"P-Sätze:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( 290 );
	gSizer2->Add( m_staticText7, 0, wxLEFT|wxTOP|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 10 );

	m_textGHSPrecautionary = new wxTextCtrl( panel, wxID_ANY, data.szPrecautionaryStatements, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gSizer2->Add( m_textGHSPrecautionary, 0, wxLEFT|wxTOP|wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 10 );

	bSizer1->Add( gSizer2, 0, 0, 0);

	// Abschließen
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePageToxicology(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Wassergefährdungsklasse:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	wxString m_WaterHarzardClassStrings[] = { _(L"NWG: Keine Wassergefährdung"), _(L"WGK 1: Schwache Wassergefährdung"), _(L"WGK 2: Wassergefährdung"), _(L"WGK 3: Schwere Wassergefährdung") };
	m_WaterHarzardClass = new wxChoice( panel, wxID_ANY, wxDefaultPosition, wxSize( 410,-1 ), 4, m_WaterHarzardClassStrings, 0 );
	m_WaterHarzardClass->SetSelection( data.iWGK );
	bSizer1->Add( m_WaterHarzardClass, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	m_MAKText = new wxStaticText( panel, wxID_ANY, _(L"MAK in mg/m³:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_MAKText->Wrap( 410 );
	bSizer1->Add( m_MAKText, 0, wxALL, 10 );

	wxFlexGridSizer* gSizer1;
	gSizer1 = new wxFlexGridSizer( 0, 2, 10, 20 );

	m_textMAK = new wxTextCtrl( panel, wxID_ANY, wxString::Format("%.2f", data.dbMAK), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer1->Add( m_textMAK, 0, wxLEFT|wxALIGN_LEFT, 30 );

	m_checkMAKppm = new wxCheckBox( panel, beID_MAKPPM, _(L"ppm"), wxDefaultPosition, wxSize( 120, -1));
	gSizer1->Add( m_checkMAKppm, 0, wxLEFT|wxALIGN_CENTER_VERTICAL|wxALIGN_LEFT, 0 );

	if ( data.bMAKppm )
	{
		// Häkchen setzen
		m_checkMAKppm->SetValue( true );

		// Text ändern
		m_MAKText->SetLabel( _(L"MAK in mL/m³") );
	}

	bSizer1->Add( gSizer1, 0, 0, 0);
	bSizer1->AddSpacer( 10 );

	// Lethale Dosen
	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"Letale Dosen (LD50):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 410 );
	bSizer1->Add( m_staticText3, 0, wxALL, 10 );

	// Liste
	m_listLD50 = new wxListCtrl( panel, beID_TOXY_LD50, wxDefaultPosition, wxSize( 410, 200 ), wxLC_REPORT );
	m_listLD50->InsertColumn( 0, _(L"Spezies, Aufnahmeweg"), wxLIST_FORMAT_LEFT, 180);
	m_listLD50->InsertColumn( 1, _(L"Wert"),	   wxLIST_FORMAT_LEFT, 70);
	m_listLD50->InsertColumn( 2, _(L"Einheit"), wxLIST_FORMAT_LEFT, 70);
	bSizer1->Add( m_listLD50, 0, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	// Liste füllen
	FillLethalDoseList();

	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePageBatch(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	// Einheiten
	// =========
	const int iNumberUnitsMass = 4;
	wxString m_UnitsMass[iNumberUnitsMass] =						{ L"mg", L"g", L"kg", L"t" };
	const int iNumberUnitsVolume = 3;
	wxString m_UnitsVolume[iNumberUnitsVolume] =					{ L"µL", L"mL", L"L" };
	const int iNumberUnitsSolutionConcentration = 4;
	wxString m_UnitsSolutionConcentration[iNumberUnitsSolutionConcentration] =	{ L"% (v/v)", L"µmol/L", L"mmol/L", L"mol/L" };

	// Ansatz in Gramm
	// ===============
	m_radioBatchMass = new wxRadioButton( panel, beID_BATCHMASS, _(L"Ansatz in Gramm (reine Substanz):"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	bSizer1->Add( m_radioBatchMass, 0, wxALL, 10 );

	wxFlexGridSizer* gSizer1; gSizer1 = new wxFlexGridSizer( 0, 2, 10, 10 );

	m_textMass = new wxTextCtrl( panel, beID_BATCHTEXTMASS, "", wxDefaultPosition, wxSize( 100,-1 ), 0 );
	gSizer1->Add( m_textMass, 0, wxLEFT|wxALIGN_LEFT, 30 );

	m_choiceMassUnit = new wxChoice( panel, beID_BATCHUNITMASS, wxDefaultPosition, wxSize( 70,-1 ), iNumberUnitsMass, m_UnitsMass, 0 );
	m_choiceMassUnit->SetSelection( 0 );
	gSizer1->Add( m_choiceMassUnit, 0, wxLEFT|wxALIGN_LEFT, 10 );

	bSizer1->Add( gSizer1, 0, 0, 0);
	bSizer1->AddSpacer( 10 );

	// Ansatz in Liter
	// ===============
	m_radioBatchVolume = new wxRadioButton( panel, beID_BATCHVOLUME, _(L"Ansatz in Liter (reine Substanz):"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_radioBatchVolume, 0, wxALL, 10 );

	wxFlexGridSizer* gSizer2; gSizer2 = new wxFlexGridSizer( 0, 2, 10, 10 );

	m_textVolume = new wxTextCtrl( panel, beID_BATCHTEXTVOL, "", wxDefaultPosition, wxSize( 100,-1 ), 0 );
	gSizer2->Add( m_textVolume, 0, wxLEFT|wxALIGN_LEFT, 30 );

	m_choiceVolumeUnit = new wxChoice( panel, beID_BATCHUNITVOL, wxDefaultPosition, wxSize( 70,-1 ), iNumberUnitsVolume, m_UnitsVolume, 0 );
	m_choiceVolumeUnit->SetSelection( 0 );
	gSizer2->Add( m_choiceVolumeUnit, 0, wxLEFT|wxALIGN_LEFT, 10 );

	bSizer1->Add( gSizer2, 0, 0, 0);
	bSizer1->AddSpacer( 10 );

	// Ansatz einer Lösung
	// ===================
	m_radioBatchSolution = new wxRadioButton( panel, beID_BATCHSOLUTION, _(L"Ansatz einer Lösung:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_radioBatchSolution, 0, wxALL, 10 );

	wxFlexGridSizer* gSizer3; gSizer3 = new wxFlexGridSizer( 0, 3, 10, 10 );

	m_staticSolutionConcentration = new wxStaticText( panel, wxID_ANY, _(L"Konzentration:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_staticSolutionConcentration, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxALIGN_LEFT, 30 );

	m_textConcentration = new wxTextCtrl( panel, beID_BATCHTEXTCONC, "", wxDefaultPosition, wxSize( 70,-1 ), 0 );
	gSizer3->Add( m_textConcentration, 0, wxLEFT|wxALIGN_LEFT, 10 );

	m_choiceConcentrationUnit = new wxChoice( panel, beID_BATCHUNITCONC, wxDefaultPosition, wxSize( 100,-1 ), iNumberUnitsSolutionConcentration, m_UnitsSolutionConcentration, 0 );
	m_choiceConcentrationUnit->SetSelection( 0 );
	gSizer3->Add( m_choiceConcentrationUnit, 0, wxLEFT|wxALIGN_LEFT, 10 );

	m_staticSolutionVolume = new wxStaticText( panel, wxID_ANY, _(L"Volumen der Lösung:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer3->Add( m_staticSolutionVolume, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxALIGN_LEFT, 30 );

	m_textSolutionVolume = new wxTextCtrl( panel, beID_BATCHTEXTSVOL, "", wxDefaultPosition, wxSize( 70,-1 ), 0 );
	gSizer3->Add( m_textSolutionVolume, 0, wxLEFT|wxALIGN_LEFT, 10 );

	m_choiceSolutionVolumeUnit = new wxChoice( panel, beID_BATCHUNITSVOL, wxDefaultPosition, wxSize( 100,-1 ), iNumberUnitsVolume, m_UnitsVolume, 0 );
	m_choiceSolutionVolumeUnit->SetSelection( 0 );
	gSizer3->Add( m_choiceSolutionVolumeUnit, 0, wxLEFT|wxALIGN_LEFT, 10 );

	bSizer1->Add( gSizer3, 0, 0, 0);
	bSizer1->AddSpacer( 30 );

	// Eingabe entspricht einer Stoffmenge von...
	// ==========================================
	m_textAmountOfSubstance = new wxStaticText( panel, wxID_ANY, _(L"Die Eingabe entspricht einer Stoffmenge von %smol."), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_textAmountOfSubstance, 0, wxALL, 10 );

	// Welchen Typ markieren?
	// ======================
	// TODO: Folgenden Abschnitt mal bei Gelegenheit überarbeiten.
	switch ( data.GetBatchType() )
	{
	case CBerylliumSubstanceData::batchSolid:
		{
			// Radioknöpfe einstellen
			m_radioBatchMass->SetValue(true);
			m_radioBatchVolume->SetValue(false);
			m_radioBatchSolution->SetValue(false);

			// Menge holen
			double amount = data.GetBatchAmount();

			// Einheit [ 0 = mg, g, kg, t ]
			double exp[] = { 1.0e-3, 1, 1.0e3, 1.0e6 };

			// Entsprechende Einheit markieren
			for ( unsigned int i = 0; i < 4; i++ )
			{
				double newamount = amount / exp[i];

				if ( (newamount < 500) || (i == 3) )
				{
					amount = newamount;
					m_choiceMassUnit->SetSelection( i );

					break;
				}
			}

			// Menge setzen
			m_textMass->SetValue( wxString::Format( "%.3f", amount) );
		};
		break;

	case CBerylliumSubstanceData::batchLiquid:
		{
			// Radioknöpfe einstellen
			m_radioBatchMass->SetValue(false);
			m_radioBatchVolume->SetValue(true);
			m_radioBatchSolution->SetValue(false);

			// Menge holen
			double amount = data.GetBatchAmount();

			// Einheit [ 0 = µL, mL, L ]
			double exp[] = { 1.0e-6, 1.0e-3, 1.0 };

			// Entsprechende Einheit markieren
			for ( unsigned int i = 0; i < 3; i++ )
			{
				double newamount = amount / exp[i];

				if ( (newamount < 500) || (i == 2) )
				{
					amount = newamount;
					m_choiceVolumeUnit->SetSelection( i );

					break;
				}
			}

			// Menge setzen
			m_textVolume->SetValue( wxString::Format( "%.3f", amount) );
		};
		break;

	case CBerylliumSubstanceData::batchSolution:
		{
			// Radioknöpfe einstellen
			m_radioBatchMass->SetValue(false);
			m_radioBatchVolume->SetValue(false);
			m_radioBatchSolution->SetValue(true);

			// Konzentration holen
			double conc = data.GetBatchConcentration();

			// Einheit [ µmol/L, mmol/L, mol/L ]
			double expconc[] = { 1.0e-6, 1.0e-3, 1.0 };

			// Relative Konzentration?
			if ( data.IsBatchConcentrationRelative() )
			{
				conc = conc * 100;
			}

			// Absolute Konzentration
			else
			{
				// Entsprechende Einheit markieren
				for ( unsigned int i = 0; i < 3; i++ )
				{
					double newconc = conc / expconc[i];

					if ( (newconc < 500) || (i == 2) )
					{
						conc = newconc;
						m_choiceConcentrationUnit->SetSelection( i+1 );

						break;
					}
				}

			}

			// Menge holen
			double amount = data.GetBatchAmount();

			// Einheit [ 0 = µL, mL, L ]
			double expamount[] = { 1.0e-6, 1.0e-3, 1.0 };

			// Entsprechende Einheit markieren
			for ( unsigned int i = 0; i < 3; i++ )
			{
				double newamount = amount / expamount[i];

				if ( (newamount < 500) || (i == 2) )
				{
					amount = newamount;
					m_choiceSolutionVolumeUnit->SetSelection( i );

					break;
				}
			}

			// Menge und Konzentration setzen
			m_textConcentration->SetValue( wxString::Format( "%.3f", conc) );
			m_textSolutionVolume->SetValue( wxString::Format( "%.3f", amount) );
		};
		break;

	case CBerylliumSubstanceData::batchLAST:
		break;
	};

	// Update der Seite
	UpdateBatchPage();

	// Fertigstellen
	// =============
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

wxPanel *CBerylliumSubstanceProperties::CreatePageAdvices(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	// Box 1: Gefahren für Mensch und Umwelt
	// =====================================
	wxStaticBoxSizer* sbBox1 = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, _(L"Gefahren für Mensch und Umwelt") ), wxVERTICAL );
	wxBoxSizer* bBoxSizer1	 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText *m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"z.B. Radioaktivität"), wxDefaultPosition, wxSize( 300, -1 ), 0 );
	m_staticText1->Wrap( -1 );
	bBoxSizer1->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxButton *m_button1 = new wxButton( panel, beID_CUSTOMDANGERS, _(L"Bearbeiten"), wxDefaultPosition, wxSize( 90, -1) , 0 );
	bBoxSizer1->Add( m_button1, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	sbBox1->Add( bBoxSizer1, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	bSizer1->Add( sbBox1, 0, wxTOP|wxLEFT|wxRIGHT, 10 );

	// Box 2: Schutzmaßnahmen
	// ======================
	wxStaticBoxSizer* sbBox2 = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, _(L"Schutzmaßnahmen") ), wxVERTICAL );
	wxBoxSizer* bBoxSizer2	 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText *m_staticText2 = new wxStaticText( panel, wxID_ANY, _(L"z.B. Verhalten beim Pipettieren"), wxDefaultPosition, wxSize( 300, -1 ), 0 );
	m_staticText2->Wrap( -1 );
	bBoxSizer2->Add( m_staticText2, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxButton *m_button2 = new wxButton( panel, beID_CUSTOMPRECAUTIONS, _(L"Bearbeiten"), wxDefaultPosition, wxSize( 90, -1) , 0 );
	bBoxSizer2->Add( m_button2, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	sbBox2->Add( bBoxSizer2, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	bSizer1->Add( sbBox2, 0, wxTOP|wxLEFT|wxRIGHT, 10 );

	// Box 3: Verhalten im Gefahrenfall
	// ================================
	wxStaticBoxSizer* sbBox3 = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, _(L"Verhalten im Gefahrenfall") ), wxVERTICAL );
	wxBoxSizer* bBoxSizer3	 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText *m_staticText3 = new wxStaticText( panel, wxID_ANY, _(L"z.B. Versiegelung der Räume"), wxDefaultPosition, wxSize( 300, -1 ), 0 );
	m_staticText3->Wrap( -1 );
	bBoxSizer3->Add( m_staticText3, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxButton *m_button3 = new wxButton( panel, beID_CUSTOMFIRSTAID, _(L"Bearbeiten"), wxDefaultPosition, wxSize( 90, -1) , 0 );
	bBoxSizer3->Add( m_button3, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	sbBox3->Add( bBoxSizer3, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	bSizer1->Add( sbBox3, 0, wxTOP|wxLEFT|wxRIGHT, 10 );

	// Box 4: Entsorgung
	// =================
	wxStaticBoxSizer* sbBox4 = new wxStaticBoxSizer( new wxStaticBox( panel, wxID_ANY, _(L"Entsorgung") ), wxVERTICAL );
	wxBoxSizer* bBoxSizer4	 = new wxBoxSizer( wxHORIZONTAL );

	wxStaticText *m_staticText4 = new wxStaticText( panel, wxID_ANY, _(L"z.B. Reduzierungen"), wxDefaultPosition, wxSize( 300, -1 ), 0 );
	m_staticText4->Wrap( -1 );
	bBoxSizer4->Add( m_staticText4, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxButton *m_button4 = new wxButton( panel, beID_CUSTOMDISPOSAL, _(L"Bearbeiten"), wxDefaultPosition, wxSize( 90, -1) , 0 );
	bBoxSizer4->Add( m_button4, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	sbBox4->Add( bBoxSizer4, 0, wxLEFT|wxRIGHT|wxBOTTOM, 5 );
	bSizer1->Add( sbBox4, 0, wxTOP|wxLEFT|wxRIGHT, 10 );

	// Fertigstellen
	// =============
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

// Initialisiert die Seite "Quelle"
wxPanel *CBerylliumSubstanceProperties::CreatePageSource(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Die Daten dieser Substanz wurden aus folgender Quelle geladen und zum angegeben Zeitpunkt durch den Benutzer verändert:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 410 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	bSizer1->AddSpacer( 10 );

	// Quelle, Link, Datum
	// -------------------
	wxFlexGridSizer* gSizer2;
	gSizer2 = new wxFlexGridSizer( 0, 3, 0, 0 );

	gSizer2->AddSpacer(30);

	wxStaticText* m_staticText5;
	m_staticText5 = new wxStaticText( panel, wxID_ANY, _(L"Quelle:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_staticText5, 0, wxLEFT|wxALIGN_RIGHT, 10 );

	wxHyperlinkCtrl* m_staticSourceProvider = new wxHyperlinkCtrl( panel, beID_SOURCELINK, data.source.provider, data.source.url, wxDefaultPosition, wxSize( 300,-1 ), wxHL_ALIGN_LEFT );
	gSizer2->Add( m_staticSourceProvider, 0, wxLEFT|wxALIGN_LEFT, 10 );

	gSizer2->AddSpacer(30);

	wxStaticText* m_staticText6;
	m_staticText6 = new wxStaticText( panel, wxID_ANY, _(L"Aufgerufen:"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_staticText6, 0, wxLEFT|wxALIGN_RIGHT, 10 );

	wxStaticText* m_staticSourceFetched;
	m_staticSourceFetched = new wxStaticText( panel, wxID_ANY, data.source.fetched, wxDefaultPosition, wxSize( 300,-1 ), 0 );
	gSizer2->Add( m_staticSourceFetched, 0, wxLEFT|wxALIGN_LEFT, 10 );

	// Nur anzeigen falls entsprechendes Feld gesetzt wurde
	if ( data.source.changed.length() > 0 )
	{
		gSizer2->AddSpacer(30);

		wxStaticText* m_staticText7;
		m_staticText7 = new wxStaticText( panel, wxID_ANY, _(L"Geändert:"), wxDefaultPosition, wxDefaultSize, 0 );
		gSizer2->Add( m_staticText7, 0, wxLEFT|wxALIGN_RIGHT, 10 );

		wxStaticText* m_staticUserChanged;
		m_staticUserChanged = new wxStaticText( panel, wxID_ANY, data.source.changed, wxDefaultPosition, wxSize( 300, -1) , 0 );
		gSizer2->Add( m_staticUserChanged, 0, wxLEFT|wxALIGN_LEFT, 10 );
	}


	bSizer1->Add( gSizer2, 0, 0, 0);


	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumSubstanceProperties::OnInitDialog( wxInitDialogEvent &event )
{
	// Weiter im Programm
	event.Skip();
}

void CBerylliumSubstanceProperties::OnOK( wxCommandEvent &event )
{
	// Seite 1: Allgemeine Daten
	// -------------------------
	wxString szNames		= m_textNames->GetValue();		data.SetNames( szNames );
	wxString szFormula	= m_textFormula->GetValue();	data.SetFormula( szFormula );
	data.szCAS = m_textCAS->GetValue();
	wxString szMolWeight = m_textMolWeight->GetValue();	data.SetMolWeight( szMolWeight );

	// Seite 2: Physikalische Daten
	// ----------------------------
	data.szMeltingPoint = m_textMeltingPoint->GetValue();
	data.szBoilingPoint = m_textBoilingPoint->GetValue();
	data.bDecomposition = m_checkDecomposition->GetValue();
	data.szFlashPoint	= m_textFlashPoint->GetValue();
	wxString szDensity = m_textDensity->GetValue();	data.SetDensity( szDensity );

	// Seite 3: Sicherheit
	// ---------------------
	data.szRisks					= m_textRisks->GetValue();
	data.szSafeties					= m_textSafeties->GetValue();
	wxString szHarzards				= m_textHarzards->GetValue(); data.SetHarzards( szHarzards );

	// Seite 4: GHS
	// ------------
	data.szSignalWord				= m_textGHSSignalWord->GetValue();
	data.szHarzardSatements			= m_textGHSHarzards->GetValue();
	data.szEuropeanHarzardSatements	= m_textGHSEuHarzards->GetValue();
	data.szPrecautionaryStatements	= m_textGHSPrecautionary->GetValue();

	// Alte Symbole löschen
	data.szGHSSymbols.clear();

	// Schauen welche Checkbox gedrückt wurde
	for ( unsigned int i = 0; i < 9; ++i )
	{
		// Häkchen?
		if ( m_GHSSymbols[i]->GetValue() )
		{
			wxString szSymbol = wxString::Format("%02d", i+1);

			data.szGHSSymbols.push_back( szSymbol );
		}
	}

	// Seite 5: Toxikologie
	// --------------------
	wxString szMAK	= m_textMAK->GetValue(); data.SetMAK( szMAK );
	data.bMAKppm		= m_checkMAKppm->GetValue();
	data.iWGK			= m_WaterHarzardClass->GetSelection();

	data.SetLD50Data( m_ld50values );

	// Ende: Weiter im Programm
	// ------------------------
	event.Skip();
}

void CBerylliumSubstanceProperties::OnDecomposition( wxCommandEvent &event )
{
	// Häkchen gesetzt?
	if ( event.IsChecked() )
	{
		m_staticBoilingPoint->Enable(false);
		m_textBoilingPoint->Enable(false);
	}

	// Nicht gesetzt!
	else
	{
		m_staticBoilingPoint->Enable(true);
		m_textBoilingPoint->Enable(true);
	}

	// Weiter im Programm
	event.Skip();
}

void CBerylliumSubstanceProperties::OnMAKppm( wxCommandEvent &event )
{
	// Häkchen gesetzt?
	if ( event.IsChecked() )
	{
		m_MAKText->SetLabel( _(L"MAK in mL/m³:") );
	}

	// Nicht gesetzt!
	else
	{
		m_MAKText->SetLabel( _(L"MAK in mg/m³:") );
	}

	// Weiter im Programm
	event.Skip();
}

void CBerylliumSubstanceProperties::OnBatch( wxCommandEvent &event )
{
	// Update der Ansatz-Seite
	UpdateBatchPage();

	// Berechnen
	CalculateBatchPage();

	// Weiter im Programm
	event.Skip();
}

void CBerylliumSubstanceProperties::UpdateBatchPage()
{
	// Welches Häkchen wurde gesetzt?
	if ( m_radioBatchMass->GetValue() )
	{
		m_textMass->Enable();
		m_choiceMassUnit->Enable();

		m_textVolume->Disable();
		m_choiceVolumeUnit->Disable();

		m_textConcentration->Disable();
		m_choiceConcentrationUnit->Disable();
		m_textSolutionVolume->Disable();
		m_choiceSolutionVolumeUnit->Disable();
		m_staticSolutionConcentration->Disable();
		m_staticSolutionVolume->Disable();

		// Typ setzen
		data.SetBatchType( data.batchSolid );

	}

	else if ( m_radioBatchVolume->GetValue() )
	{
		m_textVolume->Enable();
		m_choiceVolumeUnit->Enable();

		m_textMass->Disable();
		m_choiceMassUnit->Disable();

		m_textConcentration->Disable();
		m_choiceConcentrationUnit->Disable();
		m_textSolutionVolume->Disable();
		m_choiceSolutionVolumeUnit->Disable();
		m_staticSolutionConcentration->Disable();
		m_staticSolutionVolume->Disable();

		// Typ setzen
		data.SetBatchType( data.batchLiquid );

	}

	else if ( m_radioBatchSolution->GetValue() )
	{
		m_textConcentration->Enable();
		m_choiceConcentrationUnit->Enable();
		m_textSolutionVolume->Enable();
		m_choiceSolutionVolumeUnit->Enable();
		m_staticSolutionConcentration->Enable();
		m_staticSolutionVolume->Enable();

		m_textMass->Disable();
		m_choiceMassUnit->Disable();

		m_textVolume->Disable();
		m_choiceVolumeUnit->Disable();

		// Typ setzen
		data.SetBatchType( data.batchSolution );
	}
}

// Wird aufgerufen, wenn auf einen der Buttons auf der "Hinweis"-Seite geklickt wurde
void CBerylliumSubstanceProperties::OnStatements( wxCommandEvent &event )
{
	// Titel festlegen
	wxString titles[] = { _(L"Gefahren für Mensch und Umwelt"),
						  _(L"Schutzmaßnahmen und Verhaltensregeln"),
						  _(L"Verhalten im Gefahrenfall"),
						  _(L"Entsorgung") };

	// Index holen
	int index = event.GetId() - beID_CUSTOMDANGERS;

	// Fall abfangen hier mit ungültigem Index zu landen
	if ( (index > 3) || (index < 0) )
		index = 0;

	// Dialog erstellen
	CBerylliumStatementEditor dlg( this, titles[index] );

	// Liste übertragen
	dlg.SetStatements( data.sCustomStatements[index] );

	// Dialog anzeigen
	if ( dlg.ShowModal() == wxID_OK )
	{
		// Liste übertragen
		dlg.GetStatements( data.sCustomStatements[index] );
	}

	// Weitermachen
	event.Skip();
}

// Berechnet die "Ansatz"-Seite neu, wenn sich etwas geändert hat
void CBerylliumSubstanceProperties::CalculateBatchPage()
{
	// Berechnen der Stoffmenge
	double amount = data.CalculateAmountOfSubstance();

	// Einheit entsprechend berechnen
	wxString unit = BeTool::UnitPrefix(amount);

	// in einen String umwandeln und mit einer Einheit ausstatten
	wxString sAmount = wxString::Format("%.1f %s", amount, unit.c_str() );

	// Setzen
	m_textAmountOfSubstance->SetLabel( wxString::Format( _(L"Die Eingabe entspricht einer Stoffmenge von %smol."), sAmount.c_str() ) );
}

// Wird aufgerufen, wenn sich eines der Textfelder oder der Einheiten auf der Ansatzseite ändert
void CBerylliumSubstanceProperties::OnBatchTextChange( wxCommandEvent &event )
{
	// Menge
	double amount = 0.0;

	// Welches Häkchen wurde gesetzt?
	if ( m_radioBatchMass->GetValue() )
	{
		// Menge holen
		wxString str = m_textMass->GetValue();

		// Umwandeln
		amount = BeTool::StringToDouble( str );

		// Einheit [ 0 = mg, g, kg, t ]
		double exp[] = { 1.0e-3, 1, 1.0e3, 1.0e6 };

		if ( m_choiceMassUnit->GetSelection() < 4 )
			amount = amount * exp[m_choiceMassUnit->GetSelection()];

		// Typ setzen
		data.SetBatchType( data.batchSolid );
	}

	else if ( m_radioBatchVolume->GetValue() )
	{
		// Menge holen
		wxString str = m_textVolume->GetValue();

		// Umwandeln
		amount = BeTool::StringToDouble( str );

		// Einheit [ 0 = µL, mL, L ]
		double exp[] = { 1.0e-6, 1.0e-3, 1 };

		if ( m_choiceVolumeUnit->GetSelection() < 3 )
			amount = amount * exp[m_choiceVolumeUnit->GetSelection()];

		// Typ setzen
		data.SetBatchType( data.batchLiquid );
	}

	else if ( m_radioBatchSolution->GetValue() )
	{
		// Konzentration holen
		wxString strConc = m_textConcentration->GetValue();

		// Umwandeln
		double conc = BeTool::StringToDouble( strConc );

		// Einheit Konzentration [ 0 = %, µmol, mmol, mol ]
		double expConc[] = { 1, 1.0e-6, 1.0e-3, 1 };

		// Relative Konzentration
		data.SetBatchConcentrationRelative( ( m_choiceConcentrationUnit->GetSelection() == 0 ) );

		if ( m_choiceConcentrationUnit->GetSelection() == 0 )
			conc = conc / 100.0;

		// Konzentration berechnen
		if ( m_choiceConcentrationUnit->GetSelection() < 4 )
			conc = conc * expConc[m_choiceConcentrationUnit->GetSelection()];

		// Konzentration setzen
		data.SetBatchConcentration( conc );

		// Volumen holen
		wxString str = m_textSolutionVolume->GetValue();

		// Umwandeln
		amount = BeTool::StringToDouble( str );

		// Einheit [ 0 = µL, mL, L ]
		double expVol[] = { 1.0e-6, 1.0e-3, 1 };

		if ( m_choiceSolutionVolumeUnit->GetSelection() < 3 )
			amount = amount * expVol[m_choiceSolutionVolumeUnit->GetSelection()];

		// Typ setzen
		data.SetBatchType( data.batchSolution );
	}

	// Menge setzen
	data.SetBatchAmount( amount );

	// Neuberechnen
	CalculateBatchPage();

	// Weitermachen
	event.Skip();
}

// Füllt die Liste der LD50-Werte
void CBerylliumSubstanceProperties::FillLethalDoseList()
{
	// Alle Elemente löschen
	m_listLD50->DeleteAllItems();

	// Anzahl Element im Vektor
	unsigned int count = m_ld50values.size();

	// Alle Elemente durchgehen und einfügen
	for ( unsigned int i = 0; i < count; ++i )
	{
		// Element einfügen
		long index = m_listLD50->InsertItem(i, wxString::Format("%s, %s", m_ld50values[i].sSpecies, m_ld50values[i].sExposureType) );

		// Wert setzen
		m_listLD50->SetItem(index, 1, wxString::Format("%.6f", m_ld50values[i].dbValue) );

		// Einheit setzen
		m_listLD50->SetItem(index, 2, m_ld50values[i].sUnit );

		// ItemData setzen (mit Werte-Liste assozieren)
		m_listLD50->SetItemData(index, i);
	}


	// Anzahl der Elemente
	int iCount = m_listLD50->GetItemCount();

	// Am Ende noch einen "< Neuen Eintrag hinzufügen >"-Eintrag ans Ende machen
	m_listLD50->InsertItem(iCount, _(L"< Neuen Eintrag hinzufügen >"));
}

// Doppelklick/Enter auf Eintrag in LD50-Liste
void CBerylliumSubstanceProperties::OnLD50Activate( wxListEvent &event )
{
	// Dialogobjekt erstellen
	CBerylliumLethalDialog dlg( this );

	// Index holen
	long index = event.GetIndex();

	// Letzter Eintrag in der Liste? -> Neue Daten hinzufügen
	if ( index == (m_listLD50->GetItemCount()-1) )
	{
		// Dialog anzeigen
		if ( dlg.ShowModal() == wxID_OK )
			// Objekt holen und an Liste anhängen
			m_ld50values.push_back( dlg.GetLethalDose() );
	}

	// andernfalls den entsprechenden Eintrag editieren!
	else
	{
		// Datenindex holen (Vorzeichen wegschmeißen)
		unsigned long data = (unsigned)event.GetData();

		// Außerhalb des Bereiches? Dann raus hier!
		if ( data >= m_ld50values.size() )
			return;

		// Daten setzen
		dlg.SetLethalDose( m_ld50values[data] );

		// Dialog anzeigen
		if ( dlg.ShowModal() == wxID_OK )
			// Daten aktualisieren
			m_ld50values[data] = dlg.GetLethalDose();
	}

	// Zum Schluss die Liste aktualisieren (und evtl. neusortieren)
	FillLethalDoseList();
}

// Taste in LD50-Liste gedrückt
void CBerylliumSubstanceProperties::OnLD50KeyDown( wxListEvent &event )
{
	// Index holen
	long index = event.GetIndex();

	// Entfernen-Taste gedrückt und Eintrag selektiert?
	if ( (index > -1) && (index < (m_listLD50->GetItemCount()-1)) && ((event.GetKeyCode() == WXK_DELETE) || (event.GetKeyCode() == WXK_NUMPAD_DELETE)) )
	{
		// Datenindex holen (Vorzeichen wegschmeißen)
		unsigned long data = (unsigned)event.GetData();

		// Außerhalb des Bereiches? Dann raus hier!
		if ( data >= m_ld50values.size() )
			return;

		// Nachfrage ist nicht nötig, also Eintrag einfach entfernen
		m_ld50values.erase( m_ld50values.begin() + data );

		// Die Liste aktualisieren (und evtl. neusortieren)
		FillLethalDoseList();
	}

}
