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

#include "../beryllium.h"
#include "berylliumproviderwiki.h"

#include "../betool.h"

#include <wx/sstream.h>
#include <wx/uri.h>

// Initialisieren der Elementsymbole
const char *CBerylliumProviderWiki::cElements[112] =
	{
		 "H", "He", "Li", "Be",  "B",  "C",  "N",  "O",  "F", "Ne", "Na", "Mg", "Al", "Si", // 1 - 14
		 "P",  "S", "Cl", "Ar",  "K", "Ca", "Sc", "Ti",  "V", "Cr", "Mn", "Fe", "Co", "Ni", // 15 - 28
		"Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr",  "Y", "Zr", "Nb", "Mo",
		"Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te",  "I", "Xe", "Cs", "Ba",
		"La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
		"Lu", "Hf", "Ta",  "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Ti", "Pb", "Bi", "Po",
		"At", "Rn", "Fr", "Ra", "Ac", "Th", "Pa",  "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf",
		"Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn"
	};

CBerylliumProviderWiki::CBerylliumProviderWiki(void)
{
}

CBerylliumProviderWiki::~CBerylliumProviderWiki(void)
{
}

// 1. Suchanfrage stellen
bool CBerylliumProviderWiki::SearchForCompound( const wxString searchtext, bool bExactMatch = false )
{
	// Suchstring leer? Raus hier!
	if ( searchtext.length() == 0 )
		return false;

	// Suchstring speichern
	searchname = searchtext;

	// Erster Buchstabe -> Großschreibung
	if ( (searchname[0].GetValue() > 96) && (searchname[0].GetValue() < 122) )
		searchname[0] = searchname[0].GetValue() - 32;

	// Datenblock löschen
	CBerylliumSubstanceData newdata; tempdata = newdata;

	// Seiteninhalt löschen
	content = "";

	// Daten erfolgreich gefunden (und geladen)
	bool bGotData = false;

	// HTTP-Anfrage vorbereiten
	m_http.SetHeader( "Content-type", "text/html; charset=iso-8859-15"); //utf-8" );
	m_http.SetTimeout(60); // 60 Sekunden TimeOut
	wxLogMessage( _(L"ProviderWiki, %d: HTTP-Anfrage wird vorbereitet."), __LINE__ );

	// Anfragen
	wxString request = "/wiki/Spezial:Exportieren/%s";
	wxString wikihost = "de.wikipedia.org";

	// Anfrage an Wikipedia vorbereiten (je nach Sprache)
	if ( iLanguage == wxLANGUAGE_ENGLISH )
	{
		// Verbinden und Request ändern
		wikihost = "en.wikipedia.org";
		request  = "/wiki/Special:Export/%s";
	}

	// URL escapen
	wxURI uritext(searchtext);
	wxString newsearchtext = uritext.BuildURI();

	// Loggen
	wxLogMessage( _(L"ProviderWiki, %d: HTTP-Anfrage an 'http://%s%s'."), __LINE__, wikihost, wxString::Format( request, newsearchtext ) );

	// Anfrage mit Proxy?
	if ( proxyhost.length() > 0 )
	{
		// Verbinden
		m_http.Connect( proxyhost, proxyport );

		// Puffer setzen
		m_http.SetPostBuffer( "Host: " + wikihost);

		// Adresse erweitern
		request = "http://" + wikihost + request;

		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Verbinden über Proxy %s:%d."), __LINE__, proxyhost, proxyport );
	}

	// kein Proxy
	else
	{
		// Verbinden
		m_http.Connect( wikihost );

		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Verbinden ohne Proxy."), __LINE__ );
	}


	// InputStream erzeugen
	wxInputStream *httpStream = m_http.GetInputStream( wxString::Format( request, searchtext ));

	// Stream ok?
	if (m_http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
		wxStringOutputStream dataStream(&data);

		// Daten lesen
		httpStream->Read(dataStream);

		// Daten anzeigen
		wxLogMessage( _(L"ProviderWiki, %d: Lade Exportseite."), __LINE__ );
		bGotData = LoadData(data);

		// Daten gefunden?
		if ( bGotData )
		{
			wxLogMessage( _(L"ProviderWiki, %d: Lade Daten."), __LINE__ );
			bGotData = ParseData();
		}
	}

	// Fehler beim Verbinden?
	else
	{
		// Fehler ins Log schreiben
		wxLogError( _(L"ProviderWiki, %d: Fehler %d beim Verbinden."), __LINE__, m_http.GetError() );
	}

	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	m_http.Close();

	// Keine Daten gefunden?
	return bGotData;
}

// 2. Daten mit entsprechender ID holen
void CBerylliumProviderWiki::GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data )
{
	// Quelle anfügen
	tempdata.source.provider	= GetName();
	tempdata.source.url			= wxString::Format( "http://de.wikipedia.org/wiki/%s", searchname );
	tempdata.source.fetched		= wxDateTime::Now().Format("%d. %B %Y %H:%M");

	// Englische Wiki?
	if ( iLanguage == wxLANGUAGE_ENGLISH )
		tempdata.source.url	= wxString::Format( "http://en.wikipedia.org/wiki/%s", searchname );

	// ID interessiert uns nicht, da wir sowieso nur eine Substanz haben
	m_data = tempdata;

	// Protokoll
	wxLogMessage( _(L"ProviderWiki, %d: Daten für '%s' werden geladen."), __LINE__, tempdata.GetName() );
}

// Get: Liste mit Substanzen holen
void CBerylliumProviderWiki::GetResults( std::vector< result > &m_list ) const
{
	// Haben nur ein Ergebnis, das wir direkt übergeben
	IProvider::result temp;

	temp.name	= tempdata.GetName();
	temp.serial	= 0;

	// Hinzufügen
	m_list.push_back(temp);
}

// Lädt die Daten aus einem String
bool CBerylliumProviderWiki::LoadData( const wxString data )
{
	// String-Stream anlegen
	wxStringInputStream stream( data );

	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
	if ( !doc.Load( stream ) )
	{
		wxLogError( _(L"ProviderWiki, %d: XML-Stream konnte nicht geladen werden."), __LINE__ );
		return false;
	}

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "mediawiki")
	{
		wxLogError( _(L"ProviderWiki, %d: XML-Wurzel 'mediawiki' konnte nicht gefunden werden."), __LINE__ );
        return false;
	}

	// Daten parsen
	wxLogMessage( _(L"ProviderWiki, %d: Wiki-Daten werden eingelesen."), __LINE__ );

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = doc.GetRoot()->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächster Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Seitendaten suchen
		if ( node->GetName().compare("page") != 0 )
		{
			// Nächster Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Daten parsen
		wxLogMessage( _(L"ProviderWiki, %d: Inhalt der Seite gefunden."), __LINE__ );

		// Seitendaten lesen
		wxXmlNode *pagenode = node->GetChildren();

		while (pagenode)
		{
			// Nur Elemente sammeln
			if ( pagenode->GetType() != wxXML_ELEMENT_NODE )
			{
				// Nächste Ast
				pagenode = pagenode->GetNext();

				// Fortsetzen
				continue;
			}

			// Revision-Node?
			if ( pagenode->GetName().compare("revision") == 0 )
			{
				// Daten parsen
				wxLogMessage( _(L"ProviderWiki, %d: Revision der Seite gefunden."), __LINE__ );

				// Seitendaten lesen
				wxXmlNode *revisionnode = pagenode->GetChildren();

				while (revisionnode)
				{
					// Nur Elemente sammeln
					if ( revisionnode->GetType() != wxXML_ELEMENT_NODE )
					{
						// Nächster Ast
						revisionnode = revisionnode->GetNext();

						// Fortsetzen
						continue;
					}

					// Nur Text einsammeln
					if ( revisionnode->GetName().compare("text") == 0 )
					{
						// Daten parsen
						wxLogMessage( _(L"ProviderWiki, %d: Text der Revision geladen."), __LINE__ );

						// Text der Seite
						content = revisionnode->GetNodeContent();
					}

					// Nächster Ast
					revisionnode = revisionnode->GetNext();
				}
			}

			// Nächster Ast
			pagenode = pagenode->GetNext();
		}


		// Nächster Ast
		node = node->GetNext();
	};

	// Kein Text gefunden?
	if ( content.length() == 0 )
		return false;

	// Keine Fehler bisher... also haben wir wohl Daten geladen
	return true;
}

// Parst die Daten
bool CBerylliumProviderWiki::ParseData()
{
	// Daten parsen
	wxLogMessage( _(L"ProviderWiki, %d: Interpretiere Daten."), __LINE__ );

	// Die Infobox wird von {{ und }} eingeschlossen
	wxString szBlock = ExtractFromBrackets( content );

	// Namen löschen
	tempdata.szNames.clear();

	// Alle Blöcke holen
	while ( szBlock.length() > 0 )
	{
		// Erstmal alle Kommentare vernichten
		RemoveBlockFromString( szBlock, "<!--", "-->" );

		// Separator suchen
		wxString::size_type iSeparator = szBlock.find("|");

		// Separator gefunden? -> Multiblock
		if ( iSeparator != wxString::npos )
		{
			// Templatenamen holen
			wxString szTemplate = szBlock.substr(0,iSeparator);

			// Links und Rechts trimmen
			szTemplate.Trim( false );
			szTemplate.Trim( true );

			// Infobox: Chemikalie
			if ( szTemplate.CmpNoCase("Infobox Chemikalie") == 0 )
			{
				// Daten parsen
				wxLogMessage( _(L"ProviderWiki, %d: Multiblock 'Infobox Chemikalie' gefunden."), __LINE__ );

				ParseInfoBoxSubstance( szBlock );

				// Schleife verlassen
				break;
			}

			// Infobox: Chemisches Element
			else if ( szTemplate.CmpNoCase("Infobox Chemisches Element") == 0 )
			{
				// Daten parsen
				wxLogMessage( _(L"ProviderWiki, %d: Multiblock 'Infobox Chemisches Element' gefunden."), __LINE__ );

				ParseInfoBoxElement( szBlock );

				// Schleife verlassen
				break;
			}

			// Englische Infobox: Chemikalie
			else if ( szTemplate.CmpNoCase("chembox") == 0 )
			{
				// Daten parsen
				wxLogMessage( _(L"ProviderWiki, %d: Multiblock 'chembox' gefunden."), __LINE__ );

				ParseInfoBoxSubstance( szBlock );

				// Schleife verlassen
				break;
			}

			// Englische Infobox: Medikamente
			else if ( szTemplate.CmpNoCase("drugbox") == 0 )
			{
				// Daten parsen
				wxLogMessage( _(L"ProviderWiki, %d: Multiblock 'drugbox' gefunden."), __LINE__ );

				ParseDrugBox( szBlock );

				// Schleife verlassen
				break;
			}
		}

		// Nächsten Block suchen
		szBlock = ExtractFromBrackets( content );
	}

	// Sollte es keinen Namen geben, dann "searchname"
	if ( tempdata.szNames.size() == 0 )
		tempdata.ReplaceNamesBy( searchname.ToStdString() );

	// Keine Formel, aber tempFormel?
	if ( tempdata.GetFormula().length() == 0 )
		tempdata.SetFormula( tempSumformula );

	// Leere Substanzdaten?
	if ( (tempdata.szCAS.size() == 0) && (tempdata.GetFormula().length() == 0) )
	{
		// Daten parsen
		wxLogMessage( _(L"ProviderWiki, %d: Keine Daten geladen. Vermutlich kein standardkonformer Wiki-Artikel."), __LINE__ );

		return false;
	}

	// Ansonsten alles richtig gelaufen
	return true;
}

// Hilfsfunktion: Parst die Infobox für Chemikalien
void CBerylliumProviderWiki::ParseInfoBoxSubstance( wxString szBlock )
{
	// Parameter und Wert
	wxString szParameter;
	wxString szValue;

	// Erstes Tupel holen
	GetParameterTuple( szBlock, szParameter, szValue );

	// Hier die ganzen Parameter auslesen
	while ( szParameter.length() > 0 )
	{
		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Datentupel (%s,%s) gefunden."), __LINE__, szParameter, szValue );

		// Andere Namen
		if ( (szParameter.compare("Andere Namen") == 0) || (szParameter.compare("Name") == 0) )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Suchtext als Name anfügen
			szValue = searchname + szValue;

			// Neue Zeile, etc. entfernen
			szValue.Replace( "\n", "", true );

			// Namen setzen
			tempdata.SetNames( szValue, "*" );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Namen' interpretiert."), __LINE__ );
		}

		// Englische Wiki: Eigentlicher Name
		else if ( (szParameter.compare("IUPACName") == 0) || (szParameter.compare("IUPAC_name") == 0) )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Suchstring
			wxString newnames = searchname;

			// + IUPAC-Name, falls nicht identisch
			if ( searchname.compare(szValue) != 0 )
				newnames += "; " + szValue;

			// Namen setzen
			tempdata.SetNames( newnames, "; ");

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'IUPAC-Namen' interpretiert."), __LINE__ );
		}

		// Englische Wiki: Andere Namen
		else if ( szParameter.compare("OtherNames") == 0 )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Namen holen
			wxString oldnames = tempdata.GetNames(", ");

			// Sollte "oldnames" nur "Leere Substanzdaten" enthalten, dann durch Suchstring ersetzen
			if ( oldnames.compare( _(L"Leere Substanzdaten") ) == 0 )
				oldnames = searchname;

			// anhängen
			szValue = oldnames + ", " + szValue;

			// Neue Zeile, etc. entfernen
			szValue.Replace( "\n", "", true );
			szValue.Replace( "<br>", " ", true );

			// Namen setzen (wichtig ist das Leerzeichen nach dem Komma!)
			tempdata.SetNames( szValue, ", " );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Synonyme' interpretiert."), __LINE__ );
		}

		// Englische Wiki: SectionX (X = 1 bis 9)
		else if ( szParameter.StartsWith("Section") )
		{
			// Block rausholen
			wxString szSection = ExtractFromBrackets( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> Untersektion '%s'."), __LINE__, szParameter );

			// "SectionX" parsen
			ParseSectionBox( szSection );
		}



		// Summenformel
		else if ( szParameter.compare("Summenformel") == 0 )
		{
			// Alles ab dem ersten Leerzeichen abschneiden
			wxString::size_type iEmpty = szValue.find(" ");

			if ( iEmpty != wxString::npos )
				szValue = szValue.substr(0, iEmpty-1);

			// Formel setzen
			tempdata.SetFormula( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Summenformel' interpretiert."), __LINE__ );
		}

		// CAS-Nummer
		else if ( (szParameter.compare("CAS") == 0) || (szParameter.compare("CAS_number") == 0) )
		{
			tempdata.szCAS = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'CAS-Nummer' interpretiert."), __LINE__ );
		}

		// Molare Masse
		else if ( szParameter.compare("Molare Masse") == 0 )
		{
			tempdata.SetMolWeight( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Molare Masse' interpretiert."), __LINE__ );
		}

		// Dichte
		else if ( szParameter.compare("Dichte") == 0 )
		{
			tempdata.SetDensity( szValue );

			// Korrektur
			if ( tempdata.dbDensity > 10.0 )
				tempdata.dbDensity = tempdata.dbDensity / 1000.0;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Dichte' interpretiert."), __LINE__ );
		}

		// Siedepunkt
		else if ( szParameter.compare("Siedepunkt") == 0 )
		{
			tempdata.szBoilingPoint = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt
		else if ( szParameter.compare("Schmelzpunkt") == 0 )
		{
			// Zersetzung?
			if ( szValue.Find( "Zersetzung" ) != wxNOT_FOUND )
			{
				// Zersetzung
				tempdata.bDecomposition = true;

				// "Zersetzung bei" löschen
				szValue.Replace("Zersetzung bei ", "", true );
				szValue.Replace("Zersetzung ab ", "", true );
			}

			// Zuweisen
			tempdata.szMeltingPoint = szValue.ToStdString();

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt' interpretiert."), __LINE__ );
		}

		// Flammpunkt gibts in der deutschen Wikipedia nicht.

		// Gefahrensymbole
		else if ( szParameter.compare("Gefahrensymbole") == 0 )
		{
			ParseParameterList( tempdata.szHarzards, szValue );

			for ( unsigned int i = 0; i < tempdata.szHarzards.size(); i++ )
			{
				for ( unsigned int j = 0; j < tempdata.szHarzards[i].size(); j++ )
					if ( j != 0 )
						tempdata.szHarzards[i][j] = tolower( tempdata.szHarzards[i][j] );

				if ( tempdata.szHarzards[i].compare("Ff") == 0 )
					tempdata.szHarzards[i] = "F+";

				if ( tempdata.szHarzards[i].compare("Tt") == 0 )
					tempdata.szHarzards[i] = "T+";
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Gefahrensymbole (alt)' interpretiert."), __LINE__ );
		}

		// R-Sätze
		else if ( szParameter.compare("R") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szRisks += "-";

				tempdata.szRisks += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'R-Sätze' interpretiert."), __LINE__ );
		}

		// S-Sätze
		else if ( szParameter.compare("S") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szSafeties += "-";

				tempdata.szSafeties += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'S-Sätze' interpretiert."), __LINE__ );
		}

		// GHS-Signalwort
		else if ( szParameter.compare("GHS-Signalwort") == 0 )
		{
			tempdata.szSignalWord = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Signalwort (GHS)' interpretiert."), __LINE__ );

		}

		// GHS-Piktogramme
		else if ( szParameter.compare("GHS-Piktogramme") == 0 )
		{
			ParseParameterList( tempdata.szGHSSymbols, szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'GHS-Piktogramme' interpretiert."), __LINE__ );

		}

		// H-Sätze
		else if ( szParameter.compare("H") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szHarzardSatements += "-";

				tempdata.szHarzardSatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'H-Sätze' interpretiert."), __LINE__ );
		}

		// P-Sätze
		else if ( szParameter.compare("P") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szPrecautionaryStatements += "-";

				tempdata.szPrecautionaryStatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'P-Sätze' interpretiert."), __LINE__ );
		}

		// MAK
		else if ( szParameter.compare("MAK") == 0 )
		{
			// Wenn es ein "bzw" gibt, dann löschen wir alle Werte danach
			wxString::size_type	iBZW = szValue.find("bzw");

			if ( iBZW != wxString::npos )
				szValue = szValue.substr( 0, iBZW-1);

			// Umwandeln
			tempdata.SetMAK( szValue.ToStdString() );

			// Einheit bestimmten (standard mg/m³)
			if ( szValue.find("ml·m-3") != wxString::npos )
				tempdata.bMAKppm = true;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'MAK' interpretiert."), __LINE__ );
		}

		// WGK
		else if ( szParameter.compare("WGK") == 0 )
		{
			szValue.ToLong( (long*)&tempdata.iWGK );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'WGK' interpretiert."), __LINE__ );
		}

		// Sonst
		else
		{
			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> wurde nicht interpretiert. Schlüssel unbekannt."), __LINE__ );
		}

		// Nächster Parameter
		GetParameterTuple( szBlock, szParameter, szValue );
	}
}

// Hilfsfunktion: Parst die Infobox für Elemente
void CBerylliumProviderWiki::ParseInfoBoxElement( wxString szElement )
{
	// Parameter
	wxString szParameter;
	wxString szValue;

	// Erstes Tupel holen
	GetParameterTuple( szElement, szParameter, szValue );

	// Hier die ganzen Parameter auslesen
	while ( szParameter.length() > 0 )
	{
		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Datentupel (%s,%s) gefunden."), __LINE__, szParameter, szValue );

		// Name
		if ( szParameter.compare("Name") == 0 )
		{
			tempdata.SetNames( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Name' interpretiert."), __LINE__ );
		}

		// Summenformel
		if ( szParameter.compare("Symbol") == 0 )
		{
			tempdata.SetFormula( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Symbol/Formel' interpretiert."), __LINE__ );
		}

		// CAS-Nummer
		if ( szParameter.compare("CAS") == 0 )
		{
			tempdata.szCAS = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'CAS-Nummer' interpretiert."), __LINE__ );
		}

		// Molare Masse
		if ( szParameter.compare("Atommasse") == 0 )
		{
			tempdata.SetMolWeight( szValue.ToStdString() );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Atommasse' interpretiert."), __LINE__ );
		}

		// Dichte
		if ( szParameter.compare("Dichte") == 0 )
		{
			tempdata.SetDensity( szValue.ToStdString() );

			if ( tempdata.dbDensity > 100.0 )
				tempdata.dbDensity = tempdata.dbDensity / 1000.0;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Dichte' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt
		if ( szParameter.compare("Schmelzpunkt_C") == 0 )
		{
			tempdata.szMeltingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt' interpretiert."), __LINE__ );
		}

		// Siedepunkt
		if ( szParameter.compare("Siedepunkt_C") == 0 )
		{
			tempdata.szBoilingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt' interpretiert."), __LINE__ );
		}

		// Gefahrensymbole
		if ( szParameter.compare("Gefahrensymbole") == 0 )
		{
			ParseParameterList( tempdata.szHarzards, szValue );

			for ( unsigned int i = 0; i < tempdata.szHarzards.size(); i++ )
			{
				for ( unsigned int j = 0; j < tempdata.szHarzards[i].size(); j++ )
					if ( j != 0 )
						tempdata.szHarzards[i][j] = tolower( tempdata.szHarzards[i][j] );

				if ( tempdata.szHarzards[i].compare("Ff") == 0 )
					tempdata.szHarzards[i] = "F+";

				if ( tempdata.szHarzards[i].compare("Tt") == 0 )
					tempdata.szHarzards[i] = "T+";
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Gefahrensymbole (alt)' interpretiert."), __LINE__ );
		}

		// R-Sätze
		if ( szParameter.compare("R") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szRisks += "-";

				tempdata.szRisks += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'R-Sätze' interpretiert."), __LINE__ );
		}

		// S-Sätze
		if ( szParameter.compare("S") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szSafeties += "-";

				tempdata.szSafeties += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'S-Sätze' interpretiert."), __LINE__ );
		}

		// GHS-Signalwort
		if ( szParameter.compare("GHS-Signalwort") == 0 )
		{
			tempdata.szSignalWord = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Signalwort (GHS)' interpretiert."), __LINE__ );
		}

		// GHS-Piktogramme
		if ( szParameter.compare("GHS-Piktogramme") == 0 )
		{
			ParseParameterList( tempdata.szGHSSymbols, szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'GHS-Piktogramme' interpretiert."), __LINE__ );
		}

		// H-Sätze
		if ( szParameter.compare("H") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szHarzardSatements += "-";

				tempdata.szHarzardSatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'H-Sätze' interpretiert."), __LINE__ );
		}

		// P-Sätze
		if ( szParameter.compare("P") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szPrecautionaryStatements += "-";

				tempdata.szPrecautionaryStatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'P-Sätze' interpretiert."), __LINE__ );
		}

		// Nächster Parameter
		GetParameterTuple( szElement, szParameter, szValue );
	}
}

// Hilfsfunktion: Holt ein Parameter <-> Wert - Tupel aus der Infobox
void CBerylliumProviderWiki::GetParameterTuple( wxString &szText, wxString &szName, wxString &szValue )
{
	// Raus hier, falls Text weniger als 3 Zeichen hat
	if ( szText.length() < 3 )
	{
		// Name und Wert zurücksetzen
		szName = szValue = "";

		return;
	}

	// Separator finden
	wxString::size_type iSeparator		= -1;
	wxString::size_type iEqual			= -1;
	wxString::size_type iSeparatorNext	= -1;

	// Counter... wir wollen nur den umschließenden Block haben.
	int iCounter = 0;

	// Text durchgehen
	for ( unsigned int i = 0; i < (szText.length()-1); i++ )
	{
		// Counter erhöhen
		if ( (szText[i] == '{') && (szText[i+1] == '{') )
			iCounter++;

		else if ( (szText[i] == '[') && (szText[i+1] == '[') )
			iCounter++;

		// Counter erniedrigen
		else if ( (szText[i] == '}') && (szText[i+1] == '}') )
			iCounter--;

		else if ( (szText[i] == ']') && (szText[i+1] == ']') )
			iCounter--;

		// Wir sind auf der höchsten Ebene?
		if ( iCounter == 0 )
		{
			if ( (szText[i] == '|') && ( iSeparator == wxString::npos ) )
				iSeparator = i;

			else if ( (szText[i] == '=') && ( iEqual == wxString::npos ) )
				iEqual = i;

			else if ( (szText[i] == '|') && ( iSeparator != wxString::npos ) )
			{
				iSeparatorNext = i;
				break;
			}
		}
	}

	// Kein =-Zeichen? -> Tschüss.
	if ( iEqual == wxString::npos )
	{
		// Parameter löschen
		szName = "";

		// Zurück
		return;
	}

	// Falls kein nächste |-Zeichen vorhanden -> Ende des Textes nehmen
	if ( iSeparatorNext == wxString::npos )
		iSeparatorNext = szText.length();

	szName		= szText.substr( iSeparator+1, iEqual-iSeparator-1 );
	szValue		= szText.substr( iEqual+1, iSeparatorNext - iEqual - 1 );
	szText		= szText.substr( iSeparatorNext );

	// Parst die Werte
	ParseParameterValue( szValue );

	// HTML-Leerzeichen ersetzen
	szValue.Replace( wxT("&nbsp;"), " ", true );

	// HTML-Umbruch abschneiden (nur deutsche Wiki)
	if ( iLanguage == wxLANGUAGE_GERMAN )
	{
		wxString::size_type iBreak = szValue.Find( wxT("<br />") );

		if ( iBreak != wxString::npos )
			szValue = szValue.substr( 0, iBreak );
	}

	// andernfalls nur rauswerfen
	else
		szValue.Replace( wxT("<br />"), " ", true );

	RemoveBlockFromString( szValue, "<ref", "</ref>");
	RemoveBlockFromString( szValue, "<", ">");
	RemoveBlockFromString( szValue, "[", "]");
	RemoveBlockFromString( szValue, "&", ";"); // HTML-Codes entfernen

	// Hack für das "komische" Minuszeichen aus Wikipedia
	szValue.Replace( wxT("−"), "-", true );

	// Ersetzen von UTF8 überbleibseln
	szValue.Replace( "Â", "", true );

	// Ganz wichtig, sonst klappt die Umwandlung in einen std::string nicht mehr...
	//szValue = szValue.ToUTF8();

	// Leerzeichen usw. entfernen
	szName.Trim( false );	szName.Trim( true );
	szValue.Trim( false );	szValue.Trim( true );
	szText.Trim( false );	szText.Trim( true );

	// Zurück
	return;
}

// Hilfsfunktion: Parst "szValue" eines Parameter <-> Wert - Tupels
void CBerylliumProviderWiki::ParseParameterValue( wxString &szValue )
{
	// Counter
	int iCounter	= 0;

	// Anfang und Ende speichern
	int iBra		= -1;
	int iKet		= -1;

	while ( szValue.length() > 0 )
	{
		iBra = -1;
		iKet = -1;

		// Text durchgehen (von 0 bis n-1)
		for ( unsigned int i = 0; i < (szValue.length()-1); i++ )
		{
			// Öffnende Klammern gefunden?
			if ( (szValue[i] == '[') && (szValue[i+1] == '[') )
			{
				iBra = i;

				//iCounter++;
			}

			// Schließende Klammern gefunden?
			if ( (szValue[i] == ']') && (szValue[i+1] == ']') )
			{
				iKet = i;
				break;

				//iCounter--;
			}
		}

		// Nichts gefunden oder unvollständig?
		if ( (iBra == -1) || (iKet == -1) || (iCounter != 0) )
			return;

		// Ansonsten speichern
		wxString szBlock = szValue.substr( iBra+2, (iKet-iBra) - 2 );

		// Wenn |-Zeichen vorhanden -> Block wird durch T2 ersetzt, ansonsten durch den Text selbst
		wxString::size_type	iSeparator = szBlock.find("|");

		if ( iSeparator != wxString::npos )
			szBlock = szBlock.substr( iSeparator+1 );

		szBlock.Trim( false ); szBlock.Trim( true );

		szValue = szValue.substr( 0, iBra ) + szBlock + szValue.substr( iKet+2 );
	}
}

// Hilfsfunktion: Parst "szValue" eines Listen-Parameters und fügt die Werte in Liste ein
void CBerylliumProviderWiki::ParseParameterList( std::vector< wxString > &sList, wxString sSymbols )
{
	// Erstmal Liste löschen
	sList.clear();

	// Von {{ und }} trimmen
	int iBra = sSymbols.find("{{");

	if ( iBra != -1 )
		sSymbols = sSymbols.substr( iBra + 2 );

	int iKet = sSymbols.find("}}");

	if ( iKet != -1 )
		sSymbols = sSymbols.substr( 0, iKet );

	// Separator suchen
	int iSeparator = sSymbols.find("|");

	if ( iSeparator == -1 )
		return;

	sSymbols = sSymbols.substr( iSeparator+1 );

	while ( sSymbols.size() > 0 )
	{
		sSymbols.Trim( false );
		sSymbols.Trim( true );

		int a = sSymbols.find("|");

		// Falls kein Trennzeichen gefunden -> der ganze String ist ein Symbol
		if ( a == -1 )
		{
			if ( sSymbols.compare("-") != 0 )
				sList.push_back(sSymbols.ToStdString());

			break;
		}

		// Substring holen, pushen und entfernen
		wxString sSub = sSymbols.substr(0,a);
		sSub.Trim( false ); sSub.Trim( true );

		if ( sSub.length() > 0 )
			sList.push_back(sSub.ToStdString());

		sSymbols.erase(0,a+1);
	}
}

// Hilfsfunktion: Gibt alles zurück was zwischen {{ und }} zu finden ist
// und entfernt es aus dem Text
wxString CBerylliumProviderWiki::ExtractFromBrackets( wxString &szText )
{
	// Counter
	int iCounter	= 0;

	// Anfang und Ende speichern
	int iBra		= -1;
	int iKet		= -1;

	// Text durchgehen (von 0 bis n-1)
	for ( unsigned int i = 0; i < (szText.length()-1); i++ )
	{
		// Öffnende Klammern gefunden?
		if ( (szText[i] == '{') && (szText[i+1] == '{') )
		{
			if ( iBra == -1 )
				iBra = i;

			iCounter++;
		}

		// Schließende Klammern gefunden?
		if ( (szText[i] == '}') && (szText[i+1] == '}') )
		{
			iCounter--;

			if ( iCounter == 0 )
			{
				iKet = i;
				break;
			}
		}
	}

	// Nichts gefunden oder unvollständig?
	if ( (iBra == -1) || (iKet == -1) || (iCounter != 0) )
		return "";

	// Ansonsten speichern, aus dem Text entfernen und zurückgeben
	wxString szBlock = szText.substr( iBra+2, (iKet-iBra) - 2 );

	szText = szText.substr( 0, iBra ) + szText.substr( iKet+2 );

	return szBlock;
}

// Hilfsfunktion: Parst einen der Section-Blocks der englischen Wiki
void CBerylliumProviderWiki::ParseSectionBox( wxString szBlock )
{
	// Parameter und Wert
	wxString szParameter;
	wxString szValue;

	// Erstes Tupel holen
	GetParameterTuple( szBlock, szParameter, szValue );

	// Hier die ganzen Parameter auslesen
	while ( szParameter.length() > 0 )
	{
		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Datentupel (%s,%s) in Sektion gefunden."), __LINE__, szParameter, szValue );

		// Summenformel
		if ( szParameter.compare("Formula") == 0 )
		{
			// Alle HTML-Codes entfernen (sub/sup)
			RemoveBlockFromString( szValue, "<", ">" );

			// Formel setzen
			tempdata.SetFormula( szValue.ToStdString() );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Summenformel' interpretiert."), __LINE__ );
		}

		// CAS-Nummer
		else if ( szParameter.compare("CASNo") == 0 )
		{
			tempdata.szCAS = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'CAS-Nummer' interpretiert."), __LINE__ );
		}

		// Molare Masse
		else if ( (szParameter.compare("ExactMass") == 0) || (szParameter.compare("MolarMass") == 0) )
		{
			tempdata.SetMolWeight( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Molare Masse' interpretiert."), __LINE__ );
		}

		// Dichte
		else if ( szParameter.compare("Density") == 0 )
		{
			tempdata.SetDensity( szValue.ToStdString() );

			// Korrektur
			if ( tempdata.dbDensity > 10.0 )
				tempdata.dbDensity = tempdata.dbDensity / 1000.0;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Dichte' interpretiert."), __LINE__ );
		}

		// Siedepunkt in Kelvin
		else if ( szParameter.compare("BoilingPtK") == 0 )
		{
			// Umwandeln
			double dbBoilingPoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbBoilingPoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szBoilingPoint = wxString::Format("%.2f °C", dbBoilingPoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Siedepunkt in Grad Celsius
		else if ( szParameter.compare("BoilingPtC") == 0 )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szBoilingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt in Kelvin
		else if ( szParameter.compare("MeltingPtK") == 0 )
		{
			// Umwandeln
			double dbMeltingPoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbMeltingPoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szMeltingPoint = wxString::Format("%.2f °C", dbMeltingPoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt in Grad Celsius
		else if ( szParameter.compare("MeltingPtC") == 0 )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szMeltingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Flammenpunkt in Kelvin
		else if ( szParameter.compare("FlashPtK") == 0 )
		{
			// Umwandeln
			double dbFlashpoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbFlashpoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szFlashPoint = wxString::Format("%.2f °C", dbFlashpoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Flammenpunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Flammenpunkt in Grad Celsius
		else if ( szParameter.compare("FlashPtC") == 0 )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szFlashPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Flammenpunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Zersetzung (nur beim Schmelzen)
		else if ( szParameter.compare("Melting_notes") == 0 )
		{
			if ( szValue.Find("Decomposes") > -1 )
				tempdata.bDecomposition = true;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Zersetzung' interpretiert."), __LINE__ );
		}

		// Gefahrensymbole
		else if ( (szParameter.compare("EUClass") == 0) || (szParameter.compare("MainHazards") == 0) )
		{
			// Neues Format?
			if ( szValue.Find("{{Hazchem ") > -1 )
			{
				// in "altes Format" transferieren
				szValue.Replace("{{Hazchem ", "('''", true );
				szValue.Replace("}}", "''')", true );
			}

			// Alle Gefahrensymbole sind zwischen (''' und ''') zu finden... also müssen wir das nur ausnutzen
			wxString hazardsymbol = ExtractAndRemoveFromString( szValue, "('''", "''')" );

			// Solange wir was finden...
			while ( hazardsymbol.length() > 0 )
			{
				// Die Symbole entfernen
				hazardsymbol.Replace( "('''", "", true ); hazardsymbol.Replace( "''')", "", true );

				// ... Symbol hinzufügen
				tempdata.szHarzards.push_back(hazardsymbol);

				// und weitersuchen
				hazardsymbol = ExtractAndRemoveFromString( szValue, "('''", "''')" );
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Gefahrensymbole (alt)' interpretiert."), __LINE__ );
		}

		// R-Sätze
		else if ( szParameter.compare("RPhrases") == 0 )
		{
			// Alle Sätze sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Counter
			int n = 0;

			// Solange wir was finden...
			while ( phrase.length() > 0 )
			{
				// Die Symbole entfernen
				phrase.Replace( "{{", "", true ); phrase.Replace( "}}", "", true ); phrase.Replace( "R", "", true );

				// ... Satz hinzufügen
				tempdata.szRisks += ( (n > 0) ? "-" : "" ) + phrase;

				// und weitersuchen
				phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

				// Zählen
				n++;
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'R-Sätze' interpretiert."), __LINE__ );
		}

		// S-Sätze
		else if ( szParameter.compare("SPhrases") == 0 )
		{
			// Alle Sätze sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Counter
			int n = 0;

			// Solange wir was finden...
			while ( phrase.length() > 0 )
			{
				// Die Symbole entfernen
				phrase.Replace( "{{", "", true ); phrase.Replace( "}}", "", true ); phrase.Replace( "S", "", true );

				// ... Satz hinzufügen
				tempdata.szSafeties += ( (n > 0) ? "-" : "" ) + phrase;

				// und weitersuchen
				phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

				// Zählen
				n++;
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'S-Sätze' interpretiert."), __LINE__ );
		}

		// GHS-Signalwort
		else if ( szParameter.compare("GHSSignalWord") == 0 )
		{
			// Die Symbole entfernen
			szValue.Replace( "'''", "", true );

			// Signalwort ist normalerweise nur in Großbuchstaben geschrieben, also hier erstmal verkleinern
			tempdata.szSignalWord = szValue.MakeCapitalized();

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Signalwort (GHS)' interpretiert."), __LINE__ );
		}

		// GHS-Piktogramme
		else if ( szParameter.compare("GHSPictograms") == 0 )
		{
			// Alle Symbole sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString symbol = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Solange wir was finden...
			while ( symbol.length() > 0 )
			{
				// Die Symbole entfernen
				symbol.Replace( "{{", "", true ); symbol.Replace( "}}", "", true );

				// Symbol ersetzen
				symbol.Replace( "GHS exploding bomb", "01", true );
				symbol.Replace( "GHS flame", "02", true );
				symbol.Replace( "GHS flame over circle", "03", true );
				symbol.Replace( "GHS gas cylinder", "04", true );
				symbol.Replace( "GHS corrosion", "05", true );
				symbol.Replace( "GHS skull and crossbones", "06", true );
				symbol.Replace( "GHS exclamation mark", "07", true );
				symbol.Replace( "GHS health hazard", "08", true );
				symbol.Replace( "GHS environment", "09", true );

				// und hinzufügen
				tempdata.szGHSSymbols.push_back( symbol );

				// und weitersuchen
				symbol = ExtractAndRemoveFromString( szValue, "{{", "}}" );
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'GHS-Piktogramme' interpretiert."), __LINE__ );

		}

		// H-Sätze
		else if ( szParameter.compare("HPhrases") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szHarzardSatements += "-";

				tempdata.szHarzardSatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'H-Sätze' interpretiert."), __LINE__ );
		}

		// P-Sätze
		else if ( szParameter.compare("PPhrases") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szPrecautionaryStatements += "-";

				tempdata.szPrecautionaryStatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'P-Sätze' interpretiert."), __LINE__ );
		}

		// als letztes Elemente überprüfen (für die Summenformel)
		else
		{
			// Element gefunden?
			bool bElement = false;

			// alle Elemente durchgehen
			for ( unsigned int i = 0; i < 112; ++i )
			{
				// Übereinstimmung gefunden? Dann zur Formel hinzufügen!
				if ( szParameter.compare(cElements[i]) == 0 )
				{
					tempSumformula += szParameter + (szValue.compare("1") == 0 ? "" : szValue);

					bElement = true;

					// Loggen
					wxLogMessage( _(L"ProviderWiki, %d: -> als 'Element' interpretiert und zur temporären Summenformel hinzugefügt."), __LINE__ );
				}
			}

			// Nichts gefunden?
			if ( !bElement )
				wxLogMessage( _(L"ProviderWiki, %d: -> nicht interpretiert. Schlüssel unbekannt."), __LINE__ );
		}


		// Nächster Parameter
		GetParameterTuple( szBlock, szParameter, szValue );
	}
}

// Hilfsfunktion: Parst 'drugbox' der englischen Wiki
void CBerylliumProviderWiki::ParseDrugBox( wxString szBlock )
{
	// Parameter und Wert
	wxString szParameter;
	wxString szValue;

	// Erstes Tupel holen
	GetParameterTuple( szBlock, szParameter, szValue );

	// Hier die ganzen Parameter auslesen
	while ( szParameter.length() > 0 )
	{
		// Loggen
		wxLogMessage( _(L"ProviderWiki, %d: Datentupel (%s,%s) in Sektion gefunden."), __LINE__, szParameter, szValue );

		// Andere Namen
		if ( (szParameter.compare("Andere Namen") == 0) || (szParameter.compare("Name") == 0) )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Suchtext als Name anfügen
			szValue = searchname + szValue;

			// Neue Zeile, etc. entfernen
			szValue.Replace( "\n", "", true );

			// Namen setzen
			tempdata.SetNames( szValue, "*" );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Namen' interpretiert."), __LINE__ );
		}

		// Englische Wiki: Eigentlicher Name
		else if ( (szParameter.compare("IUPACName") == 0) || (szParameter.compare("IUPAC_name") == 0) )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Suchstring
			wxString newnames = searchname;

			// + IUPAC-Name, falls nicht identisch
			if ( searchname.compare(szValue) != 0 )
				newnames += "; " + szValue;

			// Namen setzen
			tempdata.SetNames( newnames, "; ");

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'IUPAC-Namen' interpretiert."), __LINE__ );
		}

		// Englische Wiki: Andere Namen
		else if ( szParameter.compare("OtherNames") == 0 )
		{
			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Namen holen
			wxString oldnames = tempdata.GetNames(", ");

			// Sollte "oldnames" nur "Leere Substanzdaten" enthalten, dann durch Suchstring ersetzen
			if ( oldnames.compare( _(L"Leere Substanzdaten") ) == 0 )
				oldnames = searchname;

			// anhängen
			szValue = oldnames + ", " + szValue;

			// Neue Zeile, etc. entfernen
			szValue.Replace( "\n", "", true );
			szValue.Replace( "<br>", " ", true );

			// Namen setzen (wichtig ist das Leerzeichen nach dem Komma!)
			tempdata.SetNames( szValue, ", " );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Synonyme' interpretiert."), __LINE__ );
		}

		// Englische Wiki: Synonyme
		else if ( szParameter.compare("synonyms") == 0 )
		{
			szValue.Replace("<br />", ", ", true);

			// Dinge zwischen {{ und }} entfernen
			RemoveBlockFromString( szValue, "{{", "}}" );
			RemoveBlockFromString( szValue, "<", ">" );

			// Namen holen
			wxString oldnames = tempdata.GetNames(", ");

			// Sollte "oldnames" nur "Leere Substanzdaten" enthalten, dann durch Suchstring ersetzen
			if ( oldnames.compare( _(L"Leere Substanzdaten") ) == 0 )
				oldnames = searchname;

			// anhängen
			szValue = oldnames + ", " + szValue;

			// Neue Zeile, etc. entfernen
			szValue.Replace( "\n", "", true );
			szValue.Replace( "<br>", " ", true );

			// Namen setzen (wichtig ist das Leerzeichen nach dem Komma!)
			tempdata.SetNames( szValue, ", " );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Synonyme' interpretiert."), __LINE__ );
		}

		// Englische Wiki: SectionX (X = 1 bis 9)
		else if ( szParameter.StartsWith("Section") )
		{
			// Block rausholen
			wxString szSection = ExtractFromBrackets( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> Untersektion '%s'."), __LINE__, szParameter );

			// "SectionX" parsen
			ParseSectionBox( szSection );
		}

		// Summenformel
		else if ( szParameter.compare("Formula") == 0 )
		{
			// Alle HTML-Codes entfernen (sub/sup)
			RemoveBlockFromString( szValue, "<", ">" );

			// Formel setzen
			tempdata.SetFormula( szValue.ToStdString() );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Summenformel' interpretiert."), __LINE__ );
		}

		// CAS-Nummer
		else if ( (szParameter.compare("CASNo") == 0) || (szParameter.compare("CAS_number") == 0) )
		{
			tempdata.szCAS = szValue;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'CAS-Nummer' interpretiert."), __LINE__ );
		}

		// Molare Masse
		else if ( (szParameter.compare("ExactMass") == 0) || (szParameter.compare("MolarMass") == 0)
			|| (szParameter.compare("molecular_weight") == 0) )
		{
			tempdata.SetMolWeight( szValue );

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Molare Masse' interpretiert."), __LINE__ );
		}

		// Dichte
		else if ( szParameter.CmpNoCase("Density") == 0 )
		{
			tempdata.SetDensity( szValue.ToStdString() );

			// Korrektur
			if ( tempdata.dbDensity > 10.0 )
				tempdata.dbDensity = tempdata.dbDensity / 1000.0;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Dichte' interpretiert."), __LINE__ );
		}

		// Siedepunkt in Kelvin
		else if ( szParameter.compare("BoilingPtK") == 0 )
		{
			// Umwandeln
			double dbBoilingPoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbBoilingPoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szBoilingPoint = wxString::Format("%.2f °C", dbBoilingPoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Siedepunkt in Grad Celsius
		else if ( (szParameter.compare("BoilingPtC") == 0) || (szParameter.compare("boiling_point") == 0) )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szBoilingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Siedepunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt in Kelvin
		else if ( szParameter.compare("MeltingPtK") == 0 )
		{
			// Umwandeln
			double dbMeltingPoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbMeltingPoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szMeltingPoint = wxString::Format("%.2f °C", dbMeltingPoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Schmelzpunkt in Grad Celsius
		else if ( (szParameter.compare("MeltingPtC") == 0) || (szParameter.compare("melting_point") == 0) )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szMeltingPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Schmelzpunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Flammenpunkt in Kelvin
		else if ( szParameter.compare("FlashPtK") == 0 )
		{
			// Umwandeln
			double dbFlashpoint = BeTool::StringToDouble( szValue );

			// Angabe ist in Kelvin, also subtrahieren wir für Celsius einfach 273.15
			dbFlashpoint -= 273.15f;

			// und dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szFlashPoint = wxString::Format("%.2f °C", dbFlashpoint);

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Flammenpunkt (Kelvin)' interpretiert."), __LINE__ );
		}

		// Flammenpunkt in Grad Celsius
		else if ( szParameter.compare("FlashPtC") == 0 )
		{
			// dann setzen wir das ganze einfach mit dem Gradzeichen
			tempdata.szFlashPoint = szValue + " °C";

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Flammenpunkt (Celsius)' interpretiert."), __LINE__ );
		}

		// Zersetzung (nur beim Schmelzen)
		else if ( szParameter.compare("Melting_notes") == 0 )
		{
			if ( szValue.Find("Decomposes") > -1 )
				tempdata.bDecomposition = true;

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Zersetzung' interpretiert."), __LINE__ );
		}

		// Gefahrensymbole
		else if ( (szParameter.compare("EUClass") == 0) || (szParameter.compare("MainHazards") == 0) )
		{
			// Neues Format?
			if ( szValue.Find("{{Hazchem ") > -1 )
			{
				// in "altes Format" transferieren
				szValue.Replace("{{Hazchem ", "('''", true );
				szValue.Replace("}}", "''')", true );
			}

			// Alle Gefahrensymbole sind zwischen (''' und ''') zu finden... also müssen wir das nur ausnutzen
			wxString hazardsymbol = ExtractAndRemoveFromString( szValue, "('''", "''')" );

			// Solange wir was finden...
			while ( hazardsymbol.length() > 0 )
			{
				// Die Symbole entfernen
				hazardsymbol.Replace( "('''", "", true ); hazardsymbol.Replace( "''')", "", true );

				// ... Symbol hinzufügen
				tempdata.szHarzards.push_back(hazardsymbol);

				// und weitersuchen
				hazardsymbol = ExtractAndRemoveFromString( szValue, "('''", "''')" );
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Gefahrensymbole (alt)' interpretiert."), __LINE__ );
		}

		// R-Sätze
		else if ( szParameter.compare("RPhrases") == 0 )
		{
			// Alle Sätze sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Counter
			int n = 0;

			// Solange wir was finden...
			while ( phrase.length() > 0 )
			{
				// Die Symbole entfernen
				phrase.Replace( "{{", "", true ); phrase.Replace( "}}", "", true ); phrase.Replace( "R", "", true );

				// ... Satz hinzufügen
				tempdata.szRisks += ( (n > 0) ? "-" : "" ) + phrase;

				// und weitersuchen
				phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

				// Zählen
				n++;
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'R-Sätze' interpretiert."), __LINE__ );
		}

		// S-Sätze
		else if ( szParameter.compare("SPhrases") == 0 )
		{
			// Alle Sätze sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Counter
			int n = 0;

			// Solange wir was finden...
			while ( phrase.length() > 0 )
			{
				// Die Symbole entfernen
				phrase.Replace( "{{", "", true ); phrase.Replace( "}}", "", true ); phrase.Replace( "S", "", true );

				// ... Satz hinzufügen
				tempdata.szSafeties += ( (n > 0) ? "-" : "" ) + phrase;

				// und weitersuchen
				phrase = ExtractAndRemoveFromString( szValue, "{{", "}}" );

				// Zählen
				n++;
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'S-Sätze' interpretiert."), __LINE__ );
		}

		// GHS-Signalwort
		else if ( szParameter.compare("GHSSignalWord") == 0 )
		{
			// Die Symbole entfernen
			szValue.Replace( "'''", "", true );

			// Signalwort ist normalerweise nur in Großbuchstaben geschrieben, also hier erstmal verkleinern
			tempdata.szSignalWord = szValue.MakeCapitalized();

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'Signalwort (GHS)' interpretiert."), __LINE__ );
		}

		// GHS-Piktogramme
		else if ( szParameter.compare("GHSPictograms") == 0 )
		{
			// Alle Symbole sind zwischen {{ und }} zu finden... also müssen wir das nur ausnutzen
			wxString symbol = ExtractAndRemoveFromString( szValue, "{{", "}}" );

			// Solange wir was finden...
			while ( symbol.length() > 0 )
			{
				// Die Symbole entfernen
				symbol.Replace( "{{", "", true ); symbol.Replace( "}}", "", true );

				// Symbol ersetzen
				symbol.Replace( "GHS exploding bomb", "01", true );
				symbol.Replace( "GHS flame", "02", true );
				symbol.Replace( "GHS flame over circle", "03", true );
				symbol.Replace( "GHS gas cylinder", "04", true );
				symbol.Replace( "GHS corrosion", "05", true );
				symbol.Replace( "GHS skull and crossbones", "06", true );
				symbol.Replace( "GHS exclamation mark", "07", true );
				symbol.Replace( "GHS health hazard", "08", true );
				symbol.Replace( "GHS environment", "09", true );

				// und hinzufügen
				tempdata.szGHSSymbols.push_back( symbol );

				// und weitersuchen
				symbol = ExtractAndRemoveFromString( szValue, "{{", "}}" );
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'GHS-Piktogramme' interpretiert."), __LINE__ );

		}

		// H-Sätze
		else if ( szParameter.compare("HPhrases") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szHarzardSatements += "-";

				tempdata.szHarzardSatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'H-Sätze' interpretiert."), __LINE__ );
		}

		// P-Sätze
		else if ( szParameter.compare("PPhrases") == 0 )
		{
			std::vector< wxString > sList;

			ParseParameterList( sList, szValue );

			for ( unsigned int i = 0; i < sList.size(); i++ )
			{
				if ( i != 0 )
					tempdata.szPrecautionaryStatements += "-";

				tempdata.szPrecautionaryStatements += sList[i];
			}

			// Loggen
			wxLogMessage( _(L"ProviderWiki, %d: -> als 'P-Sätze' interpretiert."), __LINE__ );
		}

		// als letztes Elemente überprüfen (für die Summenformel)
		else
		{
			// Element gefunden?
			bool bElement = false;

			// alle Elemente durchgehen
			for ( unsigned int i = 0; i < 112; ++i )
			{
				// Übereinstimmung gefunden? Dann zur Formel hinzufügen!
				if ( szParameter.compare(cElements[i]) == 0 )
				{
					tempSumformula += szParameter + (szValue.compare("1") == 0 ? "" : szValue);

					bElement = true;

					// Loggen
					wxLogMessage( _(L"ProviderWiki, %d: -> als 'Element' interpretiert und zur temporären Summenformel hinzugefügt."), __LINE__ );
				}
			}

			// Nichts gefunden?
			if ( !bElement )
				wxLogMessage( _(L"ProviderWiki, %d: -> nicht interpretiert. Schlüssel unbekannt."), __LINE__ );
		}


		// Nächster Parameter
		GetParameterTuple( szBlock, szParameter, szValue );
	}
}
