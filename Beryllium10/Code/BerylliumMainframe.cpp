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

#include <wx/print.h>
#include <wx/textdlg.h>
#include <wx/protocol/http.h>

#include <wx/sstream.h>

// Versionsinfos
#include "AutoBuild.h"

#include "BerylliumViewManager.h"

#include "BerylliumMainframe.h"
#include "berylliummainframe_identifiers.h"

#include "BerylliumApplication.h"

#include "Dialoge/BerylliumMultiplierDialog.h"
#include "Dialoge/BerylliumOptionsDialog.h"
#include "Dialoge/BerylliumInfoDialog.h"

// Easter Egg: Breakout Clone :)
#include "Misc/BerylliumBreakoutDialog.h"

// Bitmaps für Symbolleiste einfügen
#include "../Resources/Bitmaps/icon_empty_menu.xpm"
#include "../Resources/Bitmaps/icon_new.xpm"
#include "../Resources/Bitmaps/icon_new_menu.xpm"
#include "../Resources/Bitmaps/icon_open.xpm"
#include "../Resources/Bitmaps/icon_open_menu.xpm"
#include "../Resources/Bitmaps/icon_save.xpm"
#include "../Resources/Bitmaps/icon_save_menu.xpm"
#include "../Resources/Bitmaps/icon_print.xpm"
#include "../Resources/Bitmaps/icon_print_menu.xpm"
#include "../Resources/Bitmaps/icon_help.xpm"
#include "../Resources/Bitmaps/icon_help_menu.xpm"
#include "../Resources/Bitmaps/icon_left.xpm"
#include "../Resources/Bitmaps/icon_right.xpm"
#include "../Resources/Bitmaps/icon_useghs.xpm"
#include "../Resources/Bitmaps/icon_psebreaker.xpm"

// Alle Ereignisse werden hier behandelt
BEGIN_EVENT_TABLE(CBerylliumMainframe, wxFrame)
	EVT_MENU(beID_PAGE_LEFT,  CBerylliumMainframe::OnEventPageLeft)
	EVT_MENU(beID_PAGE_RIGHT, CBerylliumMainframe::OnEventPageRight)

	EVT_MENU(beID_NEW, CBerylliumMainframe::OnEventNewFile)
	EVT_MENU(beID_OPEN, CBerylliumMainframe::OnEventOpenFile)
	EVT_MENU(beID_SAVE, CBerylliumMainframe::OnEventSaveFile)
	EVT_MENU(beID_SAVEAS, CBerylliumMainframe::OnEventSaveFileAs)
	EVT_MENU(beID_PRINT, CBerylliumMainframe::OnEventPrintFile)
	EVT_MENU(beID_PRINT_PREVIEW, CBerylliumMainframe::OnEventPrintPreview)
	EVT_MENU(beID_EXIT, CBerylliumMainframe::OnEventExit)

	EVT_MENU(beID_VIEW_TOOLBAR, CBerylliumMainframe::OnEventShowToolBar)
	EVT_MENU(beID_VIEW_STATUSBAR, CBerylliumMainframe::OnEventShowStatusBar)
	EVT_MENU_RANGE(beID_VIEW_FIRST, beID_VIEW_LAST, CBerylliumMainframe::OnEventChangeView)

	EVT_MENU(beID_TOOL_MULTIPLIER, CBerylliumMainframe::OnEventSubstanceMultiplier)
	EVT_MENU(beID_TOOL_OPTIONS, CBerylliumMainframe::OnEventOptions)

	EVT_MENU_RANGE(beID_TOOL_SORTBY_FIRST, beID_TOOL_SORTBY_LAST, CBerylliumMainframe::OnEventSort)

	EVT_MENU(beID_SWITCH_GHS, CBerylliumMainframe::OnEventSwitchGHS)

	EVT_MENU(beID_HELP_TOPICS, CBerylliumMainframe::OnEventHelpTopics)
	EVT_MENU(beID_HELP_INFO, CBerylliumMainframe::OnEventInfoBox)
	EVT_MENU(beID_HELP_BREAKOUT, CBerylliumMainframe::OnEventBreakout)
	EVT_MENU(beID_HELP_UPDATES, CBerylliumMainframe::OnEventUpdateCheck)
	EVT_MENU(beID_HELP_BUGREPORT, CBerylliumMainframe::OnEventBugReport)

	EVT_MENU_RANGE(beID_POPUPMENU, beID_POPUPMENU_LAST, CBerylliumMainframe::OnEventPopupMenuClick)

	EVT_SIZE( CBerylliumMainframe::OnSize )

	EVT_CLOSE( CBerylliumMainframe::OnClose )
END_EVENT_TABLE()

// Konstruktor...
CBerylliumMainframe::CBerylliumMainframe(const wxChar *title, int xpos, int ypos, int width, int height, bool bEasterEggs = false)
: wxFrame((wxFrame *) NULL, -1, title, wxPoint(xpos, ypos), wxSize(width, height), wxDEFAULT_FRAME_STYLE), m_ViewManager( *this, m_Document ), bEasterEggsEnabled( bEasterEggs )
{
	// Symbolleiste erstellen
	BeCreateToolBar();

	// Menu erstellen
	BeCreateMenuBar();

	// Statusbar erstellen
	BeCreateStatusBar();

	// Beim Model registrieren
	m_Document.AddObserver( this );

	// ViewManager initialisieren
	m_ViewManager.InitManager();

	// Auch den ViewManager registrieren
	m_Document.AddObserver( &m_ViewManager );

	// Der Ansicht mitteilen, dass GHS angesagt ist (Änderung: 6.1.2014)
	m_ViewManager.SetGHSMode( true );

	// Auch dem Dokument mitteilen (Änderung: 6.1.2014)
	m_Document.SwitchToGHS( true );

	// Das Model updaten lassen
	m_Document.NotifyAllObserver();

}

CBerylliumMainframe::~CBerylliumMainframe(void)
{
}

// Hilfsfunktionen (Konstruktor): Erstellt das Menu
void CBerylliumMainframe::BeCreateMenuBar(void)
{
	// Menubar erstellen
	m_MenuBar = new wxMenuBar(wxMB_DOCKABLE);

	// Menupunkte
	wxMenu *menuFile = NULL, *menuView = NULL, *menuTools = NULL, *menuHelp = NULL;

	// Untermenüs
	wxMenu *subMenuLanguages = NULL, *subMenuSorting = NULL;

	// Menuitem
	wxMenuItem *item = NULL;

	// Bitmaps erstellen
	wxBitmap bmpNew(icon_new_menu_xpm);
	wxBitmap bmpOpen(icon_open_menu_xpm);
	wxBitmap bmpSave(icon_save_menu_xpm);
	wxBitmap bmpPrint(icon_print_menu_xpm);
	wxBitmap bmpHelp(icon_help_menu_xpm);
	wxBitmap bmpEmpty(icon_empty_menu_xpm);

	// Datei-Menu
	menuFile = new wxMenu();
	item = new wxMenuItem( 0, beID_NEW, _(L"&Neu\tStrg+N"), _(L"Legt eine neue Betriebsanweisung an") ); item->SetBitmap( bmpNew );
	menuFile->Append( item );
	item = new wxMenuItem( 0, beID_OPEN, _(L"Ö&ffnen\tStrg+O"), _(L"Öffnet eine existierende Betriebsanweisung") ); item->SetBitmap( bmpOpen );
	menuFile->Append( item );
	item = new wxMenuItem( 0, beID_SAVE, _(L"&Speichern\tStrg+S"), _(L"Speichert die aktuelle Betriebsanweisung") ); item->SetBitmap( bmpSave );
	menuFile->Append( item );
	item = new wxMenuItem( 0, beID_SAVEAS, _(L"Speichern &unter..."), _(L"Speichert die aktuelle Betriebsanweisung unter neuem Namen") ); item->SetBitmap( bmpEmpty );
	menuFile->Append( item );
	menuFile->AppendSeparator();
	item = new wxMenuItem( 0, beID_PRINT, _(L"&Drucken\tStrg+P"), _(L"Druckt die aktuelle Betriebsanweisung") ); item->SetBitmap( bmpPrint );
	menuFile->Append( item );
	item = new wxMenuItem( 0, beID_PRINT_PREVIEW, _(L"Druck&vorschau"), _(L"Zeigte die Druckvorschau an.") ); item->SetBitmap( bmpEmpty );
	menuFile->Append( item );
	menuFile->AppendSeparator();
	item = new wxMenuItem( 0, beID_EXIT, _(L"&Beenden"), _(L"Beendet Beryllium¹º") ); item->SetBitmap( bmpEmpty );
	menuFile->Append( item );

	// Ansicht-Menu
	menuView = new wxMenu();
	item = new wxMenuItem( 0, beID_VIEW_TOOLBAR, _(L"&Symbolleiste"), _(L"Zeigt oder versteckt die Symbolleiste"), wxITEM_CHECK ); //item->SetBitmap( bmpEmpty );
	menuView->Append( item );
	item = new wxMenuItem( 0, beID_VIEW_STATUSBAR, _(L"S&tatusleiste"), _(L"Zeigt oder versteckt die Statusleiste"), wxITEM_CHECK ); //item->SetBitmap( bmpEmpty );
	menuView->Append( item );
	menuView->AppendSeparator();
	item = new wxMenuItem( 0, beID_VIEW_SAFETYSHEET, _(L"&Gruppenbetriebsanweisung"), _(L"Zeigt die Gruppenbetriebsanweisungsansicht mit mehreren Substanzen pro Seite an"), wxITEM_RADIO ); //item->SetBitmap( bmpEmpty );
	menuView->Append( item );
	item = new wxMenuItem( 0, beID_VIEW_DATASHEET, _(L"&Einzelbetriebsanweisung"), _(L"Zeigt ausführliche Betriebsanweisungen einzelner Substanzen an"), wxITEM_RADIO ); //item->SetBitmap( bmpEmpty );
	menuView->Append( item );

	// Werkzeug-Menu
	menuTools = new wxMenu(); subMenuSorting = new wxMenu();

	item = new wxMenuItem( 0, beID_TOOL_MULTIPLIER, _(L"Substanzmengen-&Multiplikator"), _(L"Multipliziert die gegebenen Substanzmengen mit einem Faktor") ); //item->SetBitmap( bmpEmpty );
	menuTools->Append( item );

	item = new wxMenuItem( 0, beID_TOOL_SORTBY_NAME, _(L"&Name"), _(L"Sortiert die Substanzen nach Namen (von A nach Z)") );
	subMenuSorting->Append( item );
	item = new wxMenuItem( 0, beID_TOOL_SORTBY_AMOUNT, _(L"Stoff&menge"), _(L"Sortiert die Substanzen nach Stoffmenge (größte Stoffmenge zuerst)") );
	subMenuSorting->Append( item );
	item = new wxMenuItem( 0, beID_TOOL_SORTBY_DANGER, _(L"&Be¹º-Punkte"), _(L"Sortiert die Substanzen nach Be¹º-Punkten (=Einschätzung der Gefährlichkeit für den Anwender, höchste Punktzahl zuerst)") );
	subMenuSorting->Append( item );
	menuTools->AppendSubMenu( subMenuSorting, _(L"&Substanzen sortieren nach") );

	menuTools->AppendSeparator();
	item = new wxMenuItem( 0, beID_TOOL_OPTIONS, _(L"&Einstellungen"), _(L"Ruft die Einstellungen für Beryllium auf") ); //item->SetBitmap( bmpEmpty );
	menuTools->Append( item );

	// Hilfe (?)-Menu
	menuHelp = new wxMenu();
	item = new wxMenuItem( 0, beID_HELP_TOPICS, _(L"&Homepage aufrufen"), _(L"Zeigt die Homepage (http://beryllium.keksecks.de/) im Standardbrowser an.") );
	menuHelp->Append( item );
	item = new wxMenuItem( 0, beID_HELP_UPDATES, _(L"Nach &Updates suchen..."), _(L"Sucht nach einer aktuelleren Version von Beryllium¹º.") );
	menuHelp->Append( item );
	item = new wxMenuItem( 0, beID_HELP_BUGREPORT, _(L"&Feedback senden..."), _(L"Helfen Sie mit Beryllium noch besser zu machen! Senden Sie Fehlerberichte oder Verbesserungsvorschläge direkt über diese Option ein!") );
	menuHelp->Append( item );

	// Easter-Eggs: PSE-Breaker
	if ( bEasterEggsEnabled )
	{
		wxBitmap bmpPSEBreaker(icon_psebreaker_xpm);

		item = new wxMenuItem( 0, beID_HELP_BREAKOUT, _(L"PSE &Breakout..."), _T(";)") ); item->SetBitmap( bmpPSEBreaker );
		menuHelp->Append( item );
	}

	menuHelp->AppendSeparator();
	item = new wxMenuItem( 0, beID_HELP_INFO, _(L"&Info..."), _(L"Zeigt Programminformationen über Beryllium an") ); item->SetBitmap( bmpHelp );
	menuHelp->Append( item );

	// Menus hinzufügen
	m_MenuBar->Append( menuFile, _(L"&Datei") );
	m_MenuBar->Append( menuView, _(L"&Ansicht") );
	m_MenuBar->Append( menuTools, _(L"&Werkzeuge") );
	m_MenuBar->Append( menuHelp, _(L"&?") );

	// Menu setzen
	SetMenuBar( m_MenuBar );

	// Häkchen setzen
	GetMenuBar()->Check( beID_VIEW_TOOLBAR, true );
	GetMenuBar()->Check( beID_VIEW_STATUSBAR, true );
	GetMenuBar()->Check( beID_VIEW_SAFETYSHEET, true );
}

// Hilfsfunktionen (Konstruktor): Erstellt die Statusleiste
void CBerylliumMainframe::BeCreateStatusBar(void)
{
	// Statusleiste erstellen mit 2 Abschnitten, falls was schief geht... raus hier.
	if ( CreateStatusBar( 2, wxST_SIZEGRIP ) == NULL )
		return;

	// Abschnitte anpassen
	int iWidths[2] = { -1, 150 };
	SetStatusWidths( 2, iWidths );

	// Hilfetexte immer auf dem ersten Abschnitt anzeigen lassen
	SetStatusBarPane(0);
}

// Hilfsfunktionen (Konstruktor): Erstellt die Symbolleiste
void CBerylliumMainframe::BeCreateToolBar(void)
{
	// Symbolleiste erstellen
	wxToolBar *toolbar = CreateToolBar( wxTB_FLAT | wxTB_DOCKABLE );

	// Keine Symbolleiste erstellt? Dann lieber raus hier.
	if ( toolbar == NULL )
		return;

	// Bitmaps laden
	wxBitmap bmpEmpty(16, 16);
	wxBitmap bmpNew(icon_new_xpm);
	wxBitmap bmpOpen(icon_open_xpm);
	wxBitmap bmpSave(icon_save_xpm);
	wxBitmap bmpPrint(icon_print_xpm);
	wxBitmap bmpHelp(icon_help_xpm);
	wxBitmap bmpLeft(icon_left_xpm);
	wxBitmap bmpRight(icon_right_xpm);
	wxBitmap bmpGHS(icon_useghs_xpm);

	// Tools hinzufügen
	toolbar->AddTool( beID_NEW, _(L"Neue Betriebsanweisung"), bmpNew, _(L"Neue Betriebsanweisung") );
	toolbar->AddTool( beID_OPEN, _(L"Betriebsanweisung öffnen"), bmpOpen, _(L"Betriebsanweisung öffnen") );
	toolbar->AddTool( beID_SAVE, _(L"Betriebsanweisung speichern"), bmpSave, _(L"Betriebsanweisung speichern") );
	toolbar->AddSeparator();
	toolbar->AddTool( beID_PAGE_LEFT, _(L"Vorherige Seite"), bmpLeft, _(L"Vorherige Seite") );
	toolbar->AddTool( beID_PAGE_RIGHT, _(L"Nächste Seite"), bmpRight, _(L"Nächste Seite") );
	toolbar->AddSeparator();
	toolbar->AddTool( beID_SWITCH_GHS, _(L"\"Globally Harmonized System of Classification\" benutzen"), bmpGHS, _(L"\"Globally Harmonized System of Classification\" benutzen"), wxITEM_CHECK );
	toolbar->AddSeparator();
	toolbar->AddTool( beID_PRINT, _(L"Betriebsanweisung drucken"), bmpPrint, _(L"Betriebsanweisung drucken") );
	toolbar->AddTool( beID_HELP_INFO, _(L"Info über Beryllium"), bmpHelp, _(L"Info über Beryllium") );

	// Symbolleiste updaten
	toolbar->Realize();
}

// IObserver: Model wurde verändert... wir müssen jetzt darauf reagieren
void CBerylliumMainframe::ModelHasChanged()
{
	// Titel des Hauptfensters ändern...
	wxString szWindowTitle;
	szWindowTitle << m_Document.GetFilename() << ( m_Document.HasChanged() ? _(L"*") : _("") ) << _(L" - Beryllium¹º");
	SetTitle( szWindowTitle );

	// Gibt es eine aktuelle Ansicht?
	if ( m_ViewManager.GetActiveView() == NULL )
		return;

	// Seitenzahlen ermitteln
	unsigned int iNoPages	= m_ViewManager.GetActiveView()->GetNumberOfPages();
	unsigned int iPage		= m_ViewManager.GetActiveView()->GetPageNumber() + 1;

	// Einen String zusammenbasteln...
	wxString szPages;
	szPages << _(L"Seite ") << iPage << _("/") << iNoPages;

	// Statusbarupdate: Seitenanzahl
	SetStatusText( szPages, 1 );

	// GHS-Modus?
	GetToolBar()->ToggleTool( beID_SWITCH_GHS, m_Document.IsGHSMode() );

	// Neu zeichnen (zeichnet auch alle Child-Fenster neu)
	Refresh();
}

// Ereignis: Fenstergröße wurde geändert
void CBerylliumMainframe::OnSize(wxSizeEvent& event)
{
	// Ansichtsfenster erneuern
	m_ViewManager.Refresh( false );

	event.Skip();
}

void CBerylliumMainframe::OnEventInfoBox( wxCommandEvent &event )
{
	// Dialogobjekt erstellen
	CBerylliumInfoDialog dlg( this );

	// Dialog aufrufen
	dlg.ShowModal();
}

// Ereignis: Vorherige Seite
void CBerylliumMainframe::OnEventPageLeft( wxCommandEvent &event )
{
	// Gibt es eine aktuelle Ansicht?
	if ( m_ViewManager.GetActiveView() == NULL )
		return;

	// Vorherige Seite aufrufen
	m_ViewManager.GetActiveView()->PrevPage();
}

// Ereignis: Nächste Seite
void CBerylliumMainframe::OnEventPageRight( wxCommandEvent &event )
{
	// Gibt es eine aktuelle Ansicht?
	if ( m_ViewManager.GetActiveView() == NULL )
		return;

	// Nächste Seite aufrufen
	m_ViewManager.GetActiveView()->NextPage();
}

// Ereignis: Neue Datei
void CBerylliumMainframe::OnEventNewFile( wxCommandEvent &event )
{
	// Wurde das Dokument geändert, aber noch nicht gespeichert?
	if ( m_Document.HasChanged() )
	{
		int iRet = wxMessageBox(_(L"Das aktuelle Dokument wurde noch nicht gespeichert. Möchten Sie das Dokument jetzt speichern?"),_(L"Beryllium¹º"), wxYES_NO | wxCANCEL | wxICON_QUESTION);

		// [Ja] -> Speichern.
		if ( iRet == wxYES )
			ProcessCommand( beID_SAVE );

		// [Abbrechen] -> Gar nichts machen
		else if ( iRet == wxCANCEL )
			return;
	}

	// Ein neues Dokument erstellen
	CBerylliumDocument newDocument;

	// das alte Model überschreiben
	m_Document = newDocument;

	// Die Beobachter eintragen
	m_Document.AddObserver( this );
	m_Document.AddObserver( &m_ViewManager );

	// Die Ansichten resetten
	m_ViewManager.InitManager();

	// Menüpunkt für die Ansicht erneuern
	ResetViewMenu();

	// Der Ansicht mitteilen, dass GHS angesagt ist (Änderung: 12.1.2014)
	m_ViewManager.SetGHSMode( true );

	// Auch dem Dokument mitteilen (Änderung: 12.1.2014)
	m_Document.SwitchToGHS( true );

	// Und alle davon in Kenntnis setzen
	m_Document.NotifyAllObserver();
}

// Ereignis: Datei öffnen
void CBerylliumMainframe::OnEventOpenFile( wxCommandEvent &event )
{
	// Wurde das Dokument geändert, aber noch nicht gespeichert?
	if ( m_Document.HasChanged() )
	{
		int iRet = wxMessageBox(_(L"Das aktuelle Dokument wurde noch nicht gespeichert. Möchten Sie das Dokument jetzt speichern?"),_(L"Beryllium¹º"), wxYES_NO | wxCANCEL | wxICON_QUESTION);

		// [Ja] -> Speichern.
		if ( iRet == wxYES )
			ProcessCommand( beID_SAVE );

		// [Abbrechen] -> Gar nichts machen
		else if ( iRet == wxCANCEL )
			return;
	}

	// DialogBox öffnen
	wxFileDialog fd( this, _(L"Öffnen"), "", _(L"*.be"), _(L"Betriebsanweisung (*.be)|*.be|Betriebsanweisung (*.xml)|*.xml|Alle Dateien|*.*"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	// DialogBox anzeigen
	if ( fd.ShowModal() == wxID_OK )
	{
		// Leeres Model erstellen
		CBerylliumDocument newDocument;

		// kompletten Pfad + Dateinamen holen
		wxString szPath = fd.GetPath();

		// Dateinamen holen
		wxString szFilename = fd.GetFilename();

		// Dateinamen setzen
		newDocument.SetFilename( szFilename );

		// Pfad setzen
		newDocument.SetPath( szPath );

		// Datei laden
		if ( newDocument.LoadFromFile( szPath ) != 0 )
		{
			wxMessageBox( _(L"Fehler beim Laden der Datei"), _(L"Beryllium¹º"));
			return;
		}

		// Altes Model ersetzen
		m_Document = newDocument;

		// Beobachter setzen
		m_Document.AddObserver( this );
		m_Document.AddObserver( &m_ViewManager );

		// Ansichten resetten
		m_ViewManager.InitManager();

		// Menüpunkt für die Ansicht erneuern
		ResetViewMenu();

		// GHS-Modus geladen?
		m_ViewManager.SetGHSMode( m_Document.IsGHSMode() );

		// Daten wurden noch nicht verändert.
		m_Document.Saved();

		// Alle benachrichtigen
		m_Document.NotifyAllObserver();

		// Event an View weiterleiten (für besonders aufwendige Berechnungen)
		m_ViewManager.OnFileWasLoaded();
	}
}

// Ereignis: Datei speichern
void CBerylliumMainframe::OnEventSaveFile( wxCommandEvent &event )
{
	// Haben wir schon einen Dateinamen?
	// Wenn nicht, dann SaveAs ausführen!
	if ( !m_Document.HasFilename() )
	{
		ProcessCommand( beID_SAVEAS );

		return;
	}

	// Event an View weiterleiten (für besonders aufwendige Berechnungen)
	m_ViewManager.OnFileWillBeSaved();

	// Ansonsten einfach speichern
	m_Document.SaveToFile();

	// Alle benachrichtigen
	m_Document.NotifyAllObserver();
}

// Ereignis: Datei speichern unter...
void CBerylliumMainframe::OnEventSaveFileAs( wxCommandEvent &event )
{
	// Dateinamen holen...
	wxString sFileName =	m_Document.GetFilename();

	// DialogBox öffnen
	wxFileDialog fd( this, _(L"Speichern unter"), "", sFileName, _(L"Betriebsanweisung (*.be)|*.be|Betriebsanweisung (*.xml)|*.xml|Alle Dateien|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	// DialogBox anzeigen
	if ( fd.ShowModal() == wxID_OK )
	{
		// Dateinamen holen
		sFileName = fd.GetFilename();

		// Pfad holen
		wxString sPath = fd.GetPath();

		// Dateinamen setzen
		m_Document.SetFilename( sFileName );

		// Pfad setzen (komplett inkl. Datename)
		m_Document.SetPath( sPath );

		// Event an View weiterleiten (für besonders aufwendige Berechnungen)
		m_ViewManager.OnFileWillBeSaved();

		// Speichern
		m_Document.SaveToFile();

		// Alle benachrichtigen
		m_Document.NotifyAllObserver();
	}
}

// Ereignis: Datei drucken...
void CBerylliumMainframe::OnEventPrintFile( wxCommandEvent &event )
{
    // 06.12.2013: Lokales Objekt wegen wxGTK::Print-Fehler. Einziger Nachteil ist,
    // dass es die Einstellungen nicht zwischenspeichert.
    wxPrinter printer;

	// Ein wxPrintOut-Objekt von der View holen
	wxPrintout *print = m_ViewManager.GetPrintOut();

	// Drucken
	printer.Print(this, print, true);

	// Fertig
	return;
}

// Ereignis: Druckvorschau
void CBerylliumMainframe::OnEventPrintPreview( wxCommandEvent &event )
{
	// Ein wxPrintOut-Objekt von der View holen
	wxPrintout *print = m_ViewManager.GetPrintOut();

	// Keins bekommen? Raus hier!
	if ( print == NULL )
		return;

	// Vorschau vorbereiten
    wxPrintPreview *preview = new wxPrintPreview(print, NULL);

	if (!preview->IsOk() )
		wxMessageBox( _(L"Kein aktueller Drucker verfügbar."), _(L"Beryllium¹º"));

	// Fenster für die Vorschau vorbereiten
	wxPreviewFrame *frame = new wxPreviewFrame(preview, this, m_Document.GetFilename(), wxPoint(100, 100), wxSize(600, 650));

	// Zentrieren, initialsieren und zeigen
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);

	// Weitermachen
	event.Skip();
}

// Ereignis: Programm beenden
void CBerylliumMainframe::OnEventExit( wxCommandEvent &event )
{
	// Beenden
	Close();
}

// Ereignis: GHS benutzen
void CBerylliumMainframe::OnEventSwitchGHS( wxCommandEvent &event )
{
	// Der Ansicht mitteilen, dass der Benutzer das System wechseln möchte
	m_ViewManager.SetGHSMode( event.IsChecked() );

	// Auch dem Dokument mitteilen
	m_Document.SwitchToGHS( event.IsChecked() );

	// Alle benachrichtigen
	m_Document.NotifyAllObserver();
}

// Ereignis: Zeige Symbolleiste
void CBerylliumMainframe::OnEventShowToolBar( wxCommandEvent &event )
{
	// Statusleiste vorhanden?
	if ( GetToolBar() == NULL )
		return;

	// Häkchen?
	bool bCheck = GetMenuBar()->IsChecked( beID_VIEW_TOOLBAR );

	// Zeige/Verstecke die Leiste
	GetToolBar()->Show( bCheck );

	// Ansicht entsprechend anpassen
	PostSizeEvent();
}

// Ereignis: Zeige Statusleiste
void CBerylliumMainframe::OnEventShowStatusBar( wxCommandEvent &event )
{
	// Statusleiste vorhanden?
	if ( GetStatusBar() == NULL )
		return;

	// Häkchen?
	bool bCheck = GetMenuBar()->IsChecked( beID_VIEW_STATUSBAR );

	// Zeige/Verstecke die Leiste
	GetStatusBar()->Show( bCheck );

	// Ansicht entsprechend anpassen
	PostSizeEvent();
}

// Ereignis: Breakout anzeigen
void CBerylliumMainframe::OnEventBreakout( wxCommandEvent &event )
{
	// Breakout-Dialog erstellen
	CBerylliumBreakoutDialog dlg( this );

	// Anzeigen
	dlg.ShowModal();
}

// Ereignis: Popupmenu geclickt
void CBerylliumMainframe::OnEventPopupMenuClick( wxCommandEvent &event )
{
	// An View-Manager weiterreichen
	m_ViewManager.OnPopupMenuClick( event );
}

// Ereignis: Auf Updates überprüfen
void CBerylliumMainframe::OnEventUpdateCheck( wxCommandEvent &event )
{
	// Auf Updates prüfen
	if ( ::wxGetApp().IsThereANewVersion() )
	{
		if ( wxMessageBox( _(L"Eine neue Version von Beryllium¹º ist verfügbar! Möchten Sie die Beryllium¹º-Webseite dazu aufrufen?"), _(L"Beryllium¹º"), wxICON_QUESTION | wxYES_NO ) == wxYES )
			::wxGetApp().OpenUpdatePage();
	}

	// Keine neuen Updates vorhanden
	else
		wxMessageBox( _(L"Diese Version von Beryllium¹º ist aktuell!"), _(L"Beryllium¹º"), wxICON_INFORMATION | wxOK );
}

// Ereignis: Wird ausgeführt, wenn das Hauptfenster geschlossen wird
void CBerylliumMainframe::OnClose( wxCloseEvent &event )
{
	// Können wir überhaupt einschreiten?
	if ( event.CanVeto() )
	{
		// Wurde das Dokument geändert, aber noch nicht gespeichert?
		if ( m_Document.HasChanged() )
		{
			int iRet = wxMessageBox( _(L"Das aktuelle Dokument wurde noch nicht gespeichert. Möchten Sie das Dokument jetzt speichern?"), _(L"Beryllium¹º"), wxYES_NO | wxCANCEL | wxICON_QUESTION);

			// [Ja] -> Speichern.
			if ( iRet == wxYES )
				ProcessCommand( beID_SAVE );

			// [Abbrechen] -> Zerstören verhindern!
			else if ( iRet == wxCANCEL )
			{
				event.Veto();
				return;
			}
		}

	}

	// Fenster zerstören
	Destroy();
}

// Ereignis: Bugreport angwählt
void CBerylliumMainframe::OnEventBugReport( wxCommandEvent &event )
{
	// Dialog vorbereiten
	wxTextEntryDialog report( this, _(L"Kritik? Lob? Verbesserungsvorschläge? Fehlerberichte? Nur her damit!\n Ich freue mich über jedes Feedback!"), _(L"Feedback senden"), "", wxTE_MULTILINE | wxCENTRE | wxOK | wxCANCEL | wxRESIZE_BORDER );

	// Der Orginaldialog ist etwas klein...
	report.SetSize( 400, 300 );

	// Ok geklickt?
	if ( report.ShowModal() == wxID_OK )
	{
		// Verbindung anlegen
		wxHTTP http;

		// HTTP-Anfarge vorbereiten
		http.SetHeader( "Content-type", "application/x-www-form-urlencoded; charset=utf-8" );
		http.SetTimeout(20); // 20 Sekunden TimeOut

		// Post-Daten vorbereiten
		http.SetPostBuffer( wxString::Format("version=%s&feedback=%s", STRFILEVER, report.GetValue() ) );

		// Anfrage an keksecks vorbereiten
		http.Connect( "www.keksecks.de" );

		// InputStream erzeugen
		wxInputStream *httpStream = http.GetInputStream( "/beryllium10/feedback.php" );

		// Stream ok?
#ifdef _DEBUG
		if (http.GetError() == wxPROTO_NOERR)
		{

			wxString data;
			wxStringOutputStream datastream( &data );

			httpStream->Read(datastream);

			wxMessageBox(data);
		}
#endif

		// Da uns das Ergebnis nicht interessiert, einfach wieder löschen
		wxDELETE(httpStream);

		// HTTP-Anfrage schließen
		http.Close();

		// Danke (wird angezeigt egal obs geklappt hat oder nicht)
		wxMessageBox( _(L"Ihr Feedback wurde erfolgreich versendet! Vielen Dank!"), _(L"Beryllium¹º"), wxICON_INFORMATION | wxOK );
	}

	event.Skip();
}

// Ereignis: Hilfethemen anzeigen
void CBerylliumMainframe::OnEventHelpTopics( wxCommandEvent &event )
{
	// Sprache
	int iLanguage = ::wxGetApp().GetLanguage();

	// Einfach diese Seite aufrufen, je nach Sprache
	if ( iLanguage == wxLANGUAGE_GERMAN )
		wxLaunchDefaultBrowser( "http://www.keksecks.de/beryllium10" );

	// Englische Version
	else
		wxLaunchDefaultBrowser( "http://www.keksecks.de/beryllium10/?page_id=163" );
}

// Öffnet eine Datei (Kommandozeile)
void CBerylliumMainframe::OpenFile( wxString file )
{
	// Dateiname erstellen
	wxFileName fd(file);

	// Leeres Model erstellen
	CBerylliumDocument newDocument;

	// kompletten Pfad + Dateinamen holen
	wxString szPath = fd.GetFullPath();

	// Dateinamen holen
	wxString szFilename = fd.GetFullName();

	// Dateinamen setzen
	newDocument.SetFilename( szFilename );

	// Pfad setzen
	newDocument.SetPath( szPath );

	// Datei laden
	if ( newDocument.LoadFromFile( szPath ) != 0 )
	{
		wxMessageBox( _(L"Fehler beim Laden der Datei"), file);
		return;
	}

	// Altes Model ersetzen
	m_Document = newDocument;

	// Beobachter setzen
	m_Document.AddObserver( this );
	m_Document.AddObserver( &m_ViewManager );

	// Ansichten resetten
	m_ViewManager.InitManager();

	// Menüpunkt für die Ansicht erneuern
	ResetViewMenu();

	// GHS-Modus geladen?
	m_ViewManager.SetGHSMode( m_Document.IsGHSMode() );

	// Daten wurden noch nicht verändert.
	m_Document.Saved();

	// Alle benachrichtigen
	m_Document.NotifyAllObserver();

	// Event an View weiterleiten (für besonders aufwendige Berechnungen)
	m_ViewManager.OnFileWasLoaded();
}

// Ereignis: Wechsel die Ansicht
void CBerylliumMainframe::OnEventChangeView( wxCommandEvent &event )
{
	// ID der Ansicht
	unsigned int view = event.GetId() - beID_VIEW_FIRST;

	// Alle Häkchen entfernen
	for ( unsigned int i = beID_VIEW_FIRST; i < beID_VIEW_LAST; ++i )
	{
		if ( (GetMenuBar()->FindItem(i) != NULL) && (i != event.GetId()) )
			GetMenuBar()->Check( i, false );
	}

	// Ansicht setzen
	m_ViewManager.SetActiveView( view );

	// GHS-Modus an?
	bool bGHS = GetToolBar()->GetToolState(beID_SWITCH_GHS);

	// Der Ansicht mitteilen, dass der Benutzer das System wechseln möchte
	m_ViewManager.SetGHSMode( bGHS );

	// Auch dem Dokument mitteilen
	m_Document.SwitchToGHS( bGHS );

	// Alle benachrichtigen
	m_Document.NotifyAllObserver();
}



// Ereignis: Substanzmengen-Multiplikator
void CBerylliumMainframe::OnEventSubstanceMultiplier( wxCommandEvent &event )
{
	// Dialog vorbereiten
	CBerylliumMultiplierDialog dlg(this);

	// Aufrufen
	if ( dlg.ShowModal() == wxID_OK )
		m_Document.MultiplyBatchesBy( dlg.GetFactor() );
}

// Ereignis: Einstellungen
void CBerylliumMainframe::OnEventOptions( wxCommandEvent &event )
{
	// Dialog vorbereiten
	CBerylliumOptionsDialog dlg(this);

	// Sprache setzen
	dlg.SetLanguage( wxGetApp().GetLanguage() );

	// Aufrufen
	if ( dlg.ShowModal() == wxID_OK )
	{
		// Sprache setzen
		wxGetApp().SetLanguage( dlg.GetLanguage() );
	}
}

// Ereignis: Sortieren
void CBerylliumMainframe::OnEventSort( wxCommandEvent &event )
{
	// Daten holen
	CBerylliumData data; m_Document.GetData( data );

	// Sortieren nach...
	switch( event.GetId() )
	{
		// ... Name:
		case beID_TOOL_SORTBY_NAME:
			data.SortSubstancesByName();
			break;

		// ... Menge:
		case beID_TOOL_SORTBY_AMOUNT:
			data.SortSubstancesByAmount();
			break;

		// ... Gefährlichkeit:
		case beID_TOOL_SORTBY_DANGER:
			// Erstmal Hinweis zu Be¹º-Punkte anzeigen
			wxMessageBox(
				_(L"Be¹º-Punkte werden anhand der verfügbaren Gefahrstoffdaten einer Substanz berechnet. Sie sollen eine relative - keine absolute - Einschätzung der Gefahren für den Anwender der Stoffe auf dieser Betriebsanweisung liefern.\n\nDieses System erhebt keinen Anspruch auf Perfektion und es kann passieren, dass Gefahren sowohl über- als auch unterschätzt werden. Die Verantwortlichkeit liegt immer beim (ausgebildeten) Anwender!"),
				_(L"Hinweis zu Be¹º-Punkten"), wxICON_EXCLAMATION | wxCENTRE | wxOK, this);

			// Sortieren
			data.SortSubstancesByDanger();
			break;
	};

	// Daten setzen
	m_Document.SetData(data);
}

// Entfernt alle Häkchen von den Ansichten und setzt ein Häkchen bei der ersten Ansicht
void CBerylliumMainframe::ResetViewMenu()
{
	// Alle Häkchen entfernen
	for ( unsigned int i = beID_VIEW_FIRST; i < beID_VIEW_LAST; ++i )
	{
		if ( GetMenuBar()->FindItem(i) != NULL )
			GetMenuBar()->Check( i, false );
	}

	// Häkchen beim ersten Menüpunkt setzen
	if ( GetMenuBar()->FindItem(beID_VIEW_FIRST) != NULL )
			GetMenuBar()->Check( beID_VIEW_FIRST, true );
}
