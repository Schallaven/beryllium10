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

#include "BerylliumApplication.h"

#include <wx/xml/xml.h>
#include <wx/fs_arc.h>
#include <wx/fs_mem.h>
#include <wx/cmdline.h>
#include <wx/sstream.h>
#include <wx/protocol/http.h>

// Versionsinfos
#include "AutoBuild.h"

// Implementiert die eigentliche Einsprungsfunktion und die Hauptschleife
IMPLEMENT_APP(CBerylliumApplication)

// Einsprungspunkt in Beryllium. Erstellt das Hauptfenster.
bool CBerylliumApplication::OnInit()
{
	// Standardsprache setzen
	m_iLanguage = wxLANGUAGE_GERMAN;

	// Config-Datei laden
	LoadConfig(); 

	// Lädt und setzt die Sprache
	InitLocale();

	// Statements-Datei laden
	LoadStatements();

	// Vorlagen-Datei laden
	LoadTemplates();

	// Fügt alle verfügbaren Image-Handler ein
	wxInitAllImageHandlers();

	// Fügt den Archive-Handler ins virtuelle Dateisystem ein (Beryllium10.dat!)
	wxFileSystem::AddHandler( new wxArchiveFSHandler );

	// Fügt den Memory-Handler ins virtuelle Dateisystem ein (Zwischenspeicherung der Grafiken!)
	wxFileSystem::AddHandler( new wxMemoryFSHandler );

	// Lädt alle (oft benutzten) Grafiken in den Speicher
	LoadGraphicsToMemory();

	// Ausmaße des Bildschirms ermitteln
	int width = 0, height = 0;
	::wxDisplaySize( &width, &height );

	// Kommandozeile parsen
	wxCmdLineParser cmdline( this->argc, this->argv ); 

	// Mögliche Schalter
	cmdline.AddSwitch( "funatwork" );

	// evtl. Dateiübergeben
	cmdline.AddParam( "", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);

	// evtl. Datei
	wxString FileToOpen;

	// Easter Eggs
	bool bEasterEgg = false;
	
	// Ohne "Fehler"
	if (cmdline.Parse() == 0)
	{
		// "Easter-Egg"-Modus
		bEasterEgg = cmdline.Found( "funatwork" );

		// Datei angegeben?
		if ( cmdline.GetParamCount() > 0 )
			FileToOpen = cmdline.GetParam(0);
	}

	// Hauptfenster erstellen
	CBerylliumMainframe *beFrame = new CBerylliumMainframe( _(L"Beryllium¹º"), 100, 100, width - 200, height - 200, bEasterEgg);

	// Datei zu öffnen
	if ( FileToOpen.length() > 0 )
		beFrame->OpenFile( FileToOpen );

	// Symbol setzen
	beFrame->SetIcon( m_icon );

	// Zentrieren und Anzeigen
	beFrame->Center();
	beFrame->Show(true);

	// zum Hauptfenster machen
	SetTopWindow(beFrame);

	return true;
}

// Wird beim Verlassen aufgerufen
int CBerylliumApplication::OnExit()
{
	// Config-Daten speichern
	SaveConfig();

	// Abschließen
	return 0;
}

// Lädt alle oft genutzen Grafiken in den Zwischenspeicher
void CBerylliumApplication::LoadGraphicsToMemory()
{
	// Dateisystem-Objekt holen
	wxFileSystem *filesystem = new wxFileSystem;

	// Icon
	// ----
	wxFSFile *iconfile = filesystem->OpenFile("beryllium10.dat#zip:beryllium10.png");
	
	if ( iconfile != NULL )
	{
		// ein Image-Objekt anlegen
		wxImage img( *iconfile->GetStream(), wxBITMAP_TYPE_PNG  );

#ifdef WIN32
		// Skalieren (nur unter Windows)
		img = img.Scale( 32, 32, wxIMAGE_QUALITY_HIGH );
#endif

		// Bitmap erzeugen und kopieren
		m_icon.CopyFromBitmap( wxBitmap(img) );

		// Datei löschen
		delete iconfile;
	}

	// GHS-Symbole
	// -----------
	for ( unsigned int i = 0; i < 9; ++i )
	{
		// Dateiname erstellen
		wxString filename = "ghs" + wxString::Format("%02d",i+1) + ".png";

		// Datei-Stream laden
		wxFSFile *file = filesystem->OpenFile("beryllium10.dat#zip:" + filename);

		// Datei konnte nicht geladen werden
		if ( file == NULL )
			continue;
		
		// ein Image-Objekt anlegen
		wxImage img( *file->GetStream(), wxBITMAP_TYPE_PNG  );

		// Erste Version: skaliert (für den Eigenschaftsdialog), in Zwischenspeicher schieben
		wxMemoryFSHandler::AddFile( filename, img.Scale( 30, 30, wxIMAGE_QUALITY_HIGH) , wxBITMAP_TYPE_PNG );

		// Zweite Version: Original (fürs Zeichnen und Drucken), in Zwischenspeicher schieben
		wxMemoryFSHandler::AddFile( "org_"+filename, img, wxBITMAP_TYPE_PNG );

		// Datei löschen
		delete file;
	}

	// Symbole für den Substanz-Suchen-Dialog
	// --------------------------------------
	wxString substancefiles[4] = {	"substanceonline.jpg",
									"substanceoffline.jpg",
									"substancecustom.jpg",
									"substanceempty.jpg" };

	for ( unsigned int i = 0; i < 4; ++i )
	{
		// Datei-Stream laden
		wxFSFile *file = filesystem->OpenFile( "beryllium10.dat#zip:" + substancefiles[i] );

		// Datei konnte nicht geladen werden
		if ( file == NULL )
			continue;
		
		// ein Image-Objekt anlegen
		wxImage img( *file->GetStream(), wxBITMAP_TYPE_JPEG  );

		// In Zwischenspeicher schieben
		wxMemoryFSHandler::AddFile( substancefiles[i], img, wxBITMAP_TYPE_JPEG );

		// Datei löschen
		delete file;
	}

	// Zeiger zum virtuellen Dateisystem löschen
	delete filesystem;

}

// Lädt und setzt die Sprache
void CBerylliumApplication::InitLocale()
{
	// Feld mit den Dateinamen der entsprechenden Sprachen
	wxString	wxFileName = "beryllium_";

	// Welche Sprache ist gesetzt?
	switch( m_iLanguage )
	{
		// Englisch
		case wxLANGUAGE_ENGLISH:
			wxFileName += "en";
			break;

		// Standardsprache für Beryllium ist Deutsch
		default:
			wxFileName += "de";
			break;
	};

	// Setzt die "Locale" (falls verfügbar)
	// Die Sprache selbst wird dadurch gesetzt, dass weiter unten die entsprechende Katalogdatei gewählt wird
	if( m_Locale.IsAvailable( m_iLanguage ) )
		m_Locale.Init( m_iLanguage );
	else
		m_Locale.Init();

	// Pfad hinzufügen
	m_Locale.AddCatalogLookupPathPrefix(wxPathOnly(argv[0]) + "/config" );

	// Katalogdatei setzen
    m_Locale.AddCatalog( wxFileName );
}

// Lädt die Config-Datei und setzt die entsprechenden Optionen
void CBerylliumApplication::LoadConfig()
{
	// Config-Datei laden
	wxXmlDocument config;
	
	// Datei konnte nicht geladen werden? Raus hier!
	if ( !config.Load( wxPathOnly(argv[0]) + "/config/config.xml" ) )
		return;

	// Wurzel finden
    if (config.GetRoot()->GetName() != "beryllium")
        return;

    // Wurzel Ast für Ast durchgehen
    wxXmlNode *option = config.GetRoot()->GetChildren();

    while (option) 
	{
		// Sprache?
		if ((option->GetType() ==  wxXML_ELEMENT_NODE) && (option->GetName().compare("language") == 0))
		{
            // Sprache holen...
			wxString lang = option->GetNodeContent();

			// ...und setzen
			lang.ToLong( &m_iLanguage );
        }

		// Andere Einträge einfach in die HashMap schreiben
		else if ( option->GetType() == wxXML_ELEMENT_NODE )
		{
			m_configHashMap[ option->GetName() ] = option->GetNodeContent();			
		}

		// Nächster Ast
		option = option->GetNext();
    }
}

// Statements-Datei laden
void CBerylliumApplication::LoadStatements()
{
	// Feld mit den Dateinamen der entsprechenden Sprachen
	wxString wxSuffix;

	// Welche Sprache ist gesetzt?
	switch( m_iLanguage )
	{
		// Englisch
		case wxLANGUAGE_ENGLISH:
			wxSuffix += "_en";
			break;

		// Standardsprache für Beryllium ist Deutsch
		default:
			wxSuffix += "_de";
			break;
	};

	// Dateinamen
	wxString names[] = { "risks", "securities", "hazards", "precaution" };

	// Dateien einladen
	for ( unsigned int i = 0; i < 4; i++ )
	{
		// Datei laden
		wxXmlDocument file;
	
		// Datei konnte nicht geladen werden? Raus hier!
		if ( !file.Load( wxPathOnly(argv[0]) + "/config/" + names[i] + wxSuffix + ".xml" ) )
			continue;

		// Wurzel finden
		if (file.GetRoot()->GetName() != names[i])
			return;

		// Wurzel Ast für Ast durchgehen
		wxXmlNode *statement = file.GetRoot()->GetChildren();

		while (statement) 
		{
			// Gefunden?
			if ( statement->GetType() ==  wxXML_ELEMENT_NODE )
			{
				// Statement holen...
				wxString sContent = statement->GetNodeContent();

				// Nummer holen
				wxString sNumber = statement->GetAttribute( "number", "---" );

				// Gefährlichkeit holen
				wxString sDanger = statement->GetAttribute( "dangerpoints", "0" );
				unsigned int iDanger = atoi( sDanger.c_str());

				// Nummer vorhanden?
				if ( sNumber.compare( "---" ) != 0 )
					m_Statements.AddStatement( (names[i].Upper()[0] + sNumber).ToStdString(), sContent.ToStdString(), "", iDanger );
			}

			// Nächster Ast
			statement = statement->GetNext();
		}
	}
}

// Vorlagen-Datei laden
void CBerylliumApplication::LoadTemplates()
{
	// Feld mit den Dateinamen der entsprechenden Sprachen
	wxString wxSuffix;

	// Welche Sprache ist gesetzt?
	switch( m_iLanguage )
	{
		// Englisch
		case wxLANGUAGE_ENGLISH:
			wxSuffix += "_en";
			break;

		// Standardsprache für Beryllium ist Deutsch
		default:
			wxSuffix += "_de";
			break;
	};

	// Kategorien
	// wxString categories[] = { "danger", "behaviour", "firstaid", "waste" };

	// Datei laden
	wxXmlDocument file;

	// Datei konnte nicht geladen werden? Raus hier!
	if ( !file.Load( wxPathOnly(argv[0]) + "/config/templates" + wxSuffix + ".xml" ) )
		return;

	// Wurzel finden
	if (file.GetRoot()->GetName() != "templates")
		return;

	// Wurzel Ast für Ast durchgehen
	for ( wxXmlNode *statement = file.GetRoot()->GetChildren(); statement; statement = statement->GetNext() )
	{
		// Nur Elemente!
		if ( statement->GetType() !=  wxXML_ELEMENT_NODE )
			continue;

		// Statement holen...
		wxString sContent = statement->GetNodeContent();
		
		// Kategorie holen
		wxString sCategory = statement->GetAttribute( "category", "danger" );

		// Nummer(n) holen
		wxString sNumbers = statement->GetAttribute( "numbers" );

		// Keine Nummer? Dann ignorieren!
		if ( sNumbers.length() == 0 )
			continue;

		// Liste der Nummer(n) anlegen
		std::vector< wxString > list; BeTool::StringToList( sNumbers.ToStdString(), ",", list );

		// Hinzufügen für jedes Token.
		for ( unsigned j = 0; j < list.size(); ++j )
		{
			// R/S/H/P hinzufügen; der erste enthält diesen Buchstaben (hoffentlich) schon
			if ( j > 0 )
				list[j] = sNumbers[0] + list[j];

			// Hinzufügen
			m_Templates.AddStatementToToken( list[j], sContent.ToStdString(), sCategory.ToStdString() );			
		}
	}
}

void CBerylliumApplication::SaveConfig()
{
	// Config-Datei speichern
	wxXmlDocument config;

	// Version & Encoding setzen
	config.SetVersion("1.0");
	//config.SetFileEncoding("iso-8859-1");
	config.SetFileEncoding("UTF-8");

	// Wurzel erstellen
	wxXmlNode *xmlRoot = new wxXmlNode( wxXML_ELEMENT_NODE , "beryllium" );

	// Wurzel setzen
	config.SetRoot( xmlRoot );	
	
	// Sprache
	wxXmlNode *xmllanguage = new wxXmlNode( xmlRoot, wxXML_ELEMENT_NODE, "language" );
	wxXmlNode *xmllanguagetext = new wxXmlNode( xmllanguage, wxXML_TEXT_NODE, "language", wxString::Format("%ld",m_iLanguage) );
	
	// Andere Einstellungen (Hash-Map)
	configHashMapTyp::iterator it;

	for( it = m_configHashMap.begin(); it != m_configHashMap.end(); ++it )
	{
		// Schlüssel und Inhalt holen
		wxString key = it->first;
		wxString value = it->second;

		wxXmlNode *xmlconfigdata		= new wxXmlNode( xmlRoot, wxXML_ELEMENT_NODE, key );
		new wxXmlNode( xmlconfigdata, wxXML_TEXT_NODE, key, value );				
	}
	
	// Datei speichern (Einrückung von 2 Zeichen pro Zeile)
	config.Save( wxPathOnly(argv[0]) + "/config/config.xml", 2 );
}

// Check: Lädt die update.xml vom Server und überprüft, ob die Version noch aktuell ist
bool CBerylliumApplication::IsThereANewVersion()
{
	// Version-Infos spalten und speichern
	std::vector<wxString> list; BeTool::StringToList( STRFILEVER, ",", list );

	// Sollten 4 sein, ansonsten raus hier!
	if ( list.size() != 4 )
		return false;

	// Versionsblock anlegen
	versionblock version_old;

	// Version kopieren
	wxString(list[0]).ToLong( &version_old.major );
	wxString(list[1]).ToLong( &version_old.minor );
	wxString(list[2]).ToLong( &version_old.revision );
	wxString(list[3]).ToLong( &version_old.build );

	// Update.Xml laden
	LoadUpdateData();

	// Theoretisch würde es reichen nur die Build-Nummer zu überprüfen, da diese ja einfach
	// hochgezählt wird. Aber wer weiß, ob ich dieses System nicht mal ändere. Jedenfalls
	// überprüfen wir vorher AUCH die Hauptversionsnummer! 

	// Hauptversion größer
	if ( version_update.major > version_old.major )
		return true;

	// Hauptversion gleich?
	else if ( version_update.major == version_old.major )
	{
		// Build-Nummer größer?
		if ( version_update.build > version_old.build )
			return true;
	}

	// Keine neue Version
	return false;
}

// Ruft die Update-Seite (wie in der update.xml angegeben) im Browser auf
void CBerylliumApplication::OpenUpdatePage()
{
	// Sollte keine Url vorhanden sein, dann erstmal die Update.xml laden
	if ( version_update.sUrl.size() == 0 )
		LoadUpdateData();

	// Url aufrufen
	wxLaunchDefaultBrowser( version_update.sUrl );
}

// Lädt die Update.xml-Datei vom Server und speichert den Inhalt
void CBerylliumApplication::LoadUpdateData()
{
	// HTTP-Objekt
	wxHTTP http;

	// HTTP-Anfrage vorbereiten
	http.SetHeader( "Content-type", "text/html; charset=utf-8" );
	http.SetTimeout(30); // 30 Sekunden TimeOut

	// Anfrage an Keksecks.de vorbereiten
	http.Connect( "www.keksecks.de" );

	// InputStream erzeugen
	wxInputStream *httpStream = http.GetInputStream( "/beryllium10/update.xml" );

	// Stream ok?
	if (http.GetError() == wxPROTO_NOERR)
	{
		// Daten
		wxString data;

		// OutputStream erzeugen
		wxStringOutputStream dataStream(&data);

		// Daten lesen
		httpStream->Read(dataStream);
 
		// Daten holen und nach version_update kopieren
		LoadUpdateDataContent(data);

#ifdef _DEBUG
		wxMessageBox( wxString::Format("Version Update: %d.%d.%d.%d\n\nUrl: %s", version_update.major, version_update.minor, version_update.revision, version_update.build, version_update.sUrl), "DEBUG: Update-Download" );
#endif

	}

	// Stream löschen
	wxDELETE(httpStream);

	// HTTP-Anfrage schließen
	http.Close();

	// Zurück
	return;
}

void CBerylliumApplication::LoadUpdateDataContent( const wxString &data )
{
	// String-Stream anlegen
	wxStringInputStream stream( data );

	// XML-Dokument anlegen
	wxXmlDocument doc;

	// Daten konnten nicht geladen werden? Raus hier!
	if ( !doc.Load( stream ) )
		return;

	// Wurzel finden
    if (doc.GetRoot()->GetName() != "beryllium")
        return;

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = doc.GetRoot()->GetChildren();

	while (node) 
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächster Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Knoten kopieren
		if ( node->GetName().compare("major") == 0 )
			node->GetNodeContent().ToLong( &version_update.major );

		else if ( node->GetName().compare("minor") == 0 )
			node->GetNodeContent().ToLong( &version_update.minor );

		else if ( node->GetName().compare("revision") == 0 )
			node->GetNodeContent().ToLong( &version_update.revision );

		else if ( node->GetName().compare("build") == 0 )
			node->GetNodeContent().ToLong( &version_update.build );

		else if ( node->GetName().compare("url") == 0 )
			version_update.sUrl = node->GetNodeContent();

		// Nächster Ast
		node = node->GetNext();
	};

	// Zurück
	return;
}

// Setzt die Sprache (zu den verfügbaren)
void CBerylliumApplication::SetLanguage( long newlanguage )
{
	// Überprüfen
	switch (newlanguage)
	{
		// Folgende Sprachen werden unterstützt.
		case wxLANGUAGE_GERMAN:
		case wxLANGUAGE_ENGLISH:
			m_iLanguage = newlanguage;
			break;

		// Sonst: Nichts tun.
		default:
			break;
	};
}
