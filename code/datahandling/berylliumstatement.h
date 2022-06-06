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

#pragma once

#include <string>

class CBerylliumStatement
{
public:
	// Constructor
	CBerylliumStatement( wxString token = "", wxString statement = "", wxString group = "" );

	// Destructor
	~CBerylliumStatement(void);

private:
	// Token: Kürzel z.B. die Satznummer oder die Substanz
	wxString		m_sToken;

	// Statement: Sicherheitssatz
	wxString		m_sStatement;

	// Kategorie oder Gruppe
	wxString		m_sGroup;

	// Gefährlichkeit
	unsigned int	m_iDanger;

public:
	// Get: Token
	wxString GetToken() const { return m_sToken; };

	// Set: Token
	void SetToken( wxString token ) { m_sToken = token; };

	// Get: Statement
	wxString GetStatement() const { return m_sStatement; };

	// Get: Token
	wxString GetGroup() const { return m_sGroup; };

	// Set: Token
	void SetGroup( wxString group ) { m_sGroup = group; };

	// Set: Statement
	void SetStatement( wxString statement ) { m_sStatement = statement; };

	// Statement um newstatement erweitern
	void AddToStatement( wxString newstatement ) { m_sStatement += " " + newstatement; };

	// Get: Gefährlichkeit
	unsigned int GetDangerPoints() const { return m_iDanger; };

	// Set: Gefährlichkeit
	void SetDangerPoints( unsigned int idanger ) { m_iDanger = idanger; };
};
