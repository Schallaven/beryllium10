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

#include "BerylliumBreakoutDialog.h"

IMPLEMENT_CLASS(CBerylliumBreakoutDialog, wxDialog)

BEGIN_EVENT_TABLE(CBerylliumBreakoutDialog, wxDialog)
END_EVENT_TABLE()


CBerylliumBreakoutDialog::CBerylliumBreakoutDialog(wxWindow *parent)
{
	// Erstellt das Breakoutfenster
	Create(parent, wxID_ANY, "PSE Breakout - A/D for left/right - R for restart - Space for starting the ball", wxDefaultPosition, wxSize( 675, 590) );

	// Zentrieren
	Center();

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	// Spielbereich erstellen
	m_breakout = new CBerylliumBreakout( this, 0, 0, 660, 580 );
	bSizer1->Add( m_breakout, 0, wxTOP|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

	// Fertigstellen
	this->SetSizer( bSizer1 );
	this->Layout();

	// Focus setzen
	m_breakout->SetFocus();
}

CBerylliumBreakoutDialog::~CBerylliumBreakoutDialog(void)
{
}


