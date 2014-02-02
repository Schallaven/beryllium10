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

#include "BerylliumViewSafetySheets.h"
#include "../berylliummainframe_identifiers.h"

#include "../BerylliumApplication.h"

#include "BerylliumViewSafetySheetsPrinting.h"

#include "../Dialoge/BerylliumViewSafetySheetsProperties.h"
#include "../Dialoge/BerylliumSafetySheetsGeneral.h"
#include "../Dialoge/BerylliumSubstanceSearchDialog.h"
#include "../Dialoge/BerylliumSubstanceProperties.h"
#include "../Dialoge/BerylliumStatementEditor.h"
#include "../Dialoge/BerylliumPhraseSelector.h"

// Bitmaps
#include "../../Resources/Bitmaps/icon_up.xpm"
#include "../../Resources/Bitmaps/icon_down.xpm"

#include <wx/choicdlg.h>

CBerylliumViewSafetySheets::CBerylliumViewSafetySheets( CBerylliumDocument &doc, wxWindow &parent ) : m_Document(doc), m_Parent(parent)
{
	// Aktuelle Seite
	m_iPage = 0;

	// Wir zeigen standardmäßig die R-/S-Sätze
	bShowHPStatements = false;

	// Spalten erstellen
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnName( _(L"eingesetzte Stoffe und\nProdukt(e)"), -1) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnMG( _(L"MG\n[g/mol]"), 65) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnBoilingmelting( _(L"Sdp.\nSmp."), 65) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnFlashpoint( _(L"Fp"), 45) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnHarzardsymbols( _(L"Gefahren-\nsymbol(e)"), 120) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnGHSSymbols( _(L"GHS-Symbol(e)"), 105) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnSafetyphrases( _(L"Nummern der R/S-\nSätze"), 110) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnHarzardphrases( _(L"Nummern der H/P-\nSätze"), 110) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnToxicology( _(L"MAK, LD50\nWGK"), 70) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnAnsatz( _(L"für Ansatz\nbenötigt"), 75) );
	m_Columns.push_back( new CBerylliumViewSafetySheetsColumnDangerPoints( _(L"Be¹º-\nPunkte"), 65) );

	// Spalte Fp verstecken
	m_Columns[COL_FLASHPOINT]->Hide();

	// Spalte GHS-Symbol(e) verstecken
	m_Columns[COL_GHSSYMBOLS]->Hide();

	// Spalte H/P-Sätze verstecken
	m_Columns[COL_HARZARDPRECAUTION]->Hide();

	// Spalte "Be10-Punkte" verstecken
	m_Columns[COL_DANGERPOINTS]->Hide();

	// Seiten assozieren
	m_Columns[COL_NAME]->SetPage( bePAGE_SUBSTANCE_GENERAL );
	m_Columns[COL_MOLWEIGHT]->SetPage( bePAGE_SUBSTANCE_GENERAL );
	m_Columns[COL_MELTINGBOILING]->SetPage( bePAGE_SUBSTANCE_PHYSICAL );
	m_Columns[COL_FLASHPOINT]->SetPage( bePAGE_SUBSTANCE_PHYSICAL );
	m_Columns[COL_HARZARDSYMBOLS]->SetPage( bePAGE_SUBSTANCE_SAFETY );
	m_Columns[COL_GHSSYMBOLS]->SetPage( bePAGE_SUBSTANCE_GHS );
	m_Columns[COL_RISKSAFETIES]->SetPage( bePAGE_SUBSTANCE_SAFETY );
	m_Columns[COL_HARZARDPRECAUTION]->SetPage( bePAGE_SUBSTANCE_GHS );
	m_Columns[COL_TOXICOLOGY]->SetPage( bePAGE_SUBSTANCE_TOXICOLOGY );
	m_Columns[COL_BATCH]->SetPage( bePAGE_SUBSTANCE_BATCH );
	m_Columns[COL_DANGERPOINTS]->SetPage( bePAGE_SUBSTANCE_GENERAL );

	// Spaltengrößen berechnen
	ColumnsCalculate();

	// Es wurde noch nichts ausgewählt
	m_iSelected = 0;
	m_iSubselected = 0;
	m_iColumnSelected = 0;

	// GHS-Modus ausgeschaltet
	bGHSMode = false;

	// Wir drucken auch nicht oder sind im Vorschaumodus
	m_isPrinting = false;
	m_isPreview = false;
}

CBerylliumViewSafetySheets::~CBerylliumViewSafetySheets(void)
{
	// Alle Ansichten löschen
	for ( unsigned int i = 0; i < m_Columns.size(); i++ )
		delete m_Columns[i];

	// Ausleeren
	m_Columns.clear();
}

// Zeichnet die aktuelle Seite
void CBerylliumViewSafetySheets::Draw(wxDC &dc)
{
	// Druckmodus?
	if ( m_isPrinting )
		return;

	// Vorder- oder Rückseite?
	int iBackSheet = (m_iPage % 2);

	// Vorderseite zeichnen
	if ( iBackSheet == 0 )
		DrawFrontSheet( dc, m_iPage );

	// ansonsten Rückseite zeichnen
	else
		DrawBackSheet( dc, m_iPage );
}

// Gibt die Größe einer Seite zurück
wxSize CBerylliumViewSafetySheets::GetPageSize()
{
	// alle Seiten sind bei dieser Ansicht gleich groß, also brauchen wir
	// nur eine größe zurückgeben
	return wxSize( 740, 1040 );
}

// Gibt die Anzahl der Seiten zurück
unsigned int CBerylliumViewSafetySheets::GetNumberOfPages()
{
	// Daten holen
	const CBerylliumData data = m_Document.GetData();

	// Auf eine Seite passen 9 Substanzen. Für jede 9 Substanzen brauchen wir allerdings 2 Seiten
	return ((data.sdCompounds.size() / 9) + 1) * 2;
}

// Setzt die aktuelle Seite
void CBerylliumViewSafetySheets::SetPageNumber( unsigned int iPage )
{
	// Index der letzten Seite ermitteln
	unsigned int iLastPage = (GetNumberOfPages() - 1);

	// Ist iPage größer als iLastPage? Dann auf letzte Seite setzen
	if ( iPage > iLastPage )
		iPage = iLastPage;

	// Seite setzen...
	m_iPage = iPage;

	// Alle Observer informieren...
	m_Document.NotifyAllObserver();
}

// Zeichnet die Vorderseite
void CBerylliumViewSafetySheets::DrawFrontSheet( wxDC &dc, unsigned int iPage )
{
	// Hintergrund weißeln...
	dc.SetPen( wxPen( wxColour(255,255,255), 1, wxSOLID ) );
	dc.SetBrush( wxBrush( wxColour(255,255,255) ) );
	dc.DrawRectangle( 0, 0, 740, 1040 );

	// Zeichnen der Markierung (außerhalb des Druckmodus)
	if ( !m_isPrinting )
		DrawFrontSheetSelection(dc);

	// Rahmen zeichnen
	DrawFrontSheetBorder(dc);

	// Werte setzen
	dc.SetTextForeground( wxColour(0,0,0) );
	dc.SetBackgroundMode( wxTRANSPARENT );

	// Kopfdaten einfügen
	DrawFrontSheetHead( dc, iPage );

	// Personendaten einfügen
	DrawFrontSheetPersonal( dc, iPage );

	// Substanzdaten einfügen
	DrawFrontSheetSubstances( dc, iPage );

	// R-/S- bzw P-/H-Sätze einfügen
	DrawFrontSheetWarnings( dc, iPage );
}

// Zeichnet nur die Auswahlhintergründe
void CBerylliumViewSafetySheets::DrawFrontSheetSelection( wxDC &dc )
{
	// Druckmodus?
	if ( m_isPrinting )
		return;

	// Auswahlindex muss unter 100 liegen
	if ( m_iSelected > 100 )
		return;

	// Pinsel erstellen (Blauer Hintergrund)
	wxBrush brushSelection1( wxColour( 10, 36, 106 ) );

	// Pinsel erstellen (Gelber Hintergund)
	wxBrush brushSelection2( wxColour( 255, 255, 135 ) );

	// Pinsel erstellen (hellblauer Hintergund)
	wxBrush brushSelection3( wxColour( 100, 150, 255) );

	// Alten Pinsel aufheben
	wxBrush oldBrush = dc.GetBrush();

	// Pinsel wählen
	dc.SetBrush( brushSelection1 );

	// Welche Auswahl muss gezeichnet werden?
	switch( m_iSelected )
	{
		// Titelzeile 1
		case 1:
			dc.DrawRectangle( 20, 20, 700, 42 );
			break;

		// Titelzeile 2
		case 2:
			dc.DrawRectangle( 20, 60, 700, 42 );
			break;

		// Titelzeile 3
		case 3:
			dc.DrawRectangle( 20, 100, 700, 42 );
			break;

		// Name(n)
		case 4:
			dc.DrawRectangle( 20, 140, 302, 52 );
			break;

		// Platz
		case 5:
			dc.DrawRectangle( 320, 140, 152, 52 );
			break;

		// Assistent
		case 6:
			dc.DrawRectangle( 470, 140, 250, 52 );
			break;

		// Präparat
		case 7:
			dc.DrawRectangle( 20, 190, 700, 67 );
			break;

		// Spaltenköpfe
		case 8:
			dc.SetBrush( brushSelection2 );
			dc.DrawRectangle( 20, 255, 700, 52 );
			break;

		// Substanzblock
		case 9:
			// Zeichnet den Block mit dem entsprechenden Subindex
			if ( m_iSubselected > 8 )
				m_iSubselected = 0;

			// Block für die Substanz zeichen
			dc.DrawRectangle( 20, 305 + (m_iSubselected*50), 700, 52 );

			// Spaltenindex verfügbar UND wir sind NICHT in der leeren Zeile?
			if ( (m_iColumnSelected < m_Columns.size()) && (m_iSubselected < m_Document.GetData().sdCompounds.size()) )
			{
				// Spalten x und Breite ermitteln
				int x = 20;

				// Spalten durchgehen bis m_iColumnSelected und Breiten aufaddieren
				for ( unsigned int i = 0; i < m_iColumnSelected; ++i )
					if ( m_Columns[i]->IsShown() )
						x += m_Columns[i]->GetColumnWidth();

				// Pinsel austauschen
				dc.SetBrush( brushSelection3 );

				// Zeichnen
				dc.DrawRectangle( x, 305 + (m_iSubselected*50), m_Columns[m_iColumnSelected]->GetColumnWidth()+1, 52 );
			}

			break;

		// Wortlaut der ...
		case 10:
			dc.SetBrush( brushSelection2 );
			dc.DrawRectangle( 20, 755, 700, 36 );
			break;

		// R-/S-Sätze-Block
		case 11:
			dc.DrawRectangle( 20, 790, 700, 230 );
			break;

	};

	// Alten Pinsel wieder herstellen
	dc.SetBrush(oldBrush);
}

// Zeichnet nur den Rahmen für die Vorderseite
void CBerylliumViewSafetySheets::DrawFrontSheetBorder( wxDC &dc )
{
	// Außenrahmen zeichnen...
	// -----------------------
	// Stifte vorbereiten wählen
	wxPen penThick( wxColour(0,0,0), 2, wxSOLID );

	// Stift wählen
	dc.SetPen( penThick );

	// Alter Pinsel
	wxBrush oldBrush = dc.GetBrush();

	// Pinsel wählen
	dc.SetBrush( *wxTRANSPARENT_BRUSH );

	// Rahmen zeichnen
	dc.DrawRectangle( 20, 20, 700, 1000 );

	// Alten Pinsel wiederherstellen
	dc.SetBrush(oldBrush);

	// Stift vorbereiten
	wxPen penThin( wxColour(0,0,0), 1, wxSOLID );

	// Stift wählen
	dc.SetPen( penThin );

	// Die drei Titelleisten zeichnen...
	// ---------------------------------
	dc.DrawLine( 20, 60,  720, 60 );
	dc.DrawLine( 20, 100, 720, 100 );
	dc.DrawLine( 20, 140, 720, 140 );

	// Die Blöcke für Namen, etc. zeichnen
	// -----------------------------------
	dc.DrawLine( 20, 190, 720, 190 );

	// Dieser Abschnitt war ursprünglich mal dafür gedacht, Vorname und Name zu trennen. Aber es gibt nur noch ein
	// gemeinsames Namensfeld.
	//if ( data.szVorname.length() > 0 )
	//	dc.MoveTo( (int)(150 * fScaleX), (int)(120*fScaleY) ); dc.LineTo( (int)(150 * fScaleX), (int)(170*fScaleY) );

	dc.DrawLine( 320, 140, 320, 190 );
	dc.DrawLine( 470, 140, 470, 190 );

	// Abschnitt für die R-/S- bzw. P-/H- Sätze
	// ----------------------------------------
	dc.DrawLine( 20, 790, 720, 790 );
	dc.DrawLine( 370, 790, 370, 1020 );

	// Linien für Substanzen
	// ---------------------
	dc.DrawLine( 20, 255, 720, 255 );

	// für die einzelnen Substanzen eine horizontale Linie zeichnen
	for ( unsigned int i = 0; i < 10; i++ )
		dc.DrawLine( 20, 255 + (i+1)*50, 720, 255 + (i+1)*50 );

	// Spalten zeichnen
	// ----------------
	int iPosition = 20;
	int iLastIndex = m_Columns.size()-1;

	// Letzte Spalte suchen, die noch angezeigt wird
	for ( int i = (m_Columns.size()-1); i > -1; i-- )
	{
		if ( m_Columns[i]->IsShown() )
		{
			iLastIndex = i;
			break;
		}
	}

	for ( int i = 0; i < (iLastIndex); i++ )
	{
		// Spalten die versteckt sind, ignorieren...
		if ( m_Columns[i]->IsShown() == false )
			continue;

		dc.DrawLine( iPosition + m_Columns[i]->GetColumnWidth(), 255, iPosition + m_Columns[i]->GetColumnWidth(), 755 );
		iPosition += m_Columns[i]->GetColumnWidth();
	}
}

// Vorderseite: Zeichnet die Kopfdaten
void CBerylliumViewSafetySheets::DrawFrontSheetHead( wxDC &dc, unsigned int iPage )
{
	// Große Schrift erstellen, für die 3 Titelzeilen
	wxFont fontBig( 15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Schrift selektieren
	dc.SetFont( fontBig );

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// 1. Zeile: Titel
	if ( (!m_isPrinting) && (m_iSelected == 1) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Titel zeichnen (je nachdem ob GHS oder nicht)
	if ( IsGHSMode() )
		dc.DrawLabel( data.szGHSTitle, wxRect( 22, 22, 700, 40 ), wxALIGN_CENTER );
	else
		dc.DrawLabel( data.szTitle, wxRect( 22, 22, 700, 40 ), wxALIGN_CENTER );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// 2. Zeile: Ort
	if ( (!m_isPrinting) && (m_iSelected == 2) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Ort zeichnen
	dc.DrawLabel( data.szLocation, wxRect( 22, 62, 700, 40 ), wxALIGN_CENTER );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// 3. Zeile: Veranstaltung
	if ( (!m_isPrinting) && (m_iSelected == 3) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Titel zeichnen
	dc.DrawLabel( data.szEvent, wxRect( 22, 102, 700, 40 ), wxALIGN_CENTER );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );
}

// Vorderseite: Zeichnet die Personendaten
void CBerylliumViewSafetySheets::DrawFrontSheetPersonal( wxDC &dc, unsigned int iPage )
{
	// Schrift 1: Normal
	wxFont fontNormal( 11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift 2: Fett
	wxFont fontBold( 11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// "Name(n)" - Kästchen zeichnen
	// -----------------------------
	if ( (!m_isPrinting) && (m_iSelected == 4) )		dc.SetTextForeground( wxColour(255,255,255) );
	dc.SetFont( fontNormal );	dc.DrawText( _(L"Name(n)"), 25, 145 );
	dc.SetFont( fontBold );		dc.DrawText( data.GetName(), 25, 170 );
	dc.SetTextForeground( wxColour(0,0,0) );

	// "Platz" zeichnen
	// ----------------
	if ( (!m_isPrinting) && (m_iSelected == 5) )		dc.SetTextForeground( wxColour(255,255,255) );
	dc.SetFont( fontNormal );	dc.DrawText( _(L"Platz"), 325, 145 );
	dc.SetFont( fontBold );		dc.DrawText( data.szPosition, 325, 170 );
	dc.SetTextForeground( wxColour(0,0,0) );

	// "Assistent/in" zeichnen
	// ---------------------
	if ( (!m_isPrinting) && (m_iSelected == 6) )		dc.SetTextForeground( wxColour(255,255,255) );
	dc.SetFont( fontNormal );	dc.DrawText( _(L"Assistent/in"), 475, 145 );
	dc.SetFont( fontBold );		dc.DrawText( data.szSupervisor, 475, 170 );
	dc.SetTextForeground( wxColour(0,0,0) );
}

// Vorderseite: Zeichnet die Substanzdaten
void CBerylliumViewSafetySheets::DrawFrontSheetSubstances( wxDC &dc, unsigned int iPage )
{
	// Schrift 1: Normal
	wxFont fontNormal( 11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift 2: Fett
	wxFont fontBold( 11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Schrift 3: Klein
	wxFont fontSmall( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Bitmaps erstellen
	wxBitmap bmpUp(icon_up_xpm);
	wxBitmap bmpDown(icon_down_xpm);

	// Texte erstellen (Normal), Herzustellende Präparat, Ansatzgröße, Mol, Name
	// =========================================================================
	if ( (!m_isPrinting) && (m_iSelected == 7) )		dc.SetTextForeground( wxColour(255,255,255) );

	dc.SetFont( fontNormal );	dc.DrawText( _(L"Herzustellendes Präparat:"), 30, 200 );
	dc.SetFont( fontBold );

	// Namen holen
	wxString szPrepName = m_Document.GetData().sdPreparation.GetName();

	// Formel vorhanden? (aber nur bei vorhandenem Namen Komma einfügen)
	if ( m_Document.GetData().sdPreparation.HasFormula() && (szPrepName.length() > 0) )
		szPrepName += ", ";

	// Dimensionen ermitteln
	wxSize sizeText = dc.GetTextExtent( szPrepName );

	// Text zeichnen
	dc.DrawText( szPrepName, 20 + 20, 225 );

	// Evtl. Formel zeichen
	if ( m_Document.GetData().sdPreparation.HasFormula() )
		m_Document.GetData().sdPreparation.DrawFormula( dc, 20 + 20 + sizeText.GetWidth(), 225 );

	// Schriftfarbe zurücksetzen
	dc.SetTextForeground( wxColour(0,0,0) );

	// ersten Substanzindex der aktuellen Seite berechnen
	unsigned int iSubstanceIndex = ( m_iPage / 2 ) * 9;

	// Pfeile für Substanzen malen (falls nicht gedruckt wird bzw. falls kein Vorschaumodus)
	// =====================================================================================
	if (!m_isPrinting && !m_isPreview && (m_Document.GetData().sdCompounds.size() > 0))
	{
		// Berechnen
		unsigned int iLastSubstanceIndex =  m_Document.GetData().sdCompounds.size() - 1;

		// Substanzen durchgehen
		for ( unsigned int j = iSubstanceIndex; j <= iLastSubstanceIndex; ++j )
		{	
			// Erste Substanz bekommt keinen Hoch-Pfeil
			if ( j != 0 )
				dc.DrawBitmap( bmpUp, 0, 255+50+(j-iSubstanceIndex)*50 + 9, true );

			// Letzte Substanz bekommt keinen Down-Pfeil
			if ( j != iLastSubstanceIndex )
				dc.DrawBitmap( bmpDown, 0, 255+50+(j-iSubstanceIndex)*50 + 9 + 16 , true );
		}
	}

	// Spalten anzeigen
	// ================
	dc.SetFont( fontSmall );

	// Startposition für die erste Spalte
	int iPosition = 20;

	// Spaltenköpfe nacheinander zeichnen. Dabei werden die versteckten Spalten
	// natürlich ignoriert...
	for ( unsigned int i = 0; i < m_Columns.size(); i++ )
	{
		// Spalten die versteckt sind, ignorieren...
		if ( m_Columns[i]->IsShown() == false )
			continue;

		// Spaltenkopf zeichnen
		wxPoint columnpoint(iPosition, 255);
		m_Columns[i]->DrawHeader( dc, columnpoint );		

		// Inhalte zeichnen
		for ( unsigned int j = iSubstanceIndex; j < m_Document.GetData().sdCompounds.size(); j++ )
		{
			// Ist die aktuelle Substanz "markiert"? Dann natürlich die Schriftfarbe ändern!
			if ( (!m_isPrinting) && (m_iSelected == 9) && (j == m_iSubselected) )
				dc.SetTextForeground( wxColour(255,255,255) );

			// Zeichnen
			wxPoint contentpoint(iPosition, 255+50+(j-iSubstanceIndex)*50);
			m_Columns[i]->DrawContent( dc, contentpoint, m_Document.GetData().sdCompounds[j] );

			// Schriftfarbe immer neu setzen
			dc.SetTextForeground( wxColour(0,0,0) );

			// Nach der 9. Substanz (Index 8) ist aber Schluss
			if ( j == (iSubstanceIndex + 8) )
				break;
		}

		// Breite aufaddieren
		iPosition += m_Columns[i]->GetColumnWidth();
	}

	return;
}

// Vorderseite: Zeichnet die Warnhinweise
void CBerylliumViewSafetySheets::DrawFrontSheetWarnings( wxDC &dc, unsigned int iPage )
{
	// Schrift 1: Normal
	wxFont fontNormal( 11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift setzen
	dc.SetFont( fontNormal );

	// Breite und Höhe des Textes bestimmen
	wxCoord width = 0, height = 0;

	// Wortlaut der wesentlich oben genannten R- und S-Sätze...
	wxString statements = _(L"Wortlaut der wesentlichen oben genannten R- und S-Sätze:");
	if ( bShowHPStatements )
		statements = _(L"Wortlaut der wesentlichen oben genannten H- und P-Sätze:");

	// Zeichnen
	dc.GetTextExtent(statements, &width, &height );
	dc.DrawText(statements, 20 + ( 700 - width ) / 2, 755 + ( 40 - height ) / 2 );

	// Zwei Seiten gehören immer zusammen
	iPage = iPage / 2;

	// Sätze überhaupt vorhanden?
	for ( unsigned int i = 0; i < 8; i++ )
		if ( m_statements[i].size() <= iPage )
			return;

	// Präfixe
	wxString sLeft = "R";
	wxString sRight = "S";

	// Linke Liste besteht entweder aus R-Sätzen oder H-Sätzen
	CBerylliumStatementsManager listLeft	= m_statements[STATEMENT_RISKS][iPage];

	// Rechte Liste bsteht entweder aus S-Sätzen oder P-Sätzen
	CBerylliumStatementsManager listRight	= m_statements[STATEMENT_SAFETIES][iPage];

	if ( bShowHPStatements )
	{
		listLeft  = m_statements[STATEMENT_HARZARDS][iPage]; sLeft = "H";
		listRight = m_statements[STATEMENT_PRECAUTIONARIES][iPage]; sRight = "P";
	}

	// Block markiert
	if ( (!m_isPrinting) && (m_iSelected == 11) )
		dc.SetTextForeground( wxColour(255,255,255) );
	else
		dc.SetTextForeground( wxColour(0,0,0) );

	// Linken Block zeichnen
	DrawFrontSheetWarningsBlock( dc, listLeft, sLeft, wxRect( 25, 795, 340, 220) );

	// Rechten Block zeichnen
	DrawFrontSheetWarningsBlock( dc, listRight, sRight, wxRect( 375, 795, 690, 220) );
}

// Vorderseite: R-/S-Sätze-Block
void CBerylliumViewSafetySheets::DrawFrontSheetWarningsBlock( wxDC &dc, const CBerylliumStatementsManager &slist, wxString &sPrefix, wxRect rect )
{
	// Anzahl der Statements
	unsigned int iNumberOfStatements = slist.GetSize();

	// Maximales y
	int maxy = rect.y + rect.height - 5;

	// Jedes Statement zeichnen
	for ( unsigned int i = 0; i < iNumberOfStatements; i++ )
	{
		// Zeichnen
		rect.y += DrawFrontSheetWarningsSentence( dc, slist.GetStatementByIndex(i), sPrefix, rect );

		// Wenn wir über den unteren Rand sind, dann raus hier
		if ( rect.y >= maxy )
			return;
	}
}

// Vorderseite: R-/S-Sätze-Satz
int CBerylliumViewSafetySheets::DrawFrontSheetWarningsSentence( wxDC &dc, const CBerylliumStatement &sStatement, wxString &sPrefix, wxRect rect )
{
	// Einrückung der Sätze...
	const int iIndentX = 25;

	// Schrift 1: Klein
	wxFont fontSmall( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift 2: Klein, Fett
	wxFont fontSmallBold( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Das Statement in Satzfragmente in eine Liste schreiben
	std::vector< wxString > sWords; BeTool::StringToList( sStatement.GetStatement(), " ", sWords );

	// Anzahl der Wörter
	unsigned int iNumberOfWords = sWords.size();

	// Fette Schrift wählen
	dc.SetFont( fontSmallBold );

	// Position x,y
	int x = rect.x, y = rect.y;

	// Größe
	wxSize size = dc.GetTextExtent( sPrefix + sStatement.GetToken() );

	// Token zeichnen
	dc.DrawText( sPrefix + sStatement.GetToken(), x, y );

	// Einrücken...
	x += size.GetWidth() + 5;

	// Schrift wechseln
	dc.SetFont( fontSmall );

	// Wort für Wort zeichnen (+ Leerzeichen)
	for ( unsigned int i = 0; i < iNumberOfWords; i++ )
	{
		// Größe holen
		size = dc.GetTextExtent( " " + sWords[i] );

		// Passt das Wort noch in die Zeile? Wenn nicht, dann neue Zeile!
		if ( (x+size.GetWidth()) > rect.width )
		{
			// Position x zurücksetzen
			x = rect.x + iIndentX;

			// Position y erhöhen
			y += size.GetHeight();
		}

		// Wort zeichnen
		dc.DrawText( " " + sWords[i], x, y );

		// x erhöhen
		x += size.GetWidth();
	}


	// Position + Größe der letzten Zeile zurückgeben
	return y - rect.y + size.GetHeight();
}

// Zeichnet die Rückansicht
void CBerylliumViewSafetySheets::DrawBackSheet(wxDC &dc, unsigned int iPage)
{
	// Hintergrund weißeln...
	dc.SetPen( wxPen( wxColour(255,255,255), 1, wxSOLID ) );
	dc.SetBrush( wxBrush( wxColour(255,255,255) ) );
	dc.DrawRectangle( 0, 0, 740, 1040 );

	// Zeichnen der Markierung (nicht beim Drucken!)
	if ( !m_isPrinting )
		DrawBackSheetSelection(dc);

	// Rahmen zeichnen lassen
	DrawBackSheetBorder( dc );

	// Werte setzen
	dc.SetTextForeground( wxColour(0,0,0) );
	dc.SetBackgroundMode( wxTRANSPARENT );

	// Text zeichnen lassen
	DrawBackSheetLabels( dc, iPage );

	// Daten zeichnen lassen
	DrawBackSheetData( dc, iPage );
}

// Zeichnet die Auswahl (falls vorhanden)
void CBerylliumViewSafetySheets::DrawBackSheetSelection( wxDC &dc )
{
	// Druckmodus?
	if ( m_isPrinting )
		return;

	// Auswahlindex muss über 100 liegen
	if ( m_iSelected < 100 )
		return;

	// Pinsel erstellen (Blauer Hintergrund)
	wxBrush brushSelection1( wxColour( 10, 36, 106 ) );
	//wxBrush brushSelection1( wxColour( 0, 200, 255 ) );

	// Pinsel erstellen (Gelber Hintergund)
	//wxBrush brushSelection2( wxColour( 255, 255, 225 ) );
	wxBrush brushSelection2( wxColour( 255, 255, 135 ) );

	// Alten Pinsel aufheben
	wxBrush oldBrush = dc.GetBrush();

	// Pinsel wählen
	dc.SetBrush( brushSelection1 );

	// Welche Auswahl muss gezeichnet werden?
	switch( m_iSelected )
	{
		// Gefahren für Mensch und Umwelt
		case 101:
			dc.DrawRectangle( 20, 20, 700, 212 );
			break;

		// Schutzmaßnahmen
		case 102:
			dc.DrawRectangle( 20, 230, 700, 212 );
			break;

		// Verhalten im Gefahrenfall
		case 103:
			dc.DrawRectangle( 20, 440, 700, 212 );
			break;

		// Entsorgung
		case 104:
			dc.DrawRectangle( 20, 650, 700, 212 );
			break;

		// "Hiermit verpflichte ich mich..."
		case 105:
			dc.SetBrush( brushSelection2 );
			dc.DrawRectangle( 20, 860, 352, 160 );
			break;

		// "Präparat zur Synthese freigegeben..."
		case 106:
			dc.SetBrush( brushSelection2 );
			dc.DrawRectangle( 370, 860, 350, 160 );
			break;
	};

	// Alten Pinsel wieder herstellen
	dc.SetBrush(oldBrush);
}

// Zeichnet nur den Rahmen für die Rückseite
void CBerylliumViewSafetySheets::DrawBackSheetBorder( wxDC &dc )
{
	// Außenrahmen zeichnen...
	// -----------------------
	// Stifte vorbereiten wählen
	wxPen penThick( wxColour(0,0,0), 2, wxSOLID );

	// Stift wählen
	dc.SetPen( penThick );

	// Alter Pinsel
	wxBrush oldBrush = dc.GetBrush();

	// Pinsel wählen
	dc.SetBrush( *wxTRANSPARENT_BRUSH );

	// Rahmen zeichnen
	dc.DrawRectangle( 20, 20, 700, 1000 );

	// Alten Pinsel wählen
	dc.SetBrush( oldBrush );

	// Stift vorbereiten
	wxPen penThin( wxColour(0,0,0), 1, wxSOLID );

	// dünnen Stift wählen
	dc.SetPen( penThin );

	// Horizontale Striche zeichnen
	for ( int i = 0; i < 4; i++ )
		dc.DrawLine( 20, 20 + (i+1)*210, 720, 20 + (i+1)*210);

	// Senkrechten Strich zwischen den Signum-Kästchen zeichnen
	dc.DrawLine( 370, 860, 370, 1020 );
}

// Zeichnet die Texte für die Rückseite
void CBerylliumViewSafetySheets::DrawBackSheetLabels( wxDC &dc, unsigned int iPage )
{
	// Schrift vorbereiten
	wxFont fontSmall( 8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift setzen
	dc.SetFont(fontSmall);

	// Textfarbe setzen
	dc.SetTextForeground( wxColour(0,0,0) );

	// Gefahren für Mensch und Umwelt
	if ( (!m_isPrinting) && (m_iSelected == 101) ) dc.SetTextForeground( wxColour(255,255,255) );
	dc.DrawLabel( _(L"Gefahren für Mensch und Umwelt, die von den Ausgangsmaterialien bzw. dem(n) Produkt ausgehen, soweit sie nicht durch die oben\ngenannten Angaben abgedeckt sind (z.B. krebserregend, fruchtschädigend, hautresorptiv):"), wxRect( 30, 30, 680, 210 ), wxALIGN_LEFT | wxALIGN_TOP);
	dc.SetTextForeground( wxColour(0,0,0) );

	// Schutzmaßnahmen und Verhaltensregeln
	if ( (!m_isPrinting) && (m_iSelected == 102) ) dc.SetTextForeground( wxColour(255,255,255) );
	dc.DrawText( _(L"Schutzmaßnahmen und Verhaltensregeln:"), 30, 240);
	dc.SetTextForeground( wxColour(0,0,0) );

	// Verhalten im Gefahrenfall
	if ( (!m_isPrinting) && (m_iSelected == 103) ) dc.SetTextForeground( wxColour(255,255,255) );
	dc.DrawText( _(L"Verhalten im Gefahrenfall, Erste-Hilfe-Maßnahmen (gegebenenfalls Kopie der entsprechenden Literaturstelle beiheften):"), 30, 450);
	dc.SetTextForeground( wxColour(0,0,0) );

	// Entsorgung
	if ( (!m_isPrinting) && (m_iSelected == 104) ) dc.SetTextForeground( wxColour(255,255,255) );
	dc.DrawText( _(L"Entsorgung:"), 30, 660 );
	dc.SetTextForeground( wxColour(0,0,0) );

	// Hiermit verpflichte ich mich...
	DrawBackSheetLabelBox( dc, wxRect(30,870,330,110), m_Document.GetData().szStatementStudent);

	// Präparat freigegeben...
	DrawBackSheetLabelBox( dc, wxRect(380,870,330,110), m_Document.GetData().szStatementSupervisor);

	// Unterschrift "Student"
	dc.DrawLabel( m_Document.GetData().szSignaturStudent, wxRect(30, 990, 330, 50), wxALIGN_RIGHT|wxALIGN_TOP );

	// Unterschrift "Assistent"
	dc.DrawLabel( m_Document.GetData().szSignaturSupervisor, wxRect(380, 990, 330, 50), wxALIGN_RIGHT|wxALIGN_TOP );
}

// Rückseite: Texte für die Unterschriftenboxen
void CBerylliumViewSafetySheets::DrawBackSheetLabelBox( wxDC &dc, wxRect rect, const wxString &text )
{
	// Das Statement in Satzfragmente in eine Liste schreiben
	std::vector< wxString > sWords; BeTool::StringToList( text, " ", sWords );

	// Anzahl der Wörter
	unsigned int iNumberOfWords = sWords.size();

	// Breite und Höhe der Box anpassen
	rect.width	+= rect.x;
	rect.height += rect.y;

	// Position x,y
	int x = rect.x, y = rect.y;

	// Größe
	wxSize size;

	// Wort für Wort zeichnen (+ Leerzeichen)
	for ( unsigned int i = 0; i < iNumberOfWords; i++ )
	{
		// Größe holen
		size = dc.GetTextExtent( " " + sWords[i] );

		// Passt das Wort noch in die Zeile? Wenn nicht, dann neue Zeile!
		if ( (x+size.GetWidth()) > rect.width )
		{
			// Position x zurücksetzen
			x = rect.x;

			// Position y erhöhen
			y += size.GetHeight();
		}

		// Wort zeichnen
		dc.DrawText( " " + sWords[i], x, y );

		// x erhöhen
		x += size.GetWidth();
	}
}

// Zeichnet die Daten für die Rückseite
void CBerylliumViewSafetySheets::DrawBackSheetData( wxDC &dc, unsigned int iPage )
{
	// Seite halbieren (immer zwei Seiten zusammenfassen)
	iPage = iPage / 2;

	// die einzelnen Blöcke zeichnen
	for ( unsigned int i = 0; i < 4; i++ )
	{
		// Schriftfarbe setzen
		if ( (!m_isPrinting) && (m_iSelected == (101+i)) )
			dc.SetTextForeground( wxColour(255,255,255) );
		else
			dc.SetTextForeground( wxColour(0,0,0) );

		// Absatz (von oben)
		int iIndentY = 50;

		// (substanzabhängige) Statements zeichnen
		if ( m_statements[STATEMENT_DANGERS+i].size() > iPage )
			iIndentY += DrawBackSheetBlockSubstanceStatements( dc, wxRect( 20, 20 + i*210 + iIndentY, 700, 210 - iIndentY), iPage, m_statements[STATEMENT_DANGERS+i][iPage] );

		// allgemeine Statements zeichnen
		DrawBackSheetBlockCommon( dc, wxRect( 20, 20 + i*210 + iIndentY, 700, 210 - iIndentY), iPage, m_Document.GetData().sCustomStatements[i] );
	}
}

// Rückseite: Zeichnet die "substanzabhängigen" Sätze
int CBerylliumViewSafetySheets::DrawBackSheetBlockSubstanceStatements( wxDC &dc, wxRect rect, unsigned int iDoublePage, const CBerylliumStatementsManager &statements )
{
	// Anzahl der Statements
	unsigned int iNumberOfStatements = statements.GetSize();

	// Maximales y
	int maxy = rect.y + rect.height - 5;

	// Ursprüngliche Position von oben
	int iOriginY = rect.y;

	// Jedes Statement zeichnen
	for ( unsigned int i = 0; i < iNumberOfStatements; i++ )
	{
		// Zeichnen
		rect.y += DrawBackSheetSentence( dc, rect, statements.GetStatementByIndex(i).GetStatement(), statements.GetStatementByIndex(i).GetToken() );

		// Wenn wir über den unteren Rand sind, dann raus hier
		if ( rect.y >= maxy )
			return (rect.y - iOriginY);
	}

	// Absatz von oben zurückgeben
	return (rect.y - iOriginY);
}

// Rückseite: Zeichnet die "allgemeinen" Sätze
void CBerylliumViewSafetySheets::DrawBackSheetBlockCommon( wxDC &dc, wxRect rect, unsigned int iDoublePage, const std::vector<wxString> &statements )
{
	// Anzahl der Statements
	unsigned int iNumberOfStatements = statements.size();

	// Maximales y
	int maxy = rect.y + rect.height - 5;

	// Jedes Statement zeichnen
	for ( unsigned int i = 0; i < iNumberOfStatements; i++ )
	{
		// Zeichnen
		rect.y += DrawBackSheetSentence( dc, rect, statements[i], "" );

		// Wenn wir über den unteren Rand sind, dann raus hier
		if ( rect.y >= maxy )
			return;
	}
}

// Rückseite: Zeichnet einen Satz (mit Punkt)
int CBerylliumViewSafetySheets::DrawBackSheetSentence( wxDC &dc, wxRect rect, const wxString &sentence, const wxString &prefix )
{
	// Dies ist ein ganz langer langweiliger Text, den ich mir mal kopieren sollte, damit ich ihn nicht dauernd neu tippen muss... :(

	// Schrift 1: Klein
	wxFont fontSmall( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Schrift 2: Klein, Fett
	wxFont fontSmallBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Das Statement in Satzfragmente in eine Liste schreiben
	std::vector< wxString > sWords; BeTool::StringToList( sentence, " ", sWords );

	// Anzahl der Wörter
	unsigned int iNumberOfWords = sWords.size();

	// Fette Schrift wählen
	dc.SetFont( fontSmallBold );

	// Position x,y
	int x = rect.x + 25, y = rect.y;

	// Punkt zeichnen
	wxSize size = dc.GetTextExtent( L"•" );

	// Punkt zeichnen
	dc.DrawText( "•", x, y );

	// Einrücken...
	x += size.GetWidth() + 5;

	// Der weitere Text wir ab sofort immer so eingerückt
	const int iIndentX = x;

	// Prefix zeichnen, falls vorhanden
	if ( prefix.length() > 0 )
	{
		// Größe
		size = dc.GetTextExtent( prefix );

		// Token zeichnen
		dc.DrawText( prefix, x, y );

		// Einrücken...
		x += size.GetWidth() + 5;
	}

	// Schrift wechseln
	dc.SetFont( fontSmall );

	// Wort für Wort zeichnen (+ Leerzeichen)
	for ( unsigned int i = 0; i < iNumberOfWords; i++ )
	{
		// Größe holen
		size = dc.GetTextExtent( sWords[i] + " " );

		// Passt das Wort noch in die Zeile? Wenn nicht, dann neue Zeile!
		if ( (x+size.GetWidth()) > rect.width )
		{
			// Position x zurücksetzen
			x = iIndentX;

			// Position y erhöhen
			y += size.GetHeight() + 2;
		}

		// Wort zeichnen
		dc.DrawText( sWords[i] + " ", x, y );

		// x erhöhen
		x += size.GetWidth();
	}


	// Position + Größe der letzten Zeile zurückgeben
	return y - rect.y + size.GetHeight() + 2;
}



// Berechnet die Spaltenbreite und setzt diese entsprechend
void CBerylliumViewSafetySheets::ColumnsCalculate()
{
	// Maximale Breite für alle Spalten (Pixel)
	int iWidthMax = 700;

	// Anzahl der variablen Spalten
	int iNoVariables = 0;

	// Jetzt gehen wir alle Spalten durch und ziehen von dem maximalen Wert
	// alles ab, was größer als "-1" ist
	// Gleichzeitig zählen wir die Spalten die auf variabel "-1" gesetzt sind
	for ( unsigned int i = 0; i < m_Columns.size(); i++ )
	{
		// Spalten die versteckt sind, ignorieren...
		if ( m_Columns[i]->IsShown() == false )
			continue;

		// Ansonsten wie oben beschrieben verfahren...
		if ( m_Columns[i]->IsDynamic() == false )
			iWidthMax -= m_Columns[i]->GetColumnWidth();

		else
			iNoVariables++;
	}

	// Gibt es überhaupt keine Variablen Spalten mehr? Dann raus hier!
	if ( iNoVariables == 0 )
		return;

	// Die restlichen Breite teilen wir durch die Anzahl der verbliebenen Spalten
	int iWidthVariable = iWidthMax / iNoVariables;

	// Sollte es einen Rest geben, so wird das bei der allerersten Spalte draufaddiert,
	// damit kein Leerraum entsteht
	int iWidthRemaining = iWidthMax % iNoVariables;

	// Erste Spalte
	bool bFirstToSet = true;

	// Jetzt alle Spalten setzen, deren Größe vorher noch unbestimmt war
	for ( unsigned int i = 0; i < m_Columns.size(); i++ )
	{
		// Spalten die versteckt sind, ignorieren...
		if ( m_Columns[i]->IsShown() == false )
			continue;

		// Spalte hat dynamische Breite?
		if ( m_Columns[i]->IsDynamic() )
		{
			m_Columns[i]->SetColumnWidth( (bFirstToSet) ? (iWidthVariable + iWidthRemaining) : (iWidthVariable) );
			bFirstToSet = false;
		}
	}

}

// Prüft, ob eine Maustaste geklickt wurde...
void CBerylliumViewSafetySheets::OnMouseClick(wxMouseEvent &event)
{
	// Linke Taste unten?
	if ( event.LeftDown() )
	{
		// Auswahl überprüfen
		switch( m_iSelected )
		{
			// Eine der Titelzeilen, der persönlichen Daten oder des Präparates
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				ShowDialogGeneral();
				break;

			// Eine der "gelben" Eigenschaftsboxen
			case 8:		// Spaltenkopf
			case 10:	// Wortlaut der...
			case 105:	// Hiermit verpflichte...
			case 106:	// Präparat zur Synthese freigeben...
				ShowDialogProperties();
				break;

			// In den Substanzblock geklickt?
			case 9:
				ShowDialogSubstance();
				break;

			// R/S bzw H/P-Sätze
			case 11:
				{
					// Seite bestimmen
					unsigned int iPage = 0;

					// Mausposition bestimmen
					wxPoint mouse = event.GetPosition();

					iPage = ( mouse.x < 371 ) ? 0 : 1;

					// bei H/P einfach 2 dazuzählen
					if ( bShowHPStatements )
						iPage += 2;

					// Dialog anzeigen
					ShowDialogPhrases( iPage );
				}
				break;

			// Hoch/Runter-Pfeile (Sortierung)
			case 12:
				{
					// Erster Index
					unsigned int iIndexFirst = m_iSubselected + (m_iPage / 2) * 9;
					// Index zum Tauschen (mit m_iSelected)
					unsigned int iIndexSwitchWith = iIndexFirst + (( m_iColumnSelected == 0 ) ? -1 : +1); 

					// Daten holen
					CBerylliumData data; m_Document.GetData(data);

					// Tauschen
					data.SwitchSubstancesByIndex( iIndexFirst, iIndexSwitchWith); 

					// Setzen
					m_Document.SetData( data );
				}
				break;

			// Rückseite?
			case 101:
			case 102:
			case 103:
			case 104:
				ShowDialogStatementEditor();
				break;

			default:
				break;
		}
	}

	// Rechte Maustaste im Substanzblock?
	else if ( event.RightDown() && (m_iSelected == 9) )
	{
		// umgerechnete Mauskoordinaten speichern
		wxPoint mouse = event.GetPosition();

		// Popupmenu zeigen
		ShowSubstancePopupMenu( mouse.x, mouse.y );
	}
}

// Prüft, ob die Mausposition in einem auswählbaren Bereich liegt
void CBerylliumViewSafetySheets::OnMouseMove(wxMouseEvent &event)
{
	// Die alten Werte für die Auswahl speichern
	unsigned int iOldSelection = m_iSelected;
	unsigned int iOldSubSelection = m_iSubselected;
	unsigned int iOldColumnSelection = m_iColumnSelected;

	// Auswahl zurücksetzen
	m_iSelected = 0;
	m_iSubselected = 0;

	// Position der Maus holen (wurde vom ViewManager schon umgerechnet)
	wxPoint ptMouse = event.GetPosition();

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Vorder- oder
	if ( (m_iPage % 2) == 0 )
	{
		// Array mit den Rechtecken anlegen
		wxRect rectBoxes[] = {	wxRect(  -1,  -1,  -1,  -1),
								wxRect(  20,  20,  700,  41),	// Titel: 1. Zeile
								wxRect(  20,  60,  700,  41),	// Titel: 2. Zeile
								wxRect(  20, 100,  700,  41),	// Titel: 3. Zeile
								wxRect(  20, 140,  300,  51),	// Name(n)
								wxRect( 320, 140,  150,  51),	// Platznummer
								wxRect( 470, 140,  250,  51),	// Assistent
								wxRect(  20, 190,  700,  66),	// Herzustellendes Präparat
								wxRect(  20, 255,  700,  51),	// Tabellenkopf
								wxRect(  20, 305,  700, 451),	// Tabellenkörper (Substanzen)
								wxRect(  20, 758,  700,  35),	// Wortlaut der ...
								wxRect(  20, 793,  700, 230),   // R-/S-Sätze
								wxRect(   0, 305,   16, 451) }; // Sortierpfeile

		// Alle Bereiche durchgehen und schauen was selektiert wurde
		for ( unsigned int i = 0; i < 13; ++i )
			if ( rectBoxes[i].Contains( ptMouse ) )
				m_iSelected = i;

		// Substanzblock erwischt?
		if ( m_iSelected == 9 )
		{
			// Substanzindex der aktuellen Box berechnen: Alle 2 Seiten passen 9 Substanzen drauf
			// Dazu muss noch der Index der aktuellen Seite addiert werden
			// ( alle 52 Pixel eine neue Substanz)
			unsigned int iSubstanceIndex = ( m_iPage / 2 ) * 9 + (ptMouse.y - 305) / 52;

			// Ist dieser Index verfügbar?
			// +1 weil ja die letzte leere Zeile auch auswählbar sein soll
			if ( iSubstanceIndex < (data.sdCompounds.size() + 1) )
				m_iSubselected = (ptMouse.y - 305) / 52;
			else
				m_iSelected = 0;

			// Spaltenindex nur ändern, wenn NICHT die leere Spalte ausgewählt ist
			if ( iSubstanceIndex < data.sdCompounds.size() )
			{
				// Spaltenindex zurücksetzen
				m_iColumnSelected = 0;

				// relative x-Position der Maus
				int x = ptMouse.x - 20;

				// Spalten durchgehen und jeweils die Breite von der aktuellen, relativen X-Mausposition
				// abziehen. Sobald x negativ wird, haben wir unsere Spalte.
				for ( unsigned int i = 0; i < m_Columns.size(); i++ )
				{
					// Nur sichtbare Boxen beachten
					if ( m_Columns[i]->IsShown() )
					{
						// Spaltenbreite abziehen
						x -= m_Columns[i]->GetColumnWidth();

						// Negativ? Index setzen und raus hier!
						if ( x < 0 )
						{
							m_iColumnSelected = i;
							break;
						}
					}
				}
			}
		}

		// Sortierpfeile?
		else if ( m_iSelected == 12 )
		{
			// Spaltenindex zurücksetzen
			m_iColumnSelected = 0;

			// Substanzindex der aktuellen Box berechnen: Alle 2 Seiten passen 9 Substanzen drauf
			// Dazu muss noch der Index der aktuellen Seite addiert werden
			// ( alle 52 Pixel eine neue Substanz)
			unsigned int iSubstanceIndex = ( m_iPage / 2 ) * 9 + (ptMouse.y - 305) / 52;

			// Ist dieser Index verfügbar?
			if ( iSubstanceIndex < data.sdCompounds.size() )
			{
				// Zeile, die ausgewählt ist
				m_iSubselected = (ptMouse.y - 305) / 52;

				// m_iColumnSelected setzen wir auf 0, falls der Pfeil nach oben ausgewählt ist
				// und auf 1 falls der Pfeil nach unten ausgewählt ist				
				m_iColumnSelected = (( ptMouse.y - 305 - (m_iSubselected * 52) - 9 ) / 16);

				// Sollte einen Wert von 0 oder 1 haben, ansonsten nichts auswählen
				if ( m_iColumnSelected > 1 )
				{
					m_iColumnSelected = 0; m_iSelected = 0; m_iSubselected = 0;
				}

				// Allererste Substanz? Dann keinen Pfeil nach oben!
				if ( (iSubstanceIndex == 0) && (m_iColumnSelected == 0) )
					m_iSelected = 0;

				// Allerletzte Substanz? Dann keinen Pfeil nach unten!
				if ( (iSubstanceIndex == (data.sdCompounds.size()-1)) && (m_iColumnSelected == 1) )
				{
					m_iSelected = 0; m_iColumnSelected = 0; m_iSubselected = 0;
				}
			}

			// Ansonsten gar nichts auswählen
			else
			{
				m_iSelected = 0; m_iSubselected = 0;
			}

		}
	}

	// Rückseite?
	else
	{
		// Array mit den Rechtecken anlegen
		wxRect rectBoxes[] = {	wxRect(  -1,  -1,  -1,  -1),
								wxRect(  20,  20,  700,  212),	// Gefahren für Mensch und Umwelt
								wxRect(  20, 233,  700,  212),	// Schutzmaßnahmen
								wxRect(  20, 443,  700,  212),	// Verhalten im Gefahrenfall
								wxRect(  20, 653,  700,  212),	// Entsorgung
								wxRect(  20, 863,  352,  160),	// "Hiermit verpflichte ich mich..."
								wxRect( 372, 863,  350,  160)}; // "Präparat zur Synthese freigegeben..."

		// Alle Bereiche durchgehen und schauen was selektiert wurde
		for ( unsigned int i = 0; i < 7; i++ )
			if ( rectBoxes[i].Contains( ptMouse ) )
				m_iSelected = i + 100;			// Auswahlindex auf der Rückseite beginnt bei 100!

	}

	// Auswahl? Dann Mauscursor auf Zeigefinger setzen, andernfalls auf den normalen Cursor
	if ( m_iSelected > 0 )
		m_Parent.SetCursor( wxCursor( wxCURSOR_HAND ) );
	else
		m_Parent.SetCursor( wxCursor( wxCURSOR_ARROW ) );

	// Wenn die Werte sich geändert haben, nur DANN zeichnen wir neu!
	if ( (iOldSelection != m_iSelected) || (iOldSubSelection != m_iSubselected) || (iOldColumnSelection != m_iColumnSelected) )
		m_Parent.Refresh();
}

void CBerylliumViewSafetySheets::ShowDialogProperties()
{
	// Seite?
	int iPage = 0;

	// R-/S-Sätze angeklickt?
	if ( m_iSelected == 10 )
		iPage = 1;

	// Signaturboxen angeklickt
	else if ( (m_iSelected == 105) || (m_iSelected == 106) )
		iPage = 2;

	// Dialog vorbereiten
	CBerylliumViewSafetySheetsProperties m_prop( &m_Parent, m_Document.GetData(), iPage );

	// Spaltendaten übergeben
	m_prop.SetColumnsVector( m_Columns );

	// Anzeige der HP-Sätze
	m_prop.ShowHPStatements( bShowHPStatements );

	// Anzeigen und Rückgabe auswerten
	if ( m_prop.ShowModal() == wxID_OK )
	{
		// Spaltendaten holen
		m_prop.GetColumnsVector( m_Columns );

		// Anzeige der HP-Sätze
		bShowHPStatements = m_prop.ShowHPStatements();

		// Neu berechnen
		ColumnsCalculate();

		// Datenobjekt holen
		CBerylliumData data = m_prop.GetData();

		// Haben sich die Signaturdaten geändert?
		if ( (data.szSignaturStudent.compare( m_Document.GetData().szSignaturStudent ) != 0) ||
			(data.szSignaturSupervisor.compare( m_Document.GetData().szSignaturSupervisor ) != 0) ||
			(data.szStatementStudent.compare( m_Document.GetData().szStatementStudent ) != 0) ||
			(data.szStatementSupervisor.compare( m_Document.GetData().szStatementSupervisor ) != 0) )
		{
			// neue Daten setzen
			m_Document.SetData( data );
		}

		// Fenster neuzeichnen
		m_Parent.Refresh();
	}
}

void CBerylliumViewSafetySheets::ShowDialogGeneral()
{
	// Dialog vorbereiten
	CBerylliumSafetySheetsGeneral m_prop( &m_Parent, m_Document.GetData() );

	// Seite ändern, falls Auswahl die persönlichen Daten betreffen
	if ( (m_iSelected > 3) && (m_iSelected < 7) )
		m_prop.SetPage(1);

	// Seite ändern, falls Auswahl die "herzustellende Substanz" betreffen
	if ( m_iSelected == 7 )
		m_prop.SetPage(2);

	// Anzeigen und Rückgabe auswerten
	if ( m_prop.ShowModal() == wxID_OK )
	{
		// Daten holen und setzen
		m_Document.SetData( m_prop.GetData() );
	}
}

void CBerylliumViewSafetySheets::ShowDialogSubstance()
{
	// Hier gibt es zwei Möglichkeiten:
	// 1) Es wurde die letzte Spalte angeklickt, dann steht
	//    in m_iSubselected der letzte Index+1
	// 2) Es wurde eine Substanz angeklickt, dann steht in
	//    m_iSubselected der Index der Substanz
	// Es muss zu beiden noch (m_iPage/2)*9 addiert werden.

	// Anzahl der Substanzen
	unsigned int iNumberOfSubstances = m_Document.GetData().sdCompounds.size();

	// Index der Substanz (auf alle zwei Seiten passen 9 Substanzen )
	unsigned int iIndex = m_iSubselected + (m_iPage / 2) * 9;

	// Möglichkeit 1 => Suchdialog öffnen
	if ( iNumberOfSubstances == iIndex )
	{
		// Neue Substanz hinzufügen.
		SubstanceAdd();

		// Rausspringen
		return;
	}

	// Möglichkeit 2 => Eigenschaftsdialog der Substanz öffnen
	else if ( iIndex < iNumberOfSubstances )
	{
		// Eigenschaftsdialog zeigen
		SubstanceProps( iIndex );

		// Rausspringen
		return;
	}
}

void CBerylliumViewSafetySheets::ShowDialogStatementEditor()
{
	// Titel festlegen
	wxString titles[] = { _(L"Gefahren für Mensch und Umwelt"),
						  _(L"Schutzmaßnahmen und Verhaltensregeln"),
						  _(L"Verhalten im Gefahrenfall"),
						  _(L"Entsorgung") };

	// Index holen
	int index = m_iSelected - 101;

	// Fall abfangen hier mit ungültigem Index zu landen
	if ( (index > 3) || (index < 0) )
		index = 0;

	// Dialog erstellen
	CBerylliumStatementEditor dlg( &m_Parent, titles[index] );

	// Liste übertragen
	dlg.SetStatements( m_Document.GetData().sCustomStatements[index] );

	// Dialog anzeigen
	if ( dlg.ShowModal() == wxID_OK )
	{
		// Daten holen
		CBerylliumData data = m_Document.GetData();

		// Liste übertragen
		dlg.GetStatements( data.sCustomStatements[index] );

		// Daten zurückschieben
		m_Document.SetData( data );

	}

}

// GHS-Modus?
bool CBerylliumViewSafetySheets::IsGHSMode()
{
	return bGHSMode;
}

// GHS-Modus setzen
void CBerylliumViewSafetySheets::SetGHSMode( bool toGHS )
{
	// zu GHS wechseln
	if ( toGHS )
	{
		// Spalten GHS-Symbol(e) und H/P-Sätze anzeigen
		m_Columns[COL_GHSSYMBOLS]->Show(); m_Columns[COL_HARZARDPRECAUTION]->Show();

		// Spalten Gefahrensymbole und R/S-Sätze verstecken
		m_Columns[COL_HARZARDSYMBOLS]->Hide(); m_Columns[COL_RISKSAFETIES]->Hide();
	}

	// zum alten System wechseln
	else
	{
		// Spalten GHS-Symbol(e) und H/P-Sätze verstecken
		m_Columns[COL_GHSSYMBOLS]->Hide(); m_Columns[COL_HARZARDPRECAUTION]->Hide();

		// Spalten Gefahrensymbole und R/S-Sätze anzeigen
		m_Columns[COL_HARZARDSYMBOLS]->Show(); m_Columns[COL_RISKSAFETIES]->Show();
	}

	// Wortlaut der wesentlich...
	bShowHPStatements = toGHS;

	// GHS-Modus
	bGHSMode = toGHS;

	// Spalten neu berechnen
	ColumnsCalculate();

	// Fenster neuzeichnen
	m_Parent.Refresh();

	// Zurück
	return;
}

// Ordnet alle Sätze für Seite iPage neu
void CBerylliumViewSafetySheets::OrderStatements( unsigned int iPage )
{
	// 1: Überprüfen ob soviele Elemente in m_statements vorhanden sind
	for ( unsigned int i = 0; i < 8; i++ )
	{
		// Nicht genug Elemente, dann raus hier!
		if ( m_statements[i].size() <= iPage )
			return;

		// Elemente löschen
		m_statements[i][iPage].DeleteAllStatements();
	}

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Bibliothek holen
	const CBerylliumStatementsManager &library = ::wxGetApp().GetStatementLibrary();

	// Vorlagenbibliothek holen
	const CBerylliumStatementsManager &templates = ::wxGetApp().GetTemplatesLibrary();

	// Kategorien
	wxString templatecategories[] = { "danger", "behaviour", "firstaid", "waste" };

	// 2: Welche Substanzen sind betroffen? (9 Substanzen pro Seite)
	unsigned int iFirstSubstance	= iPage * 9;
	unsigned int iLastSubstance		= iFirstSubstance + 9;

	if ( iLastSubstance > data.sdCompounds.size() )
		iLastSubstance = data.sdCompounds.size();

	// Ist eine Substanz toxisch?
	bool bIsToxic = false;

	// 3: Substanzen durchgehen
	for ( unsigned int i = iFirstSubstance; i < iLastSubstance; ++i )
	{
		// H/P-Sätze
		// ---------

		// Listen anlegen
		std::vector< wxString > listHarzards, listPrecautionaries;

		// Listen füllen
		BeTool::StringToList( data.sdCompounds[i].szHarzardSatements, "-", listHarzards );
		BeTool::StringToList( data.sdCompounds[i].szPrecautionaryStatements, "-", listPrecautionaries );

		// Statements füllen
		for ( unsigned int j = 0; j < listHarzards.size(); j++ )
		{
			// Soll der Satz versteckt werden?
			if ( std::find(m_Document.m_hidephrases.begin(), m_Document.m_hidephrases.end(), "H"+listHarzards[j]) == m_Document.m_hidephrases.end() )
				m_statements[STATEMENT_HARZARDS][iPage].AddStatement( listHarzards[j], library[ "H"+listHarzards[j]] );
		}

		for ( unsigned int j = 0; j < listPrecautionaries.size(); j++ )
		{
			// Soll der Satz versteckt werden?
			if ( std::find(m_Document.m_hidephrases.begin(), m_Document.m_hidephrases.end(), "P"+listPrecautionaries[j]) == m_Document.m_hidephrases.end() )
				m_statements[STATEMENT_PRECAUTIONARIES][iPage].AddStatement( listPrecautionaries[j], library[ "P"+listPrecautionaries[j]] );
		}

		// Sortieren
		m_statements[STATEMENT_HARZARDS][iPage].Sort();
		m_statements[STATEMENT_PRECAUTIONARIES][iPage].Sort();

		// R-/S-Sätze!
		// -----------

		// Listen anlegen
		std::vector< wxString > listRisks, listSafeties;

		// S-Sätze: Klammern suchen und entfernen (1/2-Sätze für Kinder/Jugendliche)
		wxString safeties = data.sdCompounds[i].szSafeties;

		int iBra = safeties.find("(");

		// Alle Bra-Kets finden und eliminieren
		while ( iBra != -1 )
		{
			int iKet = safeties.find(")");

			if ( iKet == -1 )
				break;

			safeties.erase(iBra, iKet-iBra+1);

			iBra = safeties.find("(");
		}

		// Listen füllen
		BeTool::StringToList( data.sdCompounds[i].szRisks, "-", listRisks );
		BeTool::StringToList( safeties, "-", listSafeties );

		// Statements füllen
		for ( unsigned int j = 0; j < listRisks.size(); j++ )
		{
			// Soll der Satz versteckt werden?
			if ( std::find(m_Document.m_hidephrases.begin(), m_Document.m_hidephrases.end(), "R"+listRisks[j]) == m_Document.m_hidephrases.end() )
				m_statements[STATEMENT_RISKS][iPage].AddStatement( listRisks[j], library["R"+listRisks[j]] );
		}

		for ( unsigned int j = 0; j < listSafeties.size(); j++ )
		{
			// Soll der Satz versteckt werden?
			if ( std::find(m_Document.m_hidephrases.begin(), m_Document.m_hidephrases.end(), "S"+listSafeties[j]) == m_Document.m_hidephrases.end() )
				m_statements[STATEMENT_SAFETIES][iPage].AddStatement( listSafeties[j], library["S"+listSafeties[j]] );
		}

		// Sortieren
		m_statements[STATEMENT_RISKS][iPage].Sort();
		m_statements[STATEMENT_SAFETIES][iPage].Sort();

		// Rückseite
		// ---------

		// Name der Substanz ermitteln
		wxString sName = data.sdCompounds[i].GetName();

		// Vorlagen für R/S-Sätze
		// Anmerkung: Da diese unabhängig vom Darstellungsmodus eingebunden werden, werden keine
		// H/P-Analoga benötigt. Vorerst zumindest.
		// Geändert: H-/P-Sätze werden jetzt auch berücksichtig und eingebunden!
		for ( unsigned int j = 0; j < templates.GetSize(); ++j )
		{
			// Kategorie-Nummer
			int nCategory = STATEMENT_DANGERS;

			// Kategorie-Nummer ermitteln
			for ( unsigned int n = 0; n < 4; ++n )
			{
				if ( templates.GetStatementByIndex(j).GetGroup() == templatecategories[n] )
				{
					nCategory += n;
					break;
				}
			}

			// Token
			wxString token = templates.GetStatementByIndex(j).GetToken();

			// R-Sätze
			//if ( (token[0] == 'R') && m_statements[STATEMENT_RISKS][iPage].HasToken( token.substr(1) ) )
			if ( token[0] == 'R' )
			{
				for ( unsigned int r = 0; r < listRisks.size(); ++r )
				{
					if ( listRisks[r] == token.substr(1) )
					{
						m_statements[nCategory][iPage].AddStatementToToken( sName + ":", templates.GetStatementByIndex(j).GetStatement() );
						break;
					}
				}
			}

			// S-Sätze
			//if ( (token[0] == 'S') &&m_statements[STATEMENT_SAFETIES][iPage].HasToken( token.substr(1) ) )
			if ( token[0] == 'S' )
			{
				for ( unsigned int r = 0; r < listSafeties.size(); ++r )
				{
					if ( listSafeties[r] == token.substr(1) )
					{
						m_statements[nCategory][iPage].AddStatementToToken( sName + ":", templates.GetStatementByIndex(j).GetStatement() );
						break;
					}
				}
			}

			// H-Sätze
			if ( token[0] == 'H' )
			{
				for ( unsigned int r = 0; r < listHarzards.size(); ++r )
				{
					if ( listHarzards[r] == token.substr(1) )
					{
						m_statements[nCategory][iPage].AddStatementToToken( sName + ":", templates.GetStatementByIndex(j).GetStatement() );
						break;
					}
				}
			}

			// P-Sätze
			if ( token[0] == 'P' )
			{
				for ( unsigned int r = 0; r < listPrecautionaries.size(); ++r )
				{
					if ( listPrecautionaries[r] == token.substr(1) )
					{
						m_statements[nCategory][iPage].AddStatementToToken( sName + ":", templates.GetStatementByIndex(j).GetStatement() );
						break;
					}
				}
			}
		}

		// Toxisch? (nur überprüfen, wenn nicht schon eine andere Substanz toxisch ist)
		if ( !bIsToxic )
			bIsToxic = ( data.sdCompounds[i].HasGHSSymbol( "06" ) || data.sdCompounds[i].HasHarzard( "T" ) || data.sdCompounds[i].HasHarzard( "T+" ) );

		// Statements für die Rückseite: Gefahren
		for ( unsigned int j = 0; j < data.sdCompounds[i].sCustomStatements[0].size(); j++ )
			m_statements[STATEMENT_DANGERS][iPage].AddStatementToToken( sName + ":", data.sdCompounds[i].sCustomStatements[0][j] );

		// Statements für die Rückseite: Verhalten im Gefahrenfall
		for ( unsigned int j = 0; j < data.sdCompounds[i].sCustomStatements[1].size(); j++ )
			m_statements[STATEMENT_BEHAVIOURS][iPage].AddStatementToToken( sName + ":", data.sdCompounds[i].sCustomStatements[1][j] );

		// Statements für die Rückseite: Verhalten im Gefahrenfall
		for ( unsigned int j = 0; j < data.sdCompounds[i].sCustomStatements[2].size(); j++ )
			m_statements[STATEMENT_FIRSTAIDS][iPage].AddStatementToToken( sName + ":", data.sdCompounds[i].sCustomStatements[2][j] );

		// Statements für die Rückseite: Verhalten im Gefahrenfall
		for ( unsigned int j = 0; j < data.sdCompounds[i].sCustomStatements[3].size(); j++ )
			m_statements[STATEMENT_WASTE][iPage].AddStatementToToken( sName + ":", data.sdCompounds[i].sCustomStatements[3][j] );
	}

	// Sortieren
	m_statements[STATEMENT_DANGERS][iPage].Sort();
	m_statements[STATEMENT_BEHAVIOURS][iPage].Sort();
	m_statements[STATEMENT_FIRSTAIDS][iPage].Sort();
	m_statements[STATEMENT_WASTE][iPage].Sort();

	// Standardsätze anfügen
	m_statements[STATEMENT_BEHAVIOURS][iPage].AddStatementToToken( "", _(L"Bei der Arbeit geeignete Schutzkleidung, Schutzhandschuhe und Schutzbrille tragen.") );
	m_statements[STATEMENT_FIRSTAIDS][iPage].AddStatementToToken( _(L"Bei Berührung mit Haut oder Augen:"), _(L"Gründlich mit Wasser ab- bzw ausspülen und evtl. Arzt konsultieren.") );
	m_statements[STATEMENT_FIRSTAIDS][iPage].AddStatementToToken( _(L"Nach Einatmen:"), _(L"Sofort frische Luft zuführen.") );

	if ( bIsToxic )
		m_statements[STATEMENT_FIRSTAIDS][iPage].AddStatementToToken( _(L"Nach Verschlucken:"), _(L"Aktivkohle schlucken. Viel Wasser trinken, Mund ausspülen und Arzt konsultieren.") );
	else
		m_statements[STATEMENT_FIRSTAIDS][iPage].AddStatementToToken( _(L"Nach Verschlucken:"), _(L"Viel Wasser trinken, Mund ausspülen und evtl Arzt konsultieren.") );
}

// Ordnet alle Sätze neu (beginnend mit iStartPage)
void CBerylliumViewSafetySheets::OrderAllStatements( unsigned int iStartPage = 0 )
{
	// Anzahl der Seiten (9 Substanzen pro Seite)
	unsigned int iPages = (m_Document.GetData().sdCompounds.size() / 9);

	// "Überhang"seite
	if ( (m_Document.GetData().sdCompounds.size() % 9) > 0 )
		iPages++;

	// Setze alle Statements-Vektoren auf die "maximale" Größe (= Seitenanzahl)
	for ( unsigned int i = 0; i < 8; i++ )
		m_statements[i].resize( iPages );

	// Jede Seite ordnen
	for ( unsigned int i = iStartPage; i < iPages; i++ )
		OrderStatements( i );
}

// Zeigt das Contextmenu für Substanzen an
void CBerylliumViewSafetySheets::ShowSubstancePopupMenu( int imousex, int imousey )
{
	// Substanzindex der aktuellen Box berechnen: Alle 2 Seiten passen 9 Substanzen drauf
	// Dazu muss noch der Index der aktuellen Seite addiert werden
	// ( alle 52 Pixel eine neue Substanz)
	unsigned int iSubstanceIndex = ( m_iPage / 2 ) * 9 + (imousey - 305) / 52;

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Keine Markierung, kein Menu
	if ( iSubstanceIndex > data.sdCompounds.size() )
		return;

	// Popupmenu für die letzte Zeile zeigen
	if ( iSubstanceIndex == data.sdCompounds.size() )
	{
		// Menu vorbereiten
		wxMenu *menu = new wxMenu();

		// Menuitems einfügen
		menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_ADD, _(L"Hinzufügen") ) );
		menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_IMPORT, _(L"Import aus Bibliothek...") ) );

		// Menu anzeigen
		m_Parent.PopupMenu( menu, m_Parent.ScreenToClient(wxGetMousePosition()) );

		// Menu löschen
		delete menu;

		return;
	}

	// Substanzdaten holen
	const CBerylliumSubstanceData substdata = data.sdCompounds[iSubstanceIndex];

	// Menu vorbereiten
	wxMenu *menu = new wxMenu();

	// Menuitems einfügen
	menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_ADD, _(L"Hinzufügen") ) );
	menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_DEL, _(L"Löschen") ) );

	menu->AppendSeparator();
	menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_EXPORT, _(L"Zu Bibliothek hinzufügen...") ) );

	menu->AppendSeparator();
	menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_WIKI, _(L"Wikipedia-Artikel zu ") + substdata.GetShortestName() ) );
	if ( substdata.szCAS.length() > 0 ) menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_CAS, _(L"Sicherheitsdatenblätter zu CAS ") + substdata.szCAS ) );
	menu->AppendSeparator();
	menu->Append( new wxMenuItem( 0, beID_SUBSTANCE_PROPS, _(L"Eigenschaften") ) );

	// Menu anzeigen
	m_Parent.PopupMenu( menu, m_Parent.ScreenToClient(wxGetMousePosition()) );

	// Menu löschen
	delete menu;
}

// Verarbeitet die IDs des Popupmenus
void CBerylliumViewSafetySheets::DoPopupMenu( wxCommandEvent &event )
{
	// Anzahl der Substanzen
	// unsigned int iNumberOfSubstances = m_Document.GetData().sdCompounds.size();

	// Index der Substanz (auf alle zwei Seiten passen 9 Substanzen )
	unsigned int iIndex = m_iSubselected + (m_iPage / 2) * 9;

	// Welche ID geklickt?
	switch( event.GetId() )
	{
		// Hinzufügen
		case beID_SUBSTANCE_ADD:
			SubstanceAdd();
			break;

		// Löschen
		case beID_SUBSTANCE_DEL:
			SubstanceDel( iIndex );
			break;

		// Substanz exportieren
		case beID_SUBSTANCE_EXPORT:
			SubstanceExport( iIndex );
			break;

		// Substanz exportieren
		case beID_SUBSTANCE_IMPORT:
			SubstanceImport();
			break;

		// Substanz zu einer Bibliothek hinzufügen
		case beID_SUBSTANCE_LIBRARY:
			break;

		// Wikipedia-Seite aufrufen (falls vorhanden)
		case beID_SUBSTANCE_WIKI:
			SubstanceWiki( iIndex );
			break;

		// CAS-Nummer bei euSDB aufrufen
		case beID_SUBSTANCE_CAS:
			SubstanceEUSDB( iIndex );
			break;

			// Eigenschaftsdialog anzeigen
		case beID_SUBSTANCE_PROPS:
			SubstanceProps( iIndex );
			break;

	}

	// Weitermachen
	event.Skip();
}

// Fügt eine neue Substanz ein (zeigt Dialog, etc)
void CBerylliumViewSafetySheets::SubstanceAdd()
{
	// Dialog vorbereiten
	CBerylliumSubstanceSearchDialog m_dialogsearch( &m_Parent );

	// Anzeigen und Rückgabe auswerten
	if ( m_dialogsearch.ShowModal() == wxID_OK )
	{
		// Daten einfügen (benachrichtig automatisch alle Beobachter, etc.)
		m_Document.AddSubstance(  m_dialogsearch.GetData() );		

		// Auf welcher Seite befinden wir uns?
		unsigned int iPage = m_Document.GetData().sdCompounds.size() / 9;

		// "Neue" Seite? Dann entsprechend bei den Sätzen einfügen
		for ( unsigned int i = 0; i < 8; i++ )
			if ( m_statements[i].size() == iPage )
				m_statements[i].push_back( CBerylliumStatementsManager() );

		// Die Sätze der Seite neuordnen
		OrderStatements(iPage);
	}
}

// Löscht Substanz x aus dem Datenblatt
void CBerylliumViewSafetySheets::SubstanceDel( unsigned int iSubstance )
{
	// Daten holen
	CBerylliumData data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( iSubstance >= data.sdCompounds.size() )
		return;

	// Abfrage
	if ( wxMessageBox( wxString::Format( _(L"Möchten Sie %s wirklich entfernen?"), data.sdCompounds[iSubstance].GetName().c_str() ), _(L"Sicherheitsabfrage"), wxYES_NO | wxICON_QUESTION ) == wxYES )
	{
		// Auf welcher Seite befinden wir uns?
		int iPage = iSubstance / 9;

		// Substanz löschen
		data.sdCompounds.erase( data.sdCompounds.begin() + iSubstance );

		// Daten setzen
		m_Document.SetData( data );

		// Die Sätze ab der Seite anschließend neuordnen
		OrderAllStatements(iPage);
	}
}

// Zeigt den Eigenschaftsdialog für Substanz x an
void CBerylliumViewSafetySheets::SubstanceProps( unsigned int iSubstance )
{
	// Daten holen
	CBerylliumData data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( iSubstance >= data.sdCompounds.size() )
		return;

	// Dialog anlegen
	CBerylliumSubstanceProperties m_dialogproperties( &m_Parent,  data.GetSubstanceByIndex( iSubstance ), m_Columns[m_iColumnSelected]->GetPage() );

	// Dialog anzeigen
	if ( m_dialogproperties.ShowModal() == wxID_OK )
	{
		// Daten updaten
		data.UpdateSubstanceDataAtIndex( iSubstance, m_dialogproperties.GetData() );

		// Setzen
		m_Document.SetData( data );

		// Auf welcher Seite befinden wir uns?
		int iPage = iSubstance / 9;

		// Die Sätze der Seite neuordnen
		OrderStatements(iPage);
	}
}

// öffnet die Wikipedia-Seite
void CBerylliumViewSafetySheets::SubstanceWiki( unsigned int iSubstance )
{
	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( iSubstance >= data.sdCompounds.size() )
		return;

	// Seite aufrufen
	wxLaunchDefaultBrowser( wxString::Format( _("http://de.wikipedia.org/wiki/%s"), data.sdCompounds[iSubstance].GetName() ) );
}

// Sucht bei euSDB nach der CAS-Nummer
void CBerylliumViewSafetySheets::SubstanceEUSDB( unsigned int iSubstance )
{
	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( iSubstance >= data.sdCompounds.size() )
		return;

	// Seite aufrufen
	wxLaunchDefaultBrowser( wxString::Format( _("http://www.eusdb.de/script/c/sdbsuche.php?cas=%s#eusdbsearch"), data.sdCompounds[iSubstance].szCAS ) );
}

// Event: Datei wurde geladen
void CBerylliumViewSafetySheets::OnFileWasLoaded()
{
	// Spalten holen und anzeigen
	if ( m_Document.m_showcolumns.size() > 0 )
	{
		// Alle Spalten überprüfen
		for ( unsigned int i = 0; i < m_Columns.size(); ++i )
		{
			// Verstecken
			m_Columns[i]->Hide();

			// Überprüfen
			for ( unsigned int j = 0; j < m_Document.m_showcolumns.size(); ++j )
			{
				if ( m_Columns[i]->HasInternalName(m_Document.m_showcolumns[j]) )
					m_Columns[i]->Show();
			}
		}

		// Neu Berechnen
		ColumnsCalculate();
	}

	// Alle Daten neu berechnen
	OrderAllStatements();
}

// Event: Datei wurde gespeichert
void CBerylliumViewSafetySheets::OnFileWillBeSaved()
{
	// Spalten speichern
	m_Document.m_showcolumns.clear();

	// Alle Spalten überprüfen
	for ( unsigned int i = 0; i < m_Columns.size(); ++i )
	{
		if ( m_Columns[i]->IsShown() )
			m_Document.m_showcolumns.push_back( m_Columns[i]->GetInternalName() );
	}

}

// Gibt ein wxPrintOut-Objekt zurück
wxPrintout *CBerylliumViewSafetySheets::GetPrintOut()
{
	// Erstelle ein Objekt
	return new CBerylliumViewSafetySheetsPrinting( *this, m_Document );
}

// Setzt den Druckmodus bzw. wieder zurück
void CBerylliumViewSafetySheets::SwitchToPrintMode( bool bPrint = true )
{
	// Egal ob Druckmodus oder nicht: Zurücksetzen aller "Markierungen"
	m_iSelected			= 0;
	m_iSubselected		= 0;
	m_iColumnSelected	= 0;

	// Vorschaumodus ausschalten
	m_isPreview			= false;

	// Alle Spalten an den Druckmodus anpassen
	for ( unsigned int i = 0; i < m_Columns.size(); ++i )
		m_Columns[i]->SwitchToPrintMode( bPrint );

	// Ansonsten Druckmodus setzen
	m_isPrinting = bPrint;
}

// Setzt den Vorschaumodus bzw. wieder zurück
void CBerylliumViewSafetySheets::SwitchToPreviewMode( bool bPreview = true )
{
	// Egal ob Vorschaumodus oder nicht: Zurücksetzen aller "Markierungen"
	m_iSelected			= 0;
	m_iSubselected		= 0;
	m_iColumnSelected	= 0;

	// Druckmodus ausschalten
	m_isPrinting		= false;

	// Alle Spalten an den Vorschaumodus anpassen
	for ( unsigned int i = 0; i < m_Columns.size(); ++i )
		m_Columns[i]->SwitchToPrintMode( false );

	// Ansonsten Vorschaumodus setzen
	m_isPreview = bPreview;
}

// Importiert die Substanz
void CBerylliumViewSafetySheets::SubstanceImport()
{
	// DialogBox öffnen
	wxFileDialog fd( &m_Parent, _(L"Importieren von..."), "", _(L"*.xml"), _(L"Bibliothek (*.xml)|*.xml|Alle Dateien|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// DialogBox anzeigen
	if ( fd.ShowModal() == wxID_OK )
	{
		// kompletten Pfad + Dateinamen holen
		wxString sPath = fd.GetPath();

		// Bibliothek erstellen und laden
		CBerylliumDataLibrary lib;

		// von Datei laden
		lib.LoadFromFile( sPath );

		// Sortieren nach Name
		lib.SortByNames();

		// Array anlegen
		wxArrayString list;

		// Array füllen
		for ( int i = 0; i < lib.NumberOfSubstances(); ++i )
		{
			// CAS-Nummer vorhanden?
			if ( lib.GetItem(i).szCAS.length() > 0 )
				list.Add( lib.GetItem(i).GetName() + " [" + lib.GetItem(i).szCAS + "]" );

			// andernfalls nur Name einfügen
			else
				list.Add( lib.GetItem(i).GetName() );
		}

		// Auswahldialog erstellen
		wxSingleChoiceDialog dlg( &m_Parent, _(L"Bitte eine Substanz zum importieren wählen und auf 'Ok' klicken."), _(L"Substanz importieren"), list);

		// Der Orginaldialog ist etwas klein...
		dlg.SetSize( 300, 300 );

		// Zentrieren, falls möglich
		dlg.CenterOnParent();

		// Dialog anzeigen
		if ( dlg.ShowModal() == wxID_OK )
		{
			// Index
			int iIndex = dlg.GetSelection();

			// Nichts gefunden?
			if ( (iIndex == -1) || (iIndex > (lib.NumberOfSubstances()-1)) )
				return;

			// Ansonsten einfügen
			m_Document.AddSubstance( lib.GetItem(iIndex) );

			// Auf welcher Seite befinden wir uns?
			unsigned int iPage = m_Document.GetData().sdCompounds.size() / 9;

			// "Neue" Seite? Dann entsprechend bei den Sätzen einfügen
			for ( unsigned int i = 0; i < 8; i++ )
				if ( m_statements[i].size() == iPage )
					m_statements[i].push_back( CBerylliumStatementsManager() );

			// Die Sätze der Seite neuordnen
			OrderStatements(iPage);
		}
	}
}

// Exportiert eine Substanz
void CBerylliumViewSafetySheets::SubstanceExport( unsigned int iSubstance )
{
	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( iSubstance >= data.sdCompounds.size() )
		return;

	// Dateiname hier speichern
	wxString sFileName;

	// DialogBox öffnen
	wxFileDialog fd( &m_Parent, _(L"Exportieren nach..."), "", sFileName, _(L"Bibliothek (*.xml)|*.xml|Alle Dateien|*.*"), wxFD_SAVE);

	// DialogBox anzeigen
	if ( fd.ShowModal() == wxID_OK )
	{
		// Dateinamen holen
		sFileName = fd.GetFilename();

		// Pfad holen
		wxString sPath = fd.GetPath();

		// Bibliothek erstellen
		CBerylliumDataLibrary lib;

		// Von Datei laden (falls vorhanden)
		if ( ::wxFileExists( sPath ) )
			lib.LoadFromFile( sPath );

		// Substanz hinzufügen
		lib.Add( data.sdCompounds[iSubstance] );

		// Speichern
		lib.SaveToFile( sPath );
	}
}

// Zeigt den Sätze-verstecken-Dialog an
void CBerylliumViewSafetySheets::ShowDialogPhrases( unsigned int iPage = 0 )
{
	// Dialog erstellen
	CBerylliumPhraseSelector dlg( &m_Parent, iPage );

	// Vektor übergegeben
	dlg.SetPhraseVector( m_Document.m_hidephrases );

	// Vektor erstellen für alle Sätze
	std::vector< wxString > allphrases_r;
	std::vector< wxString > allphrases_s;
	std::vector< wxString > allphrases_p;
	std::vector< wxString > allphrases_h;

	// Vektor füllen
	for ( unsigned int i = 0; i < m_Document.GetData().sdCompounds.size(); ++i )
	{
		// H/P-Sätze
		// ---------
		// Listen füllen
		BeTool::StringToList(  m_Document.GetData().sdCompounds[i].szHarzardSatements, "-", allphrases_h, false );
		BeTool::StringToList(  m_Document.GetData().sdCompounds[i].szPrecautionaryStatements, "-", allphrases_p, false );

		// R-/S-Sätze!
		// -----------
		// S-Sätze: Klammern suchen und entfernen (1/2-Sätze für Kinder/Jugendliche)
		wxString safeties =  m_Document.GetData().sdCompounds[i].szSafeties;

		int iBra = safeties.find("(");

		// Alle Bra-Kets finden und eliminieren
		while ( iBra != -1 )
		{
			int iKet = safeties.find(")");

			if ( iKet == -1 )
				break;

			safeties.erase(iBra, iKet-iBra+1);

			iBra = safeties.find("(");
		}

		// Listen füllen
		BeTool::StringToList( m_Document.GetData().sdCompounds[i].szRisks, "-", allphrases_r, false );
		BeTool::StringToList( safeties, "-", allphrases_s, false );
	}

	// Vektor übergebe
	dlg.SetAllRisks( allphrases_r );
	dlg.SetAllSafeties( allphrases_s );
	dlg.SetAllHarzards( allphrases_h );
	dlg.SetAllPrecautionaries( allphrases_p );

	// Dialog anzeigen
	if ( dlg.ShowModal() == wxID_OK )
	{
		// Liste leeren
		m_Document.m_hidephrases.clear();

		// Liste holen
		dlg.GetPhraseVector( m_Document.m_hidephrases );

		// Update
		OrderAllStatements();

		// Alle Beobachter informieren
		m_Document.NotifyAllObserver();
	}
}

// Wird aufgerufen, falls zu dieser Ansicht gewechselt wird
void CBerylliumViewSafetySheets::SwitchedToView()
{
	// Alle Sätze neu erstellen
	OrderAllStatements();
}
