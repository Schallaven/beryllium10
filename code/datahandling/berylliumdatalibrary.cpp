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

#include "../beryllium.h"

#include "berylliumdatalibrary.h"

CBerylliumDataLibrary::CBerylliumDataLibrary(void)
{
}

CBerylliumDataLibrary::~CBerylliumDataLibrary(void)
{
}

// Operator zum schnellen Zugriff auf Objekte
CBerylliumSubstanceData const& CBerylliumDataLibrary::operator [](int iIndex)
{
	return m_vData[iIndex];
}

// Operator zum schnellen Zugriff auf Objekte
CBerylliumSubstanceData const& CBerylliumDataLibrary::GetItem( int iIndex )
{
	return m_vData[iIndex];
}

// Gibt die Anzahl der Element zurück
int CBerylliumDataLibrary::NumberOfSubstances()
{
	return m_vData.size();
}

// Löscht alle Elemente
void CBerylliumDataLibrary::Clear()
{
	m_vData.clear();
}

// Element hinzufügen
void CBerylliumDataLibrary::Add( const CBerylliumSubstanceData &add )
{
	// Falls es keine CAS-Nummer gibt für die neue Substanz, dann einfach einfügen
	if ( add.szCAS.length() == 0 )
	{
		m_vData.push_back( add );

		return;
	}

	// Zunächst wird überprüft, ob es diese Substanz nicht schon gibt. Wenn ja, dann
	// löschen wir diese zuerst. Es werden NUR CAS-Nummern überprüft
	for ( unsigned int i = 0; i < m_vData.size(); ++i )
	{
		if ( add.szCAS.compare( m_vData[i].szCAS ) == 0 )
		{
			// Löschen
			m_vData.erase( m_vData.begin() + i );

			// Raus hier.
			break;
		}
	}

	// Die neue Substanz anfügen
	m_vData.push_back( add );
}

// Sortiert die Element nach Namen (GetName())
void CBerylliumDataLibrary::SortByNames()
{
	// Sortieren...
	std::sort(m_vData.begin(), m_vData.end(), CBerylliumDataLibrary::StaticSortByNames);
}

// Sucht ein Element anhand der CAS-Nummer
int CBerylliumDataLibrary::LookForCAS( const wxString cas )
{
	// Elemente durchgehen
	for ( unsigned int i = 0; i < m_vData.size(); ++i )
	{
		// Element gefunden?
		if ( m_vData[i].szCAS.compare(cas) == 0 )
			return i;
	}

	// Nichts gefunden :(
	return -1;
}

void CBerylliumDataLibrary::LoadFromXML( wxXmlNode *root )
{
	for( wxXmlNode* element = root->GetChildren(); element; element = element->GetNext() )
	{
		// Nur "substance" Knoten finden
		if ( element->GetName().compare("substance") != 0 )
			continue;

		// Temporäres Objekt anlegen
		CBerylliumSubstanceData temp;

		// Substanz auslesen
		temp.LoadFromXmlNode(element);

		// Substanz hinzufügen
		m_vData.push_back( temp );
	}
}

void CBerylliumDataLibrary::SaveToXml( wxXmlNode *root )
{
	// Letzter eingefügter Knoten
	wxXmlNode *lastNode = NULL;

	// Nun für jede Substanz einen Block hinzufügen
	for ( unsigned int i = 0; i < m_vData.size(); ++i )
	{
		// Knoten erstellen
		wxXmlNode *xmlSubstance = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "substance" );
		root->InsertChildAfter( xmlSubstance, lastNode );

		// Daten anhängen (allerdings ohne Ansatzgröße)
		m_vData[i].SaveToXmlNode( xmlSubstance );

		// Letzten Knoten setzen
		lastNode = xmlSubstance;
	}
}

void CBerylliumDataLibrary::LoadFromFile( const wxString sFileName )
{
	// Datei laden
	wxXmlDocument file;

	// Datei konnte nicht geladen werden? Raus hier!
	if ( !file.Load( sFileName ) )
		return;

	// Wurzel finden
    if (file.GetRoot()->GetName() != "library")
        return;

	// Erstmal alle alten Daten löschen
	Clear();

	// Alle Substanzen laden
	LoadFromXML( file.GetRoot() );
}

void CBerylliumDataLibrary::SaveToFile( const wxString sFileName )
{
	// Datei speichern
	wxXmlDocument file;

	// Version setzen
	file.SetVersion("1.0");

	// Encoding setzen, seit 1.12.2013: UTF-8
	file.SetFileEncoding("UTF-8");

	// Wurzel erstellen
	wxXmlNode *xmlRoot = new wxXmlNode( wxXML_ELEMENT_NODE , "library" );

	// Wurzel setzen
	file.SetRoot( xmlRoot );

	// Substanzen hinzufügen
	SaveToXml( xmlRoot );

	// Datei speichern (Einrückung von 2 Zeichen pro Zeile)
	file.Save( sFileName, 2 );
}
