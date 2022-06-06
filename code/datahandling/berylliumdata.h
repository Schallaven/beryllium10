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


#include <vector>

#include <wx/datetime.h>

// Substanzdaten-Objekt einbinden
#include "berylliumsubstancedata.h"



// Enthält die ganzen Daten für die Substanzen, Sicherheiten, etc.
class CBerylliumData
{
public:
	// Konstruktor
	CBerylliumData(void);

	// Destruktor
	~CBerylliumData(void);

public:
	// Titel des Datenblattes (normal)
	wxString		szTitle;
	// Es gibt noch einen GHS-Titel
	wxString		szGHSTitle;

	// Allgemeine Infos
	wxString		szLocation;		// Ort
	wxString		szEvent;		// Veranstaltung, ehemalig: szChemistry
	wxString		szName;			// Name des Praktikanten
	wxString		szPrename;		// Vorname des Praktikanten (wird nicht länger benutzt)
	wxString		szPosition;		// Platznummer des Praktikanten
	wxString		szSupervisor;	// Assistent, ehemalig: szAssistent

	// Daten für die Rückseite
	std::vector< wxString > sCustomStatements[4];

	// Unterschriftsboxen, Rückseite
	wxString		szSignaturStudent;		// Unterschrift Student
	wxString		szSignaturSupervisor;	// Unterschrift Assistent
	wxString		szStatementStudent;		// Hiermit erkläre ich...
	wxString		szStatementSupervisor;	// Zur Synthese/Bearbeitung freigegeben...

	// Das was man machen will (Präparat/Verfahren)
	CBerylliumSubstanceData sdPreparation;

	// Substanzen die dafür benötigt werden
	std::vector< CBerylliumSubstanceData > sdCompounds;

	// Get: Substanz mit Index index zurückgeben
	CBerylliumSubstanceData GetSubstanceByIndex( unsigned int index ) const;

	// Set: Überschreibt die Substanzdaten bei Index index mit newdata
	void UpdateSubstanceDataAtIndex( unsigned int index, CBerylliumSubstanceData newdata );

	// Get: Name des Praktikanten
	wxString GetName() const;

	// Set: Name des Praktikanten
	void SetName( const wxString &name );

	// Multipliziert alle Substanzen mit dem entsprechenden Faktor
	void MultiplyBatchesBy( double factor );

	// Hilfsfunktion: Tauscht zwei Substanzen aus
	void SwitchSubstancesByIndex( unsigned int indexA, unsigned int indexB );

	// Hilfsfunktion: Sortiert Substanzen nach Name
	void SortSubstancesByName();

	// Hilfsfunktion: Sortiert Substanzen nach Menge
	void SortSubstancesByAmount();

	// Hilfsfunktion: Sortiert Substanzen nach Gefährlichkeit für den Anwender
	void SortSubstancesByDanger();

};
