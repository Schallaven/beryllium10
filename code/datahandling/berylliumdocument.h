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
#include <string>

#include "../interfaces/iobserver.h"
#include "berylliumdata.h"

#include <wx/xml/xml.h>

// Dokumentklasse
class CBerylliumDocument
{
public:
	// Konstruktor
	CBerylliumDocument(void);

	// Destruktor
	~CBerylliumDocument(void);

	// Daten aus Datei laden
	int LoadFromFile( wxString szFilename );

	// Daten in Datei speichern
	int SaveToFile();

	// Fügt einen Beobachter hinzu
	void AddObserver( IObserver* object );

	// Entfernt einen Beobachter...
	void RemoveObserver( IObserver* object );

	// Daten wurden geändert, d.h. alle Beobachter benachrichtigen
	void NotifyAllObserver();

	// Get: Wurden die Daten geändert?
	bool HasChanged() { return m_bChanged; };
	// Set: Daten wurden gespeichert
	void Saved() { m_bChanged = false; };

	// Set: Dateiname
	void SetFilename( const wxString strNewName );
	// Get: Dateiname
	wxString GetFilename() { return m_strFilename; };
	// Wurde ein Dateiname gesetzt?
	bool HasFilename() { return ((m_strFilename.compare("Unbenannt") != 0) && (m_strFilename.length() > 0) && (m_strPath.length() > 0)); };

	// Set: Pfad
	void SetPath( const wxString strNewPath ) { m_strPath = strNewPath; };
	// Get: Pfad
	wxString GetPath() { return m_strPath; };

	// Set: Daten
	void SetData( const CBerylliumData &newData );
	// Get: Daten
	void GetData( CBerylliumData &Data ) { Data = m_data; } ;
	const CBerylliumData &GetData() { return m_data; };

public:
	// Fügt eine Substanz in das Datenobjekt ein und
	// gibt den Index zurück
	unsigned int AddSubstance( const CBerylliumSubstanceData &substance );

	// Zum GHS-Modus wechseln
	void SwitchToGHS( bool bSwitch = true ) { bGHS = bSwitch; };

	// GHS-Modus geladen?
	bool IsGHSMode() { return bGHS; };

	// Multipliziert alle Substanzen mit dem entsprechenden Faktor
	void MultiplyBatchesBy( double factor );

	// Spalten, die gezeigt werden sollen
	std::vector< wxString > m_showcolumns;

	// Sätze die versteckt werden sollen
	std::vector< wxString > m_hidephrases;

private:
	// Liste aller Beobachter
	std::vector< IObserver* > m_Observers;

	// Flag: Daten geändert
	bool m_bChanged;

	// Dateiname
	wxString m_strFilename;

	// Pfad
	wxString m_strPath;

	// Daten des Models
	CBerylliumData m_data;

	// GHS-Modus?
	bool bGHS;

	// Hilfsfunktion: Lädt "General" Abschnitt aus einem XML-Knoten
	void LoadGeneralFromXmlNode( wxXmlNode *parent );

	// Hilfsfunktion: Lädt "Personal" Abschnitt aus einem XML-Knoten
	void LoadPersonalFromXmlNode( wxXmlNode *parent );

	// Hilfsfunktion: Lädt "Product" Abschnitt aus einem XML-Knoten
	void LoadProductFromXmlNode( wxXmlNode *parent );

	// Hilfsfunktion: Lädt "Templates" Abschnitt aus einem XML-Knoten
	void LoadTemplatesFromXmlNode( wxXmlNode *parent );

	// Hilfsfunktion: Lädt "Options" Abschnitt aus einem XML-Knoten
	void LoadOptionsFromXmlNode( wxXmlNode *parent );
};
