#include "EVBMainFrame.h"
#include "FileViewFrame.h"
#include <TGLabel.h>
#include <TGTextBuffer.h>
#include <TApplication.h>
#include <TSystem.h>

EVBMainFrame::EVBMainFrame(const TGWindow* p, UInt_t w, UInt_t h) : 
	TGMainFrame(p, w, h, kVerticalFrame)
{
	SetCleanup(kDeepCleanup);
	MAIN_W = w; MAIN_H = h;

	fInfo = new TGFileInfo();

	//Organization hints
	TGLayoutHints *fchints = new TGLayoutHints(kLHintsExpandX|kLHintsExpandY,5,5,5,5);
	TGLayoutHints *fhints = new TGLayoutHints(kLHintsExpandX|kLHintsCenterY,5,5,5,5);
	TGLayoutHints *lhints = new TGLayoutHints(kLHintsCenterY|kLHintsLeft,5,5,5,5);
	TGLayoutHints *bhints = new TGLayoutHints(kLHintsLeft|kLHintsCenterY,5,5,5,5);
	TGLayoutHints *fpbhints = new TGLayoutHints(kLHintsExpandX|kLHintsBottom,5,5,5,5);
	TGLayoutHints *mhints = new TGLayoutHints(kLHintsTop|kLHintsLeft,0,4,0,0);

	//Make the containers and link up all signals/slots

	TGVerticalFrame *InputFrame = new TGVerticalFrame(this, w, h*0.9);

	TGVerticalFrame *NameFrame = new TGVerticalFrame(InputFrame, w, h*0.4);

	TGHorizontalFrame *WorkFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* workLabel = new TGLabel(WorkFrame, "Workspace Directory:");
	fWorkField = new TGTextEntry(WorkFrame, new TGTextBuffer(120), WORKDIR);
	fWorkField->Resize(w*0.25, fWorkField->GetDefaultHeight());
	fWorkField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateWorkdir()");
	fOpenWorkButton = new TGTextButton(WorkFrame, "Open");
	fOpenWorkButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenWorkdir()");
	WorkFrame->AddFrame(workLabel, lhints);
	WorkFrame->AddFrame(fWorkField, fhints);
	WorkFrame->AddFrame(fOpenWorkButton, bhints);

	TGHorizontalFrame *SMapFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* smaplabel = new TGLabel(SMapFrame, "Board Shift File:");
	fSMapField = new TGTextEntry(SMapFrame, new TGTextBuffer(120), SMAP);
	fSMapField->Resize(w*0.25, fSMapField->GetDefaultHeight());
	fSMapField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateSMap()");
	fOpenSMapButton = new TGTextButton(SMapFrame, "Open");
	fOpenSMapButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenSMapfile()");
	SMapFrame->AddFrame(smaplabel, lhints);
	SMapFrame->AddFrame(fSMapField, fhints);
	SMapFrame->AddFrame(fOpenSMapButton, bhints);

	TGHorizontalFrame *ScalerFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* sclabel = new TGLabel(ScalerFrame, "Scaler File: ");
	fScalerField = new TGTextEntry(ScalerFrame, new TGTextBuffer(120), SCALER);
	fScalerField->Connect("ReturnPressed()","EVBMainFrame",this,"UpdateScaler()");
	fOpenScalerButton = new TGTextButton(ScalerFrame, "Open");
	fOpenScalerButton->Connect("Clicked()","EVBMainFrame", this, "DoOpenScalerfile()");
	ScalerFrame->AddFrame(sclabel, lhints);
	ScalerFrame->AddFrame(fScalerField, fhints);
	ScalerFrame->AddFrame(fOpenScalerButton, bhints);

	NameFrame->AddFrame(WorkFrame, fhints);
	NameFrame->AddFrame(SMapFrame, fhints);
	NameFrame->AddFrame(ScalerFrame, fhints);

	TGHorizontalFrame *WindowFrame = new TGHorizontalFrame(InputFrame, w, h*0.1);
	TGLabel *slowlabel = new TGLabel(WindowFrame, "Slow Coincidence Window (ps):");
	fSlowWindowField = new TGNumberEntryField(WindowFrame, SLOWWIND, 0, TGNumberEntry::kNESReal, TGNumberEntry::kNEANonNegative);
	TGLabel *buflabel = new TGLabel(WindowFrame, "Buffer Size (hits):");
	fBufferSizeField = new TGNumberEntryField(WindowFrame, BUFSIZE, 200000, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	WindowFrame->AddFrame(slowlabel, lhints);
	WindowFrame->AddFrame(fSlowWindowField, fhints);
	WindowFrame->AddFrame(buflabel, lhints);
	WindowFrame->AddFrame(fBufferSizeField, fhints);

	TGHorizontalFrame *RunFrame = new TGHorizontalFrame(InputFrame, w, h*0.1);
	TGLabel *typelabel = new TGLabel(RunFrame, "Operation Type:");
	fTypeBox = new TGComboBox(RunFrame, TYPEBOX);
	//Needs modification for new conversion based sorting GWM -- Dec 2020
	fTypeBox->AddEntry("Convert Slow", EventBuilder::EVBApp::Operation::ConvertSlow);
	fTypeBox->AddEntry("Convert", EventBuilder::EVBApp::Operation::Convert);
	fTypeBox->AddEntry("Merge ROOT", EventBuilder::EVBApp::Operation::Merge);
	fTypeBox->Resize(200,20);
	fTypeBox->Connect("Selected(Int_t, Int_t)","EVBMainFrame",this,"HandleTypeSelection(Int_t,Int_t)");
	TGLabel *rminlabel = new TGLabel(RunFrame, "Min Run:");
	fRMinField = new TGNumberEntryField(RunFrame, RMIN, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	TGLabel *rmaxlabel = new TGLabel(RunFrame, "Max Run:");
	fRMaxField = new TGNumberEntryField(RunFrame, RMAX, 0, TGNumberEntry::kNESInteger, TGNumberEntry::kNEANonNegative);
	fRunButton = new TGTextButton(RunFrame, "Run!");
	fRunButton->SetState(kButtonDisabled);
	fRunButton->Connect("Clicked()","EVBMainFrame",this,"DoRun()");
	RunFrame->AddFrame(typelabel, lhints);
	RunFrame->AddFrame(fTypeBox, fhints);
	RunFrame->AddFrame(rminlabel, lhints);
	RunFrame->AddFrame(fRMinField, fhints);
	RunFrame->AddFrame(rmaxlabel, lhints);
	RunFrame->AddFrame(fRMaxField, fhints);
	RunFrame->AddFrame(fRunButton, bhints);

	InputFrame->AddFrame(NameFrame, fhints);
	InputFrame->AddFrame(WindowFrame, fhints);
	InputFrame->AddFrame(RunFrame, fhints);

	TGVerticalFrame *PBFrame = new TGVerticalFrame(this, w, h*0.1);
	TGLabel *pbLabel = new TGLabel(PBFrame, "Build Progress");
	fProgressBar = new TGHProgressBar(PBFrame, TGProgressBar::kFancy, w);
	fProgressBar->ShowPosition();
	fProgressBar->SetBarColor("lightblue");
	PBFrame->AddFrame(pbLabel, lhints);
	PBFrame->AddFrame(fProgressBar, fhints);

	TGMenuBar* menuBar = new TGMenuBar(this, w, h*0.1);
	fFileMenu = new TGPopupMenu(gClient->GetRoot());
	fFileMenu->AddEntry("Load...", M_LOAD_CONFIG);
	fFileMenu->AddEntry("Save...", M_SAVE_CONFIG);
	fFileMenu->AddEntry("Exit", M_EXIT);
	fFileMenu->Connect("Activated(Int_t)","EVBMainFrame", this, "HandleMenuSelection(Int_t)");
	menuBar->AddPopup("File", fFileMenu, mhints);

	AddFrame(menuBar, new TGLayoutHints(kLHintsTop|kLHintsLeft,0,0,0,0));
	AddFrame(InputFrame, fchints);
	AddFrame(PBFrame, fpbhints);

	fBuilder.SetProgressCallbackFunc(BIND_PROGRESS_CALLBACK_FUNCTION(EVBMainFrame::SetProgressBarPosition));
	fBuilder.SetProgressFraction(0.01);
	SetWindowName("GWM Event Builder");
	MapSubwindows();
	Resize();
	MapWindow();

}

EVBMainFrame::~EVBMainFrame()
{
	Cleanup();
	delete fInfo;
	delete this;
}

void EVBMainFrame::CloseWindow()
{
	gApplication->Terminate();
}

void EVBMainFrame::HandleMenuSelection(int id)
{
	if(id == M_SAVE_CONFIG)
	{
		new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
		if(fInfo->fFilename)
			SaveConfig(fInfo->fFilename);
	}
	else if(id == M_LOAD_CONFIG)
	{
		new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
		if(fInfo->fFilename)
			LoadConfig(fInfo->fFilename);
	}
	else if(id == M_EXIT)
		CloseWindow();
}

void EVBMainFrame::DoOpenWorkdir()
{
	new FileViewFrame(gClient->GetRoot(), this, MAIN_W*0.5, MAIN_H*0.25, this, WORKDIR);
}

void EVBMainFrame::DoOpenSMapfile()
{
	new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
	if(fInfo->fFilename)
		DisplaySMap(fInfo->fFilename);
}

void EVBMainFrame::DoOpenScalerfile()
{
	new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
	if(fInfo->fFilename)
		DisplayScaler(fInfo->fFilename);
}

void EVBMainFrame::DoRun()
{

	DisableAllInput();

	SetParameters();

	int type = fTypeBox->GetSelected();

	switch(type)
	{
		case EventBuilder::EVBApp::Operation::Convert :
		{
			fBuilder.Convert2RawRoot();
			break;
		}
		case EventBuilder::EVBApp::Operation::Merge :
		{
			fBuilder.MergeROOTFiles();
			break;
		}
		case EventBuilder::EVBApp::Operation::ConvertSlow :
		{
			fBuilder.Convert2SortedRoot();
			break;
		}
	}

	EnableAllInput();
}

void EVBMainFrame::HandleTypeSelection(int box, int entry)
{
	fRunButton->SetState(kButtonUp);
}

bool EVBMainFrame::SetParameters()
{
	fBuilder.SetRunRange(fRMinField->GetIntNumber(), fRMaxField->GetIntNumber());
	fBuilder.SetSlowCoincidenceWindow(fSlowWindowField->GetNumber());
	fBuilder.SetBufferSize(fBufferSizeField->GetNumber());
	UpdateWorkdir();
	UpdateSMap();
	UpdateScaler();
	
	return true;
}

void EVBMainFrame::DisplayWorkdir(const char* dir)
{
	fWorkField->SetText(dir);
	fBuilder.SetWorkDirectory(dir);
}

void EVBMainFrame::DisplaySMap(const char* file)
{
	fSMapField->SetText(file);
	fBuilder.SetBoardShiftFile(file);
}

void EVBMainFrame::DisplayScaler(const char* file)
{
	fScalerField->SetText(file);
	fBuilder.SetScalerFile(file);
}

void EVBMainFrame::SaveConfig(const char* file)
{
	std::string filename = file;
	fBuilder.WriteConfigFile(filename);
}

void EVBMainFrame::LoadConfig(const char* file)
{
	std::string filename = file;
	fBuilder.ReadConfigFile(filename);

	fWorkField->SetText(fBuilder.GetWorkDirectory().c_str());
	fSMapField->SetText(fBuilder.GetBoardShiftFile().c_str());
	fScalerField->SetText(fBuilder.GetScalerFile().c_str());
	

	fSlowWindowField->SetNumber(fBuilder.GetSlowCoincidenceWindow());

	fRMaxField->SetIntNumber(fBuilder.GetRunMax());
	fRMinField->SetIntNumber(fBuilder.GetRunMin());

}

void EVBMainFrame::UpdateWorkdir()
{
	const char* dir = fWorkField->GetText();
	fBuilder.SetWorkDirectory(dir);
}

void EVBMainFrame::UpdateSMap()
{
	const char* file = fSMapField->GetText();
	fBuilder.SetBoardShiftFile(file);
}

void EVBMainFrame::UpdateScaler()
{
	const char* file = fScalerField->GetText();
	fBuilder.SetScalerFile(file);
}

void EVBMainFrame::DisableAllInput()
{
	fRunButton->SetState(kButtonDisabled);
	fOpenWorkButton->SetState(kButtonDisabled);
	fOpenSMapButton->SetState(kButtonDisabled);
	fOpenScalerButton->SetState(kButtonDisabled);

	fWorkField->SetState(false);
	fSMapField->SetState(false);
	fScalerField->SetState(false);

	fTypeBox->SetEnabled(false);
	fBufferSizeField->SetState(false);

	fRMaxField->SetState(false);
	fRMinField->SetState(false);

	fSlowWindowField->SetState(false);
}

void EVBMainFrame::EnableAllInput()
{
	fRunButton->SetState(kButtonUp);
	fOpenWorkButton->SetState(kButtonUp);
	fOpenSMapButton->SetState(kButtonUp);
	fOpenScalerButton->SetState(kButtonUp);

	fWorkField->SetState(true);
	fSMapField->SetState(true);
	fScalerField->SetState(true);

	fTypeBox->SetEnabled(true);
	fBufferSizeField->SetState(true);

	fRMaxField->SetState(true);
	fRMinField->SetState(true);

	fSlowWindowField->SetState(true);
}

void EVBMainFrame::SetProgressBarPosition(uint64_t val, uint64_t total)
{
	fProgressBar->SetMin(0);
	fProgressBar->SetMax(total);
	fProgressBar->SetPosition(val);
	gSystem->ProcessEvents();
}