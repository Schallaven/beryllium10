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
#include "BerylliumInfoDialog.h"

#include "../BerylliumApplication.h"

// Versionsinfos
#include "../AutoBuild.h"

// Constructor
CBerylliumInfoHTML::CBerylliumInfoHTML(wxWindow *parent, wxWindowID id, const wxPoint& pos,
	const wxSize& size, long style, const wxString& name)
: wxHtmlWindow(parent, id, pos, size, style, name)
{
}

// Was passiert wenn der Benutzer einen Link anklickt?
void CBerylliumInfoHTML::OnLinkClicked( const wxHtmlLinkInfo &link )
{
	// Wir rufen die Url mit dem Standardbrowser auf. So einfach ist das.
	wxLaunchDefaultBrowser( link.GetHref() );
}

IMPLEMENT_CLASS(CBerylliumInfoDialog, wxDialog)

BEGIN_EVENT_TABLE(CBerylliumInfoDialog, wxDialog)
	EVT_BUTTON( wxID_OK, CBerylliumInfoDialog::OnOK )
END_EVENT_TABLE()

CBerylliumInfoDialog::CBerylliumInfoDialog( wxWindow *parent )
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _("Beryllium¹º"), wxDefaultPosition, wxSize( 600, 520));

	// Dialogelemente erstellen
	CreateLayout();

	// Version-Infos spalten und speichern
	std::vector<wxString> list; BeTool::StringToList( STRFILEVER, ",", list );

	// Sollten 4 sein
	if ( list.size() == 4 )
	{
		wxString(list[0]).ToLong( &version.major );
		wxString(list[1]).ToLong( &version.minor );
		wxString(list[2]).ToLong( &version.revision );
		wxString(list[3]).ToLong( &version.build );
	}

	// HTML-Infoseite erstellen
	CreateInfoPage();
}

CBerylliumInfoDialog::~CBerylliumInfoDialog(void)
{
}

// Erstellt alle Steuerelemente des Dialoges
void CBerylliumInfoDialog::CreateLayout()
{
	// Sizer erstellen und festlegen
	// =============================
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* SizerMain;
	SizerMain = new wxBoxSizer( wxVERTICAL );

	// Hauptbereich - Logo
	// ===================
	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Datei-Stream laden
	wxFSFile *file = filesystem->OpenFile( "beryllium10.dat#zip:splash.png" );

	// Datei konnte geladen werden
	if ( file != NULL )
	{
		// ein Image-Objekt anlegen
		wxImage img( *file->GetStream(), wxBITMAP_TYPE_PNG  );

		// Bild nicht ok?
		if ( !img.IsOk() )
			return;

		// daraus ein Bitmap machen
		wxBitmap bmp( img );

		// Datei löschen
		delete file;

		wxStaticBitmap* bitmapLogo = new wxStaticBitmap( this, wxID_ANY, bmp, wxDefaultPosition, wxDefaultSize, 0 );
		SizerMain->Add( bitmapLogo, 0, wxEXPAND | wxALL, 0 );
	}

	// Hauptbereich - HTML-Fenster
	infobox = new CBerylliumInfoHTML( this, wxID_ANY, wxDefaultPosition, wxSize( 600, 100 ), wxHW_SCROLLBAR_AUTO );
	infobox->SetBackgroundColour( wxColour(255,255,255) );
	SizerMain->Add( infobox, 1, wxEXPAND | wxALL, 0 );

	// Buttonleiste (unten)
	// ====================
	wxStaticLine* staticLine = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	SizerMain->Add( staticLine, 0, wxEXPAND | wxALL, 0 );

	wxBoxSizer* SizerButtons = new wxBoxSizer( wxHORIZONTAL );

	SizerButtons->SetMinSize( wxSize( -1,25 ) );
	wxButton *buttonOK = new wxButton( this, wxID_OK, _(L"Schließen"), wxDefaultPosition, wxDefaultSize, 0 );
	buttonOK->SetDefault();
	SizerButtons->Add( buttonOK, 0, wxALL, 5 );

	SizerMain->Add( SizerButtons, 0, wxFIXED_MINSIZE|wxALIGN_CENTER|wxALL, 5 );

	// Fertigstellen
	// =============
	this->SetSizer( SizerMain );
	this->Layout();

	this->Centre( wxBOTH );
}

void CBerylliumInfoDialog::OnOK( wxCommandEvent &event )
{
	// Dialog beenden: OK
	EndDialog( wxID_OK );

	// Rausspringen
	return;
}

// Erstellt die HTML-Infoseite in infobox
void CBerylliumInfoDialog::CreateInfoPage()
{
	// Text Stück für Stück zusammensetzen
	wxString text;

	// 1. Beryllium, Version, wxWidgets, Version
	text += wxString::Format( _(L"<a href=\"http://beryllium.keksecks.de/\">Beryllium¹º</a> %ld.%ld (Build %ld) ist freie Software und steht unter <a href=\"http://www.gnu.org/licenses/gpl.html\">GNU&nbsp;GPL&nbsp;v3</a>.  Es wurde mit <a href=\"http://www.wxwidgets.org/\">wxWidget %d.%d.%d</a> kompiliert.<br><br>"), version.major, version.minor, version.build,  wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER  );

	// 2. Autor(en)
	text += _(L"Autor: Dr. Sven Kochmann<br><br>");

	// 3. Debug-Hinweis (natürlich nur in der Debug-Version)
#ifdef _DEBUG
	text += _(L"<b>Dies ist eine Debug-Version von Beryllium. Diese Version sollte nur in Testumgebungen eingesetzt werden.</b><br><br>");
#endif

	// 4. Danksagungen
	text += _(L"!!!Danksagungen in der Infobox!!!");

	// HTML-Tags hinzufügen
	wxString html = "<html><body>"
		"<p><font size=\"-1\">" + text + ""
		"</font></p>"
		"</body></html>";

	// Seite setzen
	infobox->SetPage( html );	
}
