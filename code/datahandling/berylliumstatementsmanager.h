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

#include <vector>
#include <string>

#include "../betool.h"
#include "berylliumstatement.h"

class CBerylliumStatementsManager
{
public:
	// Constructor
	CBerylliumStatementsManager(void);

	// Destructor
	~CBerylliumStatementsManager(void);

private:
	// Liste mit den Statements
	std::vector< CBerylliumStatement > m_statements;

public:
	// Statement hinzufügen (falls noch nicht vorhanden)
	void AddStatement( const wxString token, const wxString statement, const wxString group = "", const unsigned int iDanger = 0 );

	// Statement hinzufügen (falls vorhanden, dann wird statement einfach hinten angefügt)
	void AddStatementToToken( const wxString token, const wxString statement, const wxString group = "" );

	// Statement löschen
	void DeleteStatement( const wxString token );

	// Alle Statements löschen
	void DeleteAllStatements();

	// Ordnet die Statements
	void Sort();

	// Statement zu einem Token holen
	wxString operator[] ( wxString token ) const;

	// Token vorhanden (genau)
	bool HasToken( const wxString token ) const;

	// Token vorhanden (inkludiert), separator ist das Trennelement im Token
	bool HasTokenPart( const wxString token, const wxString separator ) const;

	// Anzahl der Statements
	unsigned int GetSize() const { return m_statements.size(); };

	// Statement by Index
	CBerylliumStatement GetStatementByIndex( unsigned int iIndex ) const { if ( iIndex < m_statements.size() ) return m_statements[iIndex]; else return CBerylliumStatement("!Nicht gefunden!", "", ""); };

	// Statement by Token (exakt)
	CBerylliumStatement GetStatementByToken( const wxString token ) const;
};
