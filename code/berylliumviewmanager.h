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

#include <vector>

#include <wx/print.h>

#include "interfaces/iview.h"
#include "datahandling/berylliumdocument.h"

// Verwalten die Ansichten des aktuellen Dokuments und das aktuelle Dokument selbst
class CBerylliumViewManager : public wxScrolledWindow, public IObserver
{
public:
	// Konstruktor
	CBerylliumViewManager( wxFrame &parent, CBerylliumDocument &doc);

	// Destruktor
	~CBerylliumViewManager(void);

	// IObserver: Model wurde verändert
	void ModelHasChanged();

	// Event: Datei wurde geladen
	void OnFileWasLoaded();

	// Event: Datei wird gespeichert
	void OnFileWillBeSaved();

	// Initialisiert alle Ansichten
	void InitManager();

	// Zeichnet das Fenster
	void OnPaint( wxPaintEvent &event );

	// Taste wurde gedrückt
	void OnKeyDown( wxKeyEvent& event );

	// Ereignis: Hintergrund wird gelöscht. Nichts unternehmen.
	void OnEraseBackground(wxEraseEvent& event) {};

	// Gibt die aktuelle Ansicht zurück
	IView *GetActiveView();

	// Setzt die aktuelle Ansicht
	void SetActiveView( unsigned int iViewIndex );

	// Wird bei einem Mausklick aufgerufen...
	void OnMouseClick( wxMouseEvent &event );

	// Wird bei einer Mausbewegung aufgerufen (gibt der aktuellen View das Event weiter)
	void OnMouseMove( wxMouseEvent &event );

	// GHS System wechseln
	void SetGHSMode( bool toGHS );

	// Wird aufgerufen, wenn die CommandID größer beID_POPUPMENU (20000) ist
	void OnPopupMenuClick( wxCommandEvent &event );

	// Gibt ein wxPrintOut-Objekt zurück
	wxPrintout *GetPrintOut();

private:
	// Referenz auf das Hauptfenster
	wxFrame &m_ParentWindow;

	// Referenz auf das Model
	CBerylliumDocument &m_Document;

	// Die Ansichten speichern
	std::vector< IView* > m_views;

	// Aktive Ansicht
	int m_active_view;

protected:
	DECLARE_EVENT_TABLE()

private:
	// Konstante: Rahmenbreite (wird um das "Blatt" freigelassen)
	static const int iBorderWidth = 40;
};
