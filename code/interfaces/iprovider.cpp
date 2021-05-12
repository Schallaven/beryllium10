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
#include "iprovider.h"

IProvider::IProvider()
{
	// Sprache setzen
	iLanguage = 0;

	// ID setzen
	iID = 0;

	// Proxy setzen
	proxyhost = "";
	proxyport = 0;
}

IProvider::~IProvider(void)
{
}

// Prüft ob ein String eine CAS-Nummer ist
bool IProvider::IsCAS( wxString text )
{
	// 1. Kriterium: String besteh nur aus Ziffern und Bindestrichen
	// 2. Kriterium: Es gibt nur 2x '-'
	int sep = 0;
	for ( unsigned int i = 0; i < text.size(); ++i )
	{
		// Wenn der String was anderes enthält, dann raus hier (false)!
		if ( (text[i] != '-') && ((text[i] < '0') || (text[i] > '9')) )
			return false;

		// Zähler für Bindestriche
		if ( text[i] == '-' )
			sep++;

		// Mehr als 2 Bindestriche?
		if ( sep > 2 )
			return false;
	}

	// Ansonsten ists eine CAS-Nummer, zumindest formal
	return true;
}

// Hilfsfunktion: Entfernt den kompletten Block zwischen left und right
void IProvider::RemoveBlockFromString( wxString &szText, wxString left, wxString right )
{
	wxString::size_type iL = szText.find( left );
	wxString::size_type iG = szText.find( right, iL );

	while ( (iL != wxString::npos) && (iG != wxString::npos) )
	{
		szText = szText.substr(0, iL) + szText.substr( iG + right.length() );

		iL = szText.find( left );
		iG = szText.find( right, iL );
	}
}

// Hilfsfunktion: Extrahiert alles was zwischen left und right zu finden ist
// und gibt es zurück
wxString IProvider::ExtractFromString( const wxString &text, wxString left, wxString right)
{
	wxString::size_type iL = text.find( left );
	wxString::size_type iG = text.find( right, iL );

	if ( (iL == wxString::npos) || (iG == wxString::npos) )
		return "";

	// Extrahieren
	return text.substr( iL, iG - iL + right.length() );
}

// Hilfsfunktion: Extrahiert alles was zwischen left und right zu finden ist, entfernt es dann
// und gibt es zurück
wxString IProvider::ExtractAndRemoveFromString( wxString &text, wxString left, wxString right)
{
	wxString::size_type iL = text.find( left );
	wxString::size_type iG = text.find( right, iL );

	if ( (iL == wxString::npos) || (iG == wxString::npos) )
		return "";

	// Extrahieren
	wxString subtext = text.substr( iL, iG - iL + right.length() );

	// Entfernen
	text = text.substr(0, iL) + text.substr( iG + right.length() );

	// Zurückgeben
	return subtext;
}

// Hilfsfunktion: Extrahiert den ganzen Text aus node, egal wie verschachtelt er ist
void IProvider::GetAllTextFromNode( wxXmlNode *node, wxString &text )
{
	// Alle Subelemente suchen
	for( wxXmlNode* subnode = node->GetChildren(); subnode; subnode = subnode->GetNext() )
	{
		// Ist das Element ein Textelement?
		if ( subnode->GetType() == wxXML_TEXT_NODE )
		{
			// Content holen
			text += subnode->GetContent();

			// weitermachen
			continue;
		}

		// Hat des Element Childs?
		else if ( subnode->GetType() == wxXML_ELEMENT_NODE )
			GetAllTextFromNode( subnode, text );
	}
}

// Hilfsfunktion: Ersetzt alle Vorkommen von "findstr" mit "replacewith"
void IProvider::ReplaceAll( wxString &text, wxString findstr, wxString replacewith )
{
	size_t pos = text.find(findstr);

	while( pos != wxString::npos)
	{
		text.replace(pos,findstr.size(),replacewith);

		pos = text.find(findstr);
	}
}
