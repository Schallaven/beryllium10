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
#include "berylliummultiplierdialog.h"

#include "../betool.h"

IMPLEMENT_CLASS(CBerylliumMultiplierDialog, wxDialog)

BEGIN_EVENT_TABLE(CBerylliumMultiplierDialog, wxDialog)
	EVT_BUTTON( wxID_OK, CBerylliumMultiplierDialog::OnOK )
END_EVENT_TABLE()

CBerylliumMultiplierDialog::CBerylliumMultiplierDialog( wxWindow *parent )
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Substanzen-Multiplikator"), wxDefaultPosition, wxSize( 600, 370));

	// Standardfaktor
	m_dbFactor = 2.00f;

	// Dialogelemente erstellen
	CreateLayout();
}

CBerylliumMultiplierDialog::~CBerylliumMultiplierDialog(void)
{
}

// Erstellt alle Steuerelemente des Dialoges
void CBerylliumMultiplierDialog::CreateLayout()
{
	// Sizer erstellen und festlegen
	// =============================
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* SizerMain;
	SizerMain = new wxBoxSizer( wxVERTICAL );

	SizerMain->SetMinSize( wxSize( 500,300 ) );
	wxPanel* panelTop;
	panelTop = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	panelTop->SetBackgroundColour( wxColour( 255, 255, 255 ) );

	wxBoxSizer* SizerTop;
	SizerTop = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* SizerText;
	SizerText = new wxBoxSizer( wxVERTICAL );

	SizerText->Add( 0, 10, 0, 0, 0 );

	// Titelabschnitt
	// ==============
	wxStaticText* staticTextTitle = new wxStaticText( panelTop, wxID_ANY, _(L"Um wieviel soll der Ansatz vergrößert oder verkleinert werden?"), wxDefaultPosition, wxDefaultSize, 0 );
	staticTextTitle->Wrap( -1 );
	staticTextTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

	SizerText->Add( staticTextTitle, 0, wxBOTTOM|wxLEFT|wxTOP, 5 );

	wxStaticText* staticTextSubTitle = new wxStaticText( panelTop, wxID_ANY, _(L"Bitte geben Sie einen Faktor ein."), wxDefaultPosition, wxDefaultSize, 0 );
	staticTextSubTitle->Wrap( -1 );
	SizerText->Add( staticTextSubTitle, 0, wxLEFT, 20 );

	SizerTop->Add( SizerText, 1, wxEXPAND|wxLEFT|wxBOTTOM, 15 );
	SizerTop->Add( 0, 0, 1, wxEXPAND, 5 );

	panelTop->SetSizer( SizerTop );
	panelTop->Layout();
	SizerTop->Fit( panelTop );
	SizerMain->Add( panelTop, 0, wxALIGN_RIGHT|wxALL|wxFIXED_MINSIZE|wxEXPAND, 0 );

	wxStaticLine* staticLineTop = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	SizerMain->Add( staticLineTop, 0, wxEXPAND | wxALL, 0 );

	// Inhaltsabschnitt
	// ================
	wxBoxSizer* SizerContent = new wxBoxSizer( wxVERTICAL );
	SizerContent->SetMinSize( wxSize( 500, 150 ) );

	// Text
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( this, wxID_ANY, _(L"Faktor:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 580 );
	SizerContent->Add( m_staticText1, 0, wxALL, 10 );

	m_textFactor = new wxTextCtrl( this, wxID_ANY, wxString::Format("%.2f", m_dbFactor), wxDefaultPosition, wxSize( 120,-1 ), 0 );
	SizerContent->Add( m_textFactor, 0, wxLEFT|wxALIGN_LEFT, 30 );

	SizerContent->AddSpacer( 10 );
	wxStaticText* m_staticText2;
	m_staticText2 = new wxStaticText( this, wxID_ANY, _(L"Geben Sie eine beliebige Dezimalzahl ein. Sie können sowohl Punkt als auch Komma als Dezimaltrennzeichen benutzen."), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( 580 );
	SizerContent->Add( m_staticText2, 0, wxALL, 10 );



	SizerMain->Add( SizerContent, 0, wxTOP|wxLEFT|wxRIGHT, 10 );
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

void CBerylliumMultiplierDialog::OnOK( wxCommandEvent &event )
{
	// Daten übernehmen und umwandeln
	wxString szFactor = m_textFactor->GetValue(); m_dbFactor = BeTool::StringToDouble( szFactor );

	// Dialog beenden: OK
	EndDialog( wxID_OK );

	// Rausspringen
	return;
}

void CBerylliumMultiplierDialog::SetFactor( double newfactor )
{
	// Faktor setzen
	m_dbFactor = newfactor;

	// Text setzen
	m_textFactor->SetValue( wxString::Format("%.2f", m_dbFactor) );
}

