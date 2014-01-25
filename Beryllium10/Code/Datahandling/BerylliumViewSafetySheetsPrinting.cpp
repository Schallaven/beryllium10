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
#include "BerylliumViewSafetySheetsPrinting.h"

CBerylliumViewSafetySheetsPrinting::CBerylliumViewSafetySheetsPrinting( CBerylliumViewSafetySheets &view, CBerylliumDocument &document ) : m_view(view), m_document(document), wxPrintout( document.GetFilename() )
{
}

CBerylliumViewSafetySheetsPrinting::~CBerylliumViewSafetySheetsPrinting(void)
{
}

// Mögliche Seiten zurückgeben
void CBerylliumViewSafetySheetsPrinting::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
	// Minimale Seite 
	*minPage = 1;

	// Maximale Seite
	*maxPage = m_view.GetNumberOfPages();

	// von/bis kann 1-1 sein
	*pageFrom = 1;
	*pageTo	 = 1;
}

// Dokument-Titel
wxString CBerylliumViewSafetySheetsPrinting::GetTitle()
{
	// Dokumenttiel zurückgeben
	return m_document.GetFilename();
}

// Hat das Dokument die Seite pageNum?
bool CBerylliumViewSafetySheetsPrinting::HasPage(int pageNum)
{
	return ( (pageNum > 0) && ( pageNum <= (int)m_view.GetNumberOfPages() ) );
}

// Event: Drucken hat begonnen (wird für jede Kopie aufgerufen)
bool CBerylliumViewSafetySheetsPrinting::OnBeginDocument(int startPage, int endPage)
{
	// Basisfunktion aufrufen
	return wxPrintout::OnBeginDocument( startPage, endPage );
}

// Event: Drucken wurde beendet
void CBerylliumViewSafetySheetsPrinting::OnEndDocument()
{
	// Basisfunktion aufrufen
	wxPrintout::OnEndDocument();
}

// Event: Drucken hat begonnen (wird einmalig aufgerufen)
void CBerylliumViewSafetySheetsPrinting::OnBeginPrinting()
{
	// Zum Druckmodus wechseln (falls keine Vorschau)
	if ( !IsPreview() )
		m_view.SwitchToPrintMode( true );

	// Vorschaumodus (entfernt alle GUI-Elemente und Hervorhebungen, aber
	// nutzt trotzdem nur die kleinen Grafiken für die Symbole [geht schneller])
	else
		m_view.SwitchToPreviewMode( true );
}

// Event: Drucken wurde beendet (auch einmalig)
void CBerylliumViewSafetySheetsPrinting::OnEndPrinting()
{
	// Zum "normalen" Modus wechseln
	m_view.SwitchToPrintMode( false );
}

// Event: Druck vorbereiten (Seitenzahlen berechnen oder sowas)
void CBerylliumViewSafetySheetsPrinting::OnPreparePrinting()
{
}

// Event: Seite drucken
bool CBerylliumViewSafetySheetsPrinting::OnPrintPage(int pageNum)
{
	// Einpassen
	FitThisSizeToPage( m_view.GetPageSize() );

	// DC holen
	wxDC *ptr = GetDC();

	if ( (ptr == NULL) || (!ptr->IsOk()) )
		return false;

	// Referenz
	wxDC &dc = *ptr;

	// Seite anfangen
	dc.StartPage();

	// Wir fangen bei 0 an zu Zählen
	pageNum--;

	// Die Seiten einfach wieder zeichnen
	if ( (pageNum % 2) == 0 )
		m_view.DrawFrontSheet( dc, pageNum );
	else
		m_view.DrawBackSheet( dc, pageNum );

	// Seite aufhören
	dc.EndPage();

	// Seite wurde gedruckt
	return true;
}