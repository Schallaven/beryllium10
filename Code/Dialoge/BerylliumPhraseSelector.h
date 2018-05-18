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

#include <vector>
#include <algorithm>
#include <wx/propdlg.h>
#include <wx/checkbox.h>


// Dialog: Selektor für R/S/H/P-Sätze
class CBerylliumPhraseSelector : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumPhraseSelector)

public:
	CBerylliumPhraseSelector(wxWindow* parent, int iPage);
	~CBerylliumPhraseSelector(void);

	// Kopiert die Liste der Sätze
	void SetPhraseVector( std::vector< wxString > hidephrases ) { m_hidephrases = hidephrases; };

	// Holt die Liste der Sätze
	void GetPhraseVector( std::vector< wxString > &hidephrases ) { hidephrases = m_hidephrases; };

	// Alle Sätze, die in der Betriebsanweisungen vorkommen
	void SetAllRisks( std::vector< wxString > allrisks ) { m_allphrases_r = allrisks; };

	// Alle Sätze, die in der Betriebsanweisungen vorkommen
	void SetAllSafeties( std::vector< wxString > allsafeties ) { m_allphrases_s = allsafeties; };

	// Alle Sätze, die in der Betriebsanweisungen vorkommen
	void SetAllHarzards( std::vector< wxString > allharzards ) { m_allphrases_h = allharzards; };

	// Alle Sätze, die in der Betriebsanweisungen vorkommen
	void SetAllPrecautionaries( std::vector< wxString > allprecautionaries ) { m_allphrases_p = allprecautionaries; };

private:
	// Initialisiert die Seite "R-Sätze"
	wxPanel *CreatePageRisks(wxWindow* parent);

	// Initialisiert die Seite "S-Sätze"
	wxPanel *CreatePageSafeties(wxWindow* parent);

	// Initialisiert die Seite "H-Sätze"
	wxPanel *CreatePageHarzards(wxWindow* parent);

	// Initialisiert die Seite "P-Sätze"
	wxPanel *CreatePagePrecautionaries(wxWindow* parent);

	// Liste aller Sätze, die versteckt werden sollen
	std::vector< wxString > m_hidephrases;

	// Liste aller Sätze
	std::vector< wxString > m_allphrases_r;
	std::vector< wxString > m_allphrases_s;
	std::vector< wxString > m_allphrases_h;
	std::vector< wxString > m_allphrases_p;

	// Zeiger zu den Listboxen, die die Sätze enzhalten
	wxCheckListBox *m_listRisks;
	wxCheckListBox *m_listSafeties;
	wxCheckListBox *m_listHarzards;
	wxCheckListBox *m_listPrecautionaries;

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

	// Hilfsfunktion: Fügt die Liste in das entsprechende Steuerelement ein
	void InsertListInto( wxCheckListBox *m_listcontrol, std::vector< wxString > &phrases, wxString prefix );

	// Hilfsfunktion: Fügt die Sätze aus dem Steuerelement in hidephrases ein
	void HidePhrasesOfList( wxCheckListBox *m_listcontrol, wxString prefix );

DECLARE_EVENT_TABLE()
};
