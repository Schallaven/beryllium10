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

// Klasse für das Hauptfenster
#include "BerylliumMainframe.h"

// Klasse für die Statements
#include "Datahandling/BerylliumStatementsManager.h"

// Hash-Map
#include <wx/hashmap.h>

// Hashmap-Typ für Config-Daten anlegen
WX_DECLARE_STRING_HASH_MAP( wxString, configHashMapTyp );

// Anwendungsklasse
class CBerylliumApplication : public wxApp
{
private:
	// Lokalisierungsobjekt: Wird für die Übersetzungen benötigt
	wxLocale m_Locale;

	// Version-Infos
	struct versionblock {
		long major;
		long minor;
		long revision;
		long build;
		wxString sUrl;
	} version_update;

	// Icon
	wxIcon m_icon;

	// Objekt, dass die ganzen H/P/R/S-Sätze enthält
	CBerylliumStatementsManager m_Statements;

	// Objekt, dass die ganzen Vorlagen (für die Rückseite) enthält
	CBerylliumStatementsManager m_Templates;

	// Hash-Map für unsere ganzen Einstellungen
	configHashMapTyp m_configHashMap;

public:
	// Initialisiert die Anwendung
	virtual bool OnInit();

	// Wird beim Verlassen aufgerufen
	virtual int OnExit();

	// Kopie der Bibliothek der ganzen H/P/R/S-Sätze
	const CBerylliumStatementsManager &GetStatementLibrary() { return m_Statements; };

	// Kopie der Bibliothek der Vorlagen für die Rückseite
	const CBerylliumStatementsManager &GetTemplatesLibrary() { return m_Templates; };

	// Get: Sprache
	long GetLanguage() { return m_iLanguage; };

	// Set: Sprache
	void SetLanguage( long newlanguage );

	// Get: Configdaten
	wxString GetConfigData( const wxString &key ) { return m_configHashMap[key]; };

	// Set: Configdaten
	void SetConfigData( const wxString &key, const wxString &value ) { m_configHashMap[key] = value; };

	// Check: Lädt die update.xml vom Server und überprüft, ob die Version noch aktuell ist
	bool IsThereANewVersion();

	// Ruft die Update-Seite (wie in der update.xml angegeben) im Browser auf
	void OpenUpdatePage();

private:
	// Gesetzte Sprache
	long m_iLanguage;

	// Lädt die Config-Datei und setzt die entsprechenden Optionen
	void LoadConfig();

	// Speichert die Config-Datei
	void SaveConfig();

	// Lädt alle oft genutzen Grafiken in den Zwischenspeicher
	void LoadGraphicsToMemory();

	// Lädt und setzt die Sprache
	void InitLocale();

	// H/P/R/S-Sätze-Datei laden
	void LoadStatements();

	// Vorlagen-Datei laden
	void LoadTemplates();

	// Lädt die Update.xml-Datei vom Server und speichert den Inhalt
	void LoadUpdateData();

	// Lädt die Daten aus der Update.xml und kopiert sie nach version_update
	void LoadUpdateDataContent( const wxString &data );
};

// Applications-Objekt festsetzen
DECLARE_APP(CBerylliumApplication)
