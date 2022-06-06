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

// Druckfunktionen
#include <wx/print.h>

// Interface: Ansichtsklasse
class IView
{
public:
	// Konstruktor
	IView(void);

	// Destruktor
	virtual ~IView(void) = 0;

	// Zeichenfunktion
	virtual void Draw( wxDC &dc ) = 0;

	// Maus wurde bewegt (für Highlights und sowas wichtig)
	virtual void OnMouseMove(wxMouseEvent &event) = 0;

	// es wurde mit der Maus geklickt (Taste steht in event)
	virtual void OnMouseClick(wxMouseEvent &event) = 0;

	// Gibt die Größe der aktuellen Seite zurück
	virtual wxSize GetPageSize() = 0;

	// Gibt die Anzahl der Seiten zurück
	virtual unsigned int GetNumberOfPages() = 0;

	// Aktuelle Seite
	virtual unsigned int GetPageNumber() = 0;
	// Seite setzen
	virtual void SetPageNumber( unsigned int iPage ) = 0;
	// Nächste Seite
	virtual void NextPage() = 0;
	// Vorherige Seite
	virtual void PrevPage() = 0;

	// Befindet sich die Ansicht im GHS-Modus?
	virtual bool IsGHSMode() = 0;

	// GHS-Modus setzen
	virtual void SetGHSMode( bool toGHS ) = 0;

	// Zur Verarbeitung eines evtl. vorhandenen PopupMenus
	virtual void DoPopupMenu( wxCommandEvent &event ) = 0;

	// Event: Datei wurde geladen
	virtual void OnFileWasLoaded() = 0;

	// Event: Datei wurde gespeichert
	virtual void OnFileWillBeSaved() = 0;

	// Gibt ein wxPrintOut-Objekt zurück
	virtual	wxPrintout* GetPrintOut() = 0;

	// Setzt den Druckmodus bzw. wieder zurück
	virtual void SwitchToPrintMode( bool bPrint = true ) = 0;

	// Setzt den Vorschaumodus bzw. wieder zurück
	virtual void SwitchToPreviewMode( bool bPreview = true ) = 0;

	// Wird aufgerufen, falls zu dieser Ansicht gewechselt wird
	virtual void SwitchedToView() = 0;
};
