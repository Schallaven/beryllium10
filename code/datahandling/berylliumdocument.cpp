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

#include "../beryllium.h"

#include "berylliumdocument.h"


// Konstruktor
CBerylliumDocument::CBerylliumDocument(void)
{
	// Daten wurden noch nicht geändert
	m_bChanged = false;

	// Dateiname
	m_strFilename = _(L"Unbenannt");

	// Pfad
	m_strPath = "";

	// Kein GHS-Modus
	bGHS = false;
}

// Destruktor
CBerylliumDocument::~CBerylliumDocument(void)
{
}

// Fügt einen neuen Beobachter hinzu
void CBerylliumDocument::AddObserver(IObserver *object)
{
	// Null-Objekt?
	if ( object == NULL )
		return;

	// Gibt es den Beobachter schon?
	for ( unsigned int i = 0; i < m_Observers.size(); i++ )
		if ( m_Observers[i] == object )
			return;

	// Hinzufügen
	m_Observers.push_back( object );
}

// Entfernt einen Beobachter
void CBerylliumDocument::RemoveObserver( IObserver *object )
{
	// Observer suchen und entfernen, wenn gefunden...
	for ( unsigned int i = 0; i < m_Observers.size(); i++ )
		if ( m_Observers[i] == object )
			m_Observers.erase( m_Observers.begin() + i );
}

// Benachrichtigt alle Beobachter
void CBerylliumDocument::NotifyAllObserver()
{
	// Berechnet die Gefährlichkeit der einzelnen Substanzen
	for ( unsigned int i = 0; i < m_data.sdCompounds.size(); ++i)
		m_data.sdCompounds[i].CalculateDangerPoints();


	// Liste durch gehen und jeden benachrichtigen
	for ( unsigned int i = 0; i < m_Observers.size(); i++ )
		m_Observers[i]->ModelHasChanged();
}

// Set: Dateinamen setzen
void CBerylliumDocument::SetFilename( const wxString strNewName )
{
	// Dateinamen setzen
	m_strFilename = strNewName;

	// Daten wurden geändert
	m_bChanged = true;

	// Alle benachrichtigen
	NotifyAllObserver();
}

// Set: Daten setzen
void CBerylliumDocument::SetData( const CBerylliumData &newData )
{
	// Daten kopieren
	m_data = newData;

	// Daten wurden geändert
	m_bChanged = true;

	// Alle benachrichtigen
	NotifyAllObserver();
}

// Fügt eine Substanz in das Datenobjekt ein und gibt den Index zurück
unsigned int CBerylliumDocument::AddSubstance( const CBerylliumSubstanceData &substance )
{
	// Daten einfügen
	m_data.sdCompounds.push_back( substance );

	// Daten wurden geändert
	m_bChanged = true;

	// Alle benachrichtigen
	NotifyAllObserver();

	// Index zurückgeben
	return (m_data.sdCompounds.size()-1);
}

// Daten aus Datei laden (in szFileName sollte der ganze Pfad enthalten sein)
int CBerylliumDocument::LoadFromFile( wxString szFilename )
{
	// Datei laden
	wxXmlDocument file;

	// Datei konnte nicht geladen werden? Raus hier!
	if ( !file.Load( szFilename ) )
		return -1;

	// Wurzel finden
    if (file.GetRoot()->GetName() != "beryllium")
        return -2;

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = file.GetRoot()->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();

		// Allgemeines
		if ( name == "general" )
			LoadGeneralFromXmlNode( node );

		// Persönliche Daten
		else if ( name == "personal" )
			LoadPersonalFromXmlNode( node );

		// Produkt/Tätigkeit/Präparat
		else if ( name == "product" )
			LoadProductFromXmlNode( node );

		// Templates
		else if ( name == "templates" )
			LoadTemplatesFromXmlNode( node );

		// Optionen
		else if ( name == "options" )
			LoadOptionsFromXmlNode( node );

		// Substanzen
		else if ( name == "substance" )
		{
			// Substanz anlegen
			CBerylliumSubstanceData data;

			// Laden
			data.LoadFromXmlNode( node );

			// Hinzufügen
			AddSubstance( data );
		}


		// Nächster Ast
		node = node->GetNext();
	};

	// neue Datei ist noch unverändert
	m_bChanged = false;

	// kein Fehler
	return 0;
}

// Daten in Datei speichern
int CBerylliumDocument::SaveToFile()
{
	// Nur wenn ein Pfad gesetzt wurde.
	if ( m_strPath.length() == 0 )
		return -1;

	// Datei speichern
	wxXmlDocument file;

	// Version setzen
	file.SetVersion("1.0");

	// Encoding setzen, 1.12.2013: UTF-8
	//file.SetFileEncoding("iso-8859-1");
	file.SetFileEncoding("UTF-8");

	// Wurzel erstellen
	wxXmlNode *xmlRoot = new wxXmlNode( wxXML_ELEMENT_NODE , "beryllium" );

	// Wurzel setzen
	file.SetRoot( xmlRoot );

	// Elemente für die allgemeinen Dinge erstellen ("general")
	wxXmlNode *xmlGeneral = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "general" );
	xmlRoot->InsertChild( xmlGeneral, NULL );

	// Dieser Bock fügt alle Elemente zu "general" hinzu
	{
		// Titel
		wxXmlNode *xmlGeneralTitle = new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "title" );
		new wxXmlNode( xmlGeneralTitle, wxXML_TEXT_NODE, "title", m_data.szTitle );

		// GHS-Titel
		wxXmlNode *xmlGeneralTitleGHS = new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "title_ghs" );
		new wxXmlNode( xmlGeneralTitleGHS, wxXML_TEXT_NODE, "title_ghs", m_data.szGHSTitle );

		// Ort
		wxXmlNode *xmlGeneralLocation = new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "location" );
		new wxXmlNode( xmlGeneralLocation, wxXML_TEXT_NODE, "location", m_data.szLocation );

		// Veranstaltung (ehemalig "Institut" oder szChemistry)
		wxXmlNode *xmlGeneralInstitute = new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "institute" );
		new wxXmlNode( xmlGeneralInstitute, wxXML_TEXT_NODE, "institute", m_data.szEvent );

		// Signaturdaten: Student-Statement
		wxXmlNode *xmlStatementStudent		= new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "statementleft" );
		new wxXmlNode( xmlStatementStudent, wxXML_TEXT_NODE, "statementleft", m_data.szStatementStudent );

		// Signaturdaten: Student-Signatur
		wxXmlNode *xmlSignaturStudent		= new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "signaturleft" );
		new wxXmlNode( xmlSignaturStudent, wxXML_TEXT_NODE, "signaturleft", m_data.szSignaturStudent );

		// Signaturdaten: Assistent-Statement
		wxXmlNode *xmlStatementSupervisor	= new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "statementright" );
		new wxXmlNode( xmlStatementSupervisor, wxXML_TEXT_NODE, "statementright", m_data.szStatementSupervisor );

		// Signaturdaten: Assistent-Signatur
		wxXmlNode *xmlSignaturSupervisor	= new wxXmlNode( xmlGeneral, wxXML_ELEMENT_NODE, "signaturright" );
		new wxXmlNode( xmlSignaturSupervisor, wxXML_TEXT_NODE, "signaturright", m_data.szSignaturSupervisor );
	};

	// Elemente für die persönlichen Dinge erstellen ("personal")
	wxXmlNode *xmlPersonal = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "personal" );
	xmlRoot->InsertChildAfter( xmlPersonal, xmlGeneral );

	// Dieser Bock fügt alle Elemente zu "personal" hinzu
	{
		// Name des Praktikanten
		wxXmlNode *xmlPersonalName = new wxXmlNode( xmlPersonal, wxXML_ELEMENT_NODE, "name" );
		new wxXmlNode( xmlPersonalName, wxXML_TEXT_NODE, "name", m_data.GetName() );

		// Platznummer ("spot") des Praktikanten
		wxXmlNode *xmlPersonalSpot = new wxXmlNode( xmlPersonal, wxXML_ELEMENT_NODE, "spot" );
		new wxXmlNode( xmlPersonalSpot, wxXML_TEXT_NODE, "spot", m_data.szPosition );

		// Name des Assistenten
		wxXmlNode *xmlPersonalSupervisor = new wxXmlNode( xmlPersonal, wxXML_ELEMENT_NODE, "assistant" );
		new wxXmlNode( xmlPersonalSupervisor, wxXML_TEXT_NODE, "assistant", m_data.szSupervisor );
	};

	// Elemente für das Präparat/Produkt/Tätigkeit erstellen ("product")
	wxXmlNode *xmlProduct = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "product" );
	xmlRoot->InsertChildAfter( xmlProduct, xmlPersonal );

	// Dieser Bock fügt alle Elemente zu "product" hinzu
	{
		// Alle Namen des Produktes hinzufügen
		m_data.sdPreparation.SaveNamesToXmlNode( xmlProduct );

		// Formel hinzufügen
		wxXmlNode *xmlProductFormula = new wxXmlNode( xmlProduct, wxXML_ELEMENT_NODE, "chemical-formula" );
		new wxXmlNode( xmlProductFormula, wxXML_TEXT_NODE, "chemical-formula", m_data.sdPreparation.GetFormula() );
	};

	// Elemente für die allgemeinen Dinge erstellen ("general")
	wxXmlNode *xmlOptions = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "options" );
	xmlRoot->InsertChild( xmlOptions, NULL );

	// Dieser Bock fügt alle Elemente zu "options" hinzu
	{
		// GHS-Modus
		wxXmlNode *xmlGHS = new wxXmlNode( xmlOptions, wxXML_ELEMENT_NODE, "ghs-mode" );
		new wxXmlNode( xmlGHS, wxXML_TEXT_NODE, "ghs-mode", ( bGHS ? "1" : "0" ) );

		// Spalten
		for ( unsigned int i = 0; i < m_showcolumns.size(); ++i )
		{
			wxXmlNode *xmlColumn = new wxXmlNode( xmlOptions, wxXML_ELEMENT_NODE, "showcolumn" );
			new wxXmlNode( xmlColumn, wxXML_TEXT_NODE, "showcolumn", m_showcolumns[i] );
		}

		// Sätze verstecken
		for ( unsigned int i = 0; i < m_hidephrases.size(); ++i )
		{
			wxXmlNode *xmlPhrase = new wxXmlNode( xmlOptions, wxXML_ELEMENT_NODE, "hidephrase" );
			new wxXmlNode( xmlPhrase, wxXML_TEXT_NODE, "hidephrase", m_hidephrases[i] );
		}
	}

	// Letzter eingefügter Knoten
	wxXmlNode *lastNode = xmlOptions;

	// Nun für jede Substanz einen Block hinzufügen
	for ( unsigned int i = 0; i < m_data.sdCompounds.size(); ++i )
	{
		// Knoten erstellen
		wxXmlNode *xmlSubstance = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "substance" );
		xmlRoot->InsertChildAfter( xmlSubstance, lastNode );

		// Daten anhängen
		m_data.sdCompounds[i].SaveToXmlNode( xmlSubstance );

		// Auch die Ansatzgrößen
		m_data.sdCompounds[i].SaveBatchToXmlNode( xmlSubstance );

		// Letzten Knoten setzen
		lastNode = xmlSubstance;
	}

	// Benutzerdefinierte Statements speichern
	wxXmlNode *xmlTemplates = new wxXmlNode( NULL, wxXML_ELEMENT_NODE, "templates" );
	xmlRoot->InsertChildAfter( xmlTemplates, lastNode );

	// Kategorien festlegen
	const char *categories[] = { "danger", "security", "behavior", "dumping" };

	// Kategorien durchgehen
	for ( unsigned int cat = 0; cat < 4; ++cat )
	{
		// Sätze durchgehen
		for ( unsigned int i = 0; i < m_data.sCustomStatements[cat].size(); ++i )
		{
			// Knoten anlegen
			wxXmlNode *xmlTemplatesChild = new wxXmlNode( xmlTemplates, wxXML_ELEMENT_NODE, "template" );

			// Attribut hinzufügen
			xmlTemplatesChild->AddAttribute( "category", categories[cat] );

			// Inhalt hinzufügen
			new wxXmlNode( xmlTemplatesChild, wxXML_TEXT_NODE, "template", m_data.sCustomStatements[cat][i] );
		}
	}

	// Keine Templates eingefügt? Dann wieder löschen
	if (xmlTemplates->GetChildren() == NULL)
		xmlRoot->RemoveChild( xmlTemplates );

	// Datei speichern (Einrückung von 2 Zeichen pro Zeile)
	file.Save( m_strPath, 2 );

	// Gespeichert!
	m_bChanged = false;

	return 0;
}

// Hilfsfunktion: Lädt "General" Abschnitt aus einem XML-Knoten
void CBerylliumDocument::LoadGeneralFromXmlNode( wxXmlNode *parent )
{
	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();
		wxString content = node->GetNodeContent();

		// Titel
		if ( name == "title" )
			m_data.szTitle = content;

		// GHS Titel
		else if ( name == "title_ghs" )
			m_data.szGHSTitle = content;

		// Ort
		else if ( name == "location" )
			m_data.szLocation = content;

		// Veranstaltung/Event (ehemalig szChemistry)
		else if ( name == "institute" )
			m_data.szEvent = content;

		// Statement: Student
		else if ( name == "statementleft" )
			m_data.szStatementStudent = content;

		// Unterschrift: Student
		else if ( name == "signaturleft" )
			m_data.szSignaturStudent = content;

		// Statement: Assistent
		else if ( name == "statementright" )
			m_data.szStatementSupervisor = content;

		// Unterschrift: Assistent
		else if ( name == "signaturright" )
			m_data.szSignaturSupervisor = content;


		// Nächster Ast
		node = node->GetNext();
	};
}

// Hilfsfunktion: Lädt "Personal" Abschnitt aus einem XML-Knoten
void CBerylliumDocument::LoadPersonalFromXmlNode( wxXmlNode *parent )
{
	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	// String für den Vor- und Nachnamen reservieren
	wxString firstname = "";
	wxString lastname = "";

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();
		wxString content = node->GetNodeContent();

		// Name des Praktikanten
		if ( name == "name" )
			lastname = content;

		// Vorname (veraltet)
		else if ( name == "firstname" )
			firstname = content;

		// Platz
		else if ( name == "spot" )
			m_data.szPosition = content;

		// Name des Assistenten
		else if ( name == "assistant" )
			m_data.szSupervisor = content;

		// Nächster Ast
		node = node->GetNext();
	};

	// Namen zusammensetzen
	wxString name = lastname + " " + firstname;
	m_data.SetName( name );
}

// Hilfsfunktion: Lädt "Product" Abschnitt aus einem XML-Knoten
void CBerylliumDocument::LoadProductFromXmlNode( wxXmlNode *parent )
{
	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();
		wxString content = node->GetNodeContent();

		// Name des Substanz
		if ( name == "name" )
			m_data.sdPreparation.SetNames( content );

		// Formel der Substanz
		else if ( name == "chemical-formula" )
			m_data.sdPreparation.SetFormula( content );

		// Nächster Ast
		node = node->GetNext();
	};
}

// Hilfsfunktion: Lädt "Templates" Abschnitt aus einem XML-Knoten
void CBerylliumDocument::LoadTemplatesFromXmlNode( wxXmlNode *parent )
{
	// Kategorien festlegen
	const char *categories[] = { "danger", "security", "behavior", "dumping" };

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	while (node)
	{
		// Nur Elemente mit Namen "template" sammeln
		if ( (node->GetType() != wxXML_ELEMENT_NODE) || (node->GetName() != "template") )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Inhalt holen
		wxString content = node->GetNodeContent();

		// Kategorie holen
		wxString category = node->GetAttribute( "category", "danger" );

		// Index der Kategorie holen
		unsigned int iCat = 0;

		for ( unsigned int i = 0; i < 4; ++i )
		{
			if ( category.compare( categories[i] ) == 0 )
			{
				iCat = i;
				break;
			}
		}

		// Template hinzufügen
		m_data.sCustomStatements[iCat].push_back( content );

		// Nächster Ast
		node = node->GetNext();
	};
}

// Hilfsfunktion: Lädt "Options" Abschnitt aus einem XML-Knoten
void CBerylliumDocument::LoadOptionsFromXmlNode( wxXmlNode *parent )
{
	// Erste Spalte?
	m_showcolumns.clear();
	m_hidephrases.clear();

	// Wurzel Ast für Ast durchgehen
    wxXmlNode *node = parent->GetChildren();

	while (node)
	{
		// Nur Elemente sammeln
		if ( node->GetType() != wxXML_ELEMENT_NODE )
		{
			// Nächste Ast
			node = node->GetNext();

			// Fortsetzen
			continue;
		}

		// Element überprüfen
		const wxString name = node->GetName();
		wxString content = node->GetNodeContent();

		// GHS-Modus
		if ( name == "ghs-mode" )
			bGHS = ( content == "1" );

		// Spalte zeigen?
		else if ( name == "showcolumn" )
			m_showcolumns.push_back( content );

		// Satz verstecken?
		else if ( name == "hidephrase" )
			m_hidephrases.push_back( content );

		// Nächster Ast
		node = node->GetNext();
	};
}

// Multipliziert alle Substanzen mit dem entsprechenden Faktor
void CBerylliumDocument::MultiplyBatchesBy( double factor )
{
	// An die Daten weitergeben
	m_data.MultiplyBatchesBy( factor );

	// Dokument wurde geändert!
	m_bChanged = true;

	// Alle Beobachter benachrichtigen
	NotifyAllObserver();
}
