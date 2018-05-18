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

#include <vector>

#include "../Interfaces/IView.h"
#include "../Datahandling/BerylliumStatementsManager.h"

#include "BerylliumViewSafetySheetsColumn.h"
#include "BerylliumDocument.h"

// Ansichtsklasse für Gruppenbetriebsanweisungen
class CBerylliumViewSafetySheets : public IView
{
private:
	// Spalten
	enum columns {
		COL_NAME,
		COL_MOLWEIGHT,
		COL_MELTINGBOILING,
		COL_FLASHPOINT,
		COL_HARZARDSYMBOLS,
		COL_GHSSYMBOLS,
		COL_RISKSAFETIES,
		COL_HARZARDPRECAUTION,
		COL_TOXICOLOGY,
		COL_BATCH,
		COL_DANGERPOINTS
	};

	// Statements
	enum statements {
		STATEMENT_RISKS,
		STATEMENT_SAFETIES,
		STATEMENT_HARZARDS,
		STATEMENT_PRECAUTIONARIES,
		STATEMENT_DANGERS,
		STATEMENT_BEHAVIOURS,
		STATEMENT_FIRSTAIDS,
		STATEMENT_WASTE
	};

	// Zeigen wir die H/P-Sätze statt den R/S-Sätzen?
	bool	bShowHPStatements;

	// GHS-Modus?
	bool	bGHSMode;

public:
	// Konstruktor
	CBerylliumViewSafetySheets( CBerylliumDocument &doc, wxWindow &parent );

	// Destruktor
	~CBerylliumViewSafetySheets(void);

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

public:
	// Zeichnet eine "Vorderseite"
	void DrawFrontSheet( wxDC &dc, unsigned int iPage );

	// Zeichnet eine "Rückseite"
	void DrawBackSheet( wxDC &dc, unsigned int iPage );

private:
	// Vorderseite: Auswahl
	void DrawFrontSheetSelection( wxDC &dc );

	// Vorderseite: Rahmen
	void DrawFrontSheetBorder( wxDC &dc );

	// Vorderseite: Kopfdaten
	void DrawFrontSheetHead( wxDC &dc, unsigned int iPage );

	// Vorderseite: Personendaten
	void DrawFrontSheetPersonal( wxDC &dc, unsigned int iPage );

	// Vorderseite: Substanzdaten
	void DrawFrontSheetSubstances( wxDC &dc, unsigned int iPage );

	// Vorderseite: R-/S-Sätze
	void DrawFrontSheetWarnings( wxDC &dc, unsigned int iPage );

	// Vorderseite: R-/S-Sätze-Block
	void DrawFrontSheetWarningsBlock( wxDC &dc, const CBerylliumStatementsManager &slist, wxString &sPrefix, wxRect rect );

	// Vorderseite: R-/S-Sätze-Satz
	int DrawFrontSheetWarningsSentence( wxDC &dc, const CBerylliumStatement &sStatement, wxString &sPrefix, wxRect rect );

	// Rückseite: Auswahl
	void DrawBackSheetSelection( wxDC &dc );

	// Rückseite: Rahmen
	void DrawBackSheetBorder( wxDC &dc );

	// Rückseite: Texte
	void DrawBackSheetLabels( wxDC &dc, unsigned int iPage );

	// Rückseite: Texte für die Unterschriftenboxen
	void DrawBackSheetLabelBox( wxDC &dc, wxRect rect, const wxString &text );

	// Rückseite: Daten
	void DrawBackSheetData( wxDC &dc, unsigned int iPage );

	// Rückseite: Zeichnet die "substanzabhängigen" Sätze
	int DrawBackSheetBlockSubstanceStatements( wxDC &dc, wxRect rect, unsigned int iDoublePage, const CBerylliumStatementsManager &statements );

	// Rückseite: Zeichnet die "allgemeinen" Sätze
	void DrawBackSheetBlockCommon( wxDC &dc, wxRect rect, unsigned int iDoublePage, const std::vector<wxString> &statements );

	// Rückseite: Zeichnet einen Satz (mit Punkt)
	int DrawBackSheetSentence( wxDC &dc, wxRect rect, const wxString &sentence, const wxString &prefix );

	// Zeigt den Eigenschaftsdialog für die Betriebsanweisung (gelb Felder)
	void ShowDialogProperties();

	// Zeigt den Eigenschaftsdialog für die Betriebsanweisung (blaue Felder)
	void ShowDialogGeneral();

	// Zeigt entweder den Eigenschaftsdialog für eine Substanz oder den Suchdialog
	// für eine neue Substanz an
	void ShowDialogSubstance();

	// Zeigt den Sätze-verstecken-Dialog an
	void ShowDialogPhrases( unsigned int iPage );

	// Zeigt den Dialog für die Texte der Rückseite an
	void ShowDialogStatementEditor();

	// Zeigt das Contextmenu für Substanzen an
	void ShowSubstancePopupMenu( int imousex, int imousey );

	// Verarbeitet die IDs des Popupmenus
	void DoPopupMenu( wxCommandEvent &event );

	// Fügt eine neue Substanz ein (zeigt Dialog, etc)
	void SubstanceAdd();

	// Löscht Substanz x aus dem Datenblatt
	void SubstanceDel( unsigned int iSubstance );

	// Zeigt den Eigenschaftsdialog für Substanz x an
	void SubstanceProps( unsigned int iSubstance );

	// öffnet die Wikipedia-Seite
	void SubstanceWiki( unsigned int iSubstance );

	// Sucht bei euSDB nach der CAS-Nummer
	void SubstanceEUSDB( unsigned int iSubstance );

	// Importiert die Substanz
	void SubstanceImport();

	// Exportiert eine Substanz
	void SubstanceExport( unsigned int iSubstance );

private:
	// Modeldaten
	CBerylliumDocument &m_Document;

	// Elternfenster
	wxWindow &m_Parent;

	// Aktuelle Seite
	unsigned int m_iPage;

	// Selektion
	unsigned int m_iSelected;
	unsigned int m_iSubselected;
	unsigned int m_iColumnSelected;

	// Druckmodus
	bool		 m_isPrinting;

	// Vorschaumodus
	bool		 m_isPreview;

	// Liste aller Spalten für die Vorderseite
	std::vector< IBerylliumViewSafetySheetsColumn* > m_Columns;

	// Berechnet die Breite der Spalten und setzt diese entsprechend
	// (wird nach Spaltenauswahl aufgerufen)
	void ColumnsCalculate();

	// Listen der entsprechenden Sätze, nach Seiten geordnet (zum Zeichnen)
	// 0: Risks, 1: Safeties, 2: Harzards, 3: Precautionary,
	// 4: Gefahren, 5: Schutzmaßnahmen, 6: Erste-Hilfe, 7: Entsorgung
	std::vector< CBerylliumStatementsManager > m_statements[8];

	// Ordnet alle Sätze für Seite iPage neu
	void OrderStatements( unsigned int iPage );

	// Ordnet alle Sätze neu (fängt mit iStartPage an)
	void OrderAllStatements( unsigned int iStartPage );
};
