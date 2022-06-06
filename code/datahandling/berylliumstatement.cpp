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
#include "berylliumstatement.h"

CBerylliumStatement::CBerylliumStatement(wxString token, wxString statement, wxString group)
{
	// Daten übernehmen
	m_sToken		= token;
	m_sStatement	= statement;
	m_sGroup		= group;

	// Gefährlichkeit
	m_iDanger		= 0;
}

CBerylliumStatement::~CBerylliumStatement(void)
{
}
