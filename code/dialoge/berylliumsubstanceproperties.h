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

#include <wx/propdlg.h>
#include <wx/statline.h>
#include <wx/listctrl.h>
#include <wx/hyperlink.h>

#include "../datahandling/berylliumsubstancedata.h"

// Seiten
#define bePAGE_SUBSTANCE_GENERAL	0
#define bePAGE_SUBSTANCE_PHYSICAL	1
#define bePAGE_SUBSTANCE_SAFETY		2
#define bePAGE_SUBSTANCE_GHS		3
#define bePAGE_SUBSTANCE_TOXICOLOGY	4
#define bePAGE_SUBSTANCE_BATCH		5
#define bePAGE_SUBSTANCE_ADVICES	6

// IDs für die Steuerelemente
#define beID_DECOMPOSITION	7001
#define beID_MAKPPM			7002
#define beID_BATCHMASS		7003
#define beID_BATCHVOLUME	7004
#define beID_BATCHSOLUTION	7005

#define beID_BATCHTEXTMASS	7006
#define beID_BATCHTEXTVOL	7007
#define beID_BATCHTEXTCONC	7008
#define beID_BATCHTEXTSVOL	7009
#define beID_BATCHUNITMASS	7010
#define beID_BATCHUNITVOL	7011
#define beID_BATCHUNITCONC	7012
#define beID_BATCHUNITSVOL	7013

// IDs für die Buttons unter "Hinweise"
#define beID_CUSTOMDANGERS		8001
#define beID_CUSTOMPRECAUTIONS	8002
#define beID_CUSTOMFIRSTAID		8003
#define beID_CUSTOMDISPOSAL		8004

// IDs für die Buttons und die Liste unter "Toxikologie"
#define beID_TOXY_LD50			8200
//#define beID_TOXY_ADD			8201
//#define beID_TOXY_DEL			8202

// ID für das HTML-Fenster unter "Quelle"
#define beID_SOURCELINK			9001

// Dialogklasse
class CBerylliumSubstanceProperties : public wxPropertySheetDialog
{
DECLARE_CLASS(CBerylliumSubstanceProperties)

public:
	CBerylliumSubstanceProperties(wxWindow* parent, CBerylliumSubstanceData newdata, int page );
	~CBerylliumSubstanceProperties(void);

	// Get: Substanzdaten holen
	CBerylliumSubstanceData GetData() { return data; };

private:
	// Substanzdaten
	CBerylliumSubstanceData data;

	// Vektor mit LD50-Werten
	std::vector< CBerylliumSubstanceData::LethalDose50 > m_ld50values;

	// Elemente
	wxTextCtrl *m_textNames;
	wxTextCtrl *m_textFormula;
	wxTextCtrl *m_textMolWeight;
	wxTextCtrl *m_textCAS;

	wxTextCtrl		*m_textMeltingPoint;
	wxCheckBox		*m_checkDecomposition;
	wxStaticText	*m_staticBoilingPoint;
	wxTextCtrl		*m_textBoilingPoint;
	wxTextCtrl		*m_textFlashPoint;
	wxTextCtrl		*m_textDensity;

	wxTextCtrl *m_textHarzards;			// Gefahrensymbole
	wxTextCtrl *m_textRisks;			// R-Sätze
	wxTextCtrl *m_textSafeties;			// S-Sätze

	wxCheckBox *m_GHSSymbols[9];		// 9 GHS-Symbole zum ankreuzen.
	wxTextCtrl *m_textGHSSymbols;		// GHS-Symbole
	wxTextCtrl *m_textGHSSignalWord;	// GHS-Signalwort
	wxTextCtrl *m_textGHSHarzards;		// H-Sätze
	wxTextCtrl *m_textGHSEuHarzards;	// EuH-Sätze
	wxTextCtrl *m_textGHSPrecautionary; // P-Sätze

	wxChoice		*m_WaterHarzardClass;	// Wassergefährdungsklasse
	wxStaticText	*m_MAKText;				// MAK-Text (Einheit)
	wxTextCtrl		*m_textMAK;				// MAK-Wert
	wxCheckBox		*m_checkMAKppm;			// MAK in ppm?
	wxListCtrl		*m_listLD50;			// LD 50 - Werte

	wxRadioButton	*m_radioBatchMass;		// Ansatz in Gramm (Masse)?
	wxRadioButton	*m_radioBatchVolume;	// Ansatz in Liter (Volumen)?
	wxRadioButton	*m_radioBatchSolution;	// Ansatz als Lösung

	wxTextCtrl		*m_textMass;					// Wieviel Gramm?
	wxChoice		*m_choiceMassUnit;				// Masseneinheit? (mg, ...)
	wxTextCtrl		*m_textVolume;					// Wieviel Volumen?
	wxChoice		*m_choiceVolumeUnit;			// Volumeneinheit? (mL, L, ...)
	wxStaticText	*m_staticSolutionConcentration;	// Text: Konzentration
	wxTextCtrl		*m_textConcentration;			// Konzentration?
	wxChoice		*m_choiceConcentrationUnit;		// Konzentrationseinheit? (mol/L, mmol/L, ... )
	wxStaticText	*m_staticSolutionVolume;		// Text: Volumen der Lösung
	wxTextCtrl		*m_textSolutionVolume;			// Lösungsvolumen?
	wxChoice		*m_choiceSolutionVolumeUnit;	// Einheit des Volumenes der Lösung ( mL, L, ... )
	wxStaticText	*m_textAmountOfSubstance;		// Stoffmenge der Substanz

	// Initialisiert die Seite "Allgemein"
	wxPanel *CreatePageGeneral(wxWindow* parent);

	// Initialisiert die Seite "Physikalische Eigenschaften"
	wxPanel *CreatePagePhysical(wxWindow* parent);

	// Initialisiert die Seite "Sicherheit"
	wxPanel *CreatePageSafety(wxWindow* parent);

	// Initialisiert die Seite "GHS"
	wxPanel *CreatePageGHS(wxWindow* parent);

	// Initialisiert die Seite "Toxikologie"
	wxPanel *CreatePageToxicology(wxWindow* parent);

	// Initialisiert die Seite "Ansatz"
	wxPanel *CreatePageBatch(wxWindow* parent);

	// Initialisiert die Seite "Hinweise"
	wxPanel *CreatePageAdvices(wxWindow* parent);

	// Initialisiert die Seite "Quelle"
	wxPanel *CreatePageSource(wxWindow* parent);

	// Füllt die Liste der LD50-Werte
	void FillLethalDoseList();

	// Wird aufgerufen, wenn der Dialog angezeigt werden soll
	void OnInitDialog( wxInitDialogEvent &event );

	// Wird aufgerufen, wenn der Dialog beendet wird
	void OnOK( wxCommandEvent &event );

	// Wird aufgerufen, wenn auf "unter Zersetzung" geklickt wird
	void OnDecomposition( wxCommandEvent &event );

	// Wird aufgerufen, wenn auf "ppm" (MAK-Wert) geklickt wird
	void OnMAKppm( wxCommandEvent &event );

	// Wird aufgerufen, wenn auf einen der Radiobuttons auf der "Ansatz"-Seite geklickt wurde
	void OnBatch( wxCommandEvent &event );

	// Wird aufgerufen, wenn auf einen der Buttons auf der "Hinweis"-Seite geklickt wurde
	void OnStatements( wxCommandEvent &event );

	// Führt ein Update der "Ansatz"-Seite durch, wenn ein Radiobutton angeklickt wurde
	void UpdateBatchPage();

	// Berechnet die "Ansatz"-Seite neu, wenn sich etwas geändert hat
	void CalculateBatchPage();

	// Wird aufgerufen, wenn sich eines der Textfelder oder der Einheiten auf der Ansatzseite ändert
	void OnBatchTextChange( wxCommandEvent &event );

	// Doppelklick/Enter auf Eintrag in LD50-Liste
	void OnLD50Activate( wxListEvent &event );

	// Taste in LD50-Liste gedrückt
	void OnLD50KeyDown( wxListEvent &event );


DECLARE_EVENT_TABLE()
};
