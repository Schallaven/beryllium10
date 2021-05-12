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

#include <sstream>

#include "interfaces/iobserver.h"
#include "datahandling/berylliumstatementsmanager.h"

#include "berylliumviewmanager.h"

// Kümmert sich um alles was das Hauptfenster betrifft...
class CBerylliumMainframe : public wxFrame, public IObserver
{
public:
	// Konstruktor
	CBerylliumMainframe(const wxChar *title, int xpos, int ypos, int width, int height, bool bEasterEggs);

	// Destructor
	~CBerylliumMainframe(void);

	// Hilfsfunktionen (Konstruktor): Erstellt das Menu
	void BeCreateMenuBar(void);

	// Hilfsfunktionen (Konstruktor): Erstellt die Statusleiste
	void BeCreateStatusBar(void);

	// Hilfsfunktionen (Konstruktor): Erstellt die Symbolleiste
	void BeCreateToolBar(void);

	// IObserver: Model wurde verändert
	void ModelHasChanged();

	// Entfernt alle Häkchen von den Ansichten und setzt ein Häkchen bei der ersten Ansicht
	void ResetViewMenu();

private:
	// Menuzeile
	wxMenuBar *m_MenuBar;

	// ViewManager
	CBerylliumViewManager m_ViewManager;

	// Enthält das Model
	CBerylliumDocument m_Document;

	// Easter-Eggs :)
	bool	bEasterEggsEnabled;

public:
	// Ereignis: Fenstergröße wurde geändert...
	void OnSize( wxSizeEvent& event );

	// Ereignis: Vorherige Seite
	void OnEventPageLeft( wxCommandEvent &event );

	// Ereignis: Nächste Seite
	void OnEventPageRight( wxCommandEvent &event );

	// Ereignis: Neue Datei
	void OnEventNewFile( wxCommandEvent &event );

	// Ereignis: Datei öffnen
	void OnEventOpenFile( wxCommandEvent &event );

	// Ereignis: Datei speichern
	void OnEventSaveFile( wxCommandEvent &event );

	// Ereignis: Datei speichern unter...
	void OnEventSaveFileAs( wxCommandEvent &event );

	// Ereignis: Datei drucken...
	void OnEventPrintFile( wxCommandEvent &event );

	// Ereignis: Druckvorschau
	void OnEventPrintPreview( wxCommandEvent &event );

	// Ereignis: Zeige Symbolleiste
	void OnEventShowToolBar( wxCommandEvent &event );

	// Ereignis: Zeige Statusleiste
	void OnEventShowStatusBar( wxCommandEvent &event );

	// Ereignis: Wechsel die Ansicht
	void OnEventChangeView( wxCommandEvent &event );

	// Ereignis: Substanzmengen-Multiplikator
	void OnEventSubstanceMultiplier( wxCommandEvent &event );

	// Ereignis: Einstellungen
	void OnEventOptions( wxCommandEvent &event );

	// Ereignis: Sortieren
	void OnEventSort( wxCommandEvent &event );

	// Ereignis: Programm beenden
	void OnEventExit( wxCommandEvent &event );

	// Ereignis: Hilfethemen anzeigen
	void OnEventHelpTopics( wxCommandEvent &event );

	// Ereignis: Infobox anzeigen
	void OnEventInfoBox( wxCommandEvent &event );

	// Ereignis: Breakout anzeigen
	void OnEventBreakout( wxCommandEvent &event );

	// Ereignis: Auf Updates überprüfen
	void OnEventUpdateCheck( wxCommandEvent &event );

	// Ereignis: Bugreport angwählt
	void OnEventBugReport( wxCommandEvent &event );

	// Ereignis: GHS benutzen
	void OnEventSwitchGHS( wxCommandEvent &event );

	// Ereignis: Popupmenu geclickt
	void OnEventPopupMenuClick( wxCommandEvent &event );

	// Ereignis: Wird ausgeführt, wenn das Hauptfenster geschlossen wird
	void OnClose( wxCloseEvent &event );

	// Öffnet eine Datei (Kommandozeile)
	void OpenFile( wxString file );

protected:
	DECLARE_EVENT_TABLE()
};
