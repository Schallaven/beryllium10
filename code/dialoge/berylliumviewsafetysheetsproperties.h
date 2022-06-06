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

#include <vector>
#include <wx/propdlg.h>
#include <wx/checkbox.h>

#include "../datahandling/berylliumviewsafetysheetscolumn.h"
#include "../datahandling/berylliumdata.h"

class CBerylliumViewSafetySheetsProperties : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumViewSafetySheetsProperties)

public:
	CBerylliumViewSafetySheetsProperties(wxWindow* parent, CBerylliumData newdata, int iPage);
	~CBerylliumViewSafetySheetsProperties(void);

	// Kopiert die Liste der Spalten
	void SetColumnsVector( std::vector< IBerylliumViewSafetySheetsColumn* > cols ) { m_Columns = cols; };

	// Holt die Liste der Spalten
	void GetColumnsVector( std::vector< IBerylliumViewSafetySheetsColumn* > &cols ) { cols = m_Columns; };

	// Set: Anzeige von H-/P-Sätzen
	void ShowHPStatements( bool show ) { m_checkshowhpstatements->SetValue(show); };

	// Get: H-/P-Sätze anzeigen?
	bool ShowHPStatements() { return m_checkshowhpstatements->GetValue(); };

	// Get: Daten holen
	CBerylliumData GetData() { return data; };

private:
	// Daten
	CBerylliumData data;

	// Checkbox für Sätze
	wxCheckBox *m_checkshowhpstatements;

	// Initialisiert die Seite "Spalten"
	wxPanel *CreatePageColumns(wxWindow* parent);

	// Initialisiert die Seite "R-/S-Sätze"
	wxPanel *CreatePagePhrases(wxWindow* parent);

	// Initialisiert die Seite "Signatur"
	wxPanel *CreatePageSignature(wxWindow* parent);

	// Liste aller Spalten für die erste Seite...
	std::vector< IBerylliumViewSafetySheetsColumn* > m_Columns;

	// Zeiger zu der Listbox, die die Spalten enthält
	wxCheckListBox *m_Columnslist;

	wxTextCtrl *m_textStatementStudent;
	wxTextCtrl *m_textStatementSupervisor;
	wxTextCtrl *m_textSignaturStudent;
	wxTextCtrl *m_textSignaturSupervisor;

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

DECLARE_EVENT_TABLE()
};
