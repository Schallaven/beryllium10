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
#include "berylliumstatementsmanager.h"

CBerylliumStatementsManager::CBerylliumStatementsManager(void)
{
}

CBerylliumStatementsManager::~CBerylliumStatementsManager(void)
{
}

// Statement hinzufügen (falls noch nicht vorhanden)
void CBerylliumStatementsManager::AddStatement( const wxString token, const wxString statement, const wxString group, const unsigned int iDanger )
{
	// Ist schon ein Objekt mit diesem Token vorhanden?
	// Dann nichts tun.
	if ( HasToken( token ) )
		return;

	// Temporäres Objekt
	CBerylliumStatement temp( token, statement, group );

	// Gefährlichkeit
	temp.SetDangerPoints( iDanger );

	// Hinzufügen
	m_statements.push_back( temp );
}

// Statement hinzufügen (falls vorhanden, dann wird statement einfach hinten am vorhandenen angefügt)
void CBerylliumStatementsManager::AddStatementToToken( const wxString token, const wxString statement, const wxString group )
{
	// Token suchen
	for ( unsigned int i = 0; i < m_statements.size(); i++ )
	{
		// Gefunden (Gruppe und Token müssen übereinstimmen!)
		if ( (m_statements[i].GetToken().compare( token ) == 0) &&
			(m_statements[i].GetGroup().compare( group ) == 0) )
		{
			// Statement erweitern
			m_statements[i].AddToStatement( statement );

			// Zurück
			return;
		}
	}

	// Nichts gefunden, also ganz normal hinzufügen
	AddStatement( token, statement, group );

	// Zurück
	return;
}

// Statement löschen
void CBerylliumStatementsManager::DeleteStatement( const wxString token )
{
	// Durchsuchen
	for ( unsigned int i = 0; i < m_statements.size(); i++ )
	{
		// Gefunden
		if ( m_statements[i].GetToken().compare( token ) == 0 )
		{
			// Löschen
			m_statements.erase( m_statements.begin() + i );

			// Zurück
			return;
		}
	}

	// Nichts gefunden
	return;
}

// Alle Statements löschen
void CBerylliumStatementsManager::DeleteAllStatements()
{
	// Alle Elemente löschen
	m_statements.clear();
}

// Ordnet die Statements (BubbleSort)
// Evtl. mal gegen ein anderes Verfahren austauschen
void CBerylliumStatementsManager::Sort()
{
	// Anzahl der Elemente
	unsigned int n = m_statements.size();

	// Mindestens zwei Elemente sollten es sein, sonst
	// hat das Sortieren wenig Sinn.
	if ( n < 2 )
		return;

	// der letzte Index ist n-1
	n--;

	// Wurde ein Element vertauscht?
	bool swapped = false;

	// Schleife
	do
	{
		// Nichts wurde vertauscht
		swapped = false;

		// Elemente durchgehen und vergleichen
		for ( unsigned int i = 0; i < n; i++ )
		{
			// Elemente nach Zahlen umwandeln
			int iFirst	= atoi( m_statements[i].GetToken().c_str() );
			int iSecond = atoi( m_statements[i+1].GetToken().c_str() );

			// Element i größer als Element i+1?
			if ( iFirst > iSecond )
			{
				// Template
				CBerylliumStatement temp = m_statements[i+1];

				// Vertauschen
				m_statements[i+1] = m_statements[i];
				m_statements[i]	  = temp;

				// Es wurde vertauscht!
				swapped = true;
			}
		}

		// Anzahl der Elemente verkleinern (die Blase kleiner machen)
		n--;

	} while ( swapped );
}

// Statement holen
wxString CBerylliumStatementsManager::operator[] ( wxString token ) const
{
	// Durchsuchen
	for ( unsigned int i = 0; i < m_statements.size(); i++ )
		if ( m_statements[i].GetToken().compare( token ) == 0 )
			return m_statements[i].GetStatement();

	// Nichts gefunden
	return wxT("!Unbekannt!");
}

// Token vorhanden (genau)
bool CBerylliumStatementsManager::HasToken( const wxString token ) const
{
	// Durchsuchen
	for ( unsigned int i = 0; i < m_statements.size(); i++ )
		if ( m_statements[i].GetToken().compare( token ) == 0 )
			return true;

	// Nichts gefunden
	return false;
}

// Token vorhanden (inkludiert)
bool CBerylliumStatementsManager::HasTokenPart( const wxString token, const wxString separator ) const
{
	// Durchsuchen
	for ( unsigned int i = 0; i < m_statements.size(); i++ )
	{
		// Liste
		std::vector< wxString > list;

		// Token in Liste umwandeln
		BeTool::StringToList( m_statements[i].GetToken(), separator, list );

		// Durchsuche die Liste der Tokens
		for ( unsigned int j = 0; j < list.size(); j++ )
			if ( list[j].compare( token ) == 0 )
				return true;
	}

	// Nichts gefunden
	return false;
}

// Statement by Token (exakt)
CBerylliumStatement CBerylliumStatementsManager::GetStatementByToken( const wxString token ) const
{
	// Leeres Statement
	CBerylliumStatement empty( "---", "---", "---" );

	// Durchsuchen und zurückgeben
	for ( unsigned int i = 0; i < m_statements.size(); ++i )
		if ( m_statements[i].GetToken().compare( token ) == 0 )
			return m_statements[i];

	// Leeres Statement zurückgeben
	return empty;
}
