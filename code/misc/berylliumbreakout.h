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

// Stoppuhr und Sound einbinden
#include <wx/stopwatch.h>
#include <wx/sound.h>

// Breakout-Klasse
class CBerylliumBreakout : public wxWindow
{
public:
	// Konstruktor
	CBerylliumBreakout(wxWindow *parent,int xpos, int ypos, int width, int height);

	// Destructor
	~CBerylliumBreakout(void);

private:
	// Timer
	wxStopWatch m_stopwatch;

private:
	// Enthält die Steine ( 0 = kein Stein, i = Elementnummer )
	static const int m_iInitStones[180];
	int m_iStones[180];

	// Enthält die Elementsymbol
	static const char *m_cElements[113];

	// Ballstatus ( 0 = "Ball klebt am Schläger", 1 = "Ball fliegt" )
	int m_iBallStatus;

	// Ballposition
	int m_iBallX;
	int m_iBallY;

	// Ballrichtungsvektor
	int iBallVelocityX;
	int iBallVelocityY;

	// Paddle x-Position
	int iPaddleX;

	// Highlight
	int iHighlight;

	// Paddle wurde als letztes nach links bewegt?
	bool bPaddleMovedLeft;

	// Punkte
	unsigned int iPoints;

	// Leben
	unsigned int iLives;

	// Steine zerlegt
	unsigned int iCrashedStones;

	// Buttons
	wxBitmap buttons[5];

	// Sounds
	wxSound	sounds[2];

	// Dateien laden
	void LoadFiles();

	// Initialisiert alle Startparameter
	void InitStart();

	// Ballberechnungen (Kollision, etc)
	void UpdateBall();

	// Zeichnenereignis
	void OnPaint( wxPaintEvent &event );

	// Ereignis: Taste wurde gedrückt
	void OnKeyDown( wxKeyEvent& event );

	// Zeitereignis
	void OnTimer(wxTimerEvent& event);

	// Idle?
	void OnIdle(wxIdleEvent& event);

	// Hintergrund löschen
	void OnErase(wxEraseEvent& event);

	// Zeichnet das aktuelle Feld
	void DrawField( wxDC &dc );

	// Paddle zeichnen (x,y geben den Mittelpunkt an!)
	void DrawPaddle( wxDC &dc, int x, int y );

	// Ball zeichnen (x,y geben den Mittelpunkt an!)
	void DrawBall( wxDC &dc, int x, int y );

	// Zeichnet einen Stein ( 20x20 )
	void DrawStone( wxDC &dc, int iNumber, int x, int y, int color );

	// Zeichnet die Punkte und die Anzahl der Bälle
	void DrawHUD( wxDC &dc );

protected:
	DECLARE_EVENT_TABLE()
};
