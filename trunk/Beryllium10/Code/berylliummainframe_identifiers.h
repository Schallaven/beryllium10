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

// Defines für die Menus/Toolbar
// -----------------------------

// Datei
#define beID_NEW			5000
#define beID_OPEN			5001
#define beID_SAVE			5002
#define beID_SAVEAS			5003
#define beID_PRINT			5004
#define beID_PRINT_PREVIEW	5005
#define beID_EXIT			5007

// Ansicht
#define beID_VIEW_TOOLBAR		6001
#define beID_VIEW_STATUSBAR		6002
	
#define beID_VIEW_FIRST			6101	// Erste Ansicht
#define beID_VIEW_SAFETYSHEET	6101	// Ansicht: Gruppenbetriebsanweisung
#define beID_VIEW_DATASHEET		6102	// Ansicht: Datenblatt für jede Substanz
#define beID_VIEW_LAST			6103	// Letzte Ansicht

// Werkzeuge (Sprache, Äquivalenz-Multiplikator, ...)
#define beID_TOOL_LANGUAGE_FIRST	6500
#define beID_TOOL_LANGUAGE_ENGLISH	6558
#define beID_TOOL_LANGUAGE_GERMAN	6589
#define beID_TOOL_LANGUAGE_LAST		6600

// Sortierungen
#define beID_TOOL_SORTBY_FIRST		6700
#define beID_TOOL_SORTBY_NAME		6700
#define beID_TOOL_SORTBY_DANGER		6701
#define beID_TOOL_SORTBY_AMOUNT		6702
#define beID_TOOL_SORTBY_LAST		6750

#define beID_TOOL_MULTIPLIER		6801
#define beID_TOOL_OPTIONS			6900

// Hilfe (?)
#define beID_HELP_INFO		7001
#define beID_HELP_BREAKOUT	7002
#define beID_HELP_TOPICS	7003
#define beID_HELP_UPDATES	7004
#define beID_HELP_BUGREPORT	7005

// Sonstige
#define beID_PAGE_LEFT		8001
#define beID_PAGE_RIGHT		8002
#define beID_SWITCH_GHS		8003

// Popupmenus (diese werden direkt von der View erstellt und verarbeitet)
#define beID_POPUPMENU		20000

#define beID_SUBSTANCE_ADD		20001
#define beID_SUBSTANCE_DEL		20002

#define beID_SUBSTANCE_IMPORT	20102
#define beID_SUBSTANCE_EXPORT	20103
#define beID_SUBSTANCE_LIBRARY	20104

#define beID_SUBSTANCE_WIKI		20105
#define beID_SUBSTANCE_CAS		20106

#define beID_SUBSTANCE_PROPS	20107

// Letzte ID fürs Popupmenu
#define beID_POPUPMENU_LAST	40000