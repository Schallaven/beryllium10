﻿// **********************************************************************************
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
