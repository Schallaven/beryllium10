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

#include "berylliumsubstancedata.h"

#include "../berylliumapplication.h"
#include "../datahandling/berylliumstatementsmanager.h"
#include "../betool.h"

// Anmerkung: Ja, ich weiß, dass "Harzard" falsch geschrieben ist ("hazard"). Blöder Fehler!
// Leider hat es sich durchs Abschreiben und Copy&Paste immer mehr vermehrt und irgendwann
// wollte und konnte ich es nicht mehr ändern. Nunja. Gibt schlimmeres :)

// Durchschnittsmensch bzw. Arbeitsplatz. Für Toxikologie-Daten ist es immer
// besser die Werte zu unterschätzen, d.h. bei geringeren Mengen schon auf die
// Gefahr hinzuweisen als bei größeren. Deswegen wird für die Masse eines
// "Durchschnittsmenschen" 50 Kilogramm angenommen und für einen "durchschnittlichen"
// Arbeitsplatz ein Abzug (dessen Lüftung ausgefallen ist) von 1.5 m³.
const double CBerylliumSubstanceData::AverageHumanMass			= 50.0f;	// in kg
const double CBerylliumSubstanceData::AverageWorkplaceVolume	= 1.5f;		// in m³

CBerylliumSubstanceData::CBerylliumSubstanceData(void)
{
	// Name(n) -> "Leere Substanz"
	szNames.push_back( _(L"Leere Substanzdaten") );

	// Molekulare Masse (g/mol)
	dbMolWeight	= 0.0;

	// Dichte (g/cm³)
	dbDensity = 1.0;

	// Summenformel
	SetFormula("");

	// Schmelzpunkt
	szMeltingPoint	= "";

	// Siedepunkt
	szBoilingPoint	= "";

	// Keine Zersetzung
	bDecomposition	= false;

	// Flammpunkt
	szFlashPoint	= "";

	// R-Sätze
	szRisks	= "";

	// S-Sätze
	szSafeties = "";

	// H-Sätze
	szHarzardSatements	= "";

	// P-Sätze
	szPrecautionaryStatements = "";

	// Signalwort
	szSignalWord = "";

	// Gruppe/Kategorie
	szGroup = "";

	// MAK-Wert
	dbMAK	= 0.0;
	bMAKppm = false;

	// WGK-Wert ( 0 = NWG )
	iWGK = 0;

	// Harzard-Symbole -> leer

	// Ansatz
	typeBatch			= batchSolid;
	dbBatch				= 0.0;
	dbConcentration		= 0.0;
}

CBerylliumSubstanceData::~CBerylliumSubstanceData(void)
{
}

wxString CBerylliumSubstanceData::GetName() const
{
	// Namen vorhanden?
	if ( szNames.size() > 0 )
		return szNames[0];

	// Ansonsten -> Leere Substanzdaten
	return _(L"Leere Substanzdaten");
}

// Get: Gibt einen sortierbaren Namen zurück (entfernt alle Zahlen, Klammern & co) und wandelt den String in Großbuchstaben um
wxString CBerylliumSubstanceData::GetSortableName() const
{
	// Kein Name vorhanden?
	if ( szNames.size() == 0 )
		return _(L"Leere Substanzdaten");

	// Temporäre Variablen
	wxString szSortableName, temp;

	// Name kopieren
	temp = szNames[0].Upper();

	// Länge des ersten Namen
	unsigned int count = temp.length();

	// Zeichen für Zeichen durchgehen
	for ( unsigned int i = 0; i < count; ++i )
	{
		// Nur lateinische Großbuchstaben übertragen
		if ( temp[i].IsAscii() && (temp[i] > 64) && (temp[i] < 91) )
			szSortableName += temp[i];
	}

	// Zurückgeben
	return szSortableName;
}

wxString CBerylliumSubstanceData::GetNames( wxString separator ) const
{
	// Kein Name vorhanden?
	if ( szNames.size() == 0 )
		return _(L"Leere Substanzdaten");

	// Rückgabewert vorbereiten
	wxString szReturn = szNames[0];

	// Andere Werte anhängen
	for ( unsigned int i = 1; i < szNames.size(); ++i )
	{
		szReturn += separator;
		szReturn += szNames[i];
	}

	// Zurückgeben
	return szReturn;
}

// Get: Gibt alle Namen außer dem ersten in einen String zurück, mit separator getrennt
wxString CBerylliumSubstanceData::GetNamesExceptFirst( wxString separator ) const
{
	// Kein Name vorhanden?
	if ( szNames.size() < 2 )
		return "";

	// Rückgabewert vorbereiten
	wxString szReturn = szNames[1];

	// Andere Werte anhängen
	for ( unsigned int i = 2; i < szNames.size(); ++i )
	{
		szReturn += separator;
		szReturn += szNames[i];
	}

	// Zurückgeben
	return szReturn;
}

wxString CBerylliumSubstanceData::GetShortestName() const
{
	// Kein Name vorhanden?
	if ( szNames.size() == 0 )
		return _(L"Leere Substanzdaten");

	// Kürzesten Namen finden
	wxString szShortName = szNames[0];

	for ( unsigned int i = 1; i < szNames.size(); i++ )
	{
		if ( szNames[i].length() < szShortName.length() )
			szShortName = szNames[i];
	}

	// Zurückgeben
	return szShortName;
}

// Get: Hat diese Substanz den Namen?
bool CBerylliumSubstanceData::HasName( const wxString name, bool bExact ) const
{
	// Name kopieren
	wxString lookfor = name; std::transform(lookfor.begin(), lookfor.end(), lookfor.begin(), toupper);

	// Namen durchgehen
	for ( unsigned int i = 0; i < szNames.size(); ++i )
	{
		// Namen kopieren
		wxString comparewith = szNames[i]; std::transform(comparewith.begin(), comparewith.end(), comparewith.begin(), toupper);

		// Exakte Suche
		if ( bExact && (comparewith.compare(lookfor) == 0) )
			return true;

		// Nicht exakte Suche
		else if ( !bExact && (comparewith.find(lookfor) != comparewith.npos) )
			return true;
	}

	// Nicht gefunden
	return false;
}

wxString CBerylliumSubstanceData::GetFormula() const
{
	return szFormula;
}

void CBerylliumSubstanceData::SetFormula( const wxString formula )
{
	// String-Repräsentation kopieren
	szFormula = formula;

	// Alte Formel löschen
	vFormula.clear();

	// Länge des Strings
	const int iLen = formula.length();

	// Modus ( Zahlen oder Text? )
	bool bNumeric = false;

	// Temporäre Formeleinheit
	SubstanceFormula temp;

	// Temporärer String
	wxString szTemp;

	// Zeichen für Zeichen den Formelstring parsen
	for ( int i = 0; i < iLen; ++i )
	{
		// Zeichen holen
		wxChar c = formula[i].GetValue();

		// Ist das Zeichen eine Zahl?
		if ( isdigit(c) )
		{
			// Waren wir vorher nicht im Zahlenmodus, dann
			// müssen wir den String noch in sSymbol verschieben
			if ( !bNumeric && (szTemp.length() > 0) )
			{
				// Verschieben
				temp.sSymbol = szTemp;

				// Leeren
				szTemp = "";
			}

			// Zahlenmodus
			bNumeric = true;

			// Anhängen
			szTemp += c;

			// Weitermachen
			continue;
		}

		// Das Zeichen ist keine Zahl... jetzt schauen wir ob es ein Steuerzeichen ist
		switch( c )
		{
			// Hoch- /Tiefstellzeichen werden ignoriert
			case '^':
			case '_':
				// Waren wir vorher im Zahlenmodus, dann müssen wir jetzt unbedingt
				// den temporären String als "tiefgestellt" markieren
				if ( szTemp.length() > 0 )
				{
					if ( bNumeric )
						// String "tiefstellen"
						temp.sLower = szTemp;

					// Andernfalls...
					else
						// String als "Symbol"
						temp.sSymbol = szTemp;
				}

				// Temporären String leeren
				szTemp = "";
				break;

			// Ladungssymbole -> Zahl bzw. das Zeichen wird hochgestellt...
			case '+':
			case '-':
				// Waren wir vorher nicht im Zahlenmodus, dann
				// müssen wir den String noch in sSymbol verschieben
				if ( !bNumeric && (szTemp.length() > 0) )
				{
					// Verschieben
					temp.sSymbol = szTemp;

					// Leeren
					szTemp = "";
				}

				// Fügt das Zeichen noch dem temporären String hinzu
				szTemp += c;

				// String "hochstellen", aber nur wenn in sSymbol schon etwas ist,
				// ansonsten einfach in sSymbol reinschieben
				if ( temp.sSymbol.length() > 0 )
					temp.sUpper = szTemp;
				else
					temp.sSymbol = szTemp;

				// Leeren
				szTemp = "";
				break;

			// * wird durch · ersetzt
			case '*':
				c = wxT('·');

			// Normales, darstellbares Zeichen
			default:
				// Waren wir vorher im Zahlenmodus, dann müssen wir jetzt unbedingt
				// den temporären String als "tiefgestellt" markieren, dann das komplette Paket
				// anhängen
				if ( bNumeric )
				{
					// Sollte das letzte Zeichen in sSymbol ein Leerzeichen gewesen sein ODER
					// der String komplett leer sein, dann wird die Zahl als normale Zeichenkette
					// angefügt.
					if ( (temp.sSymbol.length() == 0) || (*(temp.sSymbol.rbegin()) == ' ') )
					{
						// Zahl ins Symbol verschieben
						temp.sSymbol += szTemp;
					}

					// String "tiefstellen"
					else if (szTemp.length() > 0)
						temp.sLower = szTemp;

					// Anhängen
					vFormula.push_back(temp);

					// Temp leeren
					SubstanceFormula newtemp; temp = newtemp;

					// Leeren
					szTemp = "";
				}

				// Zeichen an String anhängen
				szTemp += c;

				// Raus aus dem Zahlenmodus...
				bNumeric = false;
				break;
		}
	}

	// Sind jetzt noch Zeichen im temporären String? Dann hängen wir die auch noch an!
	if ( szTemp.length() > 0 )
	{
		// Zahlen "tiefstellen", ansonsten als Symbol einfügen
		if ( bNumeric )
		{
			// Sollte das letzte Zeichen in sSymbol ein Leerzeichen gewesen sein ODER
			// der String komplett leer sein, dann wird die Zahl als normale Zeichenkette
			// angefügt.
			if ( (temp.sSymbol.length() == 0) || (*(temp.sSymbol.rbegin()) == ' ') )
			{
				// Zahl ins Symbol verschieben
				temp.sSymbol += szTemp;
			}

			// ansonsten tiefstellen
			else
				temp.sLower = szTemp;
		}
		else
			temp.sSymbol = szTemp;

		// Paket anfügen
		vFormula.push_back( temp );
	}

	// Ist noch eine nicht angehängte "temporäre" Formeleinheit da, dann hängen wir die auch noch an
	else if ( (temp.sLower.length() > 0) || (temp.sUpper.length() > 0) || (temp.sSymbol.length() > 0) )
		vFormula.push_back( temp );
}

void CBerylliumSubstanceData::ReplaceNamesBy( const wxString name )
{
	// Alle Namen löschen
	szNames.clear();

	// "name" anfügen
	szNames.push_back( name );
}

void CBerylliumSubstanceData::DrawFormula( wxDC &dc, int x, int y ) const
{
	// Wenn der Vektor leer ist, gleich rausspringen
	if ( vFormula.size() == 0 )
		return;

	// Anzahl der Elemente im Formelvektor
	const int iLen = vFormula.size();

	// Die Schrift holen
	wxFont fontTemp = dc.GetFont();

	// Kopie der Schrift machen
	wxFont fontLower = fontTemp;

	// und die Kopie um zwei Punkte verkleinern
	fontLower.SetPointSize( fontLower.GetPointSize() - 2 );

	// Position in x Richtung
	int iIndentX = 0;

	// Ansonsten jedes Element des Vektors durchgehen...
	for ( int i = 0; i < iLen; ++i )
	{
		// Normale Schrift selektieren
		dc.SetFont( fontTemp );

		// Dimension des Symbols
		wxSize sizeSymbol = dc.GetTextExtent( vFormula[i].sSymbol );

		// Symbol zeichnen
		dc.DrawText( vFormula[i].sSymbol, x + iIndentX, y );

		// Weiter nach Rechts bewegen
		iIndentX += sizeSymbol.GetWidth();

		// Gibt es weder Hoch- noch Tiefgestellte Sachen? (sollte zwar nicht vorkommen, aber naja...)
		// Dann einfach weiter machen
		if ( (vFormula[i].sLower.length() == 0) && (vFormula[i].sUpper.length() == 0) )
			continue;

		// zur kleineren Schrift wechseln
		dc.SetFont( fontLower );

		// Temporäre Einrückung in X-Richtung
		int iTempIndentX = 0;

		// Hochstellen
		if ( vFormula[i].sUpper.length() > 0 )
		{
			// Größe bestimmen
			wxSize sizeUpper = dc.GetTextExtent( vFormula[i].sUpper );

			// (temporär) Einrücken, es könnte nämlich sein, dass
			// der tiefgestellte String länger ist!
			iTempIndentX = sizeUpper.GetWidth();

			// Zeichnen
			dc.DrawText( vFormula[i].sUpper, x + iIndentX, y - (sizeSymbol.GetHeight() / 3) );
		}

		// Tiefstellen
		if ( vFormula[i].sLower.length() > 0 )
		{
			// Größe bestimmen
			wxSize sizeLower = dc.GetTextExtent( vFormula[i].sLower );

			// Sollte dieser String länger sein als der "hochgestellte", dann
			// ersetzen wir den entsprechenden Wert in iTempIndentX, ansonsten nicht.
			if ( sizeLower.GetWidth() > iTempIndentX )
				iTempIndentX = sizeLower.GetWidth();

			// Zeichnen
			dc.DrawText( vFormula[i].sLower, x + iIndentX, y + (sizeSymbol.GetHeight() / 3) );
		}

		// Zum Schluss noch entsprechend einrücken
		iIndentX += iTempIndentX;
	}

	// Schrift wiederherstellen
	dc.SetFont( fontTemp );
}

void CBerylliumSubstanceData::DrawName( wxDC &dc, int x, int y, int width, int height ) const
{
	// Die Schrift holen
	wxFont fontTemp = dc.GetFont();

	// Kopie der Schrift machen
	wxFont fontCopy = fontTemp;

	// zu Zeichnender Name
	wxString name		= GetName();
	wxString shortname	= GetShortestName();

	// Falls die Substanz als Lösung angesetzt wird, Konzentration an Namen anhängen
	if ( (typeBatch == batchSolution) && (dbConcentration > 0.0) )
	{
		double dbconc = dbConcentration;

		wxString conc;

		if ( bRelativeConcentration )
		{
			conc = wxString::Format( " (%.0f%%)", dbconc * 100.0 );
		}

		else
		{
			wxString unit = BeTool::UnitPrefix( dbconc );
			conc = wxString::Format( " (%.1f %sM)", dbconc, unit.c_str());
		}

		name += conc;
		shortname += conc;
	}

	// Dimension des Textes
	wxSize sizeText = dc.GetTextExtent( name );

	// Falls der Text nicht reinpasst, dann wird hier versucht,
	// a) den kleinsten Namen zu benutzen oder
	// b) die Schriftgröße um 2 zu verkleinern
	while ( (sizeText.GetWidth() > width) || (sizeText.GetHeight() > height) )
	{
		// Kleinsten Namen nutzen
		sizeText = dc.GetTextExtent( shortname );

		// Passt der auch nicht?
		if ((sizeText.GetWidth() > width) || (sizeText.GetHeight() > height))
		{
			// Schrift um zwei verkleinern
			fontCopy.SetPointSize( fontCopy.GetPointSize() - 2 );

			// Schrift setzen
			dc.SetFont( fontCopy );

			// Erneut berechnen
			sizeText = dc.GetTextExtent( name );
		}

		// Passt der kürzeste Name?
		else
		{
			name = shortname;
			break;
		}
	};

	// Den Namen in "name" zeichnen
	dc.DrawText( name, x, y );

	// Schrift wiederherstellen
	dc.SetFont( fontTemp );
}

// Ersetzt die Namen durch "names"
void CBerylliumSubstanceData::SetNames( const wxString names, const wxString separator )
{
	// Erstellt eine Liste aus names und separator
	BeTool::StringToList( names, separator, szNames, true );
}

// Wandelt den String density in eine Fließkommazahl um und speichert ihn als Dichte
void CBerylliumSubstanceData::SetDensity( const wxString density )
{
	// Erstellt eine Liste aus names und separator
	dbDensity = BeTool::StringToDouble( density );
}

// Wandelt den String molweight in eine Fließkommazahl um und speichert ihn als Molgewicht
void CBerylliumSubstanceData::SetMolWeight( const wxString molweight )
{
	// Erstellt eine Liste aus names und separator
	dbMolWeight = BeTool::StringToDouble( molweight );
}

// Wandelt den String mak in eine Fließkommazahl um und speichert ihn als MAK-Wert
void CBerylliumSubstanceData::SetMAK( const wxString mak )
{
	// Erstellt eine Liste aus names und separator
	dbMAK = BeTool::StringToDouble( mak );
}

// Gibt die verwendeten Gefahrensymbole als einen String zurück
wxString CBerylliumSubstanceData::GetHarzards( wxString separator ) const
{
	// BeTool benutzen
	return BeTool::ListToString( szHarzards, separator );
}

// Setzt die Gefahrensymbole
void CBerylliumSubstanceData::SetHarzards( wxString harzards, wxString separator )
{
	BeTool::StringToList( harzards, separator, szHarzards, true );
}

// Prüft auf ein bestimmtes Gefahrensymbol
bool CBerylliumSubstanceData::HasHarzard( wxString harzard ) const
{
	// Alle Symbole durchgehen und vergleichen
	for ( unsigned int i = 0; i < szHarzards.size(); i++ )
		if ( szHarzards[i].compare(harzard) == 0 )
			return true;

	// Nichts gefunden
	return false;
}

// Gibt die verwendeten GHS-Symbole als einen String zurück
wxString CBerylliumSubstanceData::GetGHSSymbols( wxString separator ) const
{
	// BeTool benutzen
	return BeTool::ListToString( szGHSSymbols, separator );
}

// Setzt die GHS-Symbole
// 06.12.2013: Prüft auf doppelte Symbole
void CBerylliumSubstanceData::SetGHSSymbols( wxString symbols, wxString separator )
{
    // Temporäre Liste erstellen
    std::vector< wxString > tempGHS;

    // Alte Liste löschen
    szGHSSymbols.clear();

    // Liste füllen
	BeTool::StringToList( symbols, separator, tempGHS, true );

	// Liste durchgehen und Stück für Stück hinzufügen (falls nicht schon vorhanden)
	for ( unsigned int i = 0; i < tempGHS.size(); ++i )
	{
	    // Hinzufügen
	    if ( !HasGHSSymbol( tempGHS[i] ) )
            szGHSSymbols.push_back(tempGHS[i]);
	}

}

// Prüft auf ein bestimmtes GHS-Symbol
bool CBerylliumSubstanceData::HasGHSSymbol( wxString symbol ) const
{
	// Alle Symbole durchgehen und vergleichen
	for ( unsigned int i = 0; i < szGHSSymbols.size(); i++ )
		if ( szGHSSymbols[i].compare(symbol) == 0 )
			return true;

	// Nichts gefunden
	return false;
}

// Berechnet die Stoffmenge und gibt sie in "mol" zurück
double CBerylliumSubstanceData::CalculateAmountOfSubstance() const
{
	// Stoffmenge
	double amount = 0.0;

	// Ohne Molmasse (außer bei Typ batchSolution) können wir nichts berechnen
	if ( (dbMolWeight == 0.0) && (typeBatch != batchSolution) )
		return 0.0;

	// Welcher Ansatztyp?
	switch ( typeBatch )
	{
		case batchSolid:
			// Stoffmenge (mol) = Masse (g) / Molgewicht (g/mol)
			amount = (dbBatch / dbMolWeight);
			break;

		case batchLiquid:
			// Stoffmenge (mol) = ( Dichte (g/mL) * Volumen (mL) ) / Molgewicht (g/mol)
			amount = ( (dbDensity * dbBatch * 1000) / dbMolWeight );
			break;

		case batchSolution:
			// Relative (%, v/v) oder absolute Konzentration angegeben?
			if ( bRelativeConcentration && (dbMolWeight > 0.0) )
				// Stoffmenge (mol) = ( relative Konzentration(%) * (Dichte (g/mL) / Molgewicht (g/mol)) ) * Volumen (mL)
				amount = ( (dbConcentration * dbDensity) / dbMolWeight ) * dbBatch * 1000;
			else
				// Stoffmenge (mol) = Konzentration (mol/L) * Volumen (L)
				amount = dbConcentration * dbBatch;
			break;

		default:
			break;
	}

	// Zurückgeben
	return amount;
}

// Get: Konzentration der Lösung (falls vorhanden)
double CBerylliumSubstanceData::GetBatchConcentration()
{
	// Ist die Konzentration noch gar nicht gesetzt? Dann berechnen wir die
	// aus Dichte und Molgewicht (falls gesetzt)
	if ( (dbConcentration == 0.0) && (dbMolWeight > 0.0) )
	{
		// Konzentration (mol/L) = Dichte (g/mL) / Molgewicht (g/mol) * 1000 (mL/L)
		dbConcentration = (dbDensity / dbMolWeight) * 1000;
	}

	// Konzentration zurückgeben
	return dbConcentration;
}

// Speichert alle Namen unter den angegebenen Knoten
void CBerylliumSubstanceData::SaveNamesToXmlNode( wxXmlNode *parent )
{
	// Letzter Knoten
	wxXmlNode *lastnode = NULL;

	// Alle Namen durchgehen
	for ( unsigned int i = 0; i < szNames.size(); ++i )
	{
		// Knoten anlegen
		wxXmlNode *xmlName = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "name" );

		// Text hinzufügen
		new wxXmlNode( xmlName, wxXML_TEXT_NODE, "name", szNames[i] );

		// Einfügen
		parent->InsertChildAfter( xmlName, lastnode );

		// Letzter Knoten
		lastnode = xmlName;
	}
}

// Speichert alle Daten in einen Xml-Knoten
void CBerylliumSubstanceData::SaveToXmlNode( wxXmlNode *parent )
{
	// Zunächste mal die Namen speichern
	SaveNamesToXmlNode( parent );

	// Gruppe speichern (wird nur für Bibliotheken benötigt)
	if ( szGroup.length() > 0 )
		AppendToXmlNode( parent, "group", szGroup );

	// CAS-Nummer
	if ( szCAS.length() > 0 )
		AppendToXmlNode( parent, "CAS", szCAS );

	// Molekulargewicht
	if ( dbMolWeight > 0.0 )
		AppendToXmlNode( parent, "molecular-weight", wxString::Format("%.5lf", dbMolWeight) );

	// Dichte
	if ( dbDensity > 0.0 )
		AppendToXmlNode( parent, "density", wxString::Format("%.5lf", dbDensity) );

	// Chemische Formel
	if ( szFormula.length() > 0 )
		AppendToXmlNode( parent, "chemical-formula", szFormula );

	// Schmelzpunkt
	if ( szMeltingPoint.length() > 0 )
	{
		// Schmelzpunkt anfügen
		wxXmlNode *xmlMeltingPoint = AppendToXmlNode( parent, "melting-point", szMeltingPoint );

		// Zersetzung am Schmelzpunkt? (Beryllium10)
		if ( bDecomposition )
			xmlMeltingPoint->AddAttribute( "decomposition", "1" );
	}

	// Siedepunkt
	if ( (szBoilingPoint.length() > 0) && !bDecomposition )
		AppendToXmlNode( parent, "boiling-point", szBoilingPoint );

	// Flammpunkt (Beryllium10)
	if ( szFlashPoint.length() > 0 )
		AppendToXmlNode( parent, "flashpoint", szFlashPoint );

	// Gefahrensymbole
	for ( unsigned int i = 0; i < szHarzards.size(); ++i )
		AppendToXmlNode( parent, "harzard", szHarzards[i] );

	// GHS-Symbole (Beryllium10)
	for ( unsigned int i = 0; i < szGHSSymbols.size(); ++i )
		AppendToXmlNode( parent, "GHS-symbol", szGHSSymbols[i] );

	// R-Sätze
	if ( szRisks.length() > 0 )
		AppendToXmlNode( parent, "risk", szRisks );

	// S-Sätze
	if ( szSafeties.length() > 0 )
		AppendToXmlNode( parent, "safety", szSafeties );

	// H-Sätze (Beryllium10)
	if ( szHarzardSatements.length() > 0 )
		AppendToXmlNode( parent, "harzard-statements", szHarzardSatements );

	// EuH-Sätze (Beryllium10)
	if ( szEuropeanHarzardSatements.length() > 0 )
		AppendToXmlNode( parent, "eu-harzard-statements", szEuropeanHarzardSatements );

	// P-Sätze (Beryllium10)
	if ( szPrecautionaryStatements.length() > 0 )
		AppendToXmlNode( parent, "precautionary-statements", szPrecautionaryStatements );

	// GHS-Signalwort (Beryllium10)
	if ( szSignalWord.length() > 0 )
		AppendToXmlNode( parent, "GHS-signalword", szSignalWord );

	// MAK-Wert
	if ( dbMAK > 0.0 )
	{
		// Knoten anlegen
		wxXmlNode *xmlMAK = AppendToXmlNode( parent, "MAK", wxString::Format("%.5lf", dbMAK) );

		// in ppm?
		if ( bMAKppm )
			xmlMAK->AddAttribute( "ppm", "1" );
	}

	// WGK
	AppendToXmlNode( parent, "WGK", wxString::Format("%d", iWGK) );

	// LD50-Werte (falls vorhanden)
	if ( ld50data.size() > 0 )
		SaveLD50ToXmlnode(parent);

	// Substanzabhängige Templates einfügen
	const char *szCat[] = { "danger", "security", "behavior", "dumping" };

	// Eigene Statements einfügen
	for ( unsigned int cat = 0; cat < 4; ++cat )
	{
		// Templates durchgehen
		for ( unsigned int i = 0; i < sCustomStatements[cat].size(); ++i )
		{
			// Knoten anlegen
			wxXmlNode *xmlStatement = AppendToXmlNode( parent, "template", sCustomStatements[cat][i] );

			// Attribut (Kategorie) setzen
			xmlStatement->AddAttribute( "category", szCat[cat] );
		}
	}

	// Quelle speichern, falls vorhanden
	if ( source.provider.length() > 0 )
	{
		// Hersteller
		AppendToXmlNode( parent, "source.provider", source.provider );

		// Geladen am
		AppendToXmlNode( parent, "source.fetched", source.fetched );

		// Falls URL vorhanden
		if ( source.url.length() > 0 )
			AppendToXmlNode( parent, "source.url", source.url );

		// Falls Änderungen vorhanden
		if ( source.changed.length() > 0 )
			AppendToXmlNode( parent, "source.changed", source.changed );
	}
}

// Speichert die Ansatzdaten in einem Xml-Knoten
void CBerylliumSubstanceData::SaveBatchToXmlNode( wxXmlNode *parent )
{
	// Ansatztyp
	switch( GetBatchType() )
	{
		// Feststoff
		case batchSolid:
			{
				// Knoten anlegen
				wxXmlNode *batch = AppendToXmlNode( parent, "setting-up", wxString::Format("%lf", GetBatchAmount() ) );

				// Typ übergeben
				batch->AddAttribute( "mass", "1" );
			};
			break;

		// Flüssigkeit
		case batchLiquid:
			{
				// Knoten anlegen
				wxXmlNode *batch = AppendToXmlNode( parent, "setting-up", wxString::Format("%lf", GetBatchAmount() ) );

				// Typ übergeben
				batch->AddAttribute( "volumina", "1" );
			};
			break;

		// Lösung
		case batchSolution:
			{
				// Knoten für Konzentration anlegen
				wxXmlNode *xmlConcentration = AppendToXmlNode( parent, "concentration", wxString::Format("%lf", GetBatchConcentration() ) );

				// Relative Konzentration?
				if ( bRelativeConcentration )
					xmlConcentration->AddAttribute( "relative", "1" );

				// Knoten für Lösungsvolumen anlegen
				AppendToXmlNode( parent, "solution-volumina", wxString::Format("%lf", GetBatchAmount() ) );
			};
			break;

		case batchLAST:
			break;

	}
}

// Lädt alle Daten aus einem Xml-Knoten (inkl. Ansatz!)
void CBerylliumSubstanceData::LoadFromXmlNode( wxXmlNode *parent )
{
	// Kategorien für die Templates festlegen
	const char *categories[] = { "danger", "security", "behavior", "dumping" };

	// Array für Namen leeren
	szNames.clear();

	// Temporäres Array für LD50-Werte anlegen
	std::vector<LethalDose50> templd50list;

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();
		wxString content = node->GetNodeContent();

		// Name
		if ( name == "name" )
			szNames.push_back( content );

		// Molekulargewicht
		else if ( name == "molecular-weight" )
			wxString(content).ToDouble( &dbMolWeight );

		// Formel
		else if ( name == "chemical-formula" )
			SetFormula( content );

		// Dichte
		else if ( name == "density" )
			wxString(content).ToDouble( &dbDensity );

		// Schmelzpunkt
		else if ( name == "melting-point" )
		{
			// Speichern
			szMeltingPoint = content;

			// Temperatur überprüfen
			CheckAndUpdateTemperature( szMeltingPoint );

			// Zersetzt sich?
			bDecomposition = (node->GetAttribute( "decomposition", "0" ) == "1");
		}

		// Siedepunkt
		else if ( name == "boiling-point" )
		{
			// Speichern
			szBoilingPoint = content;

			// Temperatur überprüfen
			CheckAndUpdateTemperature( szBoilingPoint );
		}

		// Flammpunkt (Beryllium10)
		else if ( name == "flashpoint" )
			szFlashPoint = content;

		// Gefahrensymbole
		else if ( name == "harzard" )
			szHarzards.push_back( content );

		// GHS-Symbole (Beryllium10)
		else if ( name == "GHS-symbol" )
			szGHSSymbols.push_back( content );

		// GHS-Signalwort (Beryllium10)
		else if ( name == "GHS-signalword" )
			szSignalWord = content;

		// R-Sätze
		else if ( name == "risk" )
			szRisks = content;

		// S-Sätze
		else if ( name == "safety" )
			szSafeties = content;

		// H-Sätze (Beryllium10)
		else if ( name == "harzard-statements" )
			szHarzardSatements = content;

		// EuH-Sätze (Beryllium10)
		else if ( name == "eu-harzard-statements" )
			szEuropeanHarzardSatements = content;

		// P-Sätze (Beryllium10)
		else if ( name == "precautionary-statements" )
			szPrecautionaryStatements = content;

		// MAK-Wert
		else if ( name == "MAK" )
		{
			wxString(content).ToDouble( &dbMAK );

			// in ppm?
			bMAKppm = (node->GetAttribute( "ppm", "0" ) == "1");
		}

		// Wassergefährdungsklasse
		else if ( name == "WGK" )
			wxString(content).ToULong( reinterpret_cast<unsigned long*>(&iWGK) );

		// Ld50-Werte
		else if ( name == "lethaldose50" )
		{
			// Temporäres Objekt anlegen
			LethalDose50 dose;

			// Wert einlesen
			wxString(content).ToDouble( &dose.dbValue );

			// Attribute einlesen
			dose.sExposureType	= node->GetAttribute("exposuretype", "");
			dose.sSpecies		= node->GetAttribute("species", "");
			dose.sUnit			= node->GetAttribute("unit", "");

			// Einfügen
			templd50list.push_back(dose);
		}

		// CAS-Nummer
		else if ( name == "CAS" )
			szCAS = content;

		// Gruppe (für Bibliothek wichtig)
		else if ( name == "group" )
			szGroup = content;

		// Ansatz laden (reine Substanz)
		else if ( name == "setting-up" )
		{
			wxString(content).ToDouble(&dbBatch);

			// Typ setzen
			typeBatch = batchSolid;

			// Flüssigkeit?
			if ( node->GetAttribute( "volumina", "0" ) == "1" )
				typeBatch = batchLiquid;
		}

		// Ansatz laden (Lösung, Konzentration)
		else if ( name == "concentration" )
		{
			wxString(content).ToDouble(&dbConcentration);

			// Relative Konzentration?
			bRelativeConcentration = (node->GetAttribute( "relative", "0" ) == "1" );
		}

		// Ansatz laden (Lösung, Volumen)
		else if ( name == "solution-volumina" )
		{
			wxString(content).ToDouble(&dbBatch);

			// Typ setzen
			typeBatch = batchSolution;
		}

		// substanzabhängige Templates laden
		else if ( name == "template" )
		{
			// Kategorie holen
			wxString category = node->GetAttribute( "category", "danger" );

			// Index der Kategorie holen
			unsigned int iCat = 0;

			for ( unsigned int i = 0; i < 4; ++i )
			{
				if ( category.compare( categories[i] ) == 0 )
				{
					iCat = i;
					break;
				}
			}

			// Template hinzufügen
			sCustomStatements[iCat].push_back( content );
		}

		// Quelle: Hersteller
		else if ( name == "source.provider" )
			source.provider = content;

		// Quelle: Heruntergeladen am
		else if ( name == "source.fetched" )
			source.fetched = content;

		// Quelle: URL
		else if ( name == "source.url" )
			source.url = content;

		// Quelle: Geändert am
		else if ( name == "source.changed" )
			source.changed = content;

		// Nächster Ast
		node = node->GetNext();
	};

	// LD50-Werte sortiert einfügen
	SetLD50Data( templd50list );
}

// Hilfsfunktion (inline): fügt node an parent an mit dem Textinhalt content
inline wxXmlNode* CBerylliumSubstanceData::AppendToXmlNode( wxXmlNode *parent, wxString node, wxString content )
{
	// Knoten anlegen
	wxXmlNode *xmlnode = new wxXmlNode( parent, wxXML_ELEMENT_NODE, node );

	// Text hinzufügen
	new wxXmlNode( xmlnode, wxXML_TEXT_NODE, node, content );

	// Node zurückgeben
	return xmlnode;
}

// Hilfsfunktion: Überprüft, ob eine Temperaturangabe eine Einheit (K oder °C) hat.
// Wenn nicht, ist es höchstwahrscheinlich eine Kelvinangabe aus der alten Berylliumversion.
// Dies wird auch hier gleich mitgeändert.
void CBerylliumSubstanceData::CheckAndUpdateTemperature( wxString &data )
{
	// Suche nach Einheitensymbol
	size_t iCelsius = data.find( "C" );
	size_t iKelvin  = data.find( "K" );

	// Eins von beiden gefunden?
	if ( (iCelsius != data.npos) || (iKelvin != data.npos) )
		return;

	// Umwandlung in einen double-Wert
	double temperature = BeTool::StringToDouble( data );

	// Abziehen von 273.15 (Kelvin -> Grad)
	temperature -= 273.15;

	// Umwandlung in einen String und Stellen abschneiden
	wxString buffer; buffer = wxString::Format( "%.2lf", temperature );

	// Evtl. Kommas durch Punkte ersetzen
	buffer.Replace( ",", ".", true );

	// Hinzufügen von "°C"
	buffer += " °C";

	// Kopieren
	data = buffer.ToStdString();
}

// Berechnet die Gefahr als "Beryllium"-Punkte
void CBerylliumSubstanceData::CalculateDangerPoints()
{
	// Auf Null setzen
	m_iDanger = 0;

	// Berechnen und entsprechend gewichten
	m_iDanger += 1 * CalculateDangerPointsStatements();
	m_iDanger += 1 * CalculateDangerPointsGHSSymbols();
	m_iDanger += 1 * CalculateDangerPointsToxicology();
}

// Hilfsfunktion: "Beryllium-Punkte" für H/P-Sätze
unsigned int CBerylliumSubstanceData::CalculateDangerPointsStatements()
{
	// Punkte
	unsigned int iDanger = 0;

	// Listen für H/P-Sätze anlegen
	std::vector< wxString > listHarzards, listPrecautionaries;

	// und Listen füllen
	BeTool::StringToList( szHarzardSatements, "-", listHarzards );
	BeTool::StringToList( szPrecautionaryStatements, "-", listPrecautionaries );

	// Bibliothek holen
	const CBerylliumStatementsManager &library = ::wxGetApp().GetStatementLibrary();

	// 1. H- und P-Sätze prüfen
	// ------------------------
	// Prinzipien:
	// - Große Anzahl H/P-Sätze erhöhen Gefährlichkeit
	// - Jeder Satz erhöht die Gefährlichkeit um 0-1000 Punkte

	// Anzahl der H-Sätze * 2
	iDanger += listHarzards.size() * 2;

	// Anzahl der P-Sätze * 1
	iDanger += listPrecautionaries.size() * 1;

	// Liste durchgehen und Punkte entsprechend auf addieren, H-Sätze
	for ( unsigned int i = 0; i < listHarzards.size(); ++i )
	{
		// Statement holen
		CBerylliumStatement temp = library.GetStatementByToken( "H"+listHarzards[i] );

		// Aufaddieren
		iDanger += temp.GetDangerPoints();
	}

	// Liste durchgehen und Punkte entsprechend auf addieren, P-Sätze
	for ( unsigned int i = 0; i < listPrecautionaries.size(); ++i )
	{
		// Statement holen
		CBerylliumStatement temp = library.GetStatementByToken( "P"+listPrecautionaries[i] );

		// Aufaddieren
		iDanger += temp.GetDangerPoints();
	}

	// Zurückgeben
	return iDanger;
}

// Hilfsfunktion: "Beryllium-Punkte" für GHS-Symbole/Signalwort
unsigned int CBerylliumSubstanceData::CalculateDangerPointsGHSSymbols()
{
	// 2. GHS-Symbole / Signalwort
	// ---------------------------
	// Prinzipien:
	// - jedes GHS-Symbol erhöht die Gefährlichkeit um 0-1000 Punkte
	// - das Signalwort "Gefahr"/"Danger" verdoppelt diese Punkte

	// Punkte für GHS-Symbole
	unsigned int iGHSDanger = 0;

	// Flammpunkt überprüfen
	double dbFP = BeTool::StringToDouble( szFlashPoint );

	// Celsius? Dann noch 273,15 draufaddieren :)
	if ( szFlashPoint.find("°C") != wxString::npos )
		dbFP += 273.15;

	// Jedes GHS-Symbol durchgehen
	for ( unsigned int i = 0; i < szGHSSymbols.size(); ++i )
	{
		// Explosionsgefahr
		if ( szGHSSymbols[i].compare("01") == 0 )
			iGHSDanger += 300;

		// Flamme (entzündlich)
		else if ( szGHSSymbols[i].compare("02") == 0 )
			iGHSDanger += 100 * ( (dbFP < 100.0) ? 2 : 1 );

		// Flamme mit Kreis (entzündlich/oxidierend/brandfördernd)
		else if ( szGHSSymbols[i].compare("03") == 0 )
			iGHSDanger += 200 * ( (dbFP < 100.0) ? 2 : 1 );

		// Gasflasche (Gase unter Druck)
		else if ( szGHSSymbols[i].compare("04") == 0 )
			iGHSDanger += 50;

		// Ätzend (Säuren/Basen)
		else if ( szGHSSymbols[i].compare("05") == 0 )
			iGHSDanger += 100;

		// Totenkopf (Giftig)
		else if ( szGHSSymbols[i].compare("06") == 0 )
			iGHSDanger += 500;

		// Ausrufezeichen (!)
		else if ( szGHSSymbols[i].compare("07") == 0 )
			iGHSDanger += 50;

		// Gesundheitsgefahr
		else if ( szGHSSymbols[i].compare("08") == 0 )
			iGHSDanger += 100;

		// Umweltschädigend (multipliziert mit Wassergefährdungsklasse + 1)
		else if ( szGHSSymbols[i].compare("09") == 0 )
			iGHSDanger += 100 * (iWGK + 1);
	}

	// Signalwort = Gefahr/Danger
	if ( (szSignalWord.compare("Gefahr") == 0) || (szSignalWord.compare("Danger") == 0) )
		iGHSDanger = iGHSDanger * 2;

	// Zurückgeben
	return iGHSDanger;
}

// Hilfsfunktion: "Beryllium-Punkte" für Toxikologie
unsigned int CBerylliumSubstanceData::CalculateDangerPointsToxicology()
{
	// Punkte
	unsigned int iDanger = 0;

	// 3. Toxikologie
	// --------------
	// Prinzipien:
	// - Prüfe kleinsten, toxikologischen Wert (MAK/LD50)
	// - Prüfe Ansatz und vergleiche mit kleinstem, toxikologischen Wert
	// - Durchschnittsmensch (ca. 70kg)
	// - Wassergefährdungsklasse als Faktor verwenden, falls größer 0 (WGK 3 bedeutet im Prinzip "tödlich für alle Wasserorganismen")

	// Stoffmenge des Ansatzes
	double amount = CalculateAmountOfSubstance();

	// Kleinste toxikologischste Stoffmenge berechnen
	double toxiamount = GetSmallestToxicologicAmountDouble();

	// Falls angegeben
	if ( (amount > 0.0f) && (toxiamount > 0.0f) )
	{
		// Prozentualer Anteil von amount an toxiamount * 100
		double ratio = (double)100.0f * amount / toxiamount;

		// Punkte errechnen sich durch eine hübsche Sigmoidfunktion
		// Der Gedanke ist, dass bis zum Erreichen der niedrigsten Toxikologischen Konzentration die Kurve erst langsam ansteigt
		// aber dann möglichst schnell auf dem Maximum (1000 Punkte) ist! Der Wendepunkt der Sigmoidfunktion ist bei 75, d.h.
		// dass bei amount = toxiamount die Funktion schon ihr Maximum erreicht.
		iDanger = (int)(1000.0f / (1.0f + exp(-0.1f * (ratio-75.0f)) ));

		// Multipliziert wird das Ganze noch mit einer zweiten Sigmoidfunktion
		// => Die Funktion hat dadurch VOR Erreichen des Wendepunkts (75%) einen sehr flachen, geruhsamen Anstieg, während
		// er danach um so schneller hochsteigt! Allerdings kann dieser zusätzliche Faktor höchstens 10 betragen, damit nicht
		// irgendwann der Wert in die Unendlichkeit klettert. Diese Funktion hat ihren Wendepunkt bei amount = 5 * toxiamount.
		iDanger = (int)((double)iDanger * 10.0f / (1.0f + exp(-0.009f * (ratio-500.0f))));
	}

	// Zurückgeben
	return iDanger;
}

// LD50-Werte sortiert einfügen
void CBerylliumSubstanceData::SetLD50Data( const std::vector< LethalDose50 > &data )
{
	// Alte Daten löschen
	ld50data.clear();

	// Daten stück für Stück sortiert einfügen
	for ( unsigned int i = 0; i < data.size(); ++i )
	{
		// Einfügen an diese Position
		std::vector< LethalDose50 >::iterator index = ld50data.begin();

		// Schätze Stoffmenge für die neuen Daten
		double dbAmountNewData = CalculateAmountOfSubstanceForLethalDose( data[i] );

		// Alle schon vorhandenen Daten durchgehen
		for (std::vector< LethalDose50 >::iterator j = ld50data.begin(); j < ld50data.end(); j++ )
		{
			// Daten berechnen für die schon eingefügten Daten
			double dbAmountOldData = CalculateAmountOfSubstanceForLethalDose( *j );

			// Daten vergleichen: Sind die neuen Daten kleiner als die Vergleichsdaten, dann davor einfügen!
			if ( dbAmountNewData <= dbAmountOldData )
				break;

			// Ansonsten: Index erhöhen
			index++;
		}

		// Einfügen
		ld50data.insert(index, data[i]);
	}
}

	// LD50-Werte holen
void CBerylliumSubstanceData::GetLD50Data( std::vector< LethalDose50 > &data ) const
{
	// Vector leeren
	data.clear();

	// Kopieren
	for ( unsigned int i = 0; i < ld50data.size(); ++i )
		data.push_back( ld50data[i] );
}

// Hilfsfunktion: Schätzt die Stoffmenge von toxikologischen Daten und gibt sie zurück (in mol)
double CBerylliumSubstanceData::CalculateAmountOfSubstanceForLethalDose( const LethalDose50 &data ) const
{
	// Molare Masse nicht gesetzt?
	if ( dbMolWeight <= 0.0f )
		return 0.0f;

	// Einheit überprüfen: mg/kg?
	if ( data.sUnit.compare("mg/kg") == 0 )
	{
		// Gesamtmasse (in g) = Masenkonzentration (LD50-Wert, mg/kg) * Masse des Menschen (Durchschnittsmensch, in kg) / 1000
		double mass = data.dbValue * AverageHumanMass / 1000.0f;

		// Stoffmenge (Masse in g durch molare Masse in g/mol) berechnen und zurückgeben
		return (mass / dbMolWeight);
	}

	// Einheit: mL/m³
	else if ( data.sUnit.compare("mL/m³") == 0 )
	{
		// Dichte nicht gesetzt?
		if ( dbDensity <= 0.0f )
			return 0.0f;

		// Gesamtvolumen (in mL) = Volumenkonzentration (LD50-Wert, mL/m³) * Arbeitsplatzgröße (in m³)
		double volume = data.dbValue * AverageWorkplaceVolume;

		// Gesamtmasse (in g) = Volumen (in mL) * Dichte (in g/mL)
		double mass = volume * dbDensity;

		// Stoffmenge (Masse in g durch molare Masse in g/mol) berechnen und zurückgeben
		return (mass / dbMolWeight);
	}

	// Ansonsten nichts zurückgeben
	return 0.0f;
}

// Speichert die LD50-Werte in einem Xml-Knoten
void CBerylliumSubstanceData::SaveLD50ToXmlnode( wxXmlNode *parent )
{
	// Alle Namen durchgehen
	for ( unsigned int i = 0; i < ld50data.size(); ++i )
	{
		// Knoten anlegen
		wxXmlNode *xmlLD50 = AppendToXmlNode(parent, "lethaldose50", wxString::Format("%.6f",ld50data[i].dbValue) );

		// Attribute
		xmlLD50->AddAttribute("exposuretype", ld50data[i].sExposureType );
		xmlLD50->AddAttribute("species", ld50data[i].sSpecies );
		xmlLD50->AddAttribute("unit", ld50data[i].sUnit);
	}
}

// den kleinsten Toxi-Wert suchen (MAK/LD50) und formatiert zurückgeben
wxString CBerylliumSubstanceData::GetSmallestToxicologicAmount() const
{
	// Temporärer Wert
	wxString szTemp;

	// Stoffmenge des MAK-Wertes
	double dbAmountMAK = 0.0f;

	// MAK-Wert (falls vorhanden)
	if ( dbMAK > 0.00f )
	{
		// MAK-Wert festhalten
		szTemp = wxString::Format("%02.2f" , dbMAK);

		// ',' durch '.' ersetzen
		szTemp.Replace( ",", ".", true );

		// Einheit und berechnen
		if ( bMAKppm )
		{
			szTemp += " ppm"; //" mL/m³";

			// Molekulargewicht wird benötigt
			if ( dbMolWeight > 0.0f )
				dbAmountMAK = (dbMAK * AverageWorkplaceVolume * dbDensity) / dbMolWeight;
		}
		else
		{
			szTemp += " mg/m³";

			// Molekulargewicht wird benötigt
			if ( dbMolWeight > 0.0f )
				dbAmountMAK = (dbMAK * AverageWorkplaceVolume / 1000.0f) / dbMolWeight;
		}
	}

	// Existiert kein LD50-Wert? Dann MAK-Wert zurückgeben
	if ( ld50data.size() == 0 )
		return szTemp;

	// Stoffmenge des geringsten LD50-Wertes berechnen
	double dbAmountLD50 = CalculateAmountOfSubstanceForLethalDose( ld50data[0] );

	// Ld50-Wert kleiner als MAK-Wert (falls dieser existiert)? Dann diesen zurückgeben
	if ( (dbAmountLD50 < dbAmountMAK) || !(dbMAK > 0.0f) )
		szTemp = wxString::Format("%02.2f %s", ld50data[0].dbValue, ld50data[0].sUnit );

	// Zurückgeben
	return szTemp;
}

// den kleinsten Toxi-Wert suchen (MAK/LD50) und die entsprechende Stoffmenge zurückgeben
double CBerylliumSubstanceData::GetSmallestToxicologicAmountDouble() const
{
	// Stoffmenge des niedrigsten Wertes
	double dbAmountLOW = 0.0f;

	// MAK-Wert (falls vorhanden), Molekulargewicht wird für die Berechnung benötigt!
	if ( (dbMAK > 0.00f) && (dbMolWeight > 0.0f) )
	{
		// Berechnen bzw. schätzen
		if ( bMAKppm )
				dbAmountLOW = (dbMAK * AverageWorkplaceVolume * dbDensity) / dbMolWeight;

		else
				dbAmountLOW = (dbMAK * AverageWorkplaceVolume / 1000.0f) / dbMolWeight;
	}

	// Existiert kein LD50-Wert? Dann MAK-Wert zurückgeben
	if ( ld50data.size() == 0 )
		return dbAmountLOW;

	// Stoffmenge des geringsten LD50-Wertes berechnen
	double dbAmountLD50 = CalculateAmountOfSubstanceForLethalDose( ld50data[0] );

	// Ld50-Wert kleiner als MAK-Wert (falls er existiert)? Dann diesen zurückgeben
	if ( (dbAmountLD50 < dbAmountLOW) || !(dbMAK > 0.0f) )
		dbAmountLOW = dbAmountLD50;

	// Zurückgeben
	return dbAmountLOW;
}
