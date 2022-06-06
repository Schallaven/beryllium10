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

#include "berylliumsubstancedata.h"

// Abstrakte Klasse für die Spalten...
class IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	IBerylliumViewSafetySheetsColumn(wxString szTitle = "Column", int iWidth = -1)
	{
		m_iWidth = iWidth; m_iHeight = 50; m_szTitle = szTitle;
		bShow = true;
		bPrinting = false;
		bDynamic = ( iWidth < 0 );
		m_iPage = 0;
	};

	// Destruktor
	virtual ~IBerylliumViewSafetySheetsColumn() {};

	// Zeigt die Spalte
	void Show( bool bFlag = true ) { bShow = bFlag; };

	// Versteckt die Spalte
	void Hide() { bShow = false; };

	// Wird die Spalte angezeigt?
	bool IsShown() { return bShow; };

	// Hat die Spalte eine variable Breite?
	bool IsDynamic() { return bDynamic; };

	// Wird gedruckt?
	bool IsPrinting() { return bPrinting; };

	// Hat diese internen Namen?
	bool HasInternalName( const wxString name ) { return (m_szInternalName.compare(name) == 0); };

	// Interner Name
	std::string GetInternalName() { return m_szInternalName; };

	// Set: Es wird gedruckt.
	void SwitchToPrintMode( bool print ) { bPrinting = print; };

	// Set: Setzt die assozierete Seite im Substanz-Eigenschaftsdialog
	void SetPage( int page ) { m_iPage = page; };

	// Get: Gibt die assozierte Seite im Substanz-Eigenschaftsdialog zurück
	int GetPage() { return m_iPage; };

	// Get: Gibt die Spaltenbreite zurück
	int GetColumnWidth() { return m_iWidth; };

	// Set: Setzt die Spaltenbreite
	void SetColumnWidth( int iNewWidth ) { m_iWidth = iNewWidth; };

	// Get: Gibt den Spaltennamen zurück
	std::string GetColumnTitle() { return m_szTitle; };

	// Set: Setzt den Spaltennamen
	void SetColumnTitle(wxString szNewTitle ) { m_szTitle = szNewTitle; }

	// Zeichnet den Spaltennamen, gibt die Spaltengröße zurück
	int DrawHeader( wxDC &dc, wxPoint &pt );

	// Zeichnet den Spalteninhalt von der angegebenen Substanz, gibt die Spaltengröße zurück
	virtual int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData ) = 0;

protected:
	// Wird diese Spalte angezeigt?
	bool bShow;

	// Dynamische Breite?
	bool bDynamic;

	// Spaltenbreite
	int m_iWidth;

	// Spaltenhöhe
	int m_iHeight;

	// Spaltenname/titel
	std::string m_szTitle;

	// Interner Name
	std::string m_szInternalName;

	// Drucken wir? (Dann werden evtl. Bild rescaliert)
	bool bPrinting;

	// Diese Spalte bezieht sich auf diese Seite im Substanzeigenschaftsdialog
	int m_iPage;

	// Hilfsfunktion: Zeichnet die Phrasen (R/S/H/P-Sätze) entweder ein- oder zweizeilig in das gegebene Rechteck
	// Passt auch die Schriftgröße automatisch an.
	void DrawPhrases( wxDC &dc, const wxString &phrases, wxRect rect );
};


// Ableitung für Namen
class CBerylliumViewSafetySheetsColumnName : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnName(wxString szTitle = "Column", int iWidth = -1) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "name"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnName(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Molekulargewicht
class CBerylliumViewSafetySheetsColumnMG : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnMG(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "mass"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnMG(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Siedepunkt/Schmelzpunkt
class CBerylliumViewSafetySheetsColumnBoilingmelting : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnBoilingmelting(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "boilingmelting"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnBoilingmelting(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Flammpunkt (VbF. existiert nicht mehr)
class CBerylliumViewSafetySheetsColumnFlashpoint : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnFlashpoint(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "flashpoint"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnFlashpoint(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Gefahrensymbole
class CBerylliumViewSafetySheetsColumnHarzardsymbols : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnHarzardsymbols(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "harzardsymbols"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnHarzardsymbols(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Sicherheitssätze
class CBerylliumViewSafetySheetsColumnSafetyphrases : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnSafetyphrases(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "safetyphrases"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnSafetyphrases(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Sicherheitssätze (P- und H-Sätze)
class CBerylliumViewSafetySheetsColumnHarzardphrases : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnHarzardphrases(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "harzardphrases"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnHarzardphrases(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für GHS-Symbole
class CBerylliumViewSafetySheetsColumnGHSSymbols : public IBerylliumViewSafetySheetsColumn
{
private:
	// Wir laden die Bilder einmal beim Konstruktor und dann greifen wir immer darauf zurück...
	wxImage picSmall[9];
	wxImage picBig[9];

	// Größe der großen Bilder
	int picBigWidth;
	int picBigHeight;

public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnGHSSymbols(wxString szTitle, int iWidth);

	// Destruktor
	~CBerylliumViewSafetySheetsColumnGHSSymbols();

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );

	// Druckt den Spalteninhalt von der angegebenen Substanz
	int PrintContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Toxikologie
class CBerylliumViewSafetySheetsColumnToxicology : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnToxicology(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "toxicology"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnToxicology(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Ansatz
class CBerylliumViewSafetySheetsColumnAnsatz : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnAnsatz(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "ansatz"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnAnsatz(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};

// Ableitung für Gefährlichkeitspunkte
class CBerylliumViewSafetySheetsColumnDangerPoints : public IBerylliumViewSafetySheetsColumn
{
public:
	// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
	CBerylliumViewSafetySheetsColumnDangerPoints(wxString szTitle = "Column", int iWidth = -1) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth ) { m_szInternalName = "dangerpoints"; };

	// Destruktor
	~CBerylliumViewSafetySheetsColumnDangerPoints(void) {};

	// Zeichnet den Spalteninhalt von der angegebenen Substanz
	int DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData );
};
