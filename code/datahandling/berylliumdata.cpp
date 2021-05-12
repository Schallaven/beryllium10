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

#include "../stdafx.h"

#include "berylliumdata.h"

// Konstruktor
CBerylliumData::CBerylliumData(void)
{
	szTitle			=	_(L"Versuchsbezogene Betriebsanweisungen nach §20 GefStoffV");
	szGHSTitle		=   _(L"Versuchsbezogene Betriebsanweisungen nach EG Nr. 1272/2008");
	szLocation		=	_(L"für chemische Laboratorien der Universität Regensburg");
	szEvent			=	_(L"Grundpraktikum Organische Chemie");

	szName			=   L"Doe";
	szPrename		=	L"Jane";
	szPosition		=	L"123";
	szSupervisor	=	L"Smith, A.";

	szSignaturStudent		= _(L"Unterschrift des (der) Student(in)");
	szSignaturSupervisor	= _(L"Unterschrift des (der) Assistent(in)");
	szStatementStudent		= _(L"Hiermit verpflichte ich mich, den Versuch gemäß den in dieser Betriebsanweisung aufgeführten Sicherheitsvorschriften durchzuführen.");
	szStatementSupervisor	= _(L"Präparat zur Synthese mit den auf der Vorderseite berechneten Chemikalienmengen freigegeben.");
}

// Destruktor
CBerylliumData::~CBerylliumData(void)
{
}

// Get: Setzt den kompletten Namen zusammen...
wxString CBerylliumData::GetName() const
{
	return szName + " " + szPrename;
}

// Set: Setzt den kompletten Namen...
void CBerylliumData::SetName( const wxString &name )
{
	// Vorname löschen
	szPrename = "";

	// Namen setzen
	szName = name;
}

CBerylliumSubstanceData CBerylliumData::GetSubstanceByIndex( unsigned int index ) const
{
	// Leere Substanzdaten erstellen
	CBerylliumSubstanceData data;

	// Index außerhalb
	if ( index >= sdCompounds.size() )
		return data;

	// Ansonsten entsprechenden Index zurückgeben
	return sdCompounds[index];
}

void CBerylliumData::UpdateSubstanceDataAtIndex( unsigned int index, CBerylliumSubstanceData newdata )
{
	// Index außerhalb
	if ( index >= sdCompounds.size() )
		return;

	// Überschreiben
	sdCompounds[index] = newdata;

	// Geändert-Datum setzen
	sdCompounds[index].source.changed = wxDateTime::Now().Format("%d. %B %Y %H:%M");
}

// Multipliziert alle Substanzen mit dem entsprechenden Faktor
void CBerylliumData::MultiplyBatchesBy( double factor )
{
	// Anzahl der Substanzen
	unsigned int count = sdCompounds.size();

	// Alle Substanzen durchgehen und multiplizieren
	for (unsigned int i = 0; i < count; ++i )
		sdCompounds[i].MultiplyBatchBy( factor );
}

// Hilfsfunktion: Tauscht zwei Substanzen aus
void CBerylliumData::SwitchSubstancesByIndex( unsigned int indexA, unsigned int indexB )
{
	// Indizes überprüfen
	if ( (indexA >= sdCompounds.size()) || (indexB >= sdCompounds.size())  )
		return;

	// Temporäres Objekt
	CBerylliumSubstanceData temp = sdCompounds[indexA];

	// Zuerst A mit B überschreiben
	sdCompounds[indexA] = sdCompounds[indexB];

	// Dann B mit A überschreiben (aus temp)
	sdCompounds[indexB] = temp;
}

// Hilfsfunktion: Sortiert Substanzen nach Name
void CBerylliumData::SortSubstancesByName()
{
	// Anzahl der Substanzen
	unsigned int count = sdCompounds.size();

	// Mindestens zwei Elemente vorhanden?
	if ( count < 2 )
		return;

	// Alle Substanzen durchgehen und mit allen anderen Vergleichen
	// Implementation von BubbleSort, Version 3 aus Wikipedia
	do
	{
		// "Neue" Anzahl
		unsigned int newcount = 1;

		// Alle Substanzen durchgehen
		for (unsigned int i = 0; i < (count-1); ++i )
		{
			// Namen holen
			wxString sortname = sdCompounds[i].GetSortableName();

			// Substanznamen vergleichen
			int result = sortname.compare( sdCompounds[i+1].GetSortableName() );

			// Gleich? Dann genaue Bezeichnung vergleichen!
			if ( result == 0 )
				result = sdCompounds[i].GetName().compare( sdCompounds[i+1].GetName() );

			// result > 0? Dann vertauschen!
			if ( result > 0 )
			{
				// Tauschen
				SwitchSubstancesByIndex( i, i+1 );

				// Zähler hochsetzen
				newcount = i + 1;
			}
		}

		// Setzen
		count = newcount;

	} while (count > 1);

}

// Hilfsfunktion: Sortiert Substanzen nach Menge
void CBerylliumData::SortSubstancesByAmount()
{
	// Anzahl der Substanzen
	unsigned int count = sdCompounds.size();

	// Mindestens zwei Elemente vorhanden?
	if ( count < 2 )
		return;

	// Alle Substanzen durchgehen und mit allen anderen Vergleichen
	// Implementation von BubbleSort, Version 3 aus Wikipedia
	do
	{
		// "Neue" Anzahl
		unsigned int newcount = 1;

		// Alle Substanzen durchgehen
		for (unsigned int i = 0; i < (count-1); ++i )
		{
			// Substanzmengen vergleichen und entsprechend sortieren
			if ( sdCompounds[i].CalculateAmountOfSubstance() < sdCompounds[i+1].CalculateAmountOfSubstance() )
			{
				// Tauschen
				SwitchSubstancesByIndex( i, i+1 );

				// Zähler hochsetzen
				newcount = i + 1;
			}
		}

		// Setzen
		count = newcount;

	} while (count > 1);
}

// Hilfsfunktion: Sortiert Substanzen nach Gefährlichkeit für den Anwender (Berylliumpunkte!)
void CBerylliumData::SortSubstancesByDanger()
{
	// Anzahl der Substanzen
	unsigned int count = sdCompounds.size();

	// Mindestens zwei Elemente vorhanden?
	if ( count < 2 )
		return;

	// Alle Substanzen durchgehen und mit allen anderen Vergleichen
	// Implementation von BubbleSort, Version 3 aus Wikipedia
	do
	{
		// "Neue" Anzahl
		unsigned int newcount = 1;

		// Alle Substanzen durchgehen
		for (unsigned int i = 0; i < (count-1); ++i )
		{
			// Gefahrenpotential vergleichen und entsprechend sortieren
			if ( sdCompounds[i].GetDangerPoints() < sdCompounds[i+1].GetDangerPoints() )
			{
				// Tauschen
				SwitchSubstancesByIndex( i, i+1 );

				// Zähler hochsetzen
				newcount = i + 1;
			}
		}

		// Setzen
		count = newcount;

	} while (count > 1);
}
