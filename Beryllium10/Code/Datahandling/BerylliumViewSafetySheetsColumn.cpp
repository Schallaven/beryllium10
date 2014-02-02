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

#include "stdafx.h"

#include "BerylliumViewSafetySheetsColumn.h"

#include "../betool.h"

#include <wx/filesys.h>
#include <wx/wfstream.h>

// Zeichnet einfach den Namen der Spalte mit der Labelfunktion...
int IBerylliumViewSafetySheetsColumn::DrawHeader(wxDC &dc, wxPoint &pt)
{
	// Zeichnen des Titels
	dc.DrawLabel( m_szTitle, wxRect( pt.x, pt.y, m_iWidth, m_iHeight ), wxALIGN_CENTER );

	// Breite zurückgeben
	return m_iWidth;
}

// Hilfsfunktion: Zeichnet die Phrasen (R/S/H/P-Sätze) entweder ein- oder zweizeilig in das gegebene Rechteck
// Passt auch die Schriftgröße automatisch an.
void IBerylliumViewSafetySheetsColumn::DrawPhrases( wxDC &dc, const wxString &phrases, wxRect rect )
{
	// einen Rahmen von 2 Pixeln jeweils abziehen
	rect.Deflate( 2, 2 );

	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift setzen
	dc.SetFont( fontCopy );

	// String kopieren	
	wxString szTemp = phrases;

	// Zwei Zeilen "einfügen"
	size_t breakat = szTemp.find("-", szTemp.length()/2);

	if ( breakat != szTemp.npos )
		szTemp.insert( breakat + 1, "\n" );

	// Dimension der R-Sätze
	wxSize sizeText = dc.GetTextExtent( phrases );

	// Einzeilig oder Mehrzeilig?
	bool bMultiLine = false;

	// Falls der Text nicht reinpasst, dann wird hier versucht,
	// a) den Text in zwei Zeilen reinzupressen
	// b) die Schriftgröße in Stufen um 1 zu verkleinern
	// Änderung: 1.12.2013, maximal 10 Punkte verkleinern (zum Vermeiden von Endlosschleifen)!
	int n = 0;
	while ( ((sizeText.GetWidth() > rect.GetWidth()) || (sizeText.GetHeight() > rect.GetHeight())) && (n < 10) )
	{
		// Passt das Ganze zufällig in zwei Zeilen rein?
		if ( ((sizeText.GetWidth() < (rect.GetWidth()*2)) && ((sizeText.GetHeight()*2) < rect.GetHeight())) && !bMultiLine )
		{
			// Größe holen
			sizeText = dc.GetMultiLineTextExtent( szTemp );

			// Mehrzeiler
			bMultiLine = true;
		}

		// andernfalls Schrift verkleinern
		else
		{
			// Multiline zurücksetzen
			bMultiLine = false;

			// Schrift um eins verkleinern
			fontCopy.SetPointSize( fontCopy.GetPointSize() - 1 );

			// Schrift setzen
			dc.SetFont( fontCopy );

			// Erneut berechnen
			sizeText = dc.GetTextExtent( phrases );
		}

		// n erhöhen
		n++;
	};	

	// Phrase zeichnen
	dc.DrawLabel( ( bMultiLine ) ? szTemp : phrases, wxRect(rect.x, rect.y, rect.GetWidth(),  rect.GetHeight()), wxALIGN_CENTER );
}

// Zeichnet die Namen/Formel-Spalte
int CBerylliumViewSafetySheetsColumnName::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift auf Fett setzen
	fontCopy.SetWeight( wxFONTWEIGHT_BOLD );

	// Schrift setzen
	dc.SetFont( fontCopy );

	// Namen zeichnen
	sdData.DrawName( dc, pt.x + 10, pt.y + 10, m_iWidth - 10, m_iHeight / 2 );

	// Schrift wiederherstellen
	dc.SetFont( fontTemp );

	// Formel zeichen
	sdData.DrawFormula( dc, pt.x + 20, pt.y + (m_iHeight / 2) );

	return m_iWidth;
}

// Zeichnet die Spalte für die Molare Masse
int CBerylliumViewSafetySheetsColumnMG::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Ist die Molare Masse = 0 ? Dann einen Strich anzeigen.
	if ( sdData.dbMolWeight == 0.0f )
	{
		dc.DrawLabel( "-", wxRect( pt.x, pt.y, m_iWidth, m_iHeight ), wxALIGN_CENTER );

		return m_iWidth;
	}


	// String für das Molgewicht
	wxString molWeight = wxString::Format("%.2f", sdData.dbMolWeight);

	// ',' durch '.' ersetzen
	molWeight.Replace( ",", ".", true );

	// Zeichnen des Titels
	dc.DrawLabel( molWeight, wxRect( pt.x, pt.y, m_iWidth, m_iHeight ), wxALIGN_CENTER );

	// Breite zurückgeben
	return m_iWidth;
}

// Spalte für Siede- und Schmelzpunkt
int CBerylliumViewSafetySheetsColumnBoilingmelting::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Schmelzpunkt zeichnen
	if ( sdData.szMeltingPoint.length() == 0 )
		dc.DrawLabel( "-", wxRect( pt.x, pt.y - 5 + (m_iHeight/2), m_iWidth, m_iHeight / 2), wxALIGN_CENTER );
	else
		dc.DrawLabel( sdData.szMeltingPoint, wxRect( pt.x, pt.y - 5 + (m_iHeight/2), m_iWidth, m_iHeight / 2), wxALIGN_CENTER );

	// Zersetzung?
	if ( sdData.bDecomposition )
	{
		dc.DrawLabel( _(L"Zers. bei"), wxRect( pt.x, pt.y + 5, m_iWidth, m_iHeight / 2), wxALIGN_CENTER );

		return m_iWidth;
	}

	// Siedepunkt zeichnen
	if ( sdData.szBoilingPoint.length() == 0 )
		dc.DrawLabel( "-", wxRect( pt.x, pt.y + 5, m_iWidth, m_iHeight / 2), wxALIGN_CENTER );
	else
		dc.DrawLabel( sdData.szBoilingPoint, wxRect( pt.x, pt.y + 5, m_iWidth, m_iHeight / 2), wxALIGN_CENTER );

	return m_iWidth;
}

// Spalte für Flammpunkt
int CBerylliumViewSafetySheetsColumnFlashpoint::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Größe der Schrift holen
	// wxSize size = dc.GetTextExtent( sdData.szMeltingPoint );

	// Flammpunkt zeichnen
	if ( sdData.szFlashPoint.length() == 0 )
		dc.DrawLabel( "-", wxRect(pt.x, pt.y, m_iWidth, m_iHeight), wxALIGN_CENTER );
	else
		dc.DrawLabel( sdData.szFlashPoint, wxRect(pt.x, pt.y, m_iWidth, m_iHeight), wxALIGN_CENTER );

	return m_iWidth;
}

// Gefahrensymbole
int CBerylliumViewSafetySheetsColumnHarzardsymbols::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// String holen für die Gefahrensymbole
	wxString szHarzards = BeTool::ListToString( sdData.szHarzards, ", " );

	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift auf Fett setzen
	fontCopy.SetWeight( wxFONTWEIGHT_BOLD );

	// Schriftgröße ändern
	fontCopy.SetPointSize( 16 );

	// Schrift setzen
	dc.SetFont( fontCopy );

	// Dimension des Textes
	wxSize sizeText = dc.GetTextExtent( szHarzards );

	// Falls der Text nicht reinpasst, dann wird hier versucht,
	// die Schriftgröße in Stufen um 2 zu verkleinern
	while ( (sizeText.GetWidth() > m_iWidth) || (sizeText.GetHeight() > m_iHeight) )
	{
		// Schrift um zwei verkleinern
		fontCopy.SetPointSize( fontCopy.GetPointSize() - 2 );

		// Schrift setzen
		dc.SetFont( fontCopy );

		// Erneut berechnen
		sizeText = dc.GetTextExtent( szHarzards );
	};

	// Gefahrensymbole zeichnen
	dc.DrawLabel( szHarzards, wxRect(pt.x, pt.y, m_iWidth, m_iHeight), wxALIGN_CENTER );

	// Schrift zurücksetzen
	dc.SetFont( fontTemp );

	// Und zurück
	return m_iWidth;
}

// R- und S-Sätze werden hier gezeichnet
int CBerylliumViewSafetySheetsColumnSafetyphrases::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Beide Zeichenketten leer?
	if ( (sdData.szRisks.length() == 0) && (sdData.szSafeties.length() == 0) )
		return m_iWidth;

	// R-Sätze
	wxString risks = (sdData.szRisks.length() == 0) ? "" : "R " + sdData.szRisks;

	// S-Sätze
	wxString safeties = (sdData.szSafeties.length() == 0) ? "" : "S " + sdData.szSafeties;

	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift setzen
	dc.SetFont( fontCopy );

	// R-Sätze zeichnen
	DrawPhrases( dc, risks, wxRect( pt.x, pt.y, m_iWidth, m_iHeight / 2 ) );

	// S-Sätze zeichnen
	DrawPhrases( dc, safeties, wxRect( pt.x, pt.y + (m_iHeight / 2), m_iWidth, m_iHeight / 2 ) );	

	// Schrift zurücksetzen
	dc.SetFont( fontTemp );

	// Und zurück
	return m_iWidth;
}

// P- und H-Sätze werden hier gezeichnet
int CBerylliumViewSafetySheetsColumnHarzardphrases::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Beide Zeichenketten leer?
	if ( (sdData.szHarzardSatements.length() == 0) && (sdData.szPrecautionaryStatements.length() == 0) )
		return m_iWidth;

	// H-Sätze
	wxString harzards = (sdData.szHarzardSatements.length() == 0) ? "" : "H " + sdData.szHarzardSatements;

	// P-Sätze
	wxString precautionary = (sdData.szPrecautionaryStatements.length() == 0) ? "" : "P " + sdData.szPrecautionaryStatements;

	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift setzen
	dc.SetFont( fontCopy );

	// R-Sätze zeichnen
	DrawPhrases( dc, harzards, wxRect( pt.x, pt.y, m_iWidth, m_iHeight / 2 ) );

	// S-Sätze zeichnen
	DrawPhrases( dc, precautionary, wxRect( pt.x, pt.y + (m_iHeight / 2), m_iWidth, m_iHeight / 2 ) );	

	// Schrift zurücksetzen
	dc.SetFont( fontTemp );

	// Und zurück
	return m_iWidth;
}

// Konstruktor: setzt die Standardgröße einer Spalte (-1 = variabel)
CBerylliumViewSafetySheetsColumnGHSSymbols::CBerylliumViewSafetySheetsColumnGHSSymbols(wxString szTitle = "Column", int iWidth = -1 ) : IBerylliumViewSafetySheetsColumn( szTitle, iWidth )
{
	// Interner Name
	m_szInternalName = "ghssymbols";

	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Hier holen wir uns die Symbole (klein + groß)
	for ( int i = 0; i < 9; ++i )
	{
		// Datei-Stream laden
		wxFSFile *file = filesystem->OpenFile("memory:ghs" + wxString::Format("%02d", i+1) + ".png");

		// Datei konnte nicht geladen werden
		if ( file == NULL )
			continue;

		// ein Image-Objekt anlegen
		wxImage img( *file->GetStream(), wxBITMAP_TYPE_PNG  );

		// Bild nicht ok?
		if ( !img.IsOk() )
		{
			delete file;
			continue;
		}

		// Speichern
		picSmall[i] = img.Copy();

		// Datei löschen
		delete file;

		// Datei-Stream laden
		wxFSFile *fileBig = filesystem->OpenFile("memory:org_ghs" + wxString::Format("%02d", i+1) + ".png");

		// Datei konnte nicht geladen werden
		if ( fileBig == NULL )
			continue;

		// ein Image-Objekt anlegen
		wxImage imgBig( *fileBig->GetStream(), wxBITMAP_TYPE_PNG  );

		// Bild nicht ok?
		if ( !imgBig.IsOk() )
		{
			delete fileBig;
			continue;
		}

		// Speichern
		picBig[i] = imgBig.Copy();

		// Größe speichern
		picBigWidth  = imgBig.GetWidth();
		picBigHeight = imgBig.GetHeight();

		// Datei löschen
		delete fileBig;
	}


	// Pointer zum Dateisystem löschen
	delete filesystem;
}

CBerylliumViewSafetySheetsColumnGHSSymbols::~CBerylliumViewSafetySheetsColumnGHSSymbols()
{
}

int CBerylliumViewSafetySheetsColumnGHSSymbols::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Druckmodus? Dann benutzen wir in diesem Fall eine spezielle Druckfunktion (Transparenz...)
	if ( IsPrinting() )
		return PrintContent( dc, pt, sdData );

	// Keine Symbole oder kein Signalwort? Dann raus hier!
	if ( (sdData.szSignalWord.length() == 0) || (sdData.szGHSSymbols.size() == 0) )
		return m_iWidth;

	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift auf Fett setzen
	fontCopy.SetWeight( wxFONTWEIGHT_BOLD );

	// Schrift setzen
	dc.SetFont( fontCopy );

	// Schriftgröße ermitteln
	wxSize sizeSignal = dc.GetTextExtent( sdData.szSignalWord );

	// Zuerst zeichnen wir das Signal-Wort, 90°C nach links gedreht
	dc.DrawRotatedText( sdData.szSignalWord, pt.x, pt.y + m_iHeight - ( m_iHeight - sizeSignal.GetWidth() ) / 2, 90 );

	// Anzahl der Symbole
	int iCount = sdData.szGHSSymbols.size();

	// Dann zeichnen wir hier die Symbole, denn mit den Zahlen kann niemand was anfangen
	for ( int i = 0; i < iCount; ++i )
	{
		// Index des Bildes
		long index = 0;

		// Index holen
		wxString(sdData.szGHSSymbols[i]).ToLong(&index);

		// 1 abziehen
		index--;

		// Index überprüfen
		if ( (index < 0) || (index > 8) )
			continue;

		// ein Image-Objekt anlegen
		wxImage img = picSmall[index];

		// Bild nicht ok?
		if ( !img.IsOk() )
			continue;

		// daraus ein Bitmap machen
		wxBitmap bmp( img );

		// Der Abstand eines Symbols von der oberen Kante,
		// jedes zweite Symbol wird etwas nach unten gerückt
		int iIndentY = (m_iHeight - (bmp.GetHeight()*1.5)) / 2 + ( (i%2)==0 ? 0 : (bmp.GetHeight()/2) );

		// Und zeichnen
		dc.DrawBitmap( bmp, pt.x + sizeSignal.GetHeight() + (i*bmp.GetWidth())/2, pt.y + iIndentY, true );
	}

	// Schrift zurücksetzen
	dc.SetFont( fontTemp );

	// Und zurück
	return m_iWidth;
}

// Druckt den Spalteninhalt von der angegebenen Substanz
int CBerylliumViewSafetySheetsColumnGHSSymbols::PrintContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Keine Symbole oder kein Signalwort? Dann raus hier!
	if ( (sdData.szSignalWord.length() == 0) || (sdData.szGHSSymbols.size() == 0) )
		return m_iWidth;

	// Kleiner Abstand
	pt.x +=2; pt.y +=2;

	// Breite und Höhe
	int iNewWidth	= m_iWidth - 4;
	int iNewHeight	= m_iHeight - 4;

	// Skalierungsfaktoren
	double dscalex = 1.0, dscaley = 1.0;

	// Skalierung vom DC holen
	dc.GetUserScale( &dscalex, &dscaley );

	// Setzen der Skalierung auf 1.0
	dc.SetUserScale( 1.0, 1.0 );

	// MemoryDC erstellen, das kompatibel mit dc ist
	wxMemoryDC memDC;

	// Bitmap erstellen (entsprechend skalieren)
	wxBitmap   memBitmap( iNewWidth * dscalex, iNewHeight * dscaley );

	// Bitmap in den MemDC selektieren
	memDC.SelectObject( memBitmap );

	// Hintergrund
	memDC.SetBackground( *wxWHITE_BRUSH );
	memDC.Clear();

	// Schrift holen (vor Reskalierung)
	wxFont fontTemp = dc.GetFont();

	// Schrift "fett" machen
	fontTemp.SetWeight( wxFONTWEIGHT_BOLD );

	// Schrift skalieren
	fontTemp.SetPointSize( fontTemp.GetPointSize() * dscalex );

	// Schrift in MemDC selektieren
	memDC.SetFont( fontTemp );

	// Schriftgröße ermitteln
	wxSize sizeSignal = memDC.GetTextExtent( sdData.szSignalWord );

	// Zuerst zeichnen wir das Signal-Wort, 90°C nach links gedreht
	memDC.DrawRotatedText( sdData.szSignalWord, 0, (iNewHeight*dscaley) - ( (iNewHeight*dscaley) - sizeSignal.GetWidth() ) / 2, 90 );

	// Anzahl der Symbole
	int iCount = sdData.szGHSSymbols.size();

	// Skalierung der Bilder (Höhe des Piktogrammes ist 3/5 [=0.6] der Höhe der Spalte)
	int iscaletoheight = ( (double)(iNewHeight * (0.6f) ) * dscaley );
	// Änderung vom 4.4.2012: Da Höhe und Breite quasi gleich sind, einfach übernehmen.
	int iscaletowidth = iscaletoheight; //( (double)(picBigHeight / ((double)(iNewHeight * (0.6f) ) * dscaley )) ) * picBigWidth;

	// Dann zeichnen wir hier die Symbole, denn mit den Zahlen kann niemand was anfangen
	for ( int i = 0; i < iCount; ++i )
	{
		// Index des Bildes
		long index = 0;

		// Index holen
		wxString(sdData.szGHSSymbols[i]).ToLong(&index);

		// 1 abziehen
		index--;

		// Index überprüfen
		if ( (index < 0) || (index > 8) )
			continue;

		// Skalieren
		wxImage img = picBig[index].Scale( iscaletowidth, iscaletoheight,wxIMAGE_QUALITY_HIGH );

		// daraus ein Bitmap machen
		wxBitmap bmp( img );

		// Der Abstand eines Symbols von der oberen Kante,
		// jedes zweite Symbol wird etwas nach unten gerückt
		int iIndentY = ((iNewHeight*dscaley) - (bmp.GetHeight()*1.5)) / 2 + ( (i%2)==0 ? 0 : (bmp.GetHeight()/2) );

		// Und zeichnen
		memDC.DrawBitmap( bmp, sizeSignal.GetHeight() + (i*bmp.GetWidth())/2, iIndentY, true );
	}

	// Bitmap aus dem MemDC rausholen
	memDC.SelectObject( wxNullBitmap );

	// Bitmap in DC zeichnen
	dc.DrawBitmap( memBitmap, pt.x * dscalex, pt.y * dscaley );

	// Skalierung zurücksetzen
	dc.SetUserScale( dscalex, dscaley );

	// Zurück
	return m_iWidth;
}

// Spalte für Toxikologie
int CBerylliumViewSafetySheetsColumnToxicology::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Zeichnen von MAK/LD50-Wert
	dc.DrawLabel( sdData.GetSmallestToxicologicAmount(), wxRect( pt.x, pt.y + 5, m_iWidth, m_iHeight / 2), wxALIGN_CENTER );
	
	// Wassergefährdungsklasse zeichnen
	if ( sdData.iWGK == 0 )
		dc.DrawLabel( "-", wxRect( pt.x, pt.y - 5 + (m_iHeight/2), m_iWidth, m_iHeight / 2), wxALIGN_CENTER );
	else
		dc.DrawLabel( wxString::Format(_(L"WGK %d"), sdData.iWGK), wxRect( pt.x, pt.y - 5 + (m_iHeight/2), m_iWidth, m_iHeight / 2), wxALIGN_CENTER );

	// Und zurück...
	return m_iWidth;
}

int CBerylliumViewSafetySheetsColumnAnsatz::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Ansatzgröße und Stoffmenge
	double batchamount = sdData.GetBatchAmount();
	double substamount = sdData.CalculateAmountOfSubstance();

	// Zeichne erst die "richtige" Einheit
	wxString unit	= "L"; if ( sdData.GetBatchType() == sdData.batchSolid ) unit = "g";
	wxString preunit = BeTool::UnitPrefix( batchamount );

	if ( batchamount > 0.0 )
		dc.DrawLabel( wxString::Format(L"%.1f %s%s", batchamount, preunit.c_str(), unit.c_str()), wxRect( pt.x, pt.y + 5, m_iWidth, m_iHeight / 2), wxALIGN_CENTER );

	// Zeichne Stoffmenge
	preunit = BeTool::UnitPrefix( substamount );

	if ( substamount > 0.0 )
		dc.DrawLabel( wxString::Format(L"%.1f %smol", substamount, preunit.c_str()) , wxRect( pt.x, pt.y - 5 + (m_iHeight/2), m_iWidth, m_iHeight / 2) , wxALIGN_CENTER );

	return m_iWidth;
}

// Zeichnet die Gefährlichkeitspunkte-Spalte
int CBerylliumViewSafetySheetsColumnDangerPoints::DrawContent( wxDC &dc, wxPoint &pt, const CBerylliumSubstanceData &sdData )
{
	// Font speichern
	wxFont fontTemp = dc.GetFont();

	// Kopie erstellen
	wxFont fontCopy = fontTemp;

	// Schrift auf Fett setzen
	fontCopy.SetWeight( wxFONTWEIGHT_BOLD );

	// Schrift setzen
	dc.SetFont( fontCopy );

	// Punkte zeichnen
	dc.DrawLabel( wxString::Format("%d", sdData.GetDangerPoints()) , wxRect(pt.x, pt.y, m_iWidth, m_iHeight), wxALIGN_CENTER );
	
	// Schrift wiederherstellen
	dc.SetFont( fontTemp );

	return m_iWidth;
}
