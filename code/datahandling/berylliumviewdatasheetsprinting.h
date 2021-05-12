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

#include <wx/print.h>

#include "../datahandling/berylliumdocument.h"
#include "berylliumviewdatasheets.h"

class CBerylliumViewDataSheetsPrinting : public wxPrintout
{
public:
	CBerylliumViewDataSheetsPrinting( CBerylliumViewDataSheets &view, CBerylliumDocument &document );
	~CBerylliumViewDataSheetsPrinting(void);

private:
	// Referenz zur Ansicht
	CBerylliumViewDataSheets &m_view;

	// Referenz zum Dokument
	CBerylliumDocument	&m_document;

public:
	// Mögliche Seiten zurückgeben
	void GetPageInfo(int *minPage, int *maxPage, int *pageFrom, int *pageTo);

	// Dokument-Titel
	wxString GetTitle();

	// Hat das Dokument die Seite pageNum?
	bool HasPage(int pageNum);

	// Event: Drucken hat begonnen (wird für jede Kopie aufgerufen)
	bool OnBeginDocument(int startPage, int endPage);

	// Event: Drucken wurde beendet
	void OnEndDocument();

	// Event: Drucken hat begonnen (wird einmalig aufgerufen)
	void OnBeginPrinting();

	// Event: Drucken wurde beendet (auch einmalig)
	void OnEndPrinting();

	// Event: Druck vorbereiten (Seitenzahlen berechnen oder sowas)
	void OnPreparePrinting();

	// Event: Seite drucken
	bool OnPrintPage(int pageNum);
};
