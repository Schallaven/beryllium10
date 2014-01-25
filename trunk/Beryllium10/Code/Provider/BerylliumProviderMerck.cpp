﻿// **********************************************************************************
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

#include "stdafx.h"
#include "BerylliumProviderMerck.h"

#include "../betool.h"

#include <wx/sstream.h>
#include <wx/wfstream.h>

CBerylliumProviderMerck::CBerylliumProviderMerck(void)
{
	// Land setzen
	// Liste: 0 = Deutschland, 1 = UK
	iCountry = 0;
}

CBerylliumProviderMerck::~CBerylliumProviderMerck(void)
{
}

// 1. Suchanfrage stellen
bool CBerylliumProviderMerck::SearchForCompound( const wxString searchtext, bool bExactMatch = false )
{	
	// TODO: Land extern setzen lassen (Beispielsweise über Config)
	// Land kann und soll unabhängig von Sprache sein!
	if ( iLanguage == wxLANGUAGE_ENGLISH )
		iCountry = 1;

	// Ergebnisse löschen
	m_results.clear();

	// Daten erfolgreich gefunden (und geladen)
	bool bGotData = false;

	// HTTP-Anfarge vorbereiten
	m_http.SetHeader( "Content-type", "text/html; charset=utf-8" );
	m_http.SetTimeout(60); // 60 Sekunden TimeOut

	// Anfrage an Merck vorbereiten
	m_http.Connect( "www.merckmillipore.com" );

	// Adresse setzen (Deutschland)
	wxString merckaddress = "/is-bin/INTERSHOP.enfinity/WFS/Merck-DE-Site/de_DE/-/EUR/ViewSearch-ParametricSearchIndexQuery?WFSimpleSearch_NameOrID=%s"; 

	// UK?
	if ( iCountry == 1 )
		merckaddress = "/is-bin/INTERSHOP.enfinity/WFS/Merck-GB-Site/en_US/-/GBP/ViewSearch-ParametricSearchIndexQuery?WFSimpleSearch_NameOrID=%s"; 

	// InputStream erzeugen
	wxInputStream *httpStream = m_http.GetInputStream( wxString::Format( merckaddress, searchtext ));

	// Stream ok?
	if (m_http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
#ifdef _DEBUG
		wxTempFileOutputStream dataStream("temp/mercksearch.temp");
#else
		wxTempFileOutputStream dataStream("beryllium10.temp");
#endif

		// Daten lesen
		httpStream->Read(dataStream);

		// Stream schließen
		dataStream.Close();

		// Daten vorparsen
		PreParseData();		

		// Ergebnisse laden.
		bGotData = LoadSearchData();
	}

	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	m_http.Close();

	// Keine Daten gefunden?
	return bGotData;
}

// 2. Daten mit entsprechender ID holen
void CBerylliumProviderMerck::GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data )
{
	// Datenblock löschen
	CBerylliumSubstanceData newdata; tempdata = newdata;

	// Durchsuchen
	for ( unsigned int i = 0; i < m_results.size(); ++i )
	{
		// Gefunden?
		if ( m_results[i].serial == iSerial )
		{
			// Namen setzen
			substancename = m_results[i].name;
			
			// Daten holen
			LoadDataFromUrl( m_results[i].data );

			// Quelle anfügen
			tempdata.source.provider	= GetName();
			tempdata.source.url			= m_results[i].data;
			tempdata.source.fetched		= wxDateTime::Now().Format("%d. %B %Y %H:%M");

			// Daten kopieren
			m_data = tempdata;
		}
	}
}

// Get: Liste mit Substanzen holen
void CBerylliumProviderMerck::GetResults( std::vector< result > &m_list ) const
{
	// Einfaches kopieren könnte schiefgehen
	for ( unsigned int i = 0; i < m_results.size(); ++i )
		m_list.push_back( m_results[i] );
}

// Lädt die Suchergebnisse
bool CBerylliumProviderMerck::LoadSearchData()
{	
	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
#ifdef _DEBUG
	if ( !doc.Load( "temp/mercksearch.temp" ) )
		return false;
#else
	if ( !doc.Load( "beryllium10.temp" ) )
		return false;
#endif

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "merck")
        return false;

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

		// Wir suchen nach "table" mit dem Attribute summary=Suchergebnisse
		if ( (node->GetName().compare("table") == 0) && 
			((node->GetAttribute("summary").compare("Suchergebnisse") == 0) || (node->GetAttribute("summary").compare("Search results") == 0)) )
		{
			// Dann suchen wir nach allen Subelementen
			wxXmlNode *subnode = node->GetChildren();

			while (subnode)
			{
				// Uns interessieren nur Zeilen
				if ( (subnode->GetType() != wxXML_ELEMENT_NODE) || (subnode->GetName().compare("tr") != 0) )
				{
					// Nächster Ast
					subnode = subnode->GetNext();

					// Weitermachen
					continue;
				}

				// Elementindext
				int i = 0;

				// Die Zeile auslesen
				for( wxXmlNode* cell = subnode->GetChildren(); cell; cell = cell->GetNext() )
				{
					// Keine Zelle?
					if ( (cell->GetType() != wxXML_ELEMENT_NODE) || (cell->GetName().compare("td") != 0) )
						continue;

					// (16.09.2010) Die 4. Zelle (Index = 3) enthält den Link und den Namen
					if ( i == 3 )
						LoadSearchDataCell( cell );

					// Element-Index hochzählen
					i++;						
				}

				// Nächster Ast
				subnode = subnode->GetNext();
			}
		}

		// Nächster Ast
		node = node->GetNext();
	}

	// Wenn wir Daten übernommen haben, dann waren wir erfolgreich
	return (m_results.size() > 0);
}

// Lädt eine Zelle der Suchergebnisse
void CBerylliumProviderMerck::LoadSearchDataCell( wxXmlNode *cell )
{
	// Alle Subelemente suchen
	for( wxXmlNode* element = cell->GetChildren(); element; element = element->GetNext() )
	{
		// Text holen
		wxString content; GetAllTextFromNode( element, content );

		// Neue Zeilen entfernen
		content.Replace( "\n", " ", true );

		// Leer?
		if ( (element->GetName().length() == 0) || (content.length() == 0) )
			continue;

		// Link gefunden?
		if ( element->GetName().compare("a") == 0 )
		{
			// Temporäres Element anlegen
			IProvider::result temp;

			// Attribut lesen
			temp.name	= content;
			temp.data	= element->GetAttribute("href");
			temp.serial = m_results.size();

			// In die Liste eintragen
			m_results.push_back( temp );
		}
	}
}

// Hilfsfunktion: Entfernt alle unwichtigen Dinge, um den String kleiner zu machen
// Wichtig hierbei ist, dass wir mit std::string arbeiten. wxString kapituliert unter
// Windows (UTF16) leider beim richtigen extrahieren von Substrings aus UTF8-Strings...
void CBerylliumProviderMerck::PreParseData()
{
	// Datei zum Lesen einladen
#ifdef _DEBUG
	FILE *fi = fopen( "temp/mercksearch.temp", "r" );
#else
	FILE *fi = fopen( "beryllium10.temp", "r" );
#endif

	// Inhalt der Datei kommt in diesen String
	std::string szContent;

	// Temporärer Buffer
	char buf[257];

	// Alles einlesen
	while ( !feof(fi) )
	{
		// Ein paar Bytes lesen
		size_t count = fread( buf, 1, 256, fi );

		// Wenn was gelesen wurde -> speichern
		if ( count > 0 )
		{
			// Buffer abschließen
			buf[count] = 0x00;

			// Buffer hinzufügen
			szContent += buf;
		}
	}

	// Datei schließen (ENDE: Lesemodus)
	fclose( fi );

	// Extrahierter String
	std::string szExtract;

	szExtract += "<merck>";

	// Nur die Tabelle mit den Suchergebnissen brauchen wir hier
	// Geändert: 16.09.2010				
	szExtract += ExtractFromString( szContent, "<table class=\"table01\" style=\"margin-bottom: 0pt\" summary=\"Suchergebnisse\"", "</table>" );
	szExtract += ExtractFromString( szContent, "<table class=\"table01\" style=\"margin-bottom: 0pt\" summary=\"Search results\"", "</table>" );
	szExtract += ExtractFromString( szContent, "<table class=\"table02\" summary=\"Product summary\"", "</table>" );
	szExtract += ExtractFromString( szContent, "<table class=\"table02\" summary=\"Product Information\"", "</table>" );
	szExtract += ExtractFromString( szContent, "<table class=\"table02\" summary=\"Chemical and physical data\"", "</table>" );
	szExtract += ExtractFromString( szContent, "<table class=\"table02\" summary=\"Safety information\"", "</table>" );
	// GHS-Daten: 20.11.2011
	szExtract += ExtractFromString( szContent, "<table class=\"table02\" summary=\"GHS Safety Information\"", "</table>" );

	szExtract += "</merck>";

	// Content übernehmen
	wxString data = szExtract;

	// Leere Brackets löschen
	RemoveEmptyBrackets( data );

	// Alle Script-Tags entfernen
	RemoveBlockFromString( data, "<script", "</script>" );

	// Alle NoScript-Tags entfernen
	RemoveBlockFromString( data, "<noscript", "</noscript>" );

	// GHS: Gefahrensymbole sind nur als Grafiken verfügbar
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS01.gif\" class=\"hintImage\"/>", "01 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS02.gif\" class=\"hintImage\"/>", "02 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS03.gif\" class=\"hintImage\"/>", "03 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS04.gif\" class=\"hintImage\"/>", "04 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS05.gif\" class=\"hintImage\"/>", "05 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS06.gif\" class=\"hintImage\"/>", "06 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS07.gif\" class=\"hintImage\"/>", "07 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS08.gif\" class=\"hintImage\"/>", "08 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-DE-Site/Merck/de_DE/codes/images/GHS09.gif\" class=\"hintImage\"/>", "09 " );

	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS01.gif\" class=\"hintImage\"/>", "01 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS02.gif\" class=\"hintImage\"/>", "02 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS03.gif\" class=\"hintImage\"/>", "03 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS04.gif\" class=\"hintImage\"/>", "04 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS05.gif\" class=\"hintImage\"/>", "05 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS06.gif\" class=\"hintImage\"/>", "06 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS07.gif\" class=\"hintImage\"/>", "07 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS08.gif\" class=\"hintImage\"/>", "08 " );
	data.Replace( "<img src=\"/is-bin/intershop.static/WFS/Merck-GB-Site/Merck/en_US/codes/images/GHS09.gif\" class=\"hintImage\"/>", "09 " );

	// img-Tags entfernen
	RemoveBlockFromString( data, "<img", "/>" );

	// input-Tags entfernen
	RemoveBlockFromString( data, "<input", "/>" );

	// link-tags entfernen
	RemoveBlockFromString( data, "<link", ">" );

	// die in den Links enthaltenen Ampersands werden rausgeworfen, wenn sie nicht HTML-konform sind
	data.Replace( "&BackButtonText", "&amp;BackButtonText", true );
	data.Replace( "&CatalogCategoryID", "&amp;CatalogCategoryID", true );
	data.Replace( "&OnlineFlag", "&amp;OnlineFlag", true );
	data.Replace( "&PortalCatalogUUID", "&amp;PortalCatalogUUID", true );
	data.Replace( "&ProductUUID","&amp;ProductUUID", true );				// hinzugefügt (13.09.2008)

	// <br> sind evil :(
	data.Replace( "<br>", " ", true);
	data.Replace( "<br/>", " ", true );

	// Auch böse
	data.Replace( " >", ">", true );
	data.Replace( "\n\n", "", true );
	data.Replace( "&nbsp;", " ", true );

	// Elemente die auf jedenfall rausgehören
	data.Replace( "<strong>", "", true );	data.Replace( "</strong>", "", true );
	data.Replace( "<sub>", "", true );		data.Replace( "</sub>", "", true );
	data.Replace( "<sup>", "", true );		data.Replace( "</sup>", "", true );

	// Datei zum Schreiben laden
#ifdef _DEBUG
	fi = fopen( "temp/mercksearch.temp", "w" );
#else
	fi = fopen( "beryllium10.temp", "w" );	
#endif

	// Inhalt reinschreiben
	fwrite( data.c_str(), data.length(), 1, fi );

	// Datei schließen (ENDE: Schreibmodus)
	fclose( fi );
}

// Hilfsfunktion: Entfernt Größer/Kleiner Zeichen mit Leerzeichen
void CBerylliumProviderMerck::RemoveEmptyBrackets( wxString &data )
{
	// Bool für "Innerhalb eines Strings?"
	bool bString = false;

	// Position des letzten Größer/Kleiner-Zeichens
	int iPos = -1;

	// Alle Zeichen durchgehen
	for ( unsigned int i = 0; i < data.length(); ++i )
	{
		// String Anfang/Ende
		if ( data[i] == '\"' )
			bString = !bString;

		// String? -> Dann einfach weitermachen
		if ( bString )
			continue;

		// Größer/Kleiner Zeichen gefunden?
		if ( (data[i] == '<') || (data[i] == '>') )
		{
			// Erste Zeichen im String ist ein Größer-Zeichen => Löschen
			if ( (iPos == -1) && (data[i] == '>') )
			{
				data[i] = ' ';
			}

			// Das aktuelle Zeichen ist das erste Zeichen => Einfach nur Merken und weitermachen
			else if ( iPos == -1 )
			{
				iPos = i;
			}

			// Das aktuelle Zeichen und das letzte Zeichen sind gleich => Letztes löschen
			else if ( data[iPos] == data[i] )
			{
				data[iPos] = ' ';
				iPos = i;
			}

			// Ansonsten nur einfach das aktuelle Zeichen merken
			else
			{
				iPos = i;
			};

		}		
	}
}

// Lädt die Daten für die Substanz unter URL
void CBerylliumProviderMerck::LoadDataFromUrl( wxString URL )
{
	// URL modifizieren
	URL.Replace( "http://www.merckmillipore.com", "", true );

	// HTTP-Anfarge vorbereiten
	m_http.SetHeader( "Content-type", "text/html; charset=utf-8" );
	m_http.SetTimeout(60); // 60 Sekunden TimeOut

	// Anfrage an Merck vorbereiten
	m_http.Connect( "www.merckmillipore.com" );

	// InputStream erzeugen
	wxInputStream *httpStream = m_http.GetInputStream( URL );

	// Stream ok?
	if (m_http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
#ifdef _DEBUG
		wxTempFileOutputStream dataStream("temp/mercksearch.temp");
#else
		wxTempFileOutputStream dataStream("beryllium10.temp");
#endif


		// Daten lesen
		httpStream->Read(dataStream);

		// Stream schließen
		dataStream.Close();

		// Daten vorparsen
		PreParseData();		

		// Ergebnisse laden.
		LoadSubstanceData();		
	}

	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	m_http.Close();

	//
	return;	
}

// Lädt die Daten der Substanz
void CBerylliumProviderMerck::LoadSubstanceData()
{
	// Chemische Formel löschen
	chemicalformula = "";

	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
#ifdef _DEBUG
	if ( !doc.Load( "temp/mercksearch.temp" ) )
		return;
#else
	if ( !doc.Load( "beryllium10.temp" ) )
		return;
#endif

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "merck")
        return;

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = doc.GetRoot()->GetChildren();

	// Namen der Substanz löschen
	tempdata.szNames.clear();

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

		// Wir suchen nach "table" mit dem Attribute summary=...
		if ( (node->GetName().compare("table") == 0) && (node->GetAttribute("summary").length() > 0) )
		{
			// Dann laden wir diesen Block mal...
			LoadSubstanceDataBlock( node );			
		}

		// Nächster Ast
		node = node->GetNext();
	}

	// Keine Namen? Dann wenigstens den einen setzen!
	if ( tempdata.szNames.size() == 0 )
		tempdata.ReplaceNamesBy( substancename.ToStdString() );

	// Keine Formel, dann evtl. chemische Formel setzen!
	if ( (tempdata.GetFormula().size() == 0) && (chemicalformula.size() > 0) )
		tempdata.SetFormula(chemicalformula);

	// Wenn wir Daten übernommen haben, dann waren wir erfolgreich
	return;
}

// Lädt einen Block der Substanzdaten
void CBerylliumProviderMerck::LoadSubstanceDataBlock( wxXmlNode *parent )
{
	// Alle Subelemente suchen
	for( wxXmlNode* node = parent->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Tabellenzeilen interessieren uns
		if ( node->GetName().compare("tr") == 0 )
		{
			// td-Element MIT Inhalt
			int iContent = 0;

			// Schlüssel zwischenspeichern
			wxString sKey;

			// Alle Subelemente suchen
			for( wxXmlNode* subnode = node->GetChildren(); subnode; subnode = subnode->GetNext() )
			{
				// Nur Tabellenzellen interessieren uns
				if ( subnode->GetName().compare("td") != 0 )
					continue;

				// Temporärer String
				wxString sTemp;

				// den ganzen Text einlesen
				GetAllTextFromNode( subnode, sTemp );

				// Schlüssel ...
				if ( (iContent % 2) == 0 )
				{
					// ... zwischenspeichern
					sKey = sTemp;
				}

				// ... oder Daten?
				else
				{
					// Schlüssen und Daten parsen
					LoadSubstanceDataTupel( sKey, sTemp );

					// Schlüssel nullen
					sKey = "";
				}

				// Hochzählen
				iContent++;

			};
		}
	};
}


// Lädt ein Schlüssel/Werte-Paar
void CBerylliumProviderMerck::LoadSubstanceDataTupel( wxString key, wxString value )
{
	// Funktion übersetzt die Daten in Abhängigkeit vom Schlüssel
	key.Trim( false ); key.Trim( true );
	value.Trim( false ); value.Trim( true );

	// Alternative Namen
	if ( (key.compare("Synonyme") == 0) || (key.compare("Synonyms") == 0) )
	{
		// Anzeigename hinzufügen
		value = substancename + "," + value;

		// Namen setzen
		tempdata.SetNames( value.ToStdString(), "," );
	}

	// Summenformel Hill
	else if ( (key.compare("Summenformel Hill") == 0) || (key.compare("Hill Formula") == 0) )
	{
		// Leerzeichen entfernen
		value.Replace( " ", "", true );

		// Summenformel speichern
		tempdata.SetFormula( value.ToStdString() );
	}

	// "Chemical formula"
	else if ( key.compare("Chemical formula") == 0 )
	{
		// Leerzeichen entfernen
		value.Replace( " ", "", true );

		// Abspeichern
		chemicalformula = value.ToStdString();
	}

	// Gefahrensymbole
	else if ( (key.compare("Gefahrensymbol") == 0) || (key.compare("Hazard Symbol") == 0) )
	{
		// Tabs und soweiter entfernen
		value.Replace( "\t", "", true );
		value.Replace( "\n", "", true );

		// String besteht jetzt aus Gefahrentexten... werden hier ersetzt (18.04.2010)		
		value.Replace( "Dangerous for the environment", "N ");
		value.Replace( "Highly flammable", "F+ ");	value.Replace( "Flammable", "F ");	
		value.Replace( "Harmful",			"Xn ");	value.Replace( "Irritant",  "Xi ");
		value.Replace( "Corrosive",		"C "); 
		value.Replace( "Explosive",		"E ");		value.Replace( "Oxidizing", "O ");
		value.Replace( "Very toxic",		"T+ ");	value.Replace( "Toxic",	 "T ");

		// Gefahrensymbole in die Liste schreiben (wenn nicht schon welche vorhanden sind!)
		tempdata.SetHarzards( value.ToStdString(), " " );
	}

	// 20.11.2011: Gefahren-Piktogramm (GHS)
	else if ( (key.compare("Gefahren-Piktogramm") == 0) || (key.compare("Hazard Pictogram(s)") == 0) )
	{
		// Tabs und soweiter entfernen
		value.Replace( "\t", "", true );
		value.Replace( "\n", "", true );

		// GHS-Symbole setzen
		tempdata.SetGHSSymbols( value.ToStdString(), " " );

	}

	// Molare Masse
	else if ( (key.compare("Molare Masse") == 0) || (key.compare("Molar mass") == 0) )
	{
		// Molare Masse speichern
		tempdata.SetMolWeight( value.ToStdString() );
	}

	// CAS-Nummer
	else if ( (key.compare("CAS-Nummer") == 0) || (key.compare("CAS number") == 0) )
	{
		// CAS-Nummer speichern
		tempdata.szCAS = value;
	}

	// Schmelzpunkt
	else if ( (key.compare("Schmelzpunkt") == 0) || (key.compare("Melting point") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Schmelzpunkt speichern
		tempdata.szMeltingPoint = value;
	}

	// Siedepunkt
	else if ( (key.compare("Siedepunkt") == 0) || (key.compare("Boiling point") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Siedepunkt speichern
		tempdata.szBoilingPoint = value;
	}

	// 20.11.2011: Flammpunkt
	else if ( (key.compare("Flammpunkt") == 0) || (key.compare("Flash point") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Flammpunkt zuweisen
		tempdata.szFlashPoint = value;
	}

	// Dichte
	else if ( (key.compare("Dichte") == 0) || (key.compare("Density") == 0) )
	{
		// Dichte speichern
		tempdata.SetDensity( value.ToStdString() );
	}

	// R-Satz
	else if ( (key.compare("R-Satz") == 0) || (key.compare("R Phrase") == 0) )
	{
		// Anfang entfernen
		value.Replace( "R ", "" );

		// Trimmen
		value.Trim( false );

		// Leerzeichen suchen
		wxString::size_type iBreak = value.find(" ");

		// Ab dem ersten Leerzeichen alles entfernen
		if ( iBreak != wxString::npos )
			value = value.substr(0, iBreak );

		tempdata.szRisks = value;
	}

	// S-Satz
	else if ( (key.compare("S-Satz") == 0) || (key.compare("S Phrase") == 0) )
	{
		// Anfang entfernen
		value.Replace( "S ", "" );

		// Trimmen
		value.Trim( false );

		// Leerzeichen suchen
		wxString::size_type iBreak = value.find(" ");

		// Ab dem ersten Leerzeichen alles entfernen
		if ( iBreak != wxString::npos )
			value = value.substr(0, iBreak );

		tempdata.szSafeties = value;
	}

	// 20.11.2011: H-Sätze
	else if ( (key.compare("Gefahrenhinweis(e)") == 0) || (key.compare("Hazard Statement(s)") == 0) )
	{
		// Liste erstellen
		std::vector< wxString > list;

		// Liste füllen
		BeTool::StringToList( value, ". H", list );

		// Liste durchgehen
		for ( unsigned int i = 0; i < list.size(); ++i )
		{
			// Doppelpunkt suchen
			wxString::size_type iBreak = list[i].find(":");

			// Ab dem ersten Doppelpunkt alles entfernen
			if ( iBreak != wxString::npos )
				list[i] = list[i].substr(0, iBreak );
		}

		// Liste wieder zusammensetzen
		value = BeTool::ListToString( list, "-" );

		// Leerzeichen und Ps wieder rausschmeißen
		value.Replace( "H", "", true );
		value.Replace( " ", "", true );

		// Zuweisen
		tempdata.szHarzardSatements = value;
	}

	// 20.11.2011: P-Sätze
	else if ( (key.compare("Sicherheitshinweis(e)") == 0) || (key.compare("Precautionary Statement(s)") == 0) )
	{
		// Liste erstellen
		std::vector< wxString > list;

		// Liste füllen
		BeTool::StringToList( value, ". P", list );

		// Liste durchgehen
		for ( unsigned int i = 0; i < list.size(); ++i )
		{
			// Doppelpunkt suchen
			wxString::size_type iBreak = list[i].find(":");

			// Ab dem ersten Doppelpunkt alles entfernen
			if ( iBreak != wxString::npos )
				list[i] = list[i].substr(0, iBreak );
		}

		// Liste wieder zusammensetzen
		value = BeTool::ListToString( list, "-" );

		// Leerzeichen und Ps wieder rausschmeißen
		value.Replace( "P", "", true );
		value.Replace( " ", "", true );

		// Zuweisen
		tempdata.szPrecautionaryStatements = value;
	}

	// 20.11.2011: Signalwort
	else if ( (key.compare("Signalwort") == 0) || (key.compare("Signal Word") == 0) )
	{
		// Signalwort zuweisen
		tempdata.szSignalWord = value;
	}

	// WGK
	else if ( key.compare("WGK") == 0 )
	{
		// WGK vor der Zahl entfernen...
		value.Replace("WGK ", "");

		// Wassergefährdung speichern
		tempdata.iWGK = atoi( value.c_str() );
	}
}
