#ifndef EVBMAINFRAME_H
#define EVBMAINFRAME_H

#include <TGClient.h>
#include <TGWindow.h>
#include <TGFrame.h>
#include <TGNumberEntry.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <TGMenu.h>
#include <TGTextViewStream.h>
#include <TGProgressBar.h>
#include <TGFileDialog.h>
#include <TTimer.h>
#include <TGComboBox.h>
#include "../evb/EVBApp.h"


class EVBMainFrame : public TGMainFrame
{
public:
	EVBMainFrame(const TGWindow* p, UInt_t w, UInt_t h);
	virtual ~EVBMainFrame();
	void CloseWindow();
	void HandleMenuSelection(int id);
	void DoOpenWorkdir();
	void DoOpenCMapfile();
	void DoOpenSMapfile();
	void DoOpenScalerfile();
	void DoRun();
	void HandleTypeSelection(int box, int entry);
	bool SetParameters();
	void DisplayWorkdir(const char* dir);
	void DisplayCMap(const char* file);
	void DisplaySMap(const char* file);
	void DisplayScaler(const char* file);
	void SaveConfig(const char* file);
	void LoadConfig(const char* file);
	void RunMerge(const char* dir, const char* file);
	void DisableAllInput();
	void EnableAllInput();
	void SetProgressBarPosition(uint64_t val, uint64_t total);


	enum WidgetId {
		WORKDIR,
		CMAP,
		SMAP,
		SCALER,
		SLOWWIND,
		BUFSIZE,
		TYPEBOX,
		RMIN,
		RMAX,
		M_LOAD_CONFIG,
		M_SAVE_CONFIG,
		M_EXIT
	};

	ClassDef(EVBMainFrame, 0);

private:
	TGTextButton *fRunButton, *fOpenWorkButton, *fOpenSMapButton, *fOpenScalerButton, *fOpenChannelButton;
	TGTextEntry *fWorkField;
	TGTextEntry * fSMapField;
	TGTextEntry * fCMapField;
	TGTextEntry *fScalerField;
	TGComboBox *fTypeBox;

	TGNumberEntryField *fSlowWindowField, *fBufferSizeField;
	TGNumberEntryField *fRMinField, *fRMaxField;

	TGHProgressBar* fProgressBar;

	TGPopupMenu *fFileMenu;

	TGFileInfo *fInfo;

	EventBuilder::EVBApp m_builder;
	EventBuilder::EVBParameters m_params;

	int counter;
	UInt_t MAIN_W, MAIN_H;


};
#endif
