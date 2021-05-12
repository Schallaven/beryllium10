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
#include "../interfaces/iprovider.h"

// Sucht in einem entsprechenden Wikipedia-Artikel
// Besonderheit: Enthält nur EINE Substanz.
class CBerylliumProviderWiki : public IProvider
{
public:
	// Konstruktor
	CBerylliumProviderWiki(void);

	// Destruktor
	~CBerylliumProviderWiki(void);

public:
	// 1. Suchanfrage stellen
	bool SearchForCompound( const wxString searchtext, bool bExactMatch );

	// 2. Daten mit entsprechender ID holen
	void GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data );

	// Get: Herstellername
	wxString GetName() const { return "Wikipedia"; };

	// Get: Liste mit Substanzen holen
	void GetResults( std::vector< result > &m_list ) const;

private:
	// Enthält die Elementsymbole (für englische Wiki interessant)
	static const char *cElements[112];

	// Temporärer Daten-Block
	CBerylliumSubstanceData tempdata;

	// Seiteninhalt
	wxString content;

	// Suchstring (Wird als Name eingefügt)
	wxString searchname;

	// Temporäre Summenformel
	wxString tempSumformula;

	// Lädt die Daten aus einem String
	bool LoadData( const wxString data );

	// Parst die Daten
	bool ParseData();

	// Hilfsfunktion: Parst die Infobox für Chemikalien
	void ParseInfoBoxSubstance( wxString szBlock );

	// Hilfsfunktion: Parst die Infobox für Elemente
	void ParseInfoBoxElement( wxString szElement );

	// Hilfsfunktion: Parst einen der Section-Blocks der englischen Wiki
	void ParseSectionBox( wxString szBlock );

	// Hilfsfunktion: Parst 'drugbox' der englischen Wiki
	void ParseDrugBox( wxString szBlock );

	// Hilfsfunktion: Gibt alles zurück was zwischen {{ und }} zu finden ist
	// und entfernt es aus dem Text
	wxString ExtractFromBrackets( wxString &szText );

	// Hilfsfunktion: Holt ein Parameter <-> Wert - Tupel aus der Infobox
	void GetParameterTuple( wxString &szText, wxString &szName, wxString &szValue );

	// Hilfsfunktion: Parst "szValue" eines Parameter <-> Wert - Tupels
	void ParseParameterValue( wxString &szValue );

	// Hilfsfunktion: Parst "szValue" eines Listen-Parameters und fügt die Werte in Liste ein
	void ParseParameterList( std::vector< wxString > &sList, wxString sSymbols );
};
