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

#include <wx/statline.h>
#include "../datahandling/berylliumsubstancedata.h"

class CBerylliumLethalDialog : public wxDialog
{
DECLARE_CLASS(CBerylliumLethalDialog)

public:
	// Get: Faktor
	CBerylliumSubstanceData::LethalDose50 GetLethalDose() { return m_dose; };

	// Set: Faktor
	void SetLethalDose( CBerylliumSubstanceData::LethalDose50 dose );

private:
	// Objekt zum bearbeiten
	CBerylliumSubstanceData::LethalDose50	m_dose;

	// Steuerelemente
	wxTextCtrl *m_textSpecies;		// Spezies
	wxTextCtrl *m_textExposure;		// Aufnahmeweg
	wxTextCtrl *m_textValue;		// Wert
	wxChoice   *m_choiceUnit;		// Einheit

	// Erstellt die Dialogelemente
	void CreateLayout();

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

public:
	CBerylliumLethalDialog( wxWindow *parent );
	~CBerylliumLethalDialog(void);

DECLARE_EVENT_TABLE()
};
