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

#include "../betool.h"

#include <vector>
#include <string>

#include <wx/propdlg.h>
#include <wx/bookctrl.h>

class CBerylliumStatementEditor : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumStatementEditor)

public:
	// Erstellt das Fenster
	CBerylliumStatementEditor( wxWindow *parent, wxString title );

	// Zerstört das Fenster
	~CBerylliumStatementEditor(void);

	// Set: Übergibt einen Vektor mit Sätzen, die im Textfeld angezeigt werden
	void SetStatements( const std::vector< wxString > &list );

	// Get: Holt einen Vektor mit Sätzen, die im Textfeld eingegeben wurden
	void GetStatements( std::vector< wxString> &list );

private:
	// Textfeld
	wxTextCtrl *m_textctrl;

private:
	// Initialisiert die Seite "Benutzerdefiniert"
	wxPanel *CreatePageEditor(wxWindow* parent);

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

DECLARE_EVENT_TABLE()
};
