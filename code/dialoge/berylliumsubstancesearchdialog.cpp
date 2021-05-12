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
#include "berylliumsubstancesearchdialog.h"

#include "../berylliumapplication.h"

#include <wx/progdlg.h>

// bitmap für den dialog
#include "../../resources/bitmaps/insert_substance.xpm"

// hersteller
#include "../provider/berylliumproviderwiki.h"
#include "../provider/berylliumprovidermerck.h"
#include "../provider/berylliumprovidergestis.h"
#include "../provider/berylliumproviderlocal.h"

// Für das Suchfeld (wird vom Sprachbibliotheksprogramm gefunden und übersetzt)
const wxString searchtext = _("Search");

IMPLEMENT_CLASS(CBerylliumSubstanceSearchDialog, wxDialog)

BEGIN_EVENT_TABLE(CBerylliumSubstanceSearchDialog, wxDialog)
	EVT_INIT_DIALOG( CBerylliumSubstanceSearchDialog::OnInitDialog )
	EVT_BUTTON( beID_Continue, CBerylliumSubstanceSearchDialog::OnOK )
	EVT_LISTBOX( beID_ChoiceBox, CBerylliumSubstanceSearchDialog::OnChoice )
	EVT_SEARCHCTRL_SEARCH_BTN( beID_SearchCtrl, CBerylliumSubstanceSearchDialog::OnSearch )
	EVT_TEXT_ENTER( beID_SearchCtrl, CBerylliumSubstanceSearchDialog::OnSearch )
	EVT_HTML_LINK_CLICKED( beID_ChoiceBox, CBerylliumSubstanceSearchDialog::OnLinkClick )
	EVT_BUTTON( beID_Preferences, CBerylliumSubstanceSearchDialog::OnPreferences )
	EVT_BUTTON( beID_SearchButton, CBerylliumSubstanceSearchDialog::OnSearch )

	EVT_MENU_RANGE( beID_Source_GESTIS, beID_Source_LAST, CBerylliumSubstanceSearchDialog::OnSelectSource )
	EVT_MENU_RANGE( beID_Prefs_EXACT, beID_Prefs_LAST, CBerylliumSubstanceSearchDialog::OnSearchPreferences )

	EVT_MENU( beID_Prefs_SELLOCALDB, CBerylliumSubstanceSearchDialog::OnSearchSelectLocalDatabase )
END_EVENT_TABLE()

CBerylliumSubstanceSearchDialog::CBerylliumSubstanceSearchDialog( wxWindow *parent )
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Substanz hinzufügen"), wxDefaultPosition, wxSize( 700, 570));

	// Dialogelemente erstellen
	CreateLayout();

	// Liste leeren und anfügen
	EmptyList();

	// Provider anlegen
	IProvider *gestis	= new CBerylliumProviderGESTIS;		gestis->SetId(beID_Source_GESTIS);
	IProvider *merck	= new CBerylliumProviderMerck;		merck->SetId(beID_Source_MERCK);
	IProvider *wiki		= new CBerylliumProviderWiki;		wiki->SetId(beID_Source_WIKIDE);
	IProvider *local	= new CBerylliumProviderLocal;		local->SetId(beID_Source_LOCALDB);

	// Hersteller hinzufügen (in dieser Reihenfolge)
	m_providerlist.push_back( local );
	m_providerlist.push_back( gestis );
	m_providerlist.push_back( merck );
	m_providerlist.push_back( wiki );

	// Proxy-Daten holen
	wxString proxyhost = ::wxGetApp().GetConfigData( "proxyhost" );
	wxString proxyport = ::wxGetApp().GetConfigData( "proxyport" );

	// Liste durchgehen, Sprache und Proxy setzen
	for ( unsigned int i = 0; i < m_providerlist.size(); ++i )
	{
		// Sprachen setzen
		m_providerlist[i]->SetLanguage(wxGetApp().GetLanguage());

		// Proxy setzen
		if ( (proxyhost.length() > 0) && (proxyport.length() > 0) && proxyport.IsNumber() )
			m_providerlist[i]->SetProxy( proxyhost, atoi(proxyport) );
		//m_providerlist[i]->SetProxy( "77.75.204.98", 80 );
		//m_providerlist[i]->SetProxy( "213.133.141.197", 8080 );
	}

	// Hersteller sind selektiert
	wxString providerselected = ::wxGetApp().GetConfigData( "provider" );

	// In Liste transferieren...
	std::vector< wxString > list; BeTool::StringToList( providerselected, ",", list );

	// Liste durchgehen
	for ( unsigned int i = 0; i < list.size(); i++ )
	{
		if ( list[i].compare("gestis") == 0 )
			m_selectedDB.push_back( beID_Source_GESTIS );

		if ( list[i].compare("merck") == 0 )
			m_selectedDB.push_back( beID_Source_MERCK );

		if ( list[i].compare("wiki") == 0 )
			m_selectedDB.push_back( beID_Source_WIKIDE );

		if ( list[i].compare("local") == 0 )
			m_selectedDB.push_back( beID_Source_LOCALDB );
	}

	// Keine exakte Suche
	bExactMatch = false;

	// Substanzen in lokale Datenbank einspeichern?
	bSaveLocal = ( ::wxGetApp().GetConfigData( "savetolocaldatabase" ).Upper().compare("TRUE") == 0 );

	// Protokoll am Ende der Suchanfrage bzw. beim Download der Daten anzeigen?
	bEnableLogging = ( ::wxGetApp().GetConfigData( "enablelogging" ).Upper().compare("TRUE") == 0 );

	// Lokale Datenbank suchen und öffnen
	m_localdatabasename = ::wxGetApp().GetConfigData( "localdatabase" );

	// Ersetzen potentielle "\" durch "/", damit unter Linux (wo ein "\" durchaus ein legitimies Zeichen ist)
	// nicht die falsche Datei geladen wird. Windows akzeptiert sowohl "\" als auch "/".
	m_localdatabasename.Replace( "\\", "/", true );

	// Datei vorhanden und existiert auch?
	if ( m_localdatabasename.length() > 0 )
	{
		// Dateiobjekt erstellen
		wxFileName dblocal(m_localdatabasename);

		// Existiert? Dann laden!
		if ( dblocal.FileExists() )
			m_localdatabase.LoadFromFile( m_localdatabasename );
	}

	// Fokus setzen
    wxSearchField->SetFocus();
}

CBerylliumSubstanceSearchDialog::~CBerylliumSubstanceSearchDialog(void)
{
	// Hersteller-Zeiger löschen
	for ( unsigned int i = 0; i < m_providerlist.size(); ++i )
		delete m_providerlist[i];
}

void CBerylliumSubstanceSearchDialog::OnInitDialog( wxInitDialogEvent &event )
{
	// Weiter im Programm
	event.Skip();
}

void CBerylliumSubstanceSearchDialog::OnOK( wxCommandEvent &event )
{
	// Keine Auswahl getroffen? => Leere Substanzdaten einfügen
	// "Leere Substanzdaten einfügen" (ID = 0) => Leere Substanzdaten einfügen
	if ( (wxBeChoiceList->GetSelection() == wxNOT_FOUND) ||
			(wxBeChoiceList->GetSelection() == 0) )
	{
		// Dialog beenden: OK
		EndDialog( wxID_OK );

		// Rausspringen
		return;
	}

	// Substanz ausgewählt... super, dann brauchen wir noch den Hersteller nach der Substanz fragen
	substancedata temp = m_list[iLastIndex];

	// Hersteller nicht gesetzt?
	if ( temp.provider == NULL )
		return;

	// ProgressDialog erstellen
	wxProgressDialog *dlg = new wxProgressDialog( _(L"Substanzdaten holen"), wxString::Format( _(L"Hole Daten für '%s' von %s..."), temp.name, temp.provider->GetName()), 100, this, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE );

	// Fokus setzen
	dlg->SetFocus();

	// Log anschalten
	wxLogBuffer logBuffer;

	// Protokollieren?
	logBuffer.EnableLogging( bEnableLogging );

	// Hersteller nach Substanz fragen
	temp.provider->GetDataOfCompound( temp.serial, data );

	// Substanzdaten in Datenbank kopieren (aber nur, wenn wir nicht von dieser Laden und
	// der Nutzer das überhaupt will!)
	if ( (temp.provider->GetId() != beID_Source_LOCALDB) && bSaveLocal )
	{
		m_localdatabase.Add(data);

		// Datenbank speichern (falls vorhanden)
		if ( m_localdatabasename.length() > 0 )
			m_localdatabase.SaveToFile( m_localdatabasename );
	}

	// Auf 100% setzen
	dlg->Update(100);

	// ProgressDialog löschen
	delete dlg;

	// Falls die temporäre Datei existiert: Vernichten
    wxFileName tempfile("beryllium10.temp");

    // Existiert? Dann löschen!
    if ( tempfile.FileExists() )
        ::wxRemoveFile("beryllium10.temp");

	// Log anzeigen, falls aktiviert
	if ( bEnableLogging )
	{
		wxLogMessage( _(L"Mit einem Klick auf 'Details' sehen Sie das Protokoll für den Download der Daten für '%s' von '%s'."), temp.name, temp.provider->GetName());
		logBuffer.Flush();
	}

	// Dialog beenden: OK
	EndDialog( wxID_OK );

	// Weiter im Programm
	return;
}

void CBerylliumSubstanceSearchDialog::OnChoice( wxCommandEvent &event )
{
	// Farbe zum "Highlight"
	wxColour standard  = wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );
	wxColour highlight = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT );

	// Temporärer String
	wxString temp;

	// Altes Element verpassen wir die Standardfarbe
	temp = m_list[iLastIndex].text;
	temp.Replace( "$color$", standard.GetAsString(wxC2S_HTML_SYNTAX), true );
	wxBeChoiceList->SetString( iLastIndex, temp );

	// Elementdaten des selektierten Eintrages holen
	// und Farbe ersetzen...
	temp = m_list[event.GetInt()].text;
	temp.Replace( "$color$", highlight.GetAsString(wxC2S_HTML_SYNTAX), true );
	wxBeChoiceList->SetString( event.GetInt(), temp );

	// Letzten Index neu setzen
	iLastIndex = event.GetInt();

	// Weiter im Programm
	event.Skip();
}

// Erstellt alle Steuerelemente des Dialoges
void CBerylliumSubstanceSearchDialog::CreateLayout()
{
	// Sizer erstellen und festlegen
	// =============================
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* SizerMain;
	SizerMain = new wxBoxSizer( wxVERTICAL );

	SizerMain->SetMinSize( wxSize( 700,550 ) );
	wxPanel* panelTop;
	panelTop = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), 0 );
	panelTop->SetBackgroundColour( wxColour( 255, 255, 255 ) );

	wxBoxSizer* SizerTop;
	SizerTop = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* SizerText;
	SizerText = new wxBoxSizer( wxVERTICAL );

	SizerText->Add( 0, 10, 0, 0, 0 );

	// Titelabschnitt
	// ==============
	wxStaticText* staticTextTitle = new wxStaticText( panelTop, wxID_ANY, _(L"Welche Substanz möchten Sie hinzufügen?"), wxDefaultPosition, wxDefaultSize, 0 );
	staticTextTitle->Wrap( -1 );
	staticTextTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), 70, 90, 92, false, wxEmptyString ) );

	SizerText->Add( staticTextTitle, 0, wxBOTTOM|wxLEFT|wxTOP, 5 );

	wxStaticText* staticTextSubTitle = new wxStaticText( panelTop, wxID_ANY, _(L"Bitte geben Sie den Namen, die CAS-Nummer oder die Summenformel an."), wxDefaultPosition, wxDefaultSize, 0 );
	staticTextSubTitle->Wrap( -1 );
	SizerText->Add( staticTextSubTitle, 0, wxLEFT, 20 );

	SizerTop->Add( SizerText, 1, wxEXPAND|wxLEFT, 15 );
	SizerTop->Add( 0, 0, 1, wxEXPAND, 5 );

	wxStaticBitmap* m_bitmapChemicals = new wxStaticBitmap( panelTop, wxID_ANY, wxBitmap( insert_substance_xpm ), wxDefaultPosition, wxDefaultSize, 0 );
	SizerTop->Add( m_bitmapChemicals, 0, wxALL, 0 );

	panelTop->SetSizer( SizerTop );
	panelTop->Layout();
	SizerTop->Fit( panelTop );
	SizerMain->Add( panelTop, 0, wxALIGN_RIGHT|wxALL|wxFIXED_MINSIZE|wxEXPAND, 0 );

	wxStaticLine* staticLineTop = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	SizerMain->Add( staticLineTop, 0, wxEXPAND | wxALL, 0 );

	// Inhaltsabschnitt
	// ================
	//wxBoxSizer* SizerContent = new wxBoxSizer( wxVERTICAL );
	//SizerContent->SetMinSize( wxSize( 675, 280 ) );

	// 2014.02.23: Suchknopf hinzugefügt, weil es verwirrend für neue Benutzer ist, wenn sie etwas in das Suchfeld eingeben
	// und auf "Ok" klicken
	wxBoxSizer* sizerSearch;
	sizerSearch = new wxBoxSizer( wxHORIZONTAL );

	// Suchfeld
	wxSearchField = new wxSearchCtrl( this, beID_SearchCtrl, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	sizerSearch->Add( wxSearchField, 1, wxEXPAND | wxALL, 10 );

	// Suchknopf
	m_buttonsearch = new wxButton( this, beID_SearchButton, _(L"&Suchen"), wxDefaultPosition, wxDefaultSize, 0 );
	sizerSearch->Add( m_buttonsearch, 0, wxTOP|wxRIGHT|wxBOTTOM | wxALIGN_LEFT, 10 );

	SizerMain->Add( sizerSearch, 0, wxEXPAND | wxALL, 0 );

	// SimpleHTML-Liste
	wxBeChoiceList = new wxSimpleHtmlListBox( this, beID_ChoiceBox, wxDefaultPosition, wxSize( 590, 260 ) );
	SizerMain->Add( wxBeChoiceList, 1, wxEXPAND | wxALL, 10 );

	// Buttonleiste (unten)
	// ====================
	SizerMain->AddSpacer(10);

	wxStaticLine* staticLineBottom = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	SizerMain->Add( staticLineBottom, 0, wxEXPAND | wxALL, 0 );

	wxBoxSizer* SizerButtons = new wxBoxSizer( wxHORIZONTAL );

	SizerButtons->SetMinSize( wxSize( -1,25 ) );

	m_buttonpreferences = new wxButton( this, beID_Preferences, _(L"&Optionen"), wxDefaultPosition, wxDefaultSize, 0 );
	SizerButtons->Add( m_buttonpreferences, 0, wxALL | wxALIGN_LEFT, 5 );

	SizerButtons->AddStretchSpacer();

	m_buttonContinue = new wxButton( this, beID_Continue, _(L"&Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	m_buttonContinue->SetDefault();
	SizerButtons->Add( m_buttonContinue, 0, wxALL | wxALIGN_RIGHT, 5 );

	m_buttonCancel = new wxButton( this, wxID_CANCEL, _(L"&Abbrechen"), wxDefaultPosition, wxDefaultSize, 0 );
	SizerButtons->Add( m_buttonCancel, 0, wxALL | wxALIGN_RIGHT, 5 );

	SizerMain->Add( SizerButtons, 0, wxFIXED_MINSIZE|wxALIGN_RIGHT|wxALL|wxEXPAND, 5 );

	// Fertigstellen
	// =============
	this->SetSizer( SizerMain );
	this->Layout();

	this->Centre( wxBOTH );
}

// Wird aufgerufen, wenn im Suchfeld Enter gedrückt wurde (oder die Lupe angeklickt wurde)
void CBerylliumSubstanceSearchDialog::OnSearch( wxCommandEvent &event )
{
	// Keine Hersteller gewählt?
	if ( m_selectedDB.size() == 0 )
	{
		wxMessageBox( _(L"Es wurden keine Quellen zur Substanzsuche ausgewählt. Bitte klicken Sie auf \"Optionen\" und wählen mindestens eine Quelle aus um fortzufahren."), _(L"Beryllium¹º"), wxICON_EXCLAMATION | wxOK );

		// Zurück
		return;
	}

	// Liste leeren
	EmptyList();

	// Suchstring vom Suchfeld holen
	wxString searchfor = wxSearchField->GetValue();

	// ProgressDialog erstellen
	wxProgressDialog *dlg = new wxProgressDialog( _(L"Substanzdaten suchen"), _(L"Suche nach Substanzdaten..."), 100, this, wxPD_APP_MODAL | wxPD_SMOOTH | wxPD_AUTO_HIDE );

	// Fokus geben
	dlg->SetFocus();

	// Log erstellen
	wxLogBuffer logBuffer;

	// Protokollieren?
	logBuffer.EnableLogging(bEnableLogging);

	// Anzahl Hersteller
	unsigned int count = m_providerlist.size();

	// Alle Hersteller durchgehen und nach Substanzen fragen...
	for ( unsigned int i = 0; i < count; ++i )
	{
		// Ist der Hersteller überhaupt gewünscht?
		bool bSkip = true;

		// Suchen
		for ( unsigned int j = 0; j < m_selectedDB.size(); ++j )
		{
			// Gefunden? Dann nichts tun!
			if ( m_selectedDB[j] == m_providerlist[i]->GetId() )
				bSkip = false;
		}

		if ( bSkip )
			continue;

		// Fortschritt
		int progress = (i*100) / count;

		// Update des ProgressDialoges
		dlg->Update( progress, wxString::Format( _(L"Hole Daten von %s..."), m_providerlist[i]->GetName() ) );

		// Stelle Suchanfrage
		if ( m_providerlist[i]->SearchForCompound( searchfor, bExactMatch ) )
		{
			// List anlegen
			std::vector< IProvider::result > m_results;

			// Liste füllen
			m_providerlist[i]->GetResults( m_results );

			// Ergebnisse in Liste einfügen
			for ( unsigned int j = 0; j < m_results.size(); ++j )
				AddToList( m_results[j].name, m_providerlist[i], m_results[j].serial, m_results[j].data, m_results[j].info );
		}

		// Keine Ergebnisse oder Fehler?
		else
			AddToList( _( wxString::Format( _(L"%s lieferte keine Suchergebnisse für '%s'."), m_providerlist[i]->GetName(), searchfor) ), NULL, 0, "", "" );

	}

	// Auf 100% setzen
	dlg->Update(100);

	// ProgressDialog löschen
	delete dlg;

	// Fokus setzen
    wxSearchField->SetFocus();

    // Falls die temporäre Datei existiert: Vernichten
    wxFileName tempfile("beryllium10.temp");

    // Existiert? Dann löschen!
    if ( tempfile.FileExists() )
        ::wxRemoveFile("beryllium10.temp");

	// Log anzeigen, falls aktiviert.
	if ( bEnableLogging )
	{
		wxLogMessage( _(L"Mit einem Klick auf 'Details' sehen Sie das Protokoll für Ihre Suchanfrage zu '%s'."), searchfor);
		logBuffer.Flush();
	}
}

// Löscht die Liste und fügt einen Standardeintrag hinzu
void CBerylliumSubstanceSearchDialog::EmptyList()
{
	// Liste leeren
	m_list.clear();

	// Auch die Listbox leeren
	wxBeChoiceList->Clear();

	// Index auf 0 setzen
	iLastIndex = 0;

	// Dummy-Eintrag hinzufügen
	AddToList( _(L"Leere Substanzdaten einfügen"), NULL, 0, "", "" );
}

// Fügt einen Eintrag hinzu
void CBerylliumSubstanceSearchDialog::AddToList( const wxString name, IProvider *provider, const int serial = 0, const wxString url = "", const wxString subtitle = "" )
{
	// Standardfarbe und -schrift holen
	wxColour standardcolor	= wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT );

	// Datenblock anlegen
	substancedata data;

	// Herstellername
	wxString providername;

	// Hersteller gesetzt?
	if ( provider != NULL )
		providername = provider->GetName();

	// Quelle
	wxString source;

	if ( url.length() > 0 )
		source = " (<a href=\"" + url + "\">"+ _("Quelle") +"</a>)";

	// Zweite Zeile?
	wxString moreinfo;

	if ( subtitle.length() > 0 )
		moreinfo = "<br><font size=\"1\">" + subtitle + "</font>";

	// Anzeigetext
	data.text = "<table><tr><td width=\"75%\"><font color=\"$color$\"><font size=\"2\"><b>" + name + "</b>" + source + "</font>" + moreinfo + "</font></td><td width=\"25%\" valign=\"top\"><font color=\"$color$\"><font size=\"2\">" + providername + "</font></font></td></tr></table>";

	// Hersteller
	data.provider = provider;

	// Serial
	data.serial = serial;

	// Name ist auch ganz wichtig
	data.name = name;

	// in die eigenen Liste einfügen
	m_list.push_back(data);

	// Schriftfarbe anpassen
	data.text.Replace("$color$", standardcolor.GetAsString( wxC2S_HTML_SYNTAX ), true );

	// an die Listbox anfügen
	wxBeChoiceList->Append( data.text );
}

// Wird aufgerufen, wenn ein Link in einer Zelle geklickt wurde
void CBerylliumSubstanceSearchDialog::OnLinkClick( wxHtmlLinkEvent& event )
{
	// Link aufrufen
	wxLaunchDefaultBrowser( event.GetLinkInfo().GetHref() );
}

// Wird aufgerufen, wenn auf "Optionen" geklickt wurde
void CBerylliumSubstanceSearchDialog::OnPreferences( wxCommandEvent &event )
{
	// Button nicht vorhanden?
	if ( m_buttonpreferences == NULL )
		return;

	// Menu vorbereiten
	wxMenu *menu = new wxMenu();

	// Hersteller-Menu erstellen
	wxMenu *providerMenu = new wxMenu();

	providerMenu->Append( new wxMenuItem( 0, beID_Source_GESTIS, _(L"GESTIS Stoffdatenbank"), "", wxITEM_CHECK ) );
	providerMenu->Append( new wxMenuItem( 0, beID_Source_MERCK, _(L"Merck KGaA"), "", wxITEM_CHECK ) );
	/* TODO: euSDB benötigt seit einem Update Logindaten, deswegen hier erstmal entfernt.
	// ENTFERNT: 14.09.2013
	providerMenu->Append( new wxMenuItem( 0, beID_Source_EUSDB, _("euSDB"), "", wxITEM_CHECK ) );
	*/
	providerMenu->Append( new wxMenuItem( 0, beID_Source_WIKIDE, _(L"Wikipedia (de)"), "", wxITEM_CHECK ) );
	providerMenu->Append( new wxMenuItem( 0, beID_Source_LOCALDB, _(L"Lokale Datenbank"), "", wxITEM_CHECK ) );

	// Markieren
	for ( unsigned int i = 0; i < m_selectedDB.size(); ++i )
		providerMenu->Check( m_selectedDB[i], true );

	menu->AppendSubMenu( providerMenu, _(L"&Quellen") );

	// Einstellungen-Menu erstellen
	wxMenu *prefsMenu = new wxMenu();

	prefsMenu->Append( new wxMenuItem( 0, beID_Prefs_EXACT, _(L"Exakte Suche"), "", wxITEM_CHECK ) );
	prefsMenu->Append( new wxMenuItem( 0, beID_Prefs_SAVELOCAL, _(L"Substanzen in lokale Datenbank übernehmen"), "", wxITEM_CHECK ) );
	prefsMenu->Append( new wxMenuItem( 0, beID_Prefs_ENABLELOG, _(L"Protokoll anzeigen"), "", wxITEM_CHECK ) );
	prefsMenu->AppendSeparator();
	prefsMenu->Append( new wxMenuItem( 0, beID_Prefs_SELLOCALDB, _(L"Lokale Datenbank wählen...") ) );


	// Substanzen speichern
	prefsMenu->Check( beID_Prefs_SAVELOCAL, bSaveLocal );

	// Exakte Suche?
	prefsMenu->Check( beID_Prefs_EXACT, bExactMatch);

	// Protokoll anzeigen?
	prefsMenu->Check( beID_Prefs_ENABLELOG, bEnableLogging );

	menu->AppendSubMenu( prefsMenu, _(L"&Suchoptionen") );

	// Koordinaten und Ausmaße des Buttons holen
	wxSize sizeButton = m_buttonpreferences->GetSize();
	wxPoint posButton = m_buttonpreferences->GetPosition();

	// Höhe draufzählen
	posButton.y += sizeButton.GetHeight();

	// Menu anzeigen
	this->PopupMenu( menu, posButton );

	// Menu löschen
	delete menu;
}

// Wird aufgerufen, wenn eine der "Datenbanken" im Menu angeklickt wurde
void CBerylliumSubstanceSearchDialog::OnSelectSource( wxCommandEvent &event )
{
	// GESTIS markiert? Dann Hinweis anzeigen!
	if ( (event.GetId() == beID_Source_GESTIS) && event.IsChecked() )
	{
		// Hinweis anzeigen!
		wxMessageBox( _(L"Der Datenbestand der GESTIS-Stoffdatenbank darf zum Zwecke des Arbeitsschutzes bzw. zur Informationsgewinnung über die von chemischen Stoffen ausgehenden Gefährdungen genutzt werden. Eine kommerzielle Nutzung der Daten wie auch eine teilweise oder vollständige Übernahme in andere Informationssysteme ist nicht gestattet. Die Daten in der GESTIS-Stoffdatenbank werden sorgfältig erstellt und gepflegt. Dennoch kann, gleich aus welchem Rechtsgrund, keine Haftung übernommen werden.\n\nMit dem Aufnehmen der GESTIS-Stoffdatenbank in Ihre Suche stimmen Sie dieser Nutzungsvereinbarung zu."), _(L"Hinweis zur Nutzung der GESTIS-Stoffdatenbank"), wxICON_EXCLAMATION | wxOK );
	}

	// Wurde markiert? -> Hinzufügen
	if ( event.IsChecked() )
	{
		for ( unsigned int i = 0; i < m_selectedDB.size(); ++i )
		{
			// Gefunden? Dann nichts tun!
			if ( m_selectedDB[i] == event.GetId() )
				return;
		}

		// Ansonsten hinzufügen
		m_selectedDB.push_back( event.GetId() );
	}

	// ansonsten löschen
	else
	{
		for ( unsigned int i = 0; i < m_selectedDB.size(); ++i )
		{
			if ( m_selectedDB[i] == event.GetId() )
			{
				m_selectedDB.erase( m_selectedDB.begin() + i );
				break;
			}
		}
	}

	// Diese Liste speichern...
	std::vector< wxString > list;

	for ( unsigned int i = 0; i < m_selectedDB.size(); ++i )
	{
		if ( m_selectedDB[i] == beID_Source_GESTIS )
			list.push_back( "gestis" );

		if ( m_selectedDB[i] == beID_Source_MERCK )
			list.push_back( "merck" );

		/* TODO: euSDB benötigt seit einem Update Logindaten, deswegen hier erstmal entfernt.
		// ENTFERNT: 14.09.2013
		if ( m_selectedDB[i] == beID_Source_EUSDB )
			list.push_back( "eusdb" );
		*/

		if ( m_selectedDB[i] == beID_Source_WIKIDE )
			list.push_back( "wiki" );

		if ( m_selectedDB[i] == beID_Source_LOCALDB )
			list.push_back( "local" );
	}

	// In String speichern
	wxString savelist = BeTool::ListToString( list, "," );

	// Und speichern
	wxGetApp().SetConfigData( "provider", savelist );
}

// Wird aufgerufen, wenn eine der "Suchoptionen" im Menu angeklickt wurde
void CBerylliumSubstanceSearchDialog::OnSearchPreferences( wxCommandEvent &event )
{
	// Welche ID?
	switch ( event.GetId() )
	{
		// Exakter Suchstring?
		case beID_Prefs_EXACT:
			bExactMatch = event.IsChecked();
			break;

		// Speichern in die lokale Datenbank?
		case beID_Prefs_SAVELOCAL:
			bSaveLocal = event.IsChecked();
			::wxGetApp().SetConfigData( "savetolocaldatabase", (bSaveLocal ? "true" : "false") );
			break;

		// Protokoll anzeigen?
		case beID_Prefs_ENABLELOG:
			bEnableLogging = event.IsChecked();
			::wxGetApp().SetConfigData( "enablelogging", (bEnableLogging ? "true" : "false") );
			break;
	}
}

// Wird aufgerufen, wenn "lokale Datenbank" ausgewählt wurde
void CBerylliumSubstanceSearchDialog::OnSearchSelectLocalDatabase( wxCommandEvent &event )
{
	// Dateinamen holen...
	wxString sFileName =	m_localdatabasename;

	// DialogBox öffnen
	wxFileDialog fd( this, _(L"Datenbank wählen..."), "", sFileName, _(L"Datenbank (*.xml)|*.xml|Alle Dateien|*.*"), wxFD_SAVE );

	// DialogBox anzeigen
	if ( fd.ShowModal() == wxID_OK )
	{
		// Dateinameobjekt erstellen
		wxFileName dbfile(fd.GetPath());

		// Relativen Pfad erstellen
		dbfile.MakeRelativeTo(wxPathOnly(wxGetApp().argv[0]));

		// Pfad kopieren
		m_localdatabasename = dbfile.GetFullPath();

		// Ersetzen potentielle "\" durch "/", damit unter Linux (wo ein "\" durchaus ein legitimies Zeichen ist)
        // nicht die falsche Datei geladen wird. Windows akzeptiert sowohl "\" als auch "/".
        m_localdatabasename.Replace( "\\", "/", true );

		// Speichern
		::wxGetApp().SetConfigData( "localdatabase", m_localdatabasename);

		// Diese Datenbank laden
		m_localdatabase.LoadFromFile( m_localdatabasename );
	}
}
