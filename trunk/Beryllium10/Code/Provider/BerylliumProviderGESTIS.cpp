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

#include "stdafx.h"
#include "BerylliumProviderGESTIS.h"

#include "../betool.h"

#include <wx/sstream.h>
#include <wx/wfstream.h>


CBerylliumProviderGESTIS::CBerylliumProviderGESTIS(void)
{
}

CBerylliumProviderGESTIS::~CBerylliumProviderGESTIS(void)
{
}

// 1. Suchanfrage stellen
bool CBerylliumProviderGESTIS::SearchForCompound( const wxString searchtext, bool bExactMatch = false )
{
	// Ergebnisse löschen
	m_results.clear();

	// Daten erfolgreich gefunden (und geladen)
	bool bGotData = false;

	// Suchstring
	wxString searchurl = "";

	// Loggen
	wxLogMessage( _(L"ProviderGESTIS, %d: HTTP-Anfrage wird vorbereitet."), __LINE__ );

	// CAS-Nummer?
	if ( IsCAS( searchtext ) )
	{
		searchurl = "/nxt/gateway.dll?f=xhitlist&xhitlist_q=%5BFeld%20casnr%3A" + searchtext + "%5D&xhitlist_sel=title%3Bpath%3Bfield%3Acasnr&xhitlist_x=Advanced"; //&xhitlist_x=Advanced&xhitlist_s=&xhitlist_hc=&xhitlist_d=&xhitlist_mh=2000&xhitlist_vps=500&xhitlist_xsl=xhitlist.xsl&xhitlist_vpc=first&xhitlist_sel=title%3Bpath%3Bcontent-type%3Bhome-title%3Bitem-bookmark%3Bfield%3Acasnr%3B";
	}

	// Normaler Text
	else
	{
		// Kopieren
		wxString text = searchtext;

		if ( !bExactMatch )
			text = "*" + text + "*";

		searchurl = "/nxt/gateway.dll?f=xhitlist&xhitlist_q=%5BFeld%20acstoff%3A" + text + "%5D&xhitlist_sel=title%3Bpath%3Bfield%3Acasnr&xhitlist_x=Advanced"; //&xhitlist_x=Advanced";//&xhitlist_s=&xhitlist_hc=&xhitlist_d=&xhitlist_mh=2000&xhitlist_vps=500&xhitlist_xsl=xhitlist.xsl&xhitlist_vpc=first"; //&xhitlist_x=Advanced&xhitlist_s=&xhitlist_hc=&xhitlist_d=&xhitlist_mh=2000&xhitlist_vps=500&xhitlist_xsl=xhitlist.xsl&xhitlist_vpc=first&xhitlist_sel=title%3Bpath%3Bcontent-type%3Bhome-title%3Bitem-bookmark%3Bfield%3Acasnr%3B";
	}

	// HTTP-Anfrage vorbereiten
	// Wichtig hierbei ist das Cookie, damit Gestis weiß welches Sprache wir haben wollen
	m_http.SetHeader( "Content-type", "text/html; charset=utf-8" );
	
	if ( iLanguage == wxLANGUAGE_ENGLISH )
	{
		// Feld -> field
		searchurl.Replace("%5BFeld%20", "%5Bfield%20", true );

		// Cookie setzen
		m_http.SetHeader( "Cookie", "nxt/gateway.dll/vid=gestiseng%3Asdbeng;");
	}
	else 
		m_http.SetHeader( "Cookie", "nxt/gateway.dll/vid=gestisdeu%3Asdbdeu;");

	m_http.SetTimeout(60); // 60 Sekunden TimeOut

	// Host
	wxString gestishost = "gestis.itrust.de";

	// Anfrage an Gestis vorbereiten
	if ( iLanguage == wxLANGUAGE_ENGLISH )
		gestishost = "gestis-en.itrust.de";

	// Anfrage mit Proxy?
	if ( proxyhost.length() > 0 )
	{
		// Verbinden
		m_http.Connect( proxyhost, proxyport );

		// Puffer setzen
		m_http.SetPostBuffer( "Host: " + gestishost);

		// Adresse erweitern
		searchurl = "http://" + gestishost + searchurl;

		// Loggen
		wxLogMessage( _(L"ProviderGESTIS, %d: Verbinden über Proxy %s:%d."), __LINE__, proxyhost, proxyport );
	}

	// kein Proxy
	else
	{
		// Verbinden
		m_http.Connect( gestishost );

		// Loggen
		wxLogMessage( _(L"ProviderGESTIS, %d: Verbinden ohne Proxy."), __LINE__ );
	}

	// InputStream erzeugen
	wxInputStream *httpStream = m_http.GetInputStream( searchurl );

	// Stream ok?
	if (m_http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
#ifdef _DEBUG
		wxTempFileOutputStream dataStream("temp/gestis.temp");
		wxLogMessage( _(L"ProviderGESTIS, %d: Temporäre Datei 'temp/gestis.temp' anlegen."), __LINE__ );
#else
		wxTempFileOutputStream dataStream("beryllium10.temp");
		wxLogMessage( _(L"ProviderGESTIS, %d: Temporäre Datei 'beryllium10.temp' anlegen."), __LINE__ );
#endif

		// Daten lesen
		httpStream->Read(dataStream);

		// Stream schließen
		dataStream.Close();

		// Ergebnisse laden.
		wxLogMessage( _(L"ProviderGESTIS, %d: Suchergebnisse laden."), __LINE__ );
		bGotData = LoadSearchData();
	}

	// Fehler beim Verbinden?
	else
	{
		// Fehler ins Log schreiben
		wxLogError( _(L"ProviderGESTIS, %d: Fehler %d beim Verbinden."), __LINE__, m_http.GetError() );
	}


	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	m_http.Close();

	// Keine Daten gefunden?
	return bGotData;
}

// 2. Daten mit entsprechender ID holen
void CBerylliumProviderGESTIS::GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data )
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

			// Protokoll
			wxLogMessage( _(L"ProviderGESTIS, %d: Daten für '%s' werden geladen."), __LINE__, substancename );

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
void CBerylliumProviderGESTIS::GetResults( std::vector< result > &m_list ) const
{
	// Einfaches kopieren könnte schiefgehen
	for ( unsigned int i = 0; i < m_results.size(); ++i )
		m_list.push_back( m_results[i] );
}

// Lädt die Suchergebnisse
bool CBerylliumProviderGESTIS::LoadSearchData()
{
	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
#ifdef _DEBUG
	if ( !doc.Load( "temp/gestis.temp" ) )
	{
		wxLogError( _(L"ProviderGESTIS, %d: Temporäre Datei konnte nicht als XML-Datei geöffnet werden."), __LINE__ );
		return false;
	}
#else
	if ( !doc.Load( "beryllium10.temp" ) )
	{
		wxLogError( _(L"ProviderGESTIS, %d: Temporäre Datei konnte nicht als XML-Datei geöffnet werden."), __LINE__ );
		return false;
	}
#endif

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "list-section")
    {
		wxLogError( _(L"ProviderGESTIS, %d: XML-Wurzel 'list-section' konnte nicht gefunden werden."), __LINE__ );
        return false;
	}

	// Daten parsen
	wxLogMessage( _(L"ProviderGESTIS, %d: Daten werden eingelesen."), __LINE__ );

	// Wurzel Ast für Ast durchgehen
    for ( wxXmlNode *node = doc.GetRoot()->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
			continue;

		// Wir suchen Elemente mit Namen "item"
		if ( node->GetName().compare("item") == 0 )
		{
			wxLogMessage( _(L"ProviderGESTIS, %d: Suchergebniss gefunden."), __LINE__ );

			LoadSearchDataItem( node );

			// Wir sammeln höchstens 15 Einträge
			if ( m_results.size() > 14 )
				return true;
		}
	}

	// Wenn wir Daten übernommen haben, dann waren wir erfolgreich
	return (m_results.size() > 0);
}

// Lädt ein Element der Suchergebnisse
void CBerylliumProviderGESTIS::LoadSearchDataItem( wxXmlNode *cell )
{
	// Temporäres Element anlegen
	IProvider::result temp;

	// Seriennummer geben
	temp.serial = m_results.size();

	// Alle Subelemente suchen
	for( wxXmlNode* element = cell->GetChildren(); element; element = element->GetNext() )
	{
		// Text holen
		wxString content; GetAllTextFromNode( element, content );

		// Leer?
		if ( (element->GetName().length() == 0) || (content.length() == 0) )
			continue;

		// Titel gefunden?
		if ( element->GetName().compare("title") == 0 )
			temp.name = content;

		// Pfad gefunden?
		else if ( element->GetName().compare("path") == 0 )
		{
			temp.data = "http://gestis.itrust.de/nxt/gateway.dll/" + content + "?f=templates$fn=document-frame.htm$3.0$GLOBAL=G_&G_DIEXSL=gestis.xsl$q=$uq=$x=$up=1";

			// Macht eigentlich keinen Unterschied für GESTIS, aber der Form halber...
			if ( iLanguage == wxLANGUAGE_ENGLISH )
				temp.data.Replace("gestis.itrust.de","gestis-en.itrust.de", true);
		}

		// CAS-Nummer gefunden?
		else if ( (element->GetName().compare("field") == 0)
			&& (element->GetAttribute("name").compare("casnr") == 0) )
			temp.info = "[" + content + "]";

	}

	// Element "vollständig" gefunden? Dann in Liste eintragen!
	if ( (temp.name.size() > 0) && (temp.data.size() > 0) )
	{
		wxLogMessage( _(L"ProviderGESTIS, %d: Substanz '%s' hinzugefügt."), __LINE__, temp.name );
		m_results.push_back( temp );
	}
}

// Lädt die Daten für die Substanz unter URL
void CBerylliumProviderGESTIS::LoadDataFromUrl( wxString URL )
{
	// URL modifizieren (wir wollen die XML-Variante!)	
	URL.Replace( "gestis.xsl", "gestis.xml", true );

	// Wichtig hierbei ist das Cookie, damit Gestis weiß welches Sprache wir haben wollen
	m_http.SetHeader( "Content-type", "text/html; charset=utf-8" );
	wxLogMessage( _(L"ProviderGESTIS, %d: HTTP-Anfrage wird vorbereitet."), __LINE__ );

	if ( iLanguage == wxLANGUAGE_ENGLISH )
		m_http.SetHeader( "Cookie", "nxt/gateway.dll/vid=gestiseng%3Asdbeng;");
	else 
		m_http.SetHeader( "Cookie", "nxt/gateway.dll/vid=gestisdeu%3Asdbdeu;");

	m_http.SetTimeout(60); // 60 Sekunden TimeOut

	// Anfrage an Gestis vorbereiten
	wxString gestishost = "gestis.itrust.de";

	if ( iLanguage == wxLANGUAGE_ENGLISH )
		gestishost = "gestis-en.itrust.de";

	// Anfrage mit Proxy?
	if ( proxyhost.length() > 0 )
	{
		// Verbinden
		m_http.Connect( proxyhost, proxyport );

		// Puffer setzen
		m_http.SetPostBuffer( "Host: " + gestishost);

		// Loggen
		wxLogMessage( _(L"ProviderGESTIS, %d: Verbinden über Proxy %s:%d."), __LINE__, proxyhost, proxyport );
	}

	// kein Proxy
	else
	{
		// URL modifizieren
		URL.Replace( "http://gestis.itrust.de", "", true );
		URL.Replace( "http://gestis-en.itrust.de", "", true );

		// Verbinden
		m_http.Connect( gestishost );

		// Loggen
		wxLogMessage( _(L"ProviderGESTIS, %d: Verbinden ohne Proxy."), __LINE__ );
	}	

	// InputStream erzeugen
	wxInputStream *httpStream = m_http.GetInputStream( URL );

	// Stream ok?
	if (m_http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
#ifdef _DEBUG
		wxTempFileOutputStream dataStream("temp/gestis.temp");
		wxLogMessage( _(L"ProviderGESTIS, %d: Temporäre Datei 'temp/gestis.temp' anlegen."), __LINE__ );
#else
		wxTempFileOutputStream dataStream("beryllium10.temp");
		wxLogMessage( _(L"ProviderGESTIS, %d: Temporäre Datei 'beryllium10.temp' anlegen."), __LINE__ );
#endif

		// Daten lesen
		httpStream->Read(dataStream);

		// Stream schließen
		dataStream.Close();

		// Ergebnisse laden.
		wxLogMessage( _(L"ProviderGESTIS, %d: Suchergebnisse laden."), __LINE__ );
		LoadSubstanceData();
	}

	// Fehler beim Verbinden?
	else
	{
		// Fehler ins Log schreiben
		wxLogError( _(L"ProviderGESTIS, %d: Fehler %d beim Verbinden."), __LINE__, m_http.GetError() );
	}

	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	m_http.Close();

	// Zurück
	return;
}

// Lädt die Daten der Substanz
void CBerylliumProviderGESTIS::LoadSubstanceData()
{
	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
#ifdef _DEBUG
	if ( !doc.Load( "temp/gestis.temp" ) )
	{
		wxLogError( _(L"ProviderGESTIS, %d: Temporäre Datei konnte nicht als XML-Datei geöffnet werden."), __LINE__ );
		return;
	}
#else
	if ( !doc.Load( "beryllium10.temp" ) )
	{
		wxLogError( _(L"ProviderGESTIS, %d: Temporäre Datei konnte nicht als XML-Datei geöffnet werden."), __LINE__ );
		return;
	}
#endif

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "stoffreport")
    {
		wxLogError( _(L"ProviderGESTIS, %d: XML-Wurzel 'stoffreport' konnte nicht gefunden werden."), __LINE__ );
        return;
	}

	// Daten parsen
	wxLogMessage( _(L"ProviderGESTIS, %d: Daten werden eingelesen."), __LINE__ );

	// Namen der Substanz löschen
	tempdata.szNames.clear();

	// Alle Subelemente suchen
	for( wxXmlNode* node = doc.GetRoot()->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
			continue;

		// Kopfdaten
		if ( node->GetName().compare("kopfdaten") == 0 )
		{
			wxLogMessage( _(L"ProviderGESTIS, %d: Lade Kopfdaten..."), __LINE__ );
			LoadSubstanceDataKopfdaten( node );
		}

		// Hauptkapitel!
		else if ( node->GetName().compare("hauptkapitel") == 0 )
		{
			wxLogMessage( _(L"ProviderGESTIS, %d: Lade Substanzdaten..."), __LINE__ );
			LoadSubstanceDataBlock( node );
		}
	}

	// Keine Namen? Dann wenigstens den einen setzen!
	if ( tempdata.szNames.size() == 0 )
		tempdata.ReplaceNamesBy( substancename );

	// Zurück
	return;
}

// Lädt den Kopfdatenblock der Substanzdaten
void CBerylliumProviderGESTIS::LoadSubstanceDataKopfdaten( wxXmlNode *parent )
{
	// Alle Subelemente der Kopfdaten suchen
	for( wxXmlNode* node = parent->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
			continue;

		// Name des Knoten
		wxString name = node->GetName();

		// Bevorzugter Name
		if ( name.compare("bevorzugtername") == 0 )
		{
			// Content
			wxString content; GetAllTextFromNode( node, content );

			tempdata.szNames.push_back( content );

			wxLogMessage( _(L"ProviderGESTIS, %d: Name: %s"), __LINE__, content );
		}

		// Aliasnamen
		else if ( name.compare("aliasnamen") == 0 )
		{
			// alle Aliaselemente suchen
			for( wxXmlNode* alias = node->GetChildren(); alias; alias = alias->GetNext() )
			{
				// Nur Elemente mit Namen "aliasname" sammeln
				if ( alias->GetName().compare("aliasname") != 0 )
					continue;

				// Element laden
				LoadSubstanceDataAliasname( alias );
			}
		}
	}
}

// Lädt einen "Aliasname"-Block aus den Kopfdaten
void CBerylliumProviderGESTIS::LoadSubstanceDataAliasname( wxXmlNode *parent )
{
	// Alle Subelemente suchen
	for( wxXmlNode* node = parent->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
			continue;

		// Inhalt holen
		wxString content; GetAllTextFromNode( node, content );

		// Nur Elemente mit Namen "stoffname" sammeln
		if ( node->GetName().compare("stoffname") == 0 )
		{
			tempdata.szNames.push_back( content );

			wxLogMessage( _(L"ProviderGESTIS, %d: Alias: %s"), __LINE__, content );
		}
	}
}

// Lädt einen Block der Substanzdaten
void CBerylliumProviderGESTIS::LoadSubstanceDataBlock( wxXmlNode *parent, wxString topic )
{
	// Überschrift
	wxString ueberschrift = topic;

	// Alle Subelemente suchen
	for( wxXmlNode* node = parent->GetChildren(); node; node = node->GetNext() )
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
			continue;

		// Name
		wxString name = node->GetName();

		// Inhalt, wird aber nur bei bestimmten Elementen gefüllt!
		wxString content;

		// Überschrift setzen
		if ( name.compare("ueberschrift") == 0 )
		{
			GetAllTextFromNode( node, content );
			ueberschrift = content;

			wxLogMessage( _(L"ProviderGESTIS, %d: Neue Überschrift '%s' gefunden."), __LINE__, content );
		}

		// CAS-Nr
		else if ( name.compare("casnr") == 0 )
		{
			GetAllTextFromNode( node, content );
			tempdata.szCAS = content;

			wxLogMessage( _(L"ProviderGESTIS, %d: CAS-Nummer %s gefunden."), __LINE__, content );
		}

		// Summenformel
		else if ( name.compare("summenformel") == 0 )
		{
			GetAllTextFromNode( node, content );
			tempdata.SetFormula( content );

			wxLogMessage( _(L"ProviderGESTIS, %d: Summenformel %s gefunden."), __LINE__, content );
		}

		// Bilder (z.B. GHS)
		else if ( name.compare("img") == 0 )
		{
			// Attribut speichern
			wxString imgsrc = node->GetAttribute("src");

			// GHS-Bild?
			if ( imgsrc.StartsWith("ghs") )
			{
				// Die Nummer "extrahieren"
				imgsrc.Replace( "ghs", "", true ); imgsrc.Replace( ".gif", "", true );

				// Einfügen, falls nicht schon vorhanden
				if ( !tempdata.HasGHSSymbol(imgsrc) )
				{
                    tempdata.szGHSSymbols.push_back( imgsrc );

					wxLogMessage( _(L"ProviderGESTIS, %d: GHS-Piktogramm %s gefunden."), __LINE__, imgsrc );
				}
			}
		}

		// umsetztabelle = Gefahrensymbole, Hinweise, etc
		else if ( (name.compare("table") == 0)
			&& (node->GetAttribute("class").compare("umsetztabelle") == 0)
			&& ((ueberschrift.compare("ALTE EINSTUFUNG NACH STOFFRICHTLINIE") == 0)
			|| (ueberschrift.compare("OLD CLASSIFICATION")== 0)) )
		{
			// Jetzt suchen wir nach "verstecktercode"
			for( wxXmlNode* table = node->GetChildren(); table; table = table->GetNext() )
			{
				// Nur Elemente "tr" sammeln
				if ( table->GetName().compare("verstecktercode") != 0 )
					continue;

				// Inhalt
				wxString verstecktercode; GetAllTextFromNode( table, verstecktercode );

				// R-Sätze
				if ( verstecktercode.StartsWith("R") )
				{
					verstecktercode.Replace("R", "", true);

					if ( tempdata.szRisks.length() > 0 )
						tempdata.szRisks += "-";

					// Leerzeichen mit "/" ersetzen.
					verstecktercode.Replace(" ", "/", true);

					// Hinzufügen
					tempdata.szRisks += verstecktercode;

					wxLogMessage( _(L"ProviderGESTIS, %d: 'R-Sätze' gefunden."), __LINE__ );
				}

				// Gefahrensymbole
				else
				{
					tempdata.szHarzards.push_back( verstecktercode );

					wxLogMessage( _(L"ProviderGESTIS, %d: -> Gefahrensymbole (alt) gefunden."), __LINE__ );
				}
			}
		}

		// MAK-Werte
		else if ( (name.compare("unterkapitel") == 0)
			&& (node->GetAttribute("drnr").compare("1203") == 0) )
		{
			// Block-Tabellen-Nummer
			int numberofblock = 0;

			// MAK-Wert gesetzt?
			bool bMAKset = false;

			// Subelemente Tabelle
			for( wxXmlNode* table = node->GetChildren(); table; table = table->GetNext() )
			{
				// Nur Elemente "table" mit class="block" sammeln
				if ( (table->GetName().compare("table") != 0) || (table->GetAttribute("class").compare("block") != 0) )
					continue;

				// Hochzählen
				numberofblock++;

				// Der zweite Block muss es sein, jetzt jede Zeile suchen
				if ( numberofblock != 2 )
					continue;

				for( wxXmlNode* tr = table->GetChildren(); tr; tr = tr->GetNext() )
				{
					// Nur Elemente "tr" sammeln
					if ( tr->GetName().compare("tr") != 0 )
						continue;

					// Inhalt holen
					GetAllTextFromNode( tr, content );

					// Enthält eine Einheit, die wir kennen? Dann ists hoffentlich ein MAK-Wert!
					if ( (content.Find("ml/m") > -1) || (content.Find("mg/m") > -1) )
					{
						// ml/m³ = ppm
						tempdata.bMAKppm = ( content.find("ml/m") != content.npos );

						// MAK speichern
						tempdata.SetMAK( content );

						bMAKset = true;

						wxLogMessage( _(L"ProviderGESTIS, %d: MAK-Wert %s gefunden."), __LINE__, content );
					}

					if ( bMAKset )
						break;
				}

				if ( bMAKset )
					break;
			}
		}

		// Bestimmte Zellen enthalten Informationen... z.B. Wassergefährdungsklasse
		else if ( (name.compare("td") == 0) && (node->GetAttribute("class").length() == 0) )
		{
			// Inhalt
			GetAllTextFromNode( node, content );

			// WGK
			if ( content.StartsWith("WGK") )
			{
				// Holen wir uns die Klasse
				content = content.SubString(3,5);

				// Trimmen
				content.Trim( false ); content.Trim( true );

				// WGK speichern
				tempdata.iWGK = atoi( content.c_str() );

				wxLogMessage( _(L"ProviderGESTIS, %d: WGK %d gefunden."), __LINE__, tempdata.iWGK );
			}
		}

		// Für die Tuppel suchen wir eine spezielle Tabelle
		// mit Klasse "feldmitlabel"
		else if ( (name.compare("table") == 0) && (node->GetAttribute("class").compare("feldmitlabel") == 0) )
		{
			// Subelemente der Tabelle ("tr")
			for( wxXmlNode* table = node->GetChildren(); table; table = table->GetNext() )
			{
				// Nur Elemente "tr" sammeln
				if ( table->GetName().compare("tr") != 0 )
					continue;

				// Tupel anlegen
				wxString tupelname, tupelcontent;

				// Ersters TD?
				bool firsttd = true;

				// Subelemente der Zelle ("td")
				for( wxXmlNode* tr = table->GetChildren(); tr; tr = tr->GetNext() )
				{
					// Nur Elemente "td" sammeln
					if ( tr->GetName().compare("td") != 0 )
						continue;

					// Content laden
					wxString tdcontent; GetAllTextFromNode(tr, tdcontent);

					// erstes "td"
					if ( firsttd )
					{
						tupelname = tdcontent;
						firsttd = false;
					}

					// zweites "td"
					else
						tupelcontent = tdcontent;

				}

				// Tupel gefunden?
				if ( (tupelname.length() > 0) && (tupelcontent.length() > 0) )
					LoadSubstanceDataTupel( tupelname, tupelcontent, ueberschrift );
			}
		}

		// Tabelle mit "block"
		else if ( (name.compare("table") == 0) && (node->GetAttribute("class").compare("block") == 0) )
		{
			// Tupel anlegen
			wxString tupelname, tupelcontent;

			// Ersters tr?
			int numberTR = 0;

			// Subelemente der Tabelle ("tr")
			for( wxXmlNode* table = node->GetChildren(); table; table = table->GetNext() )
			{
				// Nur Elemente "tr" sammeln
				if ( table->GetName().compare("tr") != 0 )
					continue;

				// Subelemente der Zeile ("td")
				for( wxXmlNode* tr = table->GetChildren(); tr; tr = tr->GetNext() )
				{
					// Nur Elemente "td" sammeln
					if ( tr->GetName().compare("td") != 0 )
						continue;

					// Content laden
					wxString tdcontent; GetAllTextFromNode(tr, tdcontent);

					numberTR++;

					// erstes "tr"
					if ( numberTR == 1 )
						tupelname = tdcontent;

					// zweites "tr"
					else if ( numberTR == 2 )
						tupelcontent = tdcontent;
				}
			}

			// Tupel gefunden? (nur wenn numberTR == 2 )
			if ( (numberTR == 2) && (tupelname.length() > 0) && (tupelcontent.length() > 0) )
				LoadSubstanceDataTupel( tupelname, tupelcontent, ueberschrift );

		}

		// Rekursion! Hooray!
		LoadSubstanceDataBlock( node, ueberschrift );
	}
}


// Lädt ein Schlüssel/Werte-Paar
void CBerylliumProviderGESTIS::LoadSubstanceDataTupel( wxString key, wxString value, wxString topic )
{
	// Funktion übersetzt die Daten in Abhängigkeit vom Schlüssel
	key.Trim( false ); key.Trim( true );
	value.Trim( false ); value.Trim( true );
	topic.Trim( false ); topic.Trim( true );

	wxLogMessage( _(L"ProviderGESTIS, %d: Tripel (%s,%s,%s) gefunden."), __LINE__, topic, key, value );

	// Molare Masse
	if ( (key.compare("Molmasse:") == 0) || (key.compare("Molar mass:") == 0) )
	{
		// Molare Masse speichern
		tempdata.SetMolWeight( value.ToStdString() );

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Molare Masse' interpretiert."), __LINE__ );
	}

	// Schmelzpunkt
	else if ( (key.compare("Schmelzpunkt:") == 0) || (key.compare("Melting point:") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Schmelzpunkt speichern
		tempdata.szMeltingPoint = value;

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Schmelzpunkt' interpretiert."), __LINE__ );
	}

	// Siedepunkt
	else if ( (key.compare("Siedepunkt:") == 0) || (key.compare("Boiling Point:") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Siedepunkt speichern
		tempdata.szBoilingPoint = value;

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Siedepunkt' interpretiert."), __LINE__ );
	}

	// Dichte
	else if ( ((topic.compare("DICHTE") == 0) && (key.compare("Wert:") == 0)) || ((topic.compare("DENSITY") == 0) && (key.compare("Value:") == 0)) )
	{
		// Dichte speichern (falls nicht schon ein Wert vorhanden ist)
		if ( tempdata.dbDensity == 1.0f )
		{
			tempdata.SetDensity( value );

			wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Dichte' interpretiert."), __LINE__ );
		}
	}

	// Flammpunkt
	else if ( (key.compare("Flammpunkt:") == 0) || (key.compare("Flash point:") == 0) )
	{
		// Evtl. Klammerzusätze entfernen
		RemoveBlockFromString( value, "(", ")" );

		// Flammpunkt zuweisen
		tempdata.szFlashPoint = value;

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Flammpunkt' interpretiert."), __LINE__ );
	}

	// GHS: Signalwort
	else if ( (key.compare("Signalwort:") == 0) || (key.compare("Signal Word:") == 0) )
	{
		// Anführungszeichen entfernen
		value.Replace( "\"", "", true );

		// Signalwort speichern
		tempdata.szSignalWord = value.ToStdString();

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'Signalwort (GHS)' interpretiert."), __LINE__ );
	}

	// Gefahrenhinweise
	else if ( key.StartsWith("Gefahrenhinweise") || key.StartsWith("Hazard Statement") )
	{
		// Liste erstellen
		std::vector< wxString > list;

		// Liste füllen
		BeTool::StringToList( value, ".H", list );

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

		// Leerzeichen und Hs wieder rausschmeißen
		value.Replace( "H", "", true );
		value.Replace( " ", "", true );

		// Zuweisen
		tempdata.szHarzardSatements = value;

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'H-Sätze' interpretiert."), __LINE__ );
	}

	// EUH-Sätze
	else if ( key.StartsWith("ErgÃ¤nzende Gefahrenhinweise") || key.StartsWith("Supplemental Hazard Statement") )
	{
		// Liste erstellen
		std::vector< wxString > list;

		// Liste füllen
		BeTool::StringToList( value, ".EUH", list );

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

		// Leerzeichen und EuHs wieder rausschmeißen
		value.Replace( "EUH", "", true );
		value.Replace( " ", "", true );

		// Zuweisen
		tempdata.szEuropeanHarzardSatements = value;

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'EUH-Sätze' interpretiert."), __LINE__ );
	}

	// Sicherheitshinweise
	else if ( key.StartsWith("Sicherheitshinweise") || key.StartsWith("Precautionary Statement") )
	{
		// Liste erstellen
		std::vector< wxString > list;

		// Liste füllen
		BeTool::StringToList( value, ".P", list );

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

		wxLogMessage( _(L"ProviderGESTIS, %d: -> als 'P-Sätze' interpretiert."), __LINE__ );
	}



}
