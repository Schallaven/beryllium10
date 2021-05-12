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

#include "../stdafx.h"
#include "berylliumviewdatasheetsprinting.h"

CBerylliumViewDataSheetsPrinting::CBerylliumViewDataSheetsPrinting( CBerylliumViewDataSheets &view, CBerylliumDocument &document ) : wxPrintout(document.GetFilename()), m_view(view), m_document(document)
{
}

CBerylliumViewDataSheetsPrinting::~CBerylliumViewDataSheetsPrinting(void)
{
}

// Mögliche Seiten zurückgeben
void CBerylliumViewDataSheetsPrinting::GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo)
{
	// Minimale Seite
	*minPage = 1;

	// Maximale Seite (eine Seite abziehen wegen "Neuer Substanz"-Seite
	*maxPage = (m_view.GetNumberOfPages()-1);

	// von/bis kann 1-1 sein
	*pageFrom = 1;
	*pageTo	 = 1;
}

// Dokument-Titel
wxString CBerylliumViewDataSheetsPrinting::GetTitle()
{
	// Dokumenttiel zurückgeben
	return m_document.GetFilename();
}

// Hat das Dokument die Seite pageNum?
bool CBerylliumViewDataSheetsPrinting::HasPage(int pageNum)
{
	return ( (pageNum > 0) && ( pageNum < (int)(m_view.GetNumberOfPages()) ) );
}

// Event: Drucken hat begonnen (wird für jede Kopie aufgerufen)
bool CBerylliumViewDataSheetsPrinting::OnBeginDocument(int startPage, int endPage)
{
	// Basisfunktion aufrufen
	return wxPrintout::OnBeginDocument( startPage, endPage );
}

// Event: Drucken wurde beendet
void CBerylliumViewDataSheetsPrinting::OnEndDocument()
{
	// Basisfunktion aufrufen
	wxPrintout::OnEndDocument();
}

// Event: Drucken hat begonnen (wird einmalig aufgerufen)
void CBerylliumViewDataSheetsPrinting::OnBeginPrinting()
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
void CBerylliumViewDataSheetsPrinting::OnEndPrinting()
{
	// Zum "normalen" Modus wechseln (entfernt auch den Previewmodus)
	m_view.SwitchToPrintMode( false );
}

// Event: Druck vorbereiten (Seitenzahlen berechnen oder sowas)
void CBerylliumViewDataSheetsPrinting::OnPreparePrinting()
{
}

// Event: Seite drucken
bool CBerylliumViewDataSheetsPrinting::OnPrintPage(int pageNum)
{
	// DC holen
	wxDC *ptr = GetDC();

	if ( (ptr == NULL) || (!ptr->IsOk()) )
		return false;

	// Referenz
	wxDC &dc = *ptr;

	// Seite anfangen
	dc.StartPage();

	// Einpassen
	FitThisSizeToPage( m_view.GetPageSize() );

	// Seite zeichnen
	m_view.DrawSubstancePage( dc, (pageNum-1) );

	// Seite aufhören
	dc.EndPage();

	// Seite wurde gedruckt
	return true;
}
