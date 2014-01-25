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

#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <wx/xml/xml.h>

// Enthält alle Daten für EINE Substanz und die entsprechenden Methoden...
class CBerylliumSubstanceData
{
public:
	CBerylliumSubstanceData(void);
	~CBerylliumSubstanceData(void);

private:
	// Durchschnittsdaten zur Berechnung
	static const double AverageHumanMass;		// in kg
	static const double AverageWorkplaceVolume; // in m³

	// Struktur für die Summenformel
	struct SubstanceFormula {
		wxString sSymbol;
		wxString sUpper;
		wxString sLower;
	};

	// Vector für die Formel
	std::vector< SubstanceFormula > vFormula;

	// String-Repräsentation für die Formel
	wxString						szFormula;

	// Gefährlichkeit
	unsigned int m_iDanger;

public:
	// Struktur für LD50-Werte
	struct LethalDose50 {
		wxString sSpecies;			// Ratte, Kaninchen, Mensch, ...
		wxString sExposureType;		// Oral, Dermal, ...
		double	 dbValue;			// Wert
		wxString sUnit;				// Einheit, i.d.R. "mg/kg" oder "ml/m³" (ppm)
	};

	// Ansatztyp
	enum batchType { batchSolid = 0, batchLiquid, batchSolution, batchLAST};

private:
	// Art des Ansatzes
	batchType						typeBatch;

	// Menge des Ansatzes, Masse in g (Typ 0), Volumen in L (Typ 1 + 2)
	double							dbBatch;

	// Konzentration der Lösung (Typ 2), evtl. relative Konzentration
	double							dbConcentration;
	bool							bRelativeConcentration; // Konzentration in % (v/v)

	// LD50-Werte
	std::vector< LethalDose50 >		ld50data;

public:
	// Steht in der XML-Datei
	std::vector<wxString>			szNames;
	double							dbMolWeight;
	double							dbDensity;
	wxString						szMeltingPoint;
	wxString						szBoilingPoint;
	bool							bDecomposition;
	wxString						szFlashPoint;
	wxString						szRisks;
	wxString						szSafeties;
	wxString						szHarzardSatements;
	wxString						szEuropeanHarzardSatements;
	wxString						szPrecautionaryStatements;
	wxString						szSignalWord;
	double							dbMAK;
	bool							bMAKppm;
	unsigned int					iWGK;

	// Gefahrensymbole (alt)
	std::vector<wxString>		szHarzards;

	// GHS-Symbole
	std::vector<wxString>		szGHSSymbols;

	// Gruppe/Kategorie (für Bibliotheken)
	wxString						szGroup;

	// CAS-Nummer
	wxString						szCAS;

	// Quellen
	struct sourceinfo {
		wxString		provider;	// Hersteller oder Quellenangabe (z.B. Merck oder Wikipedia)
		wxString		url;		// URL zur Quelle
		wxString		fetched;	// Zeitstempel
		wxString		changed;	// Zeitstempel
	};

	sourceinfo						source;

	// Daten für die Rückseite
	std::vector< wxString > sCustomStatements[4];

	// Get: Gibt den ersten Namen zurück oder "Leere Substanzdaten"
	wxString GetName() const;

	// Get: Gibt einen sortierbaren Namen zurück (entfernt alle Zahlen, Klammern & co) und wandelt den String in Großbuchstaben um
	wxString GetSortableName() const;

	// Get: Gibt alle Namen in einen String zurück, mit separator getrennt
	wxString GetNames( wxString separator = "; " ) const;

	// Get: Gibt alle Namen außer dem ersten in einen String zurück, mit separator getrennt
	wxString GetNamesExceptFirst( wxString separator = "; " ) const;

	// Set: Übergibt eine Liste von Namen als einzelner String, getrennt durch separator
	void SetNames( const wxString names, const wxString separator = ";" );

	// Set: Löscht alle Namen und fügt dann "name" an
	void ReplaceNamesBy( const wxString name );

	// Get: Gibt den kürzesten Namen zurück oder "Leere Substanzdaten"
	wxString GetShortestName() const;

	// Get: Hat diese Substanz den Namen oder Namenteil?
	bool HasName( const wxString name, bool bExact = true ) const;

	// Formel vorhanden?
	bool HasFormula() const { return ( szFormula.length() > 0 ); };

	// Get: Gibt die Formel als Text zurück
	wxString GetFormula() const;

	// Set: Setzt die Formel als Text
	void SetFormula( const wxString formula );

	// Draw: Zeichnet die Summenformel
	void DrawFormula( wxDC &dc, int x, int y ) const;

	// Draw: Zeichnet den Namen
	void DrawName( wxDC &dc, int x, int y, int width, int height ) const;

	// Wandelt den String density in eine Fließkommazahl um und speichert ihn als Dichte
	void SetDensity( const wxString density );

	// Wandelt den String molweight in eine Fließkommazahl um und speichert ihn als Molgewicht
	void SetMolWeight( const wxString molweight );

	// Wandelt den String mak in eine Fließkommazahl um und speichert ihn als MAK-Wert
	void SetMAK( const wxString mak );

	// Gibt die verwendeten Gefahrensymbole als einen String zurück
	wxString GetHarzards( wxString separator = ", " ) const;

	// Setzt die Gefahrensymbole
	void SetHarzards( wxString harzards, wxString separator = ", " );

	// Prüft auf ein bestimmtes Gefahrensymbol
	bool HasHarzard( wxString harzard ) const;

	// Gibt die verwendeten GHS-Symbole als einen String zurück
	wxString GetGHSSymbols( wxString separator = ", " ) const;

	// Setzt die GHS-Symbole
	void SetGHSSymbols( wxString symbols, wxString separator = ", " );

	// Prüft auf ein bestimmtes GHS-Symbole
	bool HasGHSSymbol( wxString symbol ) const;

	// Get: Typ des Ansatzes
	batchType GetBatchType() const { return typeBatch; };

	// Set: Typ des Ansatzes
	void SetBatchType( batchType type ) { typeBatch = type; };

	// Get: Menge des Ansatzes, in der jeweiligen Einheit
	double GetBatchAmount() const { return dbBatch; };

	// Set: Menge des Ansatzes, in der jeweiligen Einheit
	void SetBatchAmount( double amount ) { dbBatch = amount; };

	// Get: Konzentration der Lösung (falls vorhanden)
	double GetBatchConcentration();

	// Set: Konzentration der Lösung (falls vorhanden)
	void SetBatchConcentration( double conc ) { dbConcentration = conc; };

	// Set: relative Konzentration
	void SetBatchConcentrationRelative( bool rel ) { bRelativeConcentration = rel; };

	// Get: relative Konzentration
	bool IsBatchConcentrationRelative() { return bRelativeConcentration; };

	// Berechnet die Stoffmenge und gibt sie in "mol" zurück
	double CalculateAmountOfSubstance() const;

	// Berechnet die Gefahr als "Beryllium"-Punkte
	unsigned int GetDangerPoints() const { return m_iDanger; };

	// Berechnet die Gefahr als "Beryllium"-Punkte
	void CalculateDangerPoints();

	// LD50-Werte sortiert einfügen
	void SetLD50Data( const std::vector< LethalDose50 > &data );

	// LD50-Werte holen
	void GetLD50Data( std::vector< LethalDose50 > &data ) const;

	// den kleinsten Toxi-Wert suchen (MAK/LD50) und formatiert zurückgeben
	wxString GetSmallestToxicologicAmount() const;

	// den kleinsten Toxi-Wert suchen (MAK/LD50) und die entsprechende Stoffmenge zurückgeben
	double GetSmallestToxicologicAmountDouble() const;

	// Multipliziert den Ansatz mit dem entsprechenden Faktor
	void MultiplyBatchBy( double factor ) { dbBatch = (dbBatch * factor); };

	// Speichert alle Namen unter den angegebenen Knoten
	void SaveNamesToXmlNode( wxXmlNode *parent );

	// Speichert alle Daten in einen Xml-Knoten (außer Ansatz)
	void SaveToXmlNode( wxXmlNode *parent );

	// Speichert die Ansatzdaten in einem Xml-Knoten
	void SaveBatchToXmlNode( wxXmlNode *parent );

	// Speichert die LD50-Werte in einem Xml-Knoten
	void SaveLD50ToXmlnode( wxXmlNode *parent );

	// Lädt alle Daten aus einem Xml-Knoten (inkl. Ansatz!)
	void LoadFromXmlNode( wxXmlNode *parent );

private:
	// Hilfsfunktion (inline): fügt node an parent an mit dem Textinhalt content
	inline wxXmlNode* AppendToXmlNode( wxXmlNode *parent, wxString node, wxString content );

	// Hilfsfunktion: Überprüft, ob eine Temperaturangabe eine Einheit (K oder °C) hat.
	// Wenn nicht, ist es höchstwahrscheinlich eine Kelvinangabe aus der alten Berylliumversion.
	// Dies wird auch hier gleich mitgeändert.
	void CheckAndUpdateTemperature( wxString &data );

	// Hilfsfunktion: Schätzt die Masse von toxikologischen Daten und gibt sie zurück
	double CalculateAmountOfSubstanceForLethalDose( const LethalDose50 &data ) const;

	// Hilfsfunktion: "Beryllium-Punkte" für H/P-Sätze
	unsigned int CalculateDangerPointsStatements();

	// Hilfsfunktion: "Beryllium-Punkte" für GHS-Symbole/Signalwort
	unsigned int CalculateDangerPointsGHSSymbols();

	// Hilfsfunktion: "Beryllium-Punkte" für Toxikologie
	unsigned int CalculateDangerPointsToxicology();
};

