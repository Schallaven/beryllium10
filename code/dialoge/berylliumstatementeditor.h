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
