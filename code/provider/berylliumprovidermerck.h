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
#include "../interfaces/iprovider.h"

class CBerylliumProviderMerck : public IProvider
{
public:
	// Konstruktor
	CBerylliumProviderMerck(void);

	// Destruktor
	~CBerylliumProviderMerck(void);

private:
	// Land speichern
	int iCountry;

	// "Chemical formula" speichern, falls verfügbar und notfalls einsetzen (falls keine Hill-Formel)
	wxString chemicalformula;

public:
	// 1. Suchanfrage stellen
	bool SearchForCompound( const wxString searchtext, bool bExactMatch );

	// 2. Daten mit entsprechender ID holen
	void GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data );

	// Get: Herstellername
	wxString GetName() const { return "Merck"; };

	// Get: Liste mit Substanzen holen
	void GetResults( std::vector< IProvider::result > &m_list ) const;

private:
	// Name der Substanz
	wxString substancename;

	// Temporäre Daten
	CBerylliumSubstanceData tempdata;

	// Results
	std::vector< IProvider::result > m_results;

	// Lädt die Suchergebnisse
	bool LoadSearchData();

	// Lädt eine Zelle der Suchergebnisse
	void LoadSearchDataCell( wxXmlNode *cell );

	// Lädt die Daten für die Substanz unter URL
	void LoadDataFromUrl( wxString URL );

	// Lädt die Daten der Substanz
	void LoadSubstanceData();

	// Lädt einen Block der Substanzdaten
	void LoadSubstanceDataBlock( wxXmlNode *parent );

	// Lädt ein Schlüssel/Werte-Paar
	void LoadSubstanceDataTupel( wxString key, wxString value );

	// Hilfsfunktion: Entfernt alle unwichtigen Dinge, um den String kleiner zu machen
	void PreParseData();

	// Hilfsfunktion: Entfernt Größer/Kleiner Zeichen mit Leerzeichen
	void RemoveEmptyBrackets( wxString &data );
};
