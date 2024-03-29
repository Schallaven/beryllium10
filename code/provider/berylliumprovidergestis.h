﻿/***********************************************************************************
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

class CBerylliumProviderGESTIS :
	public IProvider
{
public:
	CBerylliumProviderGESTIS(void);
	~CBerylliumProviderGESTIS(void);

	public:
	// 1. Suchanfrage stellen
	bool SearchForCompound( const wxString searchtext, bool bExactMatch );

	// 2. Daten mit entsprechender ID holen
	void GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data );

	// Get: Herstellername
	wxString GetName() const { return "GESTIS"; };

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

	// Lädt ein Element der Suchergebnisse
	void LoadSearchDataItem( wxXmlNode *cell );

	// Lädt die Daten für die Substanz unter URL
	void LoadDataFromUrl( wxString URL );

	// Lädt die Daten der Substanz
	void LoadSubstanceData();

	// Lädt den Kopfdatenblock der Substanzdaten
	void LoadSubstanceDataKopfdaten( wxXmlNode *parent );

	// Lädt einen "Aliasname"-Block aus den Kopfdaten
	void LoadSubstanceDataAliasname( wxXmlNode *parent );

	// Lädt rekursiv einen Block (beginnend vom Hauptkapitel) der Substanzdaten
	void LoadSubstanceDataBlock( wxXmlNode *parent, wxString topic = "" );

	// Lädt ein Schlüssel/Werte-Paar
	void LoadSubstanceDataTupel( wxString key, wxString value, wxString topic = "" );

};
