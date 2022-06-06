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
#include "../datahandling/berylliumdatalibrary.h"

class CBerylliumProviderLocal :	public IProvider
{
public:
	// Konstruktor
	CBerylliumProviderLocal(void);

	// Destruktor
	~CBerylliumProviderLocal(void);

public:
	// 1. Suchanfrage stellen
	bool SearchForCompound( const wxString searchtext, bool bExactMatch );

	// 2. Daten mit entsprechender ID holen
	void GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data );

	// Get: Herstellername
	wxString GetName() const { return _("Lokale Datenbank"); };

	// Get: Liste mit Substanzen holen
	void GetResults( std::vector< result > &m_list ) const;

private:
	// Bibliothek
	CBerylliumDataLibrary library;

	// Dateiname der Bibliothek
	wxString libraryfile;

	// Results
	std::vector< IProvider::result > m_results;
};
