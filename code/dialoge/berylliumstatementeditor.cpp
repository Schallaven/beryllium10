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
#include "berylliumstatementeditor.h"


IMPLEMENT_CLASS(CBerylliumStatementEditor, wxPropertySheetDialog)

BEGIN_EVENT_TABLE(CBerylliumStatementEditor, wxPropertySheetDialog)
	EVT_INIT_DIALOG( CBerylliumStatementEditor::OnInitDialog )
	EVT_BUTTON( wxID_OK, CBerylliumStatementEditor::OnOK )
END_EVENT_TABLE()

CBerylliumStatementEditor::CBerylliumStatementEditor(wxWindow *parent, wxString title = _(L"Betriebsanweisung"))
{
	// Erstellt das Eigenschaftsfenster
	Create(parent, wxID_ANY, _(L"Eigenschaften von ") + title, wxDefaultPosition, wxSize( 675, 450)); //, wxDEFAULT_DIALOG_STYLE, wxRESIZE_BORDER);

	// Erstellt die Buttons
	CreateButtons( wxOK | wxCANCEL  );

	// TabControl (=Notebook) holen
	wxBookCtrlBase* notebook = GetBookCtrl();

	// Seiten hinzufügen
	notebook->AddPage( CreatePageEditor(notebook), _(L"Benutzerdefiniert"), true);
}

CBerylliumStatementEditor::~CBerylliumStatementEditor(void)
{
}

wxPanel *CBerylliumStatementEditor::CreatePageEditor(wxWindow* parent)
{
	// Ein Panel vorbereiten
	wxPanel* panel = new wxPanel(parent, wxID_ANY);

	// Hier werden die Elemente erstellt...
	wxBoxSizer* bSizer1;	bSizer1 = new wxBoxSizer( wxVERTICAL );

	// Erklärungstext anzeigen
	wxStaticText* m_staticText1;
	m_staticText1 = new wxStaticText( panel, wxID_ANY, _(L"Fügen Sie hier die Zeilen ein, die zusätzlich in das Datenblatt eingefügt werden sollen:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( 605 );
	bSizer1->Add( m_staticText1, 0, wxALL, 10 );

	// Textfeld anlegen
	m_textctrl = new wxTextCtrl( panel, wxID_ANY, "", wxDefaultPosition, wxSize( 605, 290 ), wxTE_MULTILINE|wxTE_DONTWRAP );
	bSizer1->Add( m_textctrl, 1, wxRIGHT|wxBOTTOM|wxALIGN_RIGHT, 10 );

	// Übernehmen
	panel->SetSizer( bSizer1 );
	panel->Layout();

	// Panel zurückgeben...
	return panel;
}

void CBerylliumStatementEditor::OnInitDialog( wxInitDialogEvent &event )
{
	// Fokus auf das Editorsteuerelement setzen
	m_textctrl->SetFocus();

	// Weiter im Programm
	event.Skip();
}

void CBerylliumStatementEditor::OnOK( wxCommandEvent &event )
{
	// Weiter im Programm
	event.Skip();
}

// Set: Übergibt einen Vektor mit Sätzen, die im Textfeld angezeigt werden
void CBerylliumStatementEditor::SetStatements( const std::vector< wxString > &list )
{
	// Liste in einen String umwandeln
	wxString temp = BeTool::ListToString( list, "\n" );

	// Textfeld setzen
	m_textctrl->SetValue( temp );
}

// Get: Holt einen Vektor mit Sätzen, die im Textfeld eingegeben wurden
void CBerylliumStatementEditor::GetStatements( std::vector< wxString> &list )
{
	// String holen
	wxString temp = m_textctrl->GetValue();

	// String in Liste umwandeln
	BeTool::StringToList( temp, "\n", list, true );
}
