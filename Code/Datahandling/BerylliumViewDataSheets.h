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

#pragma once

#include "../Interfaces/IView.h"

#include "BerylliumDocument.h"

// Ansichtsklasse für Einzelbetriebsanweisungen
class CBerylliumViewDataSheets : public IView
{
public:
	// Konstruktor
	CBerylliumViewDataSheets( CBerylliumDocument &doc, wxWindow &parent );

	// Destruktor
	~CBerylliumViewDataSheets(void);

	// Zeichenfunktion
	void Draw( wxDC &dc );

	// Mausbewegung
	void OnMouseMove(wxMouseEvent &event);

	// Mausklick
	void OnMouseClick( wxMouseEvent &event );

	// Gibt die Größe der aktuellen Seite zurück
	wxSize GetPageSize();

	// Gibt die Anzahl der Seiten zurück
	unsigned int GetNumberOfPages();

	// Get: Gibt die aktuelle Seite zurück
	unsigned int GetPageNumber() { return m_iPage; };
	// Set: Setzt die Seite
	void SetPageNumber( unsigned int iPage );
	// Set: Nächste Seite
	void NextPage() { SetPageNumber( m_iPage + 1 ); };
	// Set: Vorherige Seite
	void PrevPage() { if ( m_iPage > 0 ) SetPageNumber( m_iPage - 1 ); };

	// GHS-Modus?
	bool IsGHSMode();

	// GHS-Modus setzen
	void SetGHSMode( bool toGHS );

	// Event: Datei wurde geladen
	void OnFileWasLoaded();

	// Event: Datei wird gespeichert
	void OnFileWillBeSaved();

	// Gibt ein wxPrintOut-Objekt zurück
	wxPrintout *GetPrintOut();

	// Setzt den Druckmodus bzw. wieder zurück
	void SwitchToPrintMode( bool bPrint );

	// Setzt den Vorschaumodus bzw. wieder zurück
	void SwitchToPreviewMode( bool bPreview );

	// Wird aufgerufen, falls zu dieser Ansicht gewechselt wird
	void SwitchedToView();

	// Zeichnet eine Substanzseite
	void DrawSubstancePage( wxDC &dc, unsigned int iPage );

private:
	// Verarbeitet die IDs des Popupmenus
	void DoPopupMenu( wxCommandEvent &event );

	// Zeichnet die "Hinzufügen"-Seite
	void DrawNewSubstancePage( wxDC &dc );	

	// Substanzseite: Rahmen
	void DrawSubstancePageBorder( wxDC &dc );
	
	// Substanzseite: Zeichnet die Markierungen
	void DrawSubstancePageSelection( wxDC &dc, unsigned int iPage );

	// Substanzseite: Kopf
	void DrawSubstancePageHead( wxDC &dc, unsigned int iPage );

	// Substanzseite: allgemeine und physikalische Eigenschaften
	void DrawSubstancePageGeneralData( wxDC &dc, unsigned int iPage );

	// Substanzseite: Gefahrenhinweise
	void DrawSubstancePageHazards( wxDC &dc, unsigned int iPage );

	// Substanzseite: GHS-Daten
	void DrawSubstancePageGHS( wxDC &dc, unsigned int iPage );

	// Substanzseite: Toxikologische Daten
	void DrawSubstancePageToxicology( wxDC &dc, unsigned int iPage );

	// Substanzseite: Zeichnet die speziellen Hinweise einer Substanz (z.B. Entsorgung)
	void DrawSubstancePageStatements( wxDC &dc, unsigned int iPage );
	
	// Hilfsfunktion: Zeichnet den Text als Block und gibt die neue y-Koordinate zurück
	int DrawSubstanceTextBlock( wxDC &dc, int x, int y, int maxwidth, wxString text, int indentfirstline = 0 );

	// Hilfsfunktion: Zeichnet die GHS-Symbole (Bildschirm)
	void DrawSubstancePageGHSSymbols( wxDC &dc, const std::vector< wxString > symbols, int x, int y, int maxwidth );

	// Fügt eine neue Substanz ein (zeigt Dialog, etc)
	void SubstanceAdd();

	// Zeigt den Eigenschaftsdialog für die aktuelle Substanz auf einer bestimmten Seite an
	void SubstanceProps( unsigned int iSubpage );

private:
	// Zeigen wir die H/P-Sätze statt den R/S-Sätzen?
	bool	bShowHPStatements;

	// GHS-Modus?
	bool	bGHSMode;

	// Modeldaten
	CBerylliumDocument &m_Document;

	// Elternfenster
	wxWindow &m_Parent;

	// Aktuelle Seite
	unsigned int m_iPage;

	// Selektion
	unsigned int m_iSelected;

	// Druckmodus
	bool		 m_isPrinting;

	// Vorschaumodus
	bool		 m_isPreview;

	// Kastendimensionen
	int			m_iNewWidth;
	int			m_iNewHeight;

	// Wir laden die Bilder einmal beim Konstruktor und dann greifen wir immer darauf zurück...
	wxImage picBig[9];

	// Größe der großen Bilder
	int picBigWidth;
	int picBigHeight;

	// Variable Grenze zwischen Gefahrstoffdaten und Toxikologiedaten
	int iBorderToxicology;

};