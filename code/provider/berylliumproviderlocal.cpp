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
#include "berylliumproviderlocal.h"

#include "../berylliumapplication.h"

#include <wx/filename.h>

CBerylliumProviderLocal::CBerylliumProviderLocal(void)
{
}

CBerylliumProviderLocal::~CBerylliumProviderLocal(void)
{
}

// 1. Suchanfrage stellen
bool CBerylliumProviderLocal::SearchForCompound( const wxString searchtext, bool bExactMatch = false )
{
	// Loggen
	wxLogMessage( _(L"ProviderLocal, %d: Lade Bibliothek."), __LINE__ );

	// Ergebnisse löschen
	m_results.clear();

	// Bibliothek-Datei holen
	libraryfile = ::wxGetApp().GetConfigData( "localdatabase" );

	// Keine Datei definiert? Dann gibt es hier nichts zu tun.
	if ( libraryfile.length() == 0 )
	{
		wxLogError( _(L"ProviderLocal, %d: Keine Bibliotheksdatei gewählt."), __LINE__ );
		return false;
	}

	// Ersetzen potentielle "\" durch "/", damit unter Linux (wo ein "\" durchaus ein legitimies Zeichen ist)
	// nicht die falsche Datei geladen wird. Windows akzeptiert sowohl "\" als auch "/".
	libraryfile.Replace( "\\", "/", true );

    // Existiert die Datei gar nicht? Dann auch raus hier!
    wxFileName libfile( libraryfile );

    if ( !libfile.FileExists() )
	{
		wxLogError( _(L"ProviderLocal, %d: Datei '%s' nicht gefunden."), __LINE__, libraryfile );
        return false;
	}

	// Loggen
	wxLogMessage( _(L"ProviderLocal, %d: Lade '%s'."), __LINE__, libraryfile );

	// Datei laden
	library.LoadFromFile( libraryfile.ToStdString() );

	// Suchstring speichern
	wxString searchname = searchtext;

	// Anzahl der Daten
	int iCount = library.NumberOfSubstances();

	// Anzahl der gefundenen Ergebnisse!
	int n = 0;

	// Loggen
	if ( IsCAS( searchname ) )
		wxLogMessage( _(L"ProviderLocal, %d: Suche nach CAS %s..."), __LINE__, searchname );
	else
		wxLogMessage( _(L"ProviderLocal, %d: Suche nach Übereinstimmung mit '%s'..."), __LINE__, searchname );

	// Durchgehen und in die Ergebnisse aufnehmen (maximal 15)
	for ( int i = 0; i < iCount; ++i )
	{
		// Temporäres Element anlegen
		IProvider::result temp;

		// Attribut speichern
		temp.name	= library[i].GetName();
		temp.data	= wxString::Format( "file:///%s", libraryfile );;
		temp.serial = i;
		temp.info	= (library[i].szCAS.length() > 0) ? "[ " + library[i].szCAS + " ]" : "";

		// "Quelle" anfügen, falls verfügbar
		if ( library[i].source.provider.length() > 0 )
			temp.info += ", " + library[i].source.provider + " (" + library[i].source.fetched + ")";

		// CAS-Nummer gefunden
		if ( IsCAS( searchname ) && (library[i].szCAS.compare(searchname)==0) )
		{
			// Loggen
			wxLogMessage( _(L"ProviderLocal, %d: Substanz '%s' gefunden."), __LINE__, temp.name );

			m_results.push_back(temp);
			n++;
		}

		// Ansonsten Namen suchen
		else if ( library[i].HasName(searchname.ToStdString(), bExactMatch) )
		{
			// Loggen
			wxLogMessage( _(L"ProviderLocal, %d: Substanz '%s' gefunden."), __LINE__, temp.name );

			m_results.push_back(temp);
			n++;
		}

		// Nach 15 ist Schluss!
		if ( n == 15 )
			break;
	}

	// Nichts gefunden?
	if ( m_results.size() == 0 )
		wxLogMessage( _(L"ProviderLocal, %d: Keine Substanzen in Bibliothek gefunden."), __LINE__ );

	// Daten gefunden?
	return (m_results.size() > 0);
}

// 2. Daten mit entsprechender ID holen
void CBerylliumProviderLocal::GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data )
{
	// Loggen
	wxLogMessage( _(L"ProviderLocal, %d: Hole Daten aus Bibliothek '%s'..."), __LINE__, libraryfile );

	// Daten
	CBerylliumSubstanceData tempdata;

	// ID wird einfach aus der Bibliothek geladen
	if ( (iSerial > -1) && (iSerial < library.NumberOfSubstances()) )
	{
		tempdata = library[iSerial];
	}
	else
	{
		wxLogError( _(L"ProviderLocal, %d: Daten nicht gefunden."), __LINE__ );
		return;
	}

	// Quelle anfügen (falls noch nicht vorhanden!)
	if ( tempdata.source.provider.length() == 0 )
	{
		tempdata.source.provider	= GetName();
		tempdata.source.url			= wxString::Format( "file:///%s", libraryfile );
		tempdata.source.fetched		= wxDateTime::Now().Format("%d. %B %Y %H:%M");
	}

	// Daten kopieren
	m_data = tempdata;

	// Loggen
	wxLogMessage( _(L"ProviderLocal, %d: Daten für '%s' aus Bibliothek kopiert."), __LINE__, tempdata.GetName() );
}

// Get: Liste mit Substanzen holen
void CBerylliumProviderLocal::GetResults( std::vector< result > &m_list ) const
{
	// Einfaches kopieren könnte schiefgehen
	for ( unsigned int i = 0; i < m_results.size(); ++i )
		m_list.push_back( m_results[i] );
}
