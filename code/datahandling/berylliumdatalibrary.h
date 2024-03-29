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

#include "berylliumdata.h"

#include <wx/xml/xml.h>

// Stellt eine Bibliothek von Data-Objekten da. Kapselt vor allem die Funktionalitäten
// zum Lesen und Schreiben von XML-Bibliothekdateien.
class CBerylliumDataLibrary
{
public:
	CBerylliumDataLibrary(void);
	~CBerylliumDataLibrary(void);

private:
	std::vector< CBerylliumSubstanceData > m_vData;

public:
	// Operator zum schnellen Zugriff auf Objekte
	CBerylliumSubstanceData const& operator[]( int iIndex );
	CBerylliumSubstanceData const& GetItem( int iIndex );

	// Gibt die Anzahl der Element zurück
	int NumberOfSubstances();

	// Löscht alle Elemente
	void Clear();

	// Element hinzufügen
	void Add( const CBerylliumSubstanceData &add );

	// Sortiert die Element nach Namen (GetName())
	void SortByNames();

	// Sucht ein Element anhand der CAS-Nummer
	int LookForCAS( const wxString cas );

	// Lesen- und Schreibemethoden (root gibt jeweils das <library>-Element an)
	void LoadFromXML( wxXmlNode *root );
	void SaveToXml( wxXmlNode *root );

	// Lesen- und Schreibemethoden für die ganze Datei
	void LoadFromFile( const wxString sFileName );
	void SaveToFile( const wxString sFileName );

private:
	// Statische Sortierfunktion (nach Name)
	static bool StaticSortByNames( const CBerylliumSubstanceData &left, const CBerylliumSubstanceData &right )
	{
		return ( left.GetName().compare( right.GetName() ) < 0 );
	}

};
