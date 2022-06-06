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

#include <wx/srchctrl.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/htmllbox.h>

#include "../datahandling/berylliumsubstancedata.h"
#include "../datahandling/berylliumdatalibrary.h"
#include "../interfaces/iprovider.h"

// Buttons
#define beID_Continue			6001
#define beID_ChoiceBox			6002
#define beID_SearchCtrl			6003
#define beID_Preferences		6004
#define beID_SearchButton		6005

// Menu-Items
#define beID_Source_GESTIS		1001
#define beID_Source_MERCK		1002
#define beID_Source_EUSDB		1003
#define beID_Source_WIKIDE		1004
#define beID_Source_LOCALDB		1005
#define beID_Source_LAST		1006

#define beID_Prefs_EXACT		2001
#define beID_Prefs_SAVELOCAL	2002
#define beID_Prefs_ENABLELOG	2003
#define beID_Prefs_LAST			2003
#define beID_Prefs_SELLOCALDB	2101



class CBerylliumSubstanceSearchDialog : public wxDialog
{
DECLARE_CLASS(CBerylliumSubstanceSearchDialog)

public:
	CBerylliumSubstanceSearchDialog( wxWindow *parent );
	~CBerylliumSubstanceSearchDialog(void);

	// Get: Substanzdaten
	CBerylliumSubstanceData GetData() { return data; };

private:
	// Substanzdaten für die Liste
	struct substancedata {
		wxString	text;			// Anzeigetext für die Liste (HTML)
		wxString	name;			// Substanzname
		int			serial;			// eindeutige ID, damit das Interface die Substanz genau zuordnene kann
		IProvider	*provider;		// Zeiger zum Interface
	};

	// Lokale Datenbank
	wxString				m_localdatabasename;
	CBerylliumDataLibrary	m_localdatabase;

	// Liste mit Herstellern
	std::vector< IProvider* > m_providerlist;

	// Suchfeld
	wxSearchCtrl	*wxSearchField;

	// Liste
	wxSimpleHtmlListBox *wxBeChoiceList;
	int					iLastIndex;

	// Liste der Einträge
	std::vector< substancedata > m_list;

	// Buttonleiste
	wxButton* m_buttonpreferences;	// Button: Einstellungen (Filter, Hersteller, etc.)
	wxButton* m_buttonsearch;		// Button: Suchen
	wxButton* m_buttonContinue;		// Button: Weiter/Ok
	wxButton* m_buttonCancel;		// Button: Abbrechen

	// Enthält die gewünschten Datenbanken
	std::vector< int > m_selectedDB;

	// Exakte Suche?
	bool	bExactMatch;

	// Daten in lokale Datenbank übernehmen?
	bool	bSaveLocal;

	// Protokoll anzeigen?
	bool    bEnableLogging;

	// Substanzdaten
	CBerylliumSubstanceData data;

	// Erstellt die Dialogelemente
	void CreateLayout();

	// Löscht die Liste und fügt einen Standardeintrag hinzu
	void EmptyList();

	// Fügt einen Eintrag hinzu
	void AddToList( const wxString name, IProvider * provider, const int serial, const wxString url, const wxString subtitle );

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

	// Wird aufgerufen, wenn ein neuer Eintrag in der Liste gewählt wird
	void OnChoice( wxCommandEvent &event );

	// Wird aufgerufen, wenn im Suchfeld Enter gedrückt wurde (oder die Lupe angeklickt wurde)
	void OnSearch( wxCommandEvent &event );

	// Wird aufgerufen, wenn ein Link in einer Zelle geklickt wurde
	void OnLinkClick( wxHtmlLinkEvent& event );

	// Wird aufgerufen, wenn auf "Optionen" geklickt wurde
	void OnPreferences( wxCommandEvent &event );

	// Wird aufgerufen, wenn eine der "Datenbanken" im Menu angeklickt wurde
	void OnSelectSource( wxCommandEvent &event );

	// Wird aufgerufen, wenn eine der "Suchoptionen" im Menu angeklickt wurde
	void OnSearchPreferences( wxCommandEvent &event );

	// Wird aufgerufen, wenn "lokale Datenbank" ausgewählt wurde
	void OnSearchSelectLocalDatabase( wxCommandEvent &event );

DECLARE_EVENT_TABLE()
};
