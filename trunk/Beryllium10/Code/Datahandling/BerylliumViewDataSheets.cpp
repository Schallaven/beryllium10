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

#include "BerylliumViewDataSheets.h"
#include "../berylliummainframe_identifiers.h"

#include "../BerylliumApplication.h"

#include "../Dialoge/BerylliumSubstanceSearchDialog.h"
#include "../Dialoge/BerylliumSubstanceProperties.h"

#include "BerylliumViewDataSheetsPrinting.h"


CBerylliumViewDataSheets::CBerylliumViewDataSheets( CBerylliumDocument &doc, wxWindow &parent ) : m_Document(doc), m_Parent(parent)
{
	// Aktuelle Seite
	m_iPage = 0;

	// Wir zeigen standardmäßig die R-/S-Sätze
	bShowHPStatements = false;

	// GHS-Modus ausgeschaltet
	bGHSMode = false;

	// Wir drucken auch nicht
	m_isPrinting = false;
	m_isPreview  = false;

	// Nichts selektiert
	m_iSelected = 0;

	// Kastendimension für "Neue Substanz"
	m_iNewWidth		= 500;
	m_iNewHeight	= 150;

	// Variable Grenze zwischen Gefahrstoffdaten und Toxikologie
	iBorderToxicology = 600;

	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Hier holen wir uns die Symbole (klein + groß)
	for ( int i = 0; i < 9; ++i )
	{
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

CBerylliumViewDataSheets::~CBerylliumViewDataSheets(void)
{
}

// Zeichnet die aktuelle Seite
void CBerylliumViewDataSheets::Draw(wxDC &dc)
{
	// Druckmodus?
	if ( m_isPrinting )
		return;

	// Hintergrund weißeln...
	dc.SetPen( wxPen( wxColour(255,255,255), 1, wxSOLID ) );
	dc.SetBrush( wxBrush( wxColour(255,255,255) ) );
	dc.DrawRectangle( 0, 0, 740, 1040 );

	// Letzte Seite?
	if ( m_iPage == (GetNumberOfPages()-1) )
		DrawNewSubstancePage( dc );

	// ansonsten Substanzseite anzeigen
	else
		DrawSubstancePage( dc, m_iPage );
}

// Gibt die Größe einer Seite zurück
wxSize CBerylliumViewDataSheets::GetPageSize()
{
	// alle Seiten sind bei dieser Ansicht gleich groß, also brauchen wir
	// nur eine größe zurückgeben
	return wxSize( 740, 1040 );
}

// Gibt die Anzahl der Seiten zurück
unsigned int CBerylliumViewDataSheets::GetNumberOfPages()
{
	// Daten holen
	const CBerylliumData data = m_Document.GetData();

	// Pro Seite genau eine Substanz plus eine weitere zum anfügen einer neuen Substanz/Seite
	return (data.sdCompounds.size() + 1);
}

// Setzt die aktuelle Seite
void CBerylliumViewDataSheets::SetPageNumber( unsigned int iPage )
{
	// Index der letzten Seite ermitteln
	unsigned int iLastPage = (GetNumberOfPages() - 1);

	// Ist iPage größer als iLastPage? Dann auf letzte Seite setzen
	if ( iPage > iLastPage )
		iPage = iLastPage;

	// Seite setzen...
	m_iPage = iPage;

	// Markierung zurücknehmen
	m_iSelected = 0;

	// Alle Observer informieren...
	m_Document.NotifyAllObserver();
}

// Prüft, ob eine Maustaste geklickt wurde...
void CBerylliumViewDataSheets::OnMouseClick(wxMouseEvent &event)
{
	// Linke Taste unten?
	if ( event.LeftDown() )
	{
		// Markierung überprüfen
		switch ( m_iSelected )
		{
			// Name und allgemeine Eigenschaften
			case 1:
			case 3:
				SubstanceProps(bePAGE_SUBSTANCE_GENERAL);
				break;

			// Ansatz
			case 2:
				SubstanceProps(bePAGE_SUBSTANCE_BATCH);
				break;

			// Physikalische Daten
			case 4:
				SubstanceProps(bePAGE_SUBSTANCE_PHYSICAL);
				break;

			// Gefahrstoffdaten
			case 5:
				SubstanceProps(bePAGE_SUBSTANCE_SAFETY);
				break;

			// GHS-Daten
			case 6:
				SubstanceProps(bePAGE_SUBSTANCE_GHS);
				break;

			// Toxikologie
			case 7:
				SubstanceProps(bePAGE_SUBSTANCE_TOXICOLOGY);
				break;

			// Hinweise
			case 8:
				SubstanceProps(bePAGE_SUBSTANCE_ADVICES);
				break;

			// Auf der letzten Seite in den Kasten geklickt
			case 100:
				SubstanceAdd();
				break;
		}
	}
}

// Prüft, ob die Mausposition in einem auswählbaren Bereich liegt
void CBerylliumViewDataSheets::OnMouseMove(wxMouseEvent &event)
{
	// Alte Selektion
	int iOldSelection = m_iSelected;

	// Auswahl zurücksetzen
	m_iSelected = 0;

	// Position der Maus holen (wurde vom ViewManager schon umgerechnet)
	wxPoint ptMouse = event.GetPosition();

	// Letzte Seite?
	if ( m_iPage == (GetNumberOfPages()-1) )
	{
		// Im Rechteck?
		wxRect box((740-m_iNewWidth)/2, (520-m_iNewHeight)/2, m_iNewWidth, m_iNewHeight);

		// Maus in Rechteck?
		if ( box.Contains(ptMouse) )
			m_iSelected = 100;

		// ansonsten nichts selektiert
		else
			m_iSelected = 0;
	}

	// ansonsten
	else
	{
		// Gibt es Daten zum Ansatz zu dieser Substanz (für Titelzeile wichtig)
		bool bBatch = (m_Document.GetData().sdCompounds[m_iPage].GetBatchAmount() > 0.0f);

		// Array mit den Rechtecken anlegen
		wxRect rectBoxes[] = {	wxRect(  -1,  -1,  -1,  -1),
								wxRect(  20,  20,  700,  80),	// Titel: Name
								wxRect( 618,  20,  102,  80),	// Titel: Ansatz
								wxRect(  20, 100,  350,  107),	// Allgemeine Daten
								wxRect( 370, 100,  350,  107),  // Physikalische Daten
								wxRect(  20, 205,  700,  (iBorderToxicology+7) - 205),	// Gefahrenstoffe
								wxRect(  20, 205,  700,  (iBorderToxicology+7) - 205),	// GHS-Daten
								wxRect( 20, iBorderToxicology+7, 700, 750-(iBorderToxicology+7)+2),	// Toxikologie
								wxRect(  20, 750,  700,  270),	// Zusätzliche Hinweise
								};

		// Alle Bereiche durchgehen und schauen was selektiert wurde
		for ( unsigned int i = 0; i < 9; ++i )
			if ( rectBoxes[i].Contains( ptMouse ) )
				m_iSelected = i;

		// Falls kein Ansatz verfügbar und Ansatz ausgewählt -> Name auswählen
		if ( (m_iSelected == 2) && !bBatch )
			m_iSelected = 1;

		// GHS-Daten ausgewählt, aber umwandeln falls kein GHS-Modus
		if ( !bGHSMode && (m_iSelected == 6) )
			m_iSelected = 5;
	}

	// Auswahl? Dann Mauscursor auf Zeigefinger setzen, andernfalls auf den normalen Cursor
	if ( m_iSelected > 0 )
		m_Parent.SetCursor( wxCursor( wxCURSOR_HAND ) );
	else
		m_Parent.SetCursor( wxCursor( wxCURSOR_ARROW ) );

	// Wenn die Werte sich geändert haben, nur DANN zeichnen wir neu!
	if ( iOldSelection != m_iSelected )
		m_Parent.Refresh();
}

// GHS-Modus?
bool CBerylliumViewDataSheets::IsGHSMode()
{
	return bGHSMode;
}

// GHS-Modus setzen
void CBerylliumViewDataSheets::SetGHSMode( bool toGHS )
{
	// Wortlaut der wesentlich...
	bShowHPStatements = toGHS;

	// GHS-Modus
	bGHSMode = toGHS;

	// Fenster neuzeichnen
	m_Parent.Refresh();

	// Zurück
	return;
}

// Event: Datei wurde geladen
void CBerylliumViewDataSheets::OnFileWasLoaded()
{
}

// Event: Datei wurde gespeichert
void CBerylliumViewDataSheets::OnFileWillBeSaved()
{
}

// Gibt ein wxPrintOut-Objekt zurück
wxPrintout *CBerylliumViewDataSheets::GetPrintOut()
{
	// Erstelle ein Objekt
	return new CBerylliumViewDataSheetsPrinting( *this, m_Document );
}

// Setzt den Druckmodus bzw. wieder zurück
void CBerylliumViewDataSheets::SwitchToPrintMode( bool bPrint = true )
{
	// Ansonsten Druckmodus setzen
	m_isPrinting = bPrint;

	// Previewmodus ausschalten
	m_isPreview = false;

	// Markierung löschen
	m_iSelected = 0;
}

// Setzt den Druckmodus bzw. wieder zurück
void CBerylliumViewDataSheets::SwitchToPreviewMode( bool bPreview = true )
{
	// Druckmodus ausschalten
	m_isPrinting = false;

	// Previewmodus setzen
	m_isPreview = bPreview;

	// Markierung löschen
	m_iSelected = 0;
}

// Verarbeitet die IDs des Popupmenus
void CBerylliumViewDataSheets::DoPopupMenu( wxCommandEvent &event )
{
}

// Zeichnet die "Hinzufügen"-Seite
void CBerylliumViewDataSheets::DrawNewSubstancePage( wxDC &dc )
{
	// Große Schrift erstellen für "Neue Substanz hinzufügen"
	wxFont fontBig( 20, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Schrift setzen
	dc.SetFont( fontBig );



	// Selektiert und wird nicht gedruckt?
	if ( !m_isPrinting && (m_iSelected == 100) )
	{
		// blauer Hintergrund
		dc.SetBrush( wxBrush( wxColour( 10, 36, 106 ) ) );

		// Textfarbe = weiß
		dc.SetTextForeground( wxColour(255,255,255) );
	}

	// Nichts selektiert - oder es wird gedruckt
	else
	{
		// weißer Hintergrund
		dc.SetBrush( wxBrush( wxColour(255,255,255) ) );

		// Textfarbe = schwarz
		dc.SetTextForeground( wxColour(0,0,0) );
	}

	// Kasten in die Mitte zeichnen
	dc.SetPen( wxPen( wxColour(0,0,0), 2, wxPENSTYLE_LONG_DASH ) );
	dc.DrawRectangle( (740-m_iNewWidth)/2, (520-m_iNewHeight)/2, m_iNewWidth, m_iNewHeight );

	// Text reinmalen
	dc.DrawLabel( _(L"Neue Substanz hinzufügen"), wxRect((740-m_iNewWidth)/2, (520-m_iNewHeight)/2, m_iNewWidth, m_iNewHeight), wxALIGN_CENTER);
}

// Fügt eine neue Substanz ein (zeigt Dialog, etc)
void CBerylliumViewDataSheets::SubstanceAdd()
{
	// Dialog vorbereiten
	CBerylliumSubstanceSearchDialog m_dialogsearch( &m_Parent );

	// Anzeigen und Rückgabe auswerten
	if ( m_dialogsearch.ShowModal() == wxID_OK )
		// Daten einfügen (benachrichtig automatisch alle Beobachter, etc.)
		m_Document.AddSubstance(  m_dialogsearch.GetData() );
}

// Wird aufgerufen, falls zu dieser Ansicht gewechselt wird
void CBerylliumViewDataSheets::SwitchedToView()
{
}

// Zeichnet eine Substanzseite
void CBerylliumViewDataSheets::DrawSubstancePage( wxDC &dc, unsigned int iPage )
{
	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Substanz vorhanden? Wenn nicht, dann raus hier!
	if ( iPage >= data.sdCompounds.size() )
		return;

	// Zeichnen der Markierung (nur außerhalb des Druckmodus)
	if ( !m_isPrinting )
		DrawSubstancePageSelection(dc, iPage);

	// Rahmen zeichnen
	DrawSubstancePageBorder(dc);

	// Kopf zeichnen
	DrawSubstancePageHead(dc, iPage);

	// Allgemeine und physikalische Daten
	DrawSubstancePageGeneralData(dc, iPage);

	// Gefahrendaten zeichnen
	if ( bGHSMode )
		DrawSubstancePageGHS( dc, iPage);
	else
		DrawSubstancePageHazards(dc, iPage);

	// Toxikologiedaten
	DrawSubstancePageToxicology(dc, iPage);

	// Zusätzliche Hinweise (Entsorgung, etc.)
	DrawSubstancePageStatements(dc, iPage);
}

// Substanzseite: Rahmen
void CBerylliumViewDataSheets::DrawSubstancePageBorder( wxDC &dc )
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

	// Innenrahmen
	// -------------------------
	// Stift vorbereiten
	wxPen penThin( wxColour(0,0,0), 1, wxSOLID );

	// dicken Stift wählen
	dc.SetPen( penThick );

	// Linien zeichnen
	dc.DrawLine( 20, 100, 718, 100);

	// Dünnen Stift wählen
	dc.SetPen( penThin );

	// Linien zeichnen
	dc.DrawLine( 20, 205, 720, 205);
	dc.DrawLine( 20, 750, 720, 750);

}

// Substanzseite: Kopf
void CBerylliumViewDataSheets::DrawSubstancePageHead( wxDC &dc, unsigned int iPage )
{
	// Große Schrift erstellen, für den Namen
	wxFont fontBig( 18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Kleine Schrift erstellen, für die restlichen Namen
	wxFont fontMedium( 12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Kleine Schrift erstellen, für die restlichen Namen
	wxFont fontSmall( 9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Stift vorbereiten
	wxPen penThin( wxColour(0,0,0), 1, wxSOLID );

	// Maximaler horizontale Größe
	int iMaxWidth = 705;

	// Daten holen
	const CBerylliumData &data = m_Document.GetData();

	// Sind Angaben zur Ansatzgröße vorhanden? Dann in die rechte obere Ecke!
	if ( m_Document.GetData().sdCompounds[iPage].GetBatchAmount() > 0.0f )
	{
		// dünnen Stift wählen
		dc.SetPen( penThin );

		// Linie zeichnen
		dc.DrawLine( 618, 20, 618, 100 );

		// Schrift selektieren
		dc.SetFont( fontMedium );

		// Textfarbe: Schwarz
		dc.SetTextForeground( wxColour(0,0,0) );

		// 1. Zeile: Ansatz
		if ( (!m_isPrinting) && (m_iSelected == 2) )
			dc.SetTextForeground( wxColour(255,255,255) );

		// Daten holen
		const CBerylliumSubstanceData &sdData = data.sdCompounds[iPage];

		// Ansatzgröße und Stoffmenge
		double batchamount = sdData.GetBatchAmount();
		double substamount = sdData.CalculateAmountOfSubstance();

		// Zeichne erst die "richtige" Einheit
		wxString unit	= "L"; if ( sdData.GetBatchType() == sdData.batchSolid ) unit = "g";
		wxString preunit = BeTool::UnitPrefix( batchamount );

		if ( batchamount > 0.0 )
			dc.DrawLabel( wxString::Format("%.1f %s%s", batchamount, preunit, unit), wxRect( 618, 20, 100, 50), wxALIGN_CENTER );

		// Zeichne Stoffmenge
		preunit = BeTool::UnitPrefix( substamount );

		if ( substamount > 0.0 )
			dc.DrawLabel( wxString::Format("%.1f %smol", substamount, preunit) , wxRect( 618, 50, 100, 50) , wxALIGN_CENTER );

		// Maximale Größe veringert sich für den Rest
		iMaxWidth = 605;
	}

	// Schrift selektieren
	dc.SetFont( fontBig );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// 1. Zeile: Titel
	if ( (!m_isPrinting) && (m_iSelected == 1) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Titel zeichnen (je nachdem ob GHS oder nicht)
	dc.DrawLabel( data.sdCompounds[iPage].GetName(), wxRect( 25, 25, (iMaxWidth-15), 40 ), wxALIGN_LEFT );

	// Schrift selektieren
	dc.SetFont( fontSmall );

	// Andere Namen holen
	wxString othernames = data.sdCompounds[iPage].GetNamesExceptFirst(", ");

	// Länger als 200 Zeichen?
	if ( othernames.length() > 200)
		othernames = othernames.SubString(0, 195) + "...";

	// Die Namen in eine Liste schreiben
	std::vector< wxString > sWords; BeTool::StringToList( othernames, " ", sWords );

	// Anzahl der Wörter
	unsigned int iNumberOfWords = sWords.size();

	// Position x,y
	int x = 35, y = 55;

	// Wort für Wort zeichnen (+ Leerzeichen)
	for ( unsigned int i = 0; i < iNumberOfWords; i++ )
	{
		// Größe holen
		wxSize size = dc.GetTextExtent( sWords[i] + " " );

		// Passt das Wort noch in die Zeile? Wenn nicht, dann neue Zeile!
		if ( (x+size.GetWidth()) > iMaxWidth )
		{
			// Position x zurücksetzen
			x = 35;

			// Position y erhöhen
			y += size.GetHeight() + 2;
		}

		// Wort zeichnen
		dc.DrawText( sWords[i] + " ", x, y );

		// x erhöhen
		x += size.GetWidth();
	}

}

// Substanzseite: allgemeine und physikalische Eigenschaften
void CBerylliumViewDataSheets::DrawSubstancePageGeneralData( wxDC &dc, unsigned int iPage )
{
	// Mittlere Schrift erstellen für die Label
	wxFont fontMediumBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Eigenschaften
	wxFont fontMedium( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Daten holen
	const CBerylliumSubstanceData &sdData = m_Document.GetData().sdCompounds[iPage];

	// Schrift wählen
	dc.SetFont( fontMediumBold );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 3) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Label erstellen
	dc.DrawLabel( _(L"Summenformel:") , wxRect( 35, 115, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"CAS:") ,		   wxRect( 35, 135, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"Molare Masse:") , wxRect( 35, 155, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"Be¹º-Punkte:")  , wxRect( 35, 175, 125, 10), wxALIGN_LEFT );

	// Schrift wählen
	dc.SetFont( fontMedium );

	// Daten anzeigen
	sdData.DrawFormula( dc, 165, 115 );
	dc.DrawLabel( sdData.szCAS ,		   wxRect( 165, 135, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( wxString::Format("%.2f g/mol", sdData.dbMolWeight) , wxRect( 165, 155, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( wxString::Format("%d", sdData.GetDangerPoints())  , wxRect( 165, 175, 125, 10), wxALIGN_LEFT );


	// Schrift wählen
	dc.SetFont( fontMediumBold );

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 4) )
		dc.SetTextForeground( wxColour(255,255,255) );

	dc.DrawLabel( _(L"Dichte:") ,	   wxRect( 385, 115, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"Schmelzpunkt:") , wxRect( 385, 135, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"Siedepunkt:") ,   wxRect( 385, 155, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( _(L"Flammpunkt:")  ,  wxRect( 385, 175, 125, 10), wxALIGN_LEFT );

	// Schrift wählen
	dc.SetFont( fontMedium );

	dc.DrawLabel( wxString::Format(L"%.2f g/cm³", sdData.dbDensity) ,	   wxRect( 505, 115, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( sdData.szMeltingPoint + (sdData.bDecomposition ? _(L" (Zersetzung)") : "") , wxRect( 505, 135, 125, 10), wxALIGN_LEFT );
	if ( !sdData.bDecomposition ) dc.DrawLabel( sdData.szBoilingPoint ,   wxRect( 505, 155, 125, 10), wxALIGN_LEFT );
	dc.DrawLabel( sdData.szFlashPoint  ,  wxRect( 505, 175, 125, 10), wxALIGN_LEFT );

}

// Substanzseite: Gefahrenhinweise
void CBerylliumViewDataSheets::DrawSubstancePageHazards( wxDC &dc, unsigned int iPage )
{
	// Große Schrift erstellen, für den Namen
	wxFont fontBig( 18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Label
	wxFont fontMediumBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Eigenschaften
	wxFont fontMedium( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 5) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Schrift wechseln
	dc.SetFont( fontBig );

	// Daten holen
	const CBerylliumSubstanceData &sdData = m_Document.GetData().sdCompounds[iPage];

	// Bibliothek holen
	const CBerylliumStatementsManager &library = ::wxGetApp().GetStatementLibrary();

	// Sollte es keine Daten geben, anzeigen und rausspringen
	if ( (sdData.szHarzards.size() == 0) && (sdData.szRisks.length() == 0) && (sdData.szSafeties.length() == 0) )
	{
		// Gefahrensymbole zeichnen
		dc.DrawLabel( _(L"Keine Gefahrstoffdaten vorhanden!") , wxRect( 35, 220, 350, 20), wxALIGN_LEFT );

		// Grenze zur Toxikologie entsprechend setzen
		iBorderToxicology = 265;

		// Rausspringen
		return;
	}

	// String holen für die Gefahrensymbole
	wxString szHazards = BeTool::ListToString( sdData.szHarzards, ", " );

	// Listen anlegen
	std::vector< wxString > listRisks, listSafeties;

	// Listen füllen
	BeTool::StringToList( sdData.szRisks, "-", listRisks );
	BeTool::StringToList( sdData.szSafeties, "-", listSafeties );

	// String für die R-Sätze
	wxString szRisks;

	// R-Sätze durchgehen und Statements holen
	for ( unsigned int i = 0; i < listRisks.size(); ++i )
		szRisks += (i>0 ? " " : "") + library["R"+listRisks[i]];

	// String für die S-Sätze
	wxString szSafeties;

	// S-Sätze durchgehen und Statements holen
	for ( unsigned int i = 0; i < listSafeties.size(); ++i )
		szSafeties += (i>0 ? " " : "") + library["S"+listSafeties[i]];

	// Gefahrensymbole zeichnen
	dc.DrawLabel( szHazards , wxRect( 35, 220, 350, 20), wxALIGN_LEFT );

	// Anfänglicher y-Wert
	int y = 265;

	// Schrift setzen
	dc.SetFont( fontMediumBold );

	// R-Sätze zeichnen
	dc.DrawLabel( _(L"R-Sätze:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );

	// R-Sätze
	dc.DrawLabel( sdData.szRisks, wxRect( 140, y, 570, 15), wxALIGN_LEFT );

	// Schrift setzen
	dc.SetFont( fontMedium );

	// Sätze zeichnen
	y = DrawSubstanceTextBlock( dc, 140, y+15, 570, szRisks );

	// Kleiner Abstand
	y += 15;

	// Schrift setzen
	dc.SetFont( fontMediumBold );

	// S-Sätze zeichnen
	dc.DrawLabel( _(L"S-Sätze:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );

	// S-Sätze
	dc.DrawLabel( sdData.szSafeties, wxRect( 140, y, 570, 15), wxALIGN_LEFT );

	// Schrift setzen
	dc.SetFont( fontMedium );

	// Sätze zeichnen
	y = DrawSubstanceTextBlock( dc, 140, y+15, 570, szSafeties );

	// Grenze zur Toxikologie entsprechend setzen
	iBorderToxicology = y;
}

// Substanzseite: GHS-Daten
void CBerylliumViewDataSheets::DrawSubstancePageGHS( wxDC &dc, unsigned int iPage )
{
	// Große Schrift erstellen, für den Namen
	wxFont fontBig( 18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Label
	wxFont fontMediumBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Eigenschaften
	wxFont fontMedium( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 6) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Schrift wechseln
	dc.SetFont( fontBig );

	// Daten holen
	const CBerylliumSubstanceData &sdData = m_Document.GetData().sdCompounds[iPage];

	// Bibliothek holen
	const CBerylliumStatementsManager &library = ::wxGetApp().GetStatementLibrary();

	// Sollte es keine Daten geben, anzeigen und rausspringen
	if ( (sdData.szGHSSymbols.size() == 0) && (sdData.szHarzardSatements.length() == 0) && (sdData.szPrecautionaryStatements.length() == 0) )
	{
		// Gefahrensymbole zeichnen
		dc.DrawLabel( _(L"Keine GHS-Gefahrstoffdaten vorhanden!") , wxRect( 35, 220, 350, 20), wxALIGN_LEFT );

		// Rausspringen
		return;
	}

	// Gefahrenwort zeichnen
	dc.DrawLabel( sdData.szSignalWord, wxRect(35, 220, 200, 20), wxALIGN_LEFT );

	// Listen anlegen
	std::vector< wxString > listHazards, listPrecautionaries;

	// Listen füllen
	BeTool::StringToList( sdData.szHarzardSatements, "-", listHazards );
	BeTool::StringToList( sdData.szPrecautionaryStatements, "-", listPrecautionaries );

	// String für die H-Sätze
	wxString szHarzardSatements;

	// H-Sätze durchgehen und Statements holen
	for ( unsigned int i = 0; i < listHazards.size(); ++i )
		szHarzardSatements += (i>0 ? " " : "") + library["H"+listHazards[i]];

	// String für die P-Sätze
	wxString szPrecautionaryStatements;

	// P-Sätze durchgehen und Statements holen
	for ( unsigned int i = 0; i < listPrecautionaries.size(); ++i )
		szPrecautionaryStatements += (i>0 ? " " : "") + library["P"+listPrecautionaries[i]];

	// GHS-Symbole zeichnen
	DrawSubstancePageGHSSymbols( dc, sdData.szGHSSymbols, 350, 220, 350 );

	// Anfänglicher y-Wert
	int y = 300;

	// Schrift setzen
	dc.SetFont( fontMediumBold );

	// R-Sätze zeichnen
	dc.DrawLabel( _(L"H-Sätze:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );

	// R-Sätze
	dc.DrawLabel( sdData.szHarzardSatements, wxRect( 140, y, 570, 15), wxALIGN_LEFT );

	// Schrift setzen
	dc.SetFont( fontMedium );

	// Sätze zeichnen
	y = DrawSubstanceTextBlock( dc, 140, y+15, 570, szHarzardSatements );

	// Kleiner Abstand
	y += 15;

	// Schrift setzen
	dc.SetFont( fontMediumBold );

	// S-Sätze zeichnen
	dc.DrawLabel( _(L"P-Sätze:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );

	// S-Sätze
	dc.DrawLabel( sdData.szPrecautionaryStatements, wxRect( 140, y, 570, 15), wxALIGN_LEFT );

	// Schrift setzen
	dc.SetFont( fontMedium );

	// Sätze zeichnen
	y = DrawSubstanceTextBlock( dc, 140, y+15, 570, szPrecautionaryStatements );

	// Grenze zur Toxikologie entsprechend setzen
	iBorderToxicology = y;

}

// Hilfsfunktion: Zeichnet den Text als Block und gibt die neue y-Koordinate zurück
int CBerylliumViewDataSheets::DrawSubstanceTextBlock( wxDC &dc, int x, int y, int maxwidth, wxString text, int indentfirstline )
{
	// x und y-Koordinate
	int newy = y, newx = x + indentfirstline;

	// Das Statement in Satzfragmente in eine Liste schreiben
	std::vector< wxString > sWords; BeTool::StringToList( text, " ", sWords );

	// Anzahl der Wörter
	unsigned int iNumberOfWords = sWords.size();

	// Größe des Textes
	wxSize size;

	// Wort für Wort zeichnen (+ Leerzeichen)
	for ( unsigned int i = 0; i < iNumberOfWords; ++i )
	{
		// Größe holen
		size = dc.GetTextExtent( (newx == x ? "" : " ") + sWords[i] );

		// Passt das Wort noch in die Zeile? Wenn nicht, dann neue Zeile!
		if ( (newx+size.GetWidth()) > (x+maxwidth) )
		{
			// Position x zurücksetzen
			newx = x;

			// Position y erhöhen
			newy += size.GetHeight();
		}

		// Wort zeichnen
		dc.DrawText( (newx == x ? "" : " ") + sWords[i], newx, newy );

		// x erhöhen
		newx += size.GetWidth();
	}


	// neue y-Koordinate zurückgeben (nächste Zeile!)
	return newy + size.GetHeight();
}

// Hilfsfunktion: Zeichnet die GHS-Symbole (Bildschirm)
void CBerylliumViewDataSheets::DrawSubstancePageGHSSymbols( wxDC &dc, const std::vector< wxString > symbols, int x, int y, int maxwidth )
{
	// Anzahl der Symbole
	int iCount = symbols.size();

	// Skalierungsfaktoren
	double dscalex = 1.0, dscaley = 1.0;

	// Breite und Höhe
	int iNewHeight	= 75;
	int iNewWidth	= iCount * iNewHeight;

	// Skalierung vom DC holen (Drucken)
	if ( m_isPrinting )
		dc.GetUserScale( &dscalex, &dscaley );

	// Setzen der Skalierung (Drucken)
	if ( m_isPrinting )
		dc.SetUserScale( 1.0f, 1.0f );

	// MemoryDC erstellen, das kompatibel mit dc ist
	wxMemoryDC memDC;

	// Bitmap erstellen (entsprechend skalieren)
	wxBitmap   memBitmap( iNewWidth * dscalex, iNewHeight * dscaley );

	// Bitmap in den MemDC selektieren
	memDC.SelectObject( memBitmap );

	// Hintergrund
	wxBrush brushBackground( wxColour( 255, 50, 100) );

	// Beim Drucken / Preview kanns ruhig ein weißer Hintergrund sein!
	// (Es ist möglich, dass Transparenz nicht vom Druckertreiber unterstützt wird)
	if ( m_isPreview || m_isPrinting )
		memDC.SetBackground( *wxWHITE_BRUSH );

	// ansonsten mit Alphahintergrund zeichnen
	else
		memDC.SetBackground( brushBackground );

	memDC.Clear();

	// Skalierung der Bilder
	int iscaletoheight = iNewHeight * dscaley;
	// Da Höhe und Breite quasi gleich sind, einfach übernehmen.
	int iscaletowidth = iscaletoheight;

	// Dann zeichnen wir hier die Symbole, denn mit den Zahlen kann niemand was anfangen
	for ( int i = 0; i < iCount; ++i )
	{
		// Index des Bildes
		long index = 0;

		// Index holen
		wxString(symbols[i]).ToLong(&index);

		// 1 abziehen
		index--;

		// Index überprüfen
		if ( (index < 0) || (index > 8) )
			continue;

		// Skalieren
		wxImage	img = picBig[index].Scale( iscaletowidth, iscaletoheight, wxIMAGE_QUALITY_HIGH );

		// daraus ein Bitmap machen
		wxBitmap bmp( img );

		// Und zeichnen
		memDC.DrawBitmap( bmp, (i*bmp.GetWidth()), 0, true );
	}

	// Bitmap aus dem MemDC rausholen
	memDC.SelectObject( wxNullBitmap );

	// Maske erstellen
	wxMask *mask = new wxMask( memBitmap, wxColour( 255, 50, 100));

	// Maske setzen
	memBitmap.SetMask( mask );

	// x-Koordinate berechnen
	int newx = x + maxwidth - iNewWidth;

	// Kleiner als x? Dann an Position x zeichnen
	if ( newx < x )
		newx = x;

	// Bitmap in DC zeichnen
	dc.DrawBitmap( memBitmap, newx * dscalex, y * dscaley, true );

	// Skalierung zurücksetzen (Drucken)
	if ( m_isPrinting )
		dc.SetUserScale( dscalex, dscaley );
}

// Substanzseite: Toxikologische Daten
void CBerylliumViewDataSheets::DrawSubstancePageToxicology( wxDC &dc, unsigned int iPage )
{
	// Strings
	const wxString m_WaterHarzardClassStrings[] = { _(L"Keine Wassergefährdung"), _(L"Schwache Wassergefährdung"), _(L"Wassergefährdung"), _(L"Schwere Wassergefährdung") };

	// y-Koordinate
	int y = iBorderToxicology + 15;

	// Mittlere Schrift erstellen für die Label
	wxFont fontMediumBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Eigenschaften
	wxFont fontMedium( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 7) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Daten holen
	const CBerylliumSubstanceData &sdData = m_Document.GetData().sdCompounds[iPage];

	// LD50-Werte holen
	std::vector< CBerylliumSubstanceData::LethalDose50 > ld50data; sdData.GetLD50Data( ld50data );

	// LD50-Werte zeichnen
	if ( ld50data.size() > 0 )
	{
		// String vorbereiten
		wxString szld50;

		// Daten durchgehen
		for ( unsigned int i = 0; i < ld50data.size(); ++i )
			szld50 += (i==0 ? "" : ", ") + wxString::Format(L"%2.f %s (%s, %s)", ld50data[i].dbValue, ld50data[i].sUnit, ld50data[i].sSpecies, ld50data[i].sExposureType);

		// Zeichnen
		dc.SetFont( fontMediumBold );	dc.DrawLabel( _(L"LD50-Werte:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );
		dc.SetFont( fontMedium );		y = DrawSubstanceTextBlock( dc, 140, y, 560, szld50 );

		y += 15;
	}

	// MAK-Wert
	if ( sdData.dbMAK > 0.0f )
	{
		dc.SetFont( fontMediumBold );	dc.DrawLabel( _(L"Maximale Arbeitsplatzkonzentration:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );
		dc.SetFont( fontMedium );		dc.DrawLabel( wxString::Format(L"%.2f %s", sdData.dbMAK, (sdData.bMAKppm ? L"ppm" : L"mg/m³")) , wxRect( 300, y, 570, 15), wxALIGN_LEFT );

		y += 30;
	}

	// WGK zeichnen
	if ( sdData.iWGK >= 0 && sdData.iWGK < 4)
	{
		dc.SetFont( fontMediumBold );	dc.DrawLabel( _(L"Wassergefährdungsklasse:") , wxRect( 35, y, 125, 10), wxALIGN_LEFT );
		dc.SetFont( fontMedium );		dc.DrawLabel( wxString::Format("%d, %s", sdData.iWGK, m_WaterHarzardClassStrings[sdData.iWGK]) , wxRect( 220, y, 570, 15), wxALIGN_LEFT );
	}


}

// Substanzseite: Zeichnet die speziellen Hinweise einer Substanz (z.B. Entsorgung)
void CBerylliumViewDataSheets::DrawSubstancePageStatements( wxDC &dc, unsigned int iPage )
{
	// Strings
	const wxString szStatementClasses[] = { _(L"Gefahren für Mensch und Umwelt:"), _(L"Schutzmaßnahmen:"), _(L"Verhalten im Gefahrenfall:"), _(L"Entsorgung:") };

	// y-Koordinate
	int y = 765;

	// Mittlere Schrift erstellen für die Label
	wxFont fontMediumBold( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Arial");

	// Mittlere Schrift erstellen für die Eigenschaften
	wxFont fontMedium( 10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, "Arial");

	// Textfarbe: Schwarz
	dc.SetTextForeground( wxColour(0,0,0) );

	// Selektiert?
	if ( (!m_isPrinting) && (m_iSelected == 8) )
		dc.SetTextForeground( wxColour(255,255,255) );

	// Daten holen
	const CBerylliumSubstanceData &sdData = m_Document.GetData().sdCompounds[iPage];

	// Statements holen
	for ( unsigned int i = 0; i < 4; ++i )
	{
		// Gibt es hier überhaupt Inhalte?
		if ( sdData.sCustomStatements[i].size() == 0 )
			continue;

		// Label zeichnen
		dc.SetFont( fontMediumBold );	dc.DrawLabel(szStatementClasses[i] , wxRect( 35, y, 200, 10), wxALIGN_LEFT );

		// Text anlegen
		wxString szText = BeTool::ListToString( sdData.sCustomStatements[i], " ");

		// Text zeichnen
		dc.SetFont( fontMedium ); y = DrawSubstanceTextBlock( dc, 100, y, 590, szText, 180  );

		// Eine Zeile weiter
		y += 15;
	}

	// Wurde nichts gezeichnet? Dann zumindest kurzen Hinweis anzeigen!
	if ( y == 765 )
	{
		dc.SetFont( fontMediumBold );	dc.DrawLabel( _(L"Es sind keine weiteren Information oder Hinweise zu dieser Substanz verfügbar.") , wxRect( 35, y, 200, 10), wxALIGN_LEFT );
	}

}

// Substanzseite: Zeichnet die Markierungen
void CBerylliumViewDataSheets::DrawSubstancePageSelection( wxDC &dc, unsigned int iPage )
{
	// Druckmodus?
	if ( m_isPrinting )
		return;

	// Auswahlindex muss unter 100 liegen
	if ( m_iSelected > 100 )
		return;

	// Gibt es Daten zum Ansatz zu dieser Substanz (für Titelzeile wichtig)
	bool bBatch = (m_Document.GetData().sdCompounds[iPage].GetBatchAmount() > 0.0f);

	// Pinsel erstellen (Blauer Hintergrund)
	wxBrush brushSelection1( wxColour( 10, 36, 106 ) );

	// Alten Pinsel aufheben
	wxBrush oldBrush = dc.GetBrush();

	// Pinsel wählen
	dc.SetBrush( brushSelection1 );

	// Welche Auswahl muss gezeichnet werden?
	switch( m_iSelected )
	{
		// Titelzeile: Name
		case 1:
			if ( bBatch )
				dc.DrawRectangle( 20, 20, 600, 80 );
			else
				dc.DrawRectangle( 20, 20, 700, 80 );
			break;

		// Titelzeile: Ansatz (falls verfügbar)
		case 2:
			if ( bBatch )
				dc.DrawRectangle( 618, 20, 102, 80 );
			break;

		// Allgemeine Daten
		case 3:
			dc.DrawRectangle( 20, 100, 350, 107 );
			break;

		// Physikalische Daten
		case 4:
			dc.DrawRectangle( 370, 100, 350, 107 );
			break;

		// Gefahrenstoffe & GHS-Daten
		case 5:
		case 6:
			dc.DrawRectangle( 20, 205, 700, (iBorderToxicology+7) - 205 );
			break;

		// Toxikologie
		case 7:
			dc.DrawRectangle( 20, iBorderToxicology+7, 700, 750-(iBorderToxicology+7)+2 );
			break;

		// Zusätzliche Hinweise
		case 8:
			dc.DrawRectangle( 20, 750, 700, 270 );
			break;
	};

	// Alten Pinsel wieder herstellen
	dc.SetBrush(oldBrush);
}

// Zeigt den Eigenschaftsdialog für die aktuelle Substanz auf einer bestimmten Seite an
void CBerylliumViewDataSheets::SubstanceProps( unsigned int iSubpage )
{
	// Daten holen
	CBerylliumData data = m_Document.GetData();

	// Index außerhalb der Daten
	if ( m_iPage >= data.sdCompounds.size() )
		return;

	// Dialog anlegen
	CBerylliumSubstanceProperties m_dialogproperties( &m_Parent,  data.GetSubstanceByIndex( m_iPage ), iSubpage );

	// Dialog anzeigen
	if ( m_dialogproperties.ShowModal() == wxID_OK )
	{
		// Daten updaten
		data.UpdateSubstanceDataAtIndex( m_iPage, m_dialogproperties.GetData() );

		// Setzen
		m_Document.SetData( data );
	}
}
