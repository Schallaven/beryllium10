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

// Dialog: Substanzmultiplikator
class CBerylliumMultiplierDialog : public wxDialog
{
DECLARE_CLASS(CBerylliumMultiplierDialog)

public:
	// Get: Faktor
	double GetFactor() { return m_dbFactor; };

	// Set: Faktor
	void SetFactor( double newfactor );

private:
	// Faktor
	double		m_dbFactor;

	// Faktor
	wxTextCtrl *m_textFactor;

	// Erstellt die Dialogelemente
	void CreateLayout();

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

public:
	CBerylliumMultiplierDialog( wxWindow *parent );
	~CBerylliumMultiplierDialog(void);

DECLARE_EVENT_TABLE()
};
