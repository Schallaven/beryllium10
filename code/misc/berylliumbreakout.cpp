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
#include "berylliumbreakout.h"

#include <wx/timer.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>


// Initialisieren der Startstruktur
const int CBerylliumBreakout::m_iInitStones[180] =
	{
		  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,
		  3,  4,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  6,  7,  8,  9, 10,
		 11, 12,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 13, 14, 15, 16, 17, 18,
		 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
		 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54,
		 55, 56, 57, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86,
	     87, 88, 89,104,105,106,107,108,109,110,111,  0,  0,  0,  0,  0,  0,  0,
		  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // ----- Leer
		  0,  0, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,  0,  0,
		  0,  0, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,  0,  0
	};

// Initialisieren der Elementsymbole
const char *CBerylliumBreakout::m_cElements[113] =
	{
		 "",
		 "H", "He", "Li", "Be",  "B",  "C",  "N",  "O",  "F", "Ne", "Na", "Mg", "Al", "Si", // 1 - 14
		 "P",  "S", "Cl", "Ar",  "K", "Ca", "Sc", "Ti",  "V", "Cr", "Mn", "Fe", "Co", "Ni", // 15 - 28
		"Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr",  "Y", "Zr", "Nb", "Mo",
		"Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te",  "I", "Xe", "Cs", "Ba",
		"La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
		"Lu", "Hf", "Ta",  "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Ti", "Pb", "Bi", "Po",
		"At", "Rn", "Fr", "Ra", "Ac", "Th", "Pa",  "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf",
		"Es", "Fm", "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn"
	};


// Alle Ereignisse werden hier behandelt
BEGIN_EVENT_TABLE(CBerylliumBreakout, wxWindow)
	EVT_PAINT(CBerylliumBreakout::OnPaint)
	EVT_TIMER(12345, CBerylliumBreakout::OnTimer)
	EVT_KEY_DOWN(CBerylliumBreakout::OnKeyDown)
	EVT_ERASE_BACKGROUND(CBerylliumBreakout::OnErase)
	EVT_IDLE(CBerylliumBreakout::OnIdle)
END_EVENT_TABLE()

// Konstruktor
CBerylliumBreakout::CBerylliumBreakout( wxWindow *parent, int xpos, int ypos, int width, int height)
{
	// Sich selbst erstellen
	Create( parent, wxID_ANY, wxPoint(xpos,ypos), wxSize(width,height), wxBORDER_SUNKEN );

	// Hintergrund
	SetBackgroundStyle( wxBG_STYLE_CUSTOM );

	// Dateien laden
	LoadFiles();

	// Startparameter setzen
	InitStart();

	// Timer starten
	m_stopwatch.Start();
}

// Destruktor
CBerylliumBreakout::~CBerylliumBreakout(void)
{
}

// Dateien laden
void CBerylliumBreakout::LoadFiles()
{
	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Glasbuttons
	// -----------
	wxString files[5] = {	"buttonblue.png",
							"buttonred.png",
							"buttonyellow.png",
							"buttonviolett.png",
							"buttongreen.png" };

	for ( unsigned int i = 0; i < 5; ++i )
	{
		// Datei-Stream laden
		wxFSFile *file = filesystem->OpenFile( "beryllium10.dat#zip:" + files[i] );

		// Datei konnte nicht geladen werden
		if ( file == NULL )
			continue;

		// ein Image-Objekt anlegen
		wxImage img( *file->GetStream(), wxBITMAP_TYPE_PNG  );

		// Bild nicht ok?
		if ( !img.IsOk() )
			return;

		// daraus ein Bitmap machen
		wxBitmap bmp( img );

		// Kopieren
		buttons[i] = bmp;

		// Datei löschen
		delete file;
	}

	// Glasbuttons
	// -----------
	wxString snds[2] = {	"snd1.wav",
							"snd2.wav" };

	for ( unsigned int i = 0; i < 2; ++i )
	{
		// Datei-Stream laden
		wxFSFile *file = filesystem->OpenFile( "beryllium10.dat#zip:" + snds[i] );

		// Datei konnte nicht geladen werden
		if ( file == NULL )
			continue;

		// Größe der Datei bestimmen
		wxInputStream *stream = file->GetStream();

		// Länge des Streams
		int iLen = stream->GetSize();

		if ( iLen == 0 )
		{
			delete file;
			continue;
		}

		// Buffer anlegen
		wxByte *data = new wxByte[iLen];

		// Daten kopieren
		stream->Read( data, iLen );

		// Ein Soundobjekt anlegen
		sounds[i].Create( iLen, data );

		// Buffer löschen
		delete[] data;

		// Datei löschen
		delete file;
	}

	// Zeiger zum virtuellen Dateisystem löschen
	delete filesystem;
}

// Initialisiert alle Startparameter
void CBerylliumBreakout::InitStart()
{
	// Startfeld kopieren
	memcpy( &m_iStones, &m_iInitStones, 180 * sizeof(int) );

	// Startposition
	iPaddleX = 330;
	bPaddleMovedLeft = true;

	// Ball klebt am Schläger
	m_iBallStatus = 0;

	// Ballposition (irrelevant)
	m_iBallX = 0; m_iBallY = 0;

	// Ballvektor (irrelevant)
	iBallVelocityX = 0; iBallVelocityY = 0;

	// Keine Punkte
	iPoints = 0;

	// 3 Leben
	iLives = 3;

	// Keine Steine zerlegt
	iCrashedStones = 0;
}

void CBerylliumBreakout::OnErase(wxEraseEvent& event)
{
}

// Idle?
void CBerylliumBreakout::OnIdle(wxIdleEvent& event)
{
	// Kleine Verzögerung
	while ( m_stopwatch.Time() < 10 );

	// Alle Steine zerlegt?
	if ( iCrashedStones >= 111 )
	{
		// Hinweis anzeigen. Text wird nicht übersetzt, weil es sonst zu leicht wäre nach dem Easter Egg zu suchen :)
		wxMessageBox( "I hope you enjoyed this little easter egg! Now back to work!", _(L"Beryllium¹º"), wxICON_INFORMATION | wxOK );

		// Neues Spiel
		InitStart();
	}

	// Ball berechnen
	UpdateBall();

	// Refresh
	Refresh(false);

	// Start Timer
	m_stopwatch.Start(0);
}

// Zeichnen
void CBerylliumBreakout::OnPaint( wxPaintEvent &event )
{
	// Device Context holen
	wxPaintDC dc(this);

	// Ausmaße des Clientbereiches holen
	wxRect rectClient = GetClientRect();

	// MemoryDC und Bitmap erstellen
#ifdef WIN32
	wxBitmap memBitmap( rectClient.width, rectClient.height, dc );
#else
    wxBitmap memBitmap( rectClient.width, rectClient.height);
#endif
	wxMemoryDC memDC(memBitmap);

	// Erstmal den Hintergrund schwarz zeichnen
	memDC.SetBrush( wxBrush( wxColour( 0, 0, 0), wxSOLID ) );
	memDC.SetPen( wxPen(wxColour( 0, 0, 0), 1, wxSOLID) );
	memDC.DrawRectangle( rectClient );

	// HUD zeichnen
	DrawHUD( memDC );

	// Feld zeichnen
	DrawField( memDC );

	// Paddle zeichnen
	DrawPaddle( memDC, iPaddleX, 500 );

	// Ball zeichnen (falls "frei")
	if ( m_iBallStatus != 0 )
		DrawBall( memDC, m_iBallX, m_iBallY );

	// Kopiere das Ganze in den einen Puffer Context, der das ganze dann automatisch in den
	// ursprünglichen Device Context kopiert (bei Zerstörung)...
	dc.Blit( 0, 0, rectClient.width, rectClient.height, &memDC, 0, 0 );
}

// Zeitereignis
void CBerylliumBreakout::OnTimer(wxTimerEvent& event)
{
	// Ball berechnen
	UpdateBall();

	// Neuzeichnen
	Refresh( false );
}

// Ereignis: Taste wurde gedrückt
void CBerylliumBreakout::OnKeyDown( wxKeyEvent& event )
{
	// Tastencode holen
	int iCode = event.GetKeyCode();

	// Welche Taste gedrückt?
	switch (iCode)
	{
		// Schläger nach links bewegen
		case WXK_LEFT:
		case 'A':
			iPaddleX -= 15;
			if ( iPaddleX < 45 )
				iPaddleX = 45;
			bPaddleMovedLeft = true;
			break;

		// Schläger nach links bewegen
		case WXK_RIGHT:
		case 'D':
			iPaddleX += 15;
			if ( iPaddleX > 614 )
				iPaddleX = 614;
			bPaddleMovedLeft = false;
			break;

		// R = Restart
		case 'R':
			InitStart();
			break;

		// Leertaste
		case WXK_SPACE:
			// Klebt der Ball?
			if ( m_iBallStatus == 0 )
			{
				// Position festlegen
				m_iBallX = iPaddleX;
				m_iBallY = 500 - 10;

				// Richtung festlegen
				iBallVelocityY -= 2;
				if ( bPaddleMovedLeft )
					iBallVelocityX -= 2;
				else
					iBallVelocityX += 2;

				// Der Ball ist nun "frei"
				m_iBallStatus = 1;
			}
			break;
	}
}

// Zeichnet das aktuelle Feld
void CBerylliumBreakout::DrawField( wxDC &dc )
{
	/*
	wxString files[5] = {	"buttonblue.png",
							"buttonred.png",
							"buttonyellow.png",
							"buttonviolett.png",
							"buttongreen.png" };
	*/
	// Farbe
	int color = 0;

	// 10 Zeilen
	for ( unsigned int iLine = 0; iLine < 10; ++iLine )
	{
		// 18 Spalten
		for ( unsigned int iCol = 0; iCol < 18; ++iCol )
		{
			// Farbe nach Spalte aussuchen
			switch( iCol )
			{
			case 0:
			case 1:
				color = 1; // Rot
				break;

			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
				// Blau für Metalle, Violett für die Nichtmetalle
				if ( ((iCol-12) >= (iLine-1)) || (iCol == 17) )
					color = 3; // Violett
				else
					color = 0; // Blau
				break;

			default:
				color = 2; // Gelb
				break;
			}

			// Zeile
			if ( iLine > 7 )
				color = 4;

			// Beryllium markieren
			if ( m_iStones[ iLine*18 + iCol ] == 4 )
				color = 4;

			DrawStone( dc, m_iStones[ iLine*18 + iCol ], 60 + iCol*30, 30 + iLine*30, color );
		}
	}
}

// Zeichnet einen Stein ( 20x20 )
void CBerylliumBreakout::DrawStone( wxDC &dc, int iNumber, int x, int y, int color )
{
	// iNumber = 0? Raus!
	if ( (iNumber < 1) || (iNumber > 111) )
		return;

	// Farbe überprüfen
	if ( (color < 0) || (color > 4) )
		return;

	// Rechteckt
	wxRect rect( x, y, 30, 30 );

	// Und zeichnen
	dc.DrawBitmap( buttons[color], x, y, true );

	// Farbe
	dc.SetTextForeground( wxColour( 0,0,0 ) );

	// Text zeichnen
	dc.DrawLabel( m_cElements[iNumber], rect, wxALIGN_CENTER );
}

// Paddle zeichnen (x,y geben den Mittelpunkt an!)
void CBerylliumBreakout::DrawPaddle( wxDC &dc, int x, int y )
{
	// Pinsel und Stift erstellen
	wxBrush brush( wxColour( 220, 220, 220), wxSOLID );
	wxPen pen( wxColour( 255, 255, 255), 1, wxSOLID );

	// Festlegen
	dc.SetBrush( brush );
	dc.SetPen( pen );

	// Rechteck für das Paddle festlegen
	wxRect rect( x - 45, y - 5, 90, 10 );

	// Rechteck zeichnen
	dc.DrawRectangle( rect );

	// Ball zeichnen falls "festgeklebt"
	if ( m_iBallStatus == 0 )
		DrawBall( dc, x, y - 10 );
}

// Ball zeichnen (x,y geben den Mittelpunkt an!)
void CBerylliumBreakout::DrawBall( wxDC &dc, int x, int y )
{
	// Pinsel und Stift erstellen
	wxBrush brush( wxColour( 220, 220, 220), wxSOLID );
	wxPen pen( wxColour( 255, 255, 255), 1, wxSOLID );

	// Festlegen
	dc.SetBrush( brush );
	dc.SetPen( pen );

	// Zeichnen
	dc.DrawCircle( x, y, 5 );
}

// Zeichnet die Punkte und die Anzahl der Bälle
void CBerylliumBreakout::DrawHUD( wxDC &dc )
{
	// Schrift vorbereiten
	wxFont font( 30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, "Courier New" );

	// Alte Schrift speichern
	wxFont oldFont = dc.GetFont();

	// Schrift wählen
	dc.SetFont(font);

	// Farbe
	dc.SetTextForeground( wxColour( 255, 255, 255) );

	// Punkte zeichnen
	dc.DrawLabel( wxString::Format("%d", iPoints), wxRect( 500, 515, 150, 50 ), wxALIGN_RIGHT|wxALIGN_TOP);

	// Leben zeichnen
	dc.DrawLabel( wxString::Format("Balls: %d", iLives), wxRect( 0, 515, 150, 50), wxALIGN_LEFT|wxALIGN_TOP);

	// Alte Schrift wiederherstellen
	dc.SetFont(oldFont);
}

// Ballberechnungen (Kollision, etc)
void CBerylliumBreakout::UpdateBall()
{
	// Nur falls nicht festgeklebt
	if ( m_iBallStatus == 0 )
		return;

	// Dynamische Reflexion: Feld
	// --------------------------

	// Befinden wir uns im "Feld"?
	wxRect rectField( 60, 30, 539, 300 );

	if (  rectField.Contains( m_iBallX, m_iBallY ) )
	{
		// Neue Position berechnen
		int iNewPosX = m_iBallX + iBallVelocityX + 5*(iBallVelocityX/abs(iBallVelocityX));
		int iNewPosY = m_iBallY + iBallVelocityY + 5*(iBallVelocityY/abs(iBallVelocityY));

		// Steigung und Achsenabschnitt berechnen
		int m = (iNewPosY-m_iBallY) / (iNewPosX-m_iBallX);
		int t = m_iBallY - m * m_iBallX;

		// Schritte zum berechnen
		int iStepsX = abs(iNewPosX - m_iBallX);

		for ( int step = 0; step < iStepsX; step++ )
		{
			// Berechnete Positionen
			iNewPosX = m_iBallX + step;
			iNewPosY = m * iNewPosX + t;

			// Feldposition
			int iFieldX = (int)(iNewPosX - 60) / (int)30;
			int iFieldY = (int)(iNewPosY - 30) / (int)30;

			// Korrektur
			if ( iFieldX > 17 )
				continue;

			if ( iFieldY > 9 )
				continue;

			// Index
			int iIndex = iFieldY * 18 + iFieldX;

			// Index zu hoch?
			if ( iIndex > 179 )
				continue;

			// Ist an diesem Index nichts? Dann einfach passieren
			if ( m_iStones[iIndex] == 0 )
				continue;

			// Sound abspielen
			sounds[1].Play();

			// Punkte addieren (Sonderpunkte für Beryllium)
			if ( m_iStones[iIndex] == 4 )
				iPoints += 7185;
			else
				iPoints += (120 - m_iStones[iIndex]) * 10;

			// Steine zerlegt
			iCrashedStones++;

			// Stein löschen
			m_iStones[iIndex] = 0;

			// Kanten
			wxRect borderTop( 60 + iFieldX * 30, 30 + iFieldY * 30, 30, 5 );
			wxRect borderBottom( 60 + iFieldX * 30, 30 + iFieldY * 30 + 25, 30, 5 );
			wxRect borderLeft( 60 + iFieldX * 30, 30 + iFieldY * 30 + 5, 5, 20 );
			wxRect borderRight( 60 + iFieldX * 30 + 25, 30 + iFieldY * 30 + 5, 5, 20 );

			// Ecken

			// Unten getroffen
			if ( borderBottom.Contains( iNewPosX, iNewPosY ) )
			{
				// Vektor umkehren
				iBallVelocityY = -1 * iBallVelocityY;

				// Position setzen
				m_iBallY = 30 + iFieldY * 30 + 30 + 5;
				//iBallX += iBallVelocityX;
				m_iBallX = iNewPosX;

				return;
			}

			// Links getroffen
			else if ( borderLeft.Contains( iNewPosX, iNewPosY ) )
			{
				// Vektor umkehren
				iBallVelocityX = -1 * iBallVelocityX;

				// Position setzen
				m_iBallX = 60 + iFieldX * 30 - 5;
				//iBallY += iBallVelocityY;
				m_iBallY = iNewPosY;

				return;
			}

			// Rechts getroffen
			else if ( borderRight.Contains( iNewPosX, iNewPosY ) )
			{
				// Vektor umkehren
				iBallVelocityX = -1 * iBallVelocityX;

				// Position setzen
				m_iBallX = 60 + iFieldX * 30 + 30 + 5;
				//iBallY += iBallVelocityY;
				m_iBallY = iNewPosY;

				return;
			}

			// Oben getroffen
			else if ( borderTop.Contains( iNewPosX, iNewPosY ) )
			{
				// Vektor umkehren
				iBallVelocityY = -1 * iBallVelocityY;

				// Position setzen
				m_iBallY = 30 + iFieldY * 30 - 5;
				//iBallX += iBallVelocityX;
				m_iBallX = iNewPosX;

				return;
			}

			// Eine der Ecken getroffen, dann einfach den Vektor komplett umdrehen
			else
			{
				// Vektor umkehren
				iBallVelocityX = -1 * iBallVelocityX;
				iBallVelocityY = -1 * iBallVelocityY;

				// Position um eins zurücksetzen
				m_iBallX = m_iBallX + step - 1;
				m_iBallY = m * m_iBallX + t;

				// Zurück
				return;
			}

		}

		// Keinen Konflikt gefunden? Ball versetzen
		m_iBallX += iBallVelocityX;
		m_iBallY += iBallVelocityY;

		return;
	}

	// Statische Reflexion (Seitenränder)
	// ----------------------------------

	// Neue Position
	m_iBallX += iBallVelocityX;
	m_iBallY += iBallVelocityY;

	// Linke Seite: Reflektion
	if ( m_iBallX < 5 )
	{
		// Position anpassen
		m_iBallX = 5;

		// Neuer Richtungsvektor
		iBallVelocityX = -1 * iBallVelocityX;

		// Sound abspielen
		sounds[0].Play();
	}

	// Rechter Rand: Reflektion
	else if ( m_iBallX > 650 )
	{
		// Position anpassen
		m_iBallX = 650;

		// Neuer Richtungsvektor
		iBallVelocityX = -1 * iBallVelocityX;

		// Sound abspielen
		sounds[0].Play();
	}

	// Oberer Rand: Reflektion
	if ( m_iBallY < 5 )
	{
		// Position anpassen
		m_iBallY = 5;

		// Neuer Richtungsvektor
		iBallVelocityY = -1 * iBallVelocityY;

		// Sound abspielen
		sounds[0].Play();
	}

	// Unterer Rand: Leben verlieren
	else if ( m_iBallY > 580 )
	{
		// Neuen Ball festkleben
		m_iBallStatus = 0;

		m_iBallX = 0;
		m_iBallY = 0;

		iBallVelocityX = 0;
		iBallVelocityY = 0;

		// Ein Leben weniger
		iLives--;

		// Wenn = 0, dann Neustart
		if ( iLives == 0 )
			InitStart();

		// Zurück
		return;
	}

	// Kontakt mit Schläger
	wxRect rectPaddle( iPaddleX - 45, 500 - 5, 90, 10 );

	// Reflektion am Schläger
	if ( rectPaddle.Contains( m_iBallX, m_iBallY ) )
	{
		// Neue Position
		m_iBallY = 500 - 10;

		// Reflektion
		iBallVelocityY = -1 * iBallVelocityY;

		// Absoluter Unterschied zur Mitte des Paddles
		int dif = abs(m_iBallX - iPaddleX);

		// Je weiter außen, desto "steiler wird der Ball"
		iBallVelocityX = (iBallVelocityX/abs(iBallVelocityX)) * ((int)(dif / 15) + 1);

		// Sound abspielen
		sounds[0].Play();
	}
}
