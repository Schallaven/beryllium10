﻿/***********************************************************************************
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

#pragma once

// HTTP-Funktionen
#include <wx/protocol/http.h>

// Substanzdaten
#include "../datahandling/berylliumsubstancedata.h"

// Interface: Hersteller für Chemikalien
class IProvider
{
public:
	// Konstruktor
	IProvider();

	// Destruktor
	virtual ~IProvider(void);

	// Ergebnisdaten
	struct result {
		wxString	name;			// Name der Substanz
		wxString	info;			// Info über die Substanz (wird in einer zweiten Zeile angezeigt)
		int			serial;			// eindeutige ID, damit das Interface die Substanz genau zuordnene kann
		wxString	data;			// Datenblock
	};

protected:
	// wxHTTP-Objekt
	wxHTTP	m_http;

	// Sprache
	long iLanguage;

	// ID für diesen Provider
	int iID;

	// Proxy-Server
	wxString proxyhost;
	unsigned short proxyport;

	// Prüft ob ein String eine CAS-Nummer ist
	bool IsCAS( wxString text );

	// Hilfsfunktion: Entfernt den kompletten Block zwischen left und right
	void RemoveBlockFromString( wxString &szText, wxString left, wxString right );

	// Hilfsfunktion: Extrahiert alles was zwischen left und right zu finden ist
	// und gibt es zurück
	wxString ExtractFromString( const wxString &text, wxString left, wxString right);

	// Hilfsfunktion: Extrahiert alles was zwischen left und right zu finden ist, entfernt es dann
	// und gibt es zurück
	wxString ExtractAndRemoveFromString( wxString &text, wxString left, wxString right);

	// Hilfsfunktion: Extrahiert den ganzen Text aus node, egal wie verschachtelt er ist
	void GetAllTextFromNode( wxXmlNode *node, wxString &text );

	// Hilfsfunktion: Ersetzt alle Vorkommen von "findstr" mit "replacewith"
	void ReplaceAll( wxString &text, wxString findstr, wxString replacewith );

public:
	// 1. Suchanfrage stellen
	virtual bool SearchForCompound( const wxString searchtext, bool bExactMatch ) = 0;

	// 2. Daten mit entsprechender ID holen
	virtual void GetDataOfCompound( const int iSerial, CBerylliumSubstanceData &m_data ) = 0;

	// Get: Herstellername
	virtual wxString GetName() const = 0;

	// Get: Liste mit Substanzen holen
	virtual void GetResults( std::vector< result > &m_list ) const = 0;

	// Set: Sprache setzen
	void SetLanguage( long newlang ) { iLanguage = newlang; };

	// Get: Sprache holen
	long GetLanguage() { return iLanguage; };

	// Set: ID setzen
	void SetId( int newid ) { iID = newid; };

	// Get: ID holen
	int GetId() { return iID; };

	// Set: Proxy
	void SetProxy( const wxString newproxyhost, unsigned short newproxyport ) { proxyhost = newproxyhost; proxyport = newproxyport; };

	// Proxy löschen
	void ClearProxy() { proxyhost = ""; proxyport = 0; };

	// Get: Proxy
	wxString GetProxy() { return wxString::Format("%s:%d",proxyhost,proxyport); };
};
