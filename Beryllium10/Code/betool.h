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

// Einige Funktionen, die man öfters braucht, sind hier in einem globalen Namespace
// definiert. Einige von diesen Funktionen könnte man zwar teilweise einfach durch
// entsprechende Zeilen und Funktionen aus den Standardbibliotheken oder wxWidgets
// ersetzen. Der Vorteil hier ist aber: man kann sie hier einfacher und besser anpassen.

#pragma once

// Vektoren
#include <vector>

// Namespace: BeTool mit einigen universalen Hilfsfunktionen
namespace BeTool
{
	// Liefert für einen bestimmten Wert das Prefix und passt den Wert an 
	// die entsprechende Dimension an
	wxString UnitPrefix( double &dbValue );

	// Wandelt einen String in eine Fließkommazahl um
	// -> ersetzt , durch .
	// -> ignoriert alle Zeichen nach dem ersten Leerzeichen
	double StringToDouble( wxString s );

	// Erzeugt aus einem String eine Liste und schiebt sie in einen Vektor
	void StringToList( wxString s, wxString sep, std::vector< wxString > &sList, bool bClear = true );

	// Macht aus einer Liste einen String 
	wxString ListToString( const std::vector< wxString > & sList, wxString sep );

	// Trimmt einen String
	void trim( wxString &str );
}