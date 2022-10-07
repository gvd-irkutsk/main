#include <cmath> // Стандартная математическая библиотека Си++
#include <fstream> // Стандартная библиотека для ввода-вывода в файл
#include "TMath.h" // Математическая библиотека root
#include "TFile.h" // Библиотека root для работы с файлами
#include "TTree.h" // Библиотека root для создания древесных структур
#include "BRawMasterData.h" // Библиотека bars со стандартными функциями для работы с сырыми данными. Лежит в папке /bars/braw
#include "BRawMasterHeader.h"
#include "TGraphErrors.h" // Библиотека root для построения графиков с погрешностями
#include "TH1F.h" // Библиотека для одномерных функций
#include "TCanvas.h" // Холст
#include "TLegend.h" // Подписи на графиках

int forStudents()
{
	Int_t Season = 2021;
	Int_t cluster = 6;
	Int_t RunNumber = 343;
	Int_t fileNum = 3;
	Int_t Sdc = 195; // Можно выбирать от 192 до 215
	
	TString PathToRawEvents = Form("/home/iren/NIIPF/Data/processed/%d/cluster%d/exp/raw.root/v1.3-644-g6fd9c/%04d/%04d/k%04d.raw.events.%d.%04d.root", Season, cluster, RunNumber, fileNum, RunNumber, Sdc, fileNum);
		TFile FileRawEvents(PathToRawEvents);
		
	TString fOutWaveFileName = Form("/home/iren/NIIPF/DQM/Results/%d/cluster%d/%04d/forStudents_%d_%04d.root", Season, cluster, RunNumber, Sdc, fileNum);
		TFile fWaveout(fOutWaveFileName.Data(), "recreate");	
// Проверяем, открылся ли файл с данными		
				if (!FileRawEvents.IsOpen()) {
					cout << "ERROR - Could not find file: " << PathToRawEvents << endl;
					}
// Записываем дерево Events в переменную RawTree:
				TTree *RawTree = (TTree*)FileRawEvents.Get("Events");
// Создаем перменную rawMasterData из класса BRawMasterData:			
				BRawMasterData *rawMasterData = new BRawMasterData();
// Связываем переменную rawMasterData с веткой BRawMasterData:				
				RawTree->SetBranchAddress("BRawMasterData.", &rawMasterData);

// Создаем еще одну ветку (в ней время событий):
				BRawMasterHeader *rawMasterHeader = new BRawMasterHeader();
				RawTree->SetBranchAddress("BRawMasterHeader.", &rawMasterHeader);
// cout<<" pulses = "<<RawTree->GetEntries()<<endl;	 -- отобразит количество строк в файле (каждая строка содержит столбец времени, амплитуды и т.д.)		


// Цикл по всем событиям(=строкам иногда, иногда строк больше) в файле (функция GetEntries возвращает количество записей в файле):		
			for (int j = 0; j < 20; j++){
			
// вместо 20 должно стоять RawTree->GetEntries(), но тогда очень уж много выходных графиков (файл получается 42 МБ).			
				RawTree->GetEntry(j);
				
//				cout<<" Количество импуьсов в "<<j<<"событии = "<<rawMasterData->GetNumSamples()<<endl;
				//Double_t EventTime = rawMasterHeader->GetTimeVME(); это время самого события, оно нам не нужно, т.к. нам нужно время импульса в течение одного события
// Цикл по всем импульсам всех каналов в одном событии
				for (int i = 0; i < rawMasterData->GetNumSamples(); i++) {
		// Объект sample, в котором сидит вся информация по конкретному импульсу (амплитуда, время, номер канала...)
					BRawFADCSample *sample = rawMasterData->GetFADCSample(i);
					Int_t nch = sample->GetNch();		// Считываем номер канала (эти функции описаны в заголовочном файле bars/braw/BRawMasterData.h)
					Int_t nbins = sample->GetNbins();	// Считываем длину импульса
					Short_t *data = sample->GetData();  // (Массив из амплитуд) Записываем значения fData по этому импульсу на этом канале в переменную data
					Int_t offset = sample->GetOffset(); // Считываем задержку, компенсирующую длину кабеля
					
//cout<<" Номер канала = "<<nch<<" Длина импульса = "<<nbins<<endl;
// Вычисление пьедестала амплитуды как среднего арифметического первых 10ти точек в амплитуде:					
					double pedestal = 0;
					const int c_nPed = 8; // in BExtractor 10 counts, Lukas uses 5 counts
					for (int k = 0; k < c_nPed; k++) {
									pedestal += data[k];
								}
					pedestal /= c_nPed;
// Построение графика - зависимость амплитуды импульса от времени на текущем канале					
					TGraphErrors *WaveFormGraph = new TGraphErrors();
					WaveFormGraph->SetName(Form("waveform_chan%d_Event%d_impulse%d_cl%d", nch, j, i, cluster));
					WaveFormGraph->SetTitle(Form("waveform_chan%d_Event%d_impulse%d_cl%d", nch, j, i, cluster));
			// Цикл по отсчетам АЦП от 0 до длины импульса в кодах АЦП
					for(Int_t n = 0; n < nbins; n++) {
						WaveFormGraph->SetPoint(n, (n+offset), data[n]-pedestal);	
					}
			// Подписи осей		
					WaveFormGraph->GetXaxis()->SetTitle("time, FADC counts");
					WaveFormGraph->GetYaxis()->SetTitle("amplitude, FADC counts");
					fWaveout.cd();
			// Запись графика в файл		
					WaveFormGraph->Write();
					
				  }
			}
				
fWaveout.Close();

return 1;
}
