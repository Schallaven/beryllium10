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

#include <wx/propdlg.h>

#include "../datahandling/berylliumdata.h"

class CBerylliumSafetySheetsGeneral : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumSafetySheetsGeneral)

public:
	CBerylliumSafetySheetsGeneral( wxWindow* parent, const CBerylliumData &data );
	~CBerylliumSafetySheetsGeneral(void);

	// Daten zurückgeben
	CBerylliumData GetData() { return m_data; };

	// Seite öffnen
	void SetPage( int iPage );

private:
	// Kopie der Daten
	CBerylliumData m_data;

	// Steuerelemente
	wxTextCtrl	*m_textTitle;
	wxTextCtrl	*m_textTitleGHS;
	wxTextCtrl	*m_textLocation;
	wxTextCtrl	*m_textEvent;

	wxTextCtrl	*m_textName;
	wxTextCtrl	*m_textPosition;
	wxTextCtrl	*m_textSupervisor;

	wxTextCtrl	*m_textPrepName;
	wxTextCtrl	*m_textPrepFormula;

	// Initialisiert die Seite "Allgemeines"
	wxPanel *CreatePageGeneral(wxWindow* parent);

	// Initialisiert die Seite "Persönliches"
	wxPanel *CreatePagePersonal(wxWindow* parent);

	// Initialisiert die Seite "Präparat"
	wxPanel *CreatePagePreparation(wxWindow* parent);

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

DECLARE_EVENT_TABLE()
};
