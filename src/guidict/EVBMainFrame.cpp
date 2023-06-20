#include "EVBMainFrame.h"
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
	fOpenWorkButton = new TGTextButton(WorkFrame, "Open");
	fOpenWorkButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenWorkdir()");
	WorkFrame->AddFrame(workLabel, lhints);
	WorkFrame->AddFrame(fWorkField, fhints);
	WorkFrame->AddFrame(fOpenWorkButton, bhints);

	TGHorizontalFrame *ChannelFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* channelLabel = new TGLabel(ChannelFrame, "Channel Map:");
	fCMapField = new TGTextEntry(ChannelFrame, new TGTextBuffer(120), CMAP);
	fOpenChannelButton = new TGTextButton(ChannelFrame, "Open");
	fOpenChannelButton->Connect("Clicked()", "EVBMainFrame", this,"DoOpenCMapfile()");
	ChannelFrame->AddFrame(channelLabel, lhints);
	ChannelFrame->AddFrame(fCMapField, fhints);
	ChannelFrame->AddFrame(fOpenChannelButton, bhints);

	TGHorizontalFrame *SMapFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* smaplabel = new TGLabel(SMapFrame, "Board Shift File:");
	fSMapField = new TGTextEntry(SMapFrame, new TGTextBuffer(120), SMAP);
	fSMapField->Resize(w*0.25, fSMapField->GetDefaultHeight());
	fOpenSMapButton = new TGTextButton(SMapFrame, "Open");
	fOpenSMapButton->Connect("Clicked()","EVBMainFrame",this,"DoOpenSMapfile()");
	SMapFrame->AddFrame(smaplabel, lhints);
	SMapFrame->AddFrame(fSMapField, fhints);
	SMapFrame->AddFrame(fOpenSMapButton, bhints);

	TGHorizontalFrame *ScalerFrame = new TGHorizontalFrame(NameFrame, w, h*0.1);
	TGLabel* sclabel = new TGLabel(ScalerFrame, "Scaler File: ");
	fScalerField = new TGTextEntry(ScalerFrame, new TGTextBuffer(120), SCALER);
	fOpenScalerButton = new TGTextButton(ScalerFrame, "Open");
	fOpenScalerButton->Connect("Clicked()","EVBMainFrame", this, "DoOpenScalerfile()");
	ScalerFrame->AddFrame(sclabel, lhints);
	ScalerFrame->AddFrame(fScalerField, fhints);
	ScalerFrame->AddFrame(fOpenScalerButton, bhints);

	NameFrame->AddFrame(WorkFrame, fhints);
	NameFrame->AddFrame(ChannelFrame, fhints);
	NameFrame->AddFrame(SMapFrame, fhints);
	NameFrame->AddFrame(ScalerFrame, fhints);

	TGHorizontalFrame *WindowFrame = new TGHorizontalFrame(InputFrame, w, h*0.1);
	TGLabel *slowlabel = new TGLabel(WindowFrame, "Slow Coincidence Window (ns):");
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

	m_builder.SetProgressCallbackFunc(BIND_PROGRESS_CALLBACK_FUNCTION(EVBMainFrame::SetProgressBarPosition));
	m_builder.SetProgressFraction(0.01);
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
	new TGFileDialog(gClient->GetRoot(), this, kDOpen, fInfo);
	if(fInfo->fFilename)
	{
		std::string path_wtrailer = fInfo->fFilename + std::string("/");
		DisplayWorkdir(path_wtrailer.c_str());
	}
}

void EVBMainFrame::DoOpenSMapfile()
{
	new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
	if(fInfo->fFilename)
		DisplaySMap(fInfo->fFilename);
}

void EVBMainFrame::DoOpenCMapfile()
{
	new TGFileDialog(gClient->GetRoot(), this, kFDOpen, fInfo);
	if(fInfo->fFilename)
		DisplayCMap(fInfo->fFilename);
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
			m_builder.Convert2RawRoot();
			break;
		}
		case EventBuilder::EVBApp::Operation::Merge :
		{
			m_builder.MergeROOTFiles();
			break;
		}
		case EventBuilder::EVBApp::Operation::ConvertSlow :
		{
			m_builder.Convert2SortedRoot();
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
	m_params.runMin = fRMinField->GetIntNumber();
	m_params.runMax = fRMaxField->GetIntNumber();
	m_params.slowCoincidenceWindow = fSlowWindowField->GetNumber();
	m_params.bufferSize = fBufferSizeField->GetIntNumber();
	m_params.workspaceDir = fWorkField->GetText();
	m_params.channelMapFile = fCMapField->GetText();
	m_params.timeShiftFile = fSMapField->GetText();
	m_params.scalerFile = fScalerField->GetText();

	m_builder.SetParameters(m_params);
	
	return true;
}

void EVBMainFrame::DisplayWorkdir(const char* dir)
{
	fWorkField->SetText(dir);
	SetParameters();
}

void EVBMainFrame::DisplaySMap(const char* file)
{
	fSMapField->SetText(file);
	SetParameters();
}

void EVBMainFrame::DisplayCMap(const char* file)
{
	fCMapField->SetText(file);
	SetParameters();
}

void EVBMainFrame::DisplayScaler(const char* file)
{
	fScalerField->SetText(file);
	SetParameters();
}

void EVBMainFrame::SaveConfig(const char* file)
{
	std::string filename = file;
	m_builder.WriteConfigFile(filename);
}

void EVBMainFrame::LoadConfig(const char* file)
{
	std::string filename = file;
	m_builder.ReadConfigFile(filename);
	m_params = m_builder.GetParameters();

	fWorkField->SetText(m_params.workspaceDir.c_str());
	fSMapField->SetText(m_params.timeShiftFile.c_str());
	fScalerField->SetText(m_params.scalerFile.c_str());
	fCMapField->SetText(m_params.channelMapFile.c_str());

	fSlowWindowField->SetNumber(m_params.slowCoincidenceWindow);

	fRMaxField->SetIntNumber(m_params.runMax);
	fRMinField->SetIntNumber(m_params.runMin);
	fBufferSizeField->SetIntNumber(m_params.bufferSize);

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
