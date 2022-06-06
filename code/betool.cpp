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

#include "beryllium.h"

#include "betool.h"

// Liefert für einen bestimmten Wert das Prefix und passt den Wert an
// die entsprechende Dimension an
wxString BeTool::UnitPrefix( double &dbValue )
{
	// Zahl gleich 0 ?
	if (dbValue == 0.0)
		return "";

	// mögliche Präfixe und die zugehörigen Exponenten
	const char	*prefixes[]	= { "p", "n", "µ", "m", "", "k", "M", "G", "T" };
	double	exponents[] = { 1.0e-12,  1.0e-9, 1.0e-6, 1.0e-3, 1, 1.0e3, 1.0e6, 1.0e9, 1.0e12 };

	// Alle Möglichkeiten durchgehen
	for ( int i = 0; i < 9; i++ )
	{
		// die Zahl einfach durch den entsprechenden Exponenten teilen
		double dbNewValue = dbValue / exponents[i];

		// Ist der Wert jetzt vernünftig lesbar? (zwischen 500 und 0.5)
		if ( dbNewValue < 500.0 )
		{
			// dann Wert übernehmen und Präfix zurückgeben
			dbValue = dbNewValue;
			return prefixes[i];
		}
	}

	// Kein Präfix verfügbar (sollte eigentlich nur bei sehr großen und bei sehr kleinen Zahlen passieren)
	return "";
}

// Wandelt einen String in eine Fließkommazahl um
// -> ersetzt , durch .
// -> ignoriert alle Zeichen nach dem ersten Leerzeichen
double BeTool::StringToDouble( wxString s )
{
	// Temporären String anlegen
	wxString temp(s);

	// Evtl. "," durch "." ersetzen
	temp.Replace( ",", ".", true );

	// Double-Variable
	double d = 0.0;

	// Umwandeln (ToCDouble arbeitet mit C-Lokalisierung, d.h. immer mit Punkt)
	temp.ToCDouble(&d);

	// Zurückgeben
	return d;
}

// Erzeugt aus einem String eine Liste und schiebt sie in einen Vektor
void BeTool::StringToList( wxString sLine, wxString sSeparator, std::vector<wxString> &sList, bool bClear )
{
	// Liste löschen
	if ( bClear )
		sList.clear();

	// String durchgehen, Trennzeichen suchen und in Liste schieben
	while ( sLine.size() > 0 )
	{
		BeTool::trim(sLine);

		int a = sLine.find( sSeparator );

		// Falls kein Trennzeichen gefunden -> der ganze String ist ein Name
		if ( a == -1 )
		{
			sList.push_back(sLine);
			break;
		}

		// Substring holen, pushen und entfernen
		wxString sSub = sLine.substr(0,a); BeTool::trim(sSub);

		// Substring speichern... aber nur, wenn er nicht leer ist
		if ( sSub.length() > 0 )
			sList.push_back(sSub);

		// Aus dem String löschen
		sLine.erase(0,a+1);
	}
}

// Macht aus einer Liste einen String
wxString BeTool::ListToString( const std::vector< wxString > &sList, wxString sep )
{
	// String erstellen
	wxString sLine;

	// Liste durchgehen und alle Elemente hinzufügen
	for ( unsigned int i = 0; i < sList.size(); i++ )
	{
		if ( i > 0)
			sLine += sep;

		sLine += sList[i];
	}

	// String zurückgeben
	return sLine;
}


// Trimmt einen String
void BeTool::trim( wxString &str )
{
	// Letztes Nicht-Leerzeichen finden
	wxString::size_type pos = str.find_last_not_of(' ');

	// Gefunden?
	if( pos != wxString::npos )
	{
		// Alles danach löschen
		str.erase(pos + 1);

		// Erstes Nicht-Leerzeichen suchen
		pos = str.find_first_not_of(' ');

		// Gefunden? Dann: Bis dorthin löschen!
		if ( pos != wxString::npos )
			str.erase(0, pos);
	}

	// Ansonsten den ganzen String leeren (besteht also quasi nur aus Leerzeichen)
	else
		str.erase(str.begin(), str.end());
}
