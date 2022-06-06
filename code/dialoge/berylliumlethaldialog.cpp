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

#include "../beryllium.h"
#include "berylliumlethaldialog.h"

#include "../betool.h"

IMPLEMENT_CLASS(CBerylliumLethalDialog, wxDialog)

BEGIN_EVENT_TABLE(CBerylliumLethalDialog, wxDialog)
	EVT_BUTTON( wxID_OK, CBerylliumLethalDialog::OnOK )
END_EVENT_TABLE()

CBerylliumLethalDialog::CBerylliumLethalDialog( wxWindow *parent )
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Mittlere letale Dosis (LD50) bearbeiten"), wxDefaultPosition, wxSize( 450, 440));

	// Standardwert = 0
	m_dose.dbValue = 0.0f;

	// Dialogelemente erstellen
	CreateLayout();
}

CBerylliumLethalDialog::~CBerylliumLethalDialog(void)
{
}

// Erstellt alle Steuerelemente des Dialoges
void CBerylliumLethalDialog::CreateLayout()
{
	// Einheiten
	const int iNumberUnits			= 2;
	wxString m_Units[iNumberUnits]	= { L"mg/kg", L"mL/m³" };

	// Sizer erstellen und festlegen
	// =============================
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* SizerMain;
	SizerMain = new wxBoxSizer( wxVERTICAL );

	SizerMain->SetMinSize( wxSize( 450, 420 ) );

	// Inhaltsabschnitt
	// ================
	wxBoxSizer* SizerContent = new wxBoxSizer( wxVERTICAL );
	SizerContent->SetMinSize( wxSize( 450, 340 ) );

	// Spezies
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _(L"Spezies:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 430 );
	SizerContent->Add( m_staticText1, 0, wxALL, 10 );

	m_textSpecies = new wxTextCtrl( this, wxID_ANY, m_dose.sSpecies, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	SizerContent->Add( m_textSpecies, 0, wxLEFT|wxALIGN_LEFT, 30 );

	// Aufnahmeweg
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _(L"Aufnahmeweg:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 430 );
	SizerContent->Add( m_staticText2, 0, wxALL, 10 );

	m_textExposure = new wxTextCtrl( this, wxID_ANY, m_dose.sExposureType, wxDefaultPosition, wxSize( 120,-1 ), 0 );
	SizerContent->Add( m_textExposure, 0, wxLEFT|wxALIGN_LEFT, 30 );

	// Wert und Einheit
	wxStaticText* m_staticText3;
	m_staticText3 = new wxStaticText( this, wxID_ANY, _(L"Wert:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( 430 );
	SizerContent->Add( m_staticText3, 0, wxALL, 10 );


	wxBoxSizer* gSizer2; gSizer2 = new wxBoxSizer( wxHORIZONTAL );

	m_textValue = new wxTextCtrl( this, wxID_ANY, wxString::Format("%.6f", m_dose.dbValue), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	gSizer2->Add( m_textValue, 0, wxLEFT|wxALIGN_LEFT, 30 );

	m_choiceUnit = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), iNumberUnits, m_Units, 0 );
	m_choiceUnit->SetSelection( 0 );
	gSizer2->Add( m_choiceUnit, 0, wxLEFT|wxALIGN_LEFT, 10 );

	SizerContent->Add( gSizer2, 0, 0, 0);

	// Hinweis zur Eingabe von Dezimalwerten
	SizerContent->AddSpacer( 10 );

	wxStaticText* m_staticText4;
	m_staticText4 = new wxStaticText( this, wxID_ANY, _(L"Geben Sie eine beliebige Dezimalzahl ein. Sie können sowohl Punkt als auch Komma als Dezimaltrennzeichen benutzen."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( 400 );
	SizerContent->Add( m_staticText4, 0, wxALL, 10 );

	SizerMain->Add( SizerContent, 0, wxTOP|wxLEFT|wxRIGHT|wxEXPAND, 10 );
	SizerMain->AddStretchSpacer();

	// Buttonleiste (unten)
	// ====================
	wxStaticLine* staticLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	SizerMain->Add( staticLine, 0, wxEXPAND | wxALL, 0 );

	wxBoxSizer* SizerButtons = new wxBoxSizer( wxHORIZONTAL );

	SizerButtons->SetMinSize( wxSize( -1,25 ) );
	wxButton *buttonOK = new wxButton( this, wxID_OK, _(L"&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonOK->SetDefault();
	SizerButtons->Add( buttonOK, 0, wxALL, 5 );
	wxButton *buttonAbort = new wxButton( this, wxID_CANCEL, _(L"&Abbrechen"), wxDefaultPosition, wxDefaultSize, 0 );
	SizerButtons->Add( buttonAbort, 0, wxALL, 5 );

	SizerMain->Add( SizerButtons, 0, wxFIXED_MINSIZE|wxALIGN_RIGHT|wxALL, 5 );

	// Fertigstellen
	// =============
	this->SetSizer( SizerMain );
	this->Layout();

	this->Centre( wxBOTH );
}

void CBerylliumLethalDialog::OnOK( wxCommandEvent &event )
{
	// Daten übernehmen und umwandeln
	m_dose.sSpecies			= m_textSpecies->GetValue();
	m_dose.sExposureType	= m_textExposure->GetValue();
	m_dose.sUnit			= m_choiceUnit->GetString( m_choiceUnit->GetSelection() );

	wxString szValue = m_textValue->GetValue(); m_dose.dbValue = BeTool::StringToDouble( szValue );

	// Dialog beenden: OK
	EndDialog( wxID_OK );

	// Rausspringen
	return;
}


void CBerylliumLethalDialog::SetLethalDose( CBerylliumSubstanceData::LethalDose50 dose )
{
	// Wert kopieren
	m_dose = dose;

	// Werte übertragen
	m_textSpecies->SetValue( m_dose.sSpecies );
	m_textExposure->SetValue( m_dose.sExposureType );
	m_textValue->SetValue( wxString::Format("%.6f", m_dose.dbValue) );

	// Einheit setzen
	m_choiceUnit->SetSelection( (m_dose.sUnit.compare(L"mg/kg") == 0) ? 0 : 1 );
}
