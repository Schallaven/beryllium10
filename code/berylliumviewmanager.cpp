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

#include "berylliumviewmanager.h"

#include "datahandling/berylliumviewsafetysheets.h"
#include "datahandling/berylliumviewdatasheets.h"

// Alle Ereignisse werden hier behandelt
BEGIN_EVENT_TABLE(CBerylliumViewManager, wxScrolledWindow)
	EVT_PAINT(      CBerylliumViewManager::OnPaint)
	EVT_ERASE_BACKGROUND( CBerylliumViewManager::OnEraseBackground )
	EVT_MOTION( CBerylliumViewManager::OnMouseMove )
	EVT_KEY_DOWN( CBerylliumViewManager::OnKeyDown )

	EVT_LEFT_DOWN(CBerylliumViewManager::OnMouseClick)
	EVT_LEFT_UP(CBerylliumViewManager::OnMouseClick)
	EVT_LEFT_DCLICK(CBerylliumViewManager::OnMouseClick)
	EVT_MIDDLE_DOWN(CBerylliumViewManager::OnMouseClick)
	EVT_MIDDLE_UP(CBerylliumViewManager::OnMouseClick)
	EVT_MIDDLE_DCLICK(CBerylliumViewManager::OnMouseClick)
	EVT_RIGHT_DOWN(CBerylliumViewManager::OnMouseClick)
	EVT_RIGHT_UP(CBerylliumViewManager::OnMouseClick)
	EVT_RIGHT_DCLICK(CBerylliumViewManager::OnMouseClick)
END_EVENT_TABLE()

// Konstruktor
CBerylliumViewManager::CBerylliumViewManager( wxFrame &parent, CBerylliumDocument &doc ) : m_ParentWindow( parent ), m_Document( doc )
{
	// Sich selbst erstellen
	Create( &parent, wxID_ANY, wxPoint(0,0), wxSize(-1,-1), wxBORDER_SUNKEN );

	// Gescrollt wird nur noch manuell
	EnableScrolling( false, false );

	// Erste Ansicht
	m_active_view = 0;
}

// Destruktor
CBerylliumViewManager::~CBerylliumViewManager(void)
{
	// Alle Ansichten löschen
	for ( unsigned int i = 0; i < m_views.size(); i++ )
		delete m_views[i];

	// Ausleeren
	m_views.clear();
}

// IObserver: Model wurde verändert... wir müssen jetzt darauf reagieren
void CBerylliumViewManager::ModelHasChanged()
{

}

// Initialisiert den Manager und alle Ansichten
void CBerylliumViewManager::InitManager()
{
	// Alle Ansichten löschen
	for ( unsigned int i = 0; i < m_views.size(); i++ )
		delete m_views[i];

	// Ausleeren
	m_views.clear();

	// Ansicht: Gruppenbetriebsanweisung
	m_views.push_back( new CBerylliumViewSafetySheets(m_Document, *this) );

	// Ansicht: Substanzdatenblatt
	m_views.push_back( new CBerylliumViewDataSheets(m_Document, *this) );

	// Aktive Ansicht setzen
	SetActiveView(0);
}

// Aktuelle Ansicht zurückgeben
IView *CBerylliumViewManager::GetActiveView()
{
	return m_views[m_active_view];
}

// Setzt die aktuelle Ansicht
void CBerylliumViewManager::SetActiveView( unsigned int iViewIndex )
{
	// Außerhalb unseres Vektors?
	if ( iViewIndex > m_views.size() )
		return;

	// Neue Ansicht setzen
	m_active_view = iViewIndex;

	// Seite auf Null setzen
	GetActiveView()->SetPageNumber(0);

	// Größe der aktuellen Seite holen
	wxSize sizeView	= GetActiveView()->GetPageSize();

	// Rand aufaddieren
	sizeView.x += iBorderWidth; sizeView.y += iBorderWidth;

	// Scrollleisten berechnen (PixelsPerUnit = 20)
	SetScrollbars( 20, 20, (sizeView.x / 20), (sizeView.y / 20), 0, 0, true );

	// Event auslösen
	GetActiveView()->SwitchedToView();

	// Neue Ansicht zeichnen lassen...
	Refresh();
}

// Zeichnet das Fenster
void CBerylliumViewManager::OnPaint( wxPaintEvent &event )
{
	// Device Context holen
	wxPaintDC dc(this);

	// Ausmaße des Clientbereiches holen
	wxRect rectClient = GetClientRect();

	// Ausmaße der aktuellen Seite holen
	wxSize sizeView	= GetActiveView()->GetPageSize();

	// Puffer-Context anlegen
	wxBufferedDC dcBuffer(&dc);

	// Erstmal den Hintergrund dunkelgrau zeichnen
	dcBuffer.SetBrush( wxBrush( wxColour( 200, 200, 200), wxSOLID ) );
	dcBuffer.SetPen( wxPen(wxColour( 200, 200, 200), 1, wxSOLID) );
	dcBuffer.DrawRectangle( rectClient );

	// Memory-Objekt ( 4 Pixel für die Ränder des Blattes! )
	wxBitmap   memBitmap( sizeView.x + 4, sizeView.y + 4 );
	wxMemoryDC memDC( memBitmap );

	// Erstmal den Hintergrund dunkelgrau zeichnen...
	memDC.SetBrush( wxBrush( wxColour( 200, 200, 200), wxSOLID ) );
	memDC.SetPen( wxPen(wxColour( 200, 200, 200), 1, wxSOLID) );
	memDC.DrawRectangle( 0, 0, sizeView.x + 4, sizeView.y + 4 );

	// Dann zeichnen wir ein "Blatt", zuerst der Schatten...
	memDC.SetBrush( wxBrush( wxColour( 0, 0, 0), wxSOLID ) );
	memDC.SetPen( wxPen(wxColour( 0, 0, 0), 1, wxSOLID) );
	memDC.DrawRectangle( 2, 2, sizeView.x + 2, sizeView.y + 2 );

	// ...dann das "weiße" Papier
	memDC.SetBrush( wxBrush( wxColour( 255, 255, 255), wxSOLID ) );
	memDC.DrawRectangle( 0, 0, sizeView.x + 1, sizeView.y + 1 );

	// MemoryDC und Bitmap für die Ansicht erstellen
#ifdef WIN32
	wxBitmap viewBitmap( sizeView.x, sizeView.y, dc );
#else
	wxBitmap viewBitmap( sizeView.x, sizeView.y);
#endif
	wxMemoryDC viewDC(viewBitmap);

	// Ansicht zeichnen lassen und dann reinkopieren...
	GetActiveView()->Draw( viewDC );
	memDC.Blit( 2, 2, sizeView.x - 2, sizeView.y - 2, &viewDC, 0, 0 );

	// Zum Schluss zeichnen wir das Blatt in den "richtigen" Device Context...
	wxPoint pointDraw; GetViewStart( &pointDraw.x, &pointDraw.y );
	pointDraw.x *= -20; pointDraw.y *= -20;

	// Sollte das Blatt kleiner als der Clientbereich sein, dann ordnen wir es mittig an.
	if ( sizeView.x < rectClient.width )
		pointDraw.x = (rectClient.width - sizeView.x - 4 - 20) / 2;

	if ( sizeView.y < rectClient.height )
		pointDraw.y = (rectClient.height - sizeView.y - 4 - 20) / 2;

	// Kopiere das ganzen in den einen Puffer Context, der das ganze dann automatisch in den
	// ursprünglichen Device Context kopiert (bei Zerstörung)...
	dcBuffer.Blit( pointDraw.x + 20, pointDraw.y + 20, sizeView.x + 4, sizeView.y + 4, &memDC, 0, 0 );
}

void CBerylliumViewManager::OnMouseClick(wxMouseEvent &event)
{
	// Event kopieren
	wxMouseEvent eventCopy = event;

	// Ausmaße des Clientbereiches holen
	wxRect rectClient = GetClientRect();

	// Ausmaße der aktuellen Seite holen
	wxSize sizeView	= GetActiveView()->GetPageSize();

	// Linke obere Ecke ermitteln von unserem Blatt
	wxPoint pointDraw; GetViewStart( &pointDraw.x, &pointDraw.y );
	pointDraw.x *= -20; pointDraw.y *= -20;

	// Sollte das Blatt kleiner als der Clientbereich sein, dann ordnen wir es mittig an.
	if ( sizeView.x < rectClient.width )
		pointDraw.x = (rectClient.width - sizeView.x - 4 - 20) / 2;

	if ( sizeView.y < rectClient.height )
		pointDraw.y = (rectClient.height - sizeView.y - 4 - 20) / 2;

	eventCopy.m_x -= pointDraw.x + 20;
	eventCopy.m_y -= pointDraw.y + 20;

	// Seitengröße
	wxRect rectPage( 0, 0, sizeView.GetWidth(), sizeView.GetHeight() );

	// Aktuelle Ansicht holen, aber NUR wenn die Mausposition innerhalb des Blattes liegt
	if ( rectPage.Contains(eventCopy.m_x, eventCopy.m_y ) )
		GetActiveView()->OnMouseClick( eventCopy );

	// Event weiterleiten
	event.Skip();
}

void CBerylliumViewManager::OnMouseMove(wxMouseEvent &event)
{
	// Event kopieren
	wxMouseEvent eventCopy = event;

	// Ausmaße des Clientbereiches holen
	wxRect rectClient = GetClientRect();

	// Ausmaße der aktuellen Seite holen
	wxSize sizeView	= GetActiveView()->GetPageSize();

	// Linke obere Ecke ermitteln von unserem Blatt
	wxPoint pointDraw; GetViewStart( &pointDraw.x, &pointDraw.y );
	pointDraw.x *= -20; pointDraw.y *= -20;

	// Sollte das Blatt kleiner als der Clientbereich sein, dann ordnen wir es mittig an.
	if ( sizeView.x < rectClient.width )
		pointDraw.x = (rectClient.width - sizeView.x - 4 - 20) / 2;

	if ( sizeView.y < rectClient.height )
		pointDraw.y = (rectClient.height - sizeView.y - 4 - 20) / 2;

	eventCopy.m_x -= pointDraw.x + 20;
	eventCopy.m_y -= pointDraw.y + 20;

	// Aktuelle Ansicht holen
	GetActiveView()->OnMouseMove( eventCopy );

	// Event weiterleiten
	event.Skip();
}

// Ereignis: Taste wurde gedrückt
void CBerylliumViewManager::OnKeyDown( wxKeyEvent& event )
{
	// Tastencode holen
	int iCode = event.GetKeyCode();

	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() != NULL )
	{
		// Pfeiltaste links
		if ( iCode == WXK_LEFT )
			GetActiveView()->PrevPage();

		// Pfeiltaste rechts
		if ( iCode == WXK_RIGHT )
			GetActiveView()->NextPage();
	}

	event.Skip();
}

// Der aktuellen Ansicht mitteilen, dass sich das System geändert hat
void CBerylliumViewManager::SetGHSMode( bool toGHS )
{
	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() != NULL )
		GetActiveView()->SetGHSMode( toGHS );
}

// Wird aufgerufen, wenn die CommandID größer beID_POPUPMENU (20000) ist
void CBerylliumViewManager::OnPopupMenuClick( wxCommandEvent &event )
{
	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() != NULL )
		GetActiveView()->DoPopupMenu( event );
}

// Event: Datei wurde geladen
void CBerylliumViewManager::OnFileWasLoaded()
{
	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() != NULL )
		GetActiveView()->OnFileWasLoaded();
}

// Event: Datei wurde gespeichert
void CBerylliumViewManager::OnFileWillBeSaved()
{
	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() != NULL )
		GetActiveView()->OnFileWillBeSaved();
}

// Gibt ein wxPrintOut-Objekt zurück
wxPrintout *CBerylliumViewManager::GetPrintOut()
{
	// Gibt es eine aktuelle Ansicht?
	if ( GetActiveView() == NULL )
		return NULL;

	// Von View holen
	return GetActiveView()->GetPrintOut();
}
