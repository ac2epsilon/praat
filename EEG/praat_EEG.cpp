/* praat_EEG.cpp
 *
 * Copyright (C) 2011-2012,2013,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "EEGWindow.h"
#include "ERPWindow.h"
#include "ERPTier.h"
#include "SpectrumEditor.h"

#include "praat_TimeTier.h"

// MARK: - EEG

// MARK: Help

DIRECT (HELP_EEG_help) {
	HELP (U"EEG")
}

// MARK: View & Edit

static void cb_EEGWindow_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectralSlice = Thing_isa (publication.get(), classSpectrum) && str32equ (Thing_getName (publication.get()), U"slice");
		praat_new (publication.move());
		praat_updateSelection ();
		if (isaSpectralSlice) {
			int IOBJECT;
			FIND_ONE_WITH_IOBJECT (Spectrum)
			autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
			praat_installEditor (editor2.get(), IOBJECT);
			editor2.releaseToUser();
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_EEG_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an EEG from batch.");
	FIND_ONE_WITH_IOBJECT (EEG)
		autoEEGWindow editor = EEGWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_EEGWindow_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Query

FORM (STRING_EEG_getChannelName, U"Get channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	OK
DO
	STRING_ONE (EEG)
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		const char32 *result = my channelNames [channelNumber];
	STRING_ONE_END
}

FORM (INTEGER_EEG_getChannelNumber, U"Get channel number", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	NUMBER_ONE (EEG)
		long result = EEG_getChannelNumber (me, channelName);
	NUMBER_ONE_END (U"")
}

// MARK: Modify

DIRECT (MODIFY_EEG_detrend) {
	MODIFY_EACH (EEG)
		EEG_detrend (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_editExternalElectrodeNames, U"Edit external electrode names", nullptr) {
	WORD4 (externalElectrode1, U"External electrode 1", U"EXG1")
	WORD4 (externalElectrode2, U"External electrode 2", U"EXG2")
	WORD4 (externalElectrode3, U"External electrode 3", U"EXG3")
	WORD4 (externalElectrode4, U"External electrode 4", U"EXG4")
	WORD4 (externalElectrode5, U"External electrode 5", U"EXG5")
	WORD4 (externalElectrode6, U"External electrode 6", U"EXG6")
	WORD4 (externalElectrode7, U"External electrode 7", U"EXG7")
	WORD4 (externalElectrode8, U"External electrode 8", U"EXG8")
OK
	FIND_ONE (EEG)
		if (EEG_getNumberOfExternalElectrodes (me) == 8) {
			const long offsetExternalElectrode = EEG_getNumberOfCapElectrodes (me);
			SET_STRING (U"External electrode 1", my channelNames [offsetExternalElectrode + 1])
			SET_STRING (U"External electrode 2", my channelNames [offsetExternalElectrode + 2])
			SET_STRING (U"External electrode 3", my channelNames [offsetExternalElectrode + 3])
			SET_STRING (U"External electrode 4", my channelNames [offsetExternalElectrode + 4])
			SET_STRING (U"External electrode 5", my channelNames [offsetExternalElectrode + 5])
			SET_STRING (U"External electrode 6", my channelNames [offsetExternalElectrode + 6])
			SET_STRING (U"External electrode 7", my channelNames [offsetExternalElectrode + 7])
			SET_STRING (U"External electrode 8", my channelNames [offsetExternalElectrode + 8])
		}
DO
	MODIFY_EACH (EEG)
		if (EEG_getNumberOfExternalElectrodes (me) != 8)
			Melder_throw (U"You can do this only if there are 8 external electrodes.");
		EEG_setExternalElectrodeNames (me, externalElectrode1, externalElectrode2, externalElectrode3,
			externalElectrode4, externalElectrode5, externalElectrode6,
			externalElectrode7, externalElectrode8);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_removeTriggers, U"Remove triggers", nullptr) {
	OPTIONMENU_ENUM4 (removeEveryTriggerThat___, U"Remove every trigger that...", kMelder_string, DEFAULT)
	SENTENCE4 (___theText, U"...the text", U"hi")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_removeTriggers (me, removeEveryTriggerThat___, ___theText);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_setChannelName, U"Set channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	WORDVAR (newName, U"New name", U"BLA")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_setChannelName (me, channelNumber, newName);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_setChannelToZero, U"Set channel to zero", nullptr) {
	SENTENCE (U"Channel", U"Iz")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_setChannelToZero (me, GET_STRING (U"Channel"));
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_subtractMeanChannel, U"Subtract mean channel", nullptr) {
	LABEL (U"label", U"Range of reference channels:")
	NATURALVAR (fromChannel, U"From channel", U"1")
	NATURALVAR (toChannel, U"To channel", U"32")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_subtractMeanChannel (me, fromChannel, toChannel);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_subtractReference, U"Subtract reference", nullptr) {
	WORDVAR (referenceChannel1, U"Reference channel 1", U"MASL")
	WORDVAR (referenceChannel2, U"Reference channel 2 (optional)", U"MASR")
	OK
DO
	MODIFY_EACH (EEG)
		EEG_subtractReference (me, referenceChannel1, referenceChannel2);
	MODIFY_EACH_END
}

FORM (MODIFY_EEG_filter, U"Filter", nullptr) {
	REALVAR (lowFrequency, U"Low frequency (Hz)", U"1.0")
	REALVAR (lowWidth, U"Low width (Hz)", U"0.5")
	REALVAR (highFrequency, U"High frequency (Hz)", U"25.0")
	REALVAR (highWidth, U"High width (Hz)", U"12.5")
	BOOLEANVAR (notchAt50Hz, U"Notch at 50 Hz", true)
	OK
DO
	MODIFY_EACH (EEG)
		EEG_filter (me, lowFrequency, lowWidth, highFrequency, highWidth, notchAt50Hz);
	MODIFY_EACH_END
}

// MARK: Extract

FORM (NEW_EEG_extractChannel, U"EEG: Extract channel", nullptr) {
	SENTENCEVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	CONVERT_EACH (EEG)
		autoEEG result = EEG_extractChannel (me, channelName);
	CONVERT_EACH_END (my name, U"_", channelName)
}

FORM (NEW_EEG_extractPart, U"EEG: Extract part", nullptr) {
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"1.0")
	BOOLEANVAR (preserveTimes, U"Preserve times", false)
	OK
DO
	CONVERT_EACH (EEG)
		autoEEG result = EEG_extractPart (me, fromTime, toTime, preserveTimes);
	CONVERT_EACH_END (my name, U"_part")
}

DIRECT (NEW_EEG_extractSound) {
	CONVERT_EACH (EEG)
		if (! my sound) Melder_throw (me, U": I don't contain a waveform.");
		autoSound result = EEG_extractSound (me);
	CONVERT_EACH_END (my name)
}

DIRECT (NEW_EEG_extractTextGrid) {
	CONVERT_EACH (EEG)
		if (! my textgrid) Melder_throw (me, U": I don't contain marks.");
		autoTextGrid result = EEG_extractTextGrid (me);
	CONVERT_EACH_END (my name)
}

FORM (NEW_EEG_to_ERPTier_bit, U"To ERPTier (bit)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	NATURALVAR (markerBit, U"Marker bit", U"8")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_bit (me, fromTime, toTime, markerBit);
	CONVERT_EACH_END (my name, U"_bit", markerBit)
}

FORM (NEW_EEG_to_ERPTier_marker, U"To ERPTier (marker)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	NATURALVAR (markerNumber, U"Marker number", U"12")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_marker (me, fromTime, toTime, (uint16) markerNumber);
	CONVERT_EACH_END (my name, U"_", markerNumber)
}

FORM (NEW_EEG_to_ERPTier_triggers, U"To ERPTier (triggers)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	OPTIONMENU_ENUMVAR (getEveryEventWithATriggerThat, U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (theText, U"...the text", U"1")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_triggers (me, fromTime, toTime, getEveryEventWithATriggerThat, theText);
	CONVERT_EACH_END (my name, U"_trigger", theText)
}

FORM (NEW_EEG_to_ERPTier_triggers_preceded, U"To ERPTier (triggers, preceded)", nullptr) {
	REALVAR (fromTime, U"From time (s)", U"-0.11")
	REALVAR (toTime, U"To time (s)", U"0.39")
	OPTIONMENU_ENUMVAR (getEveryEventWithATriggerThat, U"Get every event with a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (text1, U"...the text", U"1")
	OPTIONMENU_ENUMVAR (andIsPrecededByATriggerThat, U"and is preceded by a trigger that", kMelder_string, DEFAULT)
	SENTENCEVAR (text2, U" ...the text", U"4")
	OK
DO
	CONVERT_EACH (EEG)
		autoERPTier result = EEG_to_ERPTier_triggers_preceded (me, fromTime, toTime,
			getEveryEventWithATriggerThat, text1, andIsPrecededByATriggerThat, text2);
	CONVERT_EACH_END (my name, U"_trigger", text2)
}

// MARK: Convert

DIRECT (NEW1_EEGs_concatenate) {
	CONVERT_LIST (EEG)
		autoEEG result = EEGs_concatenate (& list);
	CONVERT_LIST_END (U"chain")
}

FORM (NEW_EEG_to_MixingMatrix, U"To MixingMatrix", nullptr) {
	NATURAL4 (maximumNumberOfIterations, U"Maximum number of iterations", U"100")
	POSITIVE4 (tolerance, U"Tolerance", U"0.001")
	OPTIONMENU4x (diagonalizationMethod, U"Diagonalization method", 2, 1)
		OPTION (U"qdiag")
		OPTION (U"ffdiag")
	OK
DO
	CONVERT_EACH (EEG)
		autoMixingMatrix result = EEG_to_MixingMatrix (me,
			maximumNumberOfIterations, tolerance, diagonalizationMethod);
	CONVERT_EACH_END (my name)
}

// MARK: - EEG & TextGrid

DIRECT (MODIFY_EEG_TextGrid_replaceTextGrid) {
	MODIFY_FIRST_OF_TWO (EEG, TextGrid)
		EEG_replaceTextGrid (me, you);
	MODIFY_FIRST_OF_TWO_END
}

// MARK: - ERP

// MARK: View & Edit

static void cb_ERPWindow_publication (Editor /* editor */, autoDaata publication) {
	/*
	 * Keep the gate for error handling.
	 */
	try {
		bool isaSpectralSlice = Thing_isa (publication.get(), classSpectrum) && str32equ (Thing_getName (publication.get()), U"slice");
		praat_new (publication.move());
		praat_updateSelection ();
		if (isaSpectralSlice) {
			int IOBJECT;
			FIND_ONE_WITH_IOBJECT (Spectrum)
			autoSpectrumEditor editor2 = SpectrumEditor_create (ID_AND_FULL_NAME, me);
			praat_installEditor (editor2.get(), IOBJECT);
			editor2.releaseToUser();
		}
	} catch (MelderError) {
		Melder_flushError ();
	}
}
DIRECT (WINDOW_ERP_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot view or edit an ERP from batch.");
	FIND_ONE_WITH_IOBJECT (ERP)
		autoERPWindow editor = ERPWindow_create (ID_AND_FULL_NAME, me);
		Editor_setPublicationCallback (editor.get(), cb_ERPWindow_publication);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Tabulate

FORM (NEW_ERP_downto_Table, U"ERP: Down to Table", nullptr) {
	BOOLEAN4 (includeSampleNumber, U"Include sample number", false)
	BOOLEAN4 (includeTime, U"Include time", true)
	NATURAL4 (timeDecimals, U"Time decimals", U"6")
	NATURAL4 (voltageDecimals, U"Voltage decimals", U"12")
	RADIO4x (voltageUnits, U"Voltage units", 1, 1)
		OPTION (U"volt")
		OPTION (U"microvolt")
	OK
DO
	CONVERT_EACH (ERP)
		autoTable result = ERP_tabulate (me, includeSampleNumber,
			includeTime, timeDecimals, voltageDecimals, voltageUnits);
	CONVERT_EACH_END (my name)
}

// MARK: Draw

FORM (GRAPHICS_ERP_draw, U"ERP: Draw", nullptr) {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range", U"0.0 (= all)")
	REAL4 (fromVoltage, U"left Voltage range (V)", U"10e-6")
	REAL4 (toVoltage, U"right Voltage range", U"-10e-6")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ERP)
		ERP_drawChannel_name (me, GRAPHICS, channelName, fromTime, toTime, fromVoltage, toVoltage, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_ERP_drawScalp, U"ERP: Draw scalp", nullptr) {
	REAL4 (fromTime, U"left Time range (s)", U"0.1")
	REAL4 (toTime, U"right Time range", U"0.2")
	REAL4 (fromVoltage, U"left Voltage range (V)", U"10e-6")
	REAL4 (toVoltage, U"right Voltage range", U"-10e-6")
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ERP)
		ERP_drawScalp (me, GRAPHICS, fromTime, toTime,
			fromVoltage, toVoltage, kGraphics_colourScale_GREY, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_ERP_drawScalp_colour, U"ERP: Draw scalp (colour)", nullptr) {
	REAL4 (fromTime, U"left Time range (s)", U"0.1")
	REAL4 (toTime, U"right Time range", U"0.2")
	REAL4 (fromVoltage, U"left Voltage range (V)", U"10e-6")
	REAL4 (toVoltage, U"right Voltage range", U"-10e-6")
	RADIO_ENUM4 (colourScale, U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	BOOLEAN4 (garnish, U"Garnish", true)
	OK
DO
	GRAPHICS_EACH (ERP)
		ERP_drawScalp (me, GRAPHICS, fromTime, toTime,
			fromVoltage, toVoltage, (kGraphics_colourScale) colourScale, garnish);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_ERP_drawScalp_garnish, U"ERP: Draw scalp (garnish)", nullptr) {
	REAL4 (fromVoltage, U"left Voltage range (V)", U"10e-6")
	REAL4 (toVoltage, U"right Voltage range", U"-10e-6")
	RADIO_ENUM4 (colourScale, U"Colour scale", kGraphics_colourScale, BLUE_TO_RED)
	OK
DO
	GRAPHICS_NONE
		ERP_drawScalp_garnish (GRAPHICS, fromVoltage, toVoltage, (kGraphics_colourScale) colourScale);
	GRAPHICS_NONE_END
}

// MARK: Query

FORM (STRING_ERP_getChannelName, U"Get channel name", nullptr) {
	NATURAL4 (channelNumber, U"Channel number", U"1")
	OK
DO
	STRING_ONE (ERP)
		if (channelNumber > my ny)
			Melder_throw (me, U": there are only ", my ny, U" channels.");
		const char32 *result = my channelNames [channelNumber];
	STRING_ONE_END
}

FORM (INTEGER_ERP_getChannelNumber, U"Get channel number", nullptr) {
	WORD4 (channelName, U"Channel name", U"Cz")
	OK
DO
	NUMBER_ONE (ERP)
		long result = ERP_getChannelNumber (me, channelName);
	NUMBER_ONE_END (U" (number of channel ", channelName, U")")
}

FORM (REAL_ERP_getMinimum, U"ERP: Get minimum", U"Sound: Get minimum...") {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO4x (interpolation, U"Interpolation", 4, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	NUMBER_ONE (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double result;
		Vector_getMinimumAndX (me, fromTime, toTime, channelNumber, interpolation, & result, nullptr);
	NUMBER_ONE_END (U" Volt")
}

FORM (REAL_ERP_getTimeOfMinimum, U"ERP: Get time of minimum", U"Sound: Get time of minimum...") {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO4x (interpolation, U"Interpolation", 4, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	NUMBER_ONE (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double result;
		Vector_getMinimumAndX (me, fromTime, toTime, channelNumber, interpolation, nullptr, & result);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_ERP_getMaximum, U"ERP: Get maximum", U"Sound: Get maximum...") {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO4x (interpolation, U"Interpolation", 4, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	NUMBER_ONE (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double result;
		Vector_getMaximumAndX (me, GET_REAL (U"left Time range"), GET_REAL (U"right Time range"), channelNumber, GET_INTEGER (U"Interpolation") - 1, & result, nullptr);
	NUMBER_ONE_END (U" Volt")
}

FORM (REAL_ERP_getTimeOfMaximum, U"ERP: Get time of maximum", U"Sound: Get time of maximum...") {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"0.0 (= all)")
	RADIO4x (interpolation, U"Interpolation", 4, 0)
		RADIOBUTTON (U"None")
		RADIOBUTTON (U"Parabolic")
		RADIOBUTTON (U"Cubic")
		RADIOBUTTON (U"Sinc70")
		RADIOBUTTON (U"Sinc700")
	OK
DO
	NUMBER_ONE (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double result;
		Vector_getMaximumAndX (me, fromTime, toTime, channelNumber, interpolation, nullptr, & result);
	NUMBER_ONE_END (U" seconds")
}

FORM (REAL_ERP_getMean, U"ERP: Get mean", U"ERP: Get mean...") {
	SENTENCE4 (channelName, U"Channel name", U"Cz")
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		double result = Vector_getMean (me, fromTime, toTime, channelNumber);
	NUMBER_ONE_END (U" Volt")
}

// MARK: Modify

FORM (MODIFY_ERP_formula, U"ERP: Formula", U"ERP: Formula...") {
	LABEL (U"label1", U"! `x' is the time in seconds, `col' is the sample number.")
	LABEL (U"label2", U"x = x1   ! time associated with first sample")
	LABEL (U"label3", U"for col from 1 to ncol")
	LABEL (U"label4", U"   self [col] = ...")
	TEXTFIELD4 (formula, U"formula", U"self")
	LABEL (U"label5", U"   x = x + dx")
	LABEL (U"label6", U"endfor")
	OK
DO
	MODIFY_EACH_WEAK (ERP)
		Matrix_formula (me, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_ERP_formula_part, U"ERP: Formula (part)", U"ERP: Formula...") {
	REAL4 (fromTime, U"From time", U"0.0")
	REAL4 (toTime, U"To time", U"0.0 (= all)")
	NATURAL4 (fromChannel, U"From channel", U"1")
	NATURAL4 (toChannel, U"To channel", U"2")
	TEXTFIELD4 (formula, U"formula", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (ERP)
		Matrix_formula_part (me, fromTime, toTime,
			fromChannel - 0.5, toChannel + 0.5, formula, interpreter, nullptr);
	MODIFY_EACH_WEAK_END
}

// MARK: Extract

FORM (NEW_ERP_extractOneChannelAsSound, U"ERP: Extract one channel as Sound", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	CONVERT_EACH (ERP)
		long channelNumber = ERP_getChannelNumber (me, channelName);
		if (channelNumber == 0) Melder_throw (me, U": no channel named \"", channelName, U"\".");
		autoSound result = Sound_extractChannel (me, channelNumber);
	CONVERT_EACH_END (my name, U"_", channelName)
}

// MARK: Convert

DIRECT (NEW_ERP_downto_Sound) {
	CONVERT_EACH (ERP)
		autoSound result = ERP_downto_Sound (me);
	CONVERT_EACH_END (my name)
}

// MARK: - ERPTIER

// MARK: Help

DIRECT (HELP_ERPTier_help) {
	HELP (U"ERPTier")
}

// MARK: Query

FORM (STRING_ERPTier_getChannelName, U"Get channel name", nullptr) {
	NATURALVAR (channelNumber, U"Channel number", U"1")
	OK
DO
	STRING_ONE (ERPTier)
		if (channelNumber > my numberOfChannels)
			Melder_throw (me, U": there are only ", my numberOfChannels, U" channels.");
		const char32 *result = my channelNames [channelNumber];
	STRING_ONE_END
}

FORM (INTEGER_ERPTier_getChannelNumber, U"Get channel number", nullptr) {
	WORDVAR (channelName, U"Channel name", U"Cz")
	OK
DO
	NUMBER_ONE (ERPTier)
		long result = ERPTier_getChannelNumber (me, channelName);
	NUMBER_ONE_END (U" (number of channel ", channelName, U")")
}

FORM (REAL_ERPTier_getMean, U"ERPTier: Get mean", U"ERPTier: Get mean...") {
	NATURALVAR (pointNumber, U"Point number", U"1")
	SENTENCEVAR (channelName, U"Channel name", U"Cz")
	REALVAR (fromTime, U"left Time range (s)", U"0.0")
	REALVAR (toTime, U"right Time range (s)", U"0.0 (= all)")
	OK
DO
	NUMBER_ONE (ERPTier)
		double result = ERPTier_getMean (me, pointNumber, channelName, fromTime, toTime);
	NUMBER_ONE_END (U" Volt")
}

// MARK: Modify

FORM (MODIFY_ERPTier_rejectArtefacts, U"Reject artefacts", nullptr) {
	POSITIVEVAR (threshold, U"Threshold (V)", U"75e-6")
	OK
DO
	MODIFY_EACH (ERPTier)
		ERPTier_rejectArtefacts (me, threshold);
	MODIFY_EACH_END
}

FORM (MODIFY_ERPTier_removeEventsBetween, U"Remove events", U"ERPTier: Remove events between...") {
	REAL4 (fromTime, U"left Time range (s)", U"0.0")
	REAL4 (toTime, U"right Time range (s)", U"1.0")
	OK
DO
	MODIFY_EACH (ERPTier)
		AnyTier_removePointsBetween (me->asAnyTier(), fromTime, toTime);
	MODIFY_EACH_END
}

FORM (MODIFY_ERPTier_subtractBaseline, U"Subtract baseline", nullptr) {
	REAL4 (baselineStartTime, U"Baseline start time (s)", U"-0.11")
	REAL4 (baselineEndTime, U"Baseline end time (s)", U"0.0")
	OK
DO
	MODIFY_EACH (ERPTier)
		ERPTier_subtractBaseline (me, baselineStartTime, baselineEndTime);
	MODIFY_EACH_END
}

// MARK: Analyse

FORM (NEW_ERPTier_to_ERP, U"ERPTier: To ERP", nullptr) {
	NATURAL4 (eventNumber, U"Event number", U"1")
	OK
DO
	CONVERT_EACH (ERPTier)
		autoERP result = ERPTier_extractERP (me, eventNumber);
	CONVERT_EACH_END (my name, U"_", eventNumber)
}

DIRECT (NEW_ERPTier_to_ERP_mean) {
	CONVERT_EACH (ERPTier)
		autoERP result = ERPTier_to_ERP_mean (me);
	CONVERT_EACH_END (my name, U"_mean")
}

// MARK: - ERPTIER & TABLE

FORM (NEW1_ERPTier_Table_extractEventsWhereColumn_number, U"Extract events where column (number)", nullptr) {
	WORD4 (extractAllEventsWhereColumn___, U"Extract all events where column...", U"")
	RADIO_ENUM4 (___is___, U"...is...", kMelder_number, DEFAULT)
	REAL4 (___theNumber, U"...the number", U"0.0")
	OK
DO
	CONVERT_TWO (ERPTier, Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (you, extractAllEventsWhereColumn___);
		autoERPTier result = ERPTier_extractEventsWhereColumn_number (me, you, columnNumber, ___is___, ___theNumber);
	CONVERT_TWO_END (my name)
}

FORM (NEW1_ERPTier_Table_extractEventsWhereColumn_text, U"Extract events where column (text)", nullptr) {
	WORD4 (extractAllEventsWhereColumn___, U"Extract all events where column...", U"")
	OPTIONMENU_ENUM4 (___, U"...", kMelder_string, DEFAULT)
	SENTENCE4 (___theText, U"...the text", U"hi")
	OK
DO
	CONVERT_TWO (ERPTier, Table)
		long columnNumber = Table_getColumnIndexFromColumnLabel (you, extractAllEventsWhereColumn___);
		autoERPTier result = ERPTier_extractEventsWhereColumn_string (me, you, columnNumber, ___, ___theText);
	CONVERT_TWO_END (my name)
}

// MARK: - file recognizers

static autoDaata bdfFileRecognizer (int nread, const char * /* header */, MelderFile file) {
	const char32 *fileName = MelderFile_name (file);
	bool isBdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".bdf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".BDF");
	bool isEdfFile = Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".edf") ||
	                 Melder_stringMatchesCriterion (fileName, kMelder_string_ENDS_WITH, U".EDF");
	if (nread < 512 || (! isBdfFile && ! isEdfFile)) return autoDaata ();
	return EEG_readFromBdfFile (file);
}

// MARK: - buttons

void praat_EEG_init ();
void praat_EEG_init () {

	Thing_recognizeClassesByName (classEEG, classERPTier, classERP, nullptr);

	Data_recognizeFileType (bdfFileRecognizer);

	praat_addAction1 (classEEG, 0, U"EEG help", nullptr, 0, HELP_EEG_help);
	praat_addAction1 (classEEG, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_EEG_viewAndEdit);
	praat_addAction1 (classEEG, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 1, U"Get channel name...", nullptr, 1, STRING_EEG_getChannelName);
		praat_addAction1 (classEEG, 1, U"Get channel number...", nullptr, 1, INTEGER_EEG_getChannelNumber);
	praat_addAction1 (classEEG, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Set channel name...", nullptr, 1, MODIFY_EEG_setChannelName);
		praat_addAction1 (classEEG, 1, U"Edit external electrode names...", nullptr, 1, MODIFY_EEG_editExternalElectrodeNames);
		praat_addAction1 (classEEG, 0, U"-- processing --", nullptr, 1, nullptr);
		praat_addAction1 (classEEG, 0, U"Subtract reference...", nullptr, 1, MODIFY_EEG_subtractReference);
		praat_addAction1 (classEEG, 0, U"Subtract mean channel...", nullptr, 1, MODIFY_EEG_subtractMeanChannel);
		praat_addAction1 (classEEG, 0, U"Detrend", nullptr, 1, MODIFY_EEG_detrend);
		praat_addAction1 (classEEG, 0, U"Filter...", nullptr, 1, MODIFY_EEG_filter);
		praat_addAction1 (classEEG, 0, U"Remove triggers...", nullptr, 1, MODIFY_EEG_removeTriggers);
		praat_addAction1 (classEEG, 0, U"Set channel to zero...", nullptr, 1, MODIFY_EEG_setChannelToZero);
	praat_addAction1 (classEEG, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract channel...", nullptr, 0, NEW_EEG_extractChannel);
		praat_addAction1 (classEEG, 0, U"Extract part...", nullptr, 0, NEW_EEG_extractPart);
		praat_addAction1 (classEEG, 0, U"To ERPTier -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"To ERPTier (bit)...", nullptr, 1, NEW_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To ERPTier (marker)...", nullptr, 1, NEW_EEG_to_ERPTier_marker);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers)...", nullptr, 1, NEW_EEG_to_ERPTier_triggers);
		praat_addAction1 (classEEG, 0, U"To ERPTier (triggers, preceded)...", nullptr, 1, NEW_EEG_to_ERPTier_triggers_preceded);
		praat_addAction1 (classEEG, 0, U"To ERPTier...", nullptr, praat_DEPTH_1 + praat_HIDDEN, NEW_EEG_to_ERPTier_bit);
		praat_addAction1 (classEEG, 0, U"To MixingMatrix...", nullptr, 0, NEW_EEG_to_MixingMatrix);
	praat_addAction1 (classEEG, 0, U"Synthesize", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Concatenate", nullptr, 0, NEW1_EEGs_concatenate);
	praat_addAction1 (classEEG, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classEEG, 0, U"Extract waveforms as Sound", nullptr, 1, NEW_EEG_extractSound);
		praat_addAction1 (classEEG, 0, U"Extract marks as TextGrid", nullptr, 1, NEW_EEG_extractTextGrid);

	praat_addAction1 (classERP, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_ERP_viewAndEdit);
	praat_addAction1 (classERP, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Draw...", nullptr, 1, GRAPHICS_ERP_draw);
		praat_addAction1 (classERP, 0, U"Draw scalp...", nullptr, 1, GRAPHICS_ERP_drawScalp);
		praat_addAction1 (classERP, 0, U"Draw scalp (colour)...", nullptr, 1, GRAPHICS_ERP_drawScalp_colour);
		praat_addAction1 (classERP, 0, U"Draw scalp (garnish)...", nullptr, 1, GRAPHICS_ERP_drawScalp_garnish);
	praat_addAction1 (classERP, 0, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Table...", nullptr, 1, NEW_ERP_downto_Table);
	praat_addAction1 (classERP, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Get channel name...", nullptr, 1, STRING_ERP_getChannelName);
		praat_addAction1 (classERP, 0, U"Get channel number...", nullptr, 1, INTEGER_ERP_getChannelNumber);
		praat_addAction1 (classERP, 0, U"-- get shape --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get minimum...", nullptr, 1, REAL_ERP_getMinimum);
		praat_addAction1 (classERP, 0, U"Get time of minimum...", nullptr, 1, REAL_ERP_getTimeOfMinimum);
		praat_addAction1 (classERP, 0, U"Get maximum...", nullptr, 1, REAL_ERP_getMaximum);
		praat_addAction1 (classERP, 0, U"Get time of maximum...", nullptr, 1, REAL_ERP_getTimeOfMaximum);
		praat_addAction1 (classERP, 0, U"-- get statistics --", nullptr, 1, nullptr);
		praat_addAction1 (classERP, 0, U"Get mean...", nullptr, 1, REAL_ERP_getMean);
	praat_addAction1 (classERP, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Formula...", nullptr, 1, MODIFY_ERP_formula);
		praat_addAction1 (classERP, 0, U"Formula (part)...", nullptr, 1, MODIFY_ERP_formula_part);
	// praat_addAction1 (classERP, 0, U"Analyse -", nullptr, 0, nullptr);
		// praat_addAction1 (classERP, 0, U"To ERP (difference)", nullptr, 1, NEW_ERP_to_ERP_difference);
		// praat_addAction1 (classERP, 0, U"To ERP (mean)", nullptr, 1, NEW_ERP_to_ERP_mean);
	praat_addAction1 (classERP, 0, U"Hack -", nullptr, 0, nullptr);
		praat_addAction1 (classERP, 0, U"Down to Sound", nullptr, 1, NEW_ERP_downto_Sound);
		praat_addAction1 (classERP, 0, U"Extract one channel as Sound...", nullptr, 1, NEW_ERP_extractOneChannelAsSound);

	praat_addAction1 (classERPTier, 0, U"ERPTier help", nullptr, 0, HELP_ERPTier_help);
	// praat_addAction1 (classERPTier, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_ERPTier_viewAndEdit);
	praat_addAction1 (classERPTier, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeTier_query_init (classERPTier);
		praat_addAction1 (classERPTier, 0, U"-- channel names --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get channel name...", nullptr, 1, STRING_ERPTier_getChannelName);
		praat_addAction1 (classERPTier, 0, U"Get channel number...", nullptr, 1, INTEGER_ERPTier_getChannelNumber);
		praat_addAction1 (classERPTier, 0, U"-- erp --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Get mean...", nullptr, 1, REAL_ERPTier_getMean);
	praat_addAction1 (classERPTier, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Subtract baseline...", nullptr, 1, MODIFY_ERPTier_subtractBaseline);
		praat_addAction1 (classERPTier, 0, U"Reject artefacts...", nullptr, 1, MODIFY_ERPTier_rejectArtefacts);
		praat_addAction1 (classERPTier, 0, U"-- structure --", nullptr, 1, nullptr);
		praat_addAction1 (classERPTier, 0, U"Remove events between...", nullptr, 1, MODIFY_ERPTier_removeEventsBetween);
	praat_addAction1 (classERPTier, 0, U"Analyse", nullptr, 0, nullptr);
		praat_addAction1 (classERPTier, 0, U"Extract ERP...", nullptr, 0, NEW_ERPTier_to_ERP);
		praat_addAction1 (classERPTier, 0, U"To ERP (mean)", nullptr, 0, NEW_ERPTier_to_ERP_mean);

	praat_addAction2 (classEEG, 1, classTextGrid, 1, U"Replace TextGrid", nullptr, 0, MODIFY_EEG_TextGrid_replaceTextGrid);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract -", nullptr, 0, nullptr);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (number)...", nullptr, 1, NEW1_ERPTier_Table_extractEventsWhereColumn_number);
	praat_addAction2 (classERPTier, 1, classTable, 1, U"Extract events where column (text)...", nullptr, 1, NEW1_ERPTier_Table_extractEventsWhereColumn_text);

	structEEGWindow :: f_preferences ();
	structERPWindow :: f_preferences ();
}

/* End of file praat_EEG.cpp */
